#include<stdio.h>
#include "SOMException.hpp"
#include<stdexcept> 

int main(int argc, char **argv)
{


try
{
throw SOMException("Nasty error occurred\n", ZMQ_ERROR, __FILE__, __LINE__);
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

printf("\n\n\n\n");

try
{
try
{
throw std::logic_error("There was a logic error\n");
}
catch(const std::exception &inputException)
{
throw SOMException("Another nasty error occurred\n", ZMQ_ERROR, inputException, __FILE__, __LINE__);
}
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

printf("\n\n\n\n");

try
{
try
{
throw std::logic_error("There was another a logic error\n");
}
catch(const std::exception &inputException)
{
throw SOMException("One more nasty error occurred\n", inputException, __FILE__, __LINE__);
}
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

printf("\n\n\n\n");

try
{
try
{
throw SOMException("A file system error was imagined\n",FILE_SYSTEM_ERROR, __FILE__, __LINE__);
}
catch(const SOMException &inputSOMException)
{
throw SOMException("A file system error occurred?\n", inputSOMException, __FILE__, __LINE__);
}
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

printf("\n\n\n\n");

try
{
SOM_TRY
throw SOMException("A file system error was imagined to test macro crazyness\n",FILE_SYSTEM_ERROR, __FILE__, __LINE__);
SOM_CATCH("A file system error occurred?  To test crazyness!?\n")
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

printf("\n\n\n\n");

try
{
SOM_TRY
throw SOMException("A file system error was imagined to test macro crazyness again\n",FILE_SYSTEM_ERROR, __FILE__, __LINE__);
SOM_CATCH2("A file system error occurred?  To test crazyness!? AGAIN!?\n", AN_ASSUMPTION_WAS_VIOLATED_ERROR)
}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}

return 0;
}
