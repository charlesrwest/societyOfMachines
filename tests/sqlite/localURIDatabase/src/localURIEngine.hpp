#ifndef LOCALURIAPIH
#define LOCALURIAPIH

#include<string>
#include<unistd.h>
#include<map>
#include<set>
#include<thread>
#include<future>
#include<exception>
#include "zmq.hpp"

#include "../messages/localURI.pb.h"
#include "../messages/localURIQuery.pb.h"
#include "../messages/localURIRemovalRequest.pb.h"
#include "../messages/localURIReplyHeader.pb.h"
#include "../messages/messageContainer.pb.h"

#include "classDeclarations.h"
//#include "localReplier.h"
//#include "localRequester.h"
//#include "localStreamPublisher.h"
//#include "localStreamSubscriber.h"
#include "localURIServerSingleton.h"
#include "utilityFunctions.h"
#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"

//The maximum time to wait for a send or a receive with the database to finish
#define HOW_LONG_TO_WAIT_FOR_DATABASE_SERVER_IN_MILLISECONDS 10000

//How long the inproc proxy should wait before timing out and checking whether the proxy process should close or if the softstate needs to be restored
#define HOW_LONG_TO_BLOCK_FOR_REQUEST 500

#define LOCAL_URI_ENGINE_IN_PROC_ADDRESS "LOCAL_URI_ENGINE_IN_PROC_ADDRESS"

/*
This class allows publishing and tag based searching of information and service sources on this machine using the ZEROMQ the messaging library.
*/
class localURIEngine
{
public:

/*
This function initializes the engine(s) that allows for publishing and searching of information and service sources on this machine.  It does two main things.  It checks if there is a running URI database server process that is associated with the given folder already and starts one if there is not.  It also initializes a proxy in a new thread that forwards URI requests and registrations from all of the SOM communication objects to the URI database server.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
static void startEngine(const std::string &inputPathForMessagingDirectory);

/*
This function searches for URIs that match the parameters of the given query
@param inputQuery: The query that defines what URIs are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
static  std::vector<localURI> getURIs(const localURIQuery &inputQuery);

/*
This function searches for stream publishers that match certain tag and key/value pair criteria as defined by the inputQuery object.
@param inputQuery: The query that defines what sorts of objects are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
static  std::vector<localURI> getPublishers(const localURIQuery &inputQuery);

/*
This function searches for repliers that match certain tag and key/value pair criteria as defined by the inputQuery object.
@param inputQuery: The query that defines what sorts of objects are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
static  std::vector<localURI> getRepliers(const localURIQuery &inputQuery);

/*
This function attempts to register a local URI with the database.  It throws an exception if one of the ZMQ operations fails or the engine is invalid, returns 1 if the URI was successfully registered and 0 otherwise.
@param inputLocalURI: The URI to register
@return: 1 if successful and 0 otherwise 
@exception: This function will throw an exception if the local inproc proxy (URIEngine) is invalid or a ZMQ call fails
*/
static  int addURI(const localURI &inputLocalURI);

/*
This function attempts to remove a local URI from the database.  It throws an exception if one of the ZMQ operations fails or the engine is invalid, returns 1 if the URI was successfully registered and 0 otherwise.
@param inputLocalURIResourceName: The resource name of the URI to remove
@return: 1 if successful and 0 otherwise 
@exception: This function will throw an exception if the local inproc proxy (URIEngine) is invalid or a ZMQ call fails
*/
static  int removeURI(const std::string &inputLocalURIResourceName);

//The inproc address of the server proxy engine
static std::string *engineInProcAddress;
static zmq::context_t *ZMQContext;
static std::string pathForMessagingDirectory; //The messaging directory folder to use (empty if not set)

zmq::context_t engineContext;         //Initialization of member classes happens in order of listing, context must be higher than listing




private:
static bool serverThreadExitFlag; //This flag indicates if the inproc proxy server should shut down so that it can be joined

/*
This function constructs the singleton instance of the class and should only be invoked by the startEngine function.  Specifically, it carries out almost all of the functionality of startEngine.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
localURIEngine(const std::string &inputPathForMessagingDirectory);

/*
This function is used to ensure that there is a URI server associated with the IPC directory that is being used.  It does this by checking if one exists (in which case it returns its address) and creating a new one if there isn't one already.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@return: The IPC address of the server process
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a memory allocation fails or a file operation fails.  
*/
std::string ensureDirectoryURIServerSingletonIsRunningAndGetAddress(const std::string &inputPathForMessagingDirectory);

/*
This function cleans up the object
*/
~localURIEngine();

//See description below
friend void initializeAndRunLocalURIProxy(localURIEngine *inputLocalURIEngine, std::string inputPathForDirectoryURIServer, std::promise<bool> *inputPromise);

//Don't allow copying
localURIEngine(localURIEngine const&) = delete;
void operator=(localURIEngine const&) = delete;

std::unique_ptr<std::thread> serverThread;
std::string databaseServerIPCAddress;
};

/*
This function initializes the localURIEngine and then starts running the local URI proxy server.  It returns true via the inputPromise variable if everything initialized correctly and otherwise returns an exception.  The proxy server is terminated by a special message type that tells the to return.  This function should only be used as a seperate thread launched as part of the constructor for a localURIEngine object.
@param inputLocalURIEngine: The engine that the function is running the proxy server from
@param inputPathForDirectoryURIServer: The path to the directory that the IPC files for ZeroMQ will be created in
@param inputPromise: A promise that allows status values to be returned from the seperate thread
*/
void initializeAndRunLocalURIProxy(localURIEngine *inputLocalURIEngine, std::string inputPathForDirectoryURIServer, std::promise<bool> *inputPromise);




#endif
