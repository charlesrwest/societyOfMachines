#include "fingerTable.hpp"

/*
This function initializes the finger table and populates it with the given ID (presumably the chord node's ID). It also initializes the target chord addresses based on offsetting from the given ID.
@param inputSelfContactInfo: The contact information associated with this node (assumed to be valid from outside on the internet)
@param inputFingerTableSize: The number of nodes to have in the finger table

@exceptions: This function can throw exceptions
*/
fingerTable::fingerTable(const chordNodeContactInformation &inputSelfContactInfo, unsigned int inputFingerTableSize)
{
//Validate input
if(inputSelfContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
throw SOMException(std::string("Error, chord ID is invalid size\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

if(inputFingerTableSize > (CHORD_ID_SIZE*8-1))
{
throw SOMException(std::string("Error, finger table size is too big\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Assign ID to ourselves
if(createUInt512FromRawHash(inputSelfContactInfo.chord_id().c_str(), inputSelfContactInfo.chord_id().size(), chordID) != 1)
{
throw SOMException(std::string("Error, could not create host version of chord ID\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Assign our contact info
SOM_TRY
addQueryNodeInfo(inputSelfContactInfo);
SOM_CATCH("Error, unable to add self ID to search space")

//Create offset multiplier base
boost::multiprecision::uint512_t offset = 1;
for(int i=0; i < (CHORD_ID_SIZE*8 - inputFingerTableSize); i++)
{
offset = offset*2;
}

//Generate target addresses
char binaryBuffer[CHORD_ID_SIZE];
for(int i=1; i < inputFingerTableSize; i++)
{
targetChordAddresses.push_back(chordID+offset);

if(createRawHashFromUInt512(targetChordAddresses.back(), binaryBuffer, CHORD_ID_SIZE) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
targetChordAddressesInBinary.push_back(std::string(binaryBuffer));

offset = offset*2;
}

//Set defaults until better ones are found
for(int i=0; i<targetChordAddresses.size(); i++)
{
bestChordAddressMatchIDs.push_back(chordID);

//Add the associated references to the query structures
SOM_TRY
addQueryNodeInfo(idToContactInfo[chordID]);
SOM_CATCH("Error, unable to add self ID as default finger table entry")
}

successorID = chordID;
//Add the associated references to the query structures
SOM_TRY
addQueryNodeInfo(idToContactInfo[chordID]);
SOM_CATCH("Error, unable to add self ID as default successor")

predecessorID = chordID;
//Add the associated references to the query structures
SOM_TRY
addQueryNodeInfo(idToContactInfo[chordID]);
SOM_CATCH("Error, unable to add self ID as default predecessor")
}

/*
This function returns the contact infos (including binary ID) of the closest chord nodes that are < than the given chord address
@param inputChordAddress: The chord address to find predecessors for
@return: A list of chord node contact infos, in order of closeness (closest to target first)

@exceptions: This function can throw exceptions
*/
std::vector<chordNodeContactInformation> fingerTable::getClosestPredecessors(const boost::multiprecision::uint512_t &inputChordAddress, unsigned int inputNumberOfNodesToReturn)
{
if(inputNumberOfNodesToReturn == 0)
{
return std::vector<chordNodeContactInformation>(); //Return empty vector
}

boost::multiprecision::uint512_t lastBestMatch = inputChordAddress;
std::vector<chordNodeContactInformation> closestPredecessors;
std::set<boost::multiprecision::uint512_t>::iterator closestPredecessorIterator;

while(closestPredecessors.size() < inputNumberOfNodesToReturn)
{
//Check all nodes we have info for to find closest predecessor ID (returning the associated iterator)
closestPredecessorIterator = std::lower_bound(knownIDs.begin(), knownIDs.end(), lastBestMatch);

//Check to make sure the result is modulo correct
if(lastBestMatch <= (*closestPredecessorIterator))
{
//The target address is smaller than the smallest value in the finger table, so it wraps around (making the closest predecessor the highest value in the set)
lastBestMatch = *knownIDs.rbegin();  //Highest entry
}
else
{
lastBestMatch = (*closestPredecessorIterator);
}

closestPredecessors.push_back(idToContactInfo[lastBestMatch]);

} //End while loop

return closestPredecessors;
}


/*
This function takes a chord ID and checks if it is a better fit than one of the current best match entries (replacing the best match if so).  If a entry is closer to its upper_bound in the table than the corresponding entry in the best matches table, it replaces the entry in the best matches table.
@param inputChordNodeContactInfo: The chord info to test
*/
void fingerTable::checkAndUpdateIfBetter(const chordNodeContactInformation &inputChordNodeContactInfo)
{
//Verify input
if(inputChordNodeContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
return; //Invalid input, so skip
}

//Store for replace operation
boost::multiprecision::uint512_t inputChordID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), inputChordID) != 1)
{
return; //Couldn't make int from array
}


//Update each finger table entry (could be potentially optimized)
for(auto targetIterator = targetChordAddresses.begin(); targetIterator != targetChordAddresses.end(); targetIterator++)
{
//Check if the given address is closer to the target than the current entry
if(((*targetIterator) - bestChordAddressMatchIDs[targetIterator - targetChordAddresses.begin()]) > ((*targetIterator) - inputChordID))
{
//New id is better
bestChordAddressMatchIDs[targetIterator - targetChordAddresses.begin()] = inputChordID;

SOM_TRY
updateChordMatch(targetIterator - targetChordAddresses.begin(), inputChordNodeContactInfo);
SOM_CATCH("Error updating chord table entry\n")
}
}



//Now check if it should be our successor
if((successorID - chordID) > (inputChordID-chordID) || successorID == chordID)
{
//New id is better
successorID = inputChordID;

SOM_TRY
updateSuccessor(inputChordNodeContactInfo);
SOM_CATCH("Error updating successor contact info\n")
}

//Check if it should be our predecessor
if((chordID - predecessorID) > (chordID - inputChordID) || predecessorID == chordID)
{
//New id is better
predecessorID = inputChordID;

SOM_TRY
updatePredecessor(inputChordNodeContactInfo);
SOM_CATCH("Error updating successor contact info\n")
}

}


/*
Update the best chord address match.  The index corresponds to the entry number of the bestChordAddressMatchIDs.  This is the only way that the finger table entries should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputIndexNumber: A bounds checked index number
@param inputChordNodeContactInfo: The chord node contact info to insert

@exceptions: An exception is thrown if the index is out of bounds
*/
void fingerTable::updateChordMatch(int inputIndexNumber, const chordNodeContactInformation &inputChordNodeContactInfo)
{
if(inputIndexNumber < 0 || inputIndexNumber >= bestChordAddressMatchIDs.size())
{
throw SOMException(std::string("Error invalid index value\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
} 

//Verify input
if(inputChordNodeContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
return; //Invalid input, so skip
}

//Store for replace operation
boost::multiprecision::uint512_t buffer = bestChordAddressMatchIDs[inputIndexNumber];

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), bestChordAddressMatchIDs[inputIndexNumber]) != 1)
{
return; //Couldn't make int from array
}

//Replace the associated references to the query structures
SOM_TRY
replaceQueryNodeInfo(buffer, inputChordNodeContactInfo);
SOM_CATCH("Error, unable to update query structure info for a finger table entry")
}



/*
Update the best chord address match.  This is the only way that the predecessor entry should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputChordNodeContactInfo: The chord node contact info set the successor to
*/
void fingerTable::updatePredecessor(const chordNodeContactInformation &inputChordNodeContactInfo)
{
//Verify input
if(inputChordNodeContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
return; //Invalid input, so skip
}

//Store for replace operation
boost::multiprecision::uint512_t buffer = predecessorID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), predecessorID) != 1)
{
return; //Couldn't make int from array
}

//Replace the associated references to the query structures
SOM_TRY
replaceQueryNodeInfo(buffer, inputChordNodeContactInfo);
SOM_CATCH("Error, unable to update query structure info for a predecessor entry")
}

/*
Update the best chord address match.  This is the only way that the successor entry should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputChordNodeContactInfo: The chord node contact info set the successor to
*/
void fingerTable::updateSuccessor(const chordNodeContactInformation &inputChordNodeContactInfo)
{
//Verify input
if(inputChordNodeContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
return; //Invalid input, so skip
}

//Store for replace operation
boost::multiprecision::uint512_t buffer = predecessorID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), successorID) != 1)
{
return; //Couldn't make int from array
}

//Replace the associated references to the query structures
SOM_TRY
replaceQueryNodeInfo(buffer, inputChordNodeContactInfo);
SOM_CATCH("Error, unable to update query structure info for a successor entry")
}

/*
This function is used to update the std::set and std::map that is used to answer queries.  It either adds the requested element to the finger table and  increments the associated reference or (if it already exists) increments the associated reference.  If the chord ID matches but the contact details differ, the contact entry will be updated with the new information.
@param inputNodeToAddContactInfo: The information associated with the node to add
return: 0 if an existing contact info was updated (changing the info for other entries pointing at it) and 1 otherwise

@exceptions: This function can throw exceptions
*/
int fingerTable::addQueryNodeInfo(const chordNodeContactInformation &inputNodeToAddContactInfo)
{
//Verify input
if(inputNodeToAddContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
throw SOMException(std::string("Error, chord ID entry is the wrong size\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Assign ID to corresponding entry
boost::multiprecision::uint512_t chordAddressBuffer;
if(createUInt512FromRawHash(inputNodeToAddContactInfo.chord_id().c_str(), inputNodeToAddContactInfo.chord_id().size(), chordAddressBuffer) != 1)
{
throw SOMException(std::string("Error, unable to deserialize chord ID\n"), INVALID_FUNCTION_INPUT, __FILE__, __LINE__);
}

//Check if this ID already has an entry
if(IDReferenceCounts.count(chordAddressBuffer) == 1)
{
//Reference exists, so increment and update contact details if needed
IDReferenceCounts[chordAddressBuffer]++; //Increment associated reference count

//Update contact details if they differ
if(idToContactInfo[chordAddressBuffer].ip() != inputNodeToAddContactInfo.ip() || idToContactInfo[chordAddressBuffer].port() != inputNodeToAddContactInfo.port() || idToContactInfo[chordAddressBuffer].virtual_port() != inputNodeToAddContactInfo.virtual_port())
{
idToContactInfo[chordAddressBuffer] = inputNodeToAddContactInfo;
return 0;
}

return 1;
}

//It doesn't have an entry yet, so add it to the required datastructures
knownIDs.insert(chordAddressBuffer);
IDReferenceCounts[chordAddressBuffer] = 1; //Add reference count entry
idToContactInfo[chordAddressBuffer] = inputNodeToAddContactInfo;
return 1;
}

/*
This function is used to update the std::set and std::map that is used to answer queries.  It decrements the reference count associated with the node to remove and deletes the associated information if the reference count hits zero.  If the entry does not exist, it does nothing.
@param inputNodeToRemove: The ID of the node to remove from the search data structures
*/
void fingerTable::removeQueryNodeInfo(const boost::multiprecision::uint512_t &inputNodeToRemove)
{
//Check if this ID already has an entry
if(IDReferenceCounts.count(inputNodeToRemove) == 0)
{
//Reference doesn't exist, so do nothing since it can't be removed
return;
}

if(IDReferenceCounts[inputNodeToRemove] == 1)
{
//Reference count will now reach zero, so remove associated information
knownIDs.erase(inputNodeToRemove);
IDReferenceCounts.erase(inputNodeToRemove);
idToContactInfo.erase(inputNodeToRemove);
return;
}

//References haven't reached zero yet, so simple decrement the reference count
IDReferenceCounts[inputNodeToRemove]--;
}

/*
This function is used to update the std::set and std::map that is used to answer queries.  It decrements the reference count associated with the node to remove and deletes the associated information if the reference count hits zero.  It also either adds the requested element to the finger table and  increments the associated reference or (if it already exists) increments the associated reference.
@param inputNodeToRemove: The ID of the node to remove from the search data structures
@param inputNodeToAddContactInfo: The information associated with the node to add

@exceptions: This function can throw exceptions
*/
void fingerTable::replaceQueryNodeInfo(const boost::multiprecision::uint512_t &inputNodeToRemove, const chordNodeContactInformation &inputNodeToAddContactInfo)
{
SOM_TRY
addQueryNodeInfo(inputNodeToAddContactInfo);
SOM_CATCH("Error adding node info as part of replacement operation\n")

removeQueryNodeInfo(inputNodeToRemove);
}

