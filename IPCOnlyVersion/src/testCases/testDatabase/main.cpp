#include<stdio.h>
#include "localURIDatabase.h"
#include "utilityFunctions.h"
#include<vector>
#include<string>

int main(int argc, char **argv)
{

//Make database
localURIDatabase myURIDatabase("./testDatabase.db", true);
if(!myURIDatabase.isValid)
{
fprintf(stderr, "Error constructing database object\n");
return 1;
}


//Make a URI and stick it in the database

localURI URI0;
URI0.set_resourcename("URI0ResourceName");
URI0.set_ipcpath("URI0IPCPath");
URI0.set_originatingprocessid(0);

URI0.add_tags("tag0");
URI0.add_tags("tag1");
URI0.add_tags("tag2");
URI0.add_tags("tag3");
URI0.add_tags("tag4");

integerKeyValuePair *integerKeyBuffer;

integerKeyBuffer = URI0.add_integerpairs();
integerKeyBuffer->set_key("integerKey0");
integerKeyBuffer->set_value(0);

integerKeyBuffer = URI0.add_integerpairs();
integerKeyBuffer->set_key("integerKey1");
integerKeyBuffer->set_value(1);

integerKeyBuffer = URI0.add_integerpairs();
integerKeyBuffer->set_key("integerKey2");
integerKeyBuffer->set_value(2);

doubleKeyValuePair *doubleKeyBuffer;

doubleKeyBuffer = URI0.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey0");
doubleKeyBuffer->set_value(0);

doubleKeyBuffer = URI0.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey1");
doubleKeyBuffer->set_value(1);

doubleKeyBuffer = URI0.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey2");
doubleKeyBuffer->set_value(2);

//Add the URI to the database
if(myURIDatabase.addURI(URI0) != 1)
{
fprintf(stderr, "Error, could not add URI0 to database\n");
return 1;
}

//Test removal functionality
if(myURIDatabase.removeURI(1)!=1)
{
fprintf(stderr, "Error, could not remove URI\n");
return 1;
}

//Add it again
//Add the URI to the database
if(myURIDatabase.addURI(URI0) != 1)
{
fprintf(stderr, "Error, could not add URI0 to database again\n");
return 1;
}

//Add another URI
localURI URI1;
URI1.set_resourcename("URI1ResourceName");
URI1.set_ipcpath("URI1IPCPath");
URI1.set_originatingprocessid(0);

URI1.add_tags("tag2");
URI1.add_tags("tag3");
URI1.add_tags("tag4");
URI1.add_tags("tag5");
URI1.add_tags("tag6");

integerKeyBuffer = URI1.add_integerpairs();
integerKeyBuffer->set_key("integerKey2");
integerKeyBuffer->set_value(2);

integerKeyBuffer = URI1.add_integerpairs();
integerKeyBuffer->set_key("integerKey3");
integerKeyBuffer->set_value(3);

integerKeyBuffer = URI1.add_integerpairs();
integerKeyBuffer->set_key("integerKey4");
integerKeyBuffer->set_value(4);

doubleKeyBuffer = URI1.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey2");
doubleKeyBuffer->set_value(2);

doubleKeyBuffer = URI1.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey3");
doubleKeyBuffer->set_value(3);

doubleKeyBuffer = URI1.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey4");
doubleKeyBuffer->set_value(4);

//Add the second URI to the database
if(myURIDatabase.addURI(URI1) != 1)
{
fprintf(stderr, "Error, could not add URI1 to database\n");
return 1;
}

//Add a third URI
localURI URI2;
URI2.set_resourcename("URI2ResourceName");
URI2.set_ipcpath("URI2IPCPath");
URI2.set_originatingprocessid(0);

URI2.add_tags("tag4");
URI2.add_tags("tag5");
URI2.add_tags("tag6");
URI2.add_tags("tag7");
URI2.add_tags("tag8");

integerKeyBuffer = URI2.add_integerpairs();
integerKeyBuffer->set_key("integerKey4");
integerKeyBuffer->set_value(4);

integerKeyBuffer = URI2.add_integerpairs();
integerKeyBuffer->set_key("integerKey5");
integerKeyBuffer->set_value(5);

integerKeyBuffer = URI2.add_integerpairs();
integerKeyBuffer->set_key("integerKey6");
integerKeyBuffer->set_value(6);

doubleKeyBuffer = URI2.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey4");
doubleKeyBuffer->set_value(4);

doubleKeyBuffer = URI2.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey5");
doubleKeyBuffer->set_value(5);

doubleKeyBuffer = URI2.add_doublepairs();
doubleKeyBuffer->set_key("doubleKey6");
doubleKeyBuffer->set_value(6);

//Add the third URI to the database
if(myURIDatabase.addURI(URI2) != 1)
{
fprintf(stderr, "Error, could not add URI2 to database\n");
return 1;
}

printf("Three URIs added successfully\n");

//Make block so objects are destroyed after section completes
{
//Make query
localURIQuery myQuery;
myQuery.add_requiredtags("tag4");
myQuery.add_forbiddentags("tag6");

//Get query results
std::vector<localURI> myResults;

if(myURIDatabase.processQuery(myQuery, myResults) != 1)
{
fprintf(stderr, "Error, unable to process query\n");
return 1;
}

if(myResults.size() == 1)
{
printf("Test 1 passed: required and forbidden tags\n");
}
else
{
printf("Test 1 failed: required and forbidden tags\n");
return 1;
}
//printf("Query returned %ld results:\n", myResults.size());
//printf("%s", printURIList(myResults).c_str());
}

//Make block so objects are destroyed after section completes
{
//Make query
localURIQuery myQuery;
myQuery.add_requiredtags("tag4");

//Get query results
std::vector<localURI> myResults;

if(myURIDatabase.processQuery(myQuery, myResults) != 1)
{
fprintf(stderr, "Error, unable to process query\n");
return 1;
}

if(myResults.size() == 3)
{
printf("Test 2 passed: required tags\n");
}
else
{
printf("Test 2 failed: required tags\n");
return 1;
}
//printf("Query returned %ld results:\n", myResults.size());
//printf("%s", printURIList(myResults).c_str());
}

//Make block so objects are destroyed after section completes
{
//Make query
localURIQuery myQuery;

requiredIntegerCondition *myIntegerConditionPointer;
myIntegerConditionPointer = myQuery.add_integerconditions();
myIntegerConditionPointer->set_field("integerKey4");
myIntegerConditionPointer->set_comparison(EQUAL);
myIntegerConditionPointer->set_value(4);

//Get query results
std::vector<localURI> myResults;

if(myURIDatabase.processQuery(myQuery, myResults) != 1)
{
fprintf(stderr, "Error, unable to process query\n");
return 1;
}

if(myResults.size() == 2)
{
printf("Test 3 passed: integer condition\n");
}
else
{
printf("Test 3 failed: integer condition\n");
return 1;
}
//printf("Query returned %ld results:\n", myResults.size());
//printf("%s", printURIList(myResults).c_str());
}

//Make block so objects are destroyed after section completes
{
//Make query
localURIQuery myQuery;

requiredDoubleCondition *myDoubleConditionPointer;
myDoubleConditionPointer = myQuery.add_doubleconditions();
myDoubleConditionPointer->set_field("doubleKey4");
myDoubleConditionPointer->set_comparison(GREATER_THAN);
myDoubleConditionPointer->set_value(3.999);

//Get query results
std::vector<localURI> myResults;

if(myURIDatabase.processQuery(myQuery, myResults) != 1)
{
fprintf(stderr, "Error, unable to process query\n");
return 1;
}

if(myResults.size() == 2)
{
printf("Test 4 passed: double condition\n");
}
else
{
printf("Test 4 failed: double condition\n");
return 1;
}
//printf("Query returned %ld results:\n", myResults.size());
//printf("%s", printURIList(myResults).c_str());
}

return 0;
}
