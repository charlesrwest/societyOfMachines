#ifndef UTILITYFUNCTIONSH
#define UTILITYFUNCTIONSH

#include<string>
#include<vector>
#include<set>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h> //file operations
#include "SOMException.hpp"
#include<stdexcept>
#include <unistd.h>

#include "../messages/localURI.pb.h"

#define URI_SERVER_SINGLETON_IPC_FORMAT "URIDatabaseServer:ProcessID:"

/*
This file defines functions which are useful and likely to be needed by more than one class
*/

/*
This function returns a list of the process IDs of all currently active processes on this machine.
@return: The list of process IDs
@exceptions: This function throws an exception if one of the system calls fail
*/
std::vector<uint64_t> getActiveProcessIDList();

/*
This function returns the set of all the process IDs of currently active processes on this machine.
@return: The list of process IDs
@exceptions: This function throws an exception if one of the system calls fail
*/
std::set<uint64_t> getActiveProcessIDSet();



/*
This function returns a list of all non-directory files in a given directory.
@param inputDirectoryPath: The path of the directory to search
@return: The list of all non-directory files in a given directory
@exceptions: This function throws an execption if one of the system calls fail
*/
std::vector<std::string> getListOfFilesInDirectory(std::string inputDirectoryPath);


/*
This function returns a list of all directory files in a given directory.
@param inputDirectoryPath: The path of the directory to search
@return: The list of all directory files in a given directory
@exceptions: This function throws an execption if one of the system calls fail
*/
std::vector<std::string> getListOfFoldersInDirectory(std::string inputDirectoryPath);

/*
This function returns a list of all normal files and/or directory files in a given directory.
@param inputDirectoryPath: The path of the directory to search
@param inputFilesFoldersOrBoth: An option to indicate if we want a list of the regular files (0), directories (1) or both (2).  If an unrecognized value is given, it defaults to regular files.
@return: The list of all directory files in a given directory
@exceptions: This function throws an exception if one of the system calls fail
*/
std::vector<std::string> getListOfFilesOrFoldersInDirectory(std::string inputDirectoryPath, int inputFilesFoldersOrBoth);

/*
This function examines each of the files in the given directory, looking for files that fit the format "URIDatabaseServer:ProcessID:xxxx".  It then parses the number from each of the files and adds it to the list that is returned.  This list can then be compared with the list of active process IDs to determine if there are currently active singletons.
@param inputIPCDirectoryPath: The folder where the IPC files can be found
@return: The list of IDs
@exceptions: This function can throw an SOMexception if one of the file operations fail
*/
std::vector<uint64_t> getListOfSingletonProcessIDs(std::string inputIPCDirectoryPath);

/*
This function checks for any active singletons with URI server singleton IPC files in the given directory.  If no currently active singleton files are found, any (nonactive) singleton files are deleted.  If a active singleton file is found, the function takes no action.
@param inputIPCDirectoryPath: The folder where the IPC files can be found
@param inputSingletonProcessID: A pointer to a buffer to place the ID of the singleton (if one is found) in
@return: true if there is a previously existing singleton and false otherwise
@exceptions: Throws an exception if a file system error occurs
*/
bool checkForExistingURISingletonsAndCleanUpSingletonIPCFiles(std::string inputIPCDirectoryPath, uint64_t *inputSingletonProcessID = NULL);

/*
This function makes the calling process a daemon process.
@return: 1 if successful and 0 on error
*/
int becomeDaemon();

/*
This function takes a URI and presents it as a string suitable for viewing to get an idea of what is in the URI
@param inputURI: The URI to convert to a string
@return: The generated string
*/
std::string uriToString(const localURI &inputURI);

#endif
