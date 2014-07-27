#include<stdio.h>
#include "SOMException.h"


int main(int argc, char **argv)
{


try
{

throw SOMException("Nasty error occurred", ZMQ_ERROR, __FILE__, __LINE__);

}
catch(SOMException &inputSOMException)
{
printf("%s", inputSOMException.toString().c_str());
}



return 0;
}
