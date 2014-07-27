#ifndef LOCALURISERVERH
#define LOCALURISERVERH


#include "../messages/localURI.pb.h"
#include "../messages/localURIQuery.pb.h"
#include "../messages/localURIRemovalRequest.pb.h"
#include "../messages/localURIReplyHeader.pb.h"
#include "../messages/messageContainer.pb.h"

#include "utilityFunctions.h"
#include "SOMException.hpp"
#include "localURIDatabase.h"
#include "zmq.hpp"
#include <unistd.h>



/*
This class represents a ZMQ server which allows the publishing, querying and remove of URIs.  Only one process with this object should be permitted for a given URI folder that is being used for communication.  This is enforced by the database IPC file incorporating the process ID of the containing process.  When this class is started, it searches the folder for a file of the format "URIDatabaseServer:ProcessID:xxxx".  If such a file is found, it checks if the given process ID is active.  If it is, the process terminates but if it isn't it will remove the (presumably leftover) IPC file and then create its own (corresponding to its own req/rep server).
*/
class localURIServerSingleton
{
public:
/*
This function function initializes the singleton if one does not already exist.  As part of that initialization, it creates an in memory Sqlite3 database.
@param inputPathForMessagingDirectory: The path to the directory that the IPC files for ZeroMQ will be created in
@param inputTimeBetweenURIProcessIDChecksInMilliseconds: The amount of time (in milliseconds) between checks to remove URIs whose process ID is no longer active
@exception: This function will throw SOMException exceptions if a active localURIServerSingleton already exists, the directory is invalid, a memory allocation fails, a file operation fails, there is an error initializing the SQLite database or one of the ZMQ member objects throws an exception.
*/
localURIServerSingleton(std::string inputPathForMessagingDirectory, uint32_t inputTimeBetweenURIProcessIDChecksInMilliseconds = 60000);

/*
This function processes requests to add URIs, remove URIs and query URIs.  It will never return, but will throw an exception if it encounters an unrecoverable error (such as a ZMQ function failure).
*/
void processRequests();

//Lets see... We need a ZMQ context, a REQ/REP socket for request/reply, a SQLITE localURI database and we are handling request types: Add URI, remove URI, query URIs.  We also need to periodically check the active URIs list to remove any URI entries for processes who have an ID and it has expired.  This could be done relatively simply by spinning off a thread that sends a message to check the IDs to the main thread periodically

uint32_t timeBetweenURIProcessIDChecksInMilliseconds;
std::string pathForMessagingDirectory;

zmq::context_t ZMQContext;         //Initialization of member classes happens in order of listing, context must be higher than listing
zmq::socket_t  requestReplySocket;
localURIDatabase localDatabase;

protected:
/*
This is a convenience function that sends a reply to tell a client that it was not possible to process their request.
@param inputSourceFileName: The name of the source file this function call originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the function call originated (hopefully provided by the __LINE__ macro)
@exceptions: This function can throw a SOMException if a ZMQ call fails
*/
void sendRequestFailedMessage(const char *inputSourceFileName, int inputSourceLineNumber);

/*
This is a convenience function that sends a reply to tell a client that their request succeeded. 
@param inputSourceFileName: The name of the source file this function call originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the function call originated (hopefully provided by the __LINE__ macro)
@param inputMoreMessagesToFollow: True if we are sending more reply messages as part of the same reply and false otherwise
@exceptions: This function can throw a SOMException if a ZMQ call fails
*/
void sendRequestSucceededMessage(const char *inputSourceFileName, int inputSourceLineNumber, bool inputMoreMessagesToFollow=false);
};

#endif
