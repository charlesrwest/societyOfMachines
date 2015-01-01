#ifndef VIRTUALPORTROUTERHPP
#define VIRTUALPORTROUTERHPP

#include<cstdio>
#include<future>
#include<thread>
#include<string>
#include<random>

#include<map>
#include<cstdlib>
#include<cmath>
#include<unistd.h>

#include<zmq.hpp>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

#include "virtualPortRouterRequestOrResponse.pb.h"
#include "virtualPortMessageHeader.pb.h"

#define UDP_MESSAGE_MAX_SIZE 1500
#define EXTRA_SPACE_FOR_MESSAGE_HEADER_EXPANSION 20
//#define MAXIMUM_HEADER_LENGTH 40

/*
This class allows several threads to share a single datagram port productively.  It accomplishes this by claiming the port and opening ZMQ exclusive pair inproc sockets for the other threads to access.  Each exclusive pair socket has a locally unique integer associated with it (which can be specified in the request to form the pair, if needed).  Each datagram received by the router is suppose to have a virtual port number associated with it.  This enables a functionality similar to the traditional UDP/TCP ports where well known services bind a specific port and requests can bind a random one.

In addition to the virtual port ID, each message header can also have an optional transactionID associated with it.  The router doesn't do anything with the transactionID, but it can be used by the requester/server to allow a thread sending multiple requests to keep track of which reply corresponds to which.

The actual format of the messages should be as follows:
(message) (a libprotobuf messageHeader object) (network order 32 bit unsigned int indicating messageHeaderSize)  

The message header is deserialized and then the serialized message (including the message header) is sent forward without modification to the associated exclusive pair.  If no virtual port matches, the message is silently dropped.

A new virtual port is opened by sending a ZMQ_REQ request using a "virtualPortRouterRequestOrResponse" to the ZMQ_REP port bound by the virtualPortRouter at the value held in member variable "virtualPortRouterInprocAddress".  The address to use to bind to the exlusive pair is sent in the response (if the request was successful) and is typically of the format: virtualPortRouterInprocAddress:virtualPortNumber

*/
class virtualPortRouter
{
public:
/*
This function initializes the object and starts a thread that conducts the operations of the router.  It does not return until all initialization in the thread is completed and all of the public members are safe to read.

@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
virtualPortRouter(zmq::context_t *inputZMQContext);

/*
This function signals for the threads to shut down and then waits for it to do so.
*/
~virtualPortRouter();

/*
This function is run as a seperate thread and implements the virtual port router behavior
@param inputVirtualPortRouter: A pointer to the object associated with the object this function is operating on
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
friend void initializeAndRunVirtualPortRouterThread(virtualPortRouter *inputVirtualPortRouter, std::promise<bool> *inputPromise);

zmq::context_t *context;

std::string virtualPortRouterExtension;  //The random string unique to this router
std::string virtualPortRouterInprocAddress;
uint32_t datagramPortNumber;
bool timeToShutdownFlag;  //True if the created threads should shutdown
std::unique_ptr<std::thread> virtualPortRouterThread;
int datagramSocketFileDescriptor;  //File descriptor associated with datagram socket

private:
/*
This function initializes the UDP socket associated with the virtual port router and assigns the file descriptor number to datagramPortNumber.
@exceptions: This function can throw exceptions
*/
void initializeUDPSocket();

/*
This function generates a 10 ascii character router extension string 
@exceptions: This function can throw exceptions
*/
std::string generateRouterExtension();

};

/*
This function is run as a seperate thread and implements the virtual port router behavior
@param inputVirtualPortRouter: A pointer to the object associated with the object this function is operating on
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process)
*/
void initializeAndRunVirtualPortRouterThread(virtualPortRouter *inputVirtualPortRouter, std::promise<bool> *inputPromise);

/*
This class represents the resources used in the virtual port router thread.  Since ZMQ sockets don't appreciate being accessed across threads, this seems the best way to do it. 
*/
class virtualPortRouterResources
{
public:
/*
This function initializes the resources associated with the router thread.
@param inputVirtualPortRouter: This is a pointer to the virtual port router associated with this object.
@exceptions: This function can throw exceptions
*/
virtualPortRouterResources(virtualPortRouter *inputVirtualPortRouter);

/*
This function listens on the UDP port and all of the inproc ports for activity.  If a request, message to be sent or datagram comes it, it call the appropriate function(s).  It operates in an infinite loop that will only terminate if the parent virtualPortRouter object indicates that it is time to shut down or an exception occurs.
@exceptions: This function can throw exceptions
*/
void listenForAndProcessMessages();

/*
This function performs a nonblocking read on the datagram socket, deserializes the header and sends it to the appropriate inproc socket after updating the header.
@exceptions: This function can throw exceptions
*/
void processIncomingUDPMessage();

/*
This function performs a nonblocking read on the socket associated with a particular virtual port, deserializes the header and sends as a datagram to the appropriate IP/port after updating the header.
@param inputVirtualPortNumber: The virtual port associated with the inproc socket the message is to be found on

@exceptions: This function can throw exceptions
*/
void processOutgoingUDPMessage(uint32_t inputVirtualPortNumber);

/*
This function performs a nonblocking read on the virtualPortServer socket, deserializes the message, performs the requested operation (if possible) and replies with the success/failure.
@exceptions: This function can throw exceptions
*/
void processAddOrRemoveVirtualPortMessage();

/*
This function attempts to allocate the resources associated with a virtual port router.
@param inputVirtualPort: The port to allocate
@return: "virtualPortRouterInprocAddress:virtualPortNumber" or empty on if the port has already been allocated

@exceptions: This function can throw exceptions
*/
std::string allocateVirtualPort(uint32_t inputVirtualPort);

/*
This function attempts to remove the resources associated with a virtual port router.
@param inputVirtualPort: The port to allocate
@return: 1 if successful and 0 if the port isn't allocated
*/
int removeVirtualPort(uint32_t inputVirtualPort);

/*
Sends a reply indicating the result of a virtual port add/remove operation.
@param inputOperationSucceeded: True if the operation succeeded and false otherwise
@param inputInprocAddress: The the created port's inproc address (if applicable).  Empty is assumed to mean there isn't one 

@exceptions: This function can throw exceptions
*/
void sendVirtualPortOperationReplyMessage(bool inputOperationSucceeded, std::string inputInprocAddress = "");


virtualPortRouter *parentVirtualPortRouter;

std::unique_ptr<zmq::socket_t> virtualPortServer; //A inproc ZMQ_REP interface used to request virtual ports to be opened or closed

//A map to convert between virtual port IDs and socket IDs and vis versa
std::map<uint32_t, std::unique_ptr<zmq::socket_t> > portIDToInprocMap;
std::map<zmq::socket_t *, uint32_t > InprocToPortIDMap;
//Buffer for extracting/forwarding messages
char udpMessageBuffer[UDP_MESSAGE_MAX_SIZE + EXTRA_SPACE_FOR_MESSAGE_HEADER_EXPANSION];
};

/*
This function attempts to extract and deserialize the message header of a virtualPortRouter message (message, messageHeader, messageHeaderSizeInNetworkByteOrder).
@param inputVirtualPortMessageData: The message data
@param inputVirtualPortMessageSize: The length of the data
@param inputVirtualPortMessageHeader: The message header object to store the result in
@return: 1 if successful and 0 otherwise
*/
int getMessageHeaderFromVirtualPortMessage(char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize, virtualPortMessageHeader &inputVirtualPortMessageHeader);

/*
This function retrieves how much of the virtual port message is the content from the virtual port message.
@param inputVirtualPortMessageData: The message data
@param inputVirtualPortMessageSize: The length of the data
@return: The length of the actual message (rather than the header tail)  

@exceptions: This function can throw exceptions
*/
int getMessageContentSizeFromVirtualPortMessage(const char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize);

/*
This function attempts to insert a messageHeader into a virtualPortRouter message (message, messageHeader, messageHeaderSizeInNetworkByteOrder).
@param inputVirtualPortMessageData: The message data (where the header will be replaced)
@param inputVirtualPortMessageSize: The length of the original data
@param inputVirtualPortMaxMessageSize: The maximum size of the data after the new header has replaced the old one
@param inputVirtualPortMessageHeader: The message header object to replace the old one with
@return: The size of the complete message or 0 on failure
@exceptions: This function can throw exceptions
*/
int setMessageHeaderInVirtualPortMessage(char *inputVirtualPortMessageData, uint32_t inputVirtualPortMessageSize, uint32_t inputVirtualPortMaxMessageSize, const virtualPortMessageHeader &inputVirtualPortMessageHeader);



#endif
