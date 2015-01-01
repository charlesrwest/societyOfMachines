#include<stdio.h>
#include<unistd.h>

#include "virtualPortRouter.hpp"
#include "zmq.hpp"

int main(int argc, char **argv)
{
zmq::context_t context;
char buffer[512];

//Send a message from one port to the other
std::string messageToSend("I");
virtualPortMessageHeader messageHeader;
messageHeader.set_udpip(INADDR_LOOPBACK);
messageHeader.set_udpportnumber(9001);
messageHeader.set_destinationvirtualportid(9002);
messageHeader.set_transactionid(2000);

std::string serializedMessageHeader;
messageHeader.SerializeToString(&serializedMessageHeader);

printf("Ground truth header size: %ld\n", serializedMessageHeader.size());

uint32_t messageHeaderSize = htonl(serializedMessageHeader.size());
std::string packagedMessage(messageToSend + serializedMessageHeader + std::string((const char *) &messageHeaderSize, sizeof(messageHeaderSize)) );

//Copy to buffer
for(int i=0; i< packagedMessage.size(); i++)
{
buffer[i] = packagedMessage[i];
}

//Print out hex value of the original message
printf("Original message\n");
for (int i = 0; i < packagedMessage.size(); i++) 
{
     printf("%.2X", (unsigned char ) packagedMessage[i]);
}
printf("\n");

//Practice message retrieval
virtualPortMessageHeader messageHeaderBuffer;
if(getMessageHeaderFromVirtualPortMessage(buffer, packagedMessage.size(), messageHeaderBuffer) != 1)
{
fprintf(stderr, "Error retrieving message header\n");
}

printf("Retrieved header info: IPCheck: %d port: %d virtualPort: %d transactionID: %ld\n", messageHeader.udpip() == INADDR_LOOPBACK, messageHeader.udpportnumber(), messageHeader.destinationvirtualportid(), messageHeader.transactionid());

virtualPortMessageHeader messageHeader2;
messageHeader2.set_udpip(INADDR_LOOPBACK);
messageHeader2.set_udpportnumber(9001);
messageHeader2.set_destinationvirtualportid(9002);
messageHeader2.set_transactionid(2000);

messageHeader2.clear_udpip();
messageHeader2.clear_udpportnumber();

int resultSize = setMessageHeaderInVirtualPortMessage(buffer, packagedMessage.size(), 512, messageHeader2);

printf("Set version of message\n");
for (int i = 0; i < resultSize; i ++) 
{
     printf("%.2X", (unsigned char ) buffer[i]);
}
printf("\n");

printf("Re headdered result size %d\n", resultSize);

virtualPortMessageHeader messageHeaderBuffer3;
if(getMessageHeaderFromVirtualPortMessage(buffer, resultSize, messageHeaderBuffer3) != 1)
{
fprintf(stderr, "Error retrieving message header\n");
}

printf("Final header info: IPCheck: %d port: %d virtualPort: %d transactionID: %ld\n", messageHeaderBuffer3.udpip() == INADDR_LOOPBACK, messageHeaderBuffer3.udpportnumber(), messageHeaderBuffer3.destinationvirtualportid(), messageHeaderBuffer3.transactionid());


return 0;
}
