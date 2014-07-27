#ifndef LOCALSTREAMSUBSCRIBERH
#define LOCALSTREAMSUBSCRIBERH

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
This class connects to one or more publishers described by URIs and allows messages that they publish to be recieved
*/
class localStreamSubscriber
{
public:

/*
This function creates an object that can be connected to various publishers.
@exceptions: This function will throw an exception if the engine is invalid or a ZMQ function fails.
*/
localStreamSubscriber();

/*
This function creates a subscription to a particular publisher.
@param inputPublisherLocalURI: The URI to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void subscribe(const std::string &inputPublisherResourceName);



/*
This function creates a subscription to a particular publisher.
@param inputPublisherLocalURI: The URI to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void subscribe(const localURI &inputPublisherLocalURI);

/*
This function creates a subscription to a set of publishers.
@param inputPublisherLocalURIs: A set of URIs to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void subscribe(const std::vector<localURI> &inputPublisherLocalURIs);

/*
This function creates a subscription to a set of publishers.
@param inputPublisherLocalURIs: A set of URIs to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void subscribe(const std::vector<std::string> &inputPublisherResourceNames);

/*
This function gets a ZMQ message from the publisher(s) associated with this object.
@param inputMessageBuffer: The ZMQ message to put the received message in
@return: 1 if the message was received successfully, 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int receiveMessage(zmq::message_t *inputMessageBuffer);


/*
This function cleans up after the object
*/
~localStreamSubscriber();

private:
std::unique_ptr<zmq::socket_t> subscriberSocket; //A unique pointer to the ZMQ socket this object uses
};





















#endif
