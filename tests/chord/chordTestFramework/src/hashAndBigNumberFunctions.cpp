#include "hashAndBigNumberFunctions.hpp"



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

