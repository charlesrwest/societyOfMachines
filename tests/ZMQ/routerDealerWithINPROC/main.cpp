#include<cstdio>
#include<memory>

#include<zmq.hpp>
#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"


int main(int argc, char **argv)
{
//Create ZMQ context
std::unique_ptr<zmq::context_t> context;
SOM_TRY
context.reset(new zmq::context_t());
SOM_CATCH("Error, unable to create ZMQ context\n")

//Allocate and bind router socket
std::unique_ptr<zmq::socket_t> routerSocket;

SOM_TRY
routerSocket.reset(new zmq::socket_t((*context), ZMQ_ROUTER));
SOM_CATCH("Error, unable to create router socket\n")

SOM_TRY
routerSocket->bind("inproc://routerSocketAddress");
SOM_CATCH("Error, unable to bind router socket\n")

//Allocate and connect dealer socket
std::unique_ptr<zmq::socket_t> dealerSocket;

SOM_TRY
dealerSocket.reset(new zmq::socket_t((*context), ZMQ_DEALER));
SOM_CATCH("Error, unable to create dealer socket\n")

SOM_TRY
dealerSocket->connect("inproc://routerSocketAddress");
SOM_CATCH("Error, unable to connect dealer socket\n")

//Send router a message using the dealer
std::string originalMessage("A message sent to a well behaved echo client should be returned unchanged.\n");

SOM_TRY
//Send empty delimiter since we are using a dealer socket
dealerSocket->send(NULL, 0, ZMQ_SNDMORE);
//Send message
dealerSocket->send(originalMessage.c_str(), originalMessage.size());
SOM_CATCH("Error sending finger table update request\n")

//Retrieve connection id associated with dealer message and retrieve/show message
std::unique_ptr<zmq::message_t> ZMQMessageBuffer;
std::vector<std::unique_ptr<zmq::message_t> > messageParts;
int64_t morePartsFlag = 1;

//Get multi part message and store it in a vector
while(morePartsFlag == 1)
{
//Allocate message buffer
SOM_TRY
ZMQMessageBuffer.reset(new zmq::message_t());
SOM_CATCH("Error allocating message buffer\n")

SOM_TRY
routerSocket->recv(ZMQMessageBuffer.get());
SOM_CATCH("Error receiving message\n")

messageParts.push_back(std::move(ZMQMessageBuffer)); //Add to vector

auto morePartsFlagSize = sizeof(morePartsFlag);
SOM_TRY
routerSocket->getsockopt(ZMQ_RCVMORE, &morePartsFlag, &morePartsFlagSize);
SOM_CATCH("Error determining if message has more parts\n")
}

//Find empty part in multi part message and place index at the next message 
int index;
for(int i=0; i< messageParts.size(); i++)
{
if(messageParts[i]->size() == 0)
{
index = i+1;
}
}

if(index == 0 || index >= messageParts.size())
{
return 1; //There was no request message and/or connection ID, so the value pointed at by the index is invalid or the connection ID can't be used
}

//Print out the message
for(int i=0; i < (messageParts.size()-index); i++)
{
printf("Received: %s\n", (const char *) messageParts[i+index]->data());
}

//Echo back
for(int i=0; i<index; i++) //Send addresses/connection IDs and delimiter
{
SOM_TRY
routerSocket->send(messageParts[i]->data(), messageParts[i]->size(), ZMQ_SNDMORE);
SOM_CATCH("Error, unable to send message to echoed back\n")
}

for(int i=0; i < (messageParts.size()-index); i++) //Send message(s)
{
routerSocket->send(messageParts[index+i]->data(), messageParts[index+i]->size());
}

//Recieve at dealer side
std::unique_ptr<zmq::message_t> ZMQMessageBuffer2;
std::vector<std::unique_ptr<zmq::message_t> > messageParts2;
int64_t morePartsFlag2 = 1;
int index2;

//Get multi part message and store it in a vector
while(morePartsFlag2 == 1)
{
//Allocate message buffer
SOM_TRY
ZMQMessageBuffer2.reset(new zmq::message_t());
SOM_CATCH("Error allocating message buffer2\n")

SOM_TRY
dealerSocket->recv(ZMQMessageBuffer2.get());
SOM_CATCH("Error receiving message2\n")

messageParts2.push_back(std::move(ZMQMessageBuffer2)); //Add to vector

auto morePartsFlagSize2 = sizeof(morePartsFlag2);
SOM_TRY
dealerSocket->getsockopt(ZMQ_RCVMORE, &morePartsFlag2, &morePartsFlagSize2);
SOM_CATCH("Error determining if message has more parts2\n")
}

for(int i=0; i< messageParts2.size(); i++)
{
if(messageParts2[i]->size() == 0)
{
index2 = i+1;
}
}

if(index2 == 0 || index2 >= messageParts2.size())
{
return 1; //There was no request message and/or connection ID, so the value pointed at by the index is invalid or the connection ID can't be used
}

//Print out the message
for(int i=0; i < (messageParts2.size()-index2); i++)
{
printf("Received echoed: %s\n", (const char *) messageParts2[i+index2]->data());
}

}
