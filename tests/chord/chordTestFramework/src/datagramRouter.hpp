#ifndef DATAGRAMROUTERHPP
#define DATAGRAMROUTERHPP

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

#include "closestPredecessorRequest.pb.h"
#include "closestPredecessorRequestResponse.pb.h"
#include "datagramAddress.pb.h"
#include "informationRequest.pb.h"
#include "informationRequestResponse.pb.h"
#include "messageContainer.pb.h"
#include "predecessorRequest.pb.h"
#include "predecessorResponse.pb.h"

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

#define UDP_MESSAGE_MAX_SIZE 1500

/*
This function binds a more or less random port and uses it to allow associated threads to send and receive datagrams.  Incoming datagrams are assumed to be protobuf messageContainers (which can contain one or more messages of several types).  It then adds the ip and port of the sender to the message and publishes it to an inproc address based on the message types. 

Messages can be sent as datagrams by sending a serialized message container with a datagramAddress field to the address identified by the datagram router sendingAddress.

Currently supported message types for sending/receiving:
predecessorRequest
predecessorRequestResponse
closestPredecessorRequest
closestPredecessorRequestResponse
informationRequest
informationRequestResponse

Much of this could be done more efficiently.  This class spawns two threads: one listens to the datagram socket and forwards to the inproc and then other listens to the inproc sending address and then forwards it as a datagram 
*/
class datagramRouter
{
public:
/*
This function initializes the object and starts two threads that conducts the operations of the router.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read.
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
datagramRouter(zmq::context_t *inputZMQContext);

/*
This function signals for the threads to shut down and then waits for it to do so.
*/
~datagramRouter();

zmq::context_t *context;

int datagramPortNumber;
bool timeToShutdownFlag;  //True if the created threads should shutdown
std::unique_ptr<std::thread> datagramRouterThread;
int datagramSocketFileDescriptor;  //File descriptor associated with datagram socket

std::string routerExtension;
std::string sendingAddress;
std::string predecessorRequestAddress;
std::string predecessorRequestResponseAddress;
std::string closestPredecessorRequestAddress;
std::string closestPredecessorRequestResponseAddress;
std::string informationRequestAddress;
std::string informationRequestResponseAddress;


private:
//Don't allow copying
datagramRouter(datagramRouter const&) = delete;
void operator=(datagramRouter const&) = delete;
};

/*
This function is run as a seperate thread and implements the datagram router behavior
@param inputDatagramRouter: A pointer to the object associated with the object this function is operating on
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunDatagramRouterThread(datagramRouter *inputDatagramRouter, std::promise<bool> *inputPromise);

//This class simplifies the construction/operation of the thread associated with the datagram router
class datagramRouterThreadResources
{
public:
/*
This function initializes each of the inproc sockets associated with the datagram router and populates the protobufObjectSocketsMap so that it can be used to simplify operations applied to all of the sockets associated with protobuf datastructures
@param inputDatagramRouter: A pointer to the object associated with the object this function is operating on

@exceptions: This function can throw exceptions
*/
datagramRouterThreadResources(datagramRouter *inputDatagramRouter);

/*
This function waits until either an incoming datagram or an outgoing message is received.  It also sets the flags for checkForAndHandleDatagrams and checkForAndHandleOutgoingMessages to use.
@return: -1 if an error occured, 0 if no events occured
*/
int waitForEvent();

/*
This function checks to see if there is an incoming datagram to be processed.  If there is, it forwards it to the appropriate inproc socket

@exceptions: This function can throw exceptions
*/
void checkForAndHandleDatagrams();

/*
This function checks to see if there is an outgoing message.  If there is, it sends it as a datagram to the appropriate datagram address

@exceptions: This function can throw exceptions
*/
void checkForAndHandleOutgoingMessages();

//The datagram router this thread is working for
datagramRouter *associatedDatagramRouter;

//Socket to take requests to send a datagram message
std::unique_ptr<zmq::socket_t> outboundDatagramInprocSocket;

//Sockets for each protobuf socket type
std::unique_ptr<zmq::socket_t> predecessorRequestInprocSocket;
std::unique_ptr<zmq::socket_t> predecessorRequestResponseInprocSocket;
std::unique_ptr<zmq::socket_t> closestPredecessorRequestInprocSocket;
std::unique_ptr<zmq::socket_t> closestPredecessorRequestResponseInprocSocket;
std::unique_ptr<zmq::socket_t> informationRequestInprocSocket;
std::unique_ptr<zmq::socket_t> informationRequestResponseInprocSocket;

//Map relating socket pointers to their addresses
std::map<std::string, std::unique_ptr<zmq::socket_t>* > protobufObjectSocketsMap;

//datastructure for listening to sockets
zmq::pollitem_t items[2];

//Buffer for UDP messages
char udpMessageBuffer[UDP_MESSAGE_MAX_SIZE];
};


#endif
