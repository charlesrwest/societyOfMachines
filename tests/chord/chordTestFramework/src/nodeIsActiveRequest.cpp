#include "nodeIsActiveRequest.hpp"

/*
This function initializes the request with the contact information of the node that is being checked and initializes the number of resends to 0.
*/
nodeIsActiveRequest::nodeIsActiveRequest(const chordNodeContactInformation &inputNodeToCheck)
{
nodeToCheckIfActive = inputNodeToCheck;
//Assign ID to corresponding entry
createUInt512FromRawHash(inputNodeToCheck.chord_id().c_str(), inputNodeToCheck.chord_id().size(), nodeToCheckIfActiveID);  //Fail silently if it doesn't deserialize

resendCount = 0;
}
