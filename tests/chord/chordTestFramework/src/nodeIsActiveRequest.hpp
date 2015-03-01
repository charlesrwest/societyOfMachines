#ifndef NODEISACTIVEREQUESTHPP
#define NODEISACTIVEREQUESTHPP

#include <string>
#include <stack>
#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"
#include "chordNodeContactInformation.pb.h"
#include "SOMException.hpp"

class nodeIsActiveRequest
{
public:
/*
This function initializes the request with the contact information of the node that is being checked and initializes the number of resends to 0.
*/
nodeIsActiveRequest(const chordNodeContactInformation &inputNodeToCheck);

chordNodeContactInformation nodeToCheckIfActive;
boost::multiprecision::uint512_t nodeToCheckIfActiveID;
uint64_t resendCount; //The number of times the request has been sent
};






#endif
