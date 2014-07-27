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
This example is meant to be used with the subscriberProcess example.  It registers two publishers with URIs describing them, which the subscriber process can then search through and subscribe to.
*/
int main(int argc, char **argv)
{


try
{
//Initialize the URI engine and give it the directory to use for communication
localURIEngine::startEngine("../IPCDirectory/");

//Build a URI to describe a publisher
localURI publisherURI0;
publisherURI0.set_resourcename("PublisherURI0ResourceName");
publisherURI0.set_ipcpath("PublisherURI0IPCPath"); //Not used
publisherURI0.set_originatingprocessid(getpid());  //Won't be used once soft state is in place, also could be set inside publisher

//Add tags identifying the publisher
publisherURI0.add_tags("tag0");
publisherURI0.add_tags("tag1");
publisherURI0.add_tags("tag2");
publisherURI0.add_tags("tag3");
publisherURI0.add_tags("tag4");

//Use the URI to build a publisher object
localStreamPublisher publisher0(publisherURI0);


//Build a URI that describes a second publisher
localURI publisherURI1;
publisherURI1.set_resourcename("PublisherURI1ResourceName");
publisherURI1.set_ipcpath("PublisherURI1IPCPath"); //Not used
publisherURI1.set_originatingprocessid(getpid());  //Won't be used once soft state is in place, also could be set inside publisher

//Add tags identifying the publisher
publisherURI1.add_tags("tag4");
publisherURI1.add_tags("tag5");
publisherURI1.add_tags("tag6");
publisherURI1.add_tags("tag7");
publisherURI1.add_tags("tag8");

//Build the second publisher with the second URI
localStreamPublisher publisher1(publisherURI1);

//Publish over 9000 messages on both publishers
for(int i=0; i<9001; i++)
{
std::string publisher0String = std::string("Publisher 0: Update ") + std::to_string(i);
std::string publisher1String = std::string("Publisher 1: Update ") + std::to_string(i);

if(publisher0.publish(publisher0String.c_str(), publisher0String.size()) != 1)
{
throw SOMException("Publisher 0 had some trouble publishing\n", UNKNOWN, __FILE__, __LINE__);
}

if(publisher1.publish(publisher1String.c_str(), publisher1String.size()) != 1)
{
throw SOMException("Publisher 1 had some trouble publishing\n", UNKNOWN, __FILE__, __LINE__);
}

//Sleep for a little while so the messages aren't all published at once
sleep(1);
}


}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}

//Close down (publishers go out of scope without the process being immediately killed and get a chance to clean up after themselves)
return 0;
}
