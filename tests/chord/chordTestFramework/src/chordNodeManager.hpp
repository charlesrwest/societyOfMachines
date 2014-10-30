#ifndef CHORDNODEMANAGERHPP
#define CHORDNODEMANAGERHPP


#include<cstdio>
#include<future>
#include<string>
#include<map>
#include<memory>


#include<sodium/crypto_hash_sha512.h>
#include<zmq.hpp>

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"
#include "chordNode.hpp"

#include "pauseResumeSignal.pb.h"
#include "signalAck.pb.h"

/*
This object maintains a list of chord nodes (each with there own thread) and allows for them to be paused so that a snapshot of the chord ring can be taken.
*/
class chordNodeManager
{
public:

/*
This function initializes the chordNodeManager object so that it can be used.
@param inputInprocAddressForPauseResumeSignals: The string to use for the inproc address (the string header will be added later)
@exception: This function can throw exceptions if a ZMQ call fails
*/
chordNodeManager(std::string inputInprocAddressForPauseResumeSignals);

/*
This function creates a new chord node object/process.  The created node is fully initialized once the promise returns its result.  Any exceptions that occurred as part of the initialization are thrown when the promise is queried for its result
@param inputEntryChordNodePortNumber: The port number of the node that the new node should use to enter the network (0 indicates that no other nodes currently exist)
@return: This function returns the port number associated with the node or throws an exception
@exception: This function can throw SOMExceptions if something goes wrong
*/
uint32_t createChordNode(uint32_t inputEntryChordNodePortNumber);

/*
This function publishes the pause signal, then waits until all of the nodes have emitted the pausing signal.
*/
void pauseNodes();

/*
This function publishes the resume signal, then waits until all of the nodes have emitted the resumed signal.
*/
void resumeNodes();

/*
This function publishes the pause signal or resume signal, then waits until all of the nodes have emitted the corresponding ack.
@inputPauseNodes: True if the nodes are to be paused and false if they are to be resumed
*/
void pauseOrResumeNodes(bool inputPauseNodes);


std::unique_ptr<zmq::context_t> context; //ZMQ context, must be declared first so it goes out of scope last


std::unique_ptr<zmq::socket_t> pauseResumeSignalSocket;  //ZMQ inproc socket to publish pause/resume signals

std::unique_ptr<zmq::socket_t> pauseResumeSignalAckSocket;  //ZMQ inproc socket to receive pause acknowledgements

std::map<uint32_t, std::unique_ptr<chordNode> > chordRingIDToNode; //A list of all of the chord nodes

std::string inprocAddressForPauseResumeSignals;

private:
//Don't allow copying
chordNodeManager(chordNodeManager const&) = delete;
void operator=(chordNodeManager const&) = delete;
};


#endif
