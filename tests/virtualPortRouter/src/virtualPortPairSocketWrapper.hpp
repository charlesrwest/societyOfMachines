#ifndef VIRTUALPORTPAIRSOCKETWRAPPERHPP
#define VIRTUALPORTPAIRSOCKETWRAPPERHPP

#include<zmq.hpp>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

#include "virtualPortRouterRequestOrResponse.pb.h"
#include "virtualPortMessageHeader.pb.h"

#include "virtualPortRouter.hpp"

/*
This class is meant to simplify the process of connecting with a virtual port router and sending compatable messages.  The ZMQ_PAIR socket that is used is available for use in operations like polling.  It also removes the associated virtual port listing when it is terminated.
*/
class virtualPortPairSocketWrapper
{
public:
/*
This function initializes the ZMQ_PAIR socket used to communicate with the virtual port server and stores the virtual port router's address.
@param inputContext: The ZMQ context for this socket to use
@param inputVirtualPortRouterAddress: The string needed to connect to the virtual port router interface to register new virtual ports.

@exceptions: This function can throw exceptions
*/
virtualPortPairSocketWrapper(zmq::context_t *inputContext, std::string inputVirtualPortRouterAddress);

/*
This function requests that the virtual port server bind a virtual port on its behalf and then connects to the ZMQ_PAIR that the virtual port router makes available.
@param inputVirtualPort: The virtual port to bind
@return: 1 if successful and 0 if the request was denied

@exceptions: This function can throw exceptions
*/
int bind(uint32_t inputVirtualPort);

/*
This function provides a transparent wrapper for the socket's setsockopt function (merely passing arguments).
@param inputOption: The option type
@param inputOptionValue: The value to set the option to
@param inputOptionValueLength: The length in bytes of the option value

@exceptions: This function can throw exceptions
*/
void setsockopt(int inputOption, const void *inputOptionValue, size_t inputOptionValueLength);

/*
This function allows the sender to send a datagram to a virtual port on another machine.  It abstracts the details of packaging the message in the required format and sending over the pair connection.
@param inputData: The data to send as a datagram
@param inputDataLength: the length of the data to send
@param inputDestinationIPAddress: The uint32_t (host format) that designates the destination IP address
@param inputDestinationPort: The port number that the destination virtual port can be found at
@param inputDestinationVirtualPort: The virtual port the message is bound for
@param inputTransactionID: An ID to be used to label responses to this message
@param inputFlags: Flags for the socket operation (such as don't wait)
@return: 1 if successful and 0 otherwise

@exceptions: This function can throw exceptions
*/
int send(const void *inputData, size_t inputDataLength, uint32_t inputDestinationIPAddress, uint16_t inputDestinationPort, uint32_t inputDestinationVirtualPort, uint64_t inputTransactionID, int inputFlags = 0);

/*
This function wraps the recv operation on the pair socket and seperates the data from the (tail) header.
@param inputMessage: The ZMQ message (including the (tail) header)
@param inputMessageHeaderBuffer: A buffer to place the message's (tail) header in
@param inputFlags: Flags to be passed to the ZMQ socket for this operation
@return: The number of bytes of the message object that are content rather than header 

@exceptions: This function can throw exceptions 
*/
int recv(zmq::message_t &inputMessage, virtualPortMessageHeader &inputMessageHeaderBuffer, int inputFlags = 0);

/*
This function tells the virtual port router that it is shutting down and the virtual port it uses should be deallocated.
*/
~virtualPortPairSocketWrapper();

std::unique_ptr<zmq::socket_t> pairSocket; //The ZMQ_PAIR socket used to talk with the virtual port router
zmq::context_t *context;
std::string virtualPortRouterAddress;  //The router address
std::string pairSocketAddress;         //The address of the pair socket this object connected to
uint32_t associatedVirtualPort; //The virtual port number this object has registered
};











#endif
