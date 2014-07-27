#include "utilityFunctions.h"



/*
This function returns a list of the process IDs of all currently active processes on this machine.
@return: The list of process IDs
@exceptions: This function throws an exception if one of the system calls fail
*/
std::vector<uint64_t> getActiveProcessIDList()
{
//Get list of folders in /proc
std::vector<std::string> listOfFolders = getListOfFoldersInDirectory("/proc");  //This can throw file system error SOMExceptions, we just pass them up
std::vector<uint64_t> activeProcessIDList;

//Convert each of the numeric ones to integers and add them to list
long long int buffer;
for(int i=0; i<listOfFolders.size(); i++)
{

try
{
//Try to convert to a uint64_t
buffer = std::stoll(listOfFolders[i]);
activeProcessIDList.push_back((uint64_t) buffer);
}
catch(const std::invalid_argument &inputException)  //We are expecting some of the folders to be non-numeric, so don't do anything
{
}
catch(const std::out_of_range &inputException) //Shouldn't happen, but just skip it if it does
{
}

}

//Processed everything without getting exceptions;
return activeProcessIDList;
}

/*
This function returns the set of all the process IDs of currently active processes on this machine.
@return: The list of process IDs
@exceptions: This function throws an exception if one of the system calls fail
*/
std::set<uint64_t> getActiveProcessIDSet()
{
//Get list of folders in /proc
std::vector<std::string> listOfFolders = getListOfFoldersInDirectory("/proc");  //This can throw file system error SOMExceptions, we just pass them up
std::set<uint64_t> activeProcessIDSet;

//Convert each of the numeric ones to integers and add them to list
long long int buffer;
for(int i=0; i<listOfFolders.size(); i++)
{

try
{
//Try to convert to a uint64_t
buffer = std::stoll(listOfFolders[i]);
activeProcessIDSet.insert((uint64_t) buffer);
}
catch(const std::invalid_argument &inputException)  //We are expecting some of the folders to be non-numeric, so don't do anything
{
}
catch(const std::out_of_range &inputException) //Shouldn't happen, but just skip it if it does
{
}

}

//Processed everything without getting exceptions;
return activeProcessIDSet;
}

/*
This function returns a list of all non-directory files in a given directory.
@param inputDirectoryPath: The path of the directory to searchs
@return: The list of all non-directory files in a given directory
@exceptions: This function throws an execption if one of the system calls fail
*/
std::vector<std::string> getListOfFilesInDirectory(std::string inputDirectoryPath)
{
return getListOfFilesOrFoldersInDirectory(inputDirectoryPath, 0);
}


/*
This function returns a list of all directory files in a given directory.
@param inputDirectoryPath: The path of the directory to search
@return: The list of all directory files in a given directory
@exceptions: This function throws an execption if one of the system calls fail
*/
std::vector<std::string> getListOfFoldersInDirectory(std::string inputDirectoryPath)
{
return getListOfFilesOrFoldersInDirectory(inputDirectoryPath, 1);
}

/*
This function returns a list of all normal files and/or directory files in a given directory.
@param inputDirectoryPath: The path of the directory to search
@param inputFilesFoldersOrBoth: An option to indicate if we want a list of the regular files (0), directories (1) or both (2).  If an unrecognized value is given, it defaults to regular files.
@return: The list of all directory files in a given directory
@exceptions: This function throws an exception if one of the system calls fail
*/
std::vector<std::string> getListOfFilesOrFoldersInDirectory(std::string inputDirectoryPath, int inputFilesFoldersOrBoth)
{
std::vector<std::string> listOfFiles;

//Attemp to open given directory (returning a datastructure with information about the files)
DIR *directory;
directory=opendir(inputDirectoryPath.c_str());
if(directory == NULL)
{
throw SOMException("Error, unable to open directory: " + inputDirectoryPath+"\n", FILE_SYSTEM_ERROR, __FILE__, __LINE__); 
}



struct dirent path;
struct dirent *newEntryPointer;

//Lets check if things are working the way I think they are
struct dirent *originalAddressForPath = &path;

//Get all non directory file names
int returnValue;
while(true)
{
returnValue = readdir_r(directory, &path, &newEntryPointer);
if(returnValue != 0)
{
throw SOMException("Error retrieving file name from DIR data structure\n" + inputDirectoryPath+"\n", FILE_SYSTEM_ERROR, __FILE__, __LINE__); 
}

if(newEntryPointer == NULL)
{
break; //Reached end of the datastructure
}


//DT_REG for regular files, DT_DIR for directories (but there are still several other types of files)

//If we aren't interested in directory files and it isn't a regular file, skip it.  In addition, if we are solely interested in directories and it is not a directory file, skip it.
if(((path.d_type != DT_DIR) && !(((inputFilesFoldersOrBoth != 1) && (inputFilesFoldersOrBoth != 2)) ) || ((path.d_type != DT_DIR) && (inputFilesFoldersOrBoth == 1))) ) 
{
continue; //Skip this entry
}

listOfFiles.push_back(std::string(path.d_name));
}

//Test if things are working the way I thought
if(&path != originalAddressForPath)
{
throw SOMException("readdir_r is changing the address of path.  This can really break stuff\n", AN_ASSUMPTION_WAS_VIOLATED_ERROR, __FILE__, __LINE__); 
}


return listOfFiles;
}

/*
This function examines each of the files in the given directory, looking for files that fit the format "URIDatabaseServer:ProcessID:xxxx".  It then parses the number from each of the files and adds it to the list that is returned.  This list can then be compared with the list of active process IDs to determine if there are currently active singletons.
@param inputIPCDirectoryPath: The folder where the IPC files can be found
@return: The list of IDs
@exceptions: This function can throw an SOMexception if one of the file operations fail
*/
std::vector<uint64_t> getListOfSingletonProcessIDs(std::string inputIPCDirectoryPath)
{
//Get list of files in the directory
std::string searchString(URI_SERVER_SINGLETON_IPC_FORMAT);

std::vector<std::string> listOfFilesInDirectory;
std::vector<uint64_t> listOfSingletonProcessIDs;
try
{
listOfFilesInDirectory = getListOfFilesInDirectory(inputIPCDirectoryPath);
}
catch(SOMException &inputException)
{
throw SOMException("Error getting list of files in directory\n", inputException.exceptionType, __FILE__, __LINE__);
}

//Convert all matching ones to a list of integers
uint64_t buffer;
for(int i=0; i<listOfFilesInDirectory.size(); i++)
{
//Check for "URIDatabaseServer:ProcessID:"
if(listOfFilesInDirectory[i].find(searchString) == std::string::npos ||  listOfFilesInDirectory[i].size() == searchString.size())
{
continue;
}


//Convert the strings to uint64_t
try
{
//Try to convert to a uint64_t
buffer = std::stoll(listOfFilesInDirectory[i].substr(searchString.size()));
listOfSingletonProcessIDs.push_back(buffer);
}
catch(const std::invalid_argument &inputException)  //We are expecting some of the folders to be non-numeric, so don't do anything
{
}
catch(const std::out_of_range &inputException) //Shouldn't happen, but just skip it if it does
{
}

} 

return listOfSingletonProcessIDs;
}

/*
This function checks for any active singletons with URI server singleton IPC files in the given directory.  If no currently active singleton files are found, any (nonactive) singleton files are deleted.  If a active singleton file is found, the function takes no action.
@param inputIPCDirectoryPath: The folder where the IPC files can be found
@return: true if there is a previously existing singleton and false otherwise
@exceptions: Throws an exception if a file system error occurs
*/
bool checkForExistingURISingletonsAndCleanUpSingletonIPCFiles(std::string inputIPCDirectoryPath, uint64_t *inputSingletonProcessID)
{
//Find any existing "URIDatabaseServer:ProcessID:xxxx" files
std::vector<uint64_t> listOfSingletonProcessIDs;

try
{
listOfSingletonProcessIDs = getListOfSingletonProcessIDs(inputIPCDirectoryPath); 
}
catch(SOMException &inputException)
{
throw SOMException("Error retrieving server singleton processIDs\n", inputException.exceptionType, __FILE__, __LINE__);
}

//Get set of active process IDs and see if the database object ID is among them

std::set<uint64_t> currentlyActiveProcessIDs;

try
{
currentlyActiveProcessIDs = getActiveProcessIDSet();
}
catch(SOMException &inputException)
{
throw SOMException("Error getting active process ID set\n", inputException.exceptionType, __FILE__, __LINE__);
}


//See if any of the retrieved singleton IDs are in the active set
for(int i=0; i<listOfSingletonProcessIDs.size(); i++)
{
if(currentlyActiveProcessIDs.count(listOfSingletonProcessIDs[i]) > 0)
{
if(inputSingletonProcessID != NULL)
{
(*inputSingletonProcessID) = listOfSingletonProcessIDs[i];
}
return true;
}
}

//None of the current IPC files for singletons are for active processes, so we will delete them
for(int i=0; i< listOfSingletonProcessIDs.size(); i++)
{
std::string fileToDeletePath = inputIPCDirectoryPath + std::string(URI_SERVER_SINGLETON_IPC_FORMAT) + std::to_string(listOfSingletonProcessIDs[i]);

if(remove(fileToDeletePath.c_str()) != 0)
{
throw SOMException("Error, unable to delete singleton IPC file: " + fileToDeletePath + "\n", FILE_SYSTEM_ERROR, __FILE__, __LINE__);
}
}

return false;
}

/*
This function makes the calling process a daemon process.
@return: 1 if successful and 0 on error
*/
int becomeDaemon()
{
int maxNumberOfFileDescriptors, fileDescriptor;

switch(fork())  //Become background process
{
case -1: return 0;
case 0: break;  //Child falls through...
default:  _exit(0);  //While parent terminates
}

if(setsid()==-1)  //Become leader of a new session
return 0;

switch(fork())
{
case -1: return 0;
case 0: break;
default: _exit(0);
}

umask(0);  //Clear file mode creation mask



maxNumberOfFileDescriptors=sysconf(8192);
if(maxNumberOfFileDescriptors==-1)  //Limit is indeterminate...
maxNumberOfFileDescriptors=8192;  //So take a guess

for(fileDescriptor=0; fileDescriptor<maxNumberOfFileDescriptors; fileDescriptor++)
close(fileDescriptor);


close(STDIN_FILENO);  //Reopen standard fd's to /dev/null

fileDescriptor = open("/dev/null", O_RDWR);

if(fileDescriptor != STDIN_FILENO)  //fileDescriptor should be 0
return 0;
if(dup2(STDIN_FILENO, STDOUT_FILENO)!= STDOUT_FILENO)
return 0;
if(dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
return 0;


return 1;
}

/*
This function takes a URI and presents it as a string suitable for viewing to get an idea of what is in the URI
@param inputURI: The URI to convert to a string
@return: The generated string
*/
std::string uriToString(const localURI &inputURI)
{

std::string outputString;
outputString += "Resource name: " + inputURI.resourcename() + "\n";
outputString += "IPC path: " + inputURI.ipcpath() + "\n";

if(inputURI.has_originatingprocessid() )
{
outputString +="Originating process ID: " + std::to_string(inputURI.originatingprocessid()) + "\n";
}

outputString += "Tags:\n";
for(int i=0; i<inputURI.tags_size(); i++)
{
outputString += std::to_string(i) + " : " + inputURI.tags(i) + "\n";
}

outputString += "integerKeyValuePairs:\n";
for(int i=0; i<inputURI.integerpairs_size(); i++)
{
outputString += std::to_string(i) + " : " + inputURI.integerpairs(i).key() +"  " + std::to_string(inputURI.integerpairs(i).value()) + "\n";
}

outputString += "doubleKeyValuePairs:\n";
for(int i=0; i<inputURI.doublepairs_size(); i++)
{
outputString += std::to_string(i) + " : " + inputURI.doublepairs(i).key() +"  " + std::to_string(inputURI.doublepairs(i).value()) + "\n";
}

return outputString;
}

