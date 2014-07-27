#ifndef SOMEXCEPTIONH
#define SOMEXCEPTIONH


#include<string>

enum exceptionClass
{
ZMQ_ERROR,
SQLITE3_ERROR
};

/*
This class is used to throw an informative exception.
*/
class SOMException
{
public:
/*
This function initializes the exception object with the required information.
@param inputErrorMessage: A message specific to this error instance
@param inputExceptionClass: What class of error this is (so different types of processing can be done)
@param inputSourceFileName: The name of the source file this exception originated in (hopefully provided by the __FILE__ macro)
@param inputSourceLineNumber: The line that the exception originated (hopefully provided by the __LINE__ macro)
*/
SOMException(const std::string &inputErrorMessage, exceptionClass inputExceptionClass, const char *inputSourceFileName, int inputSourceLineNumber);

/*
This function returns a string that is a summary of the error that caused the exception
@return: The string summary of the error
*/
std::string toString();

std::string errorMessage;
exceptionClass exceptionType;
std::string sourceFileName;
std::string sourceLineNumber;
};

/*
This function converts the exceptionClass enum into a string
@param inputExceptionType: The type of exception
@return: The string equivalent
*/
std::string exceptionClassToString(exceptionClass inputExceptionType);







#endif
