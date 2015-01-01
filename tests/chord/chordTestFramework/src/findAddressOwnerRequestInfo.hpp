#ifndef FINDADDRESSOWNERREQUESTINFOHPP
#define FINDADDRESSOWNERREQUESTINFOHPP

#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"
#include "chordNodeContactInformation.pb.h"
#include "SOMException.hpp"

class findAddressOwnerRequestInfo
{
public:
/*
This function initializes the request info object so that it can be used as part of the event processing structure to fufill findAddressOwnerRequests.
@param inputAddressToFind: The address we are trying to find the owner for
@param inputClosestKnownPredecessorID: The next node to contact in the process of resolving the address owner

@exceptions: This function can throw exceptions
*/
findAddressOwnerRequestInfo(const boost::multiprecision::uint512_t &inputAddressToFind, const boost::multiprecision::uint512_t &inputClosestKnownPredecessorID);

boost::multiprecision::uint512_t addressToFind;
std::string addressToFindBinary;

boost::multiprecision::uint512_t lastClosestKnownPredecessorChordID;

uint64_t lastSentRequestID;
};













#endif
