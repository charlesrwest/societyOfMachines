#include<iostream>
#include<fstream>
#include<string>
#include "addressbook.pb.h"

using namespace std;

//Iterates through all people in the AddressBook and prints info about them
void ListPeople(const AddressBook& address_book)
{
for(int i=0; i < address_book.person_size(); i++)
{
const Person& person = address_book.person(i);

cout << "Person ID: " << person.id() << endl;
cout << "  Name: " << person.name() << endl;
if(person.has_email())
{
cout << "  E-mail address: " << person.email() << endl;
}

for(int j=0; j < person.phone_size(); j++)
{
const Person::PhoneNumber& phone_number = person.phone(j);

switch(phone_number.type())
{
case Person::MOBILE:
cout << "  Mobile phone #: ";
break;

case Person::HOME:
cout << "  Home phone #: ";
break;

case Person::WORK:
cout << "  Work phone #: ";
break;
}

cout << phone_number.number() << endl;
}

}
}

//Main function: REads the entire address book from a file and prints the information inside
int main(int argc, char **argv)
{
GOOGLE_PROTOBUF_VERIFY_VERSION;

if(argc != 2)
{
cerr << "Usage:  " << argv[0] << "ADDRESS_BOOK_FILE" << endl;
return -1;
}

AddressBook address_book;

{
//Read the existing address book.
fstream input(argv[1], ios::in | ios::binary);
if(!address_book.ParseFromIstream(&input))
{
cerr << "Failed to parse address book." << endl;
return -1;
}
}

ListPeople(address_book);

//Optional: Delete all global objects allocated by libprotobuf
google::protobuf::ShutdownProtobufLibrary();

return 0;
}

