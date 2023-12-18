#ifndef _MRD_STRUCTURE_
#define _MRD_STRUCTURE_

#if 0 
/*-------------------------------------------------------------------
 * MRD structdure reference from Econet platform
 */
#define MRD_OFFSET                  0xff00
#define MRD_SIZE                    0x100
#define MRD_SERIAL_NUM_LEN          14
#define MRD_IMEI_NUM_LEN            18
#define MRD_VENDOR_NAME_LEN         32
#define MRD_PRODUCT_NAME_LEN        32
#define MRD_MAC_ADDR_LEN            6
#define MRD_FEATUREBITS_LEN         30

typedef struct mrd {
    unsigned char SDRAM_CONF_array[4];
    unsigned char VENDOR_NAME[MRD_VENDOR_NAME_LEN];
    unsigned char PRODUCT_NAME[MRD_PRODUCT_NAME_LEN];
    unsigned char FUNC_BYTE[4];
    unsigned char ETHER_ADDR[MRD_MAC_ADDR_LEN];
    unsigned char SNMP_SYSOBJID_array[22];
    unsigned char COUNTRY_array;
    unsigned char ether_gpio;
    unsigned char power_gpio[2];
    unsigned char restore_flag;
    unsigned char unused1[32];
    unsigned char lte_gpio;
    unsigned char internet_gpio;
    unsigned char multi_upgrade_gpio[16];
    unsigned char id_76xx;
    unsigned char onu_type; //byte 0~156
//IMEI number 
#if 1
    unsigned char IMEI[MRD_IMEI_NUM_LEN];
#else
    unsigned char SN[18];
#endif
/* --- ZyXEL related --- start --- */
    unsigned char SerialNumber[MRD_SERIAL_NUM_LEN];
    unsigned char NumMacAddrs;
    unsigned char MrdVer;
    unsigned char EngDebugFlag;
    unsigned char BootingFlag;
    unsigned char MainFeatureBit;
    unsigned char FeatureBits[MRD_FEATUREBITS_LEN];
    unsigned char HTP_Switch;
    unsigned char WpaPskKey[16];
    // bootloader related information
    unsigned char BM_VER_MAJ;
    unsigned char BM_VER_MIN;
    unsigned char BM_VER_ECO;
    unsigned char builddate[4];
    unsigned char buildtime[3]; //byte 0~72
/* --- ZyXEL related --- end --- */
#ifdef BOOTROM_LARGE_SIZE	//For ZyXEL project, we always enable "BOOTROM_LARGE_SIZE"
    // mic.conf will located at the end of block 0.
    // boot_crc32 will located at the last 4 bytes of block 1.
    unsigned char unused2[6]; // let total structure have 256 bytes.
#else
    // if MRD structure locate at the end of boot.bin, need reserve 4 bytes for crc.
    unsigned char unused[2]; // let total structure have 256 bytes
    unsigned char boot_crc32[4]; //filled by trx.c
#endif
} __attribute__((packed)) MRD_DATA, *PMRD_DATA;

#else

/*-------------------------------------------------------------------
 * MRD structdure from MSTC manufacuring
 */

#ifdef ZYXEL_NR7102
#define MRD_SERIAL_NUM_LEN          14
#define MRD_GPON_SERIAL_NUM_LEN     8
#define MRD_VENDOR_NAME_LEN         32
#define MRD_PRODUCT_NAME_LEN        32
#define MRD_MAC_ADDR_LEN            6
#define MRD_FEATUREBITS_LEN         30
#endif


#ifdef QUECTEL_PLATFORM
#define MRD_OFFSET                  0x0
#else
#define MRD_OFFSET                  0xfe00
#endif

#define NUM_OF_CERT_BYTES           0x2000 //Reserve 8192 bytes for certificate 
#define NUM_OF_KEY_BYTES           0x2000 //Reserve 8192 bytes for key

#ifdef ZYXEL_LWM2M_PSK_IN_MRD
#define NUM_OF_PSK_BYTES			0x100
#else
#define NUM_OF_PSK_BYTES			0x0
#endif

#ifdef ZYXEL_RANDOM_ENCRYPTION_SEED_IN_MRD
#define NUM_OF_ENCRYPTION_SEED_BYTES 0x100
#else
#define NUM_OF_ENCRYPTION_SEED_BYTES 0x0
#endif

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
#define NUM_OF_PASSWD               4     //Reserved for user/psk/root/supervisor
#define NUM_OF_PASSWD_BYTES			0x100
#define NUM_OF_PASSWD_TOTAL_BYTES   NUM_OF_PASSWD_BYTES*NUM_OF_PASSWD
#else
#define NUM_OF_PASSWD_BYTES			0x0
#define NUM_OF_PASSWD_TOTAL_BYTES	0x0
#endif

#ifdef ZYXEL_ENCRYPTED_FIRMWARE_UPGRADE
#define NUM_OF_PUBKEY_BYTES			0x1000 //reserve 4K byte for public key
#else
#define NUM_OF_PUBKEY_BYTES			0x0 
#endif

#ifdef ZYXEL_NR7102
#define NR7102_ENTRIES_SIZE         MRD_GPON_SERIAL_NUM_LEN+10+MRD_MAC_ADDR_LEN+3+MRD_VENDOR_NAME_LEN+MRD_PRODUCT_NAME_LEN
#define MRD_SIZE                    (0x200+NR7102_ENTRIES_SIZE+NUM_OF_CERT_BYTES+NUM_OF_KEY_BYTES+NUM_OF_PASSWD_TOTAL_BYTES+NUM_OF_PSK_BYTES + \
										NUM_OF_ENCRYPTION_SEED_BYTES + NUM_OF_PUBKEY_BYTES + NUM_OF_PUBKEY_BYTES)
#else
#define MRD_SIZE                    (0x200+NUM_OF_CERT_BYTES+NUM_OF_KEY_BYTES+NUM_OF_PASSWD_TOTAL_BYTES+NUM_OF_PSK_BYTES + \
										NUM_OF_ENCRYPTION_SEED_BYTES + NUM_OF_PUBKEY_BYTES + NUM_OF_PUBKEY_BYTES)
#endif


#define MRD_DATA_SIZE			512

#define NUM_OF_NAME_BYTES           32
#define NUM_OF_SERIAL_NUMBER_BYTES	14
#define NUM_OF_MAC_BYTES             6
#define NUM_OF_COUNTRY_CODE_BYTES	 2
#define NUM_OF_FEATURE_BYTES		32
#define NUM_OF_MAGIC_BYTES 	         4
#define NUM_OF_BUILDINFO_BYTES		10
#define NUM_OF_HARDWARE_VER_BYTES    4

#if defined(AWS_FOTA_Firmware_Upgrade) || defined(AWS_FOTA_Module_Upgrade)
#define NUM_OF_RESERVED_BYTES		(MRD_DATA_SIZE - (NUM_OF_NAME_BYTES * 4) - NUM_OF_SERIAL_NUMBER_BYTES - \
						NUM_OF_MAC_BYTES - 2 - NUM_OF_COUNTRY_CODE_BYTES - NUM_OF_FEATURE_BYTES - 1 - \
						NUM_OF_BUILDINFO_BYTES - NUM_OF_HARDWARE_VER_BYTES - 1 - 1 - 1 - 1)//- 1 - 1 - 1 - 4 - 3)
#else //AWS_FOTA_Firmware_Upgrade || AWS_FOTA_Module_Upgrade
#define NUM_OF_RESERVED_BYTES		(MRD_DATA_SIZE - (NUM_OF_NAME_BYTES * 4) - NUM_OF_SERIAL_NUMBER_BYTES - \
						NUM_OF_MAC_BYTES - 2 - NUM_OF_COUNTRY_CODE_BYTES - NUM_OF_FEATURE_BYTES - 1 - \
						NUM_OF_BUILDINFO_BYTES - NUM_OF_HARDWARE_VER_BYTES - 1 - 1 - 1)//- 1 - 1 - 1 - 4 - 3)
						
#endif						
						

typedef struct mrd {
    unsigned char VendorName[NUM_OF_NAME_BYTES];
    unsigned char ProductName[NUM_OF_NAME_BYTES];
    unsigned char NameReserved[NUM_OF_NAME_BYTES];
    unsigned char SerialNumber[NUM_OF_SERIAL_NUMBER_BYTES];
    unsigned char BaseMacAddr[NUM_OF_MAC_BYTES];
    unsigned char MacAddrQuantity;
    unsigned char EngDebugFlag;
    unsigned char CountryCode[NUM_OF_COUNTRY_CODE_BYTES];
    unsigned char FeatureBits[NUM_OF_FEATURE_BYTES];
    unsigned char MainFeatureBits;
    unsigned char BuildInfo[NUM_OF_BUILDINFO_BYTES];
    unsigned char ProductExtName[NUM_OF_NAME_BYTES];
    unsigned char HardwareVersion[NUM_OF_HARDWARE_VER_BYTES];
    unsigned char COUNTRY_array; 
    unsigned char COUNTRY_array_reserve;
    unsigned char BootingFlag;	
#if defined(AWS_FOTA_Firmware_Upgrade) || defined(AWS_FOTA_Module_Upgrade)
    unsigned char FOTA_Status;
#endif	
    //unsigned char BM_VER_MAJ;
    //unsigned char BM_VER_MIN;
    //unsigned char BM_VER_ECO;
    //unsigned char builddate[4];
    //unsigned char buildtime[3];	
    unsigned char Reserved[NUM_OF_RESERVED_BYTES];
    //Reserve Certificate and key loactions for each device
#ifdef ZYXEL_NR7102
    unsigned char GponSerialNumber[MRD_GPON_SERIAL_NUM_LEN];
    unsigned char unused3[10];
    unsigned char ETHER_ADDR[MRD_MAC_ADDR_LEN];
    unsigned char HTP_Switch;
    unsigned char MainFeatureBit;
    unsigned char NumMacAddrs;
    unsigned char VENDOR_NAME[MRD_VENDOR_NAME_LEN];
    unsigned char PRODUCT_NAME[MRD_PRODUCT_NAME_LEN];
#endif
	unsigned char Certificate[NUM_OF_CERT_BYTES];
	unsigned char PrivateKey[NUM_OF_KEY_BYTES];
#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
	unsigned char password[NUM_OF_PASSWD][NUM_OF_PASSWD_BYTES];
#endif
#ifdef ZYXEL_LWM2M_PSK_IN_MRD
    unsigned char Lwm2mPsk[NUM_OF_PSK_BYTES];
#endif
#ifdef ZYXEL_RANDOM_ENCRYPTION_SEED_IN_MRD
    unsigned char EncryptionSeed[NUM_OF_ENCRYPTION_SEED_BYTES];
#endif
#ifdef ZYXEL_ENCRYPTED_FIRMWARE_UPGRADE
    unsigned char RSAPublicKey[NUM_OF_PUBKEY_BYTES];
    unsigned char RSAPublicHash[NUM_OF_PUBKEY_BYTES];
#endif
} __attribute__((packed)) MRD_DATA, *PMRD_DATA;

#endif //ZYXEL or MSTC manufacturing MRD structure.

#endif
