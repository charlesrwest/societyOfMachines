#ifndef CHORDEVENTHPP
#define CHORDEVENTHPP

#include<chrono>
#include<vector>

enum chordEventType
{
UNINITIALIZED = 1,
SEND_STABILIZATION_REQUEST = 2,
REQUEST_CLOSEST_KNOWN_PREDECESSOR_MAY_HAVE_TIMED_OUT = 3,
UPDATE_FINGER_TABLE = 4,
NODE_IS_ACTIVE_REQUEST_MAY_HAVE_TIMED_OUT = 5
};

/*
This class represents events that need to occur as part of the operation of a chord node, independent of incoming messages (such as message timeouts and periodic messages).  They are normally stored in a priority queue and are ordered by time (soonest first).
*/
class chordEvent
{
public:
/*
Empty constructor that sets event type to UNINITIALIZED
*/
chordEvent();

/*
This function makes this event trigger a stabilization request when it expires.
@param inputDelay: How long to wait (in milliseconds) before sending the stabilization request
*/
void createSendStabilizationRequestEvent(std::chrono::milliseconds inputDelay);

/*
This function makes this event signal a potential timeout event for a closest known predecessor, triggering a check to see if the reply for the request has been received and resending if not.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputRequestNumber: The request number associated with the request
*/
void createClosestKnownPredecessorTimeoutEvent(std::chrono::milliseconds inputDelay, uint64_t inputRequestNumber);

/*
This function makes this event signal a potential timeout event for a node is active request, triggering a check to see if the reply for the request has been received and resending if not.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputRequestNumber: The request number associated with the request
*/
void createNodeIsActiveTimeoutEvent(std::chrono::milliseconds inputDelay, uint64_t inputRequestNumber);

/*
This function makes this event trigger sending out a request to update a particular member of the fingerTable.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputFingerTableIndex: Which finger table entry to update
*/
void createUpdateFingerTableEvent(std::chrono::milliseconds inputDelay, uint64_t inputFingerTableIndex);

/*
This function is used by the priority queue to sort the order of the events in the queue.  It is less the < operator be used to compare the timeout time of two events.  It makes events happening sooner "greater" than those that happen later.
@param inputLeftChordEvent: The chord event on the left side of the < symbol
@param inputLeftChordEvent: The chord event on the right side of the < symbol
@return: True if the left chord event is less than the right chord event
*/
friend bool operator< (const chordEvent &inputLeftChordEvent, const chordEvent &inputChordEvent);


chordEventType eventType;
std::chrono::time_point<std::chrono::steady_clock> eventTime; //The timepoint that this event should take place at
std::vector<uint64_t> integerOptions;
std::vector<double> doubleOptions;
};



/*
This function is used by the priority queue to sort the order of the events in the queue.  It is less the < operator be used to compare the timeout time of two events.  It makes events happening sooner "greater" than those that happen later.
@param inputLeftChordEvent: The chord event on the left side of the < symbol
@param inputRightChordEvent: The chord event on the right side of the < symbol
@return: True if the left chord event is less than the right chord event
*/
bool operator< (const chordEvent &inputLeftChordEvent, const chordEvent &inputRightChordEvent);












#endif
