#include "chordNodeManager.hpp"

/*
This function initializes the chordNodeManager object so that it can be used.
@param inputInprocAddressForPauseResumeSignals: The string to use for the inproc address (the string header will be added later)
*/
chordNodeManager::chordNodeManager(std::string inputInprocAddressForPauseResumeSignals)
{

//Store address
inprocAddressForPauseResumeSignals = inputInprocAddressForPauseResumeSignals;

//Create ZMQ context
SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")


//Create pause/resume signaling socket
SOM_TRY
pauseResumeSignalSocket.reset(new zmq::socket_t(*context, ZMQ_PUB));
SOM_CATCH("Error initializing pause/resume signal socket\n")

//Now bind the socket
SOM_TRY
pauseResumeSignalSocket->bind(("inproc://" + inprocAddressForPauseResumeSignals).c_str());
SOM_CATCH("Error binding pause/resume signal socket\n")


//Create socket to receive signal acknowlegements
SOM_TRY
pauseResumeSignalAckSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error initializing socket to receive pause/resume signal acknowledgements\n")

//Set subscription filters for acknowledgement socket
SOM_TRY
pauseResumeSignalAckSocket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
SOM_CATCH("Error setting subscription on pause/resume acknowledgement socket\n")

}

/*
This function creates a new chord node object/process.  The created node is fully initialized once the promise returns its result.  Any exceptions that occurred as part of the initialization are thrown when the promise is queried for its result
@param inputEntryChordNodePortNumber: The port number of the node that the new node should use to enter the network (0 indicates that no other nodes currently exist)
@return: This function returns the port number associated with the node or throws an exception
@exception: This function can throw SOMExceptions if something goes wrong
*/
uint32_t chordNodeManager::createChordNode(uint32_t inputEntryChordNodePortNumber)
{

//Create a new node
chordNode *tempNodePointer;
SOM_TRY
tempNodePointer = new chordNode(inputEntryChordNodePortNumber, inprocAddressForPauseResumeSignals, context.get());
SOM_CATCH("Error creating node\n")

//Get the node port number and add it to the map
uint32_t nodePortNumber = tempNodePointer->nodePortNumber;

chordRingIDToNode[nodePortNumber] = std::move(std::unique_ptr<chordNode>(tempNodePointer));

//Subscribe to receive pause/resume acknowledgments
SOM_TRY
pauseResumeSignalAckSocket->connect(("inproc://" + std::to_string(nodePortNumber)).c_str()); 
SOM_CATCH("Error subscribing to receive pause/resume acknowledgements from new node\n")


return nodePortNumber;
}

/*
This function publishes the pause signal, then waits until all of the nodes have emitted the pausing signal.
*/
void chordNodeManager::pauseNodes()
{
return pauseOrResumeNodes(true);
}

/*
This function publishes the resume signal, then waits until all of the nodes have emitted the resumed signal.
*/
void chordNodeManager::resumeNodes()
{
pauseOrResumeNodes(false);
}

/*
This function publishes the pause signal or resume signal, then waits until all of the nodes have emitted the corresponding ack.
@inputPauseNodes: True if the nodes are to be paused and false if they are to be resumed
*/
void chordNodeManager::pauseOrResumeNodes(bool inputPauseNodes)
{
//Create pause or resume signal
std::string serializedPauseOrResumeMessage;
messageContainer pauseOrResumeMessage;

pauseResumeSignal pauseResumeSignalMessage;
if(inputPauseNodes == true)
{
pauseResumeSignalMessage.set_signal(PAUSE_SIGNAL);
}
else
{
pauseResumeSignalMessage.set_signal(RESUME_SIGNAL);
}

(*pauseOrResumeMessage.MutableExtension(pauseResumeSignal::aPauseResumeSignal)) = pauseResumeSignalMessage;

pauseOrResumeMessage.SerializeToString(&serializedPauseOrResumeMessage);

//Send the signal
SOM_TRY
pauseResumeSignalSocket->send(serializedPauseOrResumeMessage.c_str(), serializedPauseOrResumeMessage.size());
SOM_CATCH("Error sending pause or resume signal\n")

//Wait until all of the nodes have acknowledged the message
std::unique_ptr<zmq::message_t> messageBuffer;

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing zmq message buffer\n") 

int numberOfAcknowledgementsRecieved = 0;
while(true)
{
if(numberOfAcknowledgementsRecieved >= chordRingIDToNode.size())
{
return;
}

//Reinitialize message buffer
SOM_TRY
messageBuffer->rebuild();
SOM_CATCH("Error reinitializing zmq message buffer\n")

//Get node acknowledgements
SOM_TRY
pauseResumeSignalAckSocket->recv(messageBuffer.get());
SOM_CATCH("Error receiving the reply from inproc proxy for addURI request\n")

//Check if the acknowledgements is correct
messageContainer nodeReply;
nodeReply.ParseFromString(std::string((const char *) messageBuffer->data(), messageBuffer->size()));
if(!nodeReply.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, ack message received from node is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(!nodeReply.HasExtension(signalAck::aSignalAck))
{
//Reply isn't in proper format, so throw an exception
throw SOMException("Error, ack message received from node is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if((nodeReply.GetExtension(signalAck::aSignalAck).signaltoack() == PAUSE_SIGNAL && inputPauseNodes == true) || (nodeReply.GetExtension(signalAck::aSignalAck).signaltoack() == RESUME_SIGNAL && inputPauseNodes == false))
{
//The right acknowledgement was received
numberOfAcknowledgementsRecieved++;
}

}



}

