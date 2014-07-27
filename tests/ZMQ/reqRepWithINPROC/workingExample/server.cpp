// Hello World server

#include <zmq.hpp>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include<thread>
#include<future>
#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

void serverFunction(zmq::context_t *inputContext, std::promise<bool> *inputThreadInitializationResult)
{
try
{
// Socket to talk to clients
zmq::socket_t *responder;

//Make socket to use for connection
SOM_TRY
responder = new zmq::socket_t(*inputContext, ZMQ_REP); //Reply socket
SOM_CATCH("Error constructing ZMQ socket for responder\n")

SOMScopeGuard responderSocketGuard([&](){delete responder;});

SOM_TRY
responder->bind("inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS"); //bind socket
SOM_CATCH("Error binding socket for responder\n")

printf("Socket has been bound\n");

//inputThreadInitializationResult->set_value(true); //Initialization completed successfully

for(int i=0; i<10; i++)
{
char buffer [10];

if(i==0)
{
inputThreadInitializationResult->set_value(true); //Initialization completed successfully
}

//SOM_TRY
try
{
responder->recv(buffer, 10, 0); //Get reply
}
catch(const std::exception &inputException)
{
fprintf(stderr, "Exception caught: %s\n", inputException.what());
}
//SOM_CATCH("Error receiving client request\n")



printf("Server working\n");
sleep (1); // Do some 'work'

SOM_TRY
responder->send("World", 5, 0); //Send request
SOM_CATCH("Error sending reply message\n")
}
printf("Exited loop\n");
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
} 

return;
}

int main (void)
{
try
{
//Make context
zmq::context_t *contextPointer;

SOM_TRY
contextPointer = new zmq::context_t;
SOM_CATCH("Error creating ZMQ context\n")
SOMScopeGuard contextGuard([&](){delete contextPointer;});


//Start server thread
std::promise<bool> threadInitializationResult;
std::thread *myThread;

SOM_TRY
myThread = new std::thread(serverFunction, contextPointer, &threadInitializationResult);
SOM_CATCH("Error, unable to create thread\n");
SOMScopeGuard threadGuard([&](){delete myThread;});

//Wait for server thread to set up
bool result;
SOM_TRY
result = threadInitializationResult.get_future().get();
SOM_CATCH("Error getting result from promise\n")


//Set up requester
zmq::socket_t *requester;

//Make socket to use for connection
SOM_TRY
requester = new zmq::socket_t(*contextPointer, ZMQ_REQ); //Request socket
SOM_CATCH("Error constructing ZMQ socket for requester\n")

SOMScopeGuard requesterSocketGuard([&](){delete requester;});

printf ("Connecting to hello world server…\n");

SOM_TRY
requester->connect("inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS");
SOM_CATCH("Error, unable to connect to server\n")

int request_nbr;
for (request_nbr = 0; request_nbr != 10; request_nbr++) {
char buffer [10];
printf ("Sending Hello %d…\n", request_nbr);

SOM_TRY
requester->send("Hello", 5, 0); //Send request
SOM_CATCH("Error sending requester message\n")

SOM_TRY
requester->recv(buffer, 10, 0); //Get reply
SOM_CATCH("Error receiving reply to requester\n")

printf ("Received World %d\n", request_nbr);
}

printf("Joining\n");
myThread->join();
printf("Join completed\n");

//sleep(100);

}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}

return 0;
}



