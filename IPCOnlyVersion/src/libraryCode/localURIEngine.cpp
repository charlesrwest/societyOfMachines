#include "localURIEngine.hpp"

#define ADD_URI_REQUEST_TYPE    0
#define REMOVE_URI_REQUEST_TYPE 1
#define QUERY_URIS_REQUEST_TYPE 2
#define CLOSE_REQUEST 3 //This isn't used with the directory server, but is used with localURI in program servers to tell the thread to close down

std::string *localURIEngine::engineInProcAddress = NULL;
zmq::context_t *localURIEngine::ZMQContext = NULL; 
bool localURIEngine::serverThreadExitFlag = false;
std::string localURIEngine::pathForMessagingDirectory;
/*
This function initializes the engine(s) that allows for publishing and searching of information and service sources on this machine.  It does two main things.  It checks if there is a running URI database server process that is associated with the given folder already and starts one if there is not.  It also initializes a proxy in a new thread that forwards URI requests and registrations from all of the SOM communication objects to the URI database server.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
void localURIEngine::startEngine(const std::string &inputPathForMessagingDirectory)
{
static localURIEngine instance(inputPathForMessagingDirectory);  //Declares a single instance which goes out of scope after main
}


/*
This function constructs the singleton instance of the class and should only be invoked by the startEngine function.  Specifically, it carries out almost all of the functionality of startEngine.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a thread creation call fails, a memory allocation fails, a ZMQ operation fails or a file operation fails.  
*/
localURIEngine::localURIEngine(const std::string &inputPathForMessagingDirectory) : engineContext()
{
ZMQContext = &engineContext; 


//Get the IPC address for a the URI server associated with the directory we are using
std::string directoryURIServerAddress = ensureDirectoryURIServerSingletonIsRunningAndGetAddress(inputPathForMessagingDirectory);

//Create a promise so that we can find out if the thread was able to initialize (and if it has finished)
std::promise<bool> threadInitializationResult;


//Start a thread and pass it a this pointer
serverThread = std::unique_ptr<std::thread>(new std::thread(initializeAndRunLocalURIProxy, this,inputPathForMessagingDirectory + directoryURIServerAddress, &threadInitializationResult));

//This should block until the server thread has been initialized and throw any exceptions that the server thread threw during its initialization
try
{
   if(threadInitializationResult.get_future().get() == true)
   {
      //printf("Thread reports that it initialized successfully\n");
   }
}
catch(const std::exception &inputException)
{
   fprintf(stderr, "%s\n", inputException.what());
}


}

/*
This function is used to ensure that there is a URI server associated with the IPC directory that is being used.  It does this by checking if one exists (in which case it returns its address) and creating a new one if there isn't one already.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@return: The IPC address of the server process
@exception: This function will throw exceptions if the directory is invalid, a fork call fails, a memory allocation fails or a file operation fails.  
*/
std::string localURIEngine::ensureDirectoryURIServerSingletonIsRunningAndGetAddress(const std::string &inputPathForMessagingDirectory)
{
   //See if there is a server going already and start one if there is not
   //Check if there is an existing localURIServer
   uint64_t existingSingletonID;
   if(checkForExistingURISingletonsAndCleanUpSingletonIPCFiles   (inputPathForMessagingDirectory, &existingSingletonID))
   {
      //A singleton exists, so return its IPC path
      return std::string(URI_SERVER_SINGLETON_IPC_FORMAT) + std::to_string(existingSingletonID);
   }


   //No singleton exists, so spawn one (might replace this with a simple system call)
   pid_t forkResult = fork();

   if(forkResult == -1)
   {
      throw SOMException(std::string("Error forking to create database singleton\n"), FORK_ERROR, __FILE__, __LINE__);
   }

   if(forkResult == 0) 
   {
//We are in the child process
//Make a singleton
      try
      {

         if(becomeDaemon() != 1)
         {
            _exit(0);
         }

       //Make singleton object
         localURIServerSingleton databaseSingleton(inputPathForMessagingDirectory);

         //Process requests forever
         databaseSingleton.processRequests();
         _exit(0);
      }
      catch(std::exception &inputException)
      {
         //Swallow any exceptions and exit to make sure child process doesn't call destructor the parent's ZMQ resources
//fprintf(stderr, "Attempt to start database singleton failed: %s\n", inputException.what());
         _exit(0); //Return successfully
      }
   }

//Wait for up to a certain amount of time in increments of 100 milliseconds for a database server singleton to start
   uint64_t singletonID;
   bool foundSingleton=false;
   for(int i=0; i<HOW_LONG_TO_WAIT_FOR_DATABASE_SERVER_IN_MILLISECONDS; i+=100)
    {

      if(checkForExistingURISingletonsAndCleanUpSingletonIPCFiles(inputPathForMessagingDirectory, &singletonID))
      {
         foundSingleton = true;
         break;
      }

      if(usleep(100*1000)!=0)
      {
         throw SOMException(std::string("Error occurred attempting to sleep\n"), SYSTEM_ERROR, __FILE__, __LINE__);
      }
   }


   if(!foundSingleton)
   {
      throw SOMException(std::string("Attempt to start singleton failed\n"), SYSTEM_ERROR, __FILE__, __LINE__);
   }


   //A singleton exists, so return it
   return std::string(URI_SERVER_SINGLETON_IPC_FORMAT) + std::to_string(singletonID);
}

/*
This function searches for URIs that match the parameters of the given query
@param inputQuery: The query that defines what URIs are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
std::vector<localURI> localURIEngine::getURIs(const localURIQuery &inputQuery)
{
//Check singleton state
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}


//Send an inproc request for the list of URIs

//Construct request
std::string serializedQueryMessage;
messageContainer queryMessage;

(*queryMessage.MutableExtension(localURIQuery::aLocalURIQuery)) = inputQuery;
queryMessage.SerializeToString(&serializedQueryMessage);



//Make socket to use for connection
zmq::socket_t *queryRequestSocket;
SOM_TRY
//Initialize the ZMQ socket used to talk to the directory URI server
queryRequestSocket = new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REQ); //Request socket to talk with the database server
SOM_CATCH("Error constructing ZMQ socket for getPublishers request\n")

//TODO: Should really be using unique pointer here
SOMScopeGuard querySocketGuard([&](){delete queryRequestSocket;});

//Make connection to inproc local URI server
SOM_TRY
queryRequestSocket->connect(localURIEngine::engineInProcAddress->c_str());
SOM_CATCH("Error connecting socket to local URI server for getPublishers request\n")

//Send query
SOM_TRY
queryRequestSocket->send(serializedQueryMessage.c_str(), serializedQueryMessage.size());
SOM_CATCH("Error sending request for list of publishers to local URI server\n")

//Allocate a buffer for the reply
zmq::message_t *messageBuffer;
SOM_TRY
messageBuffer = new zmq::message_t;  //This can throw a zmq::error_t
SOM_CATCH("Error constructing ZMQ message buffer\n")
SOMScopeGuard messageGuard([&](){delete messageBuffer;});


//Get reply
SOM_TRY
//Get first part of message
queryRequestSocket->recv(messageBuffer);
SOM_CATCH("Error receiving the reply from local URI server\n")

//Check if the request succeeded
messageContainer serverReply;
serverReply.ParseFromString(std::string((const char *) messageBuffer->data(), messageBuffer->size()));
if(!serverReply.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}


if(!serverReply.HasExtension(localURIReply::aLocalURIReply))
{
//Reply isn't in proper format, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(serverReply.GetExtension(localURIReply::aLocalURIReply).value() != REQUEST_SUCCEEDED)
{
throw SOMException("Error, request to server failed\n", SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Get the second part of message and turn it into a vector of URIs
std::vector<localURI> listOfMatchingURIs;
int numberOfURIsReturned = serverReply.ExtensionSize(localURI::localURIs);
for(int i=0; i<numberOfURIsReturned; i++)
{
listOfMatchingURIs.push_back(serverReply.GetExtension(localURI::localURIs, i));
}

return listOfMatchingURIs;
}

/*
This function searches for stream publishers that match certain tag and key/value pair criteria as defined by the inputQuery object.
@param inputQuery: The query that defines what sorts of objects are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
std::vector<localURI> localURIEngine::getPublishers(const localURIQuery &inputQuery)
{
//Add the publisher label to the query
localURIQuery queryBuffer = inputQuery;
queryBuffer.add_requiredtags(PUBLISHER_TAG);

//Not bothering to trim publisher tag from result, as it is informative and extra tags can be expected (also it is an inefficient operation)
SOM_TRY
//Get URIs that match the query
return getURIs(queryBuffer);
SOM_CATCH("Error retrieving publisher URI list\n")
}

/*
This function searches for repliers that match certain tag and key/value pair criteria as defined by the inputQuery object.
@param inputQuery: The query that defines what sorts of objects are desired
@return: A list of all of the URIs (with their tags) that fit the description
@exceptions: This function will throw exceptions if the engine is invalid or a ZeroMQ operation fails.
*/
std::vector<localURI> localURIEngine::getRepliers(const localURIQuery &inputQuery)
{
//Add the publisher label to the query
localURIQuery queryBuffer = inputQuery;
queryBuffer.add_requiredtags(REPLIER_TAG);

//Not bothering to trim publisher tag from result, as it is informative and extra tags can be expected (also it is an inefficient operation)
SOM_TRY
//Get URIs that match the query
return getURIs(queryBuffer);
SOM_CATCH("Error retrieving replier URI list\n")
}

/*
This function attempts to register a local URI with the database.  It throws an exception if one of the ZMQ operations fails or the engine is invalid, returns 1 if the URI was successfully registered and 0 otherwise.
@param inputLocalURI: The URI to register
@return: 1 if successful and 0 otherwise 
@exception: This function will throw an exception if the local inproc proxy (URIEngine) is invalid or a ZMQ call fails
*/
int localURIEngine::addURI(const localURI &inputLocalURI)
{
//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Construct request
std::string serializedAddURIMessage;
messageContainer addURIMessage;

addURIMessage.AddExtension(localURI::localURIs);
(*addURIMessage.MutableExtension(localURI::localURIs, 0)) = inputLocalURI;

addURIMessage.SerializeToString(&serializedAddURIMessage);

//Make socket to use for connection
std::unique_ptr<zmq::socket_t> addURIRequestSocket;
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REQ));

addURIRequestSocket =std::move(scopeTransferPointer); //Request socket to talk with the database server
SOM_CATCH("Error constructing ZMQ socket for addURI request\n")


//Make connection to inproc local URI server
SOM_TRY
addURIRequestSocket->connect(localURIEngine::engineInProcAddress->c_str());
SOM_CATCH("Error connecting socket to local URI server for addURI request\n")

//Send request
SOM_TRY
addURIRequestSocket->send(serializedAddURIMessage.c_str(), serializedAddURIMessage.size());
SOM_CATCH("Error sending addURI request to inproc proxy\n")

//Allocate a buffer for the reply
zmq::message_t *messageBuffer;
SOM_TRY
messageBuffer = new zmq::message_t;  //This can throw a zmq::error_t
SOM_CATCH("Error constructing ZMQ message buffer\n")
SOMScopeGuard messageGuard([&](){delete messageBuffer;});

//Get reply
SOM_TRY
addURIRequestSocket->recv(messageBuffer);
SOM_CATCH("Error receiving the reply from inproc proxy for addURI request\n")

//Check if the request succeeded
messageContainer serverReply;
serverReply.ParseFromString(std::string((const char *) messageBuffer->data(), messageBuffer->size()));
if(!serverReply.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(!serverReply.HasExtension(localURIReply::aLocalURIReply))
{
//Reply isn't in proper format, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(serverReply.GetExtension(localURIReply::aLocalURIReply).value() != REQUEST_SUCCEEDED)
{
//Request to add URI failed (was rejected by local URI server)
return 0;
}

//Request succeeded
return 1;
}

/*
This function attempts to remove a local URI from the database.  It throws an exception if one of the ZMQ operations fails or the engine is invalid, returns 1 if the URI was successfully registered and 0 otherwise.
@param inputLocalURIResourceName: The resource name of the URI to remove
@return: 1 if successful and 0 otherwise 
@exception: This function will throw an exception if the local inproc proxy (URIEngine) is invalid or a ZMQ call fails
*/
int localURIEngine::removeURI(const std::string &inputLocalURIResourceName)
{
//Make sure singleton has been initialized
if(localURIEngine::engineInProcAddress == NULL || localURIEngine::ZMQContext == NULL)
{
throw SOMException(std::string("local URI engine state is invalid (the engine hasn't been initialized yet?\n"), SERVER_REQUEST_FAILED, __FILE__, __LINE__);
}

//Construct request
std::string serializedRemoveURIMessage;
messageContainer removeURIMessage;

localURIRemovalRequest removalRequest;
removalRequest.set_resourcename(inputLocalURIResourceName);

(*removeURIMessage.MutableExtension(localURIRemovalRequest::aLocalURIRemovalRequest)) = removalRequest;

removeURIMessage.SerializeToString(&serializedRemoveURIMessage);

//Make socket to use for connection
std::unique_ptr<zmq::socket_t> removeURIRequestSocket;
SOM_TRY
std::unique_ptr<zmq::socket_t> scopeTransferPointer(new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REQ));

removeURIRequestSocket =std::move(scopeTransferPointer); //Request socket to talk with the database server
SOM_CATCH("Error constructing ZMQ socket for removeURI request\n")


//Make connection to inproc local URI server
SOM_TRY
removeURIRequestSocket->connect(localURIEngine::engineInProcAddress->c_str());
SOM_CATCH("Error connecting socket to local URI server for removeURI request\n")

//Send request
SOM_TRY
removeURIRequestSocket->send(serializedRemoveURIMessage.c_str(), serializedRemoveURIMessage.size());
SOM_CATCH("Error sending removeURI request to inproc proxy\n")

//Allocate a buffer for the reply
zmq::message_t *messageBuffer;
SOM_TRY
messageBuffer = new zmq::message_t;  //This can throw a zmq::error_t
SOM_CATCH("Error constructing ZMQ message buffer\n")
SOMScopeGuard messageGuard([&](){delete messageBuffer;});

//Get reply
SOM_TRY
removeURIRequestSocket->recv(messageBuffer);
SOM_CATCH("Error receiving the reply from inproc proxy for addURI request\n")

//Check if the request succeeded
messageContainer serverReply;
serverReply.ParseFromString(std::string((const char *) messageBuffer->data(), messageBuffer->size()));
if(!serverReply.IsInitialized())
{
//Reply can't be read, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(!serverReply.HasExtension(localURIReply::aLocalURIReply))
{
//Reply isn't in proper format, so throw an exception
throw SOMException("Error, message received from local URI server is invalid\n", INCORRECT_SERVER_RESPONSE, __FILE__, __LINE__);
}

if(serverReply.GetExtension(localURIReply::aLocalURIReply).value() != REQUEST_SUCCEEDED)
{
//Request to add URI failed (was rejected by local URI server)
return 0;
}

//Request succeeded
return 1;
}



/*
This function initializes the localURIEngine and then starts running the local URI proxy server.  It returns true via the inputPromise variable if everything initialized correctly and otherwise returns an exception.  The proxy server is terminated by a special message type that tells the to return.  This function should only be used as a seperate thread launched as part of the constructor for a localURIEngine object.
@param inputLocalURIEngine: The engine that the function is running the proxy server from
@param inputPathForDirectoryURIServer: The path to the directory that the IPC files for ZeroMQ will be created in
@param inputPromise: A promise that allows status values to be returned from the seperate thread
*/
void initializeAndRunLocalURIProxy(localURIEngine *inputLocalURIEngine, std::string inputPathForDirectoryURIServer, std::promise<bool> *inputPromise)
{

zmq::socket_t *singletonRequesterSocket; //The socket used to talk with the server database
zmq::socket_t *INPROC_ServerSocket; //The socket used to talk with objects in the program



try
{
//Initialize the ZMQ socket used to talk to the directory URI server
singletonRequesterSocket = new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REQ); //Request socket to talk with the database server
}
catch(const error_t &inputException)
{
inputPromise->set_exception(std::current_exception());
return;
}
SOMScopeGuard socketScopeGuard([&](){delete singletonRequesterSocket;}); //Make sure socket gets cleaned up when function returns



//Configure socket for sending timeout
int databseWaitTime = HOW_LONG_TO_WAIT_FOR_DATABASE_SERVER_IN_MILLISECONDS;
try
{
singletonRequesterSocket->setsockopt(ZMQ_SNDTIMEO, &databseWaitTime, sizeof(databseWaitTime));

singletonRequesterSocket->setsockopt(ZMQ_RCVTIMEO, &databseWaitTime, sizeof(databseWaitTime));
}
catch(const zmq::error_t &inputError)
{
inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("ZMQ error occured while setting socket options\n") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__)));
return;
}



try
{
singletonRequesterSocket->connect(("ipc://" + inputPathForDirectoryURIServer).c_str());
}
catch(const zmq::error_t &inputError)
{
inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("ZMQ error occured connecting to database singleton\n") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__)));
return;
}

//Create inproc socket
try
{
//Initialize the ZMQ for local objects to talk to
INPROC_ServerSocket = new zmq::socket_t(*localURIEngine::ZMQContext, ZMQ_REP); //Request socket to talk with the database server
}
catch(const error_t &inputException)
{
inputPromise->set_exception(std::current_exception());
return;
}
SOMScopeGuard INPROC_SocketGuard([&](){delete INPROC_ServerSocket;}); //Make sure socket gets cleaned up when function returns

//Make it so the inproc proxy doesn't wait forever so it can periodically check and refresh things
int inprocWaitTime = HOW_LONG_TO_BLOCK_FOR_REQUEST;
try
{
INPROC_ServerSocket->setsockopt(ZMQ_SNDTIMEO, &inprocWaitTime, sizeof(inprocWaitTime));

INPROC_ServerSocket->setsockopt(ZMQ_RCVTIMEO, &inprocWaitTime, sizeof(inprocWaitTime));
}
catch(const zmq::error_t &inputError)
{
inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("ZMQ error occured while setting socket options\n") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__)));
return;
}

//Bind inproc socket
std::string inprocAddress = std::string("inproc://") +LOCAL_URI_ENGINE_IN_PROC_ADDRESS;
try
{
INPROC_ServerSocket->bind((inprocAddress).c_str());
//Set singleton global
localURIEngine::engineInProcAddress = &inprocAddress;
localURIEngine::pathForMessagingDirectory = inputPathForDirectoryURIServer;
}
catch(const zmq::error_t &inputError)
{
inputPromise->set_exception(std::make_exception_ptr(SOMException(std::string("ZMQ error occured binding inproc socket for local URI engine\n") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__)));
return;
}
inputPromise->set_value(true); //Initialization completed successfully



//By all rights, we should be checking for close messages, but for now we will just let it exit messily
//TODO: Have the thread do blocking waits and check a flag variable to see if it should exit
try
{
//*INPROC_ServerSocket, *singletonRequesterSocket
//Allocate a buffer for the reply
zmq::message_t *messageBuffer;
SOM_TRY
messageBuffer = new zmq::message_t;  //This can throw a zmq::error_t
SOM_CATCH("Error constructing ZMQ message buffer\n")
SOMScopeGuard messageGuard([&](){delete messageBuffer;});
bool multiPartMessageFlag;

while(true)
{
//Check if this thread should be closed down 
if(localURIEngine::serverThreadExitFlag == true)
{
//Time for the thread to shutdown
return;
}

//Rebuild the message buffer
SOM_TRY
messageBuffer->rebuild();
SOM_CATCH("Error rebuilding message buffer\n")

//Recieve inproc messages
SOM_TRY
if(INPROC_ServerSocket->recv(messageBuffer) == false)
{
//Inproc receive timed out, so start the cycle again
continue;
}
SOM_CATCH("Error receiving inproc request\n")

multiPartMessageFlag = messageBuffer->more();

//Send message (or message part) to the server
const char *testString="This is a string\n";

SOM_TRY
if(singletonRequesterSocket->send((*messageBuffer)) == false)
{
//Send to database server timed out, so restart cycle
continue;
}
SOM_CATCH("Error forwarding request to database server\n")

//If this is just part of the message, send the rest by restarting the loop until they are all sent
if(multiPartMessageFlag) 
{
continue;
}

while(true)
{


//Rebuild the message buffer
SOM_TRY
messageBuffer->rebuild();
SOM_CATCH("Error rebuilding message buffer\n")

//Recieve server messages
SOM_TRY
//In inner loop that is the last set of things, so break if timeout occurs to continue the primary loop
if(singletonRequesterSocket->recv(messageBuffer) == false)
{
break;
}
SOM_CATCH("Error receiving server reply\n")

multiPartMessageFlag = messageBuffer->more();

//Send message (or message part) to the server
SOM_TRY
if(INPROC_ServerSocket->send((*messageBuffer)) == false)
{
//In inner loop that is the last set of things, so break if timeout occurs to continue the primary loop
break;
}
SOM_CATCH("Error forwarding reply from database server\n")

//If this is just part of the message, send the rest by restarting the loop until they are all sent
if(multiPartMessageFlag) 
{
continue;
}
else
{
break;
}

}
}

}
catch(const std::exception &inputException)
{
return;
}

return;
}

/*
This function cleans up the object
*/
localURIEngine::~localURIEngine()
{
localURIEngine::serverThreadExitFlag = true;
serverThread->join();
}

