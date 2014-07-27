#include<stdio.h>
#include "localURIDatabase.h"
#include "utilityFunctions.h"
#include<vector>
#include<string>
#include<exception>

#include<unistd.h>

#include "localURIEngine.hpp"
#include "SOMScopeGuard.hpp"
#include "localStreamPublisher.hpp"
#include "localStreamSubscriber.hpp"
#include "SOMException.hpp"

/*
This program is meant to be used with publisherProcess.  It searches for publishers with the tag "tag4", subscribes to them and prints out the messages they publish. 
*/

int main(int argc, char **argv)
{

//Initialize inproc local URI server proxy 
try
{
//Start the URI engine
localURIEngine::startEngine("../IPCDirectory/");

//Make a query that looks for publishers with a tag of value "tag4"
localURIQuery myQuery1;
myQuery1.add_requiredtags("tag4");

std::vector<localURI> queryResults1;

//Use the URI engine to find matching publishers (this is threadsafe)
queryResults1 = localURIEngine::getPublishers(myQuery1);

//Make a subscriber and print/subscribe to each of the matching publishers
localStreamSubscriber mySubscriber;

printf("Matching URIs:\n");
for(int i=0; i<queryResults1.size(); i++)
{
//Print out the URI so we know what it is subscribing to
printf("%s\n", uriToString(queryResults1[i]).c_str());

//Subscribe
mySubscriber.subscribe(queryResults1[i]);
}

//Receive any published messages
zmq::message_t messageBuffer;
for(;;)
{
//Restore the message buffer to a blank slate
messageBuffer.rebuild();

if(mySubscriber.receiveMessage(&messageBuffer) != 1)
{
throw SOMException("Subscriber had a spot of trouble\n", UNKNOWN, __FILE__, __LINE__);
}

//Print out what we got
printf("Received: ");
fwrite(messageBuffer.data(), 1, messageBuffer.size(), stdout);
printf("\n");

}

}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}


return 0;
}
