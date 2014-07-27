#include "localStreamSubscriber.hpp"

/*
This function creates an object that can be connected to various publishers.
@exceptions: This function will throw an exception if the engine is invalid or a ZMQ function fails.
*/
localStreamSubscriber::localStreamSubscriber()
{
//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Create IPC socket
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_SUB));

subscriberSocket = std::move(scopeTransferPointer);
SOM_CATCH("Error, unable to create a socket for localStreamSubscriber\n")

//Set filter to allow all message types through
SOM_TRY
subscriberSocket->setsockopt( ZMQ_SUBSCRIBE, NULL, 0);
SOM_CATCH("Error setting stream subscriber subscription filter\n")
}

/*
This function creates a subscription to a particular publisher.
@param inputPublisherLocalURI: The URI to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void localStreamSubscriber::subscribe(const std::string &inputPublisherResourceName)
{
//Connect to publisher
SOM_TRY
subscriberSocket->connect(("ipc://"+localURIEngine::pathForMessagingDirectory+inputPublisherResourceName).c_str());
SOM_CATCH("Error connecting stream subscriber socket to publisher\n")

}



/*
This function creates a subscription to a particular publisher.
@param inputPublisherLocalURI: The URI to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void localStreamSubscriber::subscribe(const localURI &inputPublisherLocalURI)
{
SOM_TRY
subscribe(inputPublisherLocalURI.resourcename());
SOM_CATCH("Error subscribing\n")
}

/*
This function creates a subscription to a set of publishers.
@param inputPublisherLocalURIs: A set of URIs to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void localStreamSubscriber::subscribe(const std::vector<localURI> &inputPublisherLocalURIs)
{
for(int i=0; i<inputPublisherLocalURIs.size(); i++)
{
SOM_TRY
subscribe(inputPublisherLocalURIs[i]);
SOM_CATCH("Error subscribing\n")
}
}

/*
This function creates a subscription to a set of publishers.
@param inputPublisherLocalURIs: A set of URIs to subscribe to
@exceptions: This function will throw an exception if a ZMQ call fails
*/
void localStreamSubscriber::subscribe(const std::vector<std::string> &inputPublisherResourceNames)
{
for(int i=0; i<inputPublisherResourceNames.size(); i++)
{
SOM_TRY
subscribe(inputPublisherResourceNames[i]);
SOM_CATCH("Error subscribing\n")
}

}

/*
This function gets a ZMQ message from the publisher(s) associated with this object.
@param inputMessageBuffer: The ZMQ message to put the received message in
@return: 1 if the message was received successfully, 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int localStreamSubscriber::receiveMessage(zmq::message_t *inputMessageBuffer)
{
if(inputMessageBuffer == NULL)
{
return 0;
}

SOM_TRY
return subscriberSocket->recv(inputMessageBuffer);
SOM_CATCH("Error receiving published message\n")
}


/*
This function cleans up after the object
*/
localStreamSubscriber::~localStreamSubscriber()
{
}
