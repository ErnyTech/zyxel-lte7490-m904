/***************************************************************************************
 *      Copyright(c) 2014 ECONET Incorporation All rights reserved.
 *
 *      This is unpublished proprietary source code of ECONET Networks Incorporation
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 ***************************************************************************************
 */

#ifndef __SHEADER_H__
#define __SHEADER_H__

typedef enum {
	AES_MODE_DISABLE = 0,
	AES_MODE_ECB,
	AES_MODE_MAX
} AES_MODE_T;

typedef struct {
	unsigned char iv[16];
	unsigned char hmac_sha256[32];
} AES_INFO;

typedef struct {
	unsigned int magic;
	unsigned int version;
	unsigned int header_len;
	unsigned int image_len;
	unsigned char sig[512];
	unsigned char rsa_pub[512];
	AES_MODE_T aes_mode;
	unsigned char aes_img_key[256];
	AES_INFO aes_img_key_info;
	union {
		AES_INFO aes_efuse_info;
		AES_INFO aes_bootram_info;
		AES_INFO aes_kernel_info;
	};
	AES_INFO aes_spram_info;
	AES_INFO aes_lzma_info;
	AES_INFO aes_verify_info;
	unsigned char resv3[504];
	unsigned int crc;
} SECURE_HEADER_V2;

typedef struct {
	unsigned int magic;
	unsigned int version;
	unsigned char sig[256];
	unsigned int image_len;
	unsigned int resv2;
	unsigned int crc;
} SECURE_HEADER_V1;

typedef struct {
	union {
		SECURE_HEADER_V1 v1;
		SECURE_HEADER_V2 v2;
	};
} SECURE_HEADER;

typedef enum {
	SIGNATURE_VERIFY_STATUS_CORRECT = 0,
	SIGNATURE_VERIFY_INCORRECT
} SIGNATURE_VERIFY_STATUS_T;

typedef struct secure_img{
	SECURE_HEADER *sHeader;
	unsigned char *start;
	unsigned int len;
	unsigned int crc;
} SECURE_IMG_T;

#endif /* __SHEADER_H__ */
