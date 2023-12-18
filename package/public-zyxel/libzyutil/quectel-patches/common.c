#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <json/json.h>
#include <stdio.h>
#include <time.h>

#ifdef ABOG_CUSTOMIZATION
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#endif

#include <ctype.h>
#include <unistd.h>
#define _GNU_SOURCE
#include "zcfg_common.h"
#include "zcfg_debug.h"
#include "zhttpd_oid.h"

#include "zcfg_rdm_oid.h"
#include "zcfg_msg.h"
#include "zlog_api.h"
#include "zos_api.h"

#define MID_SIZE_MAX  32

#include "zyutil.h"

#include "libzyutil_wrapper.h"

#define IPTABLESRESTORE		"iptables-restore"
#define IP6TABLESRESTORE	"ip6tables-restore"

#if 0	//move to libzyutil_brcm_wrapper.c
#ifdef BROADCOM_PLATFORM
#include "board.h"
#include "bcm_hwdefs.h"
#include "bcmTag.h"

#define BOARD_DEVICE_NAME  "/dev/brcmboard"
#endif
#endif

/*
 *  Function Name: zyUtilIfaceHwAddrGet
 *  Description: Used to get the MAC Addfree of interface
 *  Parameters:
 *
 */
void zyUtilIfaceHwAddrGet(char *ifname, char *hwaddr)
{
	char buf[MID_SIZE_MAX+60];
	char sysCmd[1024];
	int i = 0, j = 0;
	char *p = NULL;
	FILE *fp = NULL;

	strcpy(hwaddr, "");

	sprintf(sysCmd, "ifconfig %s > /var/hwaddr ", ifname);
	system(sysCmd);

	fp = fopen("/var/hwaddr", "r");
	if(fp != NULL) {
		if (fgets(buf, sizeof(buf), fp)) {
			for(i = 2; i < (int)(sizeof(buf) - 5); i++) {
				if (buf[i]==':' && buf[i+3]==':') {
					p = buf+(i-2);
					for(j = 0; j < MID_SIZE_MAX-1; j++, p++) {
						if (isalnum(*p) || *p ==':') {
							hwaddr[j] = *p;
						}
						else {
							break;
						}
					}
					hwaddr[j] = '\0';
					break;
				}
			}
		}
		fclose(fp);
	}
}

bool zyUtilIsAppRunning(char *appName)
{
	char sysCmd[64];
	int size = 0;
	FILE *fp = NULL;
	bool isRunning = false;

	if(strlen(appName) == 0)
		return false;

	sprintf(sysCmd, "ps | grep %s | grep -v grep > /tmp/app", appName);
	system(sysCmd);

	fp = fopen("/tmp/app", "r");
	if(fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);

		if(size > 0)
			isRunning = true;

		fclose(fp);
	}

	return isRunning;
}

bool zyUtilCheckIfaceUp(char *devname)
{
	int skfd;
	int ret = false;
	struct ifreq intf;

	if (devname == NULL || (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return ret;
	}

	strcpy(intf.ifr_name, devname);

	/*   if interface is br0:0 and
	 *   there is no binding IP address then return down
	 */

	if (strchr(devname, ':') != NULL) {
		if (ioctl(skfd, SIOCGIFADDR, &intf) < 0) {
			close(skfd);
			return ret;
		}
	}

	// if interface flag is down then return down
	if (ioctl(skfd, SIOCGIFFLAGS, &intf) != -1) {
		if ((intf.ifr_flags & IFF_UP) != 0)
			ret = true;
	}

	close(skfd);

	return ret;
}

void zyUtilAddIntfToBridge(char *ifName, char *bridgeName)
{
	char sysCmd[64];

	if(ifName == NULL || ifName[0] == 0) return;
	if(bridgeName == NULL || bridgeName[0] == 0) return;
	//sprintf(sysCmd, "brctl delif %s %s 2>/dev/null", bridgeName, ifName);
	//system(sysCmd);
	sprintf(sysCmd, "ifconfig %s 0.0.0.0 2>/dev/null", ifName);
	if(system(sysCmd) == 0){
		sprintf(sysCmd, "ifconfig %s up", ifName);
		system(sysCmd);
		sprintf(sysCmd, "brctl addif %s %s", bridgeName, ifName);
		system(sysCmd);
		sprintf(sysCmd, "sendarp -s %s -d %s", bridgeName, bridgeName);
		system(sysCmd);
		sprintf(sysCmd, "sendarp -s %s -d %s", bridgeName, ifName);
		system(sysCmd);
	}
}

void zyUtilDelIntfFromBridge(char *ifName, char *bridgeName)
{
	char sysCmd[64];

	sprintf(sysCmd, "brctl delif %s %s", bridgeName, ifName);
	system(sysCmd);
	sprintf(sysCmd, "sendarp -s %s -d %s", bridgeName, bridgeName);
	system(sysCmd);
	sprintf(sysCmd, "sendarp -s %s -d %s", bridgeName, ifName);
	system(sysCmd);
}

#if 0	//move to libzyutil_brcm_wrapper.c
int boardIoctl(int board_ioctl, BOARD_IOCTL_ACTION action, char *string, int strLen, int offset, char *buf)
{
    BOARD_IOCTL_PARMS IoctlParms;
    int boardFd = 0;

    boardFd = open(BOARD_DEVICE_NAME, O_RDWR);
    if ( boardFd != -1 ) {
        IoctlParms.string = string;
        IoctlParms.strLen = strLen;	// using this to identify the gpio number.
        IoctlParms.offset = offset;
        IoctlParms.action = action;
        IoctlParms.buf    = buf;
        ioctl(boardFd, board_ioctl, &IoctlParms);
        close(boardFd);
        boardFd = IoctlParms.result;
    } else
        printf("Unable to open device /dev/brcmboard.\n");

    return boardFd;
}

int zyUtilGetGPIO(int gpioNum)
{
	int ret = 0;
	ret = boardIoctl(BOARD_IOCTL_GET_GPIO, 0, "", gpioNum, 0, "");
	return ret;
}
#endif

zcfgRet_t zyUtilGetDomainNameFromUrl(char *url, char *domainName, int domainNameLength)
{
	char *domainNameStartPtr = NULL;
	char *domainNameEndPtr = NULL;
	char tmpdomainName[256] = {0};

	if(!strcmp(url, "\0")){
		zcfgLog(ZCFG_LOG_INFO, "url string is empty \n");
		return ZCFG_SUCCESS;
	}

	domainNameStartPtr = strstr(url, "://");
	if (NULL == domainNameStartPtr) {
		domainNameStartPtr = url;
	}
	else {
		domainNameStartPtr += strlen("://");
	}


	if(domainNameStartPtr[0] == '[' && strstr(domainNameStartPtr, "]"))
		return ZCFG_SUCCESS;

	domainNameEndPtr = strstr(domainNameStartPtr, ":");
	if (NULL == domainNameEndPtr) {
		domainNameEndPtr = strstr(domainNameStartPtr, "/");
		if (NULL == domainNameEndPtr) {
			domainNameEndPtr = url + strlen(url);
		}
	}

	strncpy(tmpdomainName, domainNameStartPtr, (domainNameEndPtr-domainNameStartPtr));

	domainNameStartPtr = strstr(tmpdomainName, "*.");
	if (NULL == domainNameStartPtr) {
		domainNameStartPtr = tmpdomainName;
	}
	else {
		domainNameStartPtr += strlen("*.");
	}

	domainNameEndPtr = tmpdomainName + strlen(tmpdomainName);

	strncpy(domainName, domainNameStartPtr, (domainNameEndPtr-domainNameStartPtr));

	return ZCFG_SUCCESS;
}

bool zyUtilRetrieveSystemDefaultRoutIface(char *interfaceName)
{
	system("echo > /var/tr143NullInterface");//If interface is null
	FILE *fp;
	char defaultGateway[256] = {0};

	fp = popen("route", "r");
	while ((fgets(defaultGateway, 255, fp)) != NULL)
	{
		if (strstr(defaultGateway, "default") != 0)
		{
			if(strstr(defaultGateway, "eth") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "eth"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "eth")));
			else if(strstr(defaultGateway, "ptm") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "ptm"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "ptm")));
			else if(strstr(defaultGateway, "atm") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "atm"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "atm")));
			else if(strstr(defaultGateway, "ppp") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "ppp"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "ppp")));
			else if(strstr(defaultGateway, "wwan") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "wwan"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "wwan")));
			else if(strstr(defaultGateway, "br") != 0)
				strncpy(interfaceName, strstr(defaultGateway, "br"), (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "br")));
//#ifdef ZYXEL_ECONET_WAN_INTERFACE_NAMING_ENABLE
			else if(strstr(defaultGateway, "nas") != 0)
            	strncpy(interfaceName, strstr(defaultGateway, "nas")
                    	, (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "nas")));
			else if(strstr(defaultGateway, "ppp") != 0)
            	strncpy(interfaceName, strstr(defaultGateway, "ppp")
                    	, (strstr(defaultGateway, "\n")) - (strstr(defaultGateway, "ppp")));
//#endif
			else
				return false;

			return true;
		}
		else{
			strcpy(interfaceName, "br0");
		}
	}

	pclose(fp);

	return false;
}

#if 0
zcfgRet_t zyUtilAppStopByPidfile(char *pidFile)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char sysCmd[32] = {0};
	FILE *fptr = NULL;
	int pid = 0;

	zcfgLog(ZCFG_LOG_DEBUG, "%s : Enter\n", __FUNCTION__);

	if((fptr = fopen(pidFile, "r")) != NULL) {
		fscanf(fptr, "%d", &pid);
		printf("%s : Stop process with PID %d\n", __FUNCTION__, pid);
		snprintf(sysCmd, sizeof(sysCmd), "kill -9 %d", pid);
		zcfgLog(ZCFG_LOG_DEBUG, "Command %s\n", sysCmd);
		if(system(sysCmd) != 0)
			ret = ZCFG_INTERNAL_ERROR;
		fclose(fptr);
	}

	return ret;
}
#endif

zcfgRet_t zyUtilAppStopByPidfile(char *pidFile)
{
	zcfgRet_t   ret = ZCFG_SUCCESS;
	FILE        *fptr = NULL;
	int         pid = 0;

	fptr = fopen(pidFile, "r");
	if (fptr == NULL)
	{
	    ZLOG_WARNING("fail to open pid file = %s", pidFile);
	    return ret;
	}

	fscanf(fptr, "%d", &pid);
	fclose(fptr);

#if defined(SUNRISE_RELEASE_DHCP_PPP_BEFORE_IFDOWN) || defined(ECONET_PLATFORM)
	if (strstr(pidFile,"pppd") != NULL || strstr(pidFile,"/var/dhcpc") != NULL)
	{
		kill(pid, SIGTERM);
		sleep(1);
	}
#endif

	if (kill(pid, SIGKILL) == 0)
	{
        ZLOG_DEBUG("kill process with PID %d", pid);
	}
	else
	{
        ZLOG_WARNING("fail to kill process with PID %d", pid);
	}

	return ret;
}

zcfgRet_t zyUtilMacStrToNum(const char *macStr, uint8_t *macNum)
{
	char    *pToken = NULL;
	char    *pLast = NULL;
	char    *buf;
	int     i;

	if (macNum == NULL)
	{
	    ZLOG_ERROR("macNum == NULL");
		return ZCFG_INVALID_ARGUMENTS;
	}

	if (macStr == NULL)
	{
	    ZLOG_ERROR("macStr == NULL");
		return ZCFG_INVALID_ARGUMENTS;
	}

	buf = (char *)ZOS_MALLOC(MAC_STR_LEN + 1);
	if (buf == NULL)
	{
		ZLOG_ERROR("memory insufficient");
		return ZCFG_INTERNAL_ERROR;
	}

	/* need to copy since strtok_r updates string */
	strcpy(buf, macStr);

	/* Mac address has the following format
	 * xx:xx:xx:xx:xx:xx where x is hex number
	 */
	pToken = strtok_r(buf, ":", &pLast);
	macNum[0] = (uint8_t)strtol(pToken, (char **)NULL, 16);
	for (i = 1; i < MAC_ADDR_LEN; ++i)
	{
		pToken = strtok_r(NULL, ":", &pLast);
		macNum[i] = (uint8_t)strtol(pToken, (char **)NULL, 16);
	}

	ZOS_FREE(buf);

	return ZCFG_SUCCESS;
}

// array: has n units, [0,..,n-1]
// compRout: compare rout
// value: search unit by value
// l, r: array section, l=0, r=n-1 on starting
// return: searched array unit
void *zyUtilSortedArraySearch(void *array, compRout cr, void *value, int l, int r)
{
	int m = 0, ll, rr;
	void *unit = NULL;

	if(!array || !value || !cr)
		return NULL;

	if(l > r){
		return NULL;

	}else if(l == r){
		cr(array, l, value, &unit);
		return unit;
	}

	m = (l + r)/2;
	int rst = cr(array, m, value, &unit);
	if(rst == 0){
		return unit;
	}else if(rst > 0){
		ll = m+1; rr = r;
		return zyUtilSortedArraySearch(array, cr, value, ll, rr);
	}else{
		ll = l; rr = m-1;
		return zyUtilSortedArraySearch(array, cr, value, ll, rr);
	}
}

zcfgRet_t zyUtilMacNumToStr(const uint8_t *macNum, char *macStr)
{
	if(macNum == NULL || macStr == NULL) {
		return ZCFG_INVALID_ARGUMENTS;
	}

	sprintf(macStr, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
			(uint8_t) macNum[0], (uint8_t) macNum[1], (uint8_t) macNum[2],
			(uint8_t) macNum[3], (uint8_t) macNum[4], (uint8_t) macNum[5]);

   return ZCFG_SUCCESS;
}

/*!
*  replace string
*  @param[IN/OUT]	source
*  @param[IN]	find_str //keyword
*  @param[IN]	rep_str //replace
*/
void zyUtilStrReplace (char *source, char* find_str, char*rep_str){
	char result[65] = {0};
	char* str_temp = NULL;
	char* str_location = NULL;
	int rep_str_len = 0;
	int find_str_len = 0;
	int gap = 0;

	strcpy(result, source);
	find_str_len = strlen(find_str);
	rep_str_len = strlen(rep_str);
	str_temp = source;
	str_location = strstr(str_temp, find_str);

	while(str_location != NULL){
		gap += (str_location - str_temp);
		result[gap] = '\0';
		strcat(result, rep_str);
		gap += rep_str_len;
		str_temp = str_location + find_str_len;
		strcat(result, str_temp);
		str_location = strstr(str_temp, find_str);
	}

	result[strlen(result)] = '\0';
	memset(source, 0 , strlen(source));
	strcpy(source, result);
}


#if 1
void zyUtilBackslashInsert(char *str)
{
	char tmpStr[513] = {};
	int i = 0, j = 0, sym = 0;

	for (i=0; i<strlen(str); i++) {
		if ((48 <= *(str+i) && *(str+i) <= 57) || (65 <= *(str+i) && *(str+i) <= 90) || (97 <= *(str+i) && *(str+i) <= 122))
			sym = 0;
		else
			sym = 1;
		if (sym == 1) {
			*(tmpStr+j) = 0x5c;
			j++;
		}
		*(tmpStr+j) = *(str+i);
		j++;
	}
	strcpy(str, tmpStr);

	return;
}

void zyUtilBackslashInForEcho(char *str)
{
	char tmpStr[513] = {};
	int i = 0, j = 0, sym = 0;

	for (i=0; i<strlen(str); i++) {
		if ((*(str+i) == 34 ) || (*(str+i) == 36) || (*(str+i) == 92) || (*(str+i) == 96)){
			*(tmpStr+j) = 0x5c;
			j++;
		}
		*(tmpStr+j) = *(str+i);
		j++;
	}
	//strncpy(str, tmpStr, sizeof(str)); //Return limited size and null terminator(checked via strlen()).
	//Todo: the size of str should be considered to avoid overflowing
	strcpy(str, tmpStr);

	return;
}

void zyUtilPercentageInForEcho(char *str)
{
	char tmpStr[513] = {};
	int i = 0, j = 0, sym = 0;

	for (i=0; i<strlen(str); i++) {
		//insert % to esacpe from special character
		if ((*(str+i) == 37 )){
			*(tmpStr+j) = 0x25;
			j++;
		}
		*(tmpStr+j) = *(str+i);
		j++;
	}
	//strncpy(str, tmpStr, sizeof(str)); //Return limited size and null terminator(checked via strlen()).
	//Todo: the size of str should be considered to avoid overflowing
	strcpy(str, tmpStr);

	return;
}
#endif

#if 0	//move to libzyutil_brcm_wrapper.c
#ifdef BROADCOM_PLATFORM
#ifdef SUPPORT_LANVLAN
zcfgRet_t zyUtilISetIfState(char *ifName, bool up)
{
	int sockfd = 0;
	struct ifreq ifr;
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(ifName == NULL) {
        zcfgLog(ZCFG_LOG_ERR, "%s : Cannot bring up NULL interface\n", __FUNCTION__);
		ret = ZCFG_INTERNAL_ERROR;
	}
	else {
		/* Create a channel to the NET kernel. */
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            zcfgLog(ZCFG_LOG_ERR, "%s : Cannot create socket to the NET kernel\n", __FUNCTION__);
			ret = ZCFG_INTERNAL_ERROR;
		}
		else {
			strncpy(ifr.ifr_name, ifName, IFNAMSIZ);
            // get interface flags
			if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) != -1) {
				if(up)
					ifr.ifr_flags |= IFF_UP;
				else
					ifr.ifr_flags &= (~IFF_UP);

				if(ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
					zcfgLog(ZCFG_LOG_ERR, "%s : Cannot ioctl SIOCSIFFLAGS on the socket\n", __FUNCTION__);
					ret = ZCFG_INTERNAL_ERROR;
				}
			}
			else {
				zcfgLog(ZCFG_LOG_ERR, "%s : Cannot ioctl SIOCGIFFLAGS on the socket\n", __FUNCTION__);
				ret = ZCFG_INTERNAL_ERROR;
			}

			close(sockfd);
		}
	}

	return ret;
}
#endif

static uint32_t getCrc32(unsigned char *pdata, uint32_t size, uint32_t crc)
{
	while (size-- > 0)
		crc = (crc >> 8) ^ Crc32_table[(crc ^ *pdata++) & 0xff];

	return crc;
}
#if 0 //move to libzyutil_brcm_wrapper.c
int nvramDataWrite(NVRAM_DATA *nvramData)
{
	int boardFd = 0;
	int rc = 0;
	unsigned int offset = 0;
	BOARD_IOCTL_PARMS ioctlParms;
	uint32_t crc = CRC32_INIT_VALUE;

	nvramData->ulCheckSum = 0;
	crc = getCrc32((unsigned char *)nvramData, sizeof(NVRAM_DATA), crc);
	nvramData->ulCheckSum = crc;

	boardFd = open(BOARD_DEVICE_NAME, O_RDWR);

	if(boardFd != -1) {
		ioctlParms.string = nvramData;
		ioctlParms.strLen = sizeof(NVRAM_DATA);
		ioctlParms.offset = offset;
		ioctlParms.action = NVRAM;
		ioctlParms.buf    = NULL;
		ioctlParms.result = -1;

		rc = ioctl(boardFd, BOARD_IOCTL_FLASH_WRITE, &ioctlParms);
		close(boardFd);

		if(rc < 0) {
			printf("%s Set NVRAM Failure\n", __FUNCTION__);
			return -1;
		}
	}
	else {
		return -1;
	}

	return 0;
}

int nvramDataGet(NVRAM_DATA *nvramData)
{
	int boardFd = 0;
	int rc = 0;
	unsigned int offset = 0;
	BOARD_IOCTL_PARMS ioctlParms;

	boardFd = open(BOARD_DEVICE_NAME, O_RDWR);

	if(boardFd != -1) {
		ioctlParms.string = nvramData;
		ioctlParms.strLen = sizeof(NVRAM_DATA);
		ioctlParms.offset = offset;
		ioctlParms.action = NVRAM;
		ioctlParms.buf    = NULL;
		ioctlParms.result = -1;

		rc = ioctl(boardFd, BOARD_IOCTL_FLASH_READ, &ioctlParms);
		close(boardFd);

		if(rc < 0) {
			printf("%s Get NVRAM Failure\n", __FUNCTION__);
			return -1;
		}
	}
	else {
		return -1;
	}

	return 0;
}
#endif
zcfgRet_t zyUtilIGetSerailNum(char *sn)
{
	NVRAM_DATA nvramData;

	memset(&nvramData, 0, sizeof(NVRAM_DATA));

	if(nvramDataGet(&nvramData) < 0)
		return ZCFG_INTERNAL_ERROR;

	strcpy(sn, nvramData.SerialNumber);
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilIGetBaseMAC(char *mac)
{
	NVRAM_DATA nvramData;

	memset(&nvramData, 0, sizeof(NVRAM_DATA));

	if(nvramDataGet(&nvramData) < 0)
		return ZCFG_INTERNAL_ERROR;

        sprintf(mac ,"%02X%02X%02X%02X%02X%02X",
			nvramData.ucaBaseMacAddr[0],
			nvramData.ucaBaseMacAddr[1],
        	nvramData.ucaBaseMacAddr[2],
        	nvramData.ucaBaseMacAddr[3],
        	nvramData.ucaBaseMacAddr[4],
        	nvramData.ucaBaseMacAddr[5]
	);
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilIGetNumberOfMAC(int *num)
{
	NVRAM_DATA nvramData;

	memset(&nvramData, 0, sizeof(NVRAM_DATA));

	if(nvramDataGet(&nvramData) < 0)
		return ZCFG_INTERNAL_ERROR;

    if(num) *num =  nvramData.ulNumMacAddrs;
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilIGetProductName(char *pdname)
{
	if(!strcmp(ZYXEL_PRODUCT_NAME, "VMG3926-B10A") || !strcmp(ZYXEL_PRODUCT_NAME, "VMG8924-B10B")){
		int ret = 0, gpioNum = 11;
		ret = zyUtilGetGPIO(gpioNum);
		if(ret){//VMG3926
			strcpy(pdname, "VMG3926-B10A");
		}
		else{//VMG8924
			strcpy(pdname, "VMG8924-B10B");
		}
	}
	else{
		NVRAM_DATA nvramData;
		memset(&nvramData, 0, sizeof(NVRAM_DATA));

		if(nvramDataGet(&nvramData) < 0)
			return ZCFG_INTERNAL_ERROR;

		strcpy(pdname, nvramData.ProductName);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilIGetFirmwareVersion(char *fwversion)
{
	NVRAM_DATA nvramData;
#ifdef ELISA_CUSTOMIZATION
	char fwId[60] = "VMG3926-B10A_Elisa3_";
#endif
	memset(&nvramData, 0, sizeof(NVRAM_DATA));

	if(nvramDataGet(&nvramData) < 0)
		return ZCFG_INTERNAL_ERROR;
#ifdef ELISA_CUSTOMIZATION
	strcat(fwId, nvramData.FirmwareVersion);
	strcpy(fwversion, fwId);
#else
	strcpy(fwversion, nvramData.FirmwareVersion);
#endif
	return ZCFG_SUCCESS;
}

zcfgRet_t zyUtilIGetSerialNumber(char *serianumber)
{
	NVRAM_DATA nvramData;

	memset(&nvramData, 0, sizeof(NVRAM_DATA));

	if(nvramDataGet(&nvramData) < 0)
		return ZCFG_INTERNAL_ERROR;

	strcpy(serianumber, nvramData.SerialNumber);
	return ZCFG_SUCCESS;
}
#endif
#endif


char *zyUtilCsvStringStripTerm(const char *csv, const char *term)
{
	char *new_Csv = NULL;
	char *str_1 = NULL, *str_2 = NULL;
	int csvLen = strlen(csv);


	if (!csv || !term || (csvLen <= 0))
		return NULL;

	str_1 = strstr(csv, term);
	if (!str_1)
	{
		return NULL;
	}

	new_Csv = ZOS_MALLOC(csvLen);
	if (new_Csv == NULL)
	{
	    ZLOG_ERROR("memory insufficient");
	    return NULL;
	}

	memset(new_Csv, 0, csvLen);

	if (str_1 == csv)
	{
		str_2 = str_1 + strlen(term);
		if (str_2[0] != '\0')
		{
			str_2++;
			strcpy(new_Csv, str_2);
		}
	}
	else
	{
		str_2 = str_1 + strlen(term);
		strncpy(new_Csv, csv, (str_1-csv)-1);
		if (str_2[0] != '\0')
		{
			strcat(new_Csv, str_2);
		}
	}

	return new_Csv;
}

int zyUtilValidIpAddr(const char *ipAddress)
{
	struct in6_addr ipv6addr;
	struct in_addr ipaddr;

	if(ipAddress && (strlen(ipAddress) > 0) && strstr(ipAddress, ":")){
		if(inet_pton(AF_INET6, (const char *)ipAddress, (void *)&ipv6addr) != 1)
			return -1;
		else return AF_INET6;
	}else if(ipAddress && (strlen(ipAddress) > 0)) {
		if(inet_aton((const char *)ipAddress, &ipaddr) == 0)
			return AF_UNSPEC; //hostname ?
		else return AF_INET;
	}else
		return -1;
}

int zyUtilIp4AddrDomainCmp(const char *ipaddr1, const char *addrmask, const char *ipaddr2)
{
	in_addr_t addr1 = inet_addr(ipaddr1);
	in_addr_t mask = inet_addr(addrmask);

	in_addr_t addr2 = inet_addr(ipaddr2);

	if(addr1 < 0 || mask < 0 || addr2 < 0)
		return -1;

	return ((addr1 & mask) == (addr2 & mask)) ? 0 : 1;
}

#ifdef ZCFG_LOG_USE_DEBUGCFG
int zyUtilQueryUtilityEid(const char *name)
{
	//(!name || !strlen(name))



	if(!strcmp(name, "esmd")){

		return ZCFG_EID_ESMD;
	}
	else if(!strcmp(name, "tr69"))
        {

		return ZCFG_EID_TR69;
	}





	return 0;
}

// return: ZCFG_SUCCESS, it's debug config msg and processed
zcfgRet_t zyUtilHandleRecvdMsgDebugCfg(const char *msg)
{
	zcfgMsg_t *recvdMsg = (zcfgMsg_t *)msg;
	const char *recvdMsgData = NULL;
	struct json_object *debugObj = NULL;

	if(!recvdMsg)
		return ZCFG_INTERNAL_ERROR;

	if((recvdMsg->type != ZCFG_MSG_LOG_SETTING_CONFIG) || !recvdMsg->length)
		return ZCFG_REQUEST_REJECT;

	recvdMsgData = (const char *)(recvdMsg + 1);

	if(!recvdMsgData || !(debugObj = json_tokener_parse(recvdMsgData)))
		return ZCFG_INVALID_OBJECT;

	//printf("%s: debugobj %s\n", __FUNCTION__, recvdMsgData);
	struct json_object *levelObj = json_object_object_get(debugObj, "Level");
	if(!levelObj){
		json_object_put(debugObj);
		return ZCFG_INTERNAL_ERROR;
	}
	bool enable = json_object_get_boolean(json_object_object_get(debugObj, "Enable"));
	myDebugCfgObjLevel =  (enable == true) ? json_object_get_int(levelObj) : 0;
	const char *utilityName = json_object_get_string(json_object_object_get(debugObj, "Name"));
	if(utilityName && strlen(utilityName))
		printf("%s: debugobj %s\n", __FUNCTION__, utilityName);

	// it might require lock while utilizing myDebugCfgObj somewhere
	json_object_put(myDebugCfgObj);
	myDebugCfgObj = debugObj;

	return ZCFG_SUCCESS;
}
#else
int zyUtilQueryUtilityEid(const char *name)
{

	return 0;
}

zcfgRet_t zyUtilHandleRecvdMsgDebugCfg(const char *msg)
{

	return ZCFG_REQUEST_REJECT;
}
#endif

/**
 * zyUtilMailSend function parse receive massage and send email
 *
 * const char *msg retable json format string
 * json format string need "Event" key for event
 * Event string format:
 *             MAILSEND_EVENT_<your definition>
 *
 * MAILSEND_EVENT_3G_UP: 3G connection event
 * MAILSEND_EVENT_WIFI_UP: Wifi connection event
 * MAILSEND_EVENT_LOG_ALERT: Log alert event
 *
 ***/
zcfgRet_t zyUtilMailSend(const char *msg) {
	struct json_object *sendObj = NULL;
	struct json_object *value = NULL;
	char sysCmd[2084] = {0};
	char *bodypath = "/tmp/mailbody";
	const char *eventType;
	time_t time_stamp = time(NULL);
	struct tm *tPtr = localtime(&time_stamp);
	const char *filepath = NULL;
	int files = 0, delfile = 0;
	int rm_ret;
	FILE *fp;
	char buf[128];
	bool isSendSuccess = false;

	zcfgLog(ZCFG_LOG_INFO, "%s : Enter\n", __FUNCTION__);

	// Create a file and waiting for entering content, if file create fail
	// the mail will send only subject without body.
	FILE *mailbody = fopen(bodypath, "w");
	if(!mailbody)
		printf("Cannot create file for mail body, mail will send without content!");

	//printf("Got message: %s\n", msg);
	sendObj = json_tokener_parse(msg);

	fprintf(mailbody, "%s\n", json_object_get_string(json_object_object_get(sendObj, "body")));
	//fprintf(mailbody, "\nSend at\t%s", asctime(tPtr));
	fclose(mailbody);

	//for fixing Bare LFs error(should contain carriage return before newline)
	sprintf(sysCmd, "sed -i \"s/$/\\r/g\" %s", bodypath);
	system(sysCmd);
	memset(sysCmd, 0, sizeof(sysCmd));

	json_object_object_foreach(sendObj, keys, val) {
		if(!strcmp(keys,"body")) continue;
		// check if there are any file need to attach, set true
		if(!strcmp(keys,"FilePath")) {
			files = true;
			filepath = json_object_get_string(json_object_object_get(sendObj, "FilePath"));
			continue;
		}
		// check if the file need to delete or not
		if(!strcmp(keys, "Delete")) {
			if(!strcmp(json_object_get_string(json_object_object_get(sendObj, "Delete")),"true")) {
				delfile = 1;
				continue;
			}
			continue;
		}
		sprintf(sysCmd, "sed -i \"s/\\#{%s}/%s/g\" %s", keys, json_object_get_string(val), bodypath);
		system(sysCmd);
		memset(sysCmd, 0, sizeof(sysCmd));
	}

	// mailsend command
	char mailFrom[128] = {0}, security[128] = {0}, smtp[128] = {0}, mailTo[128] = {0}, user[128] = {0}, passwd[128] = {0}, subject[128] = {0};
	int port = 0;
	strcpy(mailFrom, (char *)json_object_get_string(json_object_object_get(sendObj, "mailfrom")));
	port = json_object_get_int(json_object_object_get(sendObj, "port"));
	strcpy(security, (char *)json_object_get_string(json_object_object_get(sendObj, "security")));
	strcpy(smtp, (char *)json_object_get_string(json_object_object_get(sendObj, "SMTP")));
	strcpy(mailTo, (char *)json_object_get_string(json_object_object_get(sendObj, "mailto")));
	strcpy(user, (char *)json_object_get_string(json_object_object_get(sendObj, "user")));
	strcpy(passwd, (char *)json_object_get_string(json_object_object_get(sendObj, "pass")));
	strcpy(subject, (char *)json_object_get_string(json_object_object_get(sendObj, "subject")));
	sprintf(sysCmd, "mailsend -f %s -port %d -smtp %s -domain %s -t %s -cs \"utf-8\" -auth -user %s -pass %s -sub \"%s\" ",
			mailFrom,
			port,
			smtp,
			smtp,
			mailTo,
			user,
			passwd,
			subject);

	if(strcmp(security, "none") != 0){
		strcat(sysCmd, " -");
		strcat(sysCmd, security);
	}

	if(files) {
		FILE *fp;
		char attachCmd[2048] ={0};
		char buff[1024] = {0};
		fp = popen("cat /tmp/mailbody", "r");
		fgets(buff, sizeof(buff), fp);
		/* mailsend can choose option between -attach file or -mime-type.
		 * It can attach multi files, but if it need mail body, use option -M only
		 * And now mailsend can only attach one text file */
		sprintf(attachCmd, " -attach \"%s,text/plain\" -M \"%s\"", filepath, buff);
		strcat(sysCmd, attachCmd);
		fclose(fp);
	} else {
		char *normalCmd = " -mime-type \"text/plain\" -msg-body \"/tmp/mailbody\"";
		strcat(sysCmd, normalCmd);
	}

        strcat(sysCmd, " -log /tmp/mailLog");

	printf("Sending Email...\n");
	//printf("%s\n", sysCmd);
	system(sysCmd);

	chmod("/tmp/mailLog", strtol("0777", 0, 8));
	if  ((fp = fopen("/tmp/mailLog", "r")) == NULL) {
                printf("fopen error!\n");
        }

	while (fgets(buf, sizeof(buf), fp) != NULL){
		if (strstr(buf, "Mail sent successfully") != NULL ){
			isSendSuccess = true;
			break;
		}
        }

	fclose(fp);

	unlink("/tmp/mailLog");

	if(delfile) {
		if((rm_ret = unlink(filepath)) != 0)
			printf("Cannot delete file (%s).  ret=%d\n", filepath, rm_ret);
	}
	if((rm_ret = unlink(bodypath)) != 0)
		printf("Cannot delete file (%s).  ret=%d\n", bodypath, rm_ret);

	json_object_put(sendObj);
	json_object_put(value);

	if(isSendSuccess)
		return ZCFG_SUCCESS;
	else
		return ZCFG_INTERNAL_ERROR;
}

bool zyUtilFileExists(const char * filename)
{
	FILE *file;
	if (file = fopen(filename, "r"))
	{
		fclose(file);
		return true;
	}

	return false;
}

/*ex: objNameAbbr: RDM_OID_DEVICE*/
int32_t zyUtilOidGet(const char *objNameAbbr)
{
	if(objNameAbbr == NULL)
		return -1;

	zHttpOidParallel *oidDictionaryAddr = &oidDictionary[0];
	do{
		if(strcmp(objNameAbbr, oidDictionaryAddr->abbreviation) == 0 )
			return oidDictionaryAddr->oid;
		oidDictionaryAddr++;
	}while(oidDictionaryAddr->abbreviation != NULL);

	return -1;
}

#if 0
/*zyUtilIptablesLockInit() should be only called once, for now it is called by beIptablesInit() in zcfg_be_iptables.c*/
void zyUtilIptablesLockInit(){
	union semun sem_union;

	iptables_sem_id = semget(ZCFG_IPTABLES_SEM_KEY,1,IPC_CREAT|0666);

	sem_union.val = 1;
	semctl(iptables_sem_id, 0, SETVAL, sem_union);
}

void zyUtilIptablesLock(){
	struct sembuf sem_b;

	if(iptables_sem_id < 0)
		iptables_sem_id = semget(ZCFG_IPTABLES_SEM_KEY,1,IPC_CREAT|0666);

	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	semop(iptables_sem_id, &sem_b, 1);
}

void zyUtilIptablesUnLock(){
	struct sembuf sem_b;

	if(iptables_sem_id < 0)
		iptables_sem_id = semget(ZCFG_IPTABLES_SEM_KEY,1,IPC_CREAT|0666);

	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	semop(iptables_sem_id, &sem_b, 1);
}
#endif

bool zyUtilInitIptablesConfig(iptCfg_t **ipt)
{
    if (ipt == NULL)
    {
        ZLOG_ERROR("ipt == NULL");
        return false;
    }

	*ipt = ZOS_MALLOC(sizeof(iptCfg_t));
	if (*ipt == NULL)
	{
	    ZLOG_ERROR("memory insufficient");
	    return false;
	}

	(*ipt)->fp_m = open_memstream(&(*ipt)->buf_m, &(*ipt)->len_m);
	(*ipt)->fp_f = open_memstream(&(*ipt)->buf_f, &(*ipt)->len_f);
	(*ipt)->fp_n = open_memstream(&(*ipt)->buf_n, &(*ipt)->len_n);
	(*ipt)->fp_m6 = open_memstream(&(*ipt)->buf_m6, &(*ipt)->len_m6);
	(*ipt)->fp_f6 = open_memstream(&(*ipt)->buf_f6, &(*ipt)->len_f6);
#ifdef MTKSOC_IP6TABLES_NAT_SUPPORT
	(*ipt)->fp_n6 = open_memstream(&(*ipt)->buf_n6, &(*ipt)->len_n6);
#endif

	return true;
}

void zyUtilRestoreIptablesConfig(iptCfg_t *ipt, const char *file, const char *func, int line){
	FILE *fp = NULL;
	char cmd[64] = {0};
	char fileName[32] = {0};
	int ret = 0;
	int fd;

	zcfgLog(ZCFG_LOG_INFO, "Enter %s, called from %s, %s():%d\n", __FUNCTION__, file, func, line);

	fclose(ipt->fp_m);
	fclose(ipt->fp_f);
	fclose(ipt->fp_n);
	fclose(ipt->fp_m6);
	fclose(ipt->fp_f6);
#ifdef MTKSOC_IP6TABLES_NAT_SUPPORT
	fclose(ipt->fp_n6);
#endif

	//iptables-restore
	if(ipt->len_m || ipt->len_f || ipt->len_n){
		strcpy(fileName, "/tmp/iptablesConfig-XXXXXX");
		fd = mkstemp(fileName);
		fp = fdopen(fd, "w+");
		if(ipt->len_m){
			fprintf(fp, "*mangle\n");
			fprintf(fp, "%s", ipt->buf_m);
			fprintf(fp, "COMMIT\n");
		}
		if(ipt->len_f){
			fprintf(fp, "*filter\n");
			fprintf(fp, "%s", ipt->buf_f);
			fprintf(fp, "COMMIT\n");
		}
		if(ipt->len_n){
			fprintf(fp, "*nat\n");
			fprintf(fp, "%s", ipt->buf_n);
			fprintf(fp, "COMMIT\n");
		}
		fclose(fp);

		sprintf(cmd, "%s --noflush %s", IPTABLESRESTORE, fileName);
		ret = system(cmd);
		if(ret != 0){
			zcfgLog(ZCFG_LOG_ERR, "%s fail, ret = %d, called from %s, %s():%d\n", IPTABLESRESTORE, ret, file, func, line);
			printf("%s fail, ret = %d, called from %s, %s():%d\n", IPTABLESRESTORE, ret, file, func, line);
			sprintf(cmd, "cat %s", fileName);
			system(cmd);
		}

		unlink(fileName);
	}

	//ip6tables-restore
	if(ipt->len_m6 || ipt->len_f6
#ifdef MTKSOC_IP6TABLES_NAT_SUPPORT
		|| ipt->len_n6
#endif
	){
		strcpy(fileName, "/tmp/iptablesConfig-XXXXXX");
		fd = mkstemp(fileName);
		fp = fdopen(fd, "w+");
		if(ipt->len_m6){
			fprintf(fp, "*mangle\n");
			fprintf(fp, "%s", ipt->buf_m6);
			fprintf(fp, "COMMIT\n");
		}
		if(ipt->len_f6){
			fprintf(fp, "*filter\n");
			fprintf(fp, "%s", ipt->buf_f6);
			fprintf(fp, "COMMIT\n");
		}
#ifdef MTKSOC_IP6TABLES_NAT_SUPPORT
		if(ipt->len_n6){
			fprintf(fp, "*nat\n");
			fprintf(fp, "%s", ipt->buf_n6);
			fprintf(fp, "COMMIT\n");
		}
#endif
		fclose(fp);

		sprintf(cmd, "%s --noflush %s", IP6TABLESRESTORE, fileName);
		ret = system(cmd);
		if(ret != 0){
			zcfgLog(ZCFG_LOG_ERR, "%s fail, ret = %d, called from %s, %s():%d\n",IP6TABLESRESTORE, ret, file, func, line);
			printf("%s fail, ret = %d, called from %s, %s():%d\n", IP6TABLESRESTORE, ret, file, func, line);
			sprintf(cmd, "cat %s", fileName);
			system(cmd);
		}

		unlink(fileName);
	}

	ZOS_FREE(ipt->buf_m);
	ZOS_FREE(ipt->buf_f);
	ZOS_FREE(ipt->buf_n);
	ZOS_FREE(ipt->buf_m6);
	ZOS_FREE(ipt->buf_f6);
#ifdef MTKSOC_IP6TABLES_NAT_SUPPORT
	ZOS_FREE(ipt->buf_n6);
#endif
	ZOS_FREE(ipt);
}

// return:
// -3: auth file does not exist
// -2: user account does not exist
// -1: auth fail
//  0: auth success
//  1: not support sha512 hash account password
int zyUtilAuthUserAccount(const char *username, const char *password)
{
	static unsigned int fileSerialNumber = 19475;
	char accInfo[512] = {0};
	char accFile[32] = {0};
	char syscmd[512];
	int fd, r, ret=-1;

#ifndef SHA512_PASSWD
	return 1;
#endif

	if (username == NULL || password == NULL) return -3;
	fileSerialNumber += 91;
	sprintf(accFile, "/var/tmp/tmp%d", fileSerialNumber);
	fd = open(accFile, O_RDWR|O_CREAT, 0600);
	if (fd < 0)
	{
		printf("%s.%d: cant not open file! \n", __func__, __LINE__);
		return -3;
	}

	sprintf(syscmd, "%s\r\n%s\r\n", username, password);
	write(fd, syscmd, strlen(syscmd));
	close(fd);

	sprintf(syscmd, "login -z %s", accFile);
	ZOS_SYSTEM(syscmd);

	fd = open(accFile, O_RDONLY);
	if (fd < 0)
	{
		sprintf(syscmd, "rm %s", accFile);
		ZOS_SYSTEM(syscmd);
		printf("%s.%d: cant not open file! \n", __func__, __LINE__);
		return -3;
	}

	r = read(fd, &accInfo[0], 512);
	close(fd);

	sscanf(&accInfo[0], "result:%d", &ret);

	sprintf(syscmd, "rm %s", accFile);
	ZOS_SYSTEM(syscmd);

	return ret;
}

char *zyGetAccountInfoFromFile(char *username, char *filePath){
    static char accountInfo[513] = {0};
    FILE *fp;

    fp = fopen(filePath, "r");
    if(fp == NULL){
        printf("%s: cannot open file '%s'!\n", __FUNCTION__, filePath);
        return NULL;
    }

    memset(accountInfo, 0, sizeof(accountInfo));
    while(fgets(accountInfo, sizeof(accountInfo), fp) != NULL){
        if(strncmp(username, accountInfo, strlen(username)) == 0){
            fclose(fp);
            return accountInfo;
        }
    }

    printf("%s: cannot find account '%s' in '%s'!", __FUNCTION__, username, filePath);
    fclose(fp);
    return NULL;
}

void zyRemoveAccountInfoFromFile(char *username, char *filePath)
{
    char    strBuff[513] = {0};
    char    *tmpFilePath = NULL;
    FILE    *fp;
    FILE    *tmpFp;

    fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        ZLOG_ERROR("fail to open file %s", filePath);
        return;
    }

    tmpFilePath = ZOS_MALLOC(sizeof(char) * (strlen(filePath) + 4 + 1));
	if (tmpFilePath == NULL)
	{
	    ZLOG_ERROR("memory insufficient");
	    return;
	}

    sprintf(tmpFilePath, "%s.tmp", filePath);
    tmpFp = fopen(tmpFilePath, "w");
    if (tmpFp == NULL)
    {
        ZLOG_ERROR("fail to create tmp file %s", tmpFilePath);
        fclose(fp);
        ZOS_FREE(tmpFilePath);
        return;
    }

    while (fgets(strBuff, sizeof(strBuff), fp) != NULL)
    {
        if (strncmp(username, strBuff, sizeof(username)) != 0)
        {
            fputs(strBuff, tmpFp);
        }
        memset(strBuff, 0, sizeof(strBuff));
    }

    fclose(tmpFp);
    fclose(fp);

    sprintf(strBuff, "mv %s %s", tmpFilePath, filePath);
    system(strBuff);
    sprintf(strBuff, "chmod 644 %s", filePath);
    system(strBuff);

    ZOS_FREE(tmpFilePath);
}

#ifdef ABOG_CUSTOMIZATION
#define REMOTE_MANAGEMENT_FILE_FOLDER   "/tmp/rmm/"
void zyTouchFile(int pid, char *ip)
{
	char    cmd[128] = {0};
	char    *addr = NULL;
	FILE    *fp = NULL;

	fp = fopen("/tmp/isWANRoute.sh", "r");
	if (fp != NULL)
	{
		fp = fopen("/tmp/isWANRoute.sh", "w");
		if (fp == NULL)
		{
		    ZLOG_ERROR("fp == NULL");
		}
		fprintf(fp, "route=`ip r get $1`\n");
		fprintf(fp, "if expr \"$1\" : '[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$' >/dev/null; then\n");
		fprintf(fp, "	iface=`echo $route | awk '{print substr($3,1,3)}'`\n");
		//fprintf(fp, "	echo iface is $iface\n");
		fprintf(fp, "	if [ \"$iface\" == \"nas\" ]; then\n");
		//fprintf(fp, "		echo $1 comes from wan side\n");
		fprintf(fp, "		exit 1\n");
		fprintf(fp, "	fi\n");
		fprintf(fp, "	iface=`echo $route | awk '{print substr($5,1,3)}'`\n");
		//fprintf(fp, "	echo iface is $iface\n");
		fprintf(fp, "	if [ \"$iface\" == \"nas\" ]; then\n");
		//fprintf(fp, "		echo $1 comes from wan side\n");
		fprintf(fp, "		exit 1\n");
		fprintf(fp, "	fi\n");
		//fprintf(fp, "	echo $1 comes from br side\n");
		fprintf(fp, "else\n");
		fprintf(fp, "	iface=`echo $route | awk '{print substr($5,1,3)}'`\n");
		//fprintf(fp, "	echo iface is $iface\n");
		fprintf(fp, "	if [ \"$iface\" == \"nas\" ]; then\n");
		//fprintf(fp, "		echo $1 comes from wan side\n");
		fprintf(fp, "		exit 1\n");
		fprintf(fp, "	fi\n");
		fprintf(fp, "	iface=`echo $route | awk '{print substr($7,1,3)}'`\n");
		//fprintf(fp, "	echo iface is $iface\n");
		fprintf(fp, "	if [ \"$iface\" == \"nas\" ]; then\n");
		//fprintf(fp, "		echo $1 comes from wan side\n");
		fprintf(fp, "		exit 1\n");
		fprintf(fp, "	fi\n");
		//fprintf(fp, "	echo $1 comes from br side\n");
		fprintf(fp, "fi\n");
		fprintf(fp, "exit 0\n");
#if 0
		fprintf(fp, "route=`ip r get $1`\n");
		fprintf(fp, "iface=`echo $route | awk '{print substr($3,1,2)}'`\n");
		fprintf(fp, "if [ \"$iface\" == \"nas\" ]; then\n");
		//fprintf(fp, "	echo $1 comes from wan side\n");
		fprintf(fp, "	exit 1\n");
		fprintf(fp, "fi\n");
		//fprintf(fp, "	echo $1 comes from br side\n");
		fprintf(fp, "exit 0\n");
#endif
	}
	fclose(fp);
	chmod("/tmp/isWANRoute.sh", S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH);

	if (strstr(ip, "."))
	{
		addr = strstr(ip, ":");
		if (addr)
			addr[0] = 0;
	}
	sprintf(cmd, "/tmp/isWANRoute.sh %s", ip);
	if (addr)
		addr[0] = ':';

	if (!system(cmd))
		return;

	DIR* dir = opendir(REMOTE_MANAGEMENT_FILE_FOLDER);
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
	}
	else
	{
		sprintf(cmd, "mkdir %s", REMOTE_MANAGEMENT_FILE_FOLDER);
		system(cmd);
	}
	sprintf(cmd, "touch %s%d", REMOTE_MANAGEMENT_FILE_FOLDER, pid);
	system(cmd);

	system("echo 0 > /proc/tc3162/led_pwr_green");
	system("echo 1 > /proc/tc3162/led_pwr_green");
}

static int zyCountFilesInRemoteManageFolder(void)
{
	int file_count = 0;
	DIR *dirp;
	struct dirent *entry;

	dirp = opendir(REMOTE_MANAGEMENT_FILE_FOLDER); /* There should be error handling after this */
	if (dirp == NULL)
	{
		return 0;
	}

	while ((entry = readdir(dirp)) != NULL)
	{
		if (entry->d_type == DT_REG) /* If the entry is a regular file */
		{
			++file_count;
		}
	}
	closedir(dirp);
	return file_count;
}

void zyRemoveFile(int pid)
{
	char cmd[128] = {0};

	DIR* dir = opendir(REMOTE_MANAGEMENT_FILE_FOLDER);
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
	}
	else
	{
		return;
	}

	sprintf(cmd, "rm %s%d &>/dev/null", REMOTE_MANAGEMENT_FILE_FOLDER, pid);
	system(cmd);

	if (zyCountFilesInRemoteManageFolder() == 0)
	{
		system("echo 0 > /proc/tc3162/led_pwr_green");
		system("echo 2 > /proc/tc3162/led_pwr_green");
	}
}
#endif

/*!
 *  Check if it is an alphanumeric character.
 *
 *  @param[in] unsigned char c  Input character.
 *  @return                     1:alphanumeric; 0:non-alphanumeric
 */
static int _common_isalnum(unsigned char c);
static int _common_isalnum(unsigned char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

/*!
 *  Check serial number format.
 *
 *  @param[in] char *str        Input string.
 *  @param[in] int len          Lenth of input string.
 *  @return                     0:ok; -1:error.
 */
int zyUtilChkSnFormat(char *str, int len)
{
    int i = 0, ret = -1;

    if (len != SERIAL_NUM_STR_LEN)
    {
        return ret;
    }

    ret = 0;
    for (i = 0; i < SERIAL_NUM_STR_LEN; ++i)
    {
        if ( ! _common_isalnum(str[i]) )
        {
            ret = -1; //format error
            return ret;
        }
    }

    return ret;
}
