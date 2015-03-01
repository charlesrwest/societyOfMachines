#include "virtualPortRouter.hpp"

/*
This function initializes the object and starts a thread that conducts the operations of the router.  It does not return until all initialization in the thread is completed and all of the public members are safe to read.

@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
virtualPortRouter::virtualPortRouter(zmq::context_t *inputZMQContext)
{
context = inputZMQContext;
timeToShutdownFlag = false;

//Create and bind datagram socket
SOM_TRY
initializeUDPSocket();
SOM_CATCH("Error initializing udp port for virtual port router\n")

//Create virtual port router base address
SOM_TRY
virtualPortRouterExtension = generateRouterExtension();
SOM_CATCH("Error generating router extension");

virtualPortRouterInprocAddress = "inproc://virtualPortRouter" + virtualPortRouterExtension;

//Start the router thread and make sure it initialized correctly
std::promise<bool> virtualPortRouterThreadInitializationResult;

//Start a thread and pass it a this pointer
virtualPortRouterThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunVirtualPortRouterThread, this, &virtualPortRouterThreadInitializationResult));

//This should block until the datagram receiving thread has been initialized and throw any exceptions that the router server thread threw during its initialization
if(virtualPortRouterThreadInitializationResult.get_future().get() == true)
{
   //printf("Thread reports that it initialized successfully\n");
}

}

/*
This function signals for the threads to shut down and then waits for it to do so.
*/
virtualPortRouter::~virtualPortRouter()
{
timeToShutdownFlag = true;
close(datagramSocketFileDescriptor);  //Close file descriptor to clean up
virtualPortRouterThread->join();
}

/*
This function initializes the UDP socket associated with the virtual port router and assigns the file descriptor number to datagramPortNumber.
@exceptions: This function can throw exceptions
*/
void virtualPortRouter::initializeUDPSocket()
{
//Create descriptor
datagramSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM,0);
if(datagramSocketFileDescriptor <= 0)
{
throw SOMException(std::string("Error initializing datagram socket for virtual port router\n"), SYSTEM_ERROR, __FILE__, __LINE__);
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
datagramSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
datagramSocketAddress.sin_port = htons(portToTry); 

if(bind(datagramSocketFileDescriptor, (struct sockaddr *) &datagramSocketAddress, sizeof(datagramSocketAddress)) == 0)
{
datagramPortNumber = portToTry;
break;
}

}

}

/*
This function generates a 10 ascii character router extension string 
@exceptions: This function can throw exceptions
*/
std::string virtualPortRouter::generateRouterExtension()
{
std::string lookupTable("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

std::string routerExtension;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> stringGenerationDistribution(0, lookupTable.size()-1);

//Generate 10 random characters
for(int i=0; i<10; i++)
{
routerExtension.push_back(lookupTable[stringGenerationDistribution(gen)]);
}

return routerExtension;
}


/*
This function is run as a seperate thread and implements the virtual port router behavior
@param inputVirtualPortRouter: A pointer to the object associated with the object this function is operating on
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process)
*/
void initializeAndRunVirtualPortRouterThread(virtualPortRouter *inputVirtualPortRouter, std::promise<bool> *inputPromise)
{
//Allocate associated resources
std::unique_ptr<virtualPortRouterResources> routerResources;
try
{
routerResources.reset(new virtualPortRouterResources(inputVirtualPortRouter));
}
catch(const std::exception &inputException)
{
inputPromise->set_exception(std::current_exception());
return;
}

//Tell constructor that initialization completed successfully
inputPromise->set_value(true); //Initialization completed successfully

//Listen and process messages
try
{
routerResources->listenForAndProcessMessages();
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
return;
}
}

/*
This function initializes the resources associated with the router thread.
@param inputVirtualPortRouter: This is a pointer to the virtual port router associated with this object.
@exceptions: This function can throw exceptions
*/
virtualPortRouterResources::virtualPortRouterResources(virtualPortRouter *inputVirtualPortRouter)
{
parentVirtualPortRouter = inputVirtualPortRouter;
//Initialize socket to allow requests to bind/unbind virtual ports
//Create outgoing datagram inproc socket
SOM_TRY
virtualPortServer.reset(new zmq::socket_t(*(parentVirtualPortRouter->context), ZMQ_REP));
SOM_CATCH("Error initializing virtual port setup inproc socket for virtual port router\n")

//Now bind the socket
SOM_TRY
virtualPortServer->bind((parentVirtualPortRouter->virtualPortRouterInprocAddress).c_str());
SOM_CATCH("Error binding virtual port setup inproc socket for virtual port router\n")

}

/*
This function listens on the UDP port and all of the inproc ports for activity.  If a request, message to be sent or datagram comes it, it call the appropriate function(s).  It operates in an infinite loop that will only terminate if the parent virtualPortRouter object indicates that it is time to shut down or an exception occurs.
@exceptions: This function can throw exceptions
*/
void virtualPortRouterResources::listenForAndProcessMessages()
{
while(true)
{

if(parentVirtualPortRouter->timeToShutdownFlag == true)
{
break; //Time to shutdown
}



//Construct an poll object list to represent all of the ports to listen on
//allocate memory for list
int numberOfPollObjects = 2 + portIDToInprocMap.size(); //UDP, virtual port allocator, all of the PAIR ports
std::unique_ptr<zmq::pollitem_t[]> pollItems(new zmq::pollitem_t[numberOfPollObjects]);
std::vector<int> pollItemLocationToVirtualPortNumber(numberOfPollObjects); //Create easy way to go from poll item location to virtualPortID

//UDP
pollItems[0] = {nullptr, parentVirtualPortRouter->datagramSocketFileDescriptor, ZMQ_POLLIN, 0 };
pollItemLocationToVirtualPortNumber[0] = -1;

//Virtual port creation/destruction socket
pollItems[1] = {(void *) (*virtualPortServer), 0, ZMQ_POLLIN, 0 };
pollItemLocationToVirtualPortNumber[1] = -1;


//Pair sockets
int index = 2;
for(auto iter = portIDToInprocMap.begin(); iter != portIDToInprocMap.end(); iter++)
{

pollItems[index].socket = (void *) (*iter->second);
pollItems[index].fd = 0;
pollItems[index].events = ZMQ_POLLIN;
pollItems[index].revents = 0;

pollItemLocationToVirtualPortNumber[index] = iter->first;
index++;
}

//Listen for activity for 100 milliseconds TODO
SOM_TRY
if(zmq::poll(pollItems.get(), numberOfPollObjects, 100) == 0)
//if(zmq::poll(pollItems.get(), 2) == 0)
{
continue;
}
SOM_CATCH("Error occured listening for activity on ports\n")

//Process possible UDP message
if(pollItems[0].revents & ZMQ_POLLIN)
{
SOM_TRY
processIncomingUDPMessage();
SOM_CATCH("Error processing UDP packet in virtual port router\n")
}

//Process possible request to remove or add a ZMQ_PAIR
if(pollItems[1].revents & ZMQ_POLLIN)
{
SOM_TRY
processAddOrRemoveVirtualPortMessage();
SOM_CATCH("Error processing add/remove virtual port message\n")
}



//Process possible outgoing messages
for(int i=2; i<pollItemLocationToVirtualPortNumber.size(); i++)
{ 
if(pollItems[i].revents & ZMQ_POLLIN)
{
//Call process outgoing message function
SOM_TRY
processOutgoingUDPMessage(pollItemLocationToVirtualPortNumber[i]);
SOM_CATCH("Error processing outgoing message\n")
}
}

}


}


/*
This function performs a nonblocking read on the datagram socket, deserializes the header and sends it to the appropriate inproc socket after updating the header.
@exceptions: This function can throw exceptions
*/
void virtualPortRouterResources::processIncomingUDPMessage()
{

//Create structure to store origin address
struct sockaddr_in udpSenderAddress;
unsigned int udpSenderAddressSize = sizeof(udpSenderAddress);

//Take and route datagram
int sizeOfMessage = recvfrom(parentVirtualPortRouter->datagramSocketFileDescriptor, &udpMessageBuffer, UDP_MESSAGE_MAX_SIZE, 0,(struct sockaddr *) (&udpSenderAddress), &udpSenderAddressSize);
if(sizeOfMessage < 0)
{
return; //Error occurred
}

virtualPortMessageHeader messageHeader;
if(getMessageHeaderFromVirtualPortMessage(udpMessageBuffer, sizeOfMessage, messageHeader) != 1)
{
return;  //Invalid message
}

if(messageHeader.has_destinationvirtualportid())
{
if(portIDToInprocMap.count(messageHeader.destinationvirtualportid()) == 0)
{
return; //That virtual port number hasn't been registered, so message is invalid
}
}
else
{
return; //No destination virtual port id, so message is invalid
}

//Set IP and port from the information in the UDP datagram
messageHeader.set_udpip(ntohl(udpSenderAddress.sin_addr.s_addr));
messageHeader.set_udpportnumber(ntohs(udpSenderAddress.sin_port));

//Forward message if the resulting size is not too large
int finalCompleteMessageSize = 0;
SOM_TRY
finalCompleteMessageSize = setMessageHeaderInVirtualPortMessage(udpMessageBuffer, sizeOfMessage, UDP_MESSAGE_MAX_SIZE + EXTRA_SPACE_FOR_MESSAGE_HEADER_EXPANSION, messageHeader);
if(finalCompleteMessageSize == 0)
{
return; //Couldn't add the revised header, so abort
}
SOM_CATCH("Error, buffer for message with message header is too small\n")

//Forward the message
SOM_TRY
portIDToInprocMap[messageHeader.destinationvirtualportid()]->send(udpMessageBuffer, finalCompleteMessageSize);
SOM_CATCH("Error sending datagram to inproc socket with virtual port number " + std::to_string(messageHeader.destinationvirtualportid()) + "\n")
}

/*
This function performs a nonblocking read on the socket associated with a particular virtual port, deserializes the header and sends as a datagram to the appropriate IP/port after updating the header.
@param inputVirtualPortNumber: The virtual port associated with the inproc socket the message is to be found on

@exceptions: This function can throw exceptions
*/
void virtualPortRouterResources::processOutgoingUDPMessage(uint32_t inputVirtualPortNumber)
{
//Validate input
if(portIDToInprocMap.count(inputVirtualPortNumber) == 0)
{
throw SOMException(std::string("Told to get message from invalid virtual port number\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Retrieve the message
int receivedMessageSize = 0;
SOM_TRY
receivedMessageSize = portIDToInprocMap[inputVirtualPortNumber]->recv(&udpMessageBuffer, UDP_MESSAGE_MAX_SIZE, ZMQ_NOBLOCK);
if(receivedMessageSize == 0)
{
return; //Do a nonblocking read of the socket and return immediately if there isn't a request waiting or we get a zero length request
}
SOM_CATCH("Error receiving inproc message to forward\n")


virtualPortMessageHeader messageHeader;
if(getMessageHeaderFromVirtualPortMessage(udpMessageBuffer, receivedMessageSize, messageHeader) != 1) //Extract message header
{
return;  //Invalid message
}

//Validate message
if(!messageHeader.has_destinationvirtualportid() || !messageHeader.has_udpportnumber() || !messageHeader.has_udpip())
{
return;
}



//Copy out IP and port to send to
uint32_t IPToSendTo = messageHeader.udpip();
uint16_t UDPPortToSendTo = messageHeader.udpportnumber();

//Clear out unnessisary information from the header 
messageHeader.clear_udpip();
messageHeader.clear_udpportnumber();



//Mark the virtual port that the message originated from 
messageHeader.set_sendervirtualportid(inputVirtualPortNumber);


//Forward message if the resulting size is not too large 
int finalCompleteMessageSize = 0;
SOM_TRY
finalCompleteMessageSize = setMessageHeaderInVirtualPortMessage(udpMessageBuffer, receivedMessageSize, UDP_MESSAGE_MAX_SIZE + EXTRA_SPACE_FOR_MESSAGE_HEADER_EXPANSION, messageHeader);
if(finalCompleteMessageSize == 0)
{
return; //Couldn't add the revised header, so abort
}
SOM_CATCH("Error, buffer for message with message header is too small\n")

virtualPortMessageHeader messageHeaderBuffer3;
if(getMessageHeaderFromVirtualPortMessage(udpMessageBuffer, finalCompleteMessageSize, messageHeaderBuffer3) != 1)
{
fprintf(stderr, "Error retrieving message header\n");
}

//Forward the message

struct sockaddr_in targetAddress;

memset(&targetAddress, 0, sizeof(targetAddress));
targetAddress.sin_family = AF_INET;
targetAddress.sin_port = htons(UDPPortToSendTo);
targetAddress.sin_addr.s_addr=htonl(IPToSendTo);



if(sendto(parentVirtualPortRouter->datagramSocketFileDescriptor, udpMessageBuffer, finalCompleteMessageSize, 0, (struct sockaddr *) &targetAddress, sizeof(targetAddress)) < finalCompleteMessageSize)
{
throw SOMException(std::string("Error sending UDP message\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}

}






/*
This function performs a nonblocking read on the virtualPortServer socket, deserializes the message, performs the requested operation (if possible) and replies with the success/failure.
@exceptions: This function can throw exceptions
*/
void virtualPortRouterResources::processAddOrRemoveVirtualPortMessage()
{
//Get message defining virtual port operation
zmq::message_t messageBuffer;

SOM_TRY
if(virtualPortServer->recv(&messageBuffer) != true)
{
return; //Do a nonblocking read of the socket and return immediately if there isn't a request waiting
}
SOM_CATCH("Error receiving virtual port reply\n")


//Deserialize
//Determine if the signal is valid and its type
virtualPortRouterRequestOrResponse message;
message.ParseFromArray(messageBuffer.data(), messageBuffer.size());

//Validate message
if(!message.IsInitialized() || !message.has_virtualportid() || !message.has_operationtype())
{
//Serialization failed, so ignore this message
return;
}



switch(message.operationtype())
{
case OPEN_VIRTUAL_PORT_SOCKET: //Try to allocate virtual port
{
std::string virtualPortInprocAddress;
SOM_TRY
virtualPortInprocAddress = allocateVirtualPort(message.virtualportid());
SOM_CATCH("Error opening virtual port\n")


if(virtualPortInprocAddress == "")
{
SOM_TRY
sendVirtualPortOperationReplyMessage(false); //Operation failed, so tell other side
SOM_CATCH("Error sending operation failed message\n")
}
else
{
SOM_TRY
sendVirtualPortOperationReplyMessage(true, virtualPortInprocAddress); //Operation succeeded, so tell the other side
SOM_CATCH("Error sending virtual port open succeeded message\n")
}
}
break;

case CLOSE_VIRTUAL_PORT_SOCKET:
if(removeVirtualPort(message.virtualportid()) == 0)
{
SOM_TRY
sendVirtualPortOperationReplyMessage(false);
SOM_CATCH("Error sending operation failed message\n")
}
else
{
SOM_TRY
sendVirtualPortOperationReplyMessage(true);
SOM_CATCH("Error sending remove virtual port message succeeded message\n")
}

break;

default:
return; //Invalid option value, so ignore message
break;
}

}


/*
This function attempts to allocate the resources associated with a virtual port router.
@param inputVirtualPort: The port to allocate
@return: "virtualPortRouterInprocAddress:virtualPortNumber" or empty on if the port has already been allocated
*/
std::string virtualPortRouterResources::allocateVirtualPort(uint32_t inputVirtualPort)
{
//Check if the virtual port number has already been allocated
if(portIDToInprocMap.count(inputVirtualPort) > 0)
{
return ""; //It has already been allocated
}

//Create socket and bind it to entries
std::unique_ptr<zmq::socket_t> newVirtualPort;

//Create address string to bind to
std::string virtualPortString = parentVirtualPortRouter->virtualPortRouterInprocAddress + ":" + std::to_string(inputVirtualPort);

//Initialize socket
SOM_TRY
newVirtualPort.reset(new zmq::socket_t(*(parentVirtualPortRouter->context), ZMQ_PAIR));
SOM_CATCH("Error initializing a exclusive pair socket for a new virtual port\n")

//Now bind the socket
SOM_TRY
newVirtualPort->bind(virtualPortString.c_str());
SOM_CATCH("Error binding socket for new virtual port\n")


//Add to maps
portIDToInprocMap[inputVirtualPort] = std::move(newVirtualPort); //Transfer ownership to container pointer

InprocToPortIDMap[portIDToInprocMap[inputVirtualPort].get()] = inputVirtualPort;


return virtualPortString;
}

/*
This function attempts to remove the resources associated with a virtual port router.
@param inputVirtualPort: The port to allocate
@return: 1 if successful and 0 if the port isn't allocated
*/
int virtualPortRouterResources::removeVirtualPort(uint32_t inputVirtualPort)
{
if(portIDToInprocMap.count(inputVirtualPort) == 0)
{
return 0; //The port has not been allocated
}

//Erase associated port/records
InprocToPortIDMap.erase(portIDToInprocMap[inputVirtualPort].get());
portIDToInprocMap.erase(inputVirtualPort);

return 1;
}

/*
Sends a reply indicating the result of a virtual port add/remove operation.
@param inputOperationSucceeded: True if the operation succeeded and false otherwise
@param inputInprocAddress: The the created port's inproc address (if applicable).  Empty is assumed to mean there isn't one 

@exceptions: This function can throw exceptions
*/
void virtualPortRouterResources::sendVirtualPortOperationReplyMessage(bool inputOperationSucceeded, std::string inputInprocAddress)
{
//Form operation indicating message
virtualPortRouterRequestOrResponse operationStateMessage;

operationStateMessage.set_operationsucceeded(inputOperationSucceeded);

if(inputInprocAddress.size() > 0)
{
operationStateMessage.set_inprocvirtualportaddress(inputInprocAddress);
}

//Serialize message
std::string serializedMessage;
operationStateMessage.SerializeToString(&serializedMessage);



//Send message
SOM_TRY
virtualPortServer->send(serializedMessage.c_str(), serializedMessage.size());
SOM_CATCH("Error sending virtual port opening/closing success/failure message to requester\n")
}



/*
This function attempts to extract and deserialize the message header of a virtualPortRouter message (message, messageHeader, messageHeaderSizeInNetworkByteOrder).
@param inputVirtualPortMessageData: The message data
@param inputVirtualPortMessageSize: The length of the data
@param inputVirtualPortMessageHeader: The message header object to store the result in
@return: 1 if successful and 0 otherwise
*/
int getMessageHeaderFromVirtualPortMessage(char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize, virtualPortMessageHeader &inputVirtualPortMessageHeader)
{
if(inputVirtualPortMessageSize < 4) //Must have at least 32 bits for message header size
{
return 0;
}//If something was wrong with the message, fail


//Packet format: message messageHeader messageHeaderSize
//Retrieve message header size and convert it to host byte order
int messageHeaderSizeOffset = inputVirtualPortMessageSize-sizeof(uint32_t);
uint32_t messageHeaderSize = ntohl(*((uint32_t*) &inputVirtualPortMessageData[messageHeaderSizeOffset])); 


//Deserialize messageHeader
int messageHeaderOffset = messageHeaderSizeOffset - messageHeaderSize;
if(messageHeaderOffset < 0)
{
return 0; //Bad message size value
}

char *pointerToMessageHeader = &inputVirtualPortMessageData[messageHeaderOffset]; 
inputVirtualPortMessageHeader.ParseFromArray(pointerToMessageHeader, messageHeaderSize);

if(!inputVirtualPortMessageHeader.IsInitialized())
{
return 0; //Message header serialization failed, so ignore this message
}

return 1;
}

/*
This function retrieves how much of the virtual port message is the content from the virtual port message.
@param inputVirtualPortMessageData: The message data
@param inputVirtualPortMessageSize: The length of the data
@return: The length of the actual message (rather than the header tail)

@exceptions: This function can throw exceptions
*/
int getMessageContentSizeFromVirtualPortMessage(const char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize)
{
int messageHeaderSizeOffset = inputVirtualPortMessageSize-sizeof(uint32_t);

if(messageHeaderSizeOffset < 0)
{
throw SOMException(std::string("Message to get size from is invalid\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

uint32_t messageHeaderSize = ntohl(*((uint32_t*) &inputVirtualPortMessageData[messageHeaderSizeOffset])); 

return inputVirtualPortMessageSize - ((int) messageHeaderSize)-sizeof(uint32_t);
}


/*
This function attempts to insert a messageHeader into a virtualPortRouter message (message, messageHeader, messageHeaderSizeInNetworkByteOrder).
@param inputVirtualPortMessageData: The message data (where the header will be replaced)
@param inputVirtualPortMessageSize: The length of the original data
@param inputVirtualPortMessageSize: The maximum size of the data after the new header has replaced the old one
@param inputVirtualPortMessageHeader: The message header object to replace the old one with
@return: The size of the complete message or 0 on failure
@exceptions: This function can throw exceptions
*/
int setMessageHeaderInVirtualPortMessage(char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize, uint32_t inputVirtualPortMaxMessageSize, const virtualPortMessageHeader &inputVirtualPortMessageHeader)
{
if(inputVirtualPortMessageSize < 4 || inputVirtualPortMaxMessageSize < inputVirtualPortMessageSize) //Must have at least 32 bits for message header size
{
return 0;
}//If something was wrong with the message, fail

//Packet format: message messageHeader messageHeaderSize
//Retrieve message header size and convert it to host byte order
int messageHeaderSizeOffset = inputVirtualPortMessageSize-sizeof(uint32_t);
uint32_t messageHeaderSize = ntohl(*((uint32_t*) &inputVirtualPortMessageData[messageHeaderSizeOffset])); 

int serializedMessageContentSize = ((int) inputVirtualPortMessageSize) - ((int) messageHeaderSize) - sizeof(uint32_t);

if(serializedMessageContentSize < 0)
{
return 0; //Bad message header size
}

int messageHeaderOffset = messageHeaderSizeOffset - messageHeaderSize;
if(messageHeaderOffset < 0)
{
return 0; //Bad message size value
}


char *pointerToMessageHeader = inputVirtualPortMessageData +messageHeaderOffset; 


//Forward message if the resulting size is not too large
int finalSerializedMessageTotalSize = inputVirtualPortMessageHeader.ByteSize() + sizeof(uint32_t) + serializedMessageContentSize;
if(finalSerializedMessageTotalSize > inputVirtualPortMaxMessageSize)
{
throw SOMException(std::string("Required buffer to forward UDP datagram to inproc socket is too large\n"), BUFFER_OVERFLOW, __FILE__, __LINE__);
return 0; //Can't fit message
}


for(int i=0; i<messageHeaderSize; i++)
{
pointerToMessageHeader[i] = 0xFF;
}

//Append the revised message header
inputVirtualPortMessageHeader.SerializeToArray(pointerToMessageHeader, inputVirtualPortMaxMessageSize-serializedMessageContentSize-sizeof(uint32_t)); 

//Append the revised header size
*((uint32_t *) (pointerToMessageHeader+inputVirtualPortMessageHeader.ByteSize())) = htonl(inputVirtualPortMessageHeader.ByteSize());


return finalSerializedMessageTotalSize;
}


