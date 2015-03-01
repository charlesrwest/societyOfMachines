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
for(int i=0; i < inputFingerTableSize; i++)
{
targetChordAddresses.push_back(chordID+offset);

if(createRawHashFromUInt512(targetChordAddresses.back(), binaryBuffer, CHORD_ID_SIZE) != 1)
{
throw SOMException(std::string("Error creating raw hash from uint512\n"), SYSTEM_ERROR, __FILE__, __LINE__);
}
targetChordAddressesInBinary.push_back(std::string(binaryBuffer, CHORD_ID_SIZE));

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

/*
printf("Known IDs size: %ld\n", knownIDs.size());
int a=0;
for(auto iter = knownIDs.begin(); iter != knownIDs.end(); iter++)
{
printf("Known ID %d size: %ld\n", a, idToContactInfo[*iter].chord_id().size());
a++;
}

printf("Known ID rbegin entry size: %ld\n",idToContactInfo[*knownIDs.rbegin()].chord_id().size());
*/

while(closestPredecessors.size() < inputNumberOfNodesToReturn)
{
//Check all nodes we have info for to find closest predecessor ID (returning the associated iterator)
closestPredecessorIterator = std::lower_bound(knownIDs.begin(), knownIDs.end(), lastBestMatch);

//Check to make sure the result is modulo correct
if(lastBestMatch <= (*closestPredecessorIterator) || closestPredecessorIterator == knownIDs.end())
{
printf("test");
//The target address is smaller than the smallest value in the finger table, so it wraps around (making the closest predecessor the highest value in the set)
lastBestMatch = *(knownIDs.rbegin());  //Highest entry
printf("lastBestMatch id size: %ld\n",idToContactInfo[lastBestMatch].chord_id().size());
}
else
{
lastBestMatch = (*closestPredecessorIterator);
printf("lastBestMatch id size1: %ld\n",idToContactInfo[lastBestMatch].chord_id().size());
if(knownIDs.end() == closestPredecessorIterator)
{
printf("Yep\n");
}
}



closestPredecessors.push_back(idToContactInfo[lastBestMatch]);

} //End while loop

return closestPredecessors;
}

/*
This function returns the contact infos (including binary ID) of the closest chord nodes that are > than the given chord address (useful for finding the next best table entry when one goes out of date).
@param inputChordAddress: The chord address to find successors for
@return: A list of chord node contact infos, in order of closeness (closest to target first)

@exceptions: This function can throw exceptions
*/
std::vector<chordNodeContactInformation> fingerTable::getClosestSuccessors(const boost::multiprecision::uint512_t &inputChordAddress, unsigned int inputNumberOfNodesToReturn)
{
if(inputNumberOfNodesToReturn == 0)
{
return std::vector<chordNodeContactInformation>(); //Return empty vector
}

boost::multiprecision::uint512_t lastBestMatch = inputChordAddress;
std::vector<chordNodeContactInformation> closestSuccessors;
std::set<boost::multiprecision::uint512_t>::iterator closestSuccessorIterator;

while(closestSuccessors.size() < inputNumberOfNodesToReturn)
{
//Check all nodes we have info for to find closest predecessor ID (returning the associated iterator)
closestSuccessorIterator = std::upper_bound(knownIDs.begin(), knownIDs.end(), lastBestMatch);

//Check to make sure the result is modulo correct
if(lastBestMatch >= (*closestSuccessorIterator) || closestSuccessorIterator == knownIDs.end())
{
//The target address is larger than the largest value in the finger table, so it wraps around (making the closest predecessor the lowest value in the set)
lastBestMatch = *knownIDs.begin();  //Highest entry
}
else
{
lastBestMatch = (*closestSuccessorIterator);
}

closestSuccessors.push_back(idToContactInfo[lastBestMatch]);

} //End while loop

return closestSuccessors;
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

//Check if the input information is stale (possibly no longer valid)
bool inputNodeIsStale = false;
if(inputChordNodeContactInfo.has_is_stale())
{
inputNodeIsStale = inputChordNodeContactInfo.is_stale();
}

//Update each finger table entry (could be potentially optimized)
for(auto targetIterator = targetChordAddresses.begin(); targetIterator != targetChordAddresses.end(); targetIterator++)
{
auto currentBestMatchID = bestChordAddressMatchIDs[targetIterator - targetChordAddresses.begin()];

bool currentBestMatchIsStale = false;
if(idToContactInfo[currentBestMatchID].has_is_stale())
{
currentBestMatchIsStale = idToContactInfo[currentBestMatchID].is_stale();
}

//Check if the given address is closer to the target than the current entry or less stale
if((((*targetIterator) - currentBestMatchID) > ((*targetIterator) - inputChordID)) || (currentBestMatchIsStale && !inputNodeIsStale))
{
//New id is better
SOM_TRY
updateChordMatch(targetIterator - targetChordAddresses.begin(), inputChordNodeContactInfo);
SOM_CATCH("Error updating chord table entry\n")
}
}



//Now check if it should be our successor
if(((successorID - chordID) > (inputChordID-chordID) && inputChordID != chordID) || successorID == chordID)
{
//New id is better
SOM_TRY
updateSuccessor(inputChordNodeContactInfo);
SOM_CATCH("Error updating successor contact info\n")
}

//Check if it should be our predecessor
if(((chordID - predecessorID) > (chordID - inputChordID) && inputChordID != chordID) || predecessorID == chordID)
{
//New id is better
SOM_TRY
updatePredecessor(inputChordNodeContactInfo);
SOM_CATCH("Error updating successor contact info\n")
}

}

/*
This function replaces the given node with the next best option in all finger table entries it is used in (unless it is this node's ID, in which case it does nothing).
@param inputChordNodeContactInfo: The chord info to remove
*/
void fingerTable::replaceWithNextBestOption(const chordNodeContactInformation &inputChordNodeContactInfo)
{
//Verify input
if(inputChordNodeContactInfo.chord_id().size() != CHORD_ID_SIZE)
{
return; //Invalid input, so skip
}

//Store for replace operation
boost::multiprecision::uint512_t inputChordID;

//Get the ID associated with the node to replace
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), inputChordID) != 1)
{
return; //Couldn't make int from array
}

//Make sure it isn't our own ID
if(inputChordID == chordID)
{
return;
}

//Retrieve the next best option
chordNodeContactInformation nextBestNode;
SOM_TRY
nextBestNode = getClosestSuccessors(inputChordID, 1)[0];
SOM_CATCH("Error retrieving next best successor\n")

//Replace all finger table entries that point at the contact info we are replacing
for(auto iter = bestChordAddressMatchIDs.begin(); iter != bestChordAddressMatchIDs.begin(); iter++)
{
if((*iter) == inputChordID)
{
SOM_TRY
updateChordMatch(iter - bestChordAddressMatchIDs.begin(), nextBestNode);
SOM_CATCH("Error updating chord match\n")
}
}

if(predecessorID == inputChordID)
{
SOM_TRY
updatePredecessor(nextBestNode);
SOM_CATCH("Error updating predecessor\n")
}

if(successorID == inputChordID)
{
SOM_TRY
updateSuccessor(nextBestNode);
SOM_CATCH("Error updating successor\n")
}

}

/*
This function returns the number of entries in the finger table (number of target addresses).
@return: The number of entries in the finger table
*/
uint64_t fingerTable::fingerTableSize()
{
return targetChordAddresses.size();
}

/*
This function returns the contact information associated with this node.
@return: The contact info associated with this node (from its perspective)

@exceptions: This function can throw an exception if the associated contact info cannot be found (some sort of internal error making the state invalid)
*/
chordNodeContactInformation fingerTable::selfContactInfo()
{
if(idToContactInfo.count(chordID) == 1)
{
return idToContactInfo[chordID];
}

//Somehow there is no contact info for the self ID
throw SOMException(std::string("No contact info in finger table for self ID\n"), AN_ASSUMPTION_WAS_VIOLATED_ERROR, __FILE__, __LINE__);
}

/*
This function returns the contact information associated with this node's predecessor.
@return: The contact info associated with this node (from its perspective)

@exceptions: This function can throw an exception if the associated contact info cannot be found (some sort of internal error making the state invalid)
*/
chordNodeContactInformation fingerTable::predecessorContactInfo()
{
if(idToContactInfo.count(predecessorID) == 1)
{
return idToContactInfo[predecessorID];
}

//Somehow there is no contact info for the predecessor ID
throw SOMException(std::string("No contact info in finger table for predecessor ID\n"), AN_ASSUMPTION_WAS_VIOLATED_ERROR, __FILE__, __LINE__);
}

/*
This function returns the contact information associated with this node's successor.
@return: The contact info associated with this node (from its perspective)

@exceptions: This function can throw an exception if the associated contact info cannot be found (some sort of internal error making the state invalid)
*/
chordNodeContactInformation fingerTable::successorContactInfo()
{
if(idToContactInfo.count(successorID) == 1)
{
return idToContactInfo[successorID];
}

//Somehow there is no contact info for the successor ID
printf("THERE IS AN EXCEPTION!!!!!!!\n");
throw SOMException(std::string("No contact info in finger table for successor ID\n"), AN_ASSUMPTION_WAS_VIOLATED_ERROR, __FILE__, __LINE__);
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
This function give a string that has a text summary of the values of the finger table values (primarily used for debugging).
@return: A text report with the self/predecessor/successor/finger table values
*/
std::string fingerTable::getFingerTableSummary()
{
std::string summary;
std::string buffer;

//Add self ID information
summary = "Self ID: ";
buffer = convertUInt512ToDecimalString(chordID);
summary += buffer.substr(0,5-(155-buffer.size()));
summary += "\n";

//Add predecessor info
summary += "Predecessor ID: ";
buffer = convertUInt512ToDecimalString(predecessorID);
summary += buffer.substr(0,5-(155-buffer.size()));
summary += "\n";

//Add successor info
summary += "Successor ID: ";
buffer = convertUInt512ToDecimalString(successorID);
summary += buffer.substr(0,5-(155-buffer.size()));
summary += "\n";

//Add finger table info
if(bestChordAddressMatchIDs.size() != targetChordAddresses.size())
{
summary += "Best chord addresses vector size does not match target chord addresses vector size (" + std::to_string(bestChordAddressMatchIDs.size()) + " " + std::to_string(targetChordAddresses.size()) + "\n";
return summary;
}

for(int i=0; i<bestChordAddressMatchIDs.size(); i++)
{
summary += "Best predecessor address " + std::to_string(i) + ": ";
buffer = convertUInt512ToDecimalString(bestChordAddressMatchIDs[i]);
summary += buffer.substr(0,5-(155-buffer.size())) + "\n";

summary += "Target address " + std::to_string(i) + ": ";
buffer = convertUInt512ToDecimalString(targetChordAddresses[i]);
summary += buffer.substr(0,5-(155-buffer.size())) + "\n";
}

return summary;
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
boost::multiprecision::uint512_t buffer = successorID;

//Assign ID to corresponding entry
if(createUInt512FromRawHash(inputChordNodeContactInfo.chord_id().c_str(), inputChordNodeContactInfo.chord_id().size(), successorID) != 1)
{
printf("Couldn't make INT!!!!!!\n");
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

