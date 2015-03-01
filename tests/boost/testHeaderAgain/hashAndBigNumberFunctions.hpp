#ifndef HASHANDBIGNUMBERFUNCTIONSHPP
#define HASHANDBIGNUMBERFUNCTIONSHPP

#include<sodium/crypto_hash_sha512.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <string>


//Macros to help with hex to nibble/nibble to hex conversion
//Adapted from http://stackoverflow.com/questions/9106551/converting-a-binary-char-to-a-hexadecimal-char
#define TOHEX(c) ((c) > 9 ? (c) - 10 + 'A' : (c) + '0')
#define TONIBBLE(c) ((c) > '9' ? (c) + 10 - 'A' : (c) - '0')

/*
This function takes in an array of binary bytes and places the corresponding hex character string (including the terminating '\0' character) in the given buffer
@param inputBinaryArray: The array of binary characters to convert
@param inputBinaryArraySize: The size of the binary array
@return: an std::string with "0x" followed by the corresponding hex characters
*/
std::string convertBinaryToHexString(const char *inputBinaryArray, uint32_t inputBinaryArraySize);

/*
This function takes in a big int and converts it to a printable string
@param inputBigInteger: The big int to convert to a printable string
@return: an std::string with "0x" followed by the corresponding hex characters
*/
std::string convertUInt512ToHexString(boost::multiprecision::uint512_t &inputBigInteger);

/*
This function takes in a big int and converts it to a printable string
@param inputBigInteger: The big int to convert to a printable string
@return: an std::string with followed by the corresponding decimal characters
*/
std::string convertUInt512ToDecimalString(boost::multiprecision::uint512_t &inputBigInteger);

/*
This function takes in an array of binary bytes and places the corresponding hex character string (including the terminating '\0' character) in the given buffer
@param inputHexString: The hex string to convert to binary without a leading "0x" (it does not check)
@param inputHexStringSize: The size of the hex string to convert
@param inputBinaryBuffer: The buffer to place the result in.  It must be at least 1/2 the size of the hex string
@param inputBinaryBufferSize: The size of the buffer to place the result in
@return: 1 if successful and 0 otherwise
*/
int convertHexStringToBinary(const char *inputHexString, uint32_t inputHexStringSize, char *inputBinaryBuffer, uint32_t inputBinaryBufferSize);

/*
This function takes a 64 byte hash and converts it into a boost 512 bit unsigned integer.
@param inputRawHash: The hash to be converted
@param inputRawHashSize: The size of the hash in bytes (should be 64)
@param inputBigIntegerBuffer: The variable to place the result in
@return: 1 if successful and 0 otherwise
*/
int createUInt512FromRawHash(const char *inputRawHash, uint32_t inputRawHashSize, boost::multiprecision::uint512_t &inputBigIntegerBuffer);

/*
This function takes a uint512_t and converts it into a 64 byte raw hash.
@param inputBigInteger: The uint512_t to convert
@param inputRawHashBuffer: The variable to place the raw hash buffer in
@param inputRawHashBufferSize: The size of the hash buffer (should be 64)
@return: 1 if successful and 0 otherwise
*/
int createRawHashFromUInt512(const boost::multiprecision::uint512_t &inputBigInteger, char *inputRawHashBuffer, uint32_t inputRawHashBufferSize);





#endif
