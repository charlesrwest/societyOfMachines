#ifndef DATAGRAMROUTERHPP
#define DATAGRAMROUTERHPP

#include<zmq.hpp>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#include<future>
#include <functional>

#include "SOMException.hpp"

/*
This class binds a datagram socket somewhere in the range of 1024 to 49151.  It provides a simple threadsafe interface for sending datagrams since sendto is threadsafe.  It then spawns a thread that listens to (receives from) the datagram socket.  It attempts to deserialize any messages that are received and routes the received messages to inproc pair sockets according to their message type.  The messages are bound to pair addresses in the format: "datagramRouter:routerThreadID:messageTypeName" 
*/
class datagramRouter
{
public:
/*
This function constructs the object and spawns the associated thread.
@param inputZMQContext: The ZMQ context to use for inproc communications.  If specified as NULL, the datagram router makes its own context
@exceptions: This object can throw exceptions
*/
datagramRouter(zmq::context_t *inputZMQContext = NULL);

/*
The sendto interface for the datagram socket associated with the datagram router.  This function is threadsafe.
@param inputMessage: The data to send
@param inputMessageLength: The number of bytes to send
@param inputFlags: UDP flags
@param inputDestinationAddress: A pointer to the destination address
@param inputDestinationAddressLength: How long the destination address structure is
@return: On local success, returns the number of bytes sent.  -1 is returned otherwise
*/
ssize_t sendDatagram(const void *inputMessage, size_t inputMessageLength, int inputFlags, const struct sockaddr *inputDestinationAddress, socklen_t inputDestinationAddressLength);

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
~datagramRouter();

zmq::context_t *context;  //A pointer to the ZMQ context that the datagram router is using.  This is the only way the context should be accessed by outside objects using the datagram router.
uint32_t portNumber; //The port number of the associated datagram socket
std::string baseInprocAddress;


friend void initializeAndRunDatagramRouter(datagramRouter *inputDatagramRouter, std::promise<bool> *inputPromise);

private:
std::unique_ptr<zmq::context_t> contextRAII; //ZMQ context ownership control.  Isn't used at all if a context is passed to the datagram router

std::unique_ptr<int, void(*)(int*) > datagramSocket; //A unique pointer to the file descriptor associated with the datagramSocket.  It implements RAII for the socket

std::unique_ptr<std::thread> routerThread;
bool timeToShutdownFlag;  //True if the create thread should shutdown

//Don't allow copying
datagramRouter(datagramRouter const&) = delete;
void operator=(datagramRouter const&) = delete;
};

/*
This is the function that is called in a seperate thread by the datagramRouter object.  It routes the following datagram types:
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunDatagramRouter(datagramRouter *inputDatagramRouter, std::promise<bool> *inputPromise);








#endif
