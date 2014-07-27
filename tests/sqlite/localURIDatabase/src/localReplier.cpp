#include "localReplier.hpp"


/*
@param inputReplierLocalURI: The local URI associated with the replier.  Having non-unique tags or keys associated with a URI or already containing the macro defined REPLIER_TAG tag will cause the database to reject the entry, which will result in an exception being thrown.
@param inputRequestTimeoutTimeInMilliseconds: How long (in milliseconds) the object should wait for the singleton to respond to its request before throwing an exception
@exceptions: This function will throw an exception if a ZMQ operation fails or the request to the database server is rejected
*/
localReplier::localReplier(const localURI &inputReplierLocalURI, uint32_t inputRequestTimeoutTimeInMilliseconds)
{

//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}



//Add tag marking the URI as a replier
replierURI = inputReplierLocalURI;
replierURI.add_tags(REPLIER_TAG);

//Send request to add URI
SOM_TRY
if(localURIEngine::addURI(replierURI) != 1)
{
throw SOMException("Error, local URI server refused add URI request\n", SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}
SOM_CATCH("Error, unable to register URI for localReplier\n")

//Create IPC socket
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REP));

replierSocket = std::move(scopeTransferPointer);
SOM_CATCH("Error, unable to create a socket for localReplier\n")

//Bind socket
SOM_TRY
replierSocket->bind(("ipc://"+localURIEngine::pathForMessagingDirectory+replierURI.resourcename()).c_str());
SOM_CATCH("Error, unable to bind socket for localReplier\n");
}

/*
This function gets a ZMQ request message from one of the requesters talking to this.   The sendReply function must be called to send a reply before this function can be used again (unless it is a multi part message).
@param inputMessageBuffer: The ZMQ message buffer to place the received result in
@return: 1 if the message was received successfully and 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int localReplier::getRequest(zmq::message_t *inputMessageBuffer)
{
if(inputMessageBuffer == NULL)
{
return 0;
}

SOM_TRY
return replierSocket->recv(inputMessageBuffer);
SOM_CATCH("Error receiving request from replier\n")
}

/*
This function sends a ZMQ request message to the requester associated with the previously received request.   The getRequest function must be called to get the reply before this function can be used again (unless sending a multi part message).
@param inputMessageData: A pointer to the data to be sent
@param inputMessageDataSize: The size of the data to be sent (bytes)
@param inputSendMessageMultipart: True if more pieces of the message are to follow
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if the zmq_send function returns with anything besides success or timeout
*/
int localReplier::sendReply(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart)
{
if(inputMessageData == nullptr  || inputMessageDataSize <= 0 )
{
return 0;
}

SOM_TRY
if(inputSetMessageMultipart == true)
{
return replierSocket->send(inputMessageData, inputMessageDataSize, ZMQ_SNDMORE) == inputMessageDataSize;
}
else
{
return replierSocket->send(inputMessageData, inputMessageDataSize, 0) == inputMessageDataSize;
}
SOM_CATCH("Error sending replier response\n")
}

/*
This function sends a ZMQ request message to the requester associated with the previously received request.   The getRequest function must be called to get the reply before this function can be used again (unless sending a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if the zmq_send function returns with anything besides success
*/
int localReplier::sendReply(zmq::message_t &inputMessage)
{
int messageSize = inputMessage.size();

SOM_TRY
return replierSocket->send(inputMessage) == messageSize;
SOM_CATCH("Error sending replier response\n")

return 0;
}

/*
This function cleans up the ZMQ socket associated with this object and requests for the associated URI to be removed from the server.  It does not throw exceptions but can print to stderr.
*/
localReplier::~localReplier()
{
try
{
if(localURIEngine::removeURI(replierURI.resourcename()) != 1)
{
fprintf(stderr, "Error, unable to remove URI associated with replier\n");
}
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}
}
