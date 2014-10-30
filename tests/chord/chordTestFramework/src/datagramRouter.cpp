#include "datagramRouter.hpp"



/*
This function initializes the object and starts two threads that conducts the operations of the router.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read.
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
datagramRouter::datagramRouter(zmq::context_t *inputZMQContext)
{
context = inputZMQContext;
timeToShutdownFlag = false;

//Create and bind datagram socket

//Create descriptor
datagramSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM,0);
if(datagramSocketFileDescriptor <= 0)
{
throw SOMException(std::string("Error initializing datagram socket for datagram router\n"), SYSTEM_ERROR, __FILE__, __LINE__);
return;
}
//Socket gets cleaned up in destructor, which cases thread to wake up and return

//Try to bind until we find a port we can use 1024 to 49151
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distribution(1024, 49151);
//std::uniform_int_distribution<> distribution(1024, 2024);

for(int i=1024; i<49151; i++)
//for(int i=1024; i<2024; i++)
{
int portToTry = distribution(gen);
struct sockaddr_in datagramSocketAddress;

memset(&datagramSocketAddress, 0, sizeof(datagramSocketAddress));
datagramSocketAddress.sin_family = AF_INET;
datagramSocketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
datagramSocketAddress.sin_port = htons(portToTry); 

if(bind(datagramSocketFileDescriptor, (struct sockaddr *) &datagramSocketAddress, sizeof(datagramSocketAddress)) == 0)
{
datagramPortNumber = portToTry;
break;
}

}

//Generate a random inproc address
//Generate a random string to be the info that this chord node is storing
std::string lookupTable("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

std::uniform_int_distribution<> stringGenerationDistribution(0, lookupTable.size()-1);

//Generate 10 random characters
for(int i=0; i<10; i++)
{
routerExtension.push_back(lookupTable[stringGenerationDistribution(gen)]);
}

//Create relatively unique inproc addresses for this router
sendingAddress = "datagramRouter." + routerExtension;
predecessorRequestAddress = "predecessorRequest." + routerExtension;
predecessorRequestResponseAddress = "predecessorRequestResponse." + routerExtension;
closestPredecessorRequestAddress = "closestPredecessorRequest." + routerExtension;
closestPredecessorRequestResponseAddress = "closestPredecessorRequestResponse." + routerExtension;
informationRequestAddress = "closestPredecessorRequestResponse." + routerExtension;
informationRequestResponseAddress = "closestPredecessorRequestResponse." + routerExtension;


std::promise<bool> datagramRouterThreadInitializationResult;

//Start a thread and pass it a this pointer
datagramRouterThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunDatagramRouterThread, this, &datagramRouterThreadInitializationResult));

//This should block until the datagram receiving thread has been initialized and throw any exceptions that the datagram receiving thread threw during its initialization
if(datagramRouterThreadInitializationResult.get_future().get() == true)
{
   //printf("Thread reports that it initialized successfully\n");
}

}

/*
This function signals for the threads to shut down and then waits for it to do so.
*/
datagramRouter::~datagramRouter()
{
timeToShutdownFlag = true;
close(datagramSocketFileDescriptor);  //Close file descriptor to clean up and cause datagramRouterThread to wake up if it was blocking
datagramRouterThread->join();
}

/*
This function is run as a seperate thread and implements the datagram router behavior
@param inputDatagramRouter: A pointer to the object associated with the object this function is operating on
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunDatagramRouterThread(datagramRouter *inputDatagramRouter, std::promise<bool> *inputPromise)
{

//Initialize sockets and map
std::unique_ptr<datagramRouterThreadResources> resources;

try
{
SOM_TRY
resources.reset(new datagramRouterThreadResources(inputDatagramRouter));
SOM_CATCH("Error initializing sockets for datagram router thread\n")
}
catch(const std::exception &inputException)
{
inputPromise->set_exception(std::current_exception());
return;
}

//Tell constructor that initialization completed successfully
inputPromise->set_value(true); //Initialization completed successfully

//All the sockets and poll objects have been initialized, so now start listening for events on the datagram socket and outbound datagram inproc socket



while(inputDatagramRouter->timeToShutdownFlag != true)
{ 

int numberOfEvents = resources->waitForEvent();


if(numberOfEvents < 0)
{
//Error occurred, terminate thread
return;
}

if(numberOfEvents == 0)
{
//No events occurred
continue;
}

try
{
resources->checkForAndHandleDatagrams();
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error handling datagram in datagram router: %s", inputException.what());
}

try
{
resources->checkForAndHandleOutgoingMessages();
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error handling outgoing datagram in datagram router: %s", inputException.what());
}

}

}


/*
This function initializes each of the inproc sockets associated with the datagram router and populates the protobufObjectSocketsMap so that it can be used to simplify operations applied to all of the sockets associated with protobuf datastructures
@param inputDatagramRouter: A pointer to the object associated with the object this function is operating on

@exceptions: This function can throw exceptions
*/
datagramRouterThreadResources::datagramRouterThreadResources(datagramRouter *inputDatagramRouter)
{

associatedDatagramRouter = inputDatagramRouter;

//Create outgoing datagram inproc socket
SOM_TRY
outboundDatagramInprocSocket.reset(new zmq::socket_t(*(inputDatagramRouter->context), ZMQ_PULL));
SOM_CATCH("Error initializing outboundDatagram inproc socket for datagram router\n")

//Now bind the socket
SOM_TRY
outboundDatagramInprocSocket->bind(("inproc://" + inputDatagramRouter->sendingAddress).c_str());
SOM_CATCH("Error binding outboundDatagram inproc socket for datagram router\n")


//Create map of address string to unique_ptr references to facilitate not needing to copy the initialization code 6 times (address string to pointer to associated unique_ptr

protobufObjectSocketsMap[inputDatagramRouter->predecessorRequestAddress] = &predecessorRequestInprocSocket;
protobufObjectSocketsMap[inputDatagramRouter->predecessorRequestResponseAddress] = &predecessorRequestResponseInprocSocket;
protobufObjectSocketsMap[inputDatagramRouter->closestPredecessorRequestAddress] = &closestPredecessorRequestInprocSocket;
protobufObjectSocketsMap[inputDatagramRouter->closestPredecessorRequestResponseAddress] = &closestPredecessorRequestResponseInprocSocket;
protobufObjectSocketsMap[inputDatagramRouter->informationRequestAddress] = &informationRequestInprocSocket;
protobufObjectSocketsMap[inputDatagramRouter->informationRequestResponseAddress] = &informationRequestResponseInprocSocket;

//Initialize each of the sockets and bind them to their associated inproc addresses 
for(auto iter = protobufObjectSocketsMap.begin(); iter != protobufObjectSocketsMap.end(); iter++)
{
//Create socket to go with pointer
SOM_TRY
(*iter->second).reset(new zmq::socket_t(*(inputDatagramRouter->context), ZMQ_PUB));
SOM_CATCH(std::string("Error initializing inproc socket ") + iter->first + std::string(" for datagram router\n"))

//Now bind the socket
SOM_TRY
(*iter->second)->bind(("inproc://" + iter->first).c_str());
SOM_CATCH(std::string("Error binding inproc socket ") + iter->first + std::string(" for datagram router\n"))

}

//Populate datastructure for listening to sockets

//Poll object for the datagram socket
items[0].socket = NULL;
items[0].fd = associatedDatagramRouter->datagramSocketFileDescriptor;
items[0].events = ZMQ_POLLIN;

//Poll object for the outbound datagram inproc socket
items[1].socket = (void *) (*outboundDatagramInprocSocket);
items[1].events = ZMQ_POLLIN;

}


/*
This function waits until either an incoming datagram or an outgoing message is received.  It also sets the flags for checkForAndHandleDatagrams and checkForAndHandleOutgoingMessages to use.
*/
int datagramRouterThreadResources::waitForEvent()
{
return zmq::poll(items, 2, 1000); //Return with 0 events if nothing has happened in a millisecond
}

/*
This function checks to see if there is an incoming datagram to be processed.  If there is, it forwards it to the appropriate inproc socket

@exceptions: This function can throw exceptions
*/
void datagramRouterThreadResources::checkForAndHandleDatagrams()
{
//Check to see if a datagram was received
if(!(items[0].revents & ZMQ_POLLIN))
{
return;
}

//Create structure to store origin address
struct sockaddr_in udpSenderAddress;
unsigned int udpSenderAddressSize = sizeof(udpSenderAddress);

//Take and route datagram
int sizeOfMessage = recvfrom(associatedDatagramRouter->datagramSocketFileDescriptor, &udpMessageBuffer, UDP_MESSAGE_MAX_SIZE, 0,(struct sockaddr *) (&udpSenderAddress), &udpSenderAddressSize);

if(sizeOfMessage < 0)
{
return;
}//If something was wrong with the message or socket, ignore it

//Deserialize
//Determine if the signal is valid and its type
messageContainer message;
message.ParseFromString(std::string(udpMessageBuffer, sizeOfMessage));

if(!message.IsInitialized())
{
//Serialization failed, so ignore this message
return;
}

//Add address information to message
datagramAddress senderAddress;
senderAddress.set_ip_address(udpSenderAddress.sin_addr.s_addr);
senderAddress.set_port_number(udpSenderAddress.sin_port);

(*message.MutableExtension(datagramAddress::aDatagramAddress)) = senderAddress;

//Reserialize message
std::string reserializedMessage;
message.SerializeToString(&reserializedMessage);

std::string destinationToSendTo;

//Send to appropriate inproc port
if(message.HasExtension(predecessorRequest::aPredecessorRequest))
{
destinationToSendTo = associatedDatagramRouter->predecessorRequestAddress;
}
else if(message.HasExtension(predecessorRequestResponse::aPredecessorRequestResponse))
{
destinationToSendTo = associatedDatagramRouter->predecessorRequestResponseAddress;
}
else if(message.HasExtension(closestPredecessorRequest::aClosestPredecessorRequest))
{
destinationToSendTo = associatedDatagramRouter->closestPredecessorRequestAddress;
}
else if(message.HasExtension(closestPredecessorRequestResponse::aClosestPredecessorRequestResponse))
{
destinationToSendTo = associatedDatagramRouter->closestPredecessorRequestResponseAddress;
}
else if(message.HasExtension(informationRequest::aInformationRequest))
{
destinationToSendTo = associatedDatagramRouter->informationRequestAddress;
}
else if(message.HasExtension(informationRequestResponse::aInformationRequestResponse))
{
destinationToSendTo = associatedDatagramRouter->informationRequestResponseAddress;
}
else
{
//Invalid message
return;
}

SOM_TRY
(*protobufObjectSocketsMap[destinationToSendTo])->send(reserializedMessage.c_str(), reserializedMessage.size());
SOM_CATCH("Error sending datagram to inproc socket: " + destinationToSendTo + "\n")

}

/*
This function checks to see if there is an outgoing message.  If there is, it sends it as a datagram to the appropriate datagram address

@exceptions: This function can throw exceptions
*/
void datagramRouterThreadResources::checkForAndHandleOutgoingMessages()
{
//Check to see if a message to send as a datagram was received
if(!(items[1].revents & ZMQ_POLLIN))
{
return;
}

//Get message to send as datagram
zmq::message_t messageBuffer;
try
{
outboundDatagramInprocSocket->recv(&messageBuffer);
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error sending datagram to inproc socket\n");
}

//Deserialize
//Determine if the signal is valid and its type
messageContainer message;
message.ParseFromString(std::string((const char *) messageBuffer.data(), messageBuffer.size()));

if(!message.IsInitialized())
{
//Serialization failed, so ignore this message
return;
}

//Check if there is an address to send to
if(!message.HasExtension(datagramAddress::aDatagramAddress))
{
return;
}


struct sockaddr_in destinationAddress;
memset(&destinationAddress, 0, sizeof(destinationAddress));
destinationAddress.sin_family = AF_INET;


destinationAddress.sin_addr.s_addr = (*message.MutableExtension(datagramAddress::aDatagramAddress)).ip_address();
destinationAddress.sin_port = (*message.MutableExtension(datagramAddress::aDatagramAddress)).port_number();


//Remove the address extension from the message so it isn't sent twice
message.ClearExtension(datagramAddress::aDatagramAddress);

//Reserialize message
std::string reserializedMessage;
message.SerializeToString(&reserializedMessage);

//Send data as a datagram
int numberOfBytesSent = sendto(associatedDatagramRouter->datagramSocketFileDescriptor, reserializedMessage.c_str(), reserializedMessage.size(),0,(struct sockaddr *)(&destinationAddress), sizeof(destinationAddress));

if(numberOfBytesSent != reserializedMessage.size())
{
fprintf(stderr, "Error, unable to send datagram\n");
return;
}


}

