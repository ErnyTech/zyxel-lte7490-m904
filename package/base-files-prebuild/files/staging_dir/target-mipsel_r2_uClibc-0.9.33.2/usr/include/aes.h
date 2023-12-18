#ifndef __AES_H__
#define __AES_H__

unsigned char *aesDecryptCbc256(const char *input, char *inputKey);

unsigned char *aesEncryptCbc256(const char *input, char *inputKey);


#endif
