#ifndef CHORDNODEHPP
#define CHORDNODEHPP

#include<cstdio>
#include<future>
#include<thread>
#include<string>
#include<random>
#include<mutex>

#include<cstdlib>
#include<cmath>

#include<sodium/crypto_hash_sha512.h>
#include <boost/multiprecision/cpp_int.hpp>
#include<zmq.hpp>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>


#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"
#include "datagramRouter.hpp"
#include "hashAndBigNumberFunctions.hpp"
#include "fingerTable.hpp"

#include "pauseResumeSignal.pb.h"
#include "signalAck.pb.h"


//For testing 
#include<unistd.h>

/*
This class represents a single chord node 
*/
class chordNode
{
public:
/*
This function initializes the object and starts a thread that conducts the operations of the node.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read
@param inputEntryChordPortNumber: The port number of a node in the chord network (used to allow this node to join the network).  If the port number is 0, the node just starts a new network
@param inputManagerBaseInprocAddress: A string to the root of the node manager inproc addresses
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
chordNode(uint32_t inputEntryChordNodePortNumber, std::string inputManagerBaseInprocAddress, zmq::context_t *inputZMQContext);

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
~chordNode();


zmq::context_t *context;
//std::unique_ptr<void *> pauseResumeSignalSocket;
//std::unique_ptr<void *> pauseResumeSignalAckSocket;
//std::unique_ptr<void *> findNodeAssociatedWithKeyServerSocket;
//std::unique_ptr<void *> addRemoveKeyValueRequestServerSocket;
//std::unique_ptr<void *> getKeyValueRequestServerSocket;


uint32_t nodePortNumber;
unsigned char chordID[crypto_hash_sha512_BYTES];  //The 64 byte sha-512 hash of the port number that serves as an ID
boost::multiprecision::uint512_t chordIDForComputation; //The bigint version of the chord ID, used for arithmatic operations with other node IDs
std::string nodeData; //A random string comprising the node's "data"
std::string findNodeAssociatedWithKeyServerSocketAddress;


std::string managerBaseInprocAddress;
std::unique_ptr<std::thread> nodeThread;
bool timeToShutdownFlag;  //True if the created thread should shutdown

//Allow the thread function to access private variables
friend void initializeAndRunChordNode(chordNode *inputChordNode, uint32_t inputEntryChordNodePortNumber, std::promise<bool> *inputPromise);

private:
//Don't allow copying
chordNode(chordNode const&) = delete;
void operator=(chordNode const&) = delete;
};

/*
This function is run as a seperate thread and implements the chord node behavior
@param inputChordNode: A pointer to the object associated with the node this function is running
@param inputEntryChordNodePortNumber: The port node of the chord node used to enter the chord network
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunChordNode(chordNode *inputChordNode, uint32_t inputEntryChordNodePortNumber, std::promise<bool> *inputPromise);

/*
This class bundles resources associated with the operation of the thread(s) associated with the chord node.
*/
class chordNodeResources
{
public:
/*
This function initializes the sockets to get/send signals for pausing/resuming the chord node thread and creates the datagram router needed to send/send receive udp datagrams.
@param inputChordNode: The chord node these resources are associated

@exceptions: This function can throw exceptions
*/
chordNodeResources(chordNode *inputChordNode);

/*
This function checks if a pause or resume signal has been published.
@param inputBlockForSignal: True if the function should block until a signal occurs, false if it should return immediately if there is not a signal in the queue
@return: 0 on timeout, 1 on pause signal, 2 on resume signal

@exceptions: This function can throw exceptions
*/
int checkForPauseResumeSignal(bool inputBlockForSignal = false);

/*
This function sends an acknowledgement message for either a pause signal or a resume signal.
@param inputAcknowledgePauseSignal: True if a pause should be acknowledged, false if a resume should be acknowledged

@exceptions: This function can throw exceptions
*/
void acknowledgePauseResumeSignal(bool inputAcknowledgePauseSignal);

/*
This convenience function allows for easy processing of pause/resume signals.  It will do a non blocking check to see if any pause signals have been sent.  If there is a pause signal, it will send an acknowledgement and block until a resume signal is received (which it will also acknowledge).
@param inputSignalSocket: The configured ZMQ socket to check for signals
@param inputAckSocket: The configured ZMQ socket to send acknowledgements on

@exceptions: This function can throw exceptions
*/
void processPauseResumeSignals();



/*
This function handles pausing/resuming and returns 1 if it is time for the node to shutdown.  It is used primarily to make blocking functions that still follow the required pause/resume and shutdown behaviors.
@return: 1 if it is time to shutdown and 0 otherwise
*/
int handlePauseResumeAndShutdownSignal();


/*
This function is used to generate quasi-unique numbers so that message responses can be properly correlated with with the requests.  This function is threadsafe.
@return: A uint64_t that is unique until it cycles through 2^64 entries
*/
uint64_t getNextNodeMessageNumber();

chordNode *associatedChordNode;

std::mutex nodeMessageNumberIndexMutex;
uint64_t nodeMessageNumberIndex;

std::mutex fingerTableMutex;
std::unique_ptr<fingerTable> nodeFingerTable;

std::unique_ptr<zmq::socket_t> pauseResumeSignalSocket; 
std::unique_ptr<zmq::socket_t> pauseResumeSignalAckSocket;//socket to signal acknowledgement of the pause/resume signal
std::unique_ptr<zmq::socket_t> findNodeAssociatedWithKeyServerSocket;  //Expects a closestPredecessorRequest
std::unique_ptr<datagramRouter> associatedDatagramRouter;
};




/*
This function performs the actions nessisary to join the chord ring using the entry node described by inputEntryChordNodePortNumber.
@param inputChordNode: A pointer to the node that is joining the chord ring
@param inputSocketDescriptor: The datagram socket descriptor that will be used for sending/receiving datagrams
@param inputEntryChordNodePortNumber: The port number/Inproc address associated with the entry node

@exceptions: This function can throw exceptions
*/
void joinChordNetwork(chordNode *inputChordNode, int inputSocketDescriptor, uint32_t inputEntryChordNodePortNumber);







#endif
