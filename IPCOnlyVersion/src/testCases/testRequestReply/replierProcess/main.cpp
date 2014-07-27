#include<stdio.h>
#include "localURIDatabase.h"
#include "utilityFunctions.h"
#include<vector>
#include<string>
#include<exception>

#include<unistd.h>

#include "localURIEngine.hpp"
#include "SOMScopeGuard.hpp"
#include "localReplier.hpp"
#include "localRequester.hpp"
#include "SOMException.hpp"

/*
This program is meant to be used with requesterProcess.  It sets up a replier that publishes a URI describing it and replies to messages that are sent to by the requesterProcess.
*/
int main(int argc, char **argv)
{

try
{
//Start the URI engine
localURIEngine::startEngine("../IPCDirectory/");

//Create the URI describing the replier
localURI replierURI;
replierURI.set_resourcename("replierURIResourceName");
replierURI.set_ipcpath("replierURIIPCPath"); //Not used
replierURI.set_originatingprocessid(getpid());  //Won't be used once soft state is in place, also could be set inside publisher

//Add the associated tags
replierURI.add_tags("tag0");
replierURI.add_tags("tag1");
replierURI.add_tags("tag2");
replierURI.add_tags("tag3");
replierURI.add_tags("tag4");

//Create the replier with the URI
localReplier replier(replierURI);

//Receive and respond to over 9000 messages
zmq::message_t messageBuffer;
for(int i=0; i<9001; i++)
{
//Reset the message buffer (needs to be done after each time it is used in a ZMQ operation)
messageBuffer.rebuild();

//Get a request message
if(replier.getRequest(&messageBuffer) != 1)
{
throw SOMException("Replier had some trouble getting a request\n", UNKNOWN, __FILE__, __LINE__);
}

//Print out the received request
printf("Got request: ");
fwrite(messageBuffer.data(), 1, messageBuffer.size(), stdout);
printf("\n");

//Sleep to pretend we are working
sleep(1);

//Send the reply
std::string myString = std::string("Reply ") + std::to_string(i);
printf("Sending reply: %s\n", myString.c_str());
if(replier.sendReply(myString.c_str(), myString.size()) != 1)
{
throw SOMException("Replier had some trouble sending a reply\n", UNKNOWN, __FILE__, __LINE__);
}

}


}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}


return 0;
}
