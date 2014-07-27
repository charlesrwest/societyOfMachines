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
#include "serverSingleton.hpp"


int main (void)
{
try
{

serverSingleton::initializeSingleton();

/*
This function attempts to ping the server 10 times
*/
serverSingleton::sendRequestToServer();

//sleep(100);

}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s", inputException.what());
}

return 0;
}



