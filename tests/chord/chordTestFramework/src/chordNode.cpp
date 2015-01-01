#include "chordNode.hpp"

/*
This function initializes the object and starts a thread that conducts the operations of the node.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read.
@param inputChordNodeContactInformation: The contact info for the first node to contact to join the network
@param inputManagerBaseInprocAddress: A string to the root of the node manager inproc addresses
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
chordNode::chordNode(const chordNodeContactInformation &inputChordNodeContactInformation, std::string inputManagerBaseInprocAddress, zmq::context_t *inputZMQContext)
{
if(inputZMQContext == nullptr)
{
throw SOMException("Error, zmq context pointer is null\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Do basic intitialization
managerBaseInprocAddress = inputManagerBaseInprocAddress;
context = inputZMQContext;
timeToShutdownFlag = false;

//Generate a random string to be the info that this chord node is storing
std::string lookupTable("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distribution(0, lookupTable.size()-1);

//Generate 10 random characters
//printf("Generating characters\n");
//fflush(stdout);
for(int i=0; i<10; i++)
{
nodeData.push_back(lookupTable[distribution(gen)]);
}
//printf("Done generating characters\n");
//fflush(stdout);

//Setup datagram router
SOM_TRY
datagramInterfaceRouter.reset(new virtualPortRouter(context));
SOM_CATCH("Error, problem initializing virtual port router\n")

nodePortNumber = datagramInterfaceRouter->datagramPortNumber;


std::promise<bool> threadInitializationResult;

chordNodeContactInformation buffer = inputChordNodeContactInformation;

//Start a thread and pass it a this pointer
nodeThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunChordNode, this, buffer, &threadInitializationResult));

//This should block until the server thread has been initialized and throw any exceptions that the server thread threw during its initialization
if(threadInitializationResult.get_future().get() == true)
{
   //printf("Thread reports that it initialized successfully\n");
}




}

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
chordNode::~chordNode()
{
//TODO: debug
//printf("Destructoring\n");

timeToShutdownFlag = true;
nodeThread->join();
}

/*
This function is run as a seperate thread and implements the chord node behavior
@param inputChordNode: A pointer to the object associated with the node this function is running
@param inputChordNodeContactInformation: The contact info for the first node to contact to join the network
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunChordNode(chordNode *inputChordNode, chordNodeContactInformation inputChordNodeContactInformation, std::promise<bool> *inputPromise)
{

//Initialize resources
std::unique_ptr<chordNodeResources> resources;

try
{
resources.reset(new chordNodeResources(inputChordNode));
}
catch(const std::exception &inputException)
{
inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("Error setting integer with raw hash\n") + inputException.what(), SYSTEM_ERROR, __FILE__, __LINE__)));
}

inputPromise->set_value(true); //Initialization completed successfully

while(true)
{
if(resources->handlePauseResumeAndShutdownSignal() == 1)
{
return;
}

//Do event handling loop




}
}


/*
This function initializes the sockets to get/send signals for pausing/resuming the chord node thread and creates the datagram router needed to send/send receive udp datagrams.
@param inputChordNode: The chord node these resources are associated

@exceptions: This function can throw exceptions
*/
chordNodeResources::chordNodeResources(chordNode *inputChordNode)
{
associatedChordNode = inputChordNode;
nodeMessageNumberIndex = 0;


//Generate chordID and its corresponding bigint from the port number
//Make hash
if(crypto_hash_sha512(inputChordNode->chordID, (const unsigned char *) &(inputChordNode->nodePortNumber), sizeof(inputChordNode->nodePortNumber)) != 0)
{
throw SOMException(std::string("Error, unable to generate message hash\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}

//Make bigint
if(createUInt512FromRawHash((const char *) inputChordNode->chordID, crypto_hash_sha512_BYTES, inputChordNode->chordIDForComputation) != 1)
{
throw SOMException(std::string("Error setting integer with raw hash\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}

//Create self contact information
chordNodeContactInformation selfContactInfo;
selfContactInfo.set_chord_id(std::string((const char *) inputChordNode->chordID, CHORD_ID_SIZE));
selfContactInfo.set_ip(INADDR_LOOPBACK);
selfContactInfo.set_port(inputChordNode->datagramInterfaceRouter->datagramPortNumber);
selfContactInfo.set_virtual_port(1);



//Create finger table (which is protected by associated mutex)
SOM_TRY
nodeFingerTable.reset(new fingerTable( selfContactInfo, FINGER_TABLE_SIZE));
SOM_CATCH("Error creating finger table\n")

//Create the pause/resume signal socket
SOM_TRY
pauseResumeSignalSocket.reset(new zmq::socket_t(*(inputChordNode->context), ZMQ_SUB));
SOM_CATCH("Error initializing pause/resume signal socket for node\n")

//Now subscribe
SOM_TRY
pauseResumeSignalSocket->connect(("inproc://" + inputChordNode->managerBaseInprocAddress).c_str());
SOM_CATCH("Error connecting pause/resume signal socket for node\n")

//Set it to accept any messages
SOM_TRY
pauseResumeSignalSocket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
SOM_CATCH("Error setting subscription for pause/resume signal socket for node\n")

//Make the socket to acknowledge pause/resume signals
SOM_TRY
pauseResumeSignalAckSocket.reset(new zmq::socket_t(*(inputChordNode->context), ZMQ_PUB));
SOM_CATCH("Error initializing pause/resume ack signal socket for node\n")


//Now bind the socket
SOM_TRY
pauseResumeSignalAckSocket->bind(("inproc://" + std::to_string(inputChordNode->nodePortNumber)).c_str());
SOM_CATCH("Error binding socket for node pause/resume signal ack socket\n")


}

/*
This function checks if a pause or resume signal has been published.
@param inputBlockForSignal: True if the function should block until a signal occurs, false if it should return immediately if there is not a signal in the queue
@return: 0 on timeout, 1 on pause signal, 2 on resume signal

@exceptions: This function can throw exceptions
*/
int chordNodeResources::checkForPauseResumeSignal(bool inputBlockForSignal)
{

std::unique_ptr<zmq::message_t> messageBuffer;

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing message buffer\n")

SOM_TRY
if(inputBlockForSignal != true)
{
if(pauseResumeSignalSocket->recv(messageBuffer.get(), ZMQ_DONTWAIT) == false)
{
return 0;
}
}
else
{
if(pauseResumeSignalSocket->recv(messageBuffer.get()) == false)
{
return 0;
}
}
SOM_CATCH("Error receiving signal to node\n")

//Determine if the signal is valid and its type
messageContainer signalContainer;
signalContainer.ParseFromString(std::string((const char *) messageBuffer->data(), messageBuffer->size()));
if(!signalContainer.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, signal to node is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(!signalContainer.HasExtension(pauseResumeSignal::aPauseResumeSignal))
{
//Reply isn't in proper format, so throw an exception
throw SOMException("Error, signal to node is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(signalContainer.GetExtension(pauseResumeSignal::aPauseResumeSignal).signal() == PAUSE_SIGNAL)
{
return 1;
}
else if(signalContainer.GetExtension(pauseResumeSignal::aPauseResumeSignal).signal() == RESUME_SIGNAL)
{
return 2;
}
else
{
throw SOMException("Error, signal to node is of unrecognized type\n",INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

}

/*
This function sends an acknowledgement message for either a pause signal or a resume signal.
@param inputAcknowledgePauseSignal: True if a pause should be acknowledged, false if a resume should be acknowledged

@exceptions: This function can throw exceptions
*/
void chordNodeResources::acknowledgePauseResumeSignal(bool inputAcknowledgePauseSignal)
{

//Construct acknowledgement
std::string serializedAcknowledgementMessage;
messageContainer acknowledgementMessage;

signalAck signalAckMessage;
if(inputAcknowledgePauseSignal == true)
{
signalAckMessage.set_signaltoack(PAUSE_SIGNAL);
}
else
{
signalAckMessage.set_signaltoack(RESUME_SIGNAL);
}

(*acknowledgementMessage.MutableExtension(signalAck::aSignalAck)) = signalAckMessage;

acknowledgementMessage.SerializeToString(&serializedAcknowledgementMessage);

//Send acknowledgement
SOM_TRY
pauseResumeSignalAckSocket->send(serializedAcknowledgementMessage.c_str(), serializedAcknowledgementMessage.size());
SOM_CATCH("Error sending signal acknowledgement\n")
}

/*
This convenience function allows for easy processing of pause/resume signals.  It will do a non blocking check to see if any pause signals have been sent.  If there is a pause signal, it will send an acknowledgement and block until a resume signal is received (which it will also acknowledge).
@param inputSignalSocket: The configured ZMQ socket to check for signals
@param inputAckSocket: The configured ZMQ socket to send acknowledgements on

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processPauseResumeSignals()
{
//Check if a pause signal has occurred
int signalValue;

SOM_TRY
//Check nonblocking
signalValue = checkForPauseResumeSignal();
SOM_CATCH("Error getting node signal\n")

//Check if it is a pause signal
if(signalValue != 1)
{
return;  //Only interested in pause signals if we aren't paused
}

SOM_TRY
//Send acknowledgement
acknowledgePauseResumeSignal(true);
SOM_CATCH("Error send pause signal acknowledgement\n")

//Wait blocking for a resume signal
while(true)
{
SOM_TRY
//Check blocking
signalValue = checkForPauseResumeSignal(true);
SOM_CATCH("Error getting node signal\n")

if(signalValue == 2)
{
//Resume signal received
SOM_TRY
//Send acknowledgement
acknowledgePauseResumeSignal(false);
SOM_CATCH("Error send resume signal acknowledgement\n")
break;
}
}

}


/*
This function handles pausing/resuming and returns 1 if it is time for the node to shutdown.  It is used primarily to make blocking functions that still follow the required pause/resume and shutdown behaviors.
@return: 1 if it is time to shutdown and 0 otherwise
*/
int chordNodeResources::handlePauseResumeAndShutdownSignal()
{
//Handle pause/resume signals
try
{
processPauseResumeSignals();
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
return 1;
}

if(associatedChordNode->timeToShutdownFlag == true)
{
//Shut down
return 1;
}

return 0;
}

/*
This function is used to generate quasi-unique numbers so that message responses can be properly correlated with with the requests.  This function is threadsafe.
@return: A uint64_t that is unique until it cycles through 2^64 entries
*/
uint64_t chordNodeResources::getNextNodeMessageNumber()
{
nodeMessageNumberIndexMutex.lock();
SOMScopeGuard mutexGuard([&](){nodeMessageNumberIndexMutex.unlock();}); //Make sure socket gets cleaned up when function returns

nodeMessageNumberIndex++;
return nodeMessageNumberIndex;
}

