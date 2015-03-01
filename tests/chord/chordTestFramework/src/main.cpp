#include<stdio.h>
#include<unistd.h>

#include "chordNodeManager.hpp"

#include "fingerTable.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"

int main(int argc, char **argv)
{
/*
boost::multiprecision::uint512_t testNumber0("0x8000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000A");

fingerTable myFingerTable(testNumber0);

for(int i=0; i<myFingerTable.targetChordAddresses.size(); i++)
{
printf("0x%s\n", myFingerTable.targetChordAddresses[i].str(-1,std::ios::hex).c_str());
}
*/


std::unique_ptr<chordNodeManager> nodeManager;


try
{
printf("Hello world!!\n");
nodeManager.reset(new chordNodeManager("nodeManagerInprocAddress"));


for(int i=0; i<3; i++)
{
try
{
nodeManager->createChordNode(nodeManager->nodeContactInfos);
printf("Made node: %d\n", i);
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}
}

/*
printf("Pausing nodes\n");
nodeManager.pauseNodes();
printf("Nodes have been paused\n");

sleep(1);

printf("Resuming nodes\n");
nodeManager.resumeNodes();
printf("Nodes have been resumed\n");

printf("All done.  Destructor time!\n");
*/
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}

std::this_thread::sleep_for(std::chrono::milliseconds(8000));


//Get snapshot without pausing
//Print out what each node has in its finger table
int counter0 = 0;
for(auto iter = nodeManager->chordRingIDToNode.begin(); iter != nodeManager->chordRingIDToNode.end(); iter++)
{
printf("Node %d: %s\n", counter0, (*iter).second->nodeResourcesPointer->nodeFingerTable->getFingerTableSummary().c_str());
counter0++;
fflush(stdout);
}

//Send a request to get the node that owns a given address


//Create serialized version of big int
char buffer[64];
boost::multiprecision::uint512_t bigInt = 1001;
if(createRawHashFromUInt512(bigInt, buffer, 64) != 1)
{
fprintf(stderr, "Error serializing uint512\n");
}

std::string bufferStringBuffer = "Original target chord address: ";
std::string bufferString = convertUInt512ToDecimalString(bigInt);
bufferStringBuffer += bufferString.substr(0,5-(155-bufferString.size())) + "\n";
printf("%s\n", bufferStringBuffer.c_str());

//Create message to send
findAddressOwnerRequestOrResponse addressOwnerRequest;
addressOwnerRequest.set_target_chord_address(std::string(buffer, 64));
//updateRequest.set_internal_request_id(requestID);

boost::multiprecision::uint512_t bigBufferInt;
createUInt512FromRawHash(addressOwnerRequest.target_chord_address().c_str(), addressOwnerRequest.target_chord_address().size(), bigBufferInt);

bufferStringBuffer = "Original target chord address after back and forth: ";
bufferString = convertUInt512ToDecimalString(bigBufferInt);
bufferStringBuffer += bufferString.substr(0,5-(155-bufferString.size())) + "\n";
printf("%s\n", bufferStringBuffer.c_str());

//Serialize request
std::string serializedAddressOwnerRequest;
SOM_TRY
addressOwnerRequest.SerializeToString(&serializedAddressOwnerRequest);
SOM_CATCH("Error serializing message\n")

//Send message
//Setup socket
std::unique_ptr<zmq::socket_t> requestSocket;

SOM_TRY
requestSocket.reset(new zmq::socket_t(*(nodeManager->context), ZMQ_REQ));
SOM_CATCH("Error initializing request socket\n")

SOM_TRY
requestSocket->connect((nodeManager->chordRingIDToNode.begin())->second->findNodeAssociatedWithAddressInprocAddress.c_str());
SOM_CATCH("Error connecting request socket to "+(nodeManager->chordRingIDToNode.begin())->second->findNodeAssociatedWithAddressInprocAddress+" \n")

/*
SOM_TRY
//Send message
requestSocket->send(serializedAddressOwnerRequest.c_str(), serializedAddressOwnerRequest.size());
SOM_CATCH("Error sending address owner request\n")

zmq::message_t messageBuffer;
SOM_TRY
//receive reply message
requestSocket->recv(&messageBuffer);
SOM_CATCH("Error receiving address owner response\n")


findAddressOwnerRequestOrResponse response;
response.ParseFromArray(messageBuffer.data(), messageBuffer.size());

SOM_TRY
if(!response.IsInitialized())
{
return 1; //Message header serialization failed, so ignore this message
}
SOM_CATCH("Error reading message\n")

if(!response.has_chord_address_owner_contact_info())
{
printf("Request for address owner failed!!!!!!!!!!!!!!!!\n");
}
else
{
printf("We got a response!!!!!!!!!!!!!!!!\n");
}

*/

std::this_thread::sleep_for(std::chrono::milliseconds(80000));
printf("Pausing nodes\n");
nodeManager->pauseNodes();
printf("Paused nodes\n");

//Print out what each node has in its finger table
int counter = 0;
for(auto iter = nodeManager->chordRingIDToNode.begin(); iter != nodeManager->chordRingIDToNode.end(); iter++)
{
printf("Node %d: %s\n", counter, (*iter).second->nodeResourcesPointer->nodeFingerTable->getFingerTableSummary().c_str());
counter++;
fflush(stdout);
}



while(true)
{
printf("Sleeping main thread!\n");
std::this_thread::sleep_for(std::chrono::milliseconds(30000));
}
return 0;
}
