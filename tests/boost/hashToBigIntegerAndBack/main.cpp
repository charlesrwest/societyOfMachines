#include<string>
#include<stdio.h>

#include <boost/multiprecision/cpp_int.hpp>
#include<sodium/crypto_hash_sha512.h>

#include<iostream>

std::string convertBinaryToHexString(const char *inputBinaryArray, uint32_t inputBinaryArraySize);

int convertHexStringToBinary(const char *inputHexString, uint32_t inputHexStringSize, char *inputBinaryBuffer, uint32_t inputBinaryBufferSize);

int createUInt512FromRawHash(const char *inputRawHash, uint32_t inputRawHashSize, boost::multiprecision::uint512_t &inputBigIntegerBuffer);

int createRawHashFromUInt512(const boost::multiprecision::uint512_t &inputBigInteger, char *inputRawHashBuffer, uint32_t inputRawHashBufferSize);

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
if(createUInt512FromRawHash((const char *) hashBuffer, crypto_hash_sha512_BYTES, bigInteger) != 1)
{
fprintf(stderr, "Error setting integer with raw hash\n");
return -1;
}

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





//Adapted from http://stackoverflow.com/questions/9106551/converting-a-binary-char-to-a-hexadecimal-char
#define TOHEX(c) ((c) > 9 ? (c) - 10 + 'A' : (c) + '0')

#define TONIBBLE(c) ((c) > '9' ? (c) + 10 - 'A' : (c) - '0')

/*
This function takes in an array of binary bytes and places the corresponding hex character string (including the terminating '\0' character) in the given buffer
@param inputBinaryArray: The array of binary characters to convert
@param inputBinaryArraySize: The size of the binary array
@return: an std::string with "0x" followed by the corresponding hex characters
*/
std::string convertBinaryToHexString(const char *inputBinaryArray, uint32_t inputBinaryArraySize)
{
char buffer[2];
std::string returnValue = "0x";

for(int i=0; i<inputBinaryArraySize; i++)
{
buffer[0] = TOHEX((inputBinaryArray[i] >> 4) & 0xf);
buffer[1] = TOHEX(inputBinaryArray[i] & 0xf);

returnValue.append(buffer, 2);
}

return returnValue;
}

/*
This function takes in an array of binary bytes and places the corresponding hex character string (including the terminating '\0' character) in the given buffer
@param inputHexString: The hex string to convert to binary without a leading "0x" (it does not check)
@param inputHexStringSize: The size of the hex string to convert
@param inputBinaryBuffer: The buffer to place the result in.  It must be at least 1/2 the size of the hex string
@param inputBinaryBufferSize: The size of the buffer to place the result in
@return: 1 if successful and 0 otherwise
*/
int convertHexStringToBinary(const char *inputHexString, uint32_t inputHexStringSize, char *inputBinaryBuffer, uint32_t inputBinaryBufferSize)
{
if(inputHexStringSize/2 > inputBinaryBufferSize)
{
return 0;
}

for(int i=0; i<inputHexStringSize/2; i++)
{
inputBinaryBuffer[i] = (TONIBBLE(inputHexString[i*2]) << 4) | TONIBBLE(inputHexString[i*2+1]);
}

return 1;
}

/*
This function takes a 64 byte hash and converts it into a boost 512 bit unsigned integer.
@param inputRawHash: The hash to be converted
@param inputRawHashSize: The size of the hash in bytes (should be 64)
@param inputBigIntegerBuffer: The variable to place the result in
@return: 1 if successful and 0 otherwise
*/
int createUInt512FromRawHash(const char *inputRawHash, uint32_t inputRawHashSize, boost::multiprecision::uint512_t &inputBigIntegerBuffer)
{
if(inputRawHashSize != 64 || inputRawHash == nullptr)
{
return 0;
}

inputBigIntegerBuffer = boost::multiprecision::uint512_t(convertBinaryToHexString((const char *) inputRawHash, inputRawHashSize));
return 1;
}

/*
This function takes a uint512_t and converts it into a 64 byte raw hash.
@param inputBigInteger: The uint512_t to convert
@param inputRawHashBuffer: The variable to place the raw hash buffer in
@param inputRawHashBufferSize: The size of the hash buffer (should be 64)
@return: 1 if successful and 0 otherwise
*/
int createRawHashFromUInt512(const boost::multiprecision::uint512_t &inputBigInteger, char *inputRawHashBuffer, uint32_t inputRawHashBufferSize)
{
if(inputRawHashBufferSize != 64 || inputRawHashBuffer == nullptr)
{
return 0;
}

std::string hexVersionOfBigInteger = inputBigInteger.str(-1,std::ios::hex);

return convertHexStringToBinary(hexVersionOfBigInteger.c_str(), hexVersionOfBigInteger.size(), inputRawHashBuffer, inputRawHashBufferSize);
}
