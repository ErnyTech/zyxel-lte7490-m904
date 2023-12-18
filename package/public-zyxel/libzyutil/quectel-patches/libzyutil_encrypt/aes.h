#ifndef __AES_H__
#define __AES_H__

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
#define MRD_PASSWD_PREFIX "__encrypted__"
#define MRD_KEY_LENGTH 32
//Avoid Reverse Code Engineering found encryptKey
void initKey(char *key);
void setKey1(char *key);
void setKey2(char *key);
void setKey3(char *key);
void setKey4(char *key);
void setKey5(char *key);
void setKey6(char *key);
#endif

unsigned char *aesDecryptCbc256(const char *input, char *inputKey);

unsigned char *aesEncryptCbc256(const char *input, char *inputKey);


#endif
