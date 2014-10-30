#include "datagramRouter.hpp"

/*
This function constructs the object and spawns the associated thread.
@param inputZMQContext: The ZMQ context to use for inproc communications.  If specified as NULL, the datagram router makes its own context
@exceptions: This object can throw exceptions
*/
datagramRouter::datagramRouter(zmq::context_t *inputZMQContext) : datagramSocket(nullptr, nullptr)
{
//Make sure context pointer is setup
if(inputZMQContext == NULL)
{
//Context pointer was null, so we need to make one
SOM_TRY
contextRAII.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

//Set the public pointer
context = contextRAII.get();
}
else
{
context = inputZMQContext;
}

//Setup the datagram socket


//Create descriptor
int datagramSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM,0);
if(datagramSocketFileDescriptor <= 0)
{
throw SOMException(std::string("Error initializing datagram socket for node\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}

//Make sure socket gets cleaned up when the object goes out of scope
std::unique_ptr<int, void(*)(int*) > datagramSocketBuffer(new int(datagramSocketFileDescriptor), [](int *inputPointer){ delete inputPointer; close(*inputPointer);});
datagramSocket = std::move(datagramSocketBuffer); 

//Try to bind until we find a port we can use 1024 to 49151
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distribution(1024, 49151);

for(int i=1024; i<49151; i++)
{
int portToTry = distribution(gen);
struct sockaddr_in datagramSocketAddress;

memset(&datagramSocketAddress, 0, sizeof(datagramSocketAddress));
datagramSocketAddress.sin_family = AF_INET;
datagramSocketAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
datagramSocketAddress.sin_port = htons(portToTry); 

if(bind(*datagramSocket, (struct sockaddr *) &datagramSocketAddress, sizeof(datagramSocketAddress)) == 0)
{
portNumber = portToTry;
break;
}

if(i==49150)
{
throw SOMException(std::string("Error, unable to bind any ports despite trying 49151 random ones\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
}

//Fire up the thread (which completes the rest of the initialization
timeToShutdownFlag = false;
std::promise<bool> threadInitializationResult;

//Start a thread and pass it a this pointer
routerThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunDatagramRouter, this, &threadInitializationResult));

//This should block until the server thread has been initialized and throw any exceptions that the server thread threw during its initialization
if(threadInitializationResult.get_future().get() == true)
{
   //printf("Thread reports that it initialized successfully\n");
}

}

/*
The sendto interface for the datagram socket associated with the datagram router.  This function is threadsafe.
@param inputMessage: The data to send
@param inputMessageLength: The number of bytes to send
@param inputFlags: UDP flags
@param inputDestinationAddress: A pointer to the destination address
@param inputDestinationAddressLength: How long the destination address structure is
@return: On local success, returns the number of bytes sent.  -1 is returned otherwise
*/
ssize_t datagramRouter::sendDatagram(const void *inputMessage, size_t inputMessageLength, int inputFlags, const struct sockaddr *inputDestinationAddress, socklen_t inputDestinationAddressLength)
{
return sendto(*datagramSocket, inputMessage, inputMessageLength, inputFlags, inputDestinationAddress, inputDestinationAddressLength);
}

/*
This function signals for the thread to shut down and then waits for it to do so.
*/
datagramRouter::~datagramRouter()
{
timeToShutdownFlag = true;
routerThread->join();
}

/*
This is the function that is called in a seperate thread by the datagramRouter object.
@param inputPromise: The promise used to signal when initialization has been completed and whether it was successful (accessing the promise value throws any exceptions that occurred in the initialization process
*/
void initializeAndRunDatagramRouter(datagramRouter *inputDatagramRouter, std::promise<bool> *inputPromise)
{
//Initialize the various inproc sockets

inputPromise->set_value(true); //Initialization completed successfully

while(true)
{
//Wait for datagrams and send them to the associated inproc sockets

//Check for shutdown flag
if(inputDatagramRouter->timeToShutdownFlag == true)
{
//Shut down
return;
}

}

}

