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
This program is meant to be used with replierProcess.  It seaches for repliers that have tag "tag4" and sends requests to the first one it finds.
*/
int main(int argc, char **argv)
{
 
try
{
//Start the URI engine
localURIEngine::startEngine("../IPCDirectory/");

//Construct the query to find repliers with the "tag4" tag
localURIQuery myQuery1;
myQuery1.add_requiredtags("tag4");

//Send the query
std::vector<localURI> queryResults1;
queryResults1 = localURIEngine::getRepliers(myQuery1); //Thread-safe

//Print out the URIs that were found
printf("Matching URIs:\n");
for(int i=0; i<queryResults1.size(); i++)
{
printf("%s\n", uriToString(queryResults1[i]).c_str());
}

//Check to make sure at least one match was found
if(queryResults1.size() == 0)
{
fprintf(stderr, "Error, no repliers match required criteria\n");
return 1;
}

//Build a requester with the first matching URI
localRequester myRequester(queryResults1[0]);

//Send over 9000 requests
zmq::message_t messageBuffer;
for(int i=0; i<9001; i++)
{
//Build the request string
std::string requestString = "Request number " + std::to_string(i);

//Send request
printf("Sending request %s\n", requestString.c_str());
if(myRequester.sendRequest(requestString.c_str(), requestString.size()) != 1)
{
throw SOMException("Requester had a spot of trouble sending request\n", UNKNOWN, __FILE__, __LINE__);
}

//Restore the message buffer to a blank slate
messageBuffer.rebuild();

//Get the reply
if(myRequester.getReply(&messageBuffer) != 1)
{
throw SOMException("Requester had a spot of trouble getting reply\n", UNKNOWN, __FILE__, __LINE__);
}

//Print out what the replier sent
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
