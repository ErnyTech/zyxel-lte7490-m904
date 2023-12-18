#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <endian.h>
#include <byteswap.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#include <sHeader.h>
#include <tHeader.h>

#define DBG 1
#define AES_SIMULATION_EFUSE 0
#define AES_SIMULATION_OFFSET 0
#define AES_SIMULATION_FLASH_BOOT 0

#define AES_SIMULATION_EFUSE_INFO_ERROR 0
#define AES_SIMULATION_IMG_KEY_INFO_ERROR 0
#define AES_SIMULATION_SPRAM_INFO_ERROR 0
#define AES_SIMULATION_LZMA_INFO_ERROR 0
#define SIMULATION_SIG_ERROR 0

#if DBG
	#define DBG_MSG(F, B...)	{ \
									printf(F, ##B); \
								}
#else
	#define DBG_MSG(F, B...)	
#endif

#ifdef __LITTLE_ENDIAN	//Compile PC define
#if defined(TCSUPPORT_LITTLE_ENDIAN)	//EN7580 SoC define
#define STORE32_LE(X)	(X)
#else
#define STORE32_LE(X)	bswap_32(X)
#endif
#else
#if defined(TCSUPPORT_LITTLE_ENDIAN)
#define STORE32_LE(X)	bswap_32(X)
#else
#define STORE32_LE(X)	(X)
#endif
#endif

unsigned long crc32(unsigned char *buf, unsigned int len)
{
	int j;
	unsigned long crc = 0xFFFFFFFF;
	unsigned long crc_polynomial = 0xedb88320;

	while(len--) {
		crc ^= *buf++;
		
		for(j = 0; j < 8; j++) {
			crc = (crc >> 1) ^ (-(long)(crc & 1) & crc_polynomial);
		}
	}

	return crc;
}

int parseNumber(const char *cp, unsigned int base, unsigned long *parseAddr)
{
	int retVal = 0;
	char *strtolEnd = NULL;

	errno = 0;
	*parseAddr = strtoul(cp, &strtolEnd, base);

	if (strtolEnd == cp) {
		fprintf(stderr, "%s: not a heximal number\n", cp);
		retVal = -1;
	} else if ('\0' != *strtolEnd) {
		fprintf(stderr, "%s: extra characters at end of input: %s\n", cp, strtolEnd);
		retVal = -1;
	} else if (ULONG_MAX == *parseAddr && ERANGE == errno) {
		fprintf(stderr, "%s out of range of type unsigned long\n", cp);
		retVal = -1;
	} else if (0 == *parseAddr && EINVAL == errno) {
		fprintf(stderr, "%s is invalid of type unsigned long\n", cp);
		retVal = -1;
	}

	DBG_MSG("parseAddr:0x%lu\n", *parseAddr);

	return retVal;
}

void usage()
{
	fprintf(stderr, "Usage: sheader <-i input> <-a fill_addr> <-v ver> <[-s sig] [-m aes_mode] [-k rsa_key] [-e aes_key] [-d spram] [-l lzma] [-y verify] [-b bootram]>\n");
}

int fill_cipher(AES_INFO *info, char *file, char *image, unsigned int image_len, int errSim)
{
	FILE *f = NULL;
	int ret = -1;
	unsigned int n = 0;
	
	/*
     *  The encrypted file must be structured as follows:
     *
     *        00 .. 15              Initialization Vector
     *        16 .. 31              AES Encrypted Block #1
     *           ..
     *      N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
     *  (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)
     */
	if (!(f = fopen(file, "r+"))) {
		fprintf(stderr, "can not open \"%s\" for reading\n", file);
		ret = -1;
		goto exit;
	}

	/* Fill IV info */
	n = fread(info->iv, 1, sizeof(info->iv), f);
	if (sizeof(info->iv) != n) {
		fprintf(stderr, "fread IV failure, file \"%s\".\n", file);
		ret = -1;
		goto exit;
	}
	
	/* Fill cipher */
	n = fread(image, 1, image_len, f);
	if (image_len != n) {
		fprintf(stderr, "fread ciphertext failure, file \"%s\".\n", file);
		ret = -1;
		goto exit;
	}

	/* Fill HMAC-SHA-256 info */
	n = fread(info->hmac_sha256, 1, sizeof(info->hmac_sha256), f);
	if (sizeof(info->hmac_sha256) != n) {
		fprintf(stderr, "fread hmac_sha256 failure, file \"%s\".\n", file);
		ret = -1;
		goto exit;
	}

	if(errSim == 1) {
		info->hmac_sha256[0] ^= 0xFF;
	}

	ret = 0;
	
exit:
	if(f)
		fclose(f);
	
	return ret;
}

int main(int argc, char **argv) 
{
	int c;
	char input_file[256] = {0};
	char signature_file[256] = {0};
	char signature_source_file[256] = {0};
	char rsa_cipher_file[256] = {0};
	char spram_cipher_file[256] = {0};
	char lzma_cipher_file[256] = {0};
	char verify_cipher_file[256] = {0};
	char bootram_cipher_file[256] = {0};
	char aes_img_cipher_file[256] = {0};
	char *buf = NULL;
	unsigned long fill_addr = 0;
	int isInput = 0, isSig = 0, isAddr = 0, isRsaKey = 0, isVer = 0, isSpramCipher = 0, isLzmaCipher = 0;
	int isAesMode = 0, isAesImgKey = 0, isVerifyCipher = 0, isBootramCipher = 0;
	int input_file_len = 0, sig_file_len = 0, n = 0;
	FILE *in = NULL;
	FILE *sig = NULL;
	FILE *sim = NULL;
	int retVal = -1;
	SECURE_HEADER *sh;
	TCBOOT_HEADER *th;
	int i = 0;
	struct stat st;
	unsigned long Ver = 0, AesMode = 0;
	int simulation_offset = 0;
	int efuse_info_error = 0, img_key_info_error = 0, spram_info_error = 0, lzma_info_error = 0;
	unsigned char *ch;
	int sigLen, shdrLen;

	while ((c = getopt(argc, argv, "i:a:s:k:v:d:l:m:e:y:b:")) != -1) {
		switch (c) {
			/* input file */
			case 'i':
				strncpy(input_file, optarg, sizeof(input_file) - 1);
				isInput = 1;
				break;
			/* signature file */
			case 's':
				strncpy(signature_source_file, optarg, sizeof(signature_source_file) - 1);
				isSig = 1;
				break;
			/* signature start address */
			case 'a':
				if(parseNumber(optarg, 16, &fill_addr) == -1) {
					goto free_source;
				}
				isAddr = 1;
				break;
			/* fill Encrypted AES key */
			case 'e':
				strncpy(aes_img_cipher_file, optarg, sizeof(aes_img_cipher_file) - 1);
				DBG_MSG("AES cipher file:%s\n", aes_img_cipher_file);
				isAesImgKey = 1;
				break;
			/* fill Encrypted RSA public key */
			case 'k':
				strncpy(rsa_cipher_file, optarg, sizeof(rsa_cipher_file) - 1);
				DBG_MSG("rsa cipher file:%s\n", rsa_cipher_file);
				isRsaKey = 1;
				break;
			/* version */
			case 'v':
				if(parseNumber(optarg, 10, &Ver) == -1) {
					goto free_source;
				}
				isVer = 1;
				break;
			/* fill Encrypted spram.img */
			case 'd':
				strncpy(spram_cipher_file, optarg, sizeof(spram_cipher_file) - 1);
				DBG_MSG("spram cipher file:%s\n", spram_cipher_file);
				isSpramCipher = 1;
				break;
			/* fill Encrypted lzma.img */
			case 'l':
				strncpy(lzma_cipher_file, optarg, sizeof(lzma_cipher_file) - 1);
				DBG_MSG("lzma cipher file:%s\n", lzma_cipher_file);
				isLzmaCipher = 1;
				break;
			/* fill Encrypted verify.img */
			case 'y':
				strncpy(verify_cipher_file, optarg, sizeof(verify_cipher_file) - 1);
				DBG_MSG("verify cipher file:%s\n", verify_cipher_file);
				isVerifyCipher = 1;
				break;
			/* fill Encrypted bootram.img */
			case 'b':
				strncpy(bootram_cipher_file, optarg, sizeof(bootram_cipher_file) - 1);
				DBG_MSG("verify cipher file:%s\n", bootram_cipher_file);
				isBootramCipher = 1;
				break;
			case 'm':
				if(parseNumber(optarg, 10, &AesMode) == -1) {
					goto free_source;
				}
				if(AesMode >= AES_MODE_MAX) {
					fprintf(stderr, "AES mode:%lu must be %d ~ %d.\n", AesMode, AES_MODE_DISABLE, AES_MODE_MAX - 1);
					goto free_source;
				}
				isAesMode = 1;
				break;
			default:
				usage();
				goto free_source;
		}
	}

	if((isInput == 0) || (isAddr == 0) || (isVer == 0)) {
		printf("error input.\n");
		usage();
		goto free_source;
	}

	if (!(in = fopen(input_file, "r+"))) {
		fprintf(stderr, "can not open \"%s\" for reading\n", input_file);
		usage();
		goto free_source;
	}

	/* get input file size */
	if(stat(input_file, &st) != 0) {
		fprintf(stderr, "get input file size failed\n");
		goto free_source;
	}

	/* Initialization */
	srand(time(NULL));
	
#if AES_SIMULATION_OFFSET
	simulation_offset = st.st_size;
	if(simulation_offset > 0x40000) {
		simulation_offset = 0x40000;
	}
#endif
	input_file_len = st.st_size + simulation_offset;

	if (!(buf = malloc(input_file_len))) {
		fprintf(stderr, "malloc failed\n");
		goto free_source;
	}

	n = fread(buf, 1, input_file_len, in);
	if ((input_file_len - simulation_offset) != n) {
		fprintf(stderr, "fread failure or file \"%s\" too large\n",input_file);
		fprintf(stderr, "input_file_len = %d, read length = %d\n", input_file_len, n);
		goto free_source;
	}

	if(Ver == 2) {
		shdrLen = sizeof(SECURE_HEADER_V2);
	} else {
		shdrLen = sizeof(SECURE_HEADER_V1);
	}

	sh = (SECURE_HEADER *)(buf + fill_addr);
	th = (TCBOOT_HEADER *)(buf + shdrLen);

	/* fill secure header */
#if AES_SIMULATION_FLASH_BOOT == 0
	memcpy(&sh->v2.magic, "ECNT", sizeof(sh->v2.magic));
#endif

	if(isSig == 1) {
		fprintf(stderr,"source file=%s\n",signature_source_file);
		/* get sig file size */
		if(stat(signature_source_file, &st) != 0) {
			fprintf(stderr, "get sig file size failed\n");
			goto free_source;
		}
		sig_file_len = st.st_size;

		/* image_len including CRC32 */
		if(Ver == 2) {
			sh->v2.image_len = STORE32_LE(sig_file_len + 4);
			ch = sh->v2.sig;
			sigLen = (int)sizeof(sh->v2.sig);
		} else {
			sh->v1.image_len = STORE32_LE(sig_file_len + 4);
			ch = sh->v1.sig;
			sigLen = (int)sizeof(sh->v1.sig);
		}

		strncpy(signature_file, signature_source_file, sizeof(signature_file) - 1);
		strncat(signature_file, ".sig", sizeof(signature_file) - 1);
		if (!(sig = fopen(signature_file, "rb"))) {
			fprintf(stderr, "can not open file \"%s\" for writing\n", signature_file);
			goto free_source;
		}

		i = 0;
		while((fscanf(sig, "%02X", &c) > 0) && (i < sigLen)) {
			ch[i++] = (unsigned char)c;
		}
		
#if SIMULATION_SIG_ERROR
		ch[rand() % 256] ^= 0xFF;
#endif
	}

//#if defined(TCSUPPORT_SECURE_BOOT_V2)
	if(Ver == 2) {
		sh->v2.version = STORE32_LE(Ver);
		sh->v2.header_len = STORE32_LE(shdrLen);
	} else if(Ver == 1) {
		sh->v1.version = 0;
	}
#if AES_SIMULATION_FLASH_BOOT
	sh->v2.version = 0;
#endif

	if(Ver == 2) {
		if(isRsaKey == 1) {
			memset(&(sh->v2.rsa_pub[0]), 0, sizeof(sh->v2.rsa_pub));
#if AES_SIMULATION_EFUSE_INFO_ERROR
			efuse_info_error = 1;
#endif
			if(fill_cipher(&sh->v2.aes_efuse_info, 
						   rsa_cipher_file, 
						   (char *)sh->v2.rsa_pub,
						   sizeof(sh->v2.rsa_pub),
						   efuse_info_error) != 0) {
				usage();
				goto free_source;
			}
			/* EFUSE simulation data */
#if AES_SIMULATION_EFUSE
			memset(&(sh->v2.resv3[0]), 0, sizeof(sh->v2.resv3));
			if( ( sim = fopen( "efuse_data.bin", "rb" ) ) == NULL ) {
				fprintf( stderr, "fopen(%s,rb) failed\n", "efuse_data.bin" );
				goto free_source;
			}

			/* (isEfuseTest == 1) */
			*(unsigned int *)sh->v2.resv3 = STORE32_LE(AES_MODE_ECB);

			/* 11th ~ 16th bytes for efuse 48bits */
			if( fread( &sh->v2.resv3[11], 1, 6, sim ) != 6 ) {
				fprintf( stderr, "fread(%d bytes) failed\n", 6 );
				goto free_source;
			}
			fclose(sim);
#endif
		}

		if(isAesImgKey == 1) {
#if AES_SIMULATION_IMG_KEY_INFO_ERROR
			img_key_info_error = 1;
#endif
			if(fill_cipher(&sh->v2.aes_img_key_info, 
						   aes_img_cipher_file, 
						   (char *)sh->v2.aes_img_key,
						   sizeof(sh->v2.aes_img_key),
						   img_key_info_error) != 0) {
				usage();
				goto free_source;
			}
		}

		if(isSpramCipher == 1) {
#if AES_SIMULATION_SPRAM_INFO_ERROR
			spram_info_error = 1;
#endif
			if(fill_cipher(&sh->v2.aes_spram_info, 
						   spram_cipher_file, 
						   buf + th->spram_flash_start_addr + simulation_offset,
						   th->spram_flash_end_addr - th->spram_flash_start_addr,
						   spram_info_error) != 0) {
				usage();
				goto free_source;
			}
		}

		if(isLzmaCipher == 1) {
#if AES_SIMULATION_LZMA_INFO_ERROR
			lzma_info_error = 1;
#endif
			if(fill_cipher(&sh->v2.aes_lzma_info, 
						   lzma_cipher_file, 
						   buf + th->lzma_flash_start_addr + simulation_offset,
						   th->lzma_flash_end_addr - th->lzma_flash_start_addr,
						   lzma_info_error) != 0) {
				usage();
				goto free_source;
			}
		}

		if(isVerifyCipher == 1) {
			if(fill_cipher(&sh->v2.aes_verify_info, 
						   verify_cipher_file, 
						   buf + th->verify_start_addr + simulation_offset,
						   th->verify_end_addr - th->verify_start_addr,
						   0) != 0) {
				usage();
				goto free_source;
			}
		}

		if(isBootramCipher == 1) {
			if(fill_cipher(&sh->v2.aes_bootram_info, 
						   bootram_cipher_file, 
						   buf + th->bootram_flash_start_addr + simulation_offset,
						   th->bootram_flash_end_addr - th->bootram_flash_start_addr,
						   0) != 0) {
				usage();
				goto free_source;
			}
		}

		if(isAesMode) {
			sh->v2.aes_mode = STORE32_LE((AES_MODE_T)AesMode);
		}
		
		sh->v2.crc = STORE32_LE(crc32((unsigned char *)sh, shdrLen - 4));
	} else if(Ver == 1) {
		sh->v1.crc = STORE32_LE(crc32((unsigned char *)sh, shdrLen - 4));
	}

	/* back to beginning of file */
	fseek(in, 0L, SEEK_SET);
#if AES_SIMULATION_OFFSET
	if(input_file_len > 0x80000) {
		input_file_len = 0x80000;
	}
#endif
	fwrite(buf, input_file_len, 1, in);

	retVal = 0;

free_source:
	if(in != NULL) {
		fclose(in);
	}
	if(sig != NULL) {
		fclose(sig);
	}
	if(buf != NULL) {
		free(buf);
	}

	return retVal;
}

