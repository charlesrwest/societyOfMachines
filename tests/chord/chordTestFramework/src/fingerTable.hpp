#ifndef FINGERTABLEH
#define FINGERTABLEH


#include<cmath>
#include<vector>
#include<algorithm>

#include <boost/multiprecision/cpp_int.hpp>
#include "hashAndBigNumberFunctions.hpp"
#include "SOMException.hpp"

/*
The purpose of this class is to maintain a fixed size table of nodes responsible for particular portions of the chord address space.  This means that it should support relatively fast lookup of the closest predecessor to a address space and figuring out if a given address is better than the current closest for each portion of the address space.

//To start with, I am having 512 entries in the table
*/
class fingerTable
{
public:
/*
This function initializes the finger table and populates it with the given ID (presumably the chord node's ID). It also initializes the target chord addresses based on offsetting from the given ID.
@param inputChordID: The ID associated with the node that owns the finger table

@exceptions: This function can throw exceptions
*/
fingerTable(const boost::multiprecision::uint512_t &inputChordID);

/*
This function returns the binary version of the closest chord ID entry that is < than the given chord address
@param inputChordAddress: The chord address to find a predecessor for
@return: The binary version of the predecessor
*/
std::string getClosestPredecessor(const boost::multiprecision::uint512_t &inputChordAddress);

/*
Update the best chord address match (binary and formatted).  The index corresponds to the entry number of the bestChordAddressMatches.
@param inputIndexNumber: A bounds checked index number
@param inputChordAddress: The chord ID to insert

@exceptions: An exception is thrown if the index is out of bounds
*/
void updateChordMatch(int inputIndexNumber, const boost::multiprecision::uint512_t &inputChordID);

//If a entry is closer to its upper_bound in the table than the corresponding entry in the best matches table, it replaces the entry in the best matches table
/*
This function takes a chord ID and checks if it is a better fit than one of the current best match entries (replacing the best match if so).
@param inputChordAddress: The chord ID to test
*/
void checkAndUpdateTableIfBetter(const boost::multiprecision::uint512_t &inputChordID);

std::vector<boost::multiprecision::uint512_t> bestChordAddressMatches;
std::vector<std::string> bestChordAddressMatchesInBinary; //The binary version of the target chord addresses

std::vector<boost::multiprecision::uint512_t> targetChordAddresses;
std::vector<std::string> targetChordAddressesInBinary; //The binary version of the target chord addresses

boost::multiprecision::uint512_t chordID;  //The ID of this node
std::string chordIDInBinary; //The binary version of the the chord ID

boost::multiprecision::uint512_t successorID;  //The successor of this node
std::string successorIDInBinary; //The binary version

boost::multiprecision::uint512_t predecessorID;  //The predecessor of this node
std::string predecessorIDInBinary; //The binary version
};




#endif
