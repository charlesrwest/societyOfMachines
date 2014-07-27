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
This function tests the basic URI searching functionality by creating two publishers and then searching for their URIs with a few different criteria.
*/
int main(int argc, char **argv)
{

try
{
//Initialize the URI engine
localURIEngine::startEngine("./IPCDirectory/");

//Make a URI to describe the first publisher
localURI publisherURI0;
publisherURI0.set_resourcename("PublisherURI0ResourceName");
publisherURI0.set_ipcpath("PublisherURI0IPCPath"); //Not used
publisherURI0.set_originatingprocessid(getpid());  //Won't be used once soft state is in place, also could be set inside publisher

//Add the associated tags
publisherURI0.add_tags("tag0");
publisherURI0.add_tags("tag1");
publisherURI0.add_tags("tag2");
publisherURI0.add_tags("tag3");
publisherURI0.add_tags("tag4");

//Use the URI to make a publisher
localStreamPublisher publisher0(publisherURI0);

//Make a URI to describe the second publisher
localURI publisherURI1;
publisherURI1.set_resourcename("PublisherURI1ResourceName");
publisherURI1.set_ipcpath("PublisherURI1IPCPath"); //Not used
publisherURI1.set_originatingprocessid(getpid());  //Won't be used once soft state is in place, also could be set inside publisher

//Add the associated tags
publisherURI1.add_tags("tag4");
publisherURI1.add_tags("tag5");
publisherURI1.add_tags("tag6");
publisherURI1.add_tags("tag7");
publisherURI1.add_tags("tag8");

//Use the new URI to make the second publisher
localStreamPublisher publisher1(publisherURI1);

//Make a query to find publishers with tag "tag4" that don't have tag "tag6"
localURIQuery myQuery;
myQuery.add_requiredtags("tag4");
myQuery.add_forbiddentags("tag6");

//Get matching URIs
std::vector<localURI> queryResults;
queryResults = localURIEngine::getPublishers(myQuery); //Thread-safe

//Print out each of the returned URIs
printf("Matching URIs:\n");
for(int i=0; i<queryResults.size(); i++)
{
printf("%s\n", uriToString(queryResults[i]).c_str());
}

//Now make a query that returns all of the URIs that have tag "tag4"
localURIQuery myQuery1;
myQuery1.add_requiredtags("tag4");

std::vector<localURI> queryResults1;
queryResults1 = localURIEngine::getPublishers(myQuery1); //Thread safe

//Print out the results of the second query
printf("Matching URIs:\n");
for(int i=0; i<queryResults1.size(); i++)
{
printf("%s\n", uriToString(queryResults1[i]).c_str());
}

}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}

return 0;
}
