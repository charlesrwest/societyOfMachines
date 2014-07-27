#ifndef LOCALURIDATABASEH
#define LOCALURIDATABASEH

#include<string>
#include<vector>
#include<stdio.h>
#include "sqlite3.h"
#include "../messages/localURI.pb.h"
#include "../messages/localURIQuery.pb.h"

#define MAX_NUMBER_OF_CHARACTERS_IN_RESOURCE_NAME 80
#define MAX_NUMBER_OF_TAGS_PER_URI 50
#define MAX_NUMBER_OF_CHARACTERS_IN_TAG 80
#define MAX_NUMBER_OF_INTEGER_KEY_VALUE_PAIRS 50
#define MAX_NUMBER_OF_CHARACTERS_IN_INTEGER_KEY 80
#define MAX_NUMBER_OF_DOUBLE_KEY_VALUE_PAIRS 50
#define MAX_NUMBER_OF_CHARACTERS_IN_DOUBLE_KEY 80

#define MAX_NUMBER_OF_CHARACTERS_IN_IPC_PATH 80

/*////////////////////////////////////////////////////////////////////////
Apparently you need to manually call rollback when a error occurs to make an actual atomic transaction.  If you do not, it will just execute everything except for what failed.  As it currently stands, the code does not do this.  This needs to be fixed.

Also, apparently foreign key constraints are ignored unless you enable them with a pragma at the start of your connection.  The required pragma is as follows:
PRAGMA foreign_keys=on;

It might be wise to consider creating a index on each uriID with a foreigh key constraint:
CREATE INDEX trackindex ON track(trackartist);

tags values should be made not null, as should integer key value pairs and double key value pairs (all fields).

Make a destructor for this class

*/////////////////////////////////////////////////////////////////////////


/*
This class creates an in memory (ram) SQLITE3 database and has convience functions to allow the addion, removal and searching of local URIs
*/
class localURIDatabase
{
public:
/*
This function initializes the object and (by default) creates a in memory (ram) SQL3Lite database. However, if input is given, the function will try to connect to and check the given database.  If inputCreateExternalDatabase is also set to true, the function will make a external database file (useful for logging and debugging) rather than an in memory database.
@param inputDatabase: The (optional) string of the path to find/create the database for this object to use
@param inputCreateExternalDatabase: If true, the object will try to create the database file if it isn't found.  Ignored if the inputDatabase string is empty.
*/
localURIDatabase(const std::string &inputDatabase = "", bool inputCreateExternalDatabase = false);

/*
This function adds a local URI to the database.  It takes a libprotobuf message, converts it into something that can be added into the database and then performs an insertion operation.  All of the pieces of the URI are inserted in the same transaction, so either the entire URI is stored or none of it is (and an error is returned).
@param inputLocalURI: The local URI to be added (likely from a ZMQ message)
@return: 1 if successful and 0 otherwise
*/
int addURI(const localURI &inputLocalURI);

/*
This function removes the URI with the given ID from the database (ID can be retrieved with queries).
@param inputLocalURIID: The database given ID of the URI
@return: 1 if successful and 0 otherwise
*/
int removeURI(uint64_t inputLocalURIID);

/*
This function removes the URI with the resource name from the database.
@param inputLocalURIID: The database given ID of the URI
@return: 1 if successful and 0 otherwise
*/
int removeURI(const std::string &inputResourceName);


/*
This function takes a libprotobuf query object and runs the query in the database.  The result is returned as a set of libprotobuf objects.  The queries are restricted to the same size limitations as localURIs as far as the number of tags and key value pairs.
@param inputLocalURIQuery: This is the query to process
@param inputResultBuffer: The buffer to place set of localURIs that match the given criteria in (restricted to a maximum of query.maximumNumberOfResults)
@return: 1 if successful and 0 otherwise
*/
int processQuery(const localURIQuery &inputLocalURIQuery, std::vector<localURI> &inputResultBuffer);

/*
This function takes a local URI id, runs assorted queries and constructs a libprotobuff object corresponding to the given localURI.
@param inputLocalURIID: The ID of the localURI to retrieve
@param inputLocalURIBuffer: A buffer to put the resulting object in
@return: 1 if successful and 0 otherwise
*/
int getLocalURI(uint64_t inputLocalURIID, localURI &inputLocalURIBuffer);

sqlite3 *databaseConnection;
bool isValid;

private:
sqlite3_stmt *insertURIBaseStatement;
sqlite3_stmt *insertTagStatement;
sqlite3_stmt *insertIntegerKeyValuePairStatement;
sqlite3_stmt *insertDoubleKeyValuePairStatement;

sqlite3_stmt *removeURIStatement;
sqlite3_stmt *removeURIWithResourceNameStatement;

sqlite3_stmt *getURIBaseStatement;
sqlite3_stmt *getTagsStatement;
sqlite3_stmt *getIntegerKeyValuePairsStatement;
sqlite3_stmt *getDoubleKeyValuePairsStatement;

/*
This function just compacts the code by taking care of the details to add a URI base to the database using the insertURIBaseStatement prepared statement
@param inputURIName: The name to add
@param inputIPCPath: The path to the IPC connection this URI represents
@return: 1 if successful and 0 otherwise
*/
int insertURIBase(const std::string &inputURIName, const std::string &inputIPCPath);

/*
This function just compacts the code by taking care of the details to add a URI base to the database using the insertURIBaseStatement prepared statement
@param inputURIName: The name to add
@param inputIPCPath: The path to the IPC connection this URI represents
@param inputOriginatingProcessID: The ID of the process that originated this URI
@return: 1 if successful and 0 otherwise
*/
int insertURIBase(const std::string &inputURIName, const std::string &inputIPCPath, int64_t inputOriginatingProcessID);

/*
This function just compacts the code by taking care of the details to add a URI tag to the database using the insertTagStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputURITag: The tag to add
@return: 1 if successful and 0 otherwise
*/
int insertURITag(int64_t inputURIID, const std::string &inputURITag);

/*
This function just compacts the code by taking care of the details to add a URI integer key pair to the database using the insertIntegerKeyValuePairStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputKey: The key for the pair
@param inputValue: The value for the pair
@return: 1 if successful and 0 otherwise
*/
int insertURIIntegerKeyValuePair(int64_t inputURIID, const std::string &inputKey, int inputValue);

/*
This function just compacts the code by taking care of the details to add a URI integer key pair to the database using the insertDoubleKeyValuePairStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputKey: The key for the pair
@param inputValue: The value for the pair
@return: 1 if successful and 0 otherwise
*/
int insertDoubleKeyValuePair(int64_t inputURIID, const std::string &inputKey, double inputValue);

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with tags.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no tag based parameters
*/
std::string generateTagSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber);

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with integerKeyValuePairs.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no integer key/value based parameters
*/
std::string generateIntegerKeyValuePairsSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber);

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with doubleKeyValuePairs.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no double key/value based parameters
*/
std::string generateDoubleKeyValuePairsSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber);

/*
This function generates the query string required to find the URIs matching a particular query object.
@param inputTagSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the tag requirements
@param inputIntegerKeyValueSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the integer key/value requirements
@param inputDoubleKeyValueSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the double key/value requirements
@return: The query string.  The string is empty on failure or a query with no tag based parameters
*/
std::string generateQueryStringFromSubqueries(const std::string &inputTagSubqueryString, const std::string &inputIntegerKeyValueSubquerystring, const std::string &inputDoubleKeyValueSubquerystring);

/*
This function prints to the error stream the last database error that was encountered by the connection (useful for debugging).
*/
void reportLastDatabaseError();



};

/*
This function takes a comparison operator enum and converts it to a string
@param inputComparisonOperator: The operator to convert
@return: the string that represents the operator in sql or empty on failure
*/
std::string comparisonOperatorToString(const comparisonOperator inputComparisonOperator);

/*
This is a simple convience function to print all of the URIs in a vector in a human readable format
@param inputURIList: The list of URIs to print
@return: The human readable string
*/
std::string printURIList(const std::vector<localURI> &inputURIList);

/*
This is a simple convience function to print a URI in a human readable format
@param inputURI: The URI to print
@return: The human readable string
*/
std::string printURI(const localURI &inputURI);


#endif
