#ifndef _ZCFG_UTIL_MTKSOC_RANDOM_PASSWORD_H
#define _ZCFG_UTIL_MTKSOC_RANDOM_PASSWORD_H

//#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
#if defined(ZYXEL_RANDOM_PASSWORD_IN_MRD) || defined(ZYXEL_ENCRYPTED_FIRMWARE_UPGRADE)
#define MRD_PASSWD_PREFIX "__encrypted__"
#define MRD_KEY_LENGTH 64
zcfgRet_t zyUtilIGetMrdPasswd(int index, char **passwd);
//Avoid Reverse Code Engineering found encryptKey
void initKey(char *key);
void setKey1(char *key);
void setKey2(char *key);
void setKey3(char *key);
void setKey4(char *key);
void setKey5(char *key);
void setKey6(char *key);
#endif

#ifdef ZYXEL_RANDOM_ENCRYPTION_SEED_IN_MRD
#define MRD_RANDOM_SEED_PREFIX "__randomSeed__"
#define MD5_RANDOM_SEED_INPUT_SIZE 64
#define DATAMODEL_KEY_LENGTH 64
void mergeRandomSeedKey(char *key);
int zyutilDig2str(unsigned char* dig, int dig_len, char* str, int str_len); 
#endif

#ifdef ZYXEL_LWM2M_PSK_IN_MRD
zcfgRet_t zyUtilIGetMrdLwm2mPSK(char **psk);
#endif

#endif
