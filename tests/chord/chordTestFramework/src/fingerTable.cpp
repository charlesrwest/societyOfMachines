#include "fingerTable.hpp"

/*
This function initializes the finger table and populates it with the given ID (presumably the chord node's ID). It also initializes the target chord addresses based on offsetting from the given ID.
@param inputChordID: The ID associated with the node that owns the finger table

@exceptions: This function can throw exceptions
*/
fingerTable::fingerTable(const boost::multiprecision::uint512_t &inputChordID)
{
char binaryBuffer[64];

chordID = inputChordID;
if(createRawHashFromUInt512(inputChordID, binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
chordIDInBinary = std::string(binaryBuffer);

//Generate target addresses
boost::multiprecision::uint512_t offset = 1;
for(int i=0; i<512; i++)
{
targetChordAddresses.push_back(chordID+offset);

if(createRawHashFromUInt512(targetChordAddresses.back(), binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
targetChordAddressesInBinary.push_back(std::string(binaryBuffer));

offset = offset*2;
}

//Set defaults until better ones are found
for(int i=0; i<targetChordAddresses.size(); i++)
{
bestChordAddressMatches.push_back(chordID);
bestChordAddressMatchesInBinary.push_back(chordIDInBinary);
}

successorID = chordID;
successorIDInBinary = chordIDInBinary;

predecessorID = chordID;
predecessorIDInBinary = chordIDInBinary;
}

/*
This function returns the binary version of the closest chord ID entry that is < than the given chord address
@param inputChordAddress: The chord address to find a predecessor for
@return: The binary version of the predecessor
*/
std::string fingerTable::getClosestPredecessor(const boost::multiprecision::uint512_t &inputChordAddress)
{
//Currently just checking the finger table, not successor/self/predecessor
std::vector<boost::multiprecision::uint512_t>::iterator closestPredecessorIterator;
closestPredecessorIterator = std::lower_bound(bestChordAddressMatches.begin(), bestChordAddressMatches.end(), inputChordAddress);

//Check to make sure the result is modulo correct
if(inputChordAddress < (*closestPredecessorIterator))
{
//The address is smaller than the smallest value in the finger table, so it wraps around
return bestChordAddressMatchesInBinary.back();
}


return bestChordAddressMatchesInBinary[closestPredecessorIterator - bestChordAddressMatches.begin()];
}

/*
Update the best chord address match (binary and formatted).  The index corresponds to the entry number of the bestChordAddressMatches.
@param inputIndexNumber: A bounds checked index number
@param inputChordAddress: The chord ID to insert

@exceptions: An exception is thrown if the index is out of bounds
*/
void fingerTable::updateChordMatch(int inputIndexNumber, const boost::multiprecision::uint512_t &inputChordID)
{
if(inputIndexNumber < 0 || inputIndexNumber >= bestChordAddressMatches.size())
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
} 

bestChordAddressMatches[inputIndexNumber] = inputChordID;

char binaryBuffer[64];
if(createRawHashFromUInt512(targetChordAddresses.back(), binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
bestChordAddressMatchesInBinary[inputIndexNumber] = std::string(binaryBuffer);

}

//If a entry is closer to its upper_bound in the table than the corresponding entry in the best matches table, it replaces the entry in the best matches table
/*
This function takes a chord ID and checks if it is a better fit than one of the current best match entries (replacing the best match if so).
@param inputChordAddress: The chord ID to test
*/
void fingerTable::checkAndUpdateTableIfBetter(const boost::multiprecision::uint512_t &inputChordID)
{
//Small problem in that this implementation only updates one entry (and there can be duplicates)

//Find which target address this address would be most appropriate for
std::vector<boost::multiprecision::uint512_t>::iterator closestTargetIterator;
closestTargetIterator = std::upper_bound(targetChordAddresses.begin(), targetChordAddresses.end(), inputChordID);

//Check if modulo wrap is needed
if((*closestTargetIterator) <  inputChordID)
{
closestTargetIterator = targetChordAddresses.begin();
}

//Check if the given address is closer to the target than the current entry
if(((*closestTargetIterator) - bestChordAddressMatches[closestTargetIterator - targetChordAddresses.begin()]) > ((*closestTargetIterator) - inputChordID))
{
//New id is better
bestChordAddressMatches[closestTargetIterator - targetChordAddresses.begin()] = inputChordID;

char binaryBuffer[64];
if(createRawHashFromUInt512(inputChordID, binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
bestChordAddressMatchesInBinary[closestTargetIterator - targetChordAddresses.begin()] = std::string(binaryBuffer);
}

//Now check if it should be our successor
if((successorID - chordID) > (inputChordID-chordID) || successorID == chordID)
{
//New id is better
successorID = inputChordID;

char binaryBuffer[64];
if(createRawHashFromUInt512(inputChordID, binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
successorIDInBinary = std::string(binaryBuffer);
}

//Check if it should be our predecessor
if((chordID - predecessorID) > (chordID - inputChordID) || predecessorID == chordID)
{
//New id is better
predecessorID = inputChordID;

char binaryBuffer[64];
if(createRawHashFromUInt512(inputChordID, binaryBuffer, 64) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
successorIDInBinary = std::string(binaryBuffer);
}

}
