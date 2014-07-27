// Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include<thread>
#include<future>

void threadFunction(void *inputContext, std::promise<bool> *inputThreadInitializationResult)
{
// Socket to talk to clients
void *responder = zmq_socket (inputContext, ZMQ_REP);
int rc = zmq_bind (responder, "inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS");
//assert (rc == 0);

//printf("Socket has been bound\n");
inputThreadInitializationResult->set_value(true); //Initialization completed successfully
for(int i=0; i<10; i++)
{
char buffer [10];
//printf("Waiting for message\n");
zmq_recv (responder, buffer, 10, 0);
printf ("Received Hello\n");
sleep (1); // Do some 'work'
zmq_send (responder, "World", 5, 0);
}
printf("Exited loop\n");
zmq_close (responder);

return;
}

int main (void)
{
void *context = zmq_ctx_new ();
void **contextPointer=&context;

//Start server thread
std::promise<bool> threadInitializationResult;
std::thread myThread(threadFunction, *contextPointer, &threadInitializationResult);

bool result = threadInitializationResult.get_future().get();

printf ("Connecting to hello world server…\n");

void *requester = zmq_socket (*contextPointer, ZMQ_REQ);
zmq_connect (requester, "inproc://LOCAL_URI_ENGINE_IN_PROC_ADDRESS");

int request_nbr;
for (request_nbr = 0; request_nbr != 10; request_nbr++) {
char buffer [10];
printf ("Sending Hello %d…\n", request_nbr);
zmq_send (requester, "Hello", 5, 0);
zmq_recv (requester, buffer, 10, 0);
printf ("Received World %d\n", request_nbr);
}

printf("Closing socket and context\n");
zmq_close (requester);

printf("Joining\n");
myThread.join();
printf("Join completed\n");

zmq_ctx_destroy (*contextPointer);

//sleep(100);



return 0;
}

/*
//Get the IPC address for a the URI server associated with the directory we are using
std::string directoryURIServerAddress = ensureDirectoryURIServerSingletonIsRunningAndGetAddress(inputPathForMessagingDirectory);

printf("I got a string! %s\n", directoryURIServerAddress.c_str());

//Create a promise so that we can find out if the thread was able to initialize (and if it has finished)
std::promise<bool> threadInitializationResult;


//Start a thread and pass it a this pointer
serverThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunLocalURIProxy, this, directoryURIServerAddress, &threadInitializationResult));

//This should block until the server thread has been initialized and throw any exceptions that the server thread threw during its initialization
try
{
   if(threadInitializationResult.get_future().get() == true)
   {
      printf("Thread reports that it initialized successfully\n");
   }
}
catch(const std::exception &inputException)
{
   printf("%s\n", inputException.what());
}
*/

