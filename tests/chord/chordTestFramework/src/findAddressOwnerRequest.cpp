#include "findAddressOwnerRequest.hpp"

/*
This function initializes the request info object so that it can be used as part of the event processing structure to fufill findAddressOwnerRequests.
@param inputAddressToFind: The address we are trying to find the owner for
@param inputClosestKnownPredecessor: The next node to contact in the process of resolving the address owner
@param inputThisNode: The contact information of this node
@param inputFindAddressOwnerRequestNumber: The internal request number to keep track of which node function sent this request

@exceptions: This function can throw exceptions
*/
findAddressOwnerRequest::findAddressOwnerRequest(const boost::multiprecision::uint512_t &inputAddressToFind, const chordNodeContactInformation &inputClosestKnownPredecessor, const chordNodeContactInformation &inputThisNode, uint64_t inputFindAddressOwnerRequestNumber)
{
addressToFind = inputAddressToFind;

//TODO: Do something so there aren't so many magic numbers

//Get binary version of address to find
char addressToFindBinaryBuffer[64];
if(createRawHashFromUInt512(inputAddressToFind, addressToFindBinaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error, unable to convert chord address to binary\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}
addressToFindBinary = std::string(addressToFindBinaryBuffer, 64);

//Add first node to contact
addClosestKnownPredecessorIfNew(inputClosestKnownPredecessor, inputThisNode);

findAddressOwnerRequestNumber = inputFindAddressOwnerRequestNumber;
printf("Finished initializing request\n");
}

/*
This function returns 1 if the given ID is the best successor of out of all of the nodes that have been seen while servicing this request.
@param inputAddressToTest: The address that might be the best successor
@return: 1 if it is the best successor and 0 otherwise
*/
int findAddressOwnerRequest::checkIfBestSuccessorSeen(const boost::multiprecision::uint512_t &inputAddressToTest)
{
boost::multiprecision::uint512_t distanceFromTarget = inputAddressToTest - addressToFind;

for(auto iter = idToContactInfo.begin(); iter != idToContactInfo.end(); iter++)
{
if((iter->first - addressToFind) < distanceFromTarget)
{
return 0;
}
}

//Beat all other candidates
return 1;
}

/*
This function updates the request structure with given node if it is not already present in the structure.  Does nothing if the contact info is invalid.
@param inputPredecessor: The contact info associated with the node to add
@param inputInformationSource: The ID of the provider of the node info (for bad ref info)
*/
void findAddressOwnerRequest::addClosestKnownPredecessorIfNew(const chordNodeContactInformation &inputPredecessor, const chordNodeContactInformation &inputInformationSource)
{
//Get associated chord IDs
boost::multiprecision::uint512_t deserializedPredecessorID;
boost::multiprecision::uint512_t deserializedInformationSourceID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputPredecessor.chord_id().c_str(), inputPredecessor.chord_id().size(), deserializedPredecessorID) != 1)
{
printf("Deserialization failed (size: %ld)\n", inputPredecessor.chord_id().size());
return; //Couldn't make int from array
}

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputInformationSource.chord_id().c_str(), inputInformationSource.chord_id().size(), deserializedInformationSourceID) != 1)
{
printf("Deserialization failed2\n");
return; //Couldn't make int from array
}

//Check if we have that node already
if(idToContactInfo.count(deserializedPredecessorID) > 0)
{
printf("We already have that one\n");
return; //We do
}

//Check if the replier is not who we expect (last node we talked to)
if(closestKnownPredecessors.size() > 0)
{
if(closestKnownPredecessors.top() != deserializedInformationSourceID)
{
//Someone else responded to this request (not suppose to), so ignore it
return;
}
}

//Add the information source if we don't have the contact details for it
if(idToContactInfo.count(deserializedInformationSourceID) == 0)
{
idToContactInfo[deserializedInformationSourceID] = inputInformationSource;
contactInfoSource[deserializedInformationSourceID] = deserializedInformationSourceID; //Set info source as itself
resendCount[deserializedInformationSourceID] = 0;
badReferenceCount[deserializedInformationSourceID] = 0;
}

//Assign to datastructures
idToContactInfo[deserializedPredecessorID] = inputPredecessor;
contactInfoSource[deserializedPredecessorID] = deserializedInformationSourceID;
resendCount[deserializedPredecessorID] = 0;
badReferenceCount[deserializedPredecessorID] = 0;
closestKnownPredecessors.push(deserializedPredecessorID);
}


findAddressOwnerRequest::~findAddressOwnerRequest()
{
printf("findAddressOwnerRequest destructoring\n");
fflush(stdout);
}
