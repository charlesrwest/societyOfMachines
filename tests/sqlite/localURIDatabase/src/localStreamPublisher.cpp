#include "localStreamPublisher.hpp"

/*
@param inputStreamLocalURI: The local URI associated with the stream.  Having non-unique tags or keys associated with a URI or already containing the PUBLISHER_TAG tag will cause the database to reject the entry, which will result in an exception being thrown.
@exceptions: This function will throw an exception if a ZMQ operation fails or the engine is invalid
*/
localStreamPublisher::localStreamPublisher(const localURI &inputStreamLocalURI)
{
//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Add tag marking the URI as a replier
associatedURI = inputStreamLocalURI;
associatedURI.add_tags(PUBLISHER_TAG);

//Send request to add URI
SOM_TRY
if(localURIEngine::addURI(associatedURI) != 1)
{
throw SOMException("Error, local URI server refused add URI request\n", SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}
SOM_CATCH("Error, unable to register URI for localPublisher\n")

//Create IPC socket
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_PUB));

publisherSocket = std::move(scopeTransferPointer);
SOM_CATCH("Error, unable to create a socket for localReplier\n")

//Bind socket
SOM_TRY
publisherSocket->bind(("ipc://"+localURIEngine::pathForMessagingDirectory+associatedURI.resourcename()).c_str());
SOM_CATCH("Error, unable to bind socket for localPublisher\n");
}

/*
This function publishes a ZMQ message associated with this object.
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 otherwise
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int localStreamPublisher::publish(zmq::message_t &inputMessageBuffer)
{
int messageSize = inputMessageBuffer.size();

SOM_TRY
return publisherSocket->send(inputMessageBuffer) == messageSize;
SOM_CATCH("Error sending publisher message\n")

return 0;
}

/*
This function publishes a ZMQ message associated with this object.
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@param inputSetMessageMultipart: True if there are more parts of a multi part message to follow
@return: 1 if the message was sent successfully and 0 otherwise
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int localStreamPublisher::publish(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart)
{
if(inputMessageData == nullptr  || inputMessageDataSize <= 0 )
{
return 0;
}

SOM_TRY
if(inputSetMessageMultipart == true)
{
return publisherSocket->send(inputMessageData, inputMessageDataSize, ZMQ_SNDMORE) == inputMessageDataSize;
}
else
{
return publisherSocket->send(inputMessageData, inputMessageDataSize) == inputMessageDataSize;
}
SOM_CATCH("Error sending publisher message\n")
}

/*
This function cleans up after the object
*/
localStreamPublisher::~localStreamPublisher()
{
try
{
if(localURIEngine::removeURI(associatedURI.resourcename()) != 1)
{
fprintf(stderr, "Error, unable to remove URI associated with streamPublisher\n");
}
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}

}
