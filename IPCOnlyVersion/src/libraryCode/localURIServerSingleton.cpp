#include "localURIServerSingleton.h"

/*
This function function initializes the singleton if one does not already exist.  As part of that initialization, it creates an in memory Sqlite3 database.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@param inputTimeBetweenURIProcessIDChecksInMilliseconds: The amount of time (in milliseconds) between checks to remove URIs whose process ID is no longer active
@exception: This function will throw exceptions if the directory is invalid, a memory allocation fails or a file operation fails  
*/
localURIServerSingleton::localURIServerSingleton(std::string inputPathForMessagingDirectory, uint32_t inputTimeBetweenURIProcessIDChecksInMilliseconds) 
try  //Deal with any exceptions thrown by member variables
: ZMQContext(), requestReplySocket(ZMQContext, ZMQ_REP)
{
if(localDatabase.isValid == false)
{
throw SOMException("Error constructing SQLITE database object", SQLITE3_ERROR, __FILE__, __LINE__);
}



pathForMessagingDirectory = inputPathForMessagingDirectory;

timeBetweenURIProcessIDChecksInMilliseconds = inputTimeBetweenURIProcessIDChecksInMilliseconds;

//Check if another active singleton exists and clean up any old singleton IPC files if there aren't any active ones
bool anotherSingletonExists;
try
{
anotherSingletonExists = checkForExistingURISingletonsAndCleanUpSingletonIPCFiles(inputPathForMessagingDirectory);
}
catch(SOMException &inputException)
{
throw SOMException("Error checking for existing URI server singletons\n", inputException.exceptionType, __FILE__, __LINE__);
}
if(anotherSingletonExists)
{
throw SOMException("Error, another server singleton already exists\n", SINGLETON_ALREADY_EXISTS, __FILE__, __LINE__);
}

try
{
requestReplySocket.bind(("ipc://"+inputPathForMessagingDirectory+std::string(URI_SERVER_SINGLETON_IPC_FORMAT) + std::to_string(getpid())).c_str());
}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error occured constructing localURIServerSingleton member variables") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__);
}

}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error occured constructing localURIServerSingleton member variables") + inputError.what(), ZMQ_ERROR, __FILE__, __LINE__);
}


/*
This function processes requests to add URIs, remove URIs and query URIs.  It will never return, but will throw an exception if it encounters an unrecoverable error (such as a ZMQ function failure).
*/
void localURIServerSingleton::processRequests()
{
try //Catch and rethrow zmq::error_t exceptions
{

//Process requests
while(true)
{
zmq::message_t messageBuffer;  //This can throw a zmq::error_t

//system("espeak \"I am waiting for a request\"");

//Get first part of message
try
{
requestReplySocket.recv(&messageBuffer);
}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error retrieving a ZMQ message\n"), ZMQ_ERROR, __FILE__, __LINE__);
}

//system("espeak \"I received a request\"");

//Check that the first the message is valid
messageContainer request;
request.ParseFromString(std::string((const char *) messageBuffer.data(), messageBuffer.size()));
if(!request.IsInitialized())
{
//Message is invalid, so send back a request failed message
sendRequestFailedMessage(__FILE__, __LINE__); //This can throw specific SOMExceptions
}

//Determine message type based on which extensions are availible



//Process the message
if(request.ExtensionSize(localURI::localURIs) > 0)
{
//system("espeak \"I got a add URI request\"");


//Use first localURI
localURI addURIRequest = request.GetExtension(localURI::localURIs, 0);

//Process the request
if(localDatabase.addURI(addURIRequest) != 1)
{
//Either the database had an error or their URI was invalid with respect to the database, so tell them the request failed
sendRequestFailedMessage(__FILE__, __LINE__); //This can throw specific SOMExceptions
}
else
{
sendRequestSucceededMessage(__FILE__,__LINE__);
}
}

else if(request.HasExtension(localURIRemovalRequest::aLocalURIRemovalRequest))
{
//system("espeak \"I got a removal request\"");

//Process the request
if(localDatabase.removeURI(request.MutableExtension(localURIRemovalRequest::aLocalURIRemovalRequest)->resourcename()) != 1)
{
//Either the database had an error or their URI was invalid with respect to the database, so tell them the request failed
sendRequestFailedMessage(__FILE__, __LINE__); //This can throw specific SOMExceptions
}
else
{
sendRequestSucceededMessage(__FILE__,__LINE__);
}
}

else if(request.HasExtension(localURIQuery::aLocalURIQuery))
{
//system("espeak \"I got a local URI query\"");

localURIQuery URIQueryRequest = (*request.MutableExtension(localURIQuery::aLocalURIQuery));

//Submit request to the database
std::vector<localURI> results;
if(localDatabase.processQuery(URIQueryRequest, results) != 1)
{
sendRequestFailedMessage(__FILE__, __LINE__); //This can throw specific SOMExceptions
continue;
}

//Request succeeded, so send results

//Construct response message
messageContainer response;
response.MutableExtension(localURIReply::aLocalURIReply)->set_value(REQUEST_SUCCEEDED); 

for(int i=0; i<results.size(); i++) 
{
response.AddExtension(localURI::localURIs);
(*response.MutableExtension(localURI::localURIs, i)) = results[i];
}

std::string serializedResponse;
response.SerializeToString(&serializedResponse);

//Send response message
try
{
requestReplySocket.send(serializedResponse.c_str(), serializedResponse.size());
}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error sending a ZMQ message\n"), ZMQ_ERROR, __FILE__, __LINE__);
}

}
else
{
//system("espeak \"I got an invalid request\"");


sendRequestFailedMessage(__FILE__, __LINE__); //This can throw specific SOMExceptions
}



} //While loop
}//Try
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error occured initializing ZMQ message\n"), ZMQ_ERROR, __FILE__, __LINE__);
}

}


/*
This is a convenience function that sends a reply to tell a client that it was not possible to process their request.
@param inputSourceFileName: The name of the source file this function call originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the function call originated (hopefully provided by the __LINE__ macro)
@exceptions: This function can throw a SOMException if a ZMQ call fails
*/
void localURIServerSingleton::sendRequestFailedMessage(const char *inputSourceFileName, int inputSourceLineNumber)
{
messageContainer response;
response.MutableExtension(localURIReply::aLocalURIReply)->set_value(REQUEST_FAILED);
std::string serializedResponse;
response.SerializeToString(&serializedResponse);

try
{
requestReplySocket.send(serializedResponse.c_str(), serializedResponse.size());
}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error sending a ZMQ message\n"), ZMQ_ERROR, inputSourceFileName, inputSourceLineNumber);
}
}

/*
This is a convenience function that sends a reply to tell a client that their request succeeded. 
@param inputSourceFileName: The name of the source file this function call originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the function call originated (hopefully provided by the __LINE__ macro)
@param inputMoreMessagesToFollow: True if we are sending more reply messages as part of the same reply and false otherwise
@exceptions: This function can throw a SOMException if a ZMQ call fails
*/
void localURIServerSingleton::sendRequestSucceededMessage(const char *inputSourceFileName, int inputSourceLineNumber, bool inputMoreMessagesToFollow)
{
messageContainer response;
response.MutableExtension(localURIReply::aLocalURIReply)->set_value(REQUEST_SUCCEEDED);
std::string serializedResponse;
response.SerializeToString(&serializedResponse);

try
{
requestReplySocket.send(serializedResponse.c_str(), serializedResponse.size());
}
catch(const zmq::error_t &inputError)
{
throw SOMException(std::string("Error sending a ZMQ message\n"), ZMQ_ERROR, inputSourceFileName, inputSourceLineNumber);
}
}
