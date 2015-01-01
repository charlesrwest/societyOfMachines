#include "virtualPortPairSocketWrapper.hpp"

/*
This function initializes the ZMQ_PAIR socket used to communicate with the virtual port server and stores the virtual port router's address.
@param inputContext: The ZMQ context for this socket to use
@param inputVirtualPortRouterAddress: The string needed to connect to the virtual port router interface to register new virtual ports.

@exceptions: This function can throw exceptions
*/
virtualPortPairSocketWrapper::virtualPortPairSocketWrapper(zmq::context_t *inputContext, std::string inputVirtualPortRouterAddress)
{
virtualPortRouterAddress = inputVirtualPortRouterAddress;
context = inputContext;


//Make socket
SOM_TRY
pairSocket.reset(new zmq::socket_t(*inputContext, ZMQ_PAIR));
SOM_CATCH("Error creating virtual port pair socket wrapper\n")
}

/*
This function requests that the virtual port server bind a virtual port on its behalf and then connects to the ZMQ_PAIR that the virtual port router makes available.
@param inputVirtualPort: The virtual port to bind
@return: 1 if successful and 0 if the request was denied

@exceptions: This function can throw exceptions
*/
int virtualPortPairSocketWrapper::bind(uint32_t inputVirtualPort)
{
associatedVirtualPort = inputVirtualPort;

//Make requests to allocate virtual ports
virtualPortRouterRequestOrResponse request;
request.set_virtualportid(inputVirtualPort); //Ask for virtual port
request.set_operationtype(OPEN_VIRTUAL_PORT_SOCKET);

//Serialize requests
std::string serializeRequest;
request.SerializeToString(&serializeRequest);


//Make socket to request virtual port assignments
std::unique_ptr<zmq::socket_t> requestSocket;
SOM_TRY
requestSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
SOM_CATCH("Error creating request socket\n")

//Connect it to the virtual port server
SOM_TRY
requestSocket->connect((virtualPortRouterAddress).c_str());
SOM_CATCH("Error connecting to request socket of virtual port router\n")

//Send and get responses to registration requests
SOM_TRY
requestSocket->send(serializeRequest.c_str(), serializeRequest.size());
SOM_CATCH("Error connecting to request socket of virtual port router\n")

zmq::message_t messageBuffer;
SOM_TRY
requestSocket->recv(&messageBuffer);
SOM_CATCH("Error connecting to request socket of virtual port router\n")

virtualPortRouterRequestOrResponse message;
message.ParseFromArray(messageBuffer.data(), messageBuffer.size());

//Validate message
if(!message.IsInitialized() || !message.has_operationsucceeded())
{
//Serialization failed, so ignore this message
throw SOMException(std::string("Error, reply to virtual port registration request invalid\n"), INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(!message.operationsucceeded() || !message.has_inprocvirtualportaddress())
{
throw SOMException(std::string("Error, reply to virtual port registration request invalid\n"), INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

//Connect to the provided pair address
pairSocketAddress = message.inprocvirtualportaddress();
SOM_TRY
pairSocket->connect((message.inprocvirtualportaddress()).c_str());
SOM_CATCH("Error connecting to pair socket of virtual port router\n")

return 1;
}


/*
This function provides a transparent wrapper for the socket's setsockopt function (merely passing arguments).
@param inputOption: The option type
@param inputOptionValue: The value to set the option to
@param inputOptionValueLength: The length in bytes of the option value

@exceptions: This function can throw exceptions
*/
void virtualPortPairSocketWrapper::setsockopt(int inputOption, const void *inputOptionValue, size_t inputOptionValueLength)
{
SOM_TRY
pairSocket->setsockopt(inputOption, inputOptionValue, inputOptionValueLength);
SOM_CATCH("Error setting sock opt\n")
}

/*
This function allows the sender to send a datagram to a virtual port on another machine.  It abstracts the details of packaging the message in the required format and sending over the pair connection.
@param inputData: The data to send as a datagram with 
@param inputDataLength: the length of the data to send
@param inputDestinationIPAddress: The uint32_t (host format) that designates the destination IP address
@param inputDestinationPort: The port number that the destination virtual port can be found at
@param inputDestinationVirtualPort: The virtual port the message is bound for
@param inputTransactionID: An ID to be used to label responses to this message
@param inputFlags: Flags for the socket operation (such as don't wait)
@return: 1 if successful and 0 otherwise

@exceptions: This function can throw exceptions
*/
int virtualPortPairSocketWrapper::send(const void *inputData, size_t inputDataLength, uint32_t inputDestinationIPAddress, uint16_t inputDestinationPort, uint32_t inputDestinationVirtualPort, uint64_t inputTransactionID, int inputFlags)
{
virtualPortMessageHeader messageHeader;
messageHeader.set_udpip(inputDestinationIPAddress);
messageHeader.set_udpportnumber(inputDestinationPort);
messageHeader.set_destinationvirtualportid(inputDestinationVirtualPort);
messageHeader.set_transactionid(inputTransactionID);

std::string serializedMessageHeader;
messageHeader.SerializeToString(&serializedMessageHeader);

uint32_t messageHeaderSize = htonl(serializedMessageHeader.size());

std::string packagedMessage((const char *) inputData, inputDataLength);

packagedMessage += serializedMessageHeader + std::string((const char *) &messageHeaderSize, sizeof(messageHeaderSize));

//Send request
SOM_TRY
if(pairSocket->send(packagedMessage.c_str(), packagedMessage.size()) <= 0)
{
return 0;
}
SOM_CATCH("Error sending message\n")

return 1; //Everything worked
}

/*
This function wraps the recv operation on the pair socket and seperates the data from the (tail) header.
@param inputMessage: The ZMQ message (including the (tail) header)
@param inputMessageHeaderBuffer: A buffer to place the message's (tail) header in
@param inputFlags: Flags to be passed to the ZMQ socket for this operation
@return: The number of bytes of the message object that are content rather than header

@exceptions: This function can throw exceptions 
*/
int virtualPortPairSocketWrapper::recv(zmq::message_t &inputMessage, virtualPortMessageHeader &inputMessageHeaderBuffer, int inputFlags)
{
//Try to get reply
SOM_TRY
pairSocket->recv(&inputMessage, inputFlags);
SOM_CATCH("Error receiving ZMQ message\n")

if(getMessageHeaderFromVirtualPortMessage((char *)  inputMessage.data(), inputMessage.size(), inputMessageHeaderBuffer) != 1)
{
throw SOMException(std::string("Message to get header from is invalid\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

SOM_TRY
return getMessageContentSizeFromVirtualPortMessage((const char *) inputMessage.data(), inputMessage.size());
SOM_CATCH("Error, message to get header size from is invalid\n")
}

/*
This function tells the virtual port router that it is shutting down and the virtual port it uses should be deallocated.
*/
virtualPortPairSocketWrapper::~virtualPortPairSocketWrapper()
{
//Make requests to allocate virtual ports
virtualPortRouterRequestOrResponse request;
request.set_virtualportid(associatedVirtualPort); //Ask for virtual port
request.set_operationtype(CLOSE_VIRTUAL_PORT_SOCKET);

//Serialize requests
std::string serializeRequest;
request.SerializeToString(&serializeRequest);

//Make socket to request virtual port assignments
std::unique_ptr<zmq::socket_t> requestSocket;

try
{
requestSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error creating requestSocket to deregister virtual port socket\n%s\n", inputException.what());
} 

//Connect it to the virtual port server
try
{
requestSocket->connect((virtualPortRouterAddress).c_str());
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error connecting to request socket of virtual port router to deregister virtual port socket\n%s\n", inputException.what());
} 

//Send and get responses to registration requests
try
{
requestSocket->send(serializeRequest.c_str(), serializeRequest.size());
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error sending to request socket of virtual port router to deregister virtual port socket\n%s\n", inputException.what());
}

zmq::message_t messageBuffer;
try
{
requestSocket->recv(&messageBuffer);
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Error receiving from request socket of virtual port router to deregister virtual port socket\n%s\n", inputException.what());
}

virtualPortRouterRequestOrResponse message;
message.ParseFromArray(messageBuffer.data(), messageBuffer.size());

//Validate message
if(!message.IsInitialized() || !message.has_operationsucceeded())
{
//Serialization failed, so ignore this message
fprintf(stderr, "Error, reply to virtual port registration request invalid\n");
return;
}

}
