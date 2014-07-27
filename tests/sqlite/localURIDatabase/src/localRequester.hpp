#ifndef LOCALREQUESTERH
#define LOCALREQUESTERH

#include<string>
#include "zmq.hpp"

#include "../messages/localURI.pb.h"
#include "../messages/localURIQuery.pb.h"
#include "../messages/localURIRemovalRequest.pb.h"
#include "../messages/localURIReplyHeader.pb.h"

#include "classDeclarations.h"
#include "SOMException.hpp"
#include "localURIEngine.hpp"

/*
This class connects to the replier associated with a particular URI and allows requests to be sent to it and the responses received.
*/
class localRequester
{
public:

/*
This function creates an object that tries to connect to the given replier.
@param inputReplierLocalURI: The local URI associated with the replier this object is used to talk to.
@exceptions: This function will throw an exception if the inproc proxy isn't initialized or the object was unable to connect to the replier
*/
localRequester(const localURI &inputReplierLocalURI);

/*
This function creates an object that tries to connect to the given replier.
@param inputReplierResourceName: The resource name associated with the replier this object is used to talk to.
@exceptions: This function will throw an exception if the inproc proxy isn't initialized or the object was unable to connect to the replier
*/
localRequester(const std::string &inputReplierResourceName);

/*
This function sends a ZMQ request message to the replier associated with this object.   The getReply function must be called to get the reply before this function can be used again (unless it is a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int sendRequest(zmq::message_t &inputMessage);

/*
This function sends a ZMQ request message to the replier associated with this object.   The getReply function must be called to get the reply before this function can be used again (unless it is a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int sendRequest(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart = false);


/*
This function gets a ZMQ request message from the replier associated with this object.   The sendRequest function must be called to send a request before this function can be used again (unless it is a multi part message).
@param inputMessageBuffer: The ZMQ message buffer to place the message in
@return: 1 if the message was received successfully and 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int getReply(zmq::message_t *inputMessageBuffer);


/*
This function lets the parent know that it that is being deleted, so any resources associated with it should be freed.
*/
~localRequester();

private:
std::unique_ptr<zmq::socket_t> requesterSocket; //A unique pointer to the ZMQ socket this object uses
};
















#endif
