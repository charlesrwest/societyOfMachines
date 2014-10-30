#include<stdio.h>
#include<unistd.h>

#include "chordNodeManager.hpp"

#include "fingerTable.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"

int main(int argc, char **argv)
{
boost::multiprecision::uint512_t testNumber0("0x8000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000A");

fingerTable myFingerTable(testNumber0);

for(int i=0; i<myFingerTable.targetChordAddresses.size(); i++)
{
printf("0x%s\n", myFingerTable.targetChordAddresses[i].str(-1,std::ios::hex).c_str());
}



/*
try
{
printf("Hello world!!\n");

chordNodeManager nodeManager("nodeManagerInprocAddress");

for(int i=0; i<100; i++)
{
nodeManager.createChordNode(0);
printf("Made node: %d\n", i);
}

printf("Pausing nodes\n");
nodeManager.pauseNodes();
printf("Nodes have been paused\n");

sleep(1);

printf("Resuming nodes\n");
nodeManager.resumeNodes();
printf("Nodes have been resumed\n");

printf("All done.  Destructor time!\n");
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}
*/


//sleep(1);
return 0;
}
