#include <boost/multiprecision/cpp_int.hpp>
#include<string>

std::string convertBinaryToHexString(const char *inputBinaryArray, uint32_t inputBinaryArraySize);

int convertHexStringToBinary(const char *inputHexString, uint32_t inputHexStringSize, char *inputBinaryBuffer, uint32_t inputBinaryBufferSize);

int main(int argc, char **argv)
{
char testCharacter = 0x50;

std::string hexVersion = convertBinaryToHexString(&testCharacter, 1);

boost::multiprecision::uint512_t bigInteger(hexVersion);

//First argument doesn't seem to be used, prints without leading 0x.  Also always seems to use upper case
printf("Current value: %s\n", bigInteger.str(-1,std::ios::hex).c_str());

//Test hex to binary conversion function
char character;
if(convertHexStringToBinary(hexVersion.c_str()+2, hexVersion.size()-2, &character, 1) != 1)
{
fprintf(stderr, "Error converting hex to binary\n");
return -1;
}

printf("Resulting binary value: %x\n", character);

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
inputBinaryBuffer[i] |= (TONIBBLE(inputHexString[i*2]) << 4) | TONIBBLE(inputHexString[i*2+1]);
}

return 1;
}
