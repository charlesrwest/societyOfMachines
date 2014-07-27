#include "SOMException.h"

/*
This function initializes the exception object with the required information.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException::SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const char *inputSourceFileName, int inputSourceLineNumber) : errorMessage(inputErrorMessage)
{

exceptionType = inputExceptionClass;

if(inputSourceFileName != NULL)
{
sourceFileName = std::string(inputSourceFileName);
}

sourceLineNumber = std::to_string(inputSourceLineNumber);

}

/*
This function returns a string that is a summary of the error that caused the exception
@return: The string summary of the error
*/
std::string SOMException::toString()
{
return "Error of type " + exceptionClassToString(exceptionType) + " occurred in file " + sourceFileName + " at line " + sourceLineNumber + ": " + errorMessage + "\n";
}

/*
This function converts the exceptionClass enum into a string
@param inputExceptionType: The type of exception
@return: The string equivalent
*/
std::string exceptionClassToString(exceptionClass inputExceptionType)
{
switch(inputExceptionType)
{
case ZMQ_ERROR:
return std::string("ZMQ_ERROR");
break;

case SQLITE3_ERROR:
return std::string("ZMQ_ERROR");
break;

default:
return "";
}
}

