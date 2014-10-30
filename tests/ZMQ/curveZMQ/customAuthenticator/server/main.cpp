// Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include<future>
#include<thread>

void authenticatorProxyThread(void **inputContext, std::promise<bool> *inputPromise);

int main (void)
{
//Create context
void *context = zmq_ctx_new ();

//Make authenticator thread
//Create a promise so that we can find out if the thread was able to initialize (and if it has finished)
std::promise<bool> threadInitializationResult;


//Start a thread and pass it a this pointer
std::unique_ptr<std::thread> authenticatorProxy(new std::thread(authenticatorProxyThread, &context, &threadInitializationResult));

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
   fprintf(stderr, "%s\n", inputException.what());
}





// Socket to talk to clients
void *responder = zmq_socket (context, ZMQ_REP);
int rc0 = zmq_setsockopt (responder, ZMQ_ZAP_DOMAIN, "test", strlen ("test"));
int rc = zmq_bind (responder, "tcp://*:5555");
assert (rc == 0);

while (1) {
char buffer [10];
zmq_recv (responder, buffer, 10, 0);
printf ("Received Hello\n");
sleep (1); // Do some 'work'
zmq_send (responder, "World", 5, 0);
}
return 0;
}

void authenticatorProxyThread(void **inputContext, std::promise<bool> *inputPromise)
{
// Socket to talk to clients
void *responder = zmq_socket ((*inputContext), ZMQ_REP);
int rc = zmq_bind (responder, "inproc://zeromq.zap.01");
if(rc != 0)
{
inputPromise->set_value(false);
}
else
{
inputPromise->set_value(true); //Initialization completed successfully
}

int i=0;
while (1) {
char buffer [1000];
zmq_recv (responder, buffer, 1000, 0);
if(i<8)
{
printf ("Received authentication request message %d: %s\n", i, buffer);
}
//sleep (1); // Do some 'work'
//zmq_send (responder, "World", 5, 0);
i++;
}

printf("Authenticator is closing\n");
}

