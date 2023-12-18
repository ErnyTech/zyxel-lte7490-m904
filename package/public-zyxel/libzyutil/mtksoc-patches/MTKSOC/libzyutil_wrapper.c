#ifdef MTKSOC_PLATFORM

#include <fcntl.h>
#include <byteswap.h>
#include <sys/stat.h>

#include "libzyutil_wrapper.h"
#include "libzyutil_mtksoc_wrapper.h"

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
#include "aes.h"
#endif

/* Static default value for upper layer development usage*/
char BASE_MAC_ADDR_DEFAULT[12]="000102030405";
char BASE_MAC_ADDR_WithColon[17]="00:01:02:03:04:05";
int  MAC_NUM = 2;
char MODEL_ID[4]="9999";
char PRODUCT_NAME[32]="MT7621_EVB";
char FWVERSION[32]="0000";
char SerialNumber_DEFAULT[FACTORY_SN_LEN]="0000000000";
char LOCAL_CERTIFICATE[MAXLEN_CERT+1]={0};
char PRIVATE_KEY[MAXLEN_CERT+1]={0};

//The overall MRD rule is to use 1st: nvram data , 2nd : MRD data, 
//The overall Header rule is to use 1st: partition header, 2nd : Banner Data

#if 1
int mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[255];
	int i,ret;

	if ((fp = fopen("/proc/mtd", "r"))) 
	{
		while (fgets(dev, sizeof(dev), fp)) 
		{
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) 
			{
				snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
				fclose(fp);
				return open(dev, flags);
			}
		}
		fclose(fp);
	}
	printf (" Cound not open proc/mtd or mtd device %s /n", mtd);
	return -1;
}

int read_mtd_info(const char *mtd, struct trx_header *p_trx_header)
{
	int mtd_fd = -1;
	int trx_header_len = sizeof(struct trx_header);

	if(-1 == (mtd_fd = mtd_open(mtd,O_RDONLY)))
	{
		printf("ERROR!Open mtd %s fail",mtd);
		return -1;
	}

	memset(p_trx_header,0,trx_header_len);

	if(trx_header_len != read(mtd_fd,p_trx_header,trx_header_len))
	{
		printf("ERROR!Read mtd %s fail",mtd);
		close(mtd_fd);
		return -1;
	}

	close(mtd_fd);
	return 0;
}

#endif

#if 0
static void dumpMRDinfo(PMRD_DATA mrd)
{
    int i;
    printf("\nMRD Dump(%d):\n", sizeof(MRD_DATA));
    printf("SDRAM_CONF_array[4]: 0x%02X 0x%02X 0x%02X 0x%02X\n",
           mrd->SDRAM_CONF_array[0], mrd->SDRAM_CONF_array[1], mrd->SDRAM_CONF_array[2], mrd->SDRAM_CONF_array[3]);
    printf("VENDOR_NAME[32]    : %s\n", mrd->VendorName);
    printf("PRODUCT_NAME[32]   : %s\n", mrd->ProductName);
    printf("FUNC_BYTE[4]       : 0x%02X 0x%02X 0x%02X 0x%02X\n",
           mrd->FUNC_BYTE[0], mrd->FUNC_BYTE[1], mrd->FUNC_BYTE[2], mrd->FUNC_BYTE[3]);
    printf("BaseMacAddr[6]      : %02X:%02X:%02X:%02X:%02X:%02X\n",
           mrd->BaseMacAddr[0], mrd->BaseMacAddr[1], mrd->BaseMacAddr[2], mrd->BaseMacAddr[3],
           mrd->BaseMacAddr[4], mrd->BaseMacAddr[5]);
    printf("CountryCode      : 0x%02X\n", mrd->CountryCode[1]);
    printf("ether_gpio         : 0x%02X\n", mrd->ether_gpio);
    printf("power_gpio[2]      : 0x%02X 0x%02X\n", mrd->power_gpio[0], mrd->power_gpio[1]);
    printf("restore_flag       : 0x%02X\n", mrd->restore_flag);
    printf("lte_gpio           : 0x%02X\n", mrd->lte_gpio);
    printf("internet_gpio      : 0x%02X\n", mrd->internet_gpio);
    printf("SN[18]             : %s\n", mrd->SN);
    printf("SerialNumber[14]   : %s\n", mrd->SerialNumber);
    printf("MacAddrQuantity        : %d\n", mrd->MacAddrQuantity);
    printf("MrdVer             : 0x%02X\n", mrd->MrdVer);
    printf("EngDebugFlag       : 0x%02X\n", mrd->EngDebugFlag);
    printf("MainFeatureBits     : 0x%02X\n", mrd->MainFeatureBits);
    printf("FeatureBits[30]    :");
    for (i=0; i<30; i++) {
        if (i%15==0) puts("\n     ");
        printf("0x%02X ", mrd->FeatureBits[i]);
    }
    printf("\nHTP_Switch         : 0x%02X\n", mrd->HTP_Switch);
    printf("WpaPskKey[16]      :");
    for (i=0; i<16; i++) {
        if (i%16==0) puts("\n     ");
        printf("0x%02X ", mrd->WpaPskKey[i]);
    }
    printf("\nBM_VER[3]          : %d.%d.%d\n", mrd->BM_VER_MAJ, mrd->BM_VER_MIN, mrd->BM_VER_ECO);
    printf("builddate[4]       : %02X/%02X/%02X%02X\n", mrd->builddate[0], mrd->builddate[1],
           mrd->builddate[2], mrd->builddate[3]);
    printf("buildtime[3]       : %02X:%02X:%02X\n", mrd->buildtime[0], mrd->buildtime[1],
           mrd->buildtime[2]);
}
#else
#define dumpMRDinfo(x)
#endif

/*-------------------------------------------------
 * eraseRomdPartition() :
 *-------------------------------------------------
int eraseRomdPartition(void)
{
	int ret = 0;
	char mtddev[16];

	if( zyUtilMtdDevGetByName(ROMD_PARTITON_NAME, mtddev) == ZCFG_SUCCESS){
		if (zyUtilMtdErase(mtddev) != ZCFG_SUCCESS)
			return ZCFG_INTERNAL_ERROR;
	}
	else
		return ZCFG_INTERNAL_ERROR;
	return ZCFG_SUCCESS;
}*/

zcfgRet_t zyUtilGetMrdInfo(PMRD_DATA mrd)
{
	char sysCmd[128] = {0};
	FILE *fptr = NULL;

	// clean the MRD info
	memset(mrd, 0x0, sizeof(MRD_DATA));
#if 0
	// Get MRD info from bootloader partition with offset 0xff00
	sprintf(sysCmd,FLASH_READ_CMD,MRDINFO_FILE,(long unsigned int)MRDINFO_SIZE,(long unsigned int)MRDINFO_OFFSET,MRDINFO_PARTITON_NAME);
	system(sysCmd);
	sprintf(sysCmd,"chmod 666 %s",MRDINFO_FILE);
	system(sysCmd);
	//printf("%s:sysCmd = %s\n", __FUNCTION__, sysCmd);

    /*Read MRD info to buffer*/
    if((fptr = fopen(MRDINFO_FILE, "rb")) == NULL) {
        printf("Can't fopen %s\n", MRDINFO_FILE);
		unlink(MRDINFO_FILE);
        return ZCFG_INTERNAL_ERROR;
    }
#else
	/* check MRDINFO_FILE exists or not. */
	if ((fptr = fopen(MRDINFO_FILE, "rb")) == NULL)
    {
		// Get MRD info from bootloader partition with offset 0xfe00(MSTC)
		sprintf(sysCmd,FLASH_READ_CMD,MRDINFO_FILE,(long unsigned int)MRDINFO_SIZE,(long unsigned int)MRDINFO_OFFSET,MRDINFO_PARTITON_NAME);
		system(sysCmd);
		chmod(MRDINFO_FILE, 420); // 420(decimal) = 644(octal), 438(decimal) = 666(octal) 
	}
	/*Read MRD info to buffer*/
#endif
	if ((fptr = fopen(MRDINFO_FILE, "rb")) != NULL) {
		fread(mrd, 1, (uint32_t)MRDINFO_SIZE, fptr);
		dumpMRDinfo(mrd);
		fclose(fptr);
	// unlink(MRDINFO_FILE);
		return ZCFG_SUCCESS;
		//	}
	}
	else 
	return ZCFG_INTERNAL_ERROR;
}

zcfgRet_t zyUtilSetMrdInfo(PMRD_DATA mrd)
{
	char sysCmd[128] = {0};
	FILE *fptr = NULL;

    /*Write MRD info into file*/
    if((fptr = fopen(MRDINFO_FILE, "wb+")) == NULL) {
        printf("Can't fopen %s\n", MRDINFO_FILE);
		unlink(MRDINFO_FILE);
        return ZCFG_INTERNAL_ERROR;
    }
	fwrite(mrd, 1, (uint32_t)MRDINFO_SIZE, fptr);
	fclose(fptr);
	dumpMRDinfo(mrd);

	// Set MRD info from file into bootloader partition at offset 0xfe00(MSTC)
	sprintf(sysCmd,FLASH_WRITE_CMD,MRDINFO_FILE,(long unsigned int)MRDINFO_SIZE,(long unsigned int)MRDINFO_OFFSET,MRDINFO_PARTITON_NAME);
	system(sysCmd);

	unlink(MRDINFO_FILE);
	return ZCFG_SUCCESS;
}


int32_t nvram_file_get (char * keyname, char * returnstring, char * cachefile )
{
    FILE * fp = NULL;
	char name[MAXLEN], value[MAXLEN];
	char *s, buff[256];
	int32_t len =0;
    fp = fopen(cachefile, "r");
    if (!fp)
    {
        printf("failed to load %s, try init and reload again\n", cachefile);
		system(NVRAM_SHOW_CMD);
		system(NVRAM_FACTORY_SHOW_CMD);
		fp = fopen(cachefile, "r");
		if (!fp) {
        printf("retried initialize and failed to load %s, \n", cachefile);
		return -1;
		}
	}
	while ((s = fgets (buff, sizeof buff, fp)) != NULL)
	{
	  /* Skip blank lines and comments and non ascii value string*/
	  if (buff[0] == '\n' || buff[0] == '#' || buff[0] < 32 || buff[0] > 128)
		continue;
	
	  /* Parse name/value pair from line */
		
		s = strtok (buff, "=");
		if (s==NULL)
		continue;
		else
		strncpy (name, s, MAXLEN);

		s = strtok (NULL, "=");
		if (s==NULL)
		continue;
		else
		strncpy (value, s, MAXLEN);
		//trim (value);
		
		if (strcmp(name, keyname)==0) {
			len = (strlen (value) +1 < MAXLEN)? strlen (value) + 1 : MAXLEN;
			value [ strcspn (value, "\n") ] =0;
			strncpy (returnstring, value, len);
			//printf ( "name is: %s , value is : %s , keyname is: %s , returnstring is %s , size of copy is %d", name , value, keyname ,returnstring, len);
			fclose (fp);
			return 0;
			}
	}
fclose (fp);
return -1;
}


int32_t banner_file_get (char * productname, char * firmwareversion)
{
    FILE * fp = NULL;
	char *s, buff[256];
	
    fp = fopen(BANNER_FILE, "r");
    if (!fp)
    {
        printf("failed to load %s \n", BANNER_FILE);
		return -1;
	}
	while ((s = fgets (buff, sizeof buff, fp)) != NULL)
	{
		s = strstr (buff, "Product:");
		if (s != NULL) {
			strncpy (productname, s+9, 32);
			productname [ strcspn (productname, "\n") ] =0;
			while ((s = fgets (buff, sizeof buff, fp)) != NULL) {
				s = strstr (buff, "Version:");
				if (s != NULL) {
					strncpy (firmwareversion, s+9, 32);
					firmwareversion [ strcspn (firmwareversion, "\n") ] =0;
					fclose (fp);
					
					return 0;
				}
			}
		}
	}
fclose (fp);
return -1;
}


#ifdef SUPPORT_LANVLAN
zcfgRet_t zyUtilISetIfState(char *ifName, bool up)
{
	/*Todo*/
	return ZCFG_SUCCESS;
}
#endif

zcfgRet_t zyUtilIGetBaseMAC(char *mac)
{

#if 0 //debug functional test - trigger from here
	
#endif

	struct mrd mrd_buff;
	int32_t ret ;
	ret=nvram_file_get (FACTORY_EXTRA_ENV_BASEMAC, BASE_MAC_ADDR_DEFAULT, FACTORY_STARTUP_FILE);
	printf("%s: Function executed\n",__func__);
	if (ret) {
		if (zyUtilGetMrdInfo(&mrd_buff)) 
			return ZCFG_INTERNAL_ERROR;
		else {
			sprintf(mac ,"%02X%02X%02X%02X%02X%02X",
			mrd_buff.BaseMacAddr[0],
			mrd_buff.BaseMacAddr[1],
        	mrd_buff.BaseMacAddr[2],
        	mrd_buff.BaseMacAddr[3],
        	mrd_buff.BaseMacAddr[4],
        	mrd_buff.BaseMacAddr[5]
			);
			//printf("mac:%s\n",mac);
			return ZCFG_SUCCESS;
		}
	}
	else {
	sprintf(mac ,"%s",	BASE_MAC_ADDR_DEFAULT );
	//printf("mac:%s\n",mac);
	return ZCFG_SUCCESS;
	}
}

zcfgRet_t zyUtilIGetBaseMACwithColon(char *mac)
{
	struct mrd mrd_buff;
	int32_t ret ;
	ret=nvram_file_get (FACTORY_EXTRA_ENV_BASEMAC, BASE_MAC_ADDR_DEFAULT, FACTORY_STARTUP_FILE);
	printf("%s: Function executed\n",__func__);
	if (ret) {
			if (zyUtilGetMrdInfo(&mrd_buff)) 
				return ZCFG_INTERNAL_ERROR;
			else {
				sprintf (mac ,"%02X:%02X:%02X:%02X:%02X:%02X",
				mrd_buff.BaseMacAddr[0],
				mrd_buff.BaseMacAddr[1],
				mrd_buff.BaseMacAddr[2],
				mrd_buff.BaseMacAddr[3],
				mrd_buff.BaseMacAddr[4],
				mrd_buff.BaseMacAddr[5]
			);
			//printf("mac:%s\n",mac);
			return ZCFG_SUCCESS;
				}
		}
	else {
		sprintf(BASE_MAC_ADDR_WithColon ,"%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
		BASE_MAC_ADDR_DEFAULT[0],BASE_MAC_ADDR_DEFAULT[1],
		BASE_MAC_ADDR_DEFAULT[2],BASE_MAC_ADDR_DEFAULT[3],
		BASE_MAC_ADDR_DEFAULT[4],BASE_MAC_ADDR_DEFAULT[5],
		BASE_MAC_ADDR_DEFAULT[6],BASE_MAC_ADDR_DEFAULT[7],
		BASE_MAC_ADDR_DEFAULT[8],BASE_MAC_ADDR_DEFAULT[9],
		BASE_MAC_ADDR_DEFAULT[10],BASE_MAC_ADDR_DEFAULT[11]);

		sprintf(mac ,"%s",	BASE_MAC_ADDR_WithColon );
		//printf("mac:%s\n",mac);
		return ZCFG_SUCCESS;
	}

}

zcfgRet_t zyUtilIGetNumberOfMAC(int *num)
{
	struct mrd mrd_buff;
	int32_t ret ;
	char TEMPMACNUMBER[2]="2";
	ret=nvram_file_get (FACTORY_EXTRA_ENV_MACNUMBER, TEMPMACNUMBER, FACTORY_STARTUP_FILE);
	printf("%s: Function executed\n",__func__);
	if (ret) {
			if(zyUtilGetMrdInfo(&mrd_buff)) /* Get MRD info with binary mode */
			return ZCFG_INTERNAL_ERROR;
			else {
				if(num) {
				/* MSTC store their MRD value in u_int8_t type, so we don't need to convert from char to int. */
					if (mrd_buff.MacAddrQuantity < 2) 
					{
						*num = 2;
					}
					else
					{
						*num = mrd_buff.MacAddrQuantity;
					}
					return ZCFG_SUCCESS;
				}
				else
				return ZCFG_INTERNAL_ERROR;
			}
	}
	else {
		MAC_NUM = strtol (TEMPMACNUMBER, NULL, 10);
			if(MAC_NUM) 
				*num =  MAC_NUM;
			//printf("MacAddrQuantity:%d\n",num);
			return ZCFG_SUCCESS;

	}
}

/*!
 *  Get Nth MAC base on base MAC and GDM MASK.
 *  Let MAC`s increment can in the range og GDM MASK.
 *  For example: when GDM MASK is 0xf8, MAC can be x2,x3,x4,x5,x6,x7,x0,x1
 *
 *  @param[inout]  pucaMacAddr   input Base MAC and out N`th MAC, should be 6 bytes.
 *  @param[in]     n             Get N`th MAC.
 *  @param[in]     _macQuantity  Mac quantity.
 *
 *  @note  The MAC length should be 6 bytes.
 */
static void getNthMacAddr_special( unsigned char *pucaMacAddr, unsigned long n, unsigned char _gdm_mask)
{
	unsigned char mac_mask = (~_gdm_mask)&0xff;
	unsigned int macsequence = pucaMacAddr[5]&mac_mask;
    pucaMacAddr[5] = (pucaMacAddr[5]&_gdm_mask) | ((macsequence + n) & mac_mask);
}

/*!
 *  Get Nth MAC.
 *
 *  @param[out]    pucaMacAddr   out N`th MAC, should be 6 bytes.
 *  @param[in]     n             Get N`th MAC.
 *
 *  @note  The MAC length should be 6 bytes.
 */
zcfgRet_t zyUtilIGetNthMAC(char *mac, int nth) /* For reuse ECONET WIFI usage*/
{
	unsigned char targetMac[6]={0};
	int _macQuantity=0;
	struct mrd mrd_buff;
	int i,j=0;
	
#if defined(ZYXEL_LAN_WAN_WLAN_SHARE_THE_SAME_MAC)
	unsigned char _gdm_mask = 0xf8;
#else
	unsigned char _gdm_mask = 0xf0;
#endif

	printf("%s: Function executed\n",__func__);

	if(0 != zyUtilGetMrdInfo(&mrd_buff))
		return ZCFG_INTERNAL_ERROR;
	memcpy(targetMac, mrd_buff.BaseMacAddr, NUM_OF_MAC_BYTES);


	if (ZCFG_SUCCESS !=nvram_file_get (FACTORY_EXTRA_ENV_BASEMAC, BASE_MAC_ADDR_DEFAULT, FACTORY_STARTUP_FILE)) {

			if(zyUtilGetMrdInfo(&mrd_buff)) 
				return ZCFG_INTERNAL_ERROR;
			else 
				memcpy(targetMac, mrd_buff.BaseMacAddr, NUM_OF_MAC_BYTES);
	}
	else {
	//hex string to arrary
	for (i=0, j=0; j<6; i+=2, j++)
	targetMac[j] = (BASE_MAC_ADDR_DEFAULT[i] % 32 + 9) % 25 * 16 + (BASE_MAC_ADDR_DEFAULT[i+1] % 32 + 9) % 25;
	}

	if( ZCFG_SUCCESS != zyUtilIGetNumberOfMAC(&_macQuantity) )
		return ZCFG_INTERNAL_ERROR;
	//Make sure the MAC we used are in the same MASK.
#if defined(ZYXEL_LAN_WAN_WLAN_SHARE_THE_SAME_MAC)
	if( _macQuantity != 8 ) {
		_macQuantity = 8;
	}
#else
	if( _macQuantity != 16 ) {
		_macQuantity = 16;
	}
#endif

	if( nth >= _macQuantity )
		return ZCFG_INTERNAL_ERROR;

	getNthMacAddr_special(targetMac, nth, _gdm_mask);
	//printf("%2d`s MAC Address with GDM MASK 0x%02X: %02X %02X %02X %02X %02X %02X\n", (unsigned int)nth, _gdm_mask, targetMac[0],targetMac[1],targetMac[2],targetMac[3],targetMac[4],targetMac[5]);
	memcpy(mac, targetMac, NUM_OF_MAC_BYTES);

	return ZCFG_SUCCESS;
}




zcfgRet_t zyUtilIGetModelID(char *model_id)
{
	struct mrd mrd_buff;
	int32_t ret ;
	int i=0;
	ret=nvram_file_get (UBOOT_EXTRA_ENV_MODEL_ID, MODEL_ID, UBOOT_STARTUP_FILE);
	printf("%s: Function executed\n",__func__);
	if (ret) {
			if(zyUtilGetMrdInfo(&mrd_buff)) 
			return ZCFG_INTERNAL_ERROR;
			else {
				memcpy(model_id, mrd_buff.FeatureBits, 4);
				//printf("ModelID:%x %x %x %x\n",model_id[0] model_id[1] model_id[2] model_id[3]);
				return ZCFG_SUCCESS;
				}
	}
	else {
		//hex string to arrary
		for (i=0; i<4; i++)
		model_id[i] = (MODEL_ID[i] % 32 + 9) % 25;		
		//printf("ModelID:%x %x %x %x\n",model_id[0], model_id[1], model_id[2], model_id[3]);
		return ZCFG_SUCCESS;

	}
}

#ifdef ZYXEL_DEFAULT_LOCAL_CERTIFICATE
zcfgRet_t zHttpParseCertPemFile222(FILE *fp, char *certificate, char *privateKey){
	int ret = 0;
	char linebuf[256];
	int certFound = 0;
	int keyFound = 0;
	bool startPub = false;
	bool startPriv = false;
	char *s;
	while(fgets(linebuf, sizeof(linebuf), fp)){
		//printf("linebuf=%s\n", linebuf);
		if(strstr(linebuf,"-----BEGIN CERTIFICATE-----")){
			startPub=true;
			certFound = 1;
			/* Parse name/value pair from line */
			s = strtok (linebuf, "=");
			if (s==NULL)
			{
				;
			} else {
				s = strtok (NULL, "=");
			}
			
			if(startPub && certificate)
				strcat(certificate, s);
			continue;
		}
		if(strstr(linebuf,"-----BEGIN RSA PRIVATE KEY-----") || strstr(linebuf,"-----BEGIN PRIVATE KEY-----")){
			startPriv=true;
			keyFound = 1;
			/* Parse name/value pair from line */
			s = strtok (linebuf, "=");
			if (s==NULL)
			{
				;
			} else {
				s = strtok (NULL, "=");
			}
			
			if(startPriv && privateKey)
				strcat(privateKey, s);
			continue;
		}

		if(startPub && certificate)
			strcat(certificate, linebuf);
		if(startPriv && privateKey)
			strcat(privateKey, linebuf);

		if(strstr(linebuf,"-----END CERTIFICATE-----")){
			startPub = false;
		}
		if(strstr(linebuf,"-----END RSA PRIVATE KEY-----") || strstr(linebuf,"-----BEGIN PRIVATE KEY-----")){
			startPriv = false;
		}
	}
	
	if((0==certFound) && (0==keyFound))
		ret = (-1);
	
	return ret;//0 => successfully got from "nvram cert" section
}

int32_t nvram_file_get_cert (char * keyname, char * returnstring, char * cachefile )
{
	int ret = 0;
    FILE * fp = NULL;
	char name[MAXLEN], value[MAXLEN_CERT+1];
	char *s, buff[256];
	int32_t len =0;
	//printf("cachefile=%s\n", cachefile);
	fp = fopen(cachefile, "r");
	if (!fp)
	{
		system(NVRAM_SHOW_CERT_CMD);
		fp = fopen(cachefile, "r");
		if (!fp) {
			printf("fopen NVRAM_SHOW_CERT_CMD fail, return -1\n");
			ret = (-1);
			return ret;
		}
	}
	snprintf(name, sizeof(name), "%s", keyname);
	if(strcmp(keyname, UBOOT_EXTRA_ENV_LOCAL_CERTIFICATE) == 0){
		memset(value, 0, sizeof(value));
		ret = zHttpParseCertPemFile222(fp, value, NULL);
	}
	if(strcmp(keyname, UBOOT_EXTRA_ENV_PRIVATE_KEY) == 0){
		memset(value, 0, sizeof(value));
		ret = zHttpParseCertPemFile222(fp, NULL, value);
	}
	
	if(returnstring)
		strcpy (returnstring, value);
	
	if(fp){
		fclose (fp);
		fp = NULL;
	}
	return ret;//if ret is 0 => got value from nvram successfully; use "nvram show cert" to check the results
	
}

zcfgRet_t zyUtilIGetMrdCertificate(char *pdname)		  
{
	struct mrd mrd_buff;
	int32_t ret ;

	ret = nvram_file_get_cert (UBOOT_EXTRA_ENV_LOCAL_CERTIFICATE, LOCAL_CERTIFICATE, CERT_STARTUP_FILE);

	if (ret) {
		zyUtilGetMrdInfo(&mrd_buff);
		snprintf(pdname, MAXLEN_CERT, "%s", mrd_buff.Certificate);
		ret = 0;
	}
	else{
		sprintf(pdname ,"%s",	LOCAL_CERTIFICATE );
	}

	if (ret == 0)
		return ZCFG_SUCCESS;
	else
		return ZCFG_INTERNAL_ERROR;

}

zcfgRet_t zyUtilIGetMrdPrivateKey(char *pdname)
{
	struct mrd mrd_buff;
	int32_t ret ;

	ret = nvram_file_get_cert (UBOOT_EXTRA_ENV_PRIVATE_KEY, PRIVATE_KEY, CERT_STARTUP_FILE);
	
	if (ret) {
		zyUtilGetMrdInfo(&mrd_buff);
		snprintf(pdname, MAXLEN_CERT, "%s", mrd_buff.PrivateKey);
		ret = 0;
	}
	else{
		sprintf(pdname ,"%s",	PRIVATE_KEY );
	}

	if (ret == 0)
		return ZCFG_SUCCESS;
	else
		return ZCFG_INTERNAL_ERROR;

}
#endif //#ifdef ZYXEL_DEFAULT_LOCAL_CERTIFICATE


zcfgRet_t zyUtilIGetProductName(char *pdname)
{

	struct mrd mrd_buff;
	int32_t ret ;

	ret=nvram_file_get (UBOOT_EXTRA_ENV_PRODUCT_NAME, PRODUCT_NAME, UBOOT_STARTUP_FILE);
	if (ret) {
			if(zyUtilGetMrdInfo(&mrd_buff)) {
			ret=banner_file_get (PRODUCT_NAME, FWVERSION);	//use local banner info as backup when nvram/MRD failed
			sprintf(pdname ,"%s",	PRODUCT_NAME );
			}
			else if (mrd_buff.ProductName[0] == '\n' || mrd_buff.ProductName[0] == '#' || mrd_buff.ProductName[0] < 32 || mrd_buff.ProductName[0] > 128) {
			ret=banner_file_get (PRODUCT_NAME, FWVERSION);	//use local banner info as backup when nvram/MRD failed
			sprintf(pdname ,"%s",	PRODUCT_NAME );
			}
			else {
				strcpy(pdname, mrd_buff.ProductName);
				ret=0;
			}
	}
	else
	sprintf(pdname ,"%s",	PRODUCT_NAME );
	//strcpy(pdname, PRODUCT_NAME_HERE);

	if (ret == 0)
	return ZCFG_SUCCESS;
	else
	return ZCFG_INTERNAL_ERROR;

}

#ifdef ELISA_FW_NAMING_CUSTOMIZATION
void removeStr(char *str, char *sub) {
	char *p = NULL;
	if ((p = strstr(str, sub)) != NULL)
		*p = '\0';
}
void omitChar(char *str, char omit) {
	char *src, *dst;
	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != omit) dst++;
	}
	*dst = '\0';
}
#endif

#ifdef ZYXEL_FIRMWARE_UPGRADE_DUALBANK_MODE
zcfgRet_t zyUtilIGetFirmwareVersionByIndex(char *fwversion, int index)
#else
zcfgRet_t zyUtilIGetFirmwareVersion(char *fwversion)
#endif
{
	int32_t ret=0;
	struct trx_header *trxhdr1;
	unsigned char *buf;
#ifdef ELISA_FW_NAMING_CUSTOMIZATION
#define ELISA_NO "Elisa1"
#define SUB_MODEL "-M403"
	char produnctName[32] = {0};
#endif
#ifdef ZYXEL_FIRMWARE_UPGRADE_DUALBANK_MODE
	int bootingFlag = 0;
#endif
	
        printf("%s: Function executed\n",__func__);

	int trx_header_len = sizeof(struct trx_header);
	if (!(buf = malloc(trx_header_len))) {
		printf("malloc failed\n");
		return ZCFG_INTERNAL_ERROR;
		}

	trxhdr1= (struct trx_header *) buf;

#ifdef ZYXEL_FIRMWARE_UPGRADE_DUALBANK_MODE
	if(index == -1){
		//Get the currently running firmware version
		if(zyUtilIGetBootingFlag(&bootingFlag) != ZCFG_SUCCESS){
			printf("%s:%d zyUtilIGetBootingFlag() failed\n", __func__, __LINE__);
			return ZCFG_INTERNAL_ERROR;
		}

		if(bootingFlag == 0){
			ret=read_mtd_info(DUAL_IMAGE1_PARTITON_NAME, trxhdr1);
		}
		else if(bootingFlag == 1){
			ret=read_mtd_info(DUAL_IMAGE2_PARTITON_NAME, trxhdr1);
		}
		else{
			//return the first firmware version by default
			ret=read_mtd_info(DUAL_IMAGE1_PARTITON_NAME, trxhdr1);
		}
		if (ret) {
			ret=read_mtd_info(SINGLE_IMAGE1_PARTITON_NAME, trxhdr1);
		}

	}
	else if(index == 0){
		ret=read_mtd_info(DUAL_IMAGE1_PARTITON_NAME, trxhdr1);
	}
	else if(index == 1){
		ret=read_mtd_info(DUAL_IMAGE2_PARTITON_NAME, trxhdr1);
	}
	else{
		//return the first firmware version by default
		ret=read_mtd_info(DUAL_IMAGE1_PARTITON_NAME, trxhdr1);
	}
	if (ret) {
		ret=read_mtd_info(SINGLE_IMAGE1_PARTITON_NAME, trxhdr1);
	}
#else
	ret=read_mtd_info(DUAL_IMAGE1_PARTITON_NAME, trxhdr1);
	if (ret) 
	ret=read_mtd_info(SINGLE_IMAGE1_PARTITON_NAME, trxhdr1);
#endif

	//printf ("trxinfo magic is (0x%08X) \n" , bswap_32(trxhdr1->magic));
	//printf("swVersionExt  is %s\n", trxhdr1->swVersionExt);
	if ((ret == 0) && (bswap_32(trxhdr1->magic) == TRX_MAGIC || bswap_32(trxhdr1->magic) == TRX_MAGIC1 || bswap_32(trxhdr1->magic) == TRX_MAGIC2)) {
#ifdef ELISA_FW_NAMING_CUSTOMIZATION
		zyUtilIGetProductName(produnctName);
		removeStr(produnctName, SUB_MODEL);
		omitChar(trxhdr1->swVersionExt, '.');
		omitChar(trxhdr1->swVersionExt, '(');
		omitChar(trxhdr1->swVersionExt, ')');
		sprintf(fwversion, "%s_%s_%s", produnctName, ELISA_NO, trxhdr1->swVersionExt);
#else
		sprintf(fwversion ,"%s", trxhdr1->swVersionExt );
#endif
	}
	else {
		ret=banner_file_get (PRODUCT_NAME, FWVERSION); //use version string from banner when image header failed
		sprintf(fwversion,"%s", FWVERSION );
	}
	//printf("FirmwareVersion:%s\n",fwversion);

#ifdef ZYXEL_RMA_FW_SUPPORT
	/* PeterSu: force modify the first four chars to be "RMA_" */
	fwversion[0] = 'R';
	fwversion[1] = 'M';
	fwversion[2] = 'A';
	fwversion[3] = '_';
#endif

	free (buf);
	if (ret == 0)
	return ZCFG_SUCCESS;
	else
	return ZCFG_INTERNAL_ERROR;

}

zcfgRet_t zyUtilIGetSerialNumber(char *serianumber)
{
	struct mrd mrd_buff;
	int32_t ret ;
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
	//printf("%s: Function executed\n",__func__);
#else
	printf("%s: Function executed\n",__func__);
#endif	
	ret=nvram_file_get (FACTORY_EXTRA_ENV_SN, SerialNumber_DEFAULT, FACTORY_STARTUP_FILE);
	if (ret) {
		if (zyUtilGetMrdInfo(&mrd_buff)) {
			return ZCFG_INTERNAL_ERROR;
		}
		else if (mrd_buff.SerialNumber[0] == '\n' || mrd_buff.SerialNumber[0] == '#' || mrd_buff.SerialNumber[0] < 32 || mrd_buff.SerialNumber[0] > 128) 
			return ZCFG_INTERNAL_ERROR; //no valid serial number
		else {
			 //strcpy(serianumber, mrd_buff.SerialNumber);
			strncpy(serianumber, mrd_buff.SerialNumber,sizeof(mrd_buff.SerialNumber));
			//printf("SerialNumber:%s\n",serianumber);
			return ZCFG_SUCCESS;
		}
	}
	else {
	sprintf(serianumber ,"%s",	SerialNumber_DEFAULT );
	//printf("SerialNumber:%s\n",serianumber);
	return ZCFG_SUCCESS;
	}
}

zcfgRet_t zyUtilIGetBootingFlag(int *boot_flag)
{
        struct mrd mrd_buff;
        int32_t ret ;
	int i=0;
	char BOOTINGFLAG[2]="2";
#ifdef ZYXEL_NR7102
	char sysCmd[128] = {0};
	FILE *fptr = NULL;
	char bootflag[2];

	sprintf(sysCmd,FLASH_READ_CMD,BOOT_FLAG_FILE,(long unsigned int)BOOT_FLAG_SIZE,(long unsigned int)BOOT_FLAG_OFFSET,RESERVEAREA_NAME);
	system(sysCmd);
	//printf("%s:sysCmd = %s\n", __FUNCTION__, sysCmd);

	/*Read boot flag to buffer*/
	if((fptr = fopen(BOOT_FLAG_FILE, "rb")) == NULL) {
	    unlink(BOOT_FLAG_FILE);
	    printf("Can't fopen %s\n", BOOT_FLAG_FILE);
	    return ZCFG_INTERNAL_ERROR;
	}
	memset(bootflag, 0x0, 2);
	fread(bootflag, 1, (uint32_t)BOOT_FLAG_SIZE, fptr);
	*boot_flag = bootflag[0] - '0';
	if(*boot_flag != 1)
	    *boot_flag = 0;
	//printf("%s : Current boot flag is %s(%d)\n", __FUNCTION__, bootflag, *boot_flag);
	fclose(fptr);
	unlink(BOOT_FLAG_FILE);

	return ZCFG_SUCCESS;
#else
	system (NVRAM_INIT_CMD);
	ret=nvram_file_get (UBOOT_EXTRA_ENV_BOOTINGFLAG, BOOTINGFLAG, UBOOT_STARTUP_FILE);
	//printf("%s: Function executed\n",__func__);
	if (ret == 0) {
	  if(strcmp(BOOTINGFLAG,"1") == 0)
	    *boot_flag = 1;
	  else
	    *boot_flag = 0;

		return ZCFG_SUCCESS;
	}
	else {
	  return ZCFG_INTERNAL_ERROR;
	}
#endif
}

zcfgRet_t zyUtilISetBootingFlag(int boot_flag)
{
	unsigned char flagstring[1] = "0";
	unsigned char sysCmd[128] = {0};
#ifdef ZYXEL_NR7102
	if(boot_flag !=0 && boot_flag != 1) return ZCFG_INTERNAL_ERROR;
	sprintf(sysCmd, "echo %d > %s", boot_flag, BOOT_FLAG_FILE);
	system(sysCmd);
	//printf("%s:sysCmd = %s\n", __FUNCTION__, sysCmd);
	sprintf(sysCmd,FLASH_WRITE_CMD,BOOT_FLAG_FILE,(long unsigned int)BOOT_FLAG_SIZE,(long unsigned int)BOOT_FLAG_OFFSET,RESERVEAREA_NAME);
	system(sysCmd);
	//printf("%s:sysCmd = %s\n", __FUNCTION__, sysCmd);
	unlink(BOOT_FLAG_FILE);

	return ZCFG_SUCCESS;
#else
	sprintf(flagstring,"%d",boot_flag);
	sprintf(sysCmd,NVRAM_SETRO_CMD,UBOOT_EXTRA_ENV_BOOTINGFLAG,flagstring);
	system (sysCmd);
	system(NVRAM_COMMIT_CMD);

	return ZCFG_SUCCESS;
#endif
}

#if 0 //not used, boot flag by nvram setting.
zcfgRet_t zyUtilGetBootImageHeader(void *booting_header)
{
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilGetDualImageInfo(int bootFlag, char *fwversion)
{
	return ZCFG_SUCCESS;
}
#endif

int zyUtilGetGPIO(int gpioNum)
{
	/*This function is only used to check VMG3926-B10A and VMG8924-B10B, so Cross Platform is not need*/
	return 0;
}

/*!
 *  get size of write image partition
 *
 *  @return     size of write image partition (byte)
 *
 *  @note  If get fail, popen fail, or fgets fail, return 0.
 */
int ras_size()
{
        int size=0;
        FILE *fp = NULL;
        char buffer[64];

        fp = popen("cat /proc/mtd  | grep Kernel | awk '{print $2}'", "r");

        if (fp == NULL)
        {
                zyutil_dbg_printf("ERROR : popen fail\n");
                return 0;
        }

        if (fgets(buffer, 64, fp) == NULL)
        {
                zyutil_dbg_printf("ERROR : fgets fail\n");
                pclose(fp);
                return 0;
        }

        sscanf(buffer, "%X", &size);
        pclose(fp);

        return size;

}

/*!
 *  check upload file (FW) magic number
 *
 *  @param[in]  uploadfilename  the name of upload file
 *
 *  @return     0 - non-match
 *              1 - match
 *
 *  @note  If uploadfilename is NULL, popen fail, or fgets fail, return 0.
 */
int magic_number_check(char *uploadfilename)
{
        int magic_number=-1;
        FILE *fp = NULL;
        char buffer[64];
        char cmd[256];

        if (uploadfilename == NULL)
        {
            zyutil_dbg_printf("ERROR : uploadfilename == NULL\n");
                return 0;
        }

        sprintf(cmd, "hexdump -n 2 /var/fw/%s", uploadfilename);
        fp = popen(cmd, "r");
        if (fp == NULL)
        {
            zyutil_dbg_printf("ERROR : popen hexdump -n 2 /var/fw/%s fail\n", uploadfilename);
                return 0;
        }

        if (fgets(buffer, 64, fp) == NULL)
        {
            zyutil_dbg_printf("ERROR : fgets fail\n");
            pclose(fp);
                return 0;
        }

        sscanf(buffer, "0000000 %d ", &magic_number);
        pclose(fp);

        if (magic_number == MagicNubmer) //function to be checked 
        {
                return 1;
        }
        else
        {
                return 0;
        }

}

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
zcfgRet_t zyUtilIGetMrdPasswd(int index, char **passwd){
	struct mrd mrd;
	char *input = NULL;
	char *prefix = MRD_PASSWD_PREFIX;
	bool validPassword = true;
	char mrdEncryptKey[MRD_KEY_LENGTH] = {0};
	int i = 0;

	if(index < 0 || index > 3){
		zyutil_dbg_printf("ERROR : zyUtilIGetMrdPasswd: the index [%d] is out of range.\n", index);
		return ZCFG_INTERNAL_ERROR;
	}

	if (zyUtilGetMrdInfo(&mrd) != 0) {
		zyutil_dbg_printf("ERROR : zyUtilGetMrdInfo failed.\n");
		return ZCFG_INTERNAL_ERROR;
	}

#ifdef SHA512_PASSWD
	if(strncmp(mrd.password[index], "$6$", 3) == 0){
		*passwd = strdup(mrd.password[index]);
	}
	else
#endif
	{
		i = 0;
		while(prefix[i] != '\0'){
			if((unsigned char)prefix[i] != mrd.password[index][i]){
				validPassword = false;
			}
			i++;
		}

		if(validPassword){
			input = mrd.password[index];
			input += strlen(prefix); //remove the prefix

			initKey(mrdEncryptKey);
#ifdef ZYXEL_USE_RANDOM_ENCRYPTION_SEED_FOR_MRD_PASSWORD
			mergeRandomSeedKey(mrdEncryptKey);
#endif
			*passwd = (char*)aesDecryptCbc256(input, mrdEncryptKey);
		}
		else{
			printf("%s, Error:[Index %d] No password in MRD or the data is broken.\n", __func__, index);
			*passwd = NULL;
		}
	}


	return ZCFG_SUCCESS;
}
#endif
#ifdef ZYXEL_LWM2M_PSK_IN_MRD
//for lwm2m psk 
zcfgRet_t zyUtilIGetMrdLwm2mPSK(char **psk){
	struct mrd mrd;
	char *input = NULL;
	char *prefix = MRD_PASSWD_PREFIX;
	bool validPsk = true;
	char mrdEncryptKey[MRD_KEY_LENGTH] = {0};
	int i = 0;

	if (zyUtilGetMrdInfo(&mrd) != 0) {
		zyutil_dbg_printf("ERROR : zyUtilGetMrdInfo failed.\n");
		return ZCFG_INTERNAL_ERROR;
	}

	i = 0;
	while(prefix[i] != '\0'){
		if((unsigned char)prefix[i] != mrd.Lwm2mPsk[i]){
			validPsk = false;
		}
		i++;
	}

	if(validPsk){
		input = mrd.Lwm2mPsk;
		input += strlen(prefix); //remove the prefix

		initKey(mrdEncryptKey);
#ifdef ZYXEL_USE_RANDOM_ENCRYPTION_SEED_FOR_MRD_PASSWORD
		mergeRandomSeedKey(mrdEncryptKey);
#endif
		*psk = (char*)aesDecryptCbc256(input, mrdEncryptKey);
	}
	else{
		printf("%s, Error: No PSK in MRD or the data is broken.\n", __func__);
		*psk = NULL;
	}


	return ZCFG_SUCCESS;
}
#endif
#endif

