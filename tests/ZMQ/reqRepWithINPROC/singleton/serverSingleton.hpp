#ifndef SERVERSINGLETONHPP
#define SERVERSINGLETONHPP

#include <zmq.hpp>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include<thread>
#include<future>
#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

class serverSingleton
{
public:

/*
This function creates a singleton for the class
*/
static void initializeSingleton();

/*
This function attempts to ping the server 10 times
*/
static void sendRequestToServer();

//The inproc address of the server proxy engine
static std::string *engineInProcAddress;
static zmq::context_t *ZMQContext; 

zmq::context_t engineContext;         //Initialization of member classes happens in order of listing, context must be higher than listing

friend void serverFunction(std::promise<bool> *inputThreadInitializationResult);

private:

/*
This function constructs the singleton instance of the class and should only be invoked by the startEngine function.  Specifically, it carries out almost all of the functionality of initializeSingleton.
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
serverSingleton();


};

/*
This function is called in a thread to act as the inproc server.
@param inputThreadInitializationResult: A promise object used to signal when the server functionality initialization is complete.
*/
void serverFunction(std::promise<bool> *inputThreadInitializationResult);









#endif
