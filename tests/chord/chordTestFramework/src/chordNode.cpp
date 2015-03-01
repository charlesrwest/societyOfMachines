#include "chordNode.hpp"

/*
This function initializes the object and starts a thread that conducts the operations of the node.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read.
@param inputChordNodeContactInfos: A list of contact info for nodes that this one can contact to join the network
@param inputManagerBaseInprocAddress: A string to the root of the node manager inproc addresses
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
chordNode::chordNode(const std::vector<chordNodeContactInformation> &inputChordNodeContactInfos, std::string inputManagerBaseInprocAddress, zmq::context_t *inputZMQContext)
{
if(inputZMQContext == nullptr)
{
throw SOMException("Error, zmq context pointer is null\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Do basic intitialization
managerBaseInprocAddress = inputManagerBaseInprocAddress;
context = inputZMQContext;
timeToShutdownFlag = false;

//Setup datagram router
SOM_TRY
datagramInterfaceRouter.reset(new virtualPortRouter(context));
SOM_CATCH("Error, problem initializing virtual port router\n")

nodePortNumber = datagramInterfaceRouter->datagramPortNumber;
findNodeAssociatedWithAddressInprocAddress = "inproc://findNodeAssociatedWithAddressServer:" + datagramInterfaceRouter->virtualPortRouterExtension;

std::promise<bool> threadInitializationResult;


//Start a thread and pass it a this pointer 
nodeThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunChordNode, this, inputChordNodeContactInfos, &threadInitializationResult));

//This should block until the server thread has been initialized and throw any exceptions that the server thread threw during its initialization
if(threadInitializationResult.get_future().get() == true)
{
   //printf("Thread reports that it initialized successfully\n");
}



//printf("Node initialization completed successfully\n");
}

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
chordNode::~chordNode()
{
printf("Destructoring\n");
timeToShutdownFlag = true;
nodeThread->join();
}

/*
This function is run as a seperate thread and implements the chord node behavior
@param inputChordNode: A pointer to the object associated with the node this function is running
@param inputChordNodeContactInformation: The contact info for the nodes to contact to join the network
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunChordNode(chordNode *inputChordNode, const std::vector<chordNodeContactInformation> &inputChordNodeContactInformation, std::promise<bool> *inputPromise)
{

//Initialize resources
std::unique_ptr<chordNodeResources> resources;

try
{
resources.reset(new chordNodeResources(inputChordNode, inputChordNodeContactInformation));
}
catch(const std::exception &inputException)
{
//TODO: Fix exception handling so that the promise pointer passes the exception like it is suppose to
//inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("Error initializing chord node resources\n") + inputException.what(), SYSTEM_ERROR, __FILE__, __LINE__)));
fprintf(stderr, "%s\n", (std::string("Error initializing chord node resources\n") + inputException.what()).c_str());
return;
}
inputChordNode->nodeResourcesPointer = resources.get();

//printf("Resources initialized\n");

inputPromise->set_value(true); //Initialization completed successfully

//Do event handling loop 
try
{
resources->operateNode();
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error: %s\n", inputException.what());
return;
}
}


/*
This function initializes the sockets to get/send signals for pausing/resuming the chord node thread and creates the datagram router needed to send/receive udp datagrams.
@param inputChordNode: The chord node these resources are associated
@param inputChordNetworkContactInformation: A list of chord nodes to contact to try to join the network.

@exceptions: This function can throw exceptions
*/
chordNodeResources::chordNodeResources(chordNode *inputChordNode,  std::vector<chordNodeContactInformation> inputChordNetworkContactInformation)
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
selfContactInfo.set_virtual_port(0);



//Create finger table (which is protected by associated mutex)
SOM_TRY
nodeFingerTable.reset(new fingerTable( selfContactInfo, FINGER_TABLE_SIZE));
SOM_CATCH("Error creating finger table\n")


//Give chord node class the self contact info
inputChordNode->selfContactInfo = selfContactInfo;

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

//Initialize sockets
SOM_TRY
initializeChordNodeResourcesSockets();
SOM_CATCH("Error setting up ZMQ sockets/socket wrappers\n")

//printf("Chord sockets online (%d)\n", associatedChordNode->nodePortNumber);

//Send node is active requests to all known nodes (automatically joining the network by giving them our contact info and confirming the best one as our successor)
for(int i=0; i<inputChordNetworkContactInformation.size(); i++)
{
SOM_TRY
startNodeIsActiveRequest(inputChordNetworkContactInformation[i]);
SOM_CATCH("Error, unable to start node is active request\n")
}
printf("Start node is active requests sent/setup (%d)\n", associatedChordNode->nodePortNumber);

//Schedule updates
scheduleStabilizationMessagesAndFingerUpdates();
}

/*
This function sends a node is active request to the given node and creates a datastructure/schedules events to ensure that the requests will be resent until either the node answers or the resend limit is reached.
@param inputNodeToContact: The node to check if active

@exceptions: This function can throw exceptions
*/
void chordNodeResources::startNodeIsActiveRequest(const chordNodeContactInformation &inputNodeToContact)
{
//Create request structure and store it in the associated map
uint64_t requestID = getNextMessageNumber();
std::unique_ptr<nodeIsActiveRequest> requestStructure(new nodeIsActiveRequest(inputNodeToContact));
requestIDToNodeIsActiveRequest[requestID] = std::move(requestStructure);

//Add timeout event
chordEvent requestTimeoutEvent;
requestTimeoutEvent.createNodeIsActiveTimeoutEvent(std::chrono::milliseconds(IS_NODE_ACTIVE_TIMEOUT_DELAY), requestID);
eventQueue.push(requestTimeoutEvent);
printf("Node is active request setup but message not sent (%d)\n", associatedChordNode->nodePortNumber);

//Send request
SOM_TRY
printf("Sending node is active request (%d)\n", associatedChordNode->nodePortNumber);
sendNodeIsActiveRequest(inputNodeToContact, requestID);
printf("Sending node is active request sent (%d)\n", associatedChordNode->nodePortNumber);
SOM_CATCH("Error sending node is active request\n")
}

/*
This function is used to setup all of the ZMQ sockets used by the chord node resources object.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::initializeChordNodeResourcesSockets()
{
//Initialize socket to take findNodeAssociatedWithAddress requests
SOM_TRY
findNodeAssociatedWithAddressHandler.reset(new zmq::socket_t(*(associatedChordNode->context), ZMQ_ROUTER));
SOM_CATCH("Error initializing findNodeAssociatedWithAddressHandler socket for node\n")

//Initialize socket to take closest known predecessor requests
SOM_TRY
closestKnownPredecessorRequestsHandler.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing closestKnownPredecessorRequestsHandler socket for node\n")

//Initialize socket to take stabilization requests
SOM_TRY
stabilizationRequestHandler.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing stabilizationRequestHandler socket for node\n")

//Initialize socket to take node is active requests
SOM_TRY
nodeIsActiveRequestHandler.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing nodeIsActiveRequestHandler socket for node\n")

//Initialize socket to send findNodeAssociatedWithAddress requests
SOM_TRY
findNodeAssociatedWithAddressRequestSender.reset(new zmq::socket_t(*(associatedChordNode->context), ZMQ_DEALER));
SOM_CATCH("Error initializing findNodeAssociatedWithAddressRequestSender socket for node\n")

//Initialize socket to send closest predecessor requests
SOM_TRY
closestPredecessorRequestSender.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing closestPredecessorRequestSender socket for node\n")

//Initialize socket to send stabilization requests
SOM_TRY
stabilizationRequestSender.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing stabilizationRequestSender socket for node\n")

//Initialize socket to send node is active requests
SOM_TRY
nodeIsActiveRequestSender.reset(new virtualPortPairSocketWrapper(associatedChordNode->context, associatedChordNode->datagramInterfaceRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing nodeIsActiveRequestSender socket for node\n")

//Bind virtual ports

//Make map to simplify binding the wrappers
std::vector<virtualPortPairSocketWrapper *> virtualPortWrappersToBind;
std::vector<std::string> virtualPortWrapperNames;

virtualPortWrappersToBind.push_back(nodeIsActiveRequestHandler.get());
virtualPortWrapperNames.push_back("nodeIsActiveRequestHandler");
virtualPortWrappersToBind.push_back(stabilizationRequestHandler.get());
virtualPortWrapperNames.push_back("stabilizationRequestHandler");
virtualPortWrappersToBind.push_back(closestKnownPredecessorRequestsHandler.get());
virtualPortWrapperNames.push_back("closestKnownPredecessorRequestsHandler");
virtualPortWrappersToBind.push_back(nodeIsActiveRequestSender.get());
virtualPortWrapperNames.push_back("nodeIsActiveRequestSender");
virtualPortWrappersToBind.push_back(stabilizationRequestSender.get());
virtualPortWrapperNames.push_back("stabilizationRequestSender");
virtualPortWrappersToBind.push_back(closestPredecessorRequestSender.get());
virtualPortWrapperNames.push_back("closestPredecessorRequestSender");

for(int i=0; i<virtualPortWrappersToBind.size(); i++)
{
SOM_TRY
if(virtualPortWrappersToBind[i]->bind(i) != 1)
{
throw SOMException(std::string("Error binding virtual port for " + virtualPortWrapperNames[i] + "\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
SOM_CATCH("Error binding virtual port for " + virtualPortWrapperNames[i] + "\n")

//printf("Bound %s (%d)\n", virtualPortWrapperNames[i].c_str(), associatedChordNode->nodePortNumber);
}

//Bind and connect the ZMQ sockets that need it
SOM_TRY
findNodeAssociatedWithAddressHandler->bind(associatedChordNode->findNodeAssociatedWithAddressInprocAddress.c_str());
SOM_CATCH("Error binding findNodeAssociatedWithAddressHandler to "+associatedChordNode->findNodeAssociatedWithAddressInprocAddress+" \n")

SOM_TRY
findNodeAssociatedWithAddressRequestSender->connect(associatedChordNode->findNodeAssociatedWithAddressInprocAddress.c_str());
SOM_CATCH("Error connecting findNodeAssociatedWithAddressRequestSender to "+associatedChordNode->findNodeAssociatedWithAddressInprocAddress+" \n")
}

/*
This function creates and stores the events which are responsible for sending stabilization messages and finger table update requests. 
*/
void chordNodeResources::scheduleStabilizationMessagesAndFingerUpdates()
{
//Create event to start stabilization requests
chordEvent stabilizationEvent;
stabilizationEvent.createSendStabilizationRequestEvent(std::chrono::milliseconds(STABILIZATION_MESSAGE_PERIOD));
eventQueue.push(stabilizationEvent); //Add to event queue

//Create events to start finger table updates
for(int i=0; i < nodeFingerTable->targetChordAddresses.size(); i++)
{
chordEvent fingerTableUpdateEvent;
fingerTableUpdateEvent.createUpdateFingerTableEvent(std::chrono::milliseconds(FINGER_TABLE_REFRESH_PERIOD)*(i+1), i); //Update one at a time

eventQueue.push(fingerTableUpdateEvent); //Add to event queue
}
}


/*
This function sends a node is active request to the given node without affecting the nodeIsActiveRequest structure associated with the request ID.
@param inputNodeToContact: The node to check if active
@param inputRequestID: The request ID to put in the message

@exceptions: This function can throw exceptions
*/
void chordNodeResources::sendNodeIsActiveRequest(const chordNodeContactInformation &inputNodeToContact, uint64_t inputRequestID)
{
//Create message to send
nodeIsActiveRequestResponse nodeIsActiveMessage;
nodeIsActiveMessage.set_allocated_sender_contact_info(new chordNodeContactInformation(nodeFingerTable->selfContactInfo()));

//Serialize message
std::string serializedNodeIsActiveMessage;
SOM_TRY
nodeIsActiveMessage.SerializeToString(&serializedNodeIsActiveMessage);
SOM_CATCH("Error serializing message\n")

//Send the message
SOM_TRY
nodeIsActiveRequestSender->send(serializedNodeIsActiveMessage.c_str(), serializedNodeIsActiveMessage.size(), inputNodeToContact.ip(), inputNodeToContact.port(), inputNodeToContact.virtual_port()+NODE_IS_ACTIVE_REQUEST_HANDLER_PORT_OFFSET, inputRequestID);
printf("Sending node is active request to virtual port (%d->%d): %u->%u\n",associatedChordNode->nodePortNumber, inputNodeToContact.port(), nodeIsActiveRequestSender->associatedVirtualPort, inputNodeToContact.virtual_port()+NODE_IS_ACTIVE_REQUEST_HANDLER_PORT_OFFSET);
SOM_CATCH("Error sending nodeIsActiveRequest message\n")
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
SOM_TRY
if(!signalContainer.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, signal to node is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}
SOM_CATCH("Error reading message\n")

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

SOM_TRY
acknowledgementMessage.SerializeToString(&serializedAcknowledgementMessage);
SOM_CATCH("Error serializing message\n")

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
uint64_t chordNodeResources::getNextMessageNumber()
{
nodeMessageNumberIndexMutex.lock();
SOMScopeGuard mutexGuard([&](){nodeMessageNumberIndexMutex.unlock();}); //Make sure socket gets cleaned up when function returns

nodeMessageNumberIndex++;
return nodeMessageNumberIndex;
}

/*
This function listens to the sockets and processes the event queue to perform the functions expected of a chord node.  It does not return unless something goes wrong.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::operateNode()
{
printf("Operating node (%d)\n", associatedChordNode->nodePortNumber);

//Statically allocate poll items since the number of sockets isn't going to change
zmq::pollitem_t pollItems[8];
int numberOfPollItems = 8;

//Initialize all the poll items fields that will be the same
for(int i=0; i<numberOfPollItems; i++)
{
pollItems[i].fd = 0;
pollItems[i].events = ZMQ_POLLIN;
pollItems[i].revents = 0;
}

//Setup the unique poll item pieces
pollItems[0].socket = (void *) (*findNodeAssociatedWithAddressHandler);
pollItems[1].socket = (void *) (*(closestKnownPredecessorRequestsHandler->pairSocket));
pollItems[2].socket = (void *) (*(stabilizationRequestHandler->pairSocket));
pollItems[3].socket = (void *) (*(nodeIsActiveRequestHandler->pairSocket));
pollItems[4].socket = (void *) (*findNodeAssociatedWithAddressRequestSender);
pollItems[5].socket = (void *) (*(closestPredecessorRequestSender->pairSocket));
pollItems[6].socket = (void *) (*(stabilizationRequestSender->pairSocket));
pollItems[7].socket = (void *) (*(nodeIsActiveRequestSender->pairSocket));

//Create map to call appropriate function 
std::map<int, std::function<void(void)> > pollItemIndexToFunction;
pollItemIndexToFunction[0] = std::bind(&chordNodeResources::processFindNodeAssociatedWithAddressRequest, this);
pollItemIndexToFunction[1] = std::bind(&chordNodeResources::processClosestKnownPredecessorRequest, this);
pollItemIndexToFunction[2] = std::bind(&chordNodeResources::processStabilizationRequest, this);
pollItemIndexToFunction[3] = std::bind(&chordNodeResources::processNodeIsActiveRequest, this);
pollItemIndexToFunction[4] = std::bind(&chordNodeResources::processFindAddressOwnerRequestReply, this);
pollItemIndexToFunction[5] = std::bind(&chordNodeResources::processClosestPredecessorRequestReply, this);
pollItemIndexToFunction[6] = std::bind(&chordNodeResources::processStabilizationRequestReply, this);
pollItemIndexToFunction[7] = std::bind(&chordNodeResources::processNodeIsActiveRequestReply, this);


//Create a map to allow specific errors from if those functions throw an exception
std::map<int, std::string > pollItemIndexToFunctionName;
pollItemIndexToFunctionName[0] = "processFindNodeAssociatedWithAddressRequest";
pollItemIndexToFunctionName[1] = "processClosestKnownPredecessorRequest";
pollItemIndexToFunctionName[2] = "processStabilizationRequest";
pollItemIndexToFunctionName[3] = "processNodeIsActiveRequest";
pollItemIndexToFunctionName[4] = "processFindAddressOwnerRequestReply";
pollItemIndexToFunctionName[5] = "processClosestPredecessorRequestReply";
pollItemIndexToFunctionName[6] = "processStabilizationRequestReply";
pollItemIndexToFunctionName[7] = "processNodeIsActiveRequestReply";

uint64_t amountOfTimeToPoll = 1000; //Default amount of time to pull, in milliseconds

//Loop forever, handling events and messages
try //If exceptions occur, send the message to stderr and exit
{
while(true)
{
//Check if it is time to stop
if(handlePauseResumeAndShutdownSignal() == 1)
{
return;
}

//Handle any events that are suppose to happen now or earlier, then calculate how much time there is until the next event comes due
while(true)  
{
//There should always be at least one event, as some always generate a event of the same type when processed
if(eventQueue.size() == 0)
{
throw SOMException("Error, event queue size reached 0\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//printf("Event queue size!!!! (%d) %ld\n",associatedChordNode->nodePortNumber, eventQueue.size());

if(std::chrono::steady_clock::now() < eventQueue.top().eventTime)
{
amountOfTimeToPoll = std::chrono::duration_cast<std::chrono::milliseconds>(eventQueue.top().eventTime - std::chrono::steady_clock::now()).count();
//printf("Polling for %lu milliseconds (%d)\n", amountOfTimeToPoll,associatedChordNode->nodePortNumber);
break; //There is some time, so leave the inner loop to handle messages
}

//printf("An event is occuring (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
processEvent();
SOM_CATCH("Error processing event\n")
}

//Poll on sockets
SOM_TRY
if(zmq::poll(pollItems, numberOfPollItems, amountOfTimeToPoll) == 0)
{
continue; //Returned without there being any messages waiting
} 
SOM_CATCH("Error polling on sockets\n")

//Process any possible messages by calling the appropriate functions
for(int i=0; i < numberOfPollItems; i++)
{
//Process possible messages
if(pollItems[i].revents & ZMQ_POLLIN)
{
SOM_TRY
//printf("Poll item %d probably got a message (%d)\n", i, associatedChordNode->nodePortNumber);
pollItemIndexToFunction[i]();
SOM_CATCH("Error processing " + pollItemIndexToFunctionName[i] +"\n")
}

}

}

}
catch(const std::exception &inputException)
{
fprintf(stderr, "Exception occurred in node thread: %s\n", inputException.what());
return;
}


}


/*
This function grabs the next event in the queue and takes the corresponding action.  If there is no event left, this function returns silently.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processEvent()
{
if(eventQueue.size() == 0)
{
return; //No events, so return
}

//Retrieve top event
chordEvent eventToProcess = eventQueue.top();
eventQueue.pop();

//Process event
switch(eventToProcess.eventType)
{
case UNINITIALIZED:
printf("UNINITIALIZED event!\n");
return; //Invalid event type
break;

case SEND_STABILIZATION_REQUEST:
{
SOM_TRY
//printf("SEND_STABILIZATION_REQUEST\n");
fflush(stdout); 
processStabilizationRequestTimeout();
SOM_CATCH("Error sending stabilization request\n")
}
break;

case REQUEST_CLOSEST_KNOWN_PREDECESSOR_MAY_HAVE_TIMED_OUT:
{
printf("REQUEST_CLOSEST_KNOWN_PREDECESSOR_MAY_HAVE_TIMED_OUT\n");

if(eventToProcess.integerOptions.size() != 1)
{
return; //Event is invalid (wrong number of options)
}

//Check if request actually timed out
if(requestIDToAddressOwnerRequest.count(eventToProcess.integerOptions[0]) != 1)
{
return; //Event has been processed already
}

SOM_TRY
processPossibleRequestClosestKnownPredecessorTimeOut(eventToProcess.integerOptions[0]); //Resend the request
SOM_CATCH("Error processing timed out RequestClosestKnownPredecessor\n")
}
break;

case UPDATE_FINGER_TABLE:
{
printf("UPDATE_FINGER_TABLE\n");
if(eventToProcess.integerOptions.size() != 1)
{
return; //Event is invalid (wrong number of options)
}

if(eventToProcess.integerOptions[0] > nodeFingerTable->fingerTableSize())
{
return; //Event is invalid (can't update finger table entry that doesn't exist)
}

//Schedule next update
chordEvent updateFingerTableEvent;
updateFingerTableEvent.createUpdateFingerTableEvent(std::chrono::milliseconds(FINGER_TABLE_REFRESH_PERIOD), eventToProcess.integerOptions[0]);
eventQueue.push(updateFingerTableEvent);

SOM_TRY
updateFingerTable(eventToProcess.integerOptions[0]);
SOM_CATCH("Error, problem calling function to refresh finger table entry\n")
}
break;

case NODE_IS_ACTIVE_REQUEST_MAY_HAVE_TIMED_OUT:
{
printf("NODE_IS_ACTIVE_REQUEST_MAY_HAVE_TIMED_OUT\n");

if(eventToProcess.integerOptions.size() != 1)
{
return; //Event is invalid (wrong number of options)
}

//Check if request actually timed out
if(requestIDToNodeIsActiveRequest.count(eventToProcess.integerOptions[0]) != 1)
{
return; //Event has been processed already
}

SOM_TRY
resendNodeIsActiveRequestIfTimedOut(eventToProcess.integerOptions[0]); //Resend the request
SOM_CATCH("Error processing timed out node is active request\n")
}
break;
}


}

/*
This function sends a stabilizationRequestOrResponse to this node's successor to determine if it needs to update its successor entry and schedules the next stabilization message.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processStabilizationRequestTimeout()
{
//Schedule next stabilization request
chordEvent stabilizationRequestEvent;
stabilizationRequestEvent.createSendStabilizationRequestEvent(std::chrono::milliseconds(STABILIZATION_MESSAGE_PERIOD));
eventQueue.push(stabilizationRequestEvent);

//Make message to send
stabilizationRequestOrResponse stabilizationRequest;

chordNodeContactInformation *selfContactInfo = new chordNodeContactInformation(nodeFingerTable->selfContactInfo());
stabilizationRequest.set_allocated_sender_contact_info(selfContactInfo);  //message takes ownership of sub message

//Serialize
std::string serializedStabilizationRequest;
SOM_TRY
stabilizationRequest.SerializeToString(&serializedStabilizationRequest);
SOM_CATCH("Error serializing message\n")

//Retrieve contact info for the successor so we can send to it
chordNodeContactInformation successorContactInfo = nodeFingerTable->successorContactInfo();

//printf("successorContactInfo: IP %d port %d\n", successorContactInfo.ip(), successorContactInfo.port());

SOM_TRY
stabilizationRequestSender->send(serializedStabilizationRequest.c_str(), serializedStabilizationRequest.size(), successorContactInfo.ip(), successorContactInfo.port(), successorContactInfo.virtual_port()+STABILIZATION_REQUEST_HANDLER_PORT_OFFSET);
//printf("Sending stabilization request to virtual port %u (%d)\n", successorContactInfo.virtual_port()+STABILIZATION_REQUEST_HANDLER_PORT_OFFSET, associatedChordNode->nodePortNumber);
SOM_CATCH("Error sending stabilization request")

}

/*
This function resends requestClosestKnownPredecessor message with a new request ID, updates or deletes the associated findAddressOwner datastructure if the request actually timed out and sends a request failed message to the find request owner sender if all options have been exhausted.
@param inputRequestID: The ID of the request that may have timed out

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processPossibleRequestClosestKnownPredecessorTimeOut(uint64_t inputRequestID)
{

if(requestIDToAddressOwnerRequest.count(inputRequestID) == 0)
{
return; //Request has been serviced, so no need to do anything
}

auto ownerRequest = requestIDToAddressOwnerRequest[inputRequestID].get(); //Get pointer to address owner request

//Timeout has occurred, check if the message should just be resent or we should backtrack to a different node
if(ownerRequest->resendCount[ownerRequest->closestKnownPredecessors.top()] >= MAXIMUM_NUMBER_OF_PREDECESSOR_REQUEST_RESENDS)
{
//The resend limit has been reached, so increment it's parent bad reference count, mark the node unusable and remove it from the finger table (if it is not our own ID)
ownerRequest->badReferenceCount[ownerRequest->contactInfoSource[ownerRequest->closestKnownPredecessors.top()]]++;

//Replace the associated ID with the next best option if it is in the finger 
nodeFingerTable->replaceWithNextBestOption(ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()]);
ownerRequest->closestKnownPredecessors.pop();//Remove from path stack

//See if request has failed and act accordingly
if(ownerRequest->closestKnownPredecessors.size() == 0)
{
SOM_TRY
sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(inputRequestID);
SOM_CATCH("Error indicating find address owner request failed and cleaning up the request datastructure\n")
}

//Remove the next possible node if it has too many bad references
if(ownerRequest->badReferenceCount[ownerRequest->closestKnownPredecessors.top()] > MAXIMUM_NUMBER_OF_BAD_REFERENCES)
{
nodeFingerTable->replaceWithNextBestOption(ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()]);
ownerRequest->closestKnownPredecessors.pop();//Remove from path stack
}
}
else
{
ownerRequest->resendCount[ownerRequest->closestKnownPredecessors.top()]++;
}

//See if request has failed and act accordingly
if(ownerRequest->closestKnownPredecessors.size() == 0)
{
SOM_TRY
sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(inputRequestID);
SOM_CATCH("Error indicating find address owner request failed and cleaning up the request datastructure\n")
}

//Send message and schedule timeout
SOM_TRY
sendFindPredecessorRequest(inputRequestID);
SOM_CATCH("Error sending find closest predecessor request and setting up timeout\n")
}

/*
This function sends a findAddressOwnerRequest and creates a entry in requestIDToFingerTableEntryIndex to ensure that the corresponding finger table entry is updated when the result is returned.
@param inputFingerTableIndex: The index of the finger table to update

@exceptions: This function can throw exceptions
*/
void chordNodeResources::updateFingerTable(uint64_t inputFingerTableIndex)
{
//TODO: Remove this
static int fingerTableUpdateHasTakePlace = 0;
if(fingerTableUpdateHasTakePlace == 1)
{
return;
}
else
{
fingerTableUpdateHasTakePlace = 1;
}

//Check to ensure that the index is within bounds
if(inputFingerTableIndex >= nodeFingerTable->fingerTableSize())
{
throw SOMException("Error, finger table index out of bounds\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Get a request ID
uint64_t requestID = getNextMessageNumber();

//Create message to send
findAddressOwnerRequestOrResponse updateRequest;
updateRequest.set_target_chord_address(nodeFingerTable->targetChordAddressesInBinary[inputFingerTableIndex]);
updateRequest.set_internal_request_id(requestID);

printf("Finger table target address size: %ld\n", nodeFingerTable->targetChordAddressesInBinary[inputFingerTableIndex].size());

//Serialize request
std::string serializedUpdateRequest;
SOM_TRY
updateRequest.SerializeToString(&serializedUpdateRequest);
SOM_CATCH("Error serializing message\n")

//Send message
printf("Sending find node associated with address request for finger table update\n");
SOM_TRY
//Send empty delimiter since we are using a dealer socket
findNodeAssociatedWithAddressRequestSender->send(NULL, 0, ZMQ_SNDMORE);
//Send message
findNodeAssociatedWithAddressRequestSender->send(serializedUpdateRequest.c_str(), serializedUpdateRequest.size());
SOM_CATCH("Error sending finger table update request\n")

//Make entry to allow update on return
requestIDToFingerTableEntryIndex[requestID] = inputFingerTableIndex;
}

/*
This function resends nodeIsActiveRequestResponse message with a new request ID if it has timed out.
@param inputRequestID: The ID of the request that may have timed out

@exceptions: This function can throw exceptions
*/
void chordNodeResources::resendNodeIsActiveRequestIfTimedOut(uint64_t inputRequestID)
{
//Check to make sure the request has not been processed
if(requestIDToNodeIsActiveRequest.count(inputRequestID) == 0)
{
return;  //Request has already been returned
}

//If the maximum number of timeouts have been reached, clean up the entry and return
if(requestIDToNodeIsActiveRequest[inputRequestID]->resendCount >= MAXIMUM_NUMBER_OF_IS_NODE_ACTIVE_RESENDS)
{
requestIDToNodeIsActiveRequest.erase(inputRequestID);
return;
}

//Increment count and move to new request ID
requestIDToNodeIsActiveRequest[inputRequestID]->resendCount++;
uint64_t requestID = getNextMessageNumber();
requestIDToNodeIsActiveRequest[requestID] = std::move(requestIDToNodeIsActiveRequest[inputRequestID]);
requestIDToNodeIsActiveRequest.erase(inputRequestID);

//Make message
nodeIsActiveRequestResponse request;
request.set_allocated_sender_contact_info(new chordNodeContactInformation(nodeFingerTable->selfContactInfo()));

//Serialize
std::string serializedRequest;
SOM_TRY
request.SerializeToString(&serializedRequest);
SOM_CATCH("Error serializing message\n")

//Create another timeout event
chordEvent timeoutEvent;
timeoutEvent.createNodeIsActiveTimeoutEvent(std::chrono::milliseconds(IS_NODE_ACTIVE_TIMEOUT_DELAY), requestID);
eventQueue.push(timeoutEvent);

//Resend request
SOM_TRY
nodeIsActiveRequestSender->send(serializedRequest.c_str(), serializedRequest.size(), requestIDToNodeIsActiveRequest[requestID]->nodeToCheckIfActive.ip(), requestIDToNodeIsActiveRequest[requestID]->nodeToCheckIfActive.port(), requestIDToNodeIsActiveRequest[requestID]->nodeToCheckIfActive.virtual_port()+NODE_IS_ACTIVE_REQUEST_HANDLER_PORT_OFFSET, requestID);
printf("Sending node is active request to virtual port (%d): %u\n", associatedChordNode->nodePortNumber, requestIDToNodeIsActiveRequest[requestID]->nodeToCheckIfActive.virtual_port()+NODE_IS_ACTIVE_REQUEST_HANDLER_PORT_OFFSET);
SOM_CATCH("Error resending node is active request\n")
}

/*
This function extracts the next processClosestPredecessorRequest that was gotten in response to one of this node's queries.  It then retrieves the associated findAddressOwnerRequest and performs the required operations on it.  This can include sending the next message, reassigning the findAddressOwnerRequest to a different request number and marking chordIDs as unreliable (and so, not usable).

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processClosestPredecessorRequestReply()
{
//Retrieve closestPredecessorRequestReply
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

printf("Processing ClosestPredecessorRequestReply (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = closestPredecessorRequestSender->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Deserialize message
closestPredecessorsRequestResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
printf("Message initialization failed\n");
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
printf("Invalid header\n");
return; //Invalid header
}

//Update sender info with what we saw from it's UDP packets
messageBuffer.mutable_sender_contact_info()->set_ip(headerBuffer.udpip());
messageBuffer.mutable_sender_contact_info()->set_port(headerBuffer.udpportnumber());

//Check if the associated request number is around
if(requestIDToAddressOwnerRequest.count(headerBuffer.transactionid()) == 0)
{
printf("Associated request number is not found\n");
return; //It's not, so reject message
}
findAddressOwnerRequest *requestOwner = requestIDToAddressOwnerRequest[headerBuffer.transactionid()].get();

//Get the current stack of the predecessors so that we can check and see if the stack isn't updated by this message and drop back to a lower level
boost::multiprecision::uint512_t topStackID;
if(requestOwner->closestKnownPredecessors.size() > 0)
{
topStackID = requestOwner->closestKnownPredecessors.top();
}

//Update finger tables with the information that has been gleamed from this request
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.sender_contact_info());
if(messageBuffer.has_responder_successor_contact_info())
{
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.responder_successor_contact_info());
}
for(int i=0; i<messageBuffer.closest_predecessor_contact_infos().size() && i <  NUMBER_OF_PREDECESSORS_TO_GET_PER_NODE; i++)
{
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.closest_predecessor_contact_infos(i));
}

if(messageBuffer.has_responder_successor_contact_info())
{
//Deserialize the returned ID
boost::multiprecision::uint512_t deserializedResponderSuccessorID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(messageBuffer.responder_successor_contact_info().chord_id().c_str(), messageBuffer.responder_successor_contact_info().chord_id().size(), deserializedResponderSuccessorID) != 1)
{
return; //Couldn't make int from array
}

//See if the request has been completed (lastChordID-requestChordID) > (ownerChordID - requestChordID)
printf("Checking if best successor seen\n");
if(requestOwner->checkIfBestSuccessorSeen(deserializedResponderSuccessorID) == 0)
{
//The successor ID seems bad, so mark the node as untrusted and let the timeout event continue the search process
requestOwner->badReferenceCount[requestOwner->closestKnownPredecessors.top()] = MAXIMUM_NUMBER_OF_BAD_REFERENCES;
return;
} 

printf("It is best successor seen, so sending response (request successful)\n");
//Successor seems valid, so return the result and clean up
SOM_TRY
sendNodeAssociatedWithRequestMessageAndCleanupAssociatedData(headerBuffer.transactionid(), messageBuffer.responder_successor_contact_info());
printf("Response to find address owner request sent\n");
SOM_CATCH("Error sending address owner response\n")
return;
}

//Update associated request data
for(int i=0; i<messageBuffer.closest_predecessor_contact_infos().size() && i <  NUMBER_OF_PREDECESSORS_TO_GET_PER_NODE; i++)
{
requestOwner->addClosestKnownPredecessorIfNew(messageBuffer.closest_predecessor_contact_infos(i), messageBuffer.sender_contact_info());
}

//If the stack size has reached zero, send request failed message TODO
if(requestOwner->closestKnownPredecessors.size() == 0)
{
printf("Stack emptied, giving up!!!!!!!!!!!!!!!\n");

SOM_TRY
sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(headerBuffer.transactionid());
SOM_CATCH("Error indicating find address owner request failed and cleaning up the request datastructure\n")
return;
}
else if(requestOwner->closestKnownPredecessors.top() == topStackID)
{
requestOwner->closestKnownPredecessors.pop(); //Top node to contact unchanged, so try contacting the one before it or at least reduce stack size toward termination

if(requestOwner->closestKnownPredecessors.size() == 0)
{
SOM_TRY
sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(headerBuffer.transactionid());
SOM_CATCH("Error indicating find address owner request failed and cleaning up the request datastructure\n")
return;
}
}

//Send next request
SOM_TRY
sendFindPredecessorRequest(headerBuffer.transactionid());
SOM_CATCH("Error generating/sending closest predecessor request\n")
}

/*
This function extracts the next stabilizationRequestOrResponse that has been received and updates the successor entry if needed.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processStabilizationRequestReply()
{
//Retrieve stabilizationRequestOrResponse
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

//printf("Processing stabilization reply (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = stabilizationRequestSender->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Deserialize message
stabilizationRequestOrResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
return; //Invalid header
}

//Update sender info with what we saw from it's UDP packets
messageBuffer.mutable_sender_contact_info()->set_ip(headerBuffer.udpip());
messageBuffer.mutable_sender_contact_info()->set_port(headerBuffer.udpportnumber());

//Verify that the message is from the right node
boost::multiprecision::uint512_t deserializedSenderID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(messageBuffer.sender_contact_info().chord_id().c_str(), messageBuffer.sender_contact_info().chord_id().size(), deserializedSenderID) != 1)
{
return; //Couldn't make int from array
}

if(deserializedSenderID != nodeFingerTable->successorID)
{
return; //Wrong sender, ignore this message
}

//Update if better
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.predecessor_contact_info());
}


/*
This function extracts the next processNodeIsActiveRequestReply (which is a empty message with a header and updates the finger table based on it.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processNodeIsActiveRequestReply()
{
//Retrieve nodeIsActiveRequestResponse
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

printf("Processing node is active reply (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = nodeIsActiveRequestSender->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Deserialize message
nodeIsActiveRequestResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
return; //Invalid header
}

//Verify that it matches one of our requests
if(requestIDToNodeIsActiveRequest.count(headerBuffer.transactionid()) == 0)
{
return; //It doesn't, so ignore it
}

//Update sender info with what we saw from it's UDP packets
messageBuffer.mutable_sender_contact_info()->set_ip(headerBuffer.udpip());
messageBuffer.mutable_sender_contact_info()->set_port(headerBuffer.udpportnumber());

//Verify that the message is from the right node
boost::multiprecision::uint512_t deserializedSenderID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(messageBuffer.sender_contact_info().chord_id().c_str(), messageBuffer.sender_contact_info().chord_id().size(), deserializedSenderID) != 1)
{
return; //Couldn't make int from array
}

if(deserializedSenderID != requestIDToNodeIsActiveRequest[headerBuffer.transactionid()]->nodeToCheckIfActiveID)
{
return; //Wrong ID, so ignore
}

//Update if better and remove request from map
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.sender_contact_info());
requestIDToNodeIsActiveRequest.erase(headerBuffer.transactionid());
}

/*
This function extracts the next findAddressOwnerRequestReply and takes the corresponding action depending on which type of action the request was for.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processFindAddressOwnerRequestReply()
{
printf("Processing find address owner request reply\n");
zmq::message_t ZMQMessageBuffer;

SOM_TRY
findNodeAssociatedWithAddressRequestSender->recv(&ZMQMessageBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Deserialize message
findAddressOwnerRequestOrResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), ZMQMessageBuffer.size());

SOM_TRY
if(!messageBuffer.IsInitialized())
{
printf("Message deserialization failed\n");
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

if(!messageBuffer.has_chord_address_owner_contact_info())
{
printf("Address owner request failed\n");
return; //Invalid message
}

if(!messageBuffer.has_internal_request_id())
{
printf("Message didn't have internal request id\n");
return; //Invalid message
}

//So far, we are only using this form of the function for updating finger table entries
if(requestIDToFingerTableEntryIndex.count(messageBuffer.internal_request_id()) == 0)
{
printf("Internal request ID doesn't have a map entry\n");
return; //Doesn't match any finger table entry
} 

SOM_TRY
nodeFingerTable->updateChordMatch(requestIDToFingerTableEntryIndex[messageBuffer.internal_request_id()], messageBuffer.chord_address_owner_contact_info());
SOM_CATCH("Error updating finger table\n") 

//Remove associated map entry
requestIDToFingerTableEntryIndex.erase(messageBuffer.internal_request_id());
}

/*
This function extracts the next findAddressOwnerRequestOrResponse message, creates a findAddressOwnerRequest based on it, sends the first request then creates the associated timeout event and adds it to the event queue.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processFindNodeAssociatedWithAddressRequest()
{
std::unique_ptr<zmq::message_t> ZMQMessageBuffer;
std::vector<std::unique_ptr<zmq::message_t> > messageParts;
int64_t morePartsFlag = 1;

printf("Getting find node associated with address request\n"); 

//Message is likely to be multi part
while(morePartsFlag == 1)
{
//Load new message buffer
SOM_TRY
ZMQMessageBuffer.reset(new zmq::message_t());
SOM_CATCH("Error creating message buffer\n")

SOM_TRY
findNodeAssociatedWithAddressHandler->recv(ZMQMessageBuffer.get());
SOM_CATCH("Error receiving message\n")

messageParts.push_back(std::move(ZMQMessageBuffer)); //Add to vector

auto morePartsFlagSize = sizeof(morePartsFlag);
SOM_TRY
findNodeAssociatedWithAddressHandler->getsockopt(ZMQ_RCVMORE, &morePartsFlag, &morePartsFlagSize);
SOM_CATCH("Error determining if message has more parts\n")
}

printf("Receieved %ld messages as part of find address owner request\n", messageParts.size());

//Find empty part in multi part message and place index at the next message 
int index;
for(int i=0; i< messageParts.size(); i++)
{
if(messageParts[i]->size() == 0)
{
index = i+1;
}
}

if(index == 0 || index >= messageParts.size())
{
printf("Index invalid\n");
return; //There was no request message and/or connection ID, so the value pointed at by the index is invalid or the connection ID can't be used
}

//Deserialize message
findAddressOwnerRequestOrResponse messageBuffer;
messageBuffer.ParseFromArray(messageParts[index]->data(), messageParts[index]->size());

SOM_TRY
if(!messageBuffer.IsInitialized())
{
printf("Message failed to initialize\n");
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

if(!messageBuffer.has_target_chord_address())
{
printf("Message does not have target chord address\n");
return; //Message doesn't have a address to resolve
} 

//Deserialize target chord address
boost::multiprecision::uint512_t deserializedTargetChordAddress;

//printf("Array size: %ld\n", messageBuffer.target_chord_address().size());

//Assign ID to corresponding entry
if(createUInt512FromRawHash(messageBuffer.target_chord_address().c_str(), messageBuffer.target_chord_address().size(), deserializedTargetChordAddress) != 1)
{
printf("Couldn't make int from array\n");
return; //Couldn't make int from array
}



//Get closest known predecessor node info
std::vector<chordNodeContactInformation> closestKnownPredecessors;
SOM_TRY
closestKnownPredecessors = nodeFingerTable->getClosestPredecessors(deserializedTargetChordAddress, 1);
SOM_CATCH("Error getting closest known predecessors")

printf("Closest known predecessor ID size: %ld\n", closestKnownPredecessors[0].chord_id().size());

//Create associated request structure
std::unique_ptr<findAddressOwnerRequest> requestStateData;
if(messageBuffer.has_internal_request_id())
{
SOM_TRY
requestStateData.reset(new findAddressOwnerRequest(deserializedTargetChordAddress, closestKnownPredecessors[0], nodeFingerTable->selfContactInfo(), messageBuffer.internal_request_id()));
SOM_CATCH("Error creating state for find address owner request\n")
}
else
{
SOM_TRY
requestStateData.reset(new findAddressOwnerRequest(deserializedTargetChordAddress, closestKnownPredecessors[0], nodeFingerTable->selfContactInfo()));
SOM_CATCH("Error creating state for find address owner request\n")
}

//Move the request structure to a request ID location
uint64_t requestID = getNextMessageNumber();
requestIDToAddressOwnerRequest[requestID] = std::move(requestStateData);

//Add request ID to connection ID mapping 
findAddressOwnerRequestIDToConnectionID[requestID] = std::string( (const char *) messageParts[index-2]->data(), messageParts[index-2]->size()); 

//If this node's ID is the closest predecessor, send back our successor as the address owner and clean up TODO
if(closestKnownPredecessors[0].chord_id() == nodeFingerTable->selfContactInfo().chord_id())
{
SOM_TRY
sendNodeAssociatedWithRequestMessageAndCleanupAssociatedData(requestID, nodeFingerTable->successorContactInfo());
printf("The best predecessor was this node, so sending response to find address owner request\n");
SOM_CATCH("Error sending address owner response\n")
return;
}

printf("Find node associated with address request received, about to send first predecessor request\n"); 

//Otherwise send the next request
SOM_TRY
sendFindPredecessorRequest(requestID);
printf("Sent first predecessor request\n");
SOM_CATCH("Error sending predecessor request message\n")
}

/*
This function extracts the next closestPredecessorRequest (from an external node) and answers it using the finger table to compute the required number of closest predecessors.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processClosestKnownPredecessorRequest()
{
//Retrieve request
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

printf("Received closest known predecessor request (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = closestKnownPredecessorRequestsHandler->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
return; //Invalid header
}

//Deserialize message
closestPredecessorsRequest messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
printf("Message deserialization failed here!!!!!!!!!!! (%d)\n", associatedChordNode->nodePortNumber);
return; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Add the sender to our finger table if they are better than one of the current entries
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.sender_contact_info());

//Deserialize target chord address
boost::multiprecision::uint512_t deserializedTargetChordAddress;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(messageBuffer.target_chord_address().c_str(), messageBuffer.target_chord_address().size(), deserializedTargetChordAddress) != 1)
{
return; //Couldn't make int from array
}

//Start making message to send back
closestPredecessorsRequestResponse messageToSend;
messageToSend.set_allocated_sender_contact_info(new chordNodeContactInformation(nodeFingerTable->selfContactInfo()));



//Check if our successor owns the target address and send back the successor if it does
boost::multiprecision::uint512_t successorDistance = (nodeFingerTable->successorID-deserializedTargetChordAddress); 
boost::multiprecision::uint512_t selfDistance = (nodeFingerTable->chordID - deserializedTargetChordAddress);

std::string stringToPrint;
std::string buffer;

stringToPrint = "Target chord address: ";
buffer = convertUInt512ToDecimalString(deserializedTargetChordAddress);
stringToPrint += buffer.substr(0,5-(155-buffer.size())) + "\n";

stringToPrint += "Self address: ";
buffer = convertUInt512ToDecimalString(nodeFingerTable->chordID);
stringToPrint += buffer.substr(0,5-(155-buffer.size())) + "\n";

stringToPrint += "Successor address: ";
buffer = convertUInt512ToDecimalString(nodeFingerTable->successorID);
stringToPrint += buffer.substr(0,5-(155-buffer.size())) + "\n";

stringToPrint += "Self distance: ";
buffer = convertUInt512ToDecimalString(selfDistance);
stringToPrint += buffer.substr(0,5-(155-buffer.size())) + "\n";

stringToPrint += "Successor distance: ";
buffer = convertUInt512ToDecimalString(successorDistance);
stringToPrint += buffer.substr(0,5-(155-buffer.size())) + "\n";

printf("%s\n", stringToPrint.c_str());

if((nodeFingerTable->successorID-deserializedTargetChordAddress) < (nodeFingerTable->chordID - deserializedTargetChordAddress))
{
messageToSend.set_allocated_responder_successor_contact_info(new chordNodeContactInformation(nodeFingerTable->successorContactInfo()));
printf("Sending back a successor\n");
}


//Find the closest known predecessors as part of answering the request and add it to message if the successor hasn't already been added
if(!messageToSend.has_responder_successor_contact_info())
{
std::vector<chordNodeContactInformation> closestKnownPredecessors;
uint64_t numberOfPredecessorsToReturn = messageBuffer.number_of_predecessors_to_return() < nodeFingerTable->fingerTableSize() ? messageBuffer.number_of_predecessors_to_return() : nodeFingerTable->fingerTableSize(); //Min of the two
SOM_TRY
closestKnownPredecessors = nodeFingerTable->getClosestPredecessors(deserializedTargetChordAddress, numberOfPredecessorsToReturn);
SOM_CATCH("Error, unable to get closest known predecessors\n")

for(int i=0; i<closestKnownPredecessors.size(); i++)
{
chordNodeContactInformation *newContactPointer = messageToSend.add_closest_predecessor_contact_infos();
(*newContactPointer) = closestKnownPredecessors[i];
}
}

//Serialized created message
std::string serializedMessageToSend;
SOM_TRY
messageToSend.SerializeToString(&serializedMessageToSend);
SOM_CATCH("Error serializing message\n")

//Send created message
SOM_TRY
closestKnownPredecessorRequestsHandler->send(serializedMessageToSend.c_str(), serializedMessageToSend.size(), headerBuffer.udpip(), headerBuffer.udpportnumber(), headerBuffer.sendervirtualportid(), headerBuffer.transactionid());
printf("Sending closest known predecessor response to virtual port (%d) %u\n", associatedChordNode->nodePortNumber, headerBuffer.sendervirtualportid());


boost::multiprecision::uint512_t buffer;
std::string stringBuffer;
if(messageToSend.closest_predecessor_contact_infos().size() > 0)
{
printf("With predecessor IDs:\n");
for(int i=0; i<messageToSend.closest_predecessor_contact_infos().size(); i++)
{
createUInt512FromRawHash(messageToSend.closest_predecessor_contact_infos(i).chord_id().c_str(), messageToSend.closest_predecessor_contact_infos(i).chord_id().size(), buffer);
stringBuffer = convertUInt512ToDecimalString(buffer);
stringBuffer = stringBuffer.substr(0,5-(155-stringBuffer.size()));
printf("%s\n", stringBuffer.c_str());
}
}
SOM_CATCH("Error sending closestPredecessorsRequestResponse\n")
}


/*
This function extracts the next stabilizationRequestOrResponse (from an external node) and answers it using the finger table to get this node's current predecessor.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processStabilizationRequest()
{
//Retrieve request
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

//printf("Received stabilization request (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = stabilizationRequestHandler->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
return; //Invalid header
}

//Deserialize message
stabilizationRequestOrResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
return; //Message deserialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Add the sending node to our finger table if it is better
//printf("Updating finger table if requester is better\n");
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.sender_contact_info());

//Create response message
stabilizationRequestOrResponse responseMessage;

responseMessage.set_allocated_sender_contact_info(new chordNodeContactInformation(nodeFingerTable->selfContactInfo()));
responseMessage.set_allocated_predecessor_contact_info(new chordNodeContactInformation(nodeFingerTable->predecessorContactInfo()));

//Serialize response message
std::string serializedResponseMessage;
SOM_TRY
responseMessage.SerializeToString(&serializedResponseMessage);
SOM_CATCH("Error serializing message\n")

//Send response
SOM_TRY
stabilizationRequestHandler->send(serializedResponseMessage.c_str(), serializedResponseMessage.size(), headerBuffer.udpip(), headerBuffer.udpportnumber(), headerBuffer.sendervirtualportid(), headerBuffer.transactionid());
//printf("Sending stabilization response to virtual port (%d) %u\n", associatedChordNode->nodePortNumber,  headerBuffer.sendervirtualportid());
SOM_CATCH("Error sending stabilizationRequestOrResponse\n")
}

/*
This function extracts the next nodeIsActiveRequest (from an external node) and answers to prove that this node is active.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void chordNodeResources::processNodeIsActiveRequest()
{
//Retrieve request
zmq::message_t ZMQMessageBuffer;
virtualPortMessageHeader headerBuffer;
int contentSize;

printf("Received node is active request (%d)\n", associatedChordNode->nodePortNumber);

SOM_TRY
contentSize = nodeIsActiveRequestHandler->recv(ZMQMessageBuffer, headerBuffer);
SOM_CATCH("Error receiving datagram message\n")

//Validate header
if(!headerBuffer.has_destinationvirtualportid() && !headerBuffer.has_sendervirtualportid() && !headerBuffer.has_transactionid() && !headerBuffer.has_udpportnumber() && !headerBuffer.has_udpip())
{
return; //Invalid header
}

//Deserialize message
nodeIsActiveRequestResponse messageBuffer;
messageBuffer.ParseFromArray(ZMQMessageBuffer.data(), contentSize);

SOM_TRY
if(!messageBuffer.IsInitialized())
{
return; //Message deserialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

//Add the sending node to our finger table if it is better
nodeFingerTable->checkAndUpdateIfBetter(messageBuffer.sender_contact_info());

//Create response
nodeIsActiveRequestResponse responseMessage;
responseMessage.set_allocated_sender_contact_info(new chordNodeContactInformation(nodeFingerTable->selfContactInfo()));

//Serialize response
std::string serializedResponseMessage;
SOM_TRY
responseMessage.SerializeToString(&serializedResponseMessage);
SOM_CATCH("Error serializing message\n")

//Send the response
SOM_TRY
nodeIsActiveRequestHandler->send(serializedResponseMessage.c_str(), serializedResponseMessage.size(), headerBuffer.udpip(), headerBuffer.udpportnumber(), headerBuffer.sendervirtualportid(), headerBuffer.transactionid());
printf("Sending node is active response to virtual port (%d->%d) %u->%u\n", associatedChordNode->nodePortNumber, headerBuffer.udpportnumber(), nodeIsActiveRequestHandler->associatedVirtualPort, headerBuffer.sendervirtualportid());
SOM_CATCH("Error sending nodeIsActiveRequestResponse\n")
}



/*
This function sends the message associated with a findAddressOwner request by looking up the relavent data structure and sending the request associated.  It is usually used to send a message after the datastructure is updated as part of the resolution process.  It moves the data to a new request number as it sends the message.
@param inputRequestID: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void chordNodeResources::sendFindPredecessorRequest(uint64_t inputRequestID)
{
printf("Sending find predecessor request\n");

if(requestIDToAddressOwnerRequest.count(inputRequestID) != 1)
{
printf("Throwing exception\n");
throw SOMException("Error, told to send predecessor request without a valid associated structure\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Get pointer to address owner request
auto ownerRequest = requestIDToAddressOwnerRequest[inputRequestID].get(); 


if(ownerRequest->closestKnownPredecessors.size() == 0)
{
printf("Stack is empty\n");
throw SOMException("Error, request structure has zero candidates to try in the queue\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Make request to send
closestPredecessorsRequest requestToSend;
chordNodeContactInformation *selfContactInfo = new chordNodeContactInformation(nodeFingerTable->selfContactInfo());
requestToSend.set_allocated_sender_contact_info(selfContactInfo);  //message takes ownership of sub message

requestToSend.set_target_chord_address(ownerRequest->addressToFindBinary);
requestToSend.set_number_of_predecessors_to_return(NUMBER_OF_PREDECESSORS_TO_GET_PER_NODE);

printf("Made it this far (size: %ld)\n", ownerRequest->closestKnownPredecessors.size());

std::string serializeRequestToSend;
SOM_TRY
requestToSend.SerializeToString(&serializeRequestToSend);
SOM_CATCH("Error serializing message\n")

//Get request ID to associated the datagram with and update the maps to point the ID to this request info structure
auto requestID = getNextMessageNumber();
requestIDToAddressOwnerRequest[requestID] = std::move(requestIDToAddressOwnerRequest[inputRequestID]); //Move ownership
requestIDToAddressOwnerRequest.erase(inputRequestID); //Remove old entry
ownerRequest = requestIDToAddressOwnerRequest[requestID].get();

//Update connection ID entry too 
findAddressOwnerRequestIDToConnectionID[requestID] = findAddressOwnerRequestIDToConnectionID[inputRequestID]; 
findAddressOwnerRequestIDToConnectionID.erase(inputRequestID);


//Add another timeout event for the request we are sending
chordEvent timeoutEvent;
timeoutEvent.createClosestKnownPredecessorTimeoutEvent(std::chrono::milliseconds(CLOSEST_KNOWN_PREDECESSOR_REQUEST_TIMEOUT_DELAY), requestID);
eventQueue.push(timeoutEvent);


//Send datagram using virtual port router
printf("About to try sending find predecessor request\n");
SOM_TRY
closestPredecessorRequestSender->send(
serializeRequestToSend.c_str(), 
serializeRequestToSend.size(), 
ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()].ip(), 
ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()].port(), 
ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()].virtual_port()+CLOSEST_KNOWN_PREDECESSOR_REQUESTS_HANDLER_PORT_OFFSET, requestID);
printf("Sent closest predecessor request to virtual port (%d) %u\n", associatedChordNode->nodePortNumber,  ownerRequest->idToContactInfo[ownerRequest->closestKnownPredecessors.top()].virtual_port()+CLOSEST_KNOWN_PREDECESSOR_REQUESTS_HANDLER_PORT_OFFSET);
SOM_CATCH("Error sending datagram via virtual port router")
printf("find predecessor request sent\n");
}


/*
This function sends a message indicating request failure and cleans up data associated with serving that request.  
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void chordNodeResources::sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(uint64_t inputAddressOwnerRequestNumber)
{
SOM_TRY
sendNodeAssociatedWithAddressRequestFailedMessage(inputAddressOwnerRequestNumber);
SOM_CATCH("Error, unable to send find address owner request failed message\n")

//Clean up associated map entries
findAddressOwnerRequestIDToConnectionID.erase(inputAddressOwnerRequestNumber);
requestIDToAddressOwnerRequest.erase(inputAddressOwnerRequestNumber);
findAddressOwnerRequestIDToConnectionID.erase(inputAddressOwnerRequestNumber);
}


/*
This message is used to indicate failure of a request.  It retrieves the connection ID associated with the sender of the request and sends the message to that ID.  
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void chordNodeResources::sendNodeAssociatedWithAddressRequestFailedMessage(uint64_t inputAddressOwnerRequestNumber)
{
//Make sure there is an associated connection ID
if(findAddressOwnerRequestIDToConnectionID.count(inputAddressOwnerRequestNumber) == 0)
{
throw SOMException("Error, no zmq connection ID associated with the find addess owner request\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Make message to send
findAddressOwnerRequestOrResponse emptyResponse;
std::string serializedEmptyResponse;

//Serialize
SOM_TRY
emptyResponse.SerializeToString(&serializedEmptyResponse);
SOM_CATCH("Error serializing message\n")
std::string connectionID = findAddressOwnerRequestIDToConnectionID[inputAddressOwnerRequestNumber];


//Send
SOM_TRY
//Send ID frame to identify which requester to send to
findNodeAssociatedWithAddressHandler->send(connectionID.c_str(), connectionID.size(), ZMQ_SNDMORE);
//Send empty frame as delimiter
findNodeAssociatedWithAddressHandler->send(nullptr, 0, ZMQ_SNDMORE);
//Send message
findNodeAssociatedWithAddressHandler->send(serializedEmptyResponse.c_str(), serializedEmptyResponse.size());
SOM_CATCH("Error sending failed address lookup response\n")
}

/*
This function sends a message to return the owner of a particular address in response to a query.  After sending the message, it cleans up the data that was stored as part of processing the request.
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request
@param inputAddressOwner: The contact info of the address owner

@exceptions: This function can throw exceptions
*/
void chordNodeResources::sendNodeAssociatedWithRequestMessageAndCleanupAssociatedData(uint64_t inputAddressOwnerRequestNumber, const chordNodeContactInformation &inputAddressOwner)
{
if(requestIDToAddressOwnerRequest.count(inputAddressOwnerRequestNumber) == 0)
{
printf("Request ID is invalid\n");
return; //Request ID is invalid
}
findAddressOwnerRequest *requestOwner = requestIDToAddressOwnerRequest[inputAddressOwnerRequestNumber].get();

//Make message to send
//Make sure there is an associated connection ID 
if(findAddressOwnerRequestIDToConnectionID.count(inputAddressOwnerRequestNumber) == 0)
{
printf("No zmq connection ID associated with the find address owner request\n");
throw SOMException("Error, no zmq connection ID associated with the find address owner request\n", INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Make message to send
findAddressOwnerRequestOrResponse response;
response.set_allocated_chord_address_owner_contact_info(new chordNodeContactInformation(inputAddressOwner));
response.set_internal_request_id(requestOwner->findAddressOwnerRequestNumber);

printf("I got this far\n");

//Serialize
std::string serializedResponse;
SOM_TRY
response.SerializeToString(&serializedResponse);
SOM_CATCH("Error serializing message\n")

std::string connectionID = findAddressOwnerRequestIDToConnectionID[inputAddressOwnerRequestNumber];

//Clean up associated map entries
findAddressOwnerRequestIDToConnectionID.erase(inputAddressOwnerRequestNumber);
requestIDToAddressOwnerRequest.erase(inputAddressOwnerRequestNumber);
findAddressOwnerRequestIDToConnectionID.erase(inputAddressOwnerRequestNumber);

//Send
SOM_TRY
//Send ID frame to identify which requester to send to
findNodeAssociatedWithAddressHandler->send(connectionID.c_str(), connectionID.size(), ZMQ_SNDMORE);
//Send empty frame as delimiter
findNodeAssociatedWithAddressHandler->send(nullptr, 0, ZMQ_SNDMORE);
//Send message
findNodeAssociatedWithAddressHandler->send(serializedResponse.c_str(), serializedResponse.size());
SOM_CATCH("Error sending address lookup response\n")
printf("Three messages sent\n");
}

