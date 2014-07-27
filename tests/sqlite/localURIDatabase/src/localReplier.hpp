#ifndef LOCALREPLIERH
#define LOCALREPLIERH



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
This class registers the URI associated with the replier after ensuring that it is marked with a macro defined REPLIER_TAG tag and then acts as a wrapper around a ZMQ socket. 
*/
class localReplier
{
public:
/*
@param inputReplierLocalURI: The local URI associated with the replier.  Having non-unique tags or keys associated with a URI or already containing the macro defined REPLIER_TAG tag will cause the database to reject the entry, which will result in an exception being thrown.
@param inputRequestTimeoutTimeInMilliseconds: How long (in milliseconds) the object should wait for the singleton to respond to its request before throwing an exception
@exceptions: This function will throw an exception if a ZMQ operation fails or the request to the database server is rejected
*/
localReplier(const localURI &inputReplierLocalURI, uint32_t inputRequestTimeoutTimeInMilliseconds = 1000);

/*
This function gets a ZMQ request message from one of the requesters talking to this.   The sendReply function must be called to send a reply before this function can be used again (unless it is a multi part message).
@param inputMessageBuffer: The ZMQ message buffer to place the received result in
@return: 1 if the message was received successfully and 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int getRequest(zmq::message_t *inputMessageBuffer);

/*
This function sends a ZMQ request message to the requester associated with the previously received request.   The getRequest function must be called to get the reply before this function can be used again (unless sending a multi part message).
@param inputMessageData: A pointer to the data to be sent
@param inputMessageDataSize: The size of the data to be sent (bytes)
@param inputSendMessageMultipart: True if more pieces of the message are to follow
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if the zmq_send function returns with anything besides success or timeout
*/
int sendReply(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart = false);

/*
This function sends a ZMQ request message to the requester associated with the previously received request.   The getRequest function must be called to get the reply before this function can be used again (unless sending a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if the zmq_send function returns with anything besides success
*/
int sendReply(zmq::message_t &inputMessage);

/*
This function cleans up the ZMQ socket associated with this object and requests for the associated URI to be removed from the server.  It does not throw exceptions but can print to stderr.
*/
~localReplier();

private:
localURI replierURI; //A copy of the URI associated with this replier, complete with the macro defined REPLIER_TAG tag
std::unique_ptr<zmq::socket_t> replierSocket; //A unique pointer to the ZMQ socket this object uses
};














#endif
