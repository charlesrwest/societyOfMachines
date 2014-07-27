#include "localRequester.hpp"

/*
This function creates an object that tries to connect to the given replier.
@param inputReplierLocalURI: The local URI associated with the replier this object is used to talk to.
@exceptions: This function will throw an exception if the inproc proxy isn't initialized or the object was unable to connect to the replier
*/
localRequester::localRequester(const localURI &inputReplierLocalURI) : localRequester(inputReplierLocalURI.resourcename())
{
}

/*
This function creates an object that tries to connect to the given replier.
@param inputReplierResourceName: The resource name associated with the replier this object is used to talk to.
@exceptions: This function will throw an exception if the inproc proxy isn't initialized or the object was unable to connect to the replier
*/
localRequester::localRequester(const std::string &inputReplierResourceName)
{
//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Create IPC socket
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REQ));

requesterSocket = std::move(scopeTransferPointer);
SOM_CATCH("Error, unable to create a socket for localRequester\n")

//Connect to replier
SOM_TRY
requesterSocket->connect(("ipc://"+localURIEngine::pathForMessagingDirectory+inputReplierResourceName).c_str());
SOM_CATCH("Error connecting requester socket to replier\n")
}

/*
This function sends a ZMQ request message to the replier associated with this object.   The getReply function must be called to get the reply before this function can be used again (unless it is a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 otherwise
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int localRequester::sendRequest(zmq::message_t &inputMessage)
{
int messageSize = inputMessage.size();

SOM_TRY
return requesterSocket->send(inputMessage) == messageSize;
SOM_CATCH("Error unable to send request to replier\n")
}

/*
This function sends a ZMQ request message to the replier associated with this object.   The getReply function must be called to get the reply before this function can be used again (unless it is a multi part message).
@param inputMessage: The ZMQ message to send (the message will be cleared as part of the sending, unless an error occurs)
@return: 1 if the message was sent successfully and 0 if the function was nonblocking and there wasn't sufficient space
@exceptions: Exceptions will be thrown if a ZMQ call fails
*/
int localRequester::sendRequest(const char *inputMessageData, int inputMessageDataSize, bool inputSetMessageMultipart)
{
if(inputMessageData == nullptr || inputMessageDataSize <= 0)
{
return 0;
}

SOM_TRY
if(inputSetMessageMultipart)
{
return requesterSocket->send(inputMessageData, inputMessageDataSize, ZMQ_SNDMORE) == inputMessageDataSize;
}
else
{
return requesterSocket->send(inputMessageData, inputMessageDataSize) == inputMessageDataSize;
}
SOM_CATCH("Error unable to send request to replier\n")
}

/*
This function gets a ZMQ request message from the replier associated with this object.   The sendRequest function must be called to send a request before this function can be used again (unless it is a multi part message).
@param inputMessageBuffer: The ZMQ message buffer to place the message in
@return: 1 if the message was received successfully and 0 otherwise
@exceptions: Exceptions will be thrown if there is a ZMQ failure
*/
int localRequester::getReply(zmq::message_t *inputMessageBuffer)
{
if(inputMessageBuffer == NULL)
{
return 0;
}

SOM_TRY
return requesterSocket->recv(inputMessageBuffer);
SOM_CATCH("Error receiving reply to requester\n")
}


/*
This function lets the parent know that it that is being deleted, so any resources associated with it should be freed.
*/
localRequester::~localRequester()
{
}
