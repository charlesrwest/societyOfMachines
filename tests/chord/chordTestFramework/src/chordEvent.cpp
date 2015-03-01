#include "chordEvent.hpp"

/*
Empty constructor that sets event type to UNINITIALIZED
*/
chordEvent::chordEvent()
{
eventType = UNINITIALIZED;
}

/*
This function makes this event trigger a stabilization request when it expires.
@param inputDelay: How long to wait (in milliseconds) before sending the stabilization request
*/
void chordEvent::createSendStabilizationRequestEvent(std::chrono::milliseconds inputDelay)
{
eventType = SEND_STABILIZATION_REQUEST;
eventTime =std::chrono::steady_clock::now() + inputDelay;
}

/*
This function makes this event signal a potential timeout event for a closest known predecessor, triggering a check to see if the reply for the request has been received and resending if not.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputRequestNumber: The request number associated with the request
*/
void chordEvent::createClosestKnownPredecessorTimeoutEvent(std::chrono::milliseconds inputDelay, uint64_t inputRequestNumber)
{
eventType = REQUEST_CLOSEST_KNOWN_PREDECESSOR_MAY_HAVE_TIMED_OUT;
integerOptions.push_back(inputRequestNumber);
eventTime = std::chrono::steady_clock::now() + inputDelay;
}

/*
This function makes this event signal a potential timeout event for a node is active request, triggering a check to see if the reply for the request has been received and resending if not.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputRequestNumber: The request number associated with the request
*/
void chordEvent::createNodeIsActiveTimeoutEvent(std::chrono::milliseconds inputDelay, uint64_t inputRequestNumber)
{
eventType = NODE_IS_ACTIVE_REQUEST_MAY_HAVE_TIMED_OUT;
integerOptions.push_back(inputRequestNumber);
eventTime = std::chrono::steady_clock::now() + inputDelay;
}

/*
This function makes this event trigger sending out a request to update a particular member of the fingerTable.
@param inputDelay: How long to wait (in milliseconds) before triggering
@param inputFingerTableIndex: Which finger table entry to update
*/
void chordEvent::createUpdateFingerTableEvent(std::chrono::milliseconds inputDelay, uint64_t inputFingerTableIndex)
{
eventType = UPDATE_FINGER_TABLE;
integerOptions.push_back(inputFingerTableIndex);
eventTime = std::chrono::steady_clock::now() + inputDelay;
}

/*
This function is used by the priority queue to sort the order of the events in the queue.  It is less the < operator be used to compare the timeout time of two events.  It makes events happening sooner "greater" than those that happen later.
@param inputLeftChordEvent: The chord event on the left side of the < symbol
@param inputRightChordEvent: The chord event on the right side of the < symbol
@return: True if the left chord event is less than the right chord event
*/
bool operator< (const chordEvent &inputLeftChordEvent, const chordEvent &inputRightChordEvent)
{
return inputLeftChordEvent.eventTime > inputRightChordEvent.eventTime;
}
