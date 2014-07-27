#ifndef LOCALSTREAMPUBLISHERH
#define LOCALSTREAMPUBLISHERH

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
This class registers the URI associated with the publisher after ensuring that it is marked with a macro defined PUBLISHER_TAG tag and then acts as a wrapper around a ZMQ socket. 
*/
class localStreamPublisher
{
public:
/*
@param inputStreamLocalURI: The local URI associated with the stream.  Having non-unique tags or keys associated with a URI or already containing the PUBLISHER_TAG tag will cause the database to reject the entry, which will result in an exception being thrown.
@exceptions: This function will throw an exception if a ZMQ operation fails or the engine is invalid
*/
localStreamPublisher(const localURI &inputStreamLocalURI);

/*
This function publishes a ZMQ message associated with this object.
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 otherwise
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int publish(zmq::message_t &inputMessageBuffer);

/*
This function publishes a ZMQ message associated with this object.
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@param inputSetMessageMultipart: True if there are more parts of a multi part message to follow
@return: 1 if the message was sent successfully and 0 otherwise
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int publish(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart = false);

/*
This function cleans up after the object
*/
~localStreamPublisher();

localURI associatedURI;
std::unique_ptr<zmq::socket_t> publisherSocket; //A unique pointer to the ZMQ socket this object uses
};





#endif
