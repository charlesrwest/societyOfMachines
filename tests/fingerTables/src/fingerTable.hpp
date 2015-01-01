#ifndef FINGERTABLEH
#define FINGERTABLEH


#include<cmath>
#include<vector>
#include<algorithm>
#include<set>

#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"
#include "SOMException.hpp"
#include "chordNodeContactInformation.pb.h"

#define CHORD_ID_SIZE 64
/*
The purpose of this class is to maintain a fixed size table of nodes responsible for particular portions of the chord address space.  This means that it should support relatively fast lookup of the closest predecessor to a address space and figuring out if a given address is better than the current closest for each portion of the address space.
*/
class fingerTable
{
public:
/*
This function initializes the finger table and populates it with the given ID (presumably the chord node's ID). It also initializes the target chord addresses based on offsetting from the given ID.
@param inputSelfContactInfo: The contact information associated with this node (assumed to be valid from outside on the internet)
@param inputFingerTableSize: The number of nodes to have in the finger table

@exceptions: This function can throw exceptions
*/
fingerTable(const chordNodeContactInformation &inputSelfContactInfo, unsigned int inputFingerTableSize);

/*
This function returns the contact infos (including binary ID) of the closest chord nodes that are < than the given chord address
@param inputChordAddress: The chord address to find predecessors for
@return: A list of chord node contact infos, in order of closeness (closest to target first)

@exceptions: This function can throw exceptions
*/
std::vector<chordNodeContactInformation> getClosestPredecessors(const boost::multiprecision::uint512_t &inputChordAddress, unsigned int inputNumberOfNodesToReturn);

/*
This function takes a chord ID and checks if it is a better fit than one of the current best match entries (replacing the best match if so).  If a entry is closer to its upper_bound in the table than the corresponding entry in the best matches table, it replaces the entry in the best matches table.
@param inputChordNodeContactInfo: The chord info to test
*/
void checkAndUpdateIfBetter(const chordNodeContactInformation &inputChordNodeContactInfo);

boost::multiprecision::uint512_t chordID;  //The ID of this node

private:
/*
Update the best chord address match.  The index corresponds to the entry number of the bestChordAddressMatches.  This is the only way that the finger table entries should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputIndexNumber: A bounds checked index number
@param inputChordNodeContactInfo: The chord node contact info to insert

@exceptions: An exception is thrown if the index is out of bounds
*/
void updateChordMatch(int inputIndexNumber, const chordNodeContactInformation &inputChordNodeContactInfo);

/*
Update the best chord address match.  This is the only way that the predecessor entry should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputChordNodeContactInfo: The chord node contact info set the successor to
*/
void updatePredecessor(const chordNodeContactInformation &inputChordNodeContactInfo);

/*
Update the best chord address match.  This is the only way that the successor entry should be updated.  Invalid chordNodeContactInformations are silently ignored.
@param inputChordNodeContactInfo: The chord node contact info set the successor to
*/
void updateSuccessor(const chordNodeContactInformation &inputChordNodeContactInfo);

/*
This function is used to update the std::set and std::map that is used to answer queries.  It either adds the requested element to the finger table and  increments the associated reference or (if it already exists) increments the associated reference.  If the chord ID matches but the contact details differ, the contact entry will be updated with the new information.
@param inputNodeToAddContactInfo: The information associated with the node to add
return: 0 if an existing contact info was updated (changing the info for other entries pointing at it) and 1 otherwise

@exceptions: This function can throw exceptions
*/
int addQueryNodeInfo(const chordNodeContactInformation &inputNodeToAddContactInfo);

/*
This function is used to update the std::set and std::map that is used to answer queries.  It decrements the reference count associated with the node to remove and deletes the associated information if the reference count hits zero.  If the entry does not exist, it does nothing.
@param inputNodeToRemove: The ID of the node to remove from the search data structures
*/
void removeQueryNodeInfo(const boost::multiprecision::uint512_t &inputNodeToRemove);

/*
This function is used to update the std::set and std::map that is used to answer queries.  It decrements the reference count associated with the node to remove and deletes the associated information if the reference count hits zero.  It also either adds the requested element to the finger table and  increments the associated reference or (if it already exists) increments the associated reference.
@param inputNodeToRemove: The ID of the node to remove from the search data structures
@param inputNodeToAddContactInfo: The information associated with the node to add

@exceptions: This function can throw exceptions
*/
void replaceQueryNodeInfo(const boost::multiprecision::uint512_t &inputNodeToRemove, const chordNodeContactInformation &inputNodeToAddContactInfo);

//Where the fingers are suppose to find best matches for
std::vector<boost::multiprecision::uint512_t> targetChordAddresses;
std::vector<std::string> targetChordAddressesInBinary; //The binary version of the target chord addresses

boost::multiprecision::uint512_t predecessorID;  //The predecessor of this node
boost::multiprecision::uint512_t successorID;  //The successor of this node

//The current best matches and their associated contact infos
std::vector<boost::multiprecision::uint512_t> bestChordAddressMatchIDs;

//Data structures used for answering queries and storing contact info (only change using member functions)
std::set<boost::multiprecision::uint512_t> knownIDs;
std::map<boost::multiprecision::uint512_t, unsigned int> IDReferenceCounts; //How many finger table entries are pointing to a given ID.  The corresponding contact info is deleted when the count reaches 0.
std::map<boost::multiprecision::uint512_t, chordNodeContactInformation> idToContactInfo;
};




#endif
