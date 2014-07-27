#include "serverSingleton.hpp"

std::string *serverSingleton::engineInProcAddress = nullptr;
zmq::context_t *serverSingleton::ZMQContext = nullptr;

/*
This function creates a singleton for the class
*/
void serverSingleton::initializeSingleton()
{
static serverSingleton instance;  //Declares a single instance which goes out of scope after main
}

/*
This function attempts to ping the server 10 times
*/
void serverSingleton::sendRequestToServer()
{
//Set up requester
zmq::socket_t *requester;

//Make socket to use for connection
SOM_TRY
requester = new zmq::socket_t(*serverSingleton::ZMQContext, ZMQ_REQ); //Request socket
SOM_CATCH("Error constructing ZMQ socket for requester\n")

SOMScopeGuard requesterSocketGuard([&](){delete requester;});

printf ("Connecting to hello world server…\n");

SOM_TRY
requester->connect(serverSingleton::engineInProcAddress->c_str());
SOM_CATCH("Error, unable to connect to server\n")

int request_nbr;
for (request_nbr = 0; request_nbr != 10; request_nbr++) 
{
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

}

/*
This function constructs the singleton instance of the class and should only be invoked by the startEngine function.  Specifically, it carries out almost all of the functionality of initializeSingleton.
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
serverSingleton::serverSingleton()
{
//Make context
SOM_TRY
serverSingleton::ZMQContext = &engineContext;
SOM_CATCH("Error creating ZMQ context\n")
//SOMScopeGuard contextGuard([&](){delete contextPointer;});


//Start server thread
std::promise<bool> threadInitializationResult;
std::thread *myThread;

SOM_TRY
myThread = new std::thread(serverFunction, &threadInitializationResult);
SOM_CATCH("Error, unable to create thread\n");
//SOMScopeGuard threadGuard([&](){delete myThread;});

//Wait for server thread to set up
bool result;
SOM_TRY
result = threadInitializationResult.get_future().get();
SOM_CATCH("Error getting result from promise\n")
}

/*
This function is called in a thread to act as the inproc server.
@param inputThreadInitializationResult: A promise object used to signal when the server functionality initialization is complete.
*/
void serverFunction(std::promise<bool> *inputThreadInitializationResult)
{
try
{
// Socket to talk to clients
zmq::socket_t *responder;

//Make socket to use for connection
SOM_TRY
responder = new zmq::socket_t(*serverSingleton::ZMQContext, ZMQ_REP); //Reply socket
SOM_CATCH("Error constructing ZMQ socket for responder\n")

SOMScopeGuard responderSocketGuard([&](){delete responder;});

SOM_TRY
responder->bind("inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS"); //bind socket
serverSingleton::engineInProcAddress = new std::string("inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS");
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

}
