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
#include<queue>


#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"
#include "hashAndBigNumberFunctions.hpp"
#include "fingerTable.hpp"
#include "virtualPortRouter.hpp"
#include "virtualPortPairSocketWrapper.hpp"
#include "findAddressOwnerRequest.hpp"
#include "nodeIsActiveRequest.hpp"
#include "chordEvent.hpp"

#include "pauseResumeSignal.pb.h"
#include "signalAck.pb.h"
#include "chordNodeContactInformation.pb.h"
#include "stabilizationRequestOrResponse.pb.h"
#include "closestPredecessorsRequest.pb.h"
#include "findAddressOwnerRequestOrResponse.pb.h"
#include "nodeIsActiveRequestResponse.pb.h"
#include "closestPredecessorsRequestResponse.pb.h"

//For testing 
#include<unistd.h>

#define FINGER_TABLE_SIZE 64
#define CHORD_ID_SIZE 64
#define STABILIZATION_MESSAGE_PERIOD 1000 //Period of sending stabilization messages in milliseconds
//TODO: Change back
#define FINGER_TABLE_REFRESH_PERIOD 10000//How often to refresh any given finger table entry milliseconds
#define NUMBER_OF_PREDECESSORS_TO_GET_PER_NODE 2 //How many predecessors to retrieve from each node while resolving a address owner
#define MAXIMUM_NUMBER_OF_PREDECESSOR_REQUEST_RESENDS 4 //Only send a predecessor request 5 times before giving up
#define MAXIMUM_NUMBER_OF_BAD_REFERENCES 4 //Number of bad references a node can give before it is removed from consideration
#define CLOSEST_KNOWN_PREDECESSOR_REQUEST_TIMEOUT_DELAY 1000 //The amount of time in milliseconds to wait for a closest known predecessor request to be answered
#define MAXIMUM_NUMBER_OF_IS_NODE_ACTIVE_RESENDS 10
#define IS_NODE_ACTIVE_TIMEOUT_DELAY 1000 //The amount of time in milliseconds to wait for a is node active request to be returned before resending

#define NODE_IS_ACTIVE_REQUEST_HANDLER_PORT_OFFSET               0
#define STABILIZATION_REQUEST_HANDLER_PORT_OFFSET                1
#define CLOSEST_KNOWN_PREDECESSOR_REQUESTS_HANDLER_PORT_OFFSET   2
#define NODE_IS_ACTIVE_REQUEST_SENDER_PORT_OFFSET                3
#define STABILIZATION_REQUEST_SENDER_PORT_OFFSET                 4
#define CLOSEST_PREDECESSOR_REQUEST_SENDER_PORT_OFFSET           5

class chordNodeResources;

/*
This class represents a single chord node 
*/
class chordNode
{
public:
/*
This function initializes the object and starts a thread that conducts the operations of the node.  It does not return until all initialization in the thread is completed and the nodePortNumber is safe to read.
@param inputChordNodeContactInfos: A list of contact info for nodes that this one can contact to join the network
@param inputManagerBaseInprocAddress: A string to the root of the node manager inproc addresses
@param inputZMQContext: The ZMQ context to use for inproc communications

@exceptions: This function can throw exceptions
*/
chordNode(const std::vector<chordNodeContactInformation> &inputChordNodeContactInfos, std::string inputManagerBaseInprocAddress, zmq::context_t *inputZMQContext);

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
~chordNode();


zmq::context_t *context;
std::unique_ptr<virtualPortRouter> datagramInterfaceRouter; //The virtual port router associated with this node


uint32_t nodePortNumber;
chordNodeContactInformation selfContactInfo;
unsigned char chordID[crypto_hash_sha512_BYTES];  //The 64 byte sha-512 hash of the port number that serves as an ID
boost::multiprecision::uint512_t chordIDForComputation; //The bigint version of the chord ID, used for arithmatic operations with other node IDs
std::string findNodeAssociatedWithAddressInprocAddress; //The inproc string used to connect to the address owner request socket

std::string managerBaseInprocAddress;
std::unique_ptr<std::thread> nodeThread;
bool timeToShutdownFlag;  //True if the created thread should shutdown

chordNodeResources *nodeResourcesPointer; //For debugging only

//Allow the thread function to access private variables
friend void initializeAndRunChordNode(chordNode *inputChordNode, const std::vector<chordNodeContactInformation> &inputChordNodeContactInformation, std::promise<bool> *inputPromise);

private:
//Don't allow copying
chordNode(chordNode const&) = delete;
void operator=(chordNode const&) = delete;
};

/*
This function is run as a seperate thread and implements the chord node behavior
@param inputChordNode: A pointer to the object associated with the node this function is running
@param inputChordNodeContactInformation: The contact info for the nodes to contact to join the network
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunChordNode(chordNode *inputChordNode, const std::vector<chordNodeContactInformation> &inputChordNodeContactInformation, std::promise<bool> *inputPromise);

/*
This class bundles resources associated with the operation of the thread(s) associated with the chord node.
*/
class chordNodeResources
{
public:
/*
This function initializes the sockets to get/send signals for pausing/resuming the chord node thread and creates the datagram router needed to send/receive udp datagrams.
@param inputChordNode: The chord node these resources are associated
@param inputChordNetworkContactInformation: A list of chord nodes to contact to try to join the network.

@exceptions: This function can throw exceptions
*/
chordNodeResources(chordNode *inputChordNode,  std::vector<chordNodeContactInformation> inputChordNetworkContactInformation);

/*
This function sends a node is active request to the given node and creates a datastructure/schedules events to ensure that the requests will be resent until either the node answers or the resend limit is reached.
@param inputNodeToContact: The node to check if active

@exceptions: This function can throw exceptions
*/
void startNodeIsActiveRequest(const chordNodeContactInformation &inputNodeToContact);

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
uint64_t getNextMessageNumber();

/*
This function listens to the sockets and processes the event queue to perform the functions expected of a chord node.  It does not return unless something goes wrong.

@exceptions: This function can throw exceptions
*/
void operateNode();

/*
This function grabs the next event in the queue and takes the corresponding action.  If there is no event left, this function returns silently.

@exceptions: This function can throw exceptions
*/
void processEvent();

/*
This function sends a stabilizationRequestOrResponse to this node's successor to determine if it needs to update its successor entry and schedules the next stabilization message.

@exceptions: This function can throw exceptions
*/
void processStabilizationRequestTimeout();

/*
This function resends requestClosestKnownPredecessor message with a new request ID, updates or deletes the associated findAddressOwner datastructure if the request actually timed out and sends a request failed message to the find request owner sender if all options have been exhausted.
@param inputRequestID: The ID of the request that may have timed out

@exceptions: This function can throw exceptions
*/
void processPossibleRequestClosestKnownPredecessorTimeOut(uint64_t inputRequestID);

/*
This function sends a findAddressOwnerRequest and creates a entry in requestIDToFingerTableEntryIndex to ensure that the corresponding finger table entry is updated when the result is returned.
@param inputFingerTableIndex: The index of the finger table to update

@exceptions: This function can throw exceptions
*/
void updateFingerTable(uint64_t inputFingerTableIndex);

/*
This function resends nodeIsActiveRequestResponse message with a new request ID if it has timed out.
@param inputRequestID: The ID of the request that may have timed out

@exceptions: This function can throw exceptions
*/
void resendNodeIsActiveRequestIfTimedOut(uint64_t inputRequestID);

/*
This function extracts the next processClosestPredecessorRequest that was gotten in response to one of this node's queries.  It then retrieves the associated findAddressOwnerRequest and performs the required operations on it.  This can include sending the next message, reassigning the findAddressOwnerRequest to a different request number and marking chordIDs as unreliable (and so, not usable).

@exceptions: This function can throw exceptions
*/
void processClosestPredecessorRequestReply();

/*
This function extracts the next stabilizationRequestOrResponse that has been received and updates the successor entry if needed.

@exceptions: This function can throw exceptions
*/
void processStabilizationRequestReply();

/*
This function extracts the next processNodeIsActiveRequestReply (which is a empty message with a header and updates the finger table based on it.

@exceptions: This function can throw exceptions
*/
void processNodeIsActiveRequestReply();

/*
This function extracts the next findAddressOwnerRequestReply and takes the corresponding action depending on which type of action the request was for.

@exceptions: This function can throw exceptions
*/
void processFindAddressOwnerRequestReply();

/*
This function extracts the next findAddressOwnerRequestOrResponse message, creates a findAddressOwnerRequest based on it, sends the first request then creates the associated timeout event and adds it to the event queue.  When the request is finally returned (after many sub requests to transverse the chord network).

@exceptions: This function can throw exceptions
*/
void processFindNodeAssociatedWithAddressRequest();

/*
This function extracts the next closestPredecessorRequest (from an external node) and answers it using the finger table to compute the required number of closest predecessors.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void processClosestKnownPredecessorRequest();

/*
This function extracts the next stabilizationRequestOrResponse (from an external node) and answers it using the finger table to get this node's current predecessor.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void processStabilizationRequest();

/*
This function extracts the next nodeIsActiveRequest (from an external node) and answers to prove that this node is active.  It also updates the finger table based on the received messages.

@exceptions: This function can throw exceptions
*/
void processNodeIsActiveRequest();

chordNode *associatedChordNode;

std::mutex nodeMessageNumberIndexMutex;
uint64_t nodeMessageNumberIndex;

std::unique_ptr<fingerTable> nodeFingerTable;

std::priority_queue<chordEvent> eventQueue; //This structure organizes activities that take place independent of incoming messages and also message timeouts

//This maps find the information related to the particular request
std::map<uint64_t, std::unique_ptr<findAddressOwnerRequest> > requestIDToAddressOwnerRequest; 
std::map<uint64_t, std::string> findAddressOwnerRequestIDToConnectionID;

std::map<uint64_t, uint64_t> requestIDToFingerTableEntryIndex;
std::map<uint64_t, std::unique_ptr<nodeIsActiveRequest> > requestIDToNodeIsActiveRequest;




std::unique_ptr<zmq::socket_t> pauseResumeSignalSocket; 
std::unique_ptr<zmq::socket_t> pauseResumeSignalAckSocket;//socket to signal acknowledgement of the pause/resume signal

std::unique_ptr<zmq::socket_t> findNodeAssociatedWithAddressHandler;  //Expects a findAddressOwnerRequestOrResponse and responds with the same

std::unique_ptr<virtualPortPairSocketWrapper> closestKnownPredecessorRequestsHandler; //Expects a closestPredecessorRequest and responds with a closestPredecessorRequestResponse
std::unique_ptr<virtualPortPairSocketWrapper> stabilizationRequestHandler; //Expects a stabilizationRequestOrResponse and responds with a stabilizationRequestOrResponse
std::unique_ptr<virtualPortPairSocketWrapper> nodeIsActiveRequestHandler; //Expects a empty message (with a header) and responds with a empty message (with a header)

std::unique_ptr<zmq::socket_t> findNodeAssociatedWithAddressRequestSender;  //Expects a findAddressOwnerRequestOrResponse and responds with the same (used to send requests to findNodeAssociatedWithAddressHandler and update based on the responses)
std::unique_ptr<virtualPortPairSocketWrapper> closestPredecessorRequestSender;  //Sends out closestPredecessorRequests and receives closestPredecessorRequestResponses 
std::unique_ptr<virtualPortPairSocketWrapper> stabilizationRequestSender;  //Sends out a stabilizationRequestOrResponse and receives a stabilizationRequestOrResponse
std::unique_ptr<virtualPortPairSocketWrapper> nodeIsActiveRequestSender;  

private:
/*
This function is used to setup all of the ZMQ sockets used by the chord node resources object.

@exceptions: This function can throw exceptions
*/
void initializeChordNodeResourcesSockets();

/*
This function creates and stores the events which are responsible for sending stabilization messages and finger table update requests. 
*/
void scheduleStabilizationMessagesAndFingerUpdates();

/*
This function sends a node is active request to the given node without affecting the nodeIsActiveRequest structure associated with the request ID.
@param inputNodeToContact: The node to check if active
@param inputRequestID: The request ID to put in the message

@exceptions: This function can throw exceptions
*/
void sendNodeIsActiveRequest(const chordNodeContactInformation &inputNodeToContact, uint64_t inputRequestID);

/*
This function sends the message associated with a findAddressOwner request by looking up the relavent data structure and sending the request associated.  It is usually used to send a message after the datastructure is updated as part of the resolution process.  It moves the data to a new request number as it sends the message.
@param inputRequestID: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void sendFindPredecessorRequest(uint64_t inputRequestID);

/*
This function sends a message indicating request failure and cleans up data associated with serving that request.  
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void sendNodeAssociatedWithAddressRequestFailedMessageAndCleanupAssociatedData(uint64_t inputAddressOwnerRequestNumber);

/*
This message is used to indicate failure of a request.  It retrieves the connection ID associated with the sender of the request and sends the message to that ID.  
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request

@exceptions: This function can throw exceptions
*/
void sendNodeAssociatedWithAddressRequestFailedMessage(uint64_t inputAddressOwnerRequestNumber);

/*
This function sends a message to return the owner of a particular address in response to a query.  After sending the message, it cleans up the data that was stored as part of processing the request.
@param inputAddressOwnerRequestNumber: The request number of the last "closestKnownPredecessor" message associated with the request
@param inputAddressOwner: The contact info of the address owner

@exceptions: This function can throw exceptions
*/
void sendNodeAssociatedWithRequestMessageAndCleanupAssociatedData(uint64_t inputAddressOwnerRequestNumber, const chordNodeContactInformation &inputAddressOwner);
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
