#include "localURIDatabase.h"



/*
This function initializes the object and (by default) creates a in memory (ram) SQL3Lite database. However, if input is given, the function will try to connect to and check the given database.  If inputCreateExternalDatabase is also set to true, the function will make a external database file (useful for logging and debugging) rather than an in memory database.
@param inputDatabase: The (optional) string of the path to find/create the database for this object to use
@param inputCreateExternalDatabase: If true, the object will try to create the database file if it isn't found.  Ignored if the inputDatabase string is empty.
*/
localURIDatabase::localURIDatabase(const std::string &inputDatabase, bool inputCreateExternalDatabase)
{
bool needToMakeTables = false;
databaseConnection = NULL;

if(inputDatabase.size() == 0)
{
//Empty string, so we are making an in memory (ram) database
if(sqlite3_open_v2(":memory:", &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
{
fprintf(stderr, "Error creating in memory database\n");
isValid = false;
return;
} 
needToMakeTables = true;

}
else
{
//We have a string, so we will try to connect/create to/a database there
if(inputCreateExternalDatabase)
{
//Connect to the database and create one if it isn't already there

//See if we can connect to it without making it 
needToMakeTables = true;
if(sqlite3_open_v2(inputDatabase.c_str(), &databaseConnection, SQLITE_OPEN_READWRITE,NULL) == SQLITE_OK)
{
needToMakeTables = false;
}
//We couldn't connect to it without making it, so now we will try making it
else if(sqlite3_open_v2(inputDatabase.c_str(), &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
{
fprintf(stderr, "Error loading/creating file based database at %s\n", inputDatabase.c_str());
isValid = false;
return;
}


}
else
{
//Connect to the database and set isValid false if it isn't there
if(sqlite3_open_v2(inputDatabase.c_str(), &databaseConnection, SQLITE_OPEN_READWRITE,NULL) != SQLITE_OK)
{
fprintf(stderr, "Error loading database at %s\n", inputDatabase.c_str());
isValid = false;
return;
} 

}
}


//Make sure we set foreign key relationships on for this connection
if(sqlite3_exec(databaseConnection, "PRAGMA foreign_keys = on;", NULL, NULL, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error creating tables for database\n");
reportLastDatabaseError();
isValid = false;
return;
}

//We have a database connection, so we will make the tables if we created the database
if(needToMakeTables)
{
//Make the tables with foreign key constraints and cascading deletes so that database integrate is more secure even if there are mistakes in application code.  In addition, make indexes on the foreign keys to speed processing
if(sqlite3_exec(databaseConnection, "BEGIN TRANSACTION; CREATE TABLE URIs (id integer primary key, resourceName text not null, IPCPath text unique not null, originatingProcessID integer, unique(id, resourceName)); CREATE TABLE URItags (value text, uriID integer, FOREIGN KEY(uriID) REFERENCES URIs(id) ON DELETE CASCADE, unique(value, uriID)); CREATE INDEX tagsIndex ON URItags(uriID); CREATE TABLE URIIntegerKeyValuePairs (key text, value integer, uriID integer, FOREIGN KEY(uriID) REFERENCES URIs(id) ON DELETE CASCADE, unique(key, uriID)); CREATE INDEX integerKeyValuePairsIndex ON URIIntegerKeyValuePairs(uriID); CREATE TABLE URIDoubleKeyValuePairs (key text, value real, uriID integer, FOREIGN KEY(uriID) REFERENCES URIs(id) ON DELETE CASCADE, unique(key, uriID)); CREATE INDEX doubleKeyValuePairsIndex ON URIDoubleKeyValuePairs(uriID); END TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error creating tables for database\n");
reportLastDatabaseError();
isValid = false;
return;
}
}




//We have our tables, so now we are going to prepare some queries for later operations
std::string insertURIBaseSQLCommand = "insert into URIs (id, resourceName, IPCPath, originatingProcessID) values(null, ?, ?, ?);";
if(sqlite3_prepare_v2(databaseConnection, insertURIBaseSQLCommand.c_str(), insertURIBaseSQLCommand.size(), &insertURIBaseStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement0: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string insertTagSQLCommand = "insert into URItags (value, uriID) values(?, ?);";
if(sqlite3_prepare_v2(databaseConnection, insertTagSQLCommand.c_str(), insertTagSQLCommand.size(), &insertTagStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement1: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string insertIntegerKeyPairSQLCommand = "insert into URIIntegerKeyValuePairs (key, value, uriID) values(?, ?, ?);";
if(sqlite3_prepare_v2(databaseConnection, insertIntegerKeyPairSQLCommand.c_str(), insertIntegerKeyPairSQLCommand.size(), &insertIntegerKeyValuePairStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement2: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string insertDoubleKeyPairSQLCommand = "insert into URIDoubleKeyValuePairs (key, value, uriID) values(?, ?, ?);";
if(sqlite3_prepare_v2(databaseConnection, insertDoubleKeyPairSQLCommand.c_str(), insertDoubleKeyPairSQLCommand.size(), &insertDoubleKeyValuePairStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement3: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string removeURICommand = "delete from URIs where id = ?100;";
if(sqlite3_prepare_v2(databaseConnection, removeURICommand.c_str(), removeURICommand.size(), &removeURIStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement4: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

//Make the retrieval statements
std::string getURIBaseCommand = "select * from URIs where id = ?100;";
if(sqlite3_prepare_v2(databaseConnection, getURIBaseCommand.c_str(), getURIBaseCommand.size(), &getURIBaseStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement5: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}


std::string getTagsCommand = "select * from URItags where uriID = ?100;";
if(sqlite3_prepare_v2(databaseConnection, getTagsCommand.c_str(), getTagsCommand.size(), &getTagsStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement6: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string getIntegerKeyValuePairsCommand = "select * from URIIntegerKeyValuePairs where uriID = ?100;";
if(sqlite3_prepare_v2(databaseConnection, getIntegerKeyValuePairsCommand.c_str(), getIntegerKeyValuePairsCommand.size(), &getIntegerKeyValuePairsStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement7: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

std::string getDoubleKeyValuePairsCommand = "select * from URIDoubleKeyValuePairs where uriID = ?100;";
if(sqlite3_prepare_v2(databaseConnection, getDoubleKeyValuePairsCommand.c_str(), getDoubleKeyValuePairsCommand.size(), &getDoubleKeyValuePairsStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement8: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
isValid = false;
return;
}

isValid = true;
}

/*
This function adds a local URI to the database.  It takes a libprotobuf message, converts it into something that can be added into the database and then performs an insertion operation.  All of the pieces of the URI are inserted in the same transaction, so either the entire URI is stored or none of it is (and an error is returned).
@param inputLocalURI: The local URI to be added (likely from a ZMQ message)
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::addURI(const localURI &inputLocalURI)
{

//Begin transaction
if(sqlite3_exec(databaseConnection, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to send begin transaction command\n");
return 0;
}


//Using a do while wrapper just so that I can use break statements and ensure any exit from the sequence still closes the transaction
int returnValue = 0;
do
{
//Add the URI base 
if(inputLocalURI.has_originatingprocessid())
{
//If the originating process ID has been provided
if(insertURIBase(inputLocalURI.resourcename(), inputLocalURI.ipcpath(), inputLocalURI.originatingprocessid())!=1)
{
break; //Close the transaction and return failure
}
}
else
{
//If the originating process ID has not been provided
if(insertURIBase(inputLocalURI.resourcename(), inputLocalURI.ipcpath())!=1)
{
break;  //Close the transaction and return failure
}
}

//Get the primary key ID associated with it
int64_t URIID = sqlite3_last_insert_rowid(databaseConnection);

//Add up to the MAX number of tags
for(int i=0; i<inputLocalURI.tags_size() && i < MAX_NUMBER_OF_TAGS_PER_URI; i++)
{
if(insertURITag(URIID, inputLocalURI.tags(i)) != 1)
{
break;  //Close the transaction and return failure
}
}

//Add up to the MAX number of integer key value pairs
for(int i=0; i<inputLocalURI.integerpairs_size() && i < MAX_NUMBER_OF_INTEGER_KEY_VALUE_PAIRS; i++)
{
if(insertURIIntegerKeyValuePair(URIID, inputLocalURI.integerpairs(i).key(), inputLocalURI.integerpairs(i).value()) != 1)
{
break;  //Close the transaction and return failure
}
}


//Add up to the MAX number of double key value pairs
for(int i=0; i<inputLocalURI.doublepairs_size() && i < MAX_NUMBER_OF_DOUBLE_KEY_VALUE_PAIRS; i++)
{
if(insertDoubleKeyValuePair(URIID, inputLocalURI.doublepairs(i).key(), inputLocalURI.doublepairs(i).value()) != 1)
{
break;  //Close the transaction and return failure
}
}

//We finished all of the pieces of the transaction successfully, so mark the return value to success
returnValue = 1;
} while(false);

//Finish the transaction
if(sqlite3_exec(databaseConnection, "END TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to send end transaction command\n");
return 0;
}

return returnValue;
}

/*
This function removes the URI with the given ID from the database (ID can be retrieved with queries).
@param inputLocalURIID: The database given ID of the URI
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::removeURI(uint64_t inputLocalURIID)
{
//This function/statement removes the URI and its associated tags by taking advantage of foreign key constraints and cascade deletes.  It won't work if they haven't been enabled by a pragma command (but that should have happened in the constructor) 


if(sqlite3_bind_int64(removeURIStatement,100, (int64_t) inputLocalURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement to remove URI \n");
reportLastDatabaseError();
//return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(removeURIStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step delete URI\n");
reportLastDatabaseError();
return 0;
}


//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(removeURIStatement) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to reset statement\n");
reportLastDatabaseError();
return 0;
}

return 1;
}


/*
This function takes a libprotobuf query object and runs the query in the database.  The result is returned as a set of libprotobuf objects.  The queries are restricted to the same size limitations as localURIs as far as the number of tags and key value pairs.
@param inputLocalURIQuery: This is the query to process
@param inputResultBuffer: The buffer to place set of localURIs that match the given criteria in (restricted to a maximum of query.maximumNumberOfResults)
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::processQuery(const localURIQuery &inputLocalURIQuery, std::vector<localURI> &inputResultBuffer)
{
//Generate and compile a query to that is made from the query object

//The search works by taking the intersection of uriIDs that match criteria in each table.  We start by making subqueries that find in each table uriIDs that match the requirements for that table.  We then take the intersection of those values to find only the uriIDs that match all criteria.  

//We then make a buffer object for each of those uriIDs and select all tags and key values that match that URI key and add them to the associated buffers

//select uriID from (select uriID, count(uriID) as matchCount from URItags where value in ('location-Door', 'trip-wire') group by uriID) where matchCount = 2;

//Make subqueries
unsigned int queryFieldNumbers = 1;

//Make tag based subquery string
std::string tagBasedSubquery = generateTagSubqueryString(inputLocalURIQuery, queryFieldNumbers);

//Make integer key/value based subquery string
std::string integerKeyValueBasedSubquery = generateIntegerKeyValuePairsSubqueryString(inputLocalURIQuery, queryFieldNumbers);

//Make double key/value based subquery string
std::string doubleKeyValueBasedSubquery = generateDoubleKeyValuePairsSubqueryString(inputLocalURIQuery, queryFieldNumbers);

//Make full query string
std::string queryString = generateQueryStringFromSubqueries(tagBasedSubquery, integerKeyValueBasedSubquery, doubleKeyValueBasedSubquery);

//Print it so we know what is going on
//printf("%s\n", queryString.c_str());

//Create parametric query
sqlite3_stmt *queryStatement;

if(sqlite3_prepare_v2(databaseConnection, queryString.c_str(), queryString.size(), &queryStatement, NULL) != SQLITE_OK)
{
fprintf(stderr, "Error preparing parametric sql statement for query\n"); reportLastDatabaseError();
return 0;
}

//Bind values for the query (in the same order that the string generation functions used)
int currentIndex=1;

//Bind required tags
for(int i=0; i<inputLocalURIQuery.requiredtags_size(); i++)
{
if(sqlite3_bind_text(queryStatement, currentIndex, inputLocalURIQuery.requiredtags(i).c_str(), inputLocalURIQuery.requiredtags(i).size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement required tag (%s) for URI query\n", inputLocalURIQuery.requiredtags(i).c_str());
reportLastDatabaseError();
return 0;
}

currentIndex++;
} 

//Bind forbidden tags
for(int i=0; i<inputLocalURIQuery.forbiddentags_size(); i++)
{
if(sqlite3_bind_text(queryStatement, currentIndex, inputLocalURIQuery.forbiddentags(i).c_str(), inputLocalURIQuery.forbiddentags(i).size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement forbidden tag (%s) for URI query\n", inputLocalURIQuery.forbiddentags(i).c_str());
reportLastDatabaseError();
return 0;
}

currentIndex++;
} 

//Bind integer key value pairs
for(int i=0; i<inputLocalURIQuery.integerconditions_size(); i++)
{
//Bind key
if(sqlite3_bind_text(queryStatement, currentIndex, inputLocalURIQuery.integerconditions(i).field().c_str(), inputLocalURIQuery.integerconditions(i).field().size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement integer key (%s) for URI query\n", inputLocalURIQuery.integerconditions(i).field().c_str());
reportLastDatabaseError();
return 0;
}
currentIndex++;


//Bind value
if(sqlite3_bind_int(queryStatement,currentIndex, inputLocalURIQuery.integerconditions(i).value()) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement integer value\n");
reportLastDatabaseError();
return 0;
}
currentIndex++;

}

//Bind double key value pairs
for(int i=0; i<inputLocalURIQuery.doubleconditions_size(); i++)
{
//Bind key
if(sqlite3_bind_text(queryStatement, currentIndex, inputLocalURIQuery.doubleconditions(i).field().c_str(), inputLocalURIQuery.doubleconditions(i).field().size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement double key (%s) for URI query\n", inputLocalURIQuery.doubleconditions(i).field().c_str());
reportLastDatabaseError();
return 0;
}
currentIndex++;


//Bind value
if(sqlite3_bind_int(queryStatement,currentIndex, inputLocalURIQuery.doubleconditions(i).value()) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement double value \n");
reportLastDatabaseError();
return 0;
}
currentIndex++;

}

if(sqlite3_column_count(queryStatement) != 1)
{
//Wrong number of columns, something is wrong
sqlite3_finalize(queryStatement);
fprintf(stderr, "Error: ID retrieval for a query returned more than one column\n");
return 0;
}

//Run the query, processing each row returned until we hit SQLITE_DONE
int stepReturnValue;
while(true)
{

//Process the statement to get the next row
stepReturnValue = sqlite3_step(queryStatement);

if(stepReturnValue == SQLITE_ROW)
{

//Double check the result type
if(sqlite3_column_type(queryStatement, 0) != SQLITE_INTEGER)
{
fprintf(stderr, "Error, URI query id result has non-integer type\n");
sqlite3_finalize(queryStatement);
return 0;
}

//Process ROW to get ID
uint64_t idResult = sqlite3_column_int(queryStatement, 0);

//Run function (with its own query) to get the associated URI structure and store it to be sent 
localURI localURIBuffer;


if(getLocalURI(idResult, localURIBuffer) != 1)
{
fprintf(stderr, "Error, URI query was not able to retrieve one of the returned localURIs\n");
sqlite3_finalize(queryStatement);
return 0;
}

//Add the result to the result vector
inputResultBuffer.push_back(localURIBuffer);
}
else if(stepReturnValue == SQLITE_DONE)
{
sqlite3_finalize(queryStatement);
break;  //Finished query successfully
}
else
{
fprintf(stderr, "Error, unable to step URI query\n");
reportLastDatabaseError();
sqlite3_finalize(queryStatement);
return 0;
}
}

return 1;//Made it to end, so everything was successful
}

/*
This function takes a local URI id, runs assorted queries and constructs a libprotobuff object corresponding to the given localURI.
@param inputLocalURIID: The ID of the localURI to retrieve
@param inputLocalURIBuffer: A buffer to put the resulting object in
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::getLocalURI(uint64_t inputLocalURIID, localURI &inputLocalURIBuffer)
{
//Clear buffer
inputLocalURIBuffer.clear_tags();
inputLocalURIBuffer.clear_integerpairs();
inputLocalURIBuffer.clear_doublepairs();

int stepReturnValue;

//Get URIbase
if(sqlite3_bind_int64(getURIBaseStatement,100, inputLocalURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement to retrieve URI base\n");
reportLastDatabaseError();
return 0;
}

stepReturnValue = sqlite3_step(getURIBaseStatement);

//Check the number of columns
if(sqlite3_column_count(getURIBaseStatement) != 4)
{
fprintf(stderr, "Error, URI base result has wrong number of columns\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
}

if(stepReturnValue == SQLITE_ROW)
{
//Get the values and set the appropriate fields
const unsigned char *stringPointer;

//Get resource name
if(sqlite3_column_type(getURIBaseStatement, 1) != SQLITE_TEXT)
{
fprintf(stderr, "Error, URI base resourceName column isn't of type text\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
}

stringPointer = sqlite3_column_text(getURIBaseStatement, 1);
if(stringPointer == NULL)
{
inputLocalURIBuffer.set_resourcename("");
}
else
{
inputLocalURIBuffer.set_resourcename((const char*) stringPointer);
}

//Get IPC path
if(sqlite3_column_type(getURIBaseStatement, 2) != SQLITE_TEXT)
{
fprintf(stderr, "Error, URI base IPCPath column isn't of type text\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
}

stringPointer = sqlite3_column_text(getURIBaseStatement, 2);
if(stringPointer == NULL)
{
inputLocalURIBuffer.set_ipcpath("");
}
else
{
inputLocalURIBuffer.set_ipcpath((const char *) stringPointer);
}

//Get originating process ID
if(sqlite3_column_type(getURIBaseStatement, 3) != SQLITE_INTEGER)
{
fprintf(stderr, "Error, URI base IPCPath column isn't of type integer\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
}

inputLocalURIBuffer.set_originatingprocessid(sqlite3_column_int64(getURIBaseStatement, 3));
sqlite3_reset(getURIBaseStatement);
}
else
{
fprintf(stderr, "Error, unable to step get URI base\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
return 0;
}

//Get any and all tags
if(sqlite3_bind_int64(getTagsStatement,100, inputLocalURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement to retrieve URI tags\n");
reportLastDatabaseError();
return 0;
}

while(true)
{
stepReturnValue = sqlite3_step(getTagsStatement);

//Check the number of columns
if(sqlite3_column_count(getTagsStatement) != 2)
{
fprintf(stderr, "Error, URI tags result has wrong number of columns\n");
sqlite3_reset(getTagsStatement);
reportLastDatabaseError();
return 0;
}

if(stepReturnValue == SQLITE_ROW)
{
//Get the values and set the appropriate fields
const unsigned char *stringPointer;

//Get the tag value
if(sqlite3_column_type(getTagsStatement, 0) != SQLITE_TEXT)
{
fprintf(stderr, "Error, URI tags value column isn't of type text\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
return 0;
}

stringPointer = sqlite3_column_text(getTagsStatement, 0);
if(stringPointer == NULL)
{
}
else
{
inputLocalURIBuffer.add_tags((const char *) stringPointer);
}

}
else if(stepReturnValue == SQLITE_DONE)
{
sqlite3_reset(getTagsStatement);
break; //Got all the integer pairs, so exit loop
}
else
{
fprintf(stderr, "Error, unable to step get URI tags\n");
sqlite3_reset(getURIBaseStatement);
reportLastDatabaseError();
return 0;
}
}

//Get any and all integers
integerKeyValuePair *integerKeyBuffer;
if(sqlite3_bind_int64(getIntegerKeyValuePairsStatement,100, inputLocalURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement to retrieve URI tags\n");
reportLastDatabaseError();
return 0;
}

while(true)
{
stepReturnValue = sqlite3_step(getIntegerKeyValuePairsStatement);

//Check the number of columns
if(sqlite3_column_count(getIntegerKeyValuePairsStatement) != 3)
{
fprintf(stderr, "Error, URI integer key values result has wrong number of columns\n");
sqlite3_reset(getIntegerKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

if(stepReturnValue == SQLITE_ROW)
{
//Get the values and set the appropriate fields
const unsigned char *stringPointer;

integerKeyBuffer = inputLocalURIBuffer.add_integerpairs();

//Get the integer key value pair key
if(sqlite3_column_type(getIntegerKeyValuePairsStatement, 0) != SQLITE_TEXT)
{
fprintf(stderr, "Error, URI integer key value pair key column isn't of type text\n");
sqlite3_reset(getIntegerKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

stringPointer = sqlite3_column_text(getIntegerKeyValuePairsStatement, 0);
if(stringPointer == NULL)
{
}
else
{
integerKeyBuffer->set_key((const char*) stringPointer);
}

//Get the integer key value pair value
if(sqlite3_column_type(getIntegerKeyValuePairsStatement, 1) != SQLITE_INTEGER)
{
fprintf(stderr, "Error, URI integer key value pair value column isn't of type integer\n");
sqlite3_reset(getIntegerKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

integerKeyBuffer->set_value(sqlite3_column_int(getIntegerKeyValuePairsStatement, 1));

}
else if(stepReturnValue == SQLITE_DONE)
{
sqlite3_reset(getIntegerKeyValuePairsStatement);
break; //Got all the integer pairs, so exit loop
}
else
{
fprintf(stderr, "Error, unable to step get URI integer key value pairs\n");
sqlite3_reset(getIntegerKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}
}

doubleKeyValuePair *doubleKeyBuffer;
if(sqlite3_bind_int64(getDoubleKeyValuePairsStatement,100, inputLocalURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement to retrieve URI tags\n");
reportLastDatabaseError();
return 0;
}

while(true)
{
stepReturnValue = sqlite3_step(getDoubleKeyValuePairsStatement);

//Check the number of columns
if(sqlite3_column_count(getDoubleKeyValuePairsStatement) != 3)
{
fprintf(stderr, "Error, URI double key values result has wrong number of columns\n");
sqlite3_reset(getDoubleKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

if(stepReturnValue == SQLITE_ROW)
{
//Get the values and set the appropriate fields
const unsigned char *stringPointer;

doubleKeyBuffer = inputLocalURIBuffer.add_doublepairs();

//Get the double key value pair key
if(sqlite3_column_type(getDoubleKeyValuePairsStatement, 0) != SQLITE_TEXT)
{
fprintf(stderr, "Error, URI double key value pair key column isn't of type text\n");
sqlite3_reset(getDoubleKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

stringPointer = sqlite3_column_text(getDoubleKeyValuePairsStatement, 0);
if(stringPointer == NULL)
{
}
else
{
doubleKeyBuffer->set_key((const char*) stringPointer);
}

//Get the integer key value pair value
if(sqlite3_column_type(getDoubleKeyValuePairsStatement, 1) != SQLITE_FLOAT)
{
fprintf(stderr, "Error, URI double key value pair value column isn't of type integer\n");
sqlite3_reset(getDoubleKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}

doubleKeyBuffer->set_value(sqlite3_column_double(getDoubleKeyValuePairsStatement, 1));
}
else if(stepReturnValue == SQLITE_DONE)
{
sqlite3_reset(getDoubleKeyValuePairsStatement);
break; //Got all the double pairs, so exit loop
}
else
{
fprintf(stderr, "Error, unable to step get URI double key value pairs\n");
sqlite3_reset(getDoubleKeyValuePairsStatement);
reportLastDatabaseError();
return 0;
}
}

return 1;
}

/*
This function just compacts the code by taking care of the details to add a URI base to the database using the insertURIBaseStatement prepared statement
@param inputURIName: The name to add
@param inputIPCPath: The path to the IPC connection this URI represents
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::insertURIBase(const std::string &inputURIName, const std::string &inputIPCPath)
{
//(key, resourceName, IPCPath, originatingProcessID)
if(inputURIName.size() <= MAX_NUMBER_OF_CHARACTERS_IN_RESOURCE_NAME && inputIPCPath.size() <= MAX_NUMBER_OF_CHARACTERS_IN_IPC_PATH)
{
if(sqlite3_bind_text(insertURIBaseStatement, 1, inputURIName.c_str(), inputURIName.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI resource name %s\n", inputURIName.c_str());
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_text(insertURIBaseStatement, 2, inputIPCPath.c_str(), inputIPCPath.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI IPCPath %s\n", inputIPCPath.c_str());
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_null(insertURIBaseStatement, 3) != SQLITE_OK)
{
fprintf(stderr, "Error binding the process ID to NULL\n");
reportLastDatabaseError();
return 0;
}

}
else
{
fprintf(stderr, "Error, the resource name or IPCPath is too long\n");
reportLastDatabaseError();
return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(insertURIBaseStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step insert URI base\n");
reportLastDatabaseError();
return 0;
}

//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(insertURIBaseStatement) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to reset statement\n");
reportLastDatabaseError();
return 0;
}

return 1;
}

/*
This function just compacts the code by taking care of the details to add a URI base to the database using the insertURIBaseStatement prepared statement
@param inputURIName: The name to add
@param inputIPCPath: The path to the IPC connection this URI represents
@param inputOriginatingProcessID: The ID of the process that originated this URI
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::insertURIBase(const std::string &inputURIName, const std::string &inputIPCPath, int64_t inputOriginatingProcessID)
{
//(key, resourceName, IPCPath, originatingProcessID)
if(inputURIName.size() <= MAX_NUMBER_OF_CHARACTERS_IN_RESOURCE_NAME && inputIPCPath.size() <= MAX_NUMBER_OF_CHARACTERS_IN_IPC_PATH)
{
if(sqlite3_bind_text(insertURIBaseStatement, 1, inputURIName.c_str(), inputURIName.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI resource name %s\n", inputURIName.c_str());
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_text(insertURIBaseStatement, 2, inputIPCPath.c_str(), inputIPCPath.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI IPCPath %s\n", inputIPCPath.c_str());
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_int64(insertURIBaseStatement,3, inputOriginatingProcessID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI base \n");
reportLastDatabaseError();
return 0;
}

}
else
{
fprintf(stderr, "Error, the resource name or IPCPath is too long\n");
reportLastDatabaseError();
return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(insertURIBaseStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step insert URI base\n");
reportLastDatabaseError();
return 0;
}

//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(insertURIBaseStatement) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to reset statement\n");
reportLastDatabaseError();
return 0;
}

return 1;
}

/*
This function just compacts the code by taking care of the details to add a URI tag to the database using the insertTagStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputURITag: The tag to add
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::insertURITag(int64_t inputURIID, const std::string &inputURITag)
{
if(inputURITag.size() <= MAX_NUMBER_OF_CHARACTERS_IN_TAG)
{

if(sqlite3_bind_text(insertTagStatement, 1, inputURITag.c_str(), inputURITag.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI tag %s\n", inputURITag.c_str());
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_int64(insertTagStatement,2, inputURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI tag %s\n", inputURITag.c_str());
reportLastDatabaseError();
return 0;
}


}
else
{
fprintf(stderr, "Error, the tag is too long\n");
return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(insertTagStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step insert URI tag\n");
reportLastDatabaseError();
return 0;
}

//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(insertTagStatement) != SQLITE_OK)
{
reportLastDatabaseError();
fprintf(stderr, "Error, unable to reset statement\n");
return 0;
}

return 1;
}

/*
This function just compacts the code by taking care of the details to add a URI integer key pair to the database using the insertIntegerKeyValuePairStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputKey: The key for the pair
@param inputValue: The value for the pair
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::insertURIIntegerKeyValuePair(int64_t inputURIID, const std::string &inputKey, int inputValue)
{
if(inputKey.size() <= MAX_NUMBER_OF_CHARACTERS_IN_INTEGER_KEY)
{

if(sqlite3_bind_text(insertIntegerKeyValuePairStatement, 1, inputKey.c_str(), inputKey.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI integer key/value pair0 %s\n", inputKey.c_str());
reportLastDatabaseError();
return 0;
}


if(sqlite3_bind_int(insertIntegerKeyValuePairStatement,2, inputValue) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI integer key/value pair1\n");
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_int64(insertIntegerKeyValuePairStatement,3, inputURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI integer key/value pair2\n");
reportLastDatabaseError();
return 0;
}

}
else
{
fprintf(stderr, "Error, the integer key is too long\n");
return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(insertIntegerKeyValuePairStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step insert URI integer key/value pair\n");
reportLastDatabaseError();
return 0;
}

//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(insertIntegerKeyValuePairStatement) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to reset statement\n");
return 0;
}

return 1;
}

/*
This function just compacts the code by taking care of the details to add a URI integer key pair to the database using the insertDoubleKeyValuePairStatement prepared statement
@param inputURIID: The ID identifying the URI
@param inputKey: The key for the pair
@param inputValue: The value for the pair
@return: 1 if successful and 0 otherwise
*/
int localURIDatabase::insertDoubleKeyValuePair(int64_t inputURIID, const std::string &inputKey, double inputValue)
{
if(inputKey.size() <= MAX_NUMBER_OF_CHARACTERS_IN_DOUBLE_KEY)
{

if(sqlite3_bind_text(insertDoubleKeyValuePairStatement, 1, inputKey.c_str(), inputKey.size(), SQLITE_TRANSIENT) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI double key/value pair0 %s\n", inputKey.c_str());
reportLastDatabaseError();
return 0;
}


if(sqlite3_bind_double(insertDoubleKeyValuePairStatement,2, inputValue) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI double key/value pair1\n");
reportLastDatabaseError();
return 0;
}

if(sqlite3_bind_int64(insertDoubleKeyValuePairStatement,3, inputURIID) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to bind prepared statement for URI double key/value pair2\n");
reportLastDatabaseError();
return 0;
}

}
else
{
fprintf(stderr, "Error, the double key is too long\n");
return 0;
}

//We successfully bound the value to the query, so now we execute it
if(sqlite3_step(insertDoubleKeyValuePairStatement) != SQLITE_DONE)
{
fprintf(stderr, "Error, unable to step insert URI double key/value pair\n");
reportLastDatabaseError();
return 0;
}

//The step completed successfully, so now we reset the prepared statement for later use
if(sqlite3_reset(insertDoubleKeyValuePairStatement) != SQLITE_OK)
{
fprintf(stderr, "Error, unable to reset statement\n");
reportLastDatabaseError();
return 0;
}

return 1;
}

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with tags.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no tag based parameters
*/
std::string localURIDatabase::generateTagSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber)
{
//Make tag subquery
//select uriID from (select uriID, count(uriID) as matchCount from URItags where URItags.value in ('location-Door', 'trip-wire') group by uriID) where matchCount = 1 except select uriID from (select uriID from URItags where URItags.value in ('This thing we dont want', 'Undesired thing') group by uriID);


std::string tagSubqueryCommand = "select uriID from (select uriID, count(uriID) as matchCount from URItags where ";

if(inputLocalURIQuery.requiredtags_size() > 0 || inputLocalURIQuery.forbiddentags_size() > 0)
{

if(inputLocalURIQuery.requiredtags_size() > 0)
{
tagSubqueryCommand += "URItags.value in (";
for(int i=0; i<inputLocalURIQuery.requiredtags_size(); i++)
{
tagSubqueryCommand +="?" + std::to_string(inputStartingQueryFieldNumber);
inputStartingQueryFieldNumber++;
if(i+1 < inputLocalURIQuery.requiredtags_size())
{
tagSubqueryCommand += ", ";
} 
}

tagSubqueryCommand+= ")";
}

tagSubqueryCommand+= " group by uriID) where matchCount = ";

//Add the number of entries we are expecting
tagSubqueryCommand+=std::to_string(inputLocalURIQuery.requiredtags_size());

//select uriID from (select uriID, count(uriID) as matchCount from URItags where URItags.value in ('location-Door', 'trip-wire') group by uriID) where matchCount = 1 except (select uriID from (select uriID from URItags where URItags.value in ('This thing we dont want', 'Undesired thing') group by uriID);


if(inputLocalURIQuery.requiredtags_size() > 0 && inputLocalURIQuery.forbiddentags_size() > 0)
{
tagSubqueryCommand +=" except select uriID from (select uriID from URItags where URItags.value in ( ";
}

if(inputLocalURIQuery.forbiddentags_size() > 0)
{
for(int i=0; i<inputLocalURIQuery.forbiddentags_size(); i++)
{
tagSubqueryCommand +="?" + std::to_string(inputStartingQueryFieldNumber);
inputStartingQueryFieldNumber++;
if(i+1 < inputLocalURIQuery.forbiddentags_size())
{
tagSubqueryCommand += ", ";
} 
}
tagSubqueryCommand+=") group by uriID)";
}


return tagSubqueryCommand;
}

return ""; //No tags are forbidden or required, so skip this subquery
}

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with integerKeyValuePairs.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no tag based parameters
*/
std::string localURIDatabase::generateIntegerKeyValuePairsSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber)
{
//Make integer key value pairs subquery
//select uriID from (select uriID, count(uriID) as matchCount from URIIntegerKeyValuePairs where (URIIntegerKeyValuePairs.key = "count" AND URIIntegerKeyValuePairs.value > 52) OR (URIIntegerKeyValuePairs.key = "otherCount" AND URIIntegerKeyValuePairs.value < 31) group by uriID) where matchCount = 2;
std::string integerKeyValuesSubqueryCommand;

if(inputLocalURIQuery.integerconditions_size() > 0)
{
integerKeyValuesSubqueryCommand = "select uriID from (select uriID, count(uriID) as matchCount from URIIntegerKeyValuePairs where (";

//Add each integer condition with the parameter placeholder
for(int i=0; i<inputLocalURIQuery.integerconditions_size(); i++)
{
if(i>0)
{
//Add an or
integerKeyValuesSubqueryCommand += " OR ";
}

integerKeyValuesSubqueryCommand += "(URIIntegerKeyValuePairs.key = ?" + std::to_string(inputStartingQueryFieldNumber);
inputStartingQueryFieldNumber++;

integerKeyValuesSubqueryCommand += " AND URIIntegerKeyValuePairs.value ";
//Add conditional operator
integerKeyValuesSubqueryCommand += comparisonOperatorToString(inputLocalURIQuery.integerconditions(i).comparison());

integerKeyValuesSubqueryCommand += " ?" + std::to_string(inputStartingQueryFieldNumber) + " )";
inputStartingQueryFieldNumber++;
}

integerKeyValuesSubqueryCommand += " ) group by uriID) where matchCount = " + std::to_string(inputLocalURIQuery.integerconditions_size());
return integerKeyValuesSubqueryCommand;
}

return "";
}

/*
This function generates part of the query string required to find the URIs matching a particular query object.  In particular, it generates the subquery string that is used to make a list of all of the URIs that match the requirements associated with doubleKeyValuePairs.
@param inputLocalURIQuery: The query that this string will assist with
@param inputStartingQueryFieldNumber: The number to start the parameter numbers in the query with.  This number is incremented (affected) by the function.
@return: The subquery string.  The string is empty on failure or a query with no tag based parameters
*/
std::string localURIDatabase::generateDoubleKeyValuePairsSubqueryString(const localURIQuery &inputLocalURIQuery, unsigned int &inputStartingQueryFieldNumber)
{
//Make double key value pairs subquery
//select uriID from (select uriID, count(uriID) as matchCount from URIDoubleKeyValuePairs where (URIDoubleKeyValuePairs.key = "count" AND URIDoubleKeyValuePairs.value > 52) OR (URIDoubleKeyValuePairs.key = "otherCount" AND URIDoubleKeyValuePairs.value < 31 )  group by uriID) where matchCount = 2;

std::string doubleKeyValuesSubqueryCommand;

if(inputLocalURIQuery.doubleconditions_size() > 0)
{
doubleKeyValuesSubqueryCommand = "select uriID from (select uriID, count(uriID) as matchCount from URIDoubleKeyValuePairs where (";

//Add each integer condition with the parameter placeholder
for(int i=0; i<inputLocalURIQuery.doubleconditions_size(); i++)
{
if(i>0)
{
//Add an or
doubleKeyValuesSubqueryCommand += " OR ";
}

doubleKeyValuesSubqueryCommand += "(URIDoubleKeyValuePairs.key = ?" + std::to_string(inputStartingQueryFieldNumber);
inputStartingQueryFieldNumber++;

doubleKeyValuesSubqueryCommand += " AND URIDoubleKeyValuePairs.value ";
//Add conditional operator
doubleKeyValuesSubqueryCommand += comparisonOperatorToString(inputLocalURIQuery.doubleconditions(i).comparison());

doubleKeyValuesSubqueryCommand += " ?" + std::to_string(inputStartingQueryFieldNumber) + " )";
inputStartingQueryFieldNumber++;
}
doubleKeyValuesSubqueryCommand += ")  group by uriID ) where matchCount = " + std::to_string(inputLocalURIQuery.doubleconditions_size());
return doubleKeyValuesSubqueryCommand;
}

return "";
}

/*
This function prints to the error stream the last database error that was encountered by the connection (useful for debugging).
*/
void localURIDatabase::reportLastDatabaseError()
{
fprintf(stderr, "Database error signal: %s\n", sqlite3_errstr(sqlite3_extended_errcode(databaseConnection)));
}

/*
This function generates the query string required to find the URIs matching a particular query object.
@param inputTagSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the tag requirements
@param inputIntegerKeyValueSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the integer key/value requirements
@param inputDoubleKeyValueSubqueryString: A string for a numbered parameter subquery that returns the set of URIs that match the double key/value requirements
@return: The query string.  The string is empty on failure or a query with no tag based parameters
*/
std::string localURIDatabase::generateQueryStringFromSubqueries(const std::string &inputTagSubqueryString, const std::string &inputIntegerKeyValueSubquerystring, const std::string &inputDoubleKeyValueSubquerystring)
{
//The search works by taking the intersection of uriIDs that match criteria in each table.  We start by making subqueries that find in each table uriIDs that match the requirements for that table.  We then take the intersection of those values to find only the uriIDs that match all criteria.  

//We then make a buffer object for each of those uriIDs and select all tags and key values that match that URI key and add them to the associated buffers

//select uriID from (select uriID, count(uriID) as matchCount from URItags where value in ('location-Door', 'trip-wire') group by uriID) where matchCount = 2;

//Take the intersection of all subqueries
std::string queryString;
if(inputTagSubqueryString.size() > 0)
{
queryString += inputTagSubqueryString;
}

if((inputTagSubqueryString.size() > 0) && (inputIntegerKeyValueSubquerystring.size() > 0 || inputDoubleKeyValueSubquerystring.size() > 0))
{
queryString += " intersect ";
}

if(inputIntegerKeyValueSubquerystring.size() > 0)
{
queryString += inputIntegerKeyValueSubquerystring;
}

if(inputIntegerKeyValueSubquerystring.size() > 0 && inputDoubleKeyValueSubquerystring.size() > 0)
{
queryString += " intersect ";
}

if(inputDoubleKeyValueSubquerystring.size() > 0)
{
queryString += inputDoubleKeyValueSubquerystring;
}

queryString +=  ";";

return queryString;
}

/*
This function takes a comparison operator enum and converts it to a string
@param inputComparisonOperator: The operator to convert
@return: the string that represents the operator in sql or empty on failure
*/
std::string comparisonOperatorToString(const comparisonOperator inputComparisonOperator)
{
//Add conditional operator
switch(inputComparisonOperator)
{
case LESS_THAN:
return "<";
break;

case LESS_THAN_OR_EQUAL:
return "<=";
break;

case GREATER_THAN:
return  ">";
break;

case GREATER_THAN_OR_EQUAL:
return ">=";
break;

case EQUAL:
return "=";
break;

case NOT_EQUAL:
return "<>";
break;

default:
return "";
}

return ""; //Should never get here
}

/*
This is a simple convience function to print all of the URIs in a vector in a human readable format
@param inputURIList: The list of URIs to print
@return: The human readable string
*/
std::string printURIList(const std::vector<localURI> &inputURIList)
{
std::string resultString;
for(int i=0; i<inputURIList.size(); i++)
{
resultString += "URI" + std::to_string(i) + ":\n";
resultString += printURI(inputURIList[i]);
}

return resultString;
}

/*
This is a simple convience function to print a URI in a human readable format
@param inputURI: The URI to print
@return: The human readable string
*/
std::string printURI(const localURI &inputURI)
{
std::string resultString;

resultString = "Base: " + inputURI.resourcename() + " " + inputURI.ipcpath() + " " + std::to_string(inputURI.originatingprocessid()) + "\n";
resultString += "Tags:\n";

for(int ii=0; ii<inputURI.tags_size(); ii++)
{
resultString += std::to_string(ii) +": " + inputURI.tags(ii) + "\n";
}

resultString += "integerKeyValuePairs:\n";
for(int ii=0; ii<inputURI.integerpairs_size(); ii++)
{
resultString += std::to_string(ii) + ": " + inputURI.integerpairs(ii).key() + " " + std::to_string(inputURI.integerpairs(ii).value()) + "\n";
}

resultString += "doubleKeyValuePairs:\n";
for(int ii=0; ii<inputURI.doublepairs_size(); ii++)
{
resultString += std::to_string(ii) + ": " + inputURI.doublepairs(ii).key() + " " + std::to_string(inputURI.doublepairs(ii).value()) + "\n";
}

return resultString;
}
