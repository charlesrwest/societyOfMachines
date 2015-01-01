#include<stdio.h>
#include<unistd.h>

#include "virtualPortRouter.hpp"
#include "virtualPortPairSocketWrapper.hpp"
#include "zmq.hpp"

int main(int argc, char **argv)
{
zmq::context_t context;

std::unique_ptr<virtualPortRouter> vRouter;

SOM_TRY
vRouter.reset(new virtualPortRouter(&context));
SOM_CATCH("Error initializing virtual port router\n")

//Initialize socket 1
std::unique_ptr<virtualPortPairSocketWrapper> socket1;

SOM_TRY
socket1.reset(new virtualPortPairSocketWrapper(&context, vRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing socket wrapper 1\n")

SOM_TRY
socket1->bind(9001);
SOM_CATCH("Error binding socket1\n")


//Initialize socket 2
std::unique_ptr<virtualPortPairSocketWrapper> socket2;

SOM_TRY
socket2.reset(new virtualPortPairSocketWrapper(&context, vRouter->virtualPortRouterInprocAddress));
SOM_CATCH("Error initializing socket wrapper 2\n")

SOM_TRY
socket2->bind(9002);
SOM_CATCH("Error binding socket2\n")

//Send a message from one port to the other
std::string messageToSend("We hold this truth to be self evident.  When you send a message addressed to yourself, you should get back what you sent");

printf("Message to send: %s\n", messageToSend.c_str());

//Send a message and get it back
SOM_TRY
if(socket1->send(messageToSend.c_str(), messageToSend.size(), INADDR_LOOPBACK, vRouter->datagramPortNumber, 9002, 2000) != 1)
{
fprintf(stderr, "Error, not able to send message via socket 1\n");
return -1;
}
SOM_CATCH("Error sending message\n")

//Get it back
int retrievedMessageContentSize;
zmq::message_t messageBuffer;
virtualPortMessageHeader virtualPortMessageHeaderBuffer;
SOM_TRY
retrievedMessageContentSize = socket2->recv(messageBuffer, virtualPortMessageHeaderBuffer);
SOM_CATCH("Error, problem getting the message back\n")

std::string receivedMessage((const char *) messageBuffer.data(), retrievedMessageContentSize);
printf("Retrieved message: %s\n", receivedMessage.c_str());

return 0;
}
