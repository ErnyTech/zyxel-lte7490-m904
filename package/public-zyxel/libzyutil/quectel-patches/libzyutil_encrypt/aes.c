/**
  AES encryption/decryption demo program using OpenSSL EVP apis
  gcc -Wall openssl_aes.c -lcrypto

  this is public domain code.

  Saju Pillai (saju.pillai@gmail.com)
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "zos_api.h"
#include "zlog_api.h"


char *base64_encode(unsigned char *input, int length)
{
	BIO *bmem, *b64;
	BUF_MEM *bptr;
	char *buffer;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	buffer= (char *)malloc(bptr->length + 1);
    if (buffer == NULL)
    {
        ZLOG_ERROR("memory insufficient");
        BIO_free_all(b64);
        return NULL;
    }

	memcpy(buffer, bptr->data, bptr->length);
	buffer[bptr->length] = 0;

	BIO_free_all(b64);

	return buffer;
}

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD 
//Avoid Reverse Code Engineering found encryptKey
void setKey2(char *key){
	key[27] = '\0';
	key[1] = 'E';
	key[6] = 'A';
	key[16] = 'W';
	key[5] = 'R';

}
#endif
char *base64_decode(unsigned char *input, int *length)
{
	BIO *b64, *bmem;
	BUF_MEM *bptr;
	char *buffer;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new_mem_buf(input, -1);
	bmem = BIO_push(b64, bmem);

	BIO_get_mem_ptr(bmem, &bptr);

	buffer = (char *)ZOS_MALLOC(bptr->length + 1);
    if (buffer == NULL)
    {
        ZLOG_ERROR("memory insufficient");
        BIO_free_all(bmem);
        return NULL;
    }

	memset(buffer, 0, bptr->length);
	*length = bptr->length;
	BIO_read(bmem, buffer, bptr->length);
	BIO_free_all(bmem);

	return buffer;
}
#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
//Avoid Reverse Code Engineering found encryptKey
void setKey3(char *key){
	key[19] = 'd';
	key[0] = 'K';
	key[14] = 's';
	key[24] = 'M';
	key[3] = 'f';
	key[21] = 'r';
}
#endif
//int cmsB64_encode(const unsigned char *src, unsigned int srclen, char **dest);
//int cmsB64_decode(const char *base64Str, unsigned char **binaryBuf, unsigned int *binaryBufLen);

/**
 * Create an 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx,
             EVP_CIPHER_CTX *d_ctx)
{
  int i, nrounds = 5;
  unsigned char key[32], iv[32];

  /*
   * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
   * nrounds is the number of times the we hash the material. More rounds are more secure but
   * slower.
   */
  i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
  if (i != 32) {
    printf("Key size is %d bits - should be 256 bits\n", i);
    return -1;
  }

  if(e_ctx){
	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
  }
  if(d_ctx){
	EVP_CIPHER_CTX_init(d_ctx);
	EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
  }

  return 0;
}

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
//Avoid Reverse Code Engineering found encryptKey
void setKey1(char *key){
	key[25] = 'r';
	key[22] = 'o';
	key[9] = 'e';
	key[18] = 'r';
	key[13] = 'A';
}
#endif
/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len)
{
#if 0
  /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
  int c_len = *len + AES_BLOCK_SIZE -1, f_len = 0;
  unsigned char *ciphertext = malloc(c_len);
#else
    int c_len=*len, f_len=0;
    unsigned char *ciphertext = (unsigned char *)ZOS_MALLOC(c_len + EVP_MAX_BLOCK_LENGTH + 1);

    if (ciphertext == NULL)
    {
        ZLOG_ERROR("memory insufficient");
        return NULL;
    }

    c_len = 0;
#endif

  /* allows reusing of 'e' for multiple encryption cycles */
  EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

  /* update ciphertext, c_len is filled with the length of ciphertext generated,
    *len is the size of plaintext in bytes */
  EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

  /* update ciphertext with the final remaining bytes */
  EVP_EncryptFinal_ex(e, ciphertext+c_len, &f_len);

  *len = c_len + f_len;
  return ciphertext;
}

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = (unsigned char *)ZOS_MALLOC(p_len+EVP_MAX_BLOCK_LENGTH + 1);

    if (plaintext == NULL)
    {
        ZLOG_ERROR("memory insufficient");
        return NULL;
    }

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
//Avoid Reverse Code Engineering found encryptKey
void setKey6(char *key){
	key[12] = 'p';
	key[7] = 'C';
	key[15] = 's';
	key[10] = 's';
	key[8] = 'C';
}
#endif
unsigned char *aesEncryptCbc256(const char *input, char *inputKey)
{
  /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
     status of enc/dec operations */
// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  EVP_CIPHER_CTX en;
#else
  EVP_CIPHER_CTX *en = NULL;
#endif

  /* 8 bytes to salt the key_data during key generation. This is an example of
     compiled in salt. We just read the bit pattern created by these two 4 byte
     integers on the stack as 64 bits of contigous salt material -
     ofcourse this only works if sizeof(int) >= 4 */
  unsigned int salt[] = {12345, 54321};
  unsigned char *key_data;
  int key_data_len;

  unsigned char *ciphertextB64 = NULL;

  /* the key_data is read from the argument list */
  key_data = (unsigned char *)inputKey;
  key_data_len = strlen(inputKey);

// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
// #else
#if OPENSSL_VERSION_NUMBER >= 0x10100000
  en = EVP_CIPHER_CTX_new();
  if (en == NULL)
  {
    return NULL;
  }
#endif

  /* gen key and iv. init the cipher ctx object */
// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  if (aes_init(key_data, key_data_len, (unsigned char *)&salt, &en, NULL))
#else
  if (aes_init(key_data, key_data_len, (unsigned char *)&salt, en, NULL))
#endif
  {
    printf("Couldn't initialize AES cipher\n");
    return NULL;
  }

  /* encrypt and decrypt each input string and compare with the original */
  unsigned char *ciphertext;
  int olen, len;

  /* The enc/dec functions deal with binary data and not C strings. strlen() will
     return length of the string without counting the '\0' string marker. We always
     pass in the marker byte to the encrypt/decrypt functions so that after decryption
     we end up with a legal C string */
  olen = len = strlen(input)+1;

// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  ciphertext = aes_encrypt(&en, (unsigned char *)input, &len);
#else
  ciphertext = aes_encrypt(en, (unsigned char *)input, &len);
#endif

  ciphertextB64 = (unsigned char *)base64_encode((unsigned char*)ciphertext, len);

  ZOS_FREE(ciphertext);

// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  EVP_CIPHER_CTX_cleanup(&en);
#else
  EVP_CIPHER_CTX_free(en);
#endif

  return ciphertextB64;
}

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
//Avoid Reverse Code Engineering found encryptKey
void setKey4(char *key){
	key[26] = 'D';
	key[17] = 'O';
	key[20] = 'f';
	key[2] = 'Y';
}
void setKey5(char *key){
	key[4] = 'O';
	key[23] = 'M';
	key[11] = 's';
}
#endif
unsigned char *aesDecryptCbc256(const char *input, char *inputKey)
{
  /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
     status of enc/dec operations */
// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  EVP_CIPHER_CTX de;
#else
  EVP_CIPHER_CTX *de = NULL;
#endif

  /* 8 bytes to salt the key_data during key generation. This is an example of
     compiled in salt. We just read the bit pattern created by these two 4 byte
     integers on the stack as 64 bits of contigous salt material -
     ofcourse this only works if sizeof(int) >= 4 */
  unsigned int salt[] = {12345, 54321};
  unsigned char *key_data;
  int key_data_len;

  char *decode = NULL;
  int decode_len = 0;
  unsigned char *ciphertext = (unsigned char *) input;

  /* the key_data is read from the argument list */
  key_data = (unsigned char *)inputKey;
  key_data_len = strlen(inputKey);

// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
#else
  de = EVP_CIPHER_CTX_new();
  if (de == NULL)
  {
    return NULL;
  }
#endif

  /* gen key and iv. init the cipher ctx object */
// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  if (aes_init(key_data, key_data_len, (unsigned char *)&salt, NULL, &de))
#else
  if (aes_init(key_data, key_data_len, (unsigned char *)&salt, NULL, de))
#endif
  {
    printf("Couldn't initialize AES cipher\n");
    return NULL;
  }

  /* encrypt and decrypt each input string and compare with the original */
  char *plaintext = NULL;

  /* The enc/dec functions deal with binary data and not C strings. strlen() will
     return length of the string without counting the '\0' string marker. We always
     pass in the marker byte to the encrypt/decrypt functions so that after decryption
     we end up with a legal C string */

  decode = base64_decode( ciphertext, &decode_len);
  //cmsB64_decode( (char*) ciphertext, (unsigned char**)&decode, &decode_len);
  if(decode){
// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
    plaintext = (char *)aes_decrypt(&de, (unsigned char *)decode, &decode_len);
#else
    plaintext = (char *)aes_decrypt(de, (unsigned char *)decode, &decode_len);
#endif
	ZOS_FREE(decode);
  }

// #ifdef OPENSSL_VERSION_1_0_2n /* Jake, 2020-02-07, upgrade to openssl 1.1.1b. */
#if OPENSSL_VERSION_NUMBER < 0x10100000
  EVP_CIPHER_CTX_cleanup(&de);
#else
  EVP_CIPHER_CTX_free(de);
#endif

  return (unsigned char*)plaintext;
}
#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
//Avoid Reverse Code Engineering found encryptKey
void initKey(char *key){
	setKey2(key);
	setKey4(key);
	setKey5(key);
	setKey6(key);
	setKey1(key);
	setKey3(key);
}
#endif
