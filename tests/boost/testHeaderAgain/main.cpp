#include "hashAndBigNumberFunctions.hpp"


int main(int argc, char **argv)
{
//Define data to hash
const unsigned char *message = (const unsigned char *) "testThisTest";
int messageLength = strlen((const char *) message);

//Define buffer to store hash in
unsigned char hashBuffer[crypto_hash_sha512_BYTES];

//Generate hash
if(crypto_hash_sha512(hashBuffer, message, messageLength) != 0)
{
fprintf(stderr, "Error, unable to generate message hash\n");
return -1;
}

//Print out hash
printf("Hash result: \n0x");

for(int i=0; i<crypto_hash_sha512_BYTES; i++)
{
printf("%.2X", hashBuffer[i]);
}

printf("\n");


//Make big int out of hash
boost::multiprecision::uint512_t bigInteger;
/* TODO: Change back
if(createUInt512FromRawHash((const char *) hashBuffer, crypto_hash_sha512_BYTES, bigInteger) != 1)
{
fprintf(stderr, "Error setting integer with raw hash\n");
return -1;
}
*/

bigInteger = 0;

//Make hex string from big int
std::string hexVersionOfHash = convertBinaryToHexString((const char *) hashBuffer, crypto_hash_sha512_BYTES);

//First argument doesn't seem to be used, prints without leading 0x.  Also always seems to use upper case
printf("Current value of bigint: \n0x%s\n", bigInteger.str(-1,std::ios::hex).c_str());

printf("hexVersionOfHash: \n0x");
for(int i=0; i<hexVersionOfHash.size()-2; i++)
{
printf("%c", *(hexVersionOfHash.c_str()+2+i));
}
printf("\n");

//Test hex to binary conversion function
unsigned char hashBuffer2[crypto_hash_sha512_BYTES];
if(convertHexStringToBinary(hexVersionOfHash.c_str()+2, hexVersionOfHash.size()-2, (char *)  hashBuffer2, crypto_hash_sha512_BYTES) != 1)
{
fprintf(stderr, "Error converting hex to binary\n");
return -1;
}

printf("Hex to binary conversion result: \n0x");

for(int i=0; i<crypto_hash_sha512_BYTES; i++)
{
printf("%.2X", hashBuffer2[i]);
}

printf("\n");

//Test big int to binary hash conversion
unsigned char hashBuffer4[crypto_hash_sha512_BYTES];
if(createRawHashFromUInt512(bigInteger, (char *) hashBuffer4, crypto_hash_sha512_BYTES) != 1)
{
printf("Failed to convert big int to binary hash\n");
}

printf("Big int to binary conversion result: \n0x");
for(int i=0; i<crypto_hash_sha512_BYTES; i++)
{
printf("%.2X", hashBuffer4[i]);
}
printf("\n");


//Test overflow

//Make another hash
const unsigned char *message2 = (const unsigned char *) "testThatTest";
int messageLength2 = strlen((const char *) message2);

//Define buffer to store hash in
unsigned char hashBuffer3[crypto_hash_sha512_BYTES];

//Generate hash
if(crypto_hash_sha512(hashBuffer3, message2, messageLength2) != 0)
{
fprintf(stderr, "Error, unable to generate message hash\n");
return -1;
}

printf("Second message hash result: \n0x");
for(int i=0; i<crypto_hash_sha512_BYTES; i++)
{
printf("%.2X", hashBuffer3[i]);
}
printf("\n");

//Make the corresponding bigint

boost::multiprecision::uint512_t bigInteger2(convertBinaryToHexString((const char *) hashBuffer3, crypto_hash_sha512_BYTES));

//Print out second bigint
printf("Second bigint value: \n0x%s\n", bigInteger2.str(-1,std::ios::hex).c_str());

boost::multiprecision::uint512_t result = bigInteger+bigInteger2;

printf("bigint + bigint2: \n0x%s\n", result.str(-1,std::ios::hex).c_str());



return 0;
}

