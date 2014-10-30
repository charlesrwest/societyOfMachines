#include<cstdio>
#include<sodium/crypto_hash_sha512.h>
#include<cstring>

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
printf("Hash result: ");

for(int i=0; i<crypto_hash_sha512_BYTES; i++)
{
printf("%.2x", hashBuffer[i]);
}

printf("\n");

return 0;
}
