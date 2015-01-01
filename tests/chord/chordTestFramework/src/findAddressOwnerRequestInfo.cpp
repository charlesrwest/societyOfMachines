#include "findAddressOwnerRequestInfo.hpp"

/*
This function initializes the request info object so that it can be used as part of the event processing structure to fufill findAddressOwnerRequests.
@param inputAddressToFind: The address we are trying to find the owner for
@param inputClosestKnownPredecessorID: The next node to contact in the process of resolving the address owner

@exceptions: This function can throw exceptions
*/
findAddressOwnerRequestInfo::findAddressOwnerRequestInfo(const boost::multiprecision::uint512_t &inputAddressToFind, const boost::multiprecision::uint512_t &inputClosestKnownPredecessorID)
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

lastClosestKnownPredecessorChordID = inputClosestKnownPredecessorID;
}
