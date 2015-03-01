#ifndef FINDADDRESSOWNERREQUESTHPP
#define FINDADDRESSOWNERREQUESTHPP

#include <string>
#include <stack>
#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"
#include "chordNodeContactInformation.pb.h"
#include "SOMException.hpp"

class findAddressOwnerRequest
{
public:
/*
This function initializes the request info object so that it can be used as part of the event processing structure to fufill findAddressOwnerRequests.
@param inputAddressToFind: The address we are trying to find the owner for
@param inputClosestKnownPredecessor: The next node to contact in the process of resolving the address owner
@param inputThisNode: The contact information of this node
@param inputFindAddressOwnerRequestNumber: The internal request number to keep track of which node function sent this request

@exceptions: This function can throw exceptions
*/
findAddressOwnerRequest(const boost::multiprecision::uint512_t &inputAddressToFind, const chordNodeContactInformation &inputClosestKnownPredecessor, const chordNodeContactInformation &inputThisNode, uint64_t inputFindAddressOwnerRequestNumber = 0);

/*
This function returns 1 if the given ID is the best successor of out of all of the nodes that have been seen while servicing this request.
@param inputAddressToTest: The address that might be the best successor
@return: 1 if it is the best successor and 0 otherwise
*/
int checkIfBestSuccessorSeen(const boost::multiprecision::uint512_t &inputAddressToTest);

/*
This function updates the request structure with given node if it is not already present in the structure.  Does nothing if the contact info is invalid.
@param inputPredecessor: The contact info associated with the node to add
@param inputInformationSource: The ID of the provider of the node info (for bad ref info)
*/
void addClosestKnownPredecessorIfNew(const chordNodeContactInformation &inputPredecessor, const chordNodeContactInformation &inputInformationSource);

~findAddressOwnerRequest(); //TODO: Change back

boost::multiprecision::uint512_t addressToFind;
std::string addressToFindBinary;

uint64_t findAddressOwnerRequestNumber; //The ID of this request to ensure functions internal to the node (which share a socket) can figure out which one it goes to
std::stack<boost::multiprecision::uint512_t> closestKnownPredecessors; //Make sure to check incoming entries using the map to ensure no entry is added more than once
std::map<boost::multiprecision::uint512_t, uint64_t> resendCount; //How many times a request has been resent to this node
std::map<boost::multiprecision::uint512_t, uint64_t> badReferenceCount; //How many bad references this node has given

std::map<boost::multiprecision::uint512_t, chordNodeContactInformation> idToContactInfo;
std::map<boost::multiprecision::uint512_t, boost::multiprecision::uint512_t> contactInfoSource; //Who gave the information for this node
};













#endif
