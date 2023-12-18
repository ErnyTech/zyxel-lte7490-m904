#include "vendor_operate_async.h"

#include "dfota.h"
#include <fcntl.h>
#define DFOTA_S_PORT "/dev/ttyUSB2"
#define DFOTA_DOWNLOAD_TRY 10

int checkQfirehoseFile(const char *fileName, const int fileSize, const char *fileChecksum);
int fileSizeLastCheck = 0, fileStalledTimes = 0;


// borrowed from be_diagnostic_config.h
typedef struct uploadDiagParmData_s {
	bool addSpecificV4Rout;
	char username[250], password[250], host[50], port[10], dir[250];
	char interfaceName[30], intfaddr[50], addrmask[50];
	char defaultRouterIp[36];
	char rdmIfaceName[256+1];
} uploadDiagParmData_t;

enum {
	DIAG_TCP_HTTP,
	DIAG_TCP_FTP
};

extern asyncOperReqThread_t asyncOperReqThread[MAX_ASYNC_REQUEST_NUM];

int checkQfirehoseFile(const char *fileName, const int fileSize, const char *fileChecksum){
	FILE *pfp = NULL, *fp = NULL;
	char commandOutputs[256] = {0}, commands[256] = {0}, checksumDownload[33] = {0};
	int fileSizeNowCheck = 0, fileCheckTimes = 0;
	char *searchedString = NULL;

	//wait wget establishing connection
	while(1){
		if((fp = fopen(fileName, "r")) == NULL){
			printf("%s(%d) Error: open file %s failed.\n", __FUNCTION__, __LINE__, fileName);
			fileCheckTimes++;
			if(fileCheckTimes >= 3){
				return -1;
			}
			else{
				sleep(10);
			}
		}
		else{
			break;
		}
	}

	fseek(fp, 0L, SEEK_END);
	fileSizeNowCheck = ftell(fp);
	if(fp != NULL){
		fclose(fp);
		fp = NULL;
	}

	//printf("[QFirehose]fileSizeNowCheck is %d, fileSizeLastCheck is %d.\n", fileSizeNowCheck, fileSizeLastCheck);
	if(fileSizeNowCheck >= fileSize){
		printf("[QFirehose] The file %s is downloaded, now going to check its checksum value.\n", fileName);

		snprintf(commands, sizeof(commands), "md5sum %s", fileName);
		pfp = popen(commands, "r");
		if(pfp == NULL){
			printf("%s(%d) Error: popen failed.\n", __FUNCTION__, __LINE__);
			return -1;
		}
		else{
			memset(commandOutputs, 0, sizeof(commandOutputs));
			if(fgets(commandOutputs, 255, pfp) != NULL){
				memcpy(checksumDownload, commandOutputs, 32);
				checksumDownload[32] = '\0';
				printf("[QFirehose] checksum of file %s is %s, fileChecksum is %s.\n", fileName, checksumDownload, fileChecksum);
				if(strcasecmp(checksumDownload, fileChecksum) == 0){
					printf("[QFirehose] Checksum matched.\n");
					if(pfp != NULL){
						pclose(pfp);
						pfp = NULL;
					}
					return 1;
				}
				else{
					printf("[QFirehose] Checksum does not match.\n");
					if(pfp != NULL){
						pclose(pfp);
						pfp = NULL;
					}
					return -1;
				}
				
			}
			else{
				printf("[QFirehose]%s(%d): No md5 outputs of file %s.\n", __FUNCTION__, __LINE__, fileName);
				if(pfp != NULL){
					pclose(pfp);
					pfp = NULL;
				}
				return -1;
			}
		}
	}
	else if(fileSizeNowCheck > fileSizeLastCheck){
		printf("[QFirehose] %d > %d, The file is still downloading...\n", fileSizeNowCheck, fileSizeLastCheck);
		fileSizeLastCheck = fileSizeNowCheck;
	}
	else{
		fileStalledTimes++;
		printf("[Qfirehose] No files are downloaded(%d/%d), stalled(%d)...?\n", fileSizeNowCheck, fileSizeLastCheck, fileStalledTimes);
		//The network maybe disconnected.
		if(fileStalledTimes >= 5){
			if(pfp != NULL){
				pclose(pfp);
				pfp = NULL;
			}
			return -1;
		}
	}


	if(pfp != NULL){
		pclose(pfp);
		pfp = NULL;
	}
	return 0;
}

int addTransferCompleteParam(asyncOperReqThread_t *instance)
{
	USP_ARG_Add(instance->transferCompleteOuputArgs, "Command", (instance->cond)->command);
	USP_ARG_Add(instance->transferCompleteOuputArgs, "CommandKey", (instance->cond)->commandKey);
	USP_ARG_Add(instance->transferCompleteOuputArgs, "Requestor", (instance->cond)->originator);
	USP_ARG_Add(instance->transferCompleteOuputArgs, "Affected", (instance->cond)->parentPath);

	return USP_ERR_INTERNAL_ERROR;
}

int beIntIpAddrGet(const char *intf, char *intfAddr, int intfAddr_len)
{
	objIndex_t routeIid;
	rdm_RoutingRouter_t *routeObj = NULL;
	objIndex_t ipIfIid;
	rdm_IpIfaceV4Addr_t *ipv4Addr = NULL;
	objIndex_t v4AddrIid = {0};

	if (!strcmp(intf, "")) {
                strcpy(intf, "Device.");
                IID_INIT(routeIid);
                routeIid.level = 1;
                routeIid.idx[0] = 1;
                if( zcfgFeObjStructGet(RDM_OID_ROUTING_ROUTER, &routeIid, (void **)&routeObj) == ZCFG_SUCCESS){
                        strcat(intf, routeObj->X_ZYXEL_ActiveDefaultGateway);
                }
        }
	
	if ( strcmp(intf, "") && strstr(intf, "Device.IP.Interface.") ){
		IID_INIT(ipIfIid);
		sscanf(intf, "Device.IP.Interface.%hhu", &ipIfIid.idx[0]);
		IID_INIT(v4AddrIid);
		v4AddrIid.idx[0] = ipIfIid.idx[0];
		v4AddrIid.idx[1] = 1;
		v4AddrIid.level = 2;

		  if(zcfgFeObjStructGet(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &ipv4Addr) == ZCFG_SUCCESS){
			if(ipv4Addr->Enable == true) {
				//USP_LOG_Info("%s:  IPAddress %s\n", __FUNCTION__, ipv4Addr->IPAddress);
				memset(intfAddr, 0, intfAddr_len);
				strcpy(intfAddr, ipv4Addr->IPAddress);
				free(ipv4Addr);
				return USP_ERR_OK;
			}
			free(ipv4Addr);
		}
	}

	return USP_ERR_INTERNAL_ERROR;
}

int beIntNameGet(const char *intf, char *intfName, int intfAddr_len)
{
	objIndex_t ipIfIid;
	rdm_IpIface_t *ipv4Addr = NULL;
	objIndex_t v4AddrIid = {0};

	if ( strcmp(intf, "") && strstr(intf, "Device.IP.Interface.") ){
		IID_INIT(ipIfIid);
		sscanf(intf, "Device.IP.Interface.%hhu", &ipIfIid.idx[0]);
		IID_INIT(v4AddrIid);
		v4AddrIid.idx[0] = ipIfIid.idx[0];
		v4AddrIid.level = 1;

		  if(zcfgFeObjStructGet(RDM_OID_IP_IFACE, &v4AddrIid, &ipv4Addr) == ZCFG_SUCCESS){
			if(ipv4Addr->Enable == true) {
				USP_LOG_Info("%s:  Interface Name %s\n", __FUNCTION__, ipv4Addr->X_ZYXEL_IfName);
				memset(intfName, 0, intfAddr_len);
				strcpy(intfName, ipv4Addr->X_ZYXEL_IfName);
				free(ipv4Addr);
				return USP_ERR_OK;
			}
			free(ipv4Addr);
		}
	}

	return USP_ERR_INTERNAL_ERROR;
}

int beParseUrlParams(char *urlStr, char *username, char *password, char *host, char *port, char *dir)
{
	char *urlItems = NULL;
	char *hostUrlStr = NULL;
	int pst = 0, diagStatus = USP_ERR_OK;

	if(strstr(urlStr, "@")) {
		pst = sscanf((const char *)urlStr, "%[^:]:%[^@]@", username, password);
		if(pst == 2)
			hostUrlStr = urlStr + strlen(username) + strlen(password) + 2;
		else {
			diagStatus = USP_ERR_INVALID_ARGUMENTS;
		}
	}else {
		hostUrlStr = urlStr;
	}

#ifdef ZYXEL_IPV6_UPLOAD_DIAG_ENABLE
	/* Ian added */
	if(strstr(hostUrlStr, "[")) {
		pst = sscanf(hostUrlStr, "[%[^]]]:%[^/]/%s", ip, port, dir);
		if(pst != 3)
			diagStatus = USP_ERR_INVALID_ARGUMENTS;
		ipv6 = true;
	}
	else if(strstr(hostUrlStr, ":")) {
#else
	if(strstr(hostUrlStr, ":")) {
#endif
		pst = sscanf((const char *)hostUrlStr, "%[^:]:%[^/]/%s", host, port, dir);
		if(pst != 3) diagStatus = USP_ERR_INVALID_ARGUMENTS;
	}else {
		pst = sscanf((const char *)hostUrlStr, "%[^/]/%s", host, dir);
		if(pst != 2) diagStatus = USP_ERR_INVALID_ARGUMENTS;
	}

	return diagStatus;
}


bool zyUtilRetrieveSystemDefaultRoutIface(char *interfaceName)
{
	system("echo > /var/tr143NullInterface"); //If interface is null
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

BOOL zHttpCustomrequest_post(char *url,  char *ipaddr, int port, char *requestpath, char *fileStorePath)
{

	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;
	int ret = -1;
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;


#ifdef ZYXEL_TR69_SUPPORT_RPC_OF_HTTPS
	char httpscmd[128] = {0};
	if (strstr(url, "https")) {
		USP_LOG_Info("%s(%d), Detected HTTPS downlaod url..\n", __func__, __LINE__);
		ret = execute_command ("curl","-k",url,"--output",fileStorePath,"-S","--fail",NULL,NULL,NULL,NULL);
		USP_LOG_Info("Now downloading....\n");
		if( 0 == ret ){
			USP_LOG_Info("Downloaded upgrade file successfully.\n");
			return true;
		} else{
			USP_LOG_Info("Failed to download upgrade file.\n");
			return false;
		}
	}
#endif

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, requestpath);
	cg_http_request_setcontentlength(httpReq, 0);

	/*post http request*/
	httpRes = cg_http_customrequest_post(httpReq, ipaddr, port, fileStorePath);

	if ( !cg_http_response_issuccessful(httpRes)) {
		cg_http_request_delete(httpReq);
		return false;
	}

	cg_http_request_delete(httpReq);
	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return true;
}

int getCurrentTimeInDatetimeFormat(char* currentTime){
	struct tm *tm_ptr;
	time_t the_time;
	char buf[256];

	/* CurrentLocalTime */
	(void) time(&the_time);
	tm_ptr = localtime(&the_time);
	strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm_ptr);
	strncpy(currentTime, buf, DATETIME_LENGTH);

	return USP_ERR_OK;
}

void *AsyncOperation_Download_Thread(void *param){
	int i = 0, j = 0, ret = 0;
	int AutoActivate = -1;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	int rebootCountDown = -1, downloadInstance = -1;
	bool downloadStillSendingNotify = false;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;

	int tryDFOTADLCount, dfotaRet = 0;
	FILE *fp = NULL, *pfp = NULL;
	char commands[256] = {0}, qFireFileName[128] = {0}, tmp[256] = {0}, qFileFileChecksum[33] = {0};
	char *p_start = NULL, *p_end = NULL;
	int fileState = 0, fileTryDownloadTimes = 0, fileSize = 0, waitResetCount = 0;
	int rebootNeeded = 1;
	int fwInstalledPart = 1;


	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	
	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "Download");
	
	addTransferCompleteParam(reqThd);

	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		    USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			rebootNeeded = 0;
			goto exit;
	}

	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "URL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					faultCode = USP_ERR_INTERNAL_ERROR;
					rebootNeeded = 0;
					goto exit;
				}
				cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);
			}
			else if(strcmp(kv->key, "AutoActivate") == 0){
				if( !strcmp(kv->value, "true") || !strcmp(kv->value, "1") ){
					AutoActivate = 1;
				}
				else{
					AutoActivate = 0;
				}
			}
		}
	}

	printf("[Debugging Check] AutoActivate is %d.\n", AutoActivate);
	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0 || AutoActivate == -1){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		rebootNeeded = 0;
		goto exit;
	}


	if(strcmp((reqThd->cond)->fullPath, "Device.DeviceInfo.FirmwareImage.1.Download()") == 0
		|| strcmp((reqThd->cond)->fullPath, "Device.DeviceInfo.FirmwareImage.2.Download()") == 0
			) { // FirmwareImage for first partition or second partition
		sscanf((reqThd->cond)->fullPath, "Device.DeviceInfo.FirmwareImage.%d.Download()", &fwInstalledPart);
		if(fwInstalledPart != 1 && fwInstalledPart != 2){
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			rebootNeeded = 0;
			goto exit;
		}

		printf("%s(%d) FirmwareImage Download to partition %d.\n", __func__, __LINE__, fwInstalledPart);
		//Get the file via http get
		if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, FIRMWARE_STORE_PATH)){
			printf("%s(%d) Get file %s successfully, now is going to upgrade the system..\n", __func__, __LINE__, FIRMWARE_STORE_PATH);

			if((ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) != 0){
				USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_DISABLE).\n", __func__, __LINE__, ret);
				faultCode = USP_ERR_INTERNAL_ERROR;
				err = USP_ERR_COMMAND_FAILURE;
				rebootNeeded = 0;
				goto exit;
			}

			//start to upgrade firmware
			if(((fwInstalledPart == 1) && zcfgFeFwUpgradeWriteFirstPart(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS)
					|| ((fwInstalledPart == 2) && zcfgFeFwUpgradeWriteSecondPart(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS)
			){
				rebootNeeded = AutoActivate;
				USP_LOG_Info("%s(%d) zcfgFeFwUpgradeWrite() return ZCFG_SUCCESS.\n", __func__, __LINE__);
				if(AutoActivate)
				{
					USP_LOG_Info("%s(%d) The AutoActive is true, now is going to reboot...\n", __func__, __LINE__);
				}
				else
				{
					USP_LOG_Info("%s(%d) The AutoActive is false, no need to reboot now.\n", __func__, __LINE__);
				}
			}
			else{//fail to upgrade
				USP_LOG_Info("%s(%d) zcfgFeFwUpgrade() return ZCFG_FAIL.\n", __func__, __LINE__);
				USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to upgrade.", __FUNCTION__);
				USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to upgrade.", __FUNCTION__);
				faultCode = USP_ERR_INTERNAL_ERROR;
				err = USP_ERR_COMMAND_FAILURE;
				goto exit;
			}
		}
		else{//fail to download file
			USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, FIRMWARE_STORE_PATH);
			USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
			USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			rebootNeeded = 0;
			goto exit;
		}
		
	}
	else if(strcmp((reqThd->cond)->fullPath, "Device.DeviceInfo.FirmwareImage.3.Download()") == 0) { //Module DFOTA Image
		printf("%s(%d) Module DFOTA Image Download.\n", __func__, __LINE__);
		fprintf(stdout, "\n\t DFOTA MODULE FIRMWARE UPDATE\r\n\n");
		if(AutoActivate){
			/* before doing module upgrade, 
			make sure any process associated with module AT command channel does not exist, such as rilcmd, lte_srv_diag...
			*/
			//avoid module reset by network monitor
			system("killall lte_srv_diag");
			//delete rilcmd to avoid /dev/ttyUSB2 being used while updating
			system("killall rilcmd.sh");
			//workaround to avoid the ttyUSB is still used by rilcmd
			sleep(10);

			tryDFOTADLCount = DFOTA_DOWNLOAD_TRY;
			printf("DFOTA now update with url : %s.\n", servInfoPtr.url);

			if(strlen(servInfoPtr.url) > 0){
				while( tryDFOTADLCount >= 0 ){
					tryDFOTADLCount--;
					dfotaRet = updateModuleByDFOTA(DFOTA_S_PORT, servInfoPtr.url);
					if(0 == dfotaRet){
						printf( "****DFOTA Module Upgrade Success.****\n" );
						USP_LOG_Info( "DFOTA Module Upgrade Success.\n");
						break;
					}
					else if((tryDFOTADLCount > 0) && ((dfotaRet == 601) || (dfotaRet == 701))){//601:FTP unknown error, 701: HTTP(S) unknown error
						//try at least tryDFOTADLCount times to download the file via HTTP/FTP link
						printf( "**** DFOTA download fail(%d). Try it again(Remaining times = %d).****\n" , dfotaRet, tryDFOTADLCount);
						sleep(1);
						if(tryDFOTADLCount < 0)
						{
							USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: DFOTA download fail(%d).", __FUNCTION__, dfotaRet);
							USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: DFOTA download fail(%d).", __FUNCTION__, dfotaRet);
							faultCode = USP_ERR_INTERNAL_ERROR;
							err = USP_ERR_COMMAND_FAILURE;
						}
						continue;
					}
					else{
						if((tryDFOTADLCount <= 0) && ((dfotaRet == 601) || (dfotaRet == 701))){//601:FTP unknown error, 701: HTTP(S) unknown error
							USP_LOG_Error("****DFOTA download fail(%d)****\n", dfotaRet);
							printf( "****DFOTA download fail(%d)****\n" , dfotaRet);
							USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: DFOTA download fail(%d).", __FUNCTION__, dfotaRet);
							USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: DFOTA download fail(%d).", __FUNCTION__, dfotaRet);

							faultCode = USP_ERR_INTERNAL_ERROR;
							err = USP_ERR_COMMAND_FAILURE;
							break;
						}
						else{
							USP_LOG_Error("****DFOTA Module Upgrade Fail(%d)!!****\n", dfotaRet);
							printf("****DFOTA Module Upgrade Fail(%d)!!****\n", dfotaRet);
							USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: DFOTA Module Upgrade Fail(%d).", __FUNCTION__, dfotaRet);
							USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: DFOTA Module Upgrade Fail(%d).", __FUNCTION__, dfotaRet);
							faultCode = USP_ERR_INTERNAL_ERROR;
							err = USP_ERR_COMMAND_FAILURE;
							break;
						}
					}
				}//end while( tryDFOTADLCount >= 0 )
			} else {
				USP_LOG_Error("****DFOTA Module Upgrade Fail!!Invalid download url(%s)****", servInfoPtr.url);
				printf( "****DFOTA Module Upgrade Fail!!Invalid download url(%s)****\n" , servInfoPtr.url);
				USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: DFOTA Module Upgrade Fail!!Invalid download url(%s).", __FUNCTION__, servInfoPtr.url);
				USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: DFOTA Module Upgrade Fail!!Invalid download url(%s).", __FUNCTION__, servInfoPtr.url);
				faultCode = USP_ERR_INTERNAL_ERROR;
				err = USP_ERR_COMMAND_FAILURE;
			}

			//unlink(LOCAL_DFOTA_PATH);//router board does not download the .zip file, we just pass the url to LTE module, and LTE module will download the .zip file by itself
		}
		else {
			printf("[Debugging Check] AutoActivate(%d) should be true if upgrading the module.\n", AutoActivate);
			rebootNeeded = 0;
		}
	}
	else if(strcmp((reqThd->cond)->fullPath, "Device.DeviceInfo.FirmwareImage.4.Download()") == 0) { //Module QFirehose Image
		printf("%s(%d) Module QFirehose Image Download.\n", __func__, __LINE__);

		if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, FIRMWARE_STORE_PATH)){
			printf("%s Get file %s to %s successfully.\n", __FUNCTION__, servInfoPtr.url, FIRMWARE_STORE_PATH);
			USP_LOG_Info( "Get file %s to %s successfully.\n", servInfoPtr.url, FIRMWARE_STORE_PATH);

			printf("%s Jessie servInfoPtr.path=%s\n", __FUNCTION__, servInfoPtr.path);
			if(servInfoPtr.path && strstr(servInfoPtr.path, ".txt")){//to be refined .list? .txt?
				fprintf(stdout, "\n\t QFIREHOSE MODULE FIRMWARE DOWNLOAD\r\n\n");
	
					if((fp = fopen(FIRMWARE_STORE_PATH, "r")) == NULL){
						printf( "****QFIREHOSE Module DOWNLOAD Fail!! %s not found****\n", FIRMWARE_STORE_PATH);
						USP_LOG_Error("%s is not found. QFIREHOSE Module DOWNLOAD Fail.", FIRMWARE_STORE_PATH);
						USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: %s is not found. QFIREHOSE Module DOWNLOAD Fail.", __FUNCTION__, FIRMWARE_STORE_PATH);
						USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: %s is not found. QFIREHOSE Module DOWNLOAD Fail.", __FUNCTION__, FIRMWARE_STORE_PATH);
						faultCode = USP_ERR_INTERNAL_ERROR;
						err = USP_ERR_COMMAND_FAILURE;
						rebootNeeded = 0;
						goto exit;
					}
					else{
						printf("Now download with Qfirehose.\n");
						USP_LOG_Info( "QFIREHOSE Module DOWNLOAD is ready to download!\n");
						memset(commands, 0, sizeof(commands));
						while(fgets(commands, 255, fp) != NULL){

							if(commands[strlen(commands) - 1] == '\n'){
								commands[strlen(commands) - 1] = '\0';
							}

							printf("\n%s\n", commands);
							USP_LOG_Info( "Commands %s\n", commands);
							p_start = strstr(commands, "FILE:");
							if(p_start != NULL){
								//this line contains file information
								//get the file name
								p_start += 5;
								p_end = strstr(p_start, ":");
								memcpy(qFireFileName, p_start, p_end - p_start);
								qFireFileName[p_end-p_start] = '\0';


								//get the file size
								p_start = p_end + 1;
								p_end = strstr(p_start, ":");
								memcpy(tmp, p_start, p_end - p_start);
								tmp[p_end-p_start] = '\0';
								fileSize = atoi(tmp);

								//the last field contains checksum
								memcpy(qFileFileChecksum, p_end + 1, 32);
								qFileFileChecksum[32] = '\0';
								printf("The file's infomation is %s(%d/%s).\n", qFireFileName, fileSize, qFileFileChecksum);
								USP_LOG_Info( "The file's infomation is %s(%d/%s).\n", qFireFileName, fileSize, qFileFileChecksum);
								continue;	
							}
							if(strstr(commands, "wget") != NULL){
								//this line is wget command
								strcat(commands, " &");
								fileTryDownloadTimes = 0;
Retry_Download_file:
								fileTryDownloadTimes++;
								fileSizeLastCheck = 0;
								fileStalledTimes = 0;
								system(commands);
								//wait for wget establishing connection to HTTP file server
								sleep(10);
								while(fileTryDownloadTimes <= 3){
									fileState = checkQfirehoseFile(qFireFileName, fileSize, qFileFileChecksum);
									//file is downloading, continue to check
									if(0 == fileState){
										sleep(1);
										continue;
									}
									//file was downloaded, and its checksum value matched
									else if(1 == fileState){
										printf("The file %s is downloaded. Go to next.\n", qFireFileName);
										USP_LOG_Info( "The file %s is downloaded. Go to next.\n", qFireFileName);
										break;
									}
									//got error when downloading, maybe the network or the server is down
									else{
										system("killall wget");
										printf("Retry state: fileTryDownloadTimes is %d.\n", fileTryDownloadTimes);
										USP_LOG_Info( "Retry state: fileTryDownloadTimes is %d.\n", fileTryDownloadTimes);
										if(fileTryDownloadTimes <= 3){
											goto Retry_Download_file;
										}
										else{
											printf( "****QFIREHOSE Module Download Fail!!The file %s can't be dwonloaded.****\n", qFireFileName);
											USP_LOG_Info( "QFIREHOSE file %s can't be dwonloaded.\n", qFireFileName);
											USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: QFIREHOSE file %s can't be dwonloaded.", __FUNCTION__, qFireFileName);
											USP_SNPRINTF(faultString, sizeof(faultString), "%s: QFIREHOSE file %s can't be dwonloaded.", __FUNCTION__, qFireFileName);
											faultCode = USP_ERR_INTERNAL_ERROR;
											err = USP_ERR_COMMAND_FAILURE;
											rebootNeeded = 0;
											if(fp != NULL){
												fclose(fp);
												fp = NULL;
											}
											goto exit;
										}
									}
								}//end of while checking file state
							}//end of if: this command is wget
							else if(strstr(commands, "QFirehose") != NULL){
								//do nothing
								continue;
							}
							else if(strstr(commands, "killall rilcmd") != NULL){//just in case the command killall rilcmd in QFIREHOSE list
								//do nothing (do not kill rilcmd when downloading files only)
								continue;
							}
							else{
								//other commands like create folder
								system(commands);
							}
							memset(commands, 0, sizeof(commands));
						}//end of while fgets(): read commands from update list
					}//end of else: the update list file exists
					//do not unlink FIRMWARE_STORE_PATH, it will be used when execute /5/0/2
			}
		}//end of zHttpCustomrequest_post
		else {//fail to download file
			USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, FIRMWARE_STORE_PATH);
			USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
			USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			rebootNeeded = 0;
			if(fp != NULL){
				fclose(fp);
				fp = NULL;
			}
			goto exit;
		}

		if(fp != NULL){
			fclose(fp);
			fp = NULL;
		}

		if(AutoActivate){
			fprintf(stdout, "\n\t QFIREHOSE MODULE FIRMWARE UPDATE\r\n\n");

			if((fp = fopen(FIRMWARE_STORE_PATH, "r")) == NULL){

				printf( "****QFIREHOSE Module Upgrade Fail!! %s not found****\n", FIRMWARE_STORE_PATH);
				USP_LOG_Error("%s is not found. QFIREHOSE Module Upgrade Fail.", FIRMWARE_STORE_PATH);
				USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: %s is not found. QFIREHOSE Module Upgrade Fail.", __FUNCTION__, FIRMWARE_STORE_PATH);
				USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: %s is not found. QFIREHOSE Module Upgrade Fail.", __FUNCTION__, FIRMWARE_STORE_PATH);
				faultCode = USP_ERR_INTERNAL_ERROR;
				err = USP_ERR_COMMAND_FAILURE;
				rebootNeeded = 0;
			}
			else{
				printf("Now update with Qfirehose.\n");
				USP_LOG_Info("QFIREHOSE Module Upgrade is ready to start!\n");
				memset(commands, 0, sizeof(commands));
				while(fgets(commands, 255, fp) != NULL){

					if(commands[strlen(commands) - 1] == '\n'){
						commands[strlen(commands) - 1] = '\0';
					}

					printf("\n%s\n", commands);
					USP_LOG_Info("Commands %s\n", commands);
					p_start = strstr(commands, "FILE:");
					if(p_start != NULL){
						continue;	
					}
					if(strstr(commands, "wget") != NULL){
						//do nothing
						continue;
					} else if(strstr(commands, "QFirehose") != NULL){

						//avoid module reset by network monitor
						system("killall lte_srv_diag");
						//delete rilcmd to avoid /dev/ttyUSB2 being used while updating
						system("killall rilcmd.sh");
						//workaround to avoid the ttyUSB is still used by rilcmd
						sleep(10);

						//system(commands);
						err = (-1);
						pfp = popen(commands, "r");
						if(pfp == NULL){
							printf("Failed to execute commands %s\n ", commands);
						}
						else{
							while(fgets(tmp, 256, pfp) != NULL){
								printf("%s", tmp);
								if(strstr(tmp, "Upgrade module successfully") != NULL){
									printf( "****QFirehose Module Upgrade Success.****\n" );
									USP_LOG_Info("QFirehose Module Upgrade Success.\n");
									err = USP_ERR_OK;
									if(fp != NULL){
										fclose(fp);
										fp = NULL;
									}

									if(pfp != NULL){
										pclose(pfp);
										pfp = NULL;
									}
									goto exit;
								}
							}
						}
					}
					else{
						//other commands like create folder
						//do nothing
						continue;
					}
					memset(commands, 0, sizeof(commands));
				}//end of while fgets(): read commands from update list
			}//end of else: the update list file exists

			
			if(0 == err){
				//wait module complete reseting then reboot
				printf("Now wait module reset.\n");
				USP_LOG_Info("Now wait module reset.\n");
				sleep(10);
				waitResetCount = 0;
				//wait for reset at last to 1 min
				while(1){
					if(waitResetCount > 10){
						printf("waitResetCount > 10, not wait module reset.\n");
						USP_LOG_Info("waitResetCount > 10, not wait module reset.\n");
						break;
					}
					if(open("/dev/ttyUSB2", O_RDWR | O_NDELAY) == -1){
						printf("waitResetCount(%d) , still wait module reset....\n", waitResetCount);
						USP_LOG_Info("waitResetCount(%d) , still wait module reset.\n", waitResetCount);
						sleep(5);
						waitResetCount++;
					}
					else{
						printf("Module had been reset successfully.\n");
						USP_LOG_Info("Module had been reset successfully.\n");
						sleep(12);
						break;
					}
				}
			}
			else {
				printf( "****QFirehose Module Upgrade Fail.****\n" );
				USP_LOG_Info("QFirehose Module Upgrade Fail.\n");
				USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: QFirehose Module Upgrade Fail.", __FUNCTION__);
				USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: QFirehose Module Upgrade Fail.", __FUNCTION__);
				faultCode = USP_ERR_INTERNAL_ERROR;
				err = USP_ERR_COMMAND_FAILURE;
			}
			
			unlink(FIRMWARE_STORE_PATH);
			//unlink qfirehose fw?

			if(fp != NULL){
				fclose(fp);
				fp = NULL;
			}

			if(pfp != NULL){
				pclose(pfp);
				pfp = NULL;
			}
		}
		else {
			printf("[Debugging Check] AutoActivate(%d) should be true if upgrading the module.\n", AutoActivate);
			rebootNeeded = 0;
		}
	}

	/*
	printf("%s(%d) req path: %s\n", __func__, __LINE__, (reqThd->cond)->fullPath);
	printf("%s(%d) path: %s\n", __func__, __LINE__, (reqThd->cond)->parentPath);
	printf("%s(%d) action: %s\n", __func__, __LINE__, (reqThd->cond)->action);
	*/
exit:
	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);
	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);


	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}
	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	downloadInstance = (reqThd->cond)->request_instance;
	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);

	if(1 == rebootNeeded){
		char requestPath[32] = {0};

		downloadStillSendingNotify = false;
		int_vector_t requestVector;
		INT_VECTOR_Init(&requestVector);
		snprintf(requestPath, sizeof(requestPath), "%s", DEVICE_REQ_ROOT);

		rebootCountDown = REBOOT_COUNTDOWN_TIMES;
		do{
			downloadStillSendingNotify = false;
			//FIXME: Remove data_model API, use zcfg API instead ?
			if(DATA_MODEL_GetInstances(requestPath, &requestVector) == USP_ERR_OK){
				for (i=0; i < requestVector.num_entries; i++)
				{
					if(downloadInstance == requestVector.vector[i]){
						USP_LOG_Info("%s(%d) Get requestPath %s.%d successfully.\n", __func__, __LINE__, requestPath, downloadInstance);
						USP_LOG_Info("Slepp 1 seconds then checking again...\n");
						downloadStillSendingNotify = true;
						rebootCountDown--;
						sleep(1);
						continue;
					}
				}
			}
			else{
				USP_LOG_Error("%s(%d) Fail to get requestPath %s.\n", __func__, __LINE__, requestPath);
				break;
			}

		}while(rebootCountDown > 0 && downloadStillSendingNotify);

#if 0
		if(rebootCountDown <= 0 && downloadStillSendingNotify){
			printf("rebootCountDown <=0, requestPath %s.%d still exists.\n", requestPath, downloadInstance);
		}
#endif

		printf("Rebooting...\n");
		zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "Rebooting...");
		zcfgFeReqReboot(NULL);
	}
	return NULL;
}

int localagentReqCancel(int reqInstance){
	asyncOperReqThread_t *reqThd = &(asyncOperReqThread[reqInstance]);
	int err = USP_ERR_OK;
	char path[MAX_DM_PATH];
	void *res;
	char buf[256];
	USP_LOG_Info("Now enter %s(%d) Instance %d, the threadID is %lu.\n", __func__, __LINE__, reqInstance, reqThd->threadID);
	// Exit if unable to find this instance in the request table
	if (IsRequestInstanceValid(reqInstance) == false){
		// This should only ever happen, if an operation has been cancelled but the vendor had previously queued the operation complete
		USP_LOG_Error("%s(%d): %s.%d (invalid instance)", __FUNCTION__, __LINE__, DEVICE_REQ_ROOT, reqInstance);
		return -1;
	}

	if(reqThd->auxiliary){
		snprintf(buf, sizeof(buf), "kill -9 `ps -w | grep %s | grep -v grep | awk '{print $1}'`", (char*)reqThd->auxiliary);
		USP_LOG_Info("%s() kill command %s\n", __FUNCTION__, buf);
		system(buf);
		unlink((char*)reqThd->auxiliary);
		system(buf);
		free(reqThd->auxiliary);
		reqThd->auxiliary = NULL;
	}
	
	//cancel the thread
	if((err = pthread_cancel(reqThd->threadID)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_cancel.\n", __func__, __LINE__, err);
		return err;
	}
	else{
		USP_LOG_Info("%s(%d) The thread with instance %d has been canceled.\n", __func__, __LINE__, reqInstance);

		//Delete the request instance: Device.LocalAgent.Request.[index]
		USP_SNPRINTF(path, sizeof(path), "%s.%d", DEVICE_REQ_ROOT, reqInstance);

		USP_SIGNAL_ObjectDeleted(path);
		//Only use zcfg API in non-main thread
		//err = DATA_MODEL_DeleteInstance(path, 0);
		err = zcfg_del_object(path);
		if (err != USP_ERR_OK){
			USP_LOG_Error("%s(%d): Error(= %d) Unable to delete %s after operation completed", __FUNCTION__, __LINE__, err, path);
			return err;
		}
		
		//release the resources after cancel the thread
		if((reqThd->cond) != NULL){
			if((reqThd->cond)->input_args != NULL){
				USP_FREE((reqThd->cond)->input_args);
				(reqThd->cond)->input_args = NULL;
			}
				USP_FREE((reqThd->cond));
				(reqThd->cond) = NULL;
		}

		if(reqThd->operCompleteArgs != NULL){
			USP_ARG_Destroy(reqThd->operCompleteArgs);
			reqThd->operCompleteArgs = NULL;
		}

		if(reqThd->transferCompleteOuputArgs != NULL){
			USP_ARG_Destroy(reqThd->transferCompleteOuputArgs);
			reqThd->transferCompleteOuputArgs = NULL;
		}
    }

	/*
	printf("[Marcus debug] %s(%d) tid %lu\n", __func__, __LINE__, (unsigned long)(reqThd->threadID));
	if((err = pthread_join(reqThd->threadID, &res)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_join.\n", __func__, __LINE__, err);
		return err;
	}
	*/


	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return err;
}


void *AsyncOperation_IPPing_Thread(void *param){//Device.IP.Diagnostics.IPPing()
	int i = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;

	char diagnosticsState[256] = {0};
	char interface[256] = {0};
	char host[256] = {0};
	char protocolVersion[256] = {0};
	char numberOfRepetitions[256] = {0};
	char timeout[256] = {0};
	char **ptr = NULL;
	uint32_t timeout_digit = 0;
	char dataBlockSize[256] = {0};
	char dscp[256] = {0};

	char ifName[33] = {0};
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	char ipAddressUsed[32] = "";
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	char diagResultFile[256];
	char diagProcessFile[256];
	struct stat statBuf;
	struct json_object *diagResultParams = NULL;
	char tmpString[64];
	
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	snprintf(diagResultFile, sizeof(diagResultFile), "%s.ping%d", DIAG_RESULT_PATH, (reqThd->cond)->request_instance);
	remove(diagResultFile);
	snprintf(diagProcessFile, sizeof(diagProcessFile), "%s.ping%d", DIAG_PROCESS_PATH, (reqThd->cond)->request_instance);
	remove(diagProcessFile);
	USP_LOG_Info("%s() diagProcessFile=%s.\n", __func__, diagProcessFile);
	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}
	//get input params
	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "DiagnosticsState") == 0){
				snprintf(diagnosticsState, sizeof(diagnosticsState), "%s", kv->value);
			} else if(strcmp(kv->key, "Interface") == 0){
				snprintf(interface, sizeof(interface), "%s", kv->value);
			} else if(strcmp(kv->key, "ProtocolVersion") == 0){
				snprintf(protocolVersion, sizeof(protocolVersion), "%s", kv->value);
			} else if(strcmp(kv->key, "Host") == 0){
				snprintf(host, sizeof(host), "%s", kv->value);
			} else if(strcmp(kv->key, "NumberOfRepetitions") == 0){
				snprintf(numberOfRepetitions, sizeof(numberOfRepetitions), "%s", kv->value);
			} else if(strcmp(kv->key, "Timeout") == 0){
				snprintf(timeout, sizeof(timeout), "%s", kv->value);
			} else if(strcmp(kv->key, "DataBlockSize") == 0){
				snprintf(dataBlockSize, sizeof(dataBlockSize), "%s", kv->value);
			} else if(strcmp(kv->key, "DSCP") == 0){
				snprintf(dscp, sizeof(dscp), "%s", kv->value);
			}
		}
	}
	//create symbolic link to identify request_instance (Device.LocalAgent.Request.i.)
	char symLinkBuf[64];
	struct stat st = {0};
	snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);
	if (stat(symLinkBuf, &st) == -1) {
		mkdir(symLinkBuf, 0700);
	}
	snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/ping", PATH_REQUEST, (reqThd->cond)->request_instance);
	USP_LOG_Info("%s() symLinkBuf=%s\n", __FUNCTION__, symLinkBuf);
	symlink(PATH_PING, symLinkBuf);
	
	//create complete ping command with command options
	char sysCmd[256+64] = {0};
	char *pCmd = sysCmd;

	sprintf(pCmd, symLinkBuf);
	strcat(pCmd, " ");//add space
	pCmd += strlen(pCmd);
	if(strcmp(protocolVersion, "Any")){
		strcat(pCmd, "-");
		pCmd++;
	}

	if(!strcmp(protocolVersion, "IPv6"))
		sprintf(pCmd, "6 ");
	else if(!strcmp(protocolVersion, "IPv4"))
		sprintf(pCmd, "4 ");
	else sprintf(pCmd, "4 ");//default IPv4
	pCmd += 2;

	snprintf(tmpString, sizeof(tmpString), "-p %d", (reqThd->cond)->request_instance);
	strcat(pCmd, tmpString);
	
	if(strlen(dataBlockSize) > 0){
		strcat(pCmd, " -s ");
		strcat(pCmd, dataBlockSize);
	}
	if(strlen(numberOfRepetitions) > 0){
		strcat(pCmd, " -c ");
		strcat(pCmd, numberOfRepetitions);
	} else {
		strcat(pCmd, " -c ");
		strcat(pCmd, DEFAULT_IPPING_NUM_REPETITION);
	}
	if(strlen(interface)>0){
		beIntNameGet(interface, ifName, sizeof(ifName));
		strcat(pCmd, " -I ");
		strcat(pCmd, ifName);
	}

	if(strlen(timeout) > 0){
		timeout_digit = strtoul(timeout, ptr, 10) / 1000;
		timeout_digit = timeout_digit > 0 ? timeout_digit : 1;

		strcat(pCmd, " -W ");
		snprintf(pCmd+strlen(pCmd), sizeof(pCmd)-strlen(pCmd)-1, "%u", timeout_digit);
		//strcat(pCmd, timeout);
	}

	snprintf(tmpString, sizeof(tmpString), " %s >%s&", host, diagProcessFile);
	printf("%s() Jessie tmpString %s\n", __FUNCTION__, tmpString);
	strcat(pCmd, tmpString);
	printf("%s() Jessie pCmd %s\n", __FUNCTION__, pCmd);
	system(sysCmd);

	USP_LOG_Info("%s() diagResultFile=%s,sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);
	
	//waiting for the diagResultFile file
	while(stat(diagResultFile, &statBuf)){
		sleep(1);
	}
	//prepare output params
	diagResultParams = json_object_from_file(diagResultFile);

exit:
	// Save all results into the output arguments using KV_VECTOR_ functions
	(reqThd->operCompleteArgs) = USP_ARG_Create();
	
	if(diagResultParams){
		beIntIpAddrGet(interface, ipAddressUsed, sizeof(ipAddressUsed));

		uint32_t averageResponseTimeDetailed = json_object_get_int(json_object_object_get(diagResultParams,"AverageResponseTime"));
		uint32_t minimumResponseTimeDetailed = json_object_get_int(json_object_object_get(diagResultParams,"MinimumResponseTime"));
		uint32_t maximumResponseTimeDetailed = json_object_get_int(json_object_object_get(diagResultParams,"MaximumResponseTime"));
		uint32_t averageResponseTime = averageResponseTimeDetailed / 1000;
		uint32_t minimumResponseTime = minimumResponseTimeDetailed / 1000;
		uint32_t maximumResponseTime = maximumResponseTimeDetailed / 1000;

		USP_ARG_Add((reqThd->operCompleteArgs), "Status", json_object_get_string(json_object_object_get(diagResultParams,"DiagnosticsState")));
		USP_ARG_Add((reqThd->operCompleteArgs), "IPAddressUsed", ipAddressUsed);
		USP_ARG_Add((reqThd->operCompleteArgs), "SuccessCount", json_object_get_string(json_object_object_get(diagResultParams,"SuccessCount")));
		USP_ARG_Add((reqThd->operCompleteArgs), "FailureCount", json_object_get_string(json_object_object_get(diagResultParams,"FailureCount")));
		snprintf(tmpString, sizeof(tmpString), "%u", averageResponseTimeDetailed);
		USP_ARG_Add((reqThd->operCompleteArgs), "AverageResponseTimeDetailed", tmpString);
		snprintf(tmpString, sizeof(tmpString), "%u", minimumResponseTimeDetailed);
		USP_ARG_Add((reqThd->operCompleteArgs), "MinimumResponseTimeDetailed", tmpString);
		snprintf(tmpString, sizeof(tmpString), "%u", maximumResponseTimeDetailed);
		USP_ARG_Add((reqThd->operCompleteArgs), "MaximumResponseTimeDetailed", tmpString);

		snprintf(tmpString, sizeof(tmpString), "%u", averageResponseTime);
		USP_ARG_Add((reqThd->operCompleteArgs), "AverageResponseTime", tmpString);
		snprintf(tmpString, sizeof(tmpString), "%u", minimumResponseTime);
		USP_ARG_Add((reqThd->operCompleteArgs), "MinimumResponseTime", tmpString);
		snprintf(tmpString, sizeof(tmpString), "%u", maximumResponseTime);
		USP_ARG_Add((reqThd->operCompleteArgs), "MaximumResponseTime", tmpString);
	}

	if(err != USP_ERR_OK){
		err_msg = res->err_msg;
	}
	//NOTIFY message with OperationComplete including output params
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, (reqThd->operCompleteArgs));
	reqThd->operCompleteArgs = NULL;

	//free after use
	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}
	
	unlink(symLinkBuf);
	
	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_TraceRoute_Thread(void *param){//Device.IP.Diagnostics.TraceRoute()
	int i = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;

	char diagnosticsState[MAX_BUF_LEN] = {0};
	char interface[MAX_BUF_LEN] = {0};
	char host[MAX_BUF_LEN] = {0};
	char protocolVersion[MAX_BUF_LEN] = {0};
	char numberOfTries[MAX_BUF_LEN] = {0};
	char timeout[MAX_BUF_LEN] = {0};
	char dataBlockSize[MAX_BUF_LEN] = {0};
	char dscp[MAX_BUF_LEN] = {0};
	char maxHopCount[MAX_BUF_LEN] = {0};

	char ifName[33] = {0};
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	char ipAddressUsed[32] = "";
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	char diagResultFile[256];
	char diagProcessFile[256];
	struct stat statBuf;
	struct json_object *diagResultParams = NULL;
	char tmpString[64];
	
	objIndex_t objIid;
	rdm_IpDiagTraceRt_t *traceRtObj = NULL;
	rdm_IpDiagTracertRtHops_t *rtHop = NULL;
	struct json_object *route_hops = NULL;
	struct json_object *resultHops = NULL;
	struct json_object *value = NULL;
	int count = 0, c = 0, ii = 1;
	char strBuf[64];
	
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	snprintf(diagResultFile, sizeof(diagResultFile), "%s.traceroute%d", DIAG_RESULT_PATH, (reqThd->cond)->request_instance);
	remove(diagResultFile);
	snprintf(diagProcessFile, sizeof(diagProcessFile), "%s.traceroute%d", DIAG_PROCESS_PATH, (reqThd->cond)->request_instance);
	remove(diagProcessFile);
	
	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}
	//get input params
	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "DiagnosticsState") == 0){
				snprintf(diagnosticsState, sizeof(diagnosticsState), "%s", kv->value);
			} else if(strcmp(kv->key, "Interface") == 0){
				snprintf(interface, sizeof(interface), "%s", kv->value);
			} else if(strcmp(kv->key, "ProtocolVersion") == 0){
				snprintf(protocolVersion, sizeof(protocolVersion), "%s", kv->value);
			} else if(strcmp(kv->key, "Host") == 0){
				snprintf(host, sizeof(host), "%s", kv->value);
			} else if(strcmp(kv->key, "NumberOfTries") == 0){
				snprintf(numberOfTries, sizeof(numberOfTries), "%s", kv->value);
			} else if(strcmp(kv->key, "Timeout") == 0){
				snprintf(timeout, sizeof(timeout), "%s", kv->value);
			} else if(strcmp(kv->key, "DataBlockSize") == 0){
				snprintf(dataBlockSize, sizeof(dataBlockSize), "%s", kv->value);
			} else if(strcmp(kv->key, "DSCP") == 0){
				snprintf(dscp, sizeof(dscp), "%s", kv->value);
			} else if(strcmp(kv->key, "MaxHopCount") == 0){
				snprintf(maxHopCount, sizeof(maxHopCount), "%s", kv->value);
			}
		}
	}
	//create symbolic link to identify request_instance (Device.LocalAgent.Request.i.)
	char symLinkBuf[64];
	struct stat st = {0};
	snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);
	if (stat(symLinkBuf, &st) == -1) {
		mkdir(symLinkBuf, 0700);
	}
	
	//create complete traceroute command with command options
	char sysCmd[256+64] = {0};
	char *pCmd = sysCmd;
	
	if(strcmp(protocolVersion, "Any") || !strcmp(protocolVersion, "IPv4")){
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/traceroute", PATH_REQUEST, (reqThd->cond)->request_instance);	
	} else if(!strcmp(protocolVersion, "IPv6"))
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/traceroute6", PATH_REQUEST, (reqThd->cond)->request_instance);
	else snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/traceroute", PATH_REQUEST, (reqThd->cond)->request_instance);
	
	USP_LOG_Info("%s() symLinkBuf=%s\n", __FUNCTION__, symLinkBuf);
	symlink(PATH_TRACEROUTE, symLinkBuf);
	
	sprintf(pCmd, symLinkBuf);
	strcat(pCmd, " ");//add space
	pCmd += strlen(pCmd);

	snprintf(tmpString, sizeof(tmpString), "-a %d", (reqThd->cond)->request_instance);
	strcat(pCmd, tmpString);	
	
	if(strlen(numberOfTries) > 0){
		strcat(pCmd, " -q ");
		strcat(pCmd, numberOfTries);
	} else {
		strcat(pCmd, " -q ");
		strcat(pCmd, DEFAULT_TRACE_ROUTE_NUM_TRIES);
	}
	if(strlen(maxHopCount) > 0){
		strcat(pCmd, " -m ");
		strcat(pCmd, maxHopCount);
	} else {
		strcat(pCmd, " -m ");
		strcat(pCmd, DEFAULT_TRACE_ROUTE_MAX_HOP);
	}
	if(strlen(timeout) > 0){
		strcat(pCmd, " -w ");
		strcat(pCmd, timeout);
	} else {
		strcat(pCmd, " -w ");
		strcat(pCmd, DEFAULT_TRACE_ROUTE_TIMEOUT);
	}
	if(strlen(dscp) > 0){
		strcat(pCmd, " -t ");
		strcat(pCmd, dscp);
	} else {
		strcat(pCmd, " -t ");
		strcat(pCmd, DEFAULT_DSCP);
	}
	if(strlen(interface)>0){
		beIntNameGet(interface, ifName, sizeof(ifName));
		strcat(pCmd, " -i ");
		strcat(pCmd, ifName);
	}
	if(strlen(host)>0){
		strcat(pCmd, " ");
		strcat(pCmd, host);
	}
	if(strlen(dataBlockSize) > 0){
		strcat(pCmd, " ");
		strcat(pCmd, dataBlockSize);
	} else {
		strcat(pCmd, " ");
		strcat(pCmd, DEFAULT_TRACE_ROUTE_DATABLOCK_SIZE);
	}
	strcat(pCmd, " -k 1 ");
	snprintf(tmpString, sizeof(tmpString), " &>%s&", diagProcessFile);
	strcat(pCmd, tmpString);
	system(sysCmd);

	USP_LOG_Info("%s() diagResultFile=%s,sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);
	
	//waiting for the diagResultFile file
	while(stat(diagResultFile, &statBuf)){
		sleep(1);
	}
	//prepare output params
	diagResultParams = json_object_from_file(diagResultFile);

exit:
	// Save all results into the output arguments using KV_VECTOR_ functions
	(reqThd->operCompleteArgs) = USP_ARG_Create();

	if(diagResultParams){
		beIntIpAddrGet(interface, ipAddressUsed, sizeof(ipAddressUsed));
		USP_ARG_Add((reqThd->operCompleteArgs), "Status", json_object_get_string(json_object_object_get(diagResultParams,"DiagnosticsState")));
		USP_ARG_Add((reqThd->operCompleteArgs), "IPAddressUsed", ipAddressUsed);
		USP_ARG_Add((reqThd->operCompleteArgs), "ResponseTime", json_object_get_string(json_object_object_get(diagResultParams,"ResponseTime")));
		
		route_hops = json_object_object_get(diagResultParams, "RouteHops");

		count = route_hops ? json_object_array_length(route_hops) : 0;
		
		for(c = 0; c < count; c++) {
			resultHops = json_object_array_get_idx(route_hops, c);
			if(!resultHops) {
				continue;
			}
			snprintf(tmpString, sizeof(tmpString), "RouteHops.%d.", ii++);

			value = json_object_object_get(resultHops, "Host");
			const char *host = json_object_get_string(value);
			snprintf(strBuf, sizeof(strBuf), "%s%s", tmpString, "Host");
			if(host) {
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, host);
			}
			else
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, "");

			snprintf(strBuf, sizeof(strBuf), "%s%s", tmpString, "HostAddress");
			value = json_object_object_get(resultHops, "HostAddress");
			const char *hostAddr = json_object_get_string(value);
			if(hostAddr) {
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, hostAddr);
			}
			else
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, "");

			snprintf(strBuf, sizeof(strBuf), "%s%s", tmpString, "ErrorCode");
			value = json_object_object_get(resultHops, "ErrorCode");
			if(value != NULL)
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, json_object_get_string(value));
			else
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, "");

			snprintf(strBuf, sizeof(strBuf), "%s%s", tmpString, "RTTimes");
			value = json_object_object_get(resultHops, "RTTimes");
			const char *rtt = json_object_get_string(value);
			if(rtt) {
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, rtt);
			}
			else
				USP_ARG_Add((reqThd->operCompleteArgs), strBuf, "0");
		}
	}

	if(err != USP_ERR_OK){
		err_msg = res->err_msg;
	}
	//NOTIFY message with OperationComplete including output params
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, (reqThd->operCompleteArgs));
	reqThd->operCompleteArgs = NULL;

	//free after use
	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}
	
	unlink(symLinkBuf);
	
	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_DownloadDiagnostics_Thread(void *param){//Device.IP.Diagnostics.DownloadDiagnostics()

	int i = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char ipAddressUsed[32] = "";
	char DownloadTransports[10] = "";
	char diagResultFile[256];
	char faultString[256] = {0};
	char ifName[33] = {0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	char thehostname[STRSIZE(SIZE_URL)]={0};
	servInfo_t servInfoPtr;

	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;

	rdm_IpDiagDlDiag_t	 *objIPDiagDlDiag = NULL;
	rdm_IpDiagDlDiag_t obj_IpDiagDlDiag;

	oper_output_res_t result;
	oper_output_res_t *res = &result;

	struct json_object *diagResultParams = NULL;
	struct stat statBuf;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	snprintf(diagResultFile, sizeof(diagResultFile), "%s.dlDiag%d", DIAG_RESULT_PATH, (reqThd->cond)->request_instance);
	remove(diagResultFile);
	
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}
	//set default values
	obj_IpDiagDlDiag.DSCP = DEFAULT_DLDIAG_DSCP;
	obj_IpDiagDlDiag.EthernetPriority = DEFAULT_DLDIAG_ETHERNET_PRIORITY;
	obj_IpDiagDlDiag.TimeBasedTestDuration = DEFAULT_DLDIAG_TIME_BASED_TEST_DURATION;
	obj_IpDiagDlDiag.TimeBasedTestMeasurementInterval = DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_INTERVAL;
	obj_IpDiagDlDiag.TimeBasedTestMeasurementOffset = DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_OFFSET;
	snprintf(obj_IpDiagDlDiag.ProtocolVersion, sizeof(obj_IpDiagDlDiag.ProtocolVersion), "%s", DEFAULT_DLDIAG_PROTOCOL_VERSION);
	obj_IpDiagDlDiag.NumberOfConnections = DEFAULT_DLDIAG_NUM_CONNECTIONS;
	obj_IpDiagDlDiag.EnablePerConnectionResults = DEFAULT_DLDIAG_ENABLE_PER_CONNECTION_RESULTS;

	memset(&obj_IpDiagDlDiag, 0, sizeof(obj_IpDiagDlDiag));
	//get input params
	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);

			if(strcmp(kv->key, "Interface") == 0){
				snprintf(obj_IpDiagDlDiag.Interface, sizeof(obj_IpDiagDlDiag.Interface), "%s", kv->value);
			} 
			else if(strcmp(kv->key, "DownloadURL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				snprintf(obj_IpDiagDlDiag.DownloadURL, sizeof(obj_IpDiagDlDiag.DownloadURL), "%s", kv->value);
			}
			else if(strcmp(kv->key, "DSCP") == 0){
				obj_IpDiagDlDiag.DSCP = atoi(kv->value);
			}
			else if(strcmp(kv->key, "EthernetPriority") == 0){
				obj_IpDiagDlDiag.EthernetPriority = atoi(kv->value);
			}
			else if(strcmp(kv->key, "TimeBasedTestDuration") == 0){
				obj_IpDiagDlDiag.TimeBasedTestDuration = atoi(kv->value);
			}
			else if(strcmp(kv->key, "TimeBasedTestMeasurementInterval") == 0){
				obj_IpDiagDlDiag.TimeBasedTestMeasurementInterval = atoi(kv->value);
			}
			else if(strcmp(kv->key, "TimeBasedTestMeasurementOffset") == 0){
				obj_IpDiagDlDiag.TimeBasedTestMeasurementOffset = atoi(kv->value);
			}
			else if(strcmp(kv->key, "ProtocolVersion") == 0){
				snprintf(obj_IpDiagDlDiag.ProtocolVersion, sizeof(obj_IpDiagDlDiag.ProtocolVersion), "%s", kv->value);
			}
			else if(strcmp(kv->key, "NumberOfConnections") == 0){
				obj_IpDiagDlDiag.NumberOfConnections = atoi(kv->value);
			}
			else if(strcmp(kv->key, "EnablePerConnectionResults") == 0){
				obj_IpDiagDlDiag.EnablePerConnectionResults = (strcmp(kv->value, true) == 0)?true:false;
			}
		}// end of for
	}// end of if((reqThd->cond)->input_args->num_entries) {

	if(strlen(obj_IpDiagDlDiag.DownloadURL) == 0){
		USP_LOG_Info("%s(%d) DownloadURL is empty.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: DownloadURL is empty.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: DownloadURL is empty.", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	//create symbolic link to identify request_instance (Device.LocalAgent.Request.i.)
	char symLinkBuf[64];
	struct stat st = {0};
	
	//create complete ping command with command options
	//Ex. /usr/bin/wget http://124.219.31.142:8080/updatelist.txt -P /tmp -i wwan0 -d 0 -M 0 &
	char sysCmd[256+64] = {0};
	char *pCmd = sysCmd;
	char strTmp[16] = {0};
	char localFilePath[256] = {0};

	if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}
	cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
	printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

	if(strstr(obj_IpDiagDlDiag.DownloadURL, "http://") != NULL){
		snprintf(DownloadTransports, sizeof(DownloadTransports), "HTTP"); 
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);
		if (stat(symLinkBuf, &st) == -1) {
			mkdir(symLinkBuf, 0700);
		}
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/wget", PATH_REQUEST, (reqThd->cond)->request_instance);
		USP_LOG_Info("%s(%d) symLinkBuf=%s\n", __FUNCTION__, __LINE__, symLinkBuf);
		symlink(PATH_PING, symLinkBuf);

		snprintf(sysCmd, sizeof(sysCmd), "%s %s -P %s", symLinkBuf, obj_IpDiagDlDiag.DownloadURL, DEFAULT_DLDIAG_DOWNLOAD_DIRECTORY);

		if(strlen(obj_IpDiagDlDiag.Interface)>0){
			beIntNameGet(obj_IpDiagDlDiag.Interface, ifName, sizeof(ifName));
			strcat(pCmd, " -i ");
			strcat(pCmd, ifName);
		}

		if(obj_IpDiagDlDiag.DSCP >= 0){
		strcat(pCmd, " -d ");
			snprintf(strTmp, sizeof(strTmp), "%d", obj_IpDiagDlDiag.DSCP);
			strcat(pCmd, strTmp);
		}

		if(obj_IpDiagDlDiag.EthernetPriority >= 0){
		strcat(pCmd, " -M ");
			snprintf(strTmp, sizeof(strTmp), "%d", obj_IpDiagDlDiag.EthernetPriority);
			strcat(pCmd, strTmp);
		}

		snprintf(strTmp, sizeof(strTmp), "%d", (reqThd->cond)->request_instance);
		strcat(pCmd, " -N ");
		strcat(pCmd, strTmp);
		strcat(pCmd, " & ");
	}
	else if(strstr(obj_IpDiagDlDiag.DownloadURL, "ftp://") != NULL){
		snprintf(DownloadTransports, sizeof(DownloadTransports), "FTP");
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);
		if (stat(symLinkBuf, &st) == -1) {
			mkdir(symLinkBuf, 0700);
		}
		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/ftpget", PATH_REQUEST, (reqThd->cond)->request_instance);
		USP_LOG_Info("%s(%d) symLinkBuf=%s\n", __FUNCTION__, __LINE__, symLinkBuf);
		symlink(PATH_PING, symLinkBuf);


		snprintf(sysCmd, sizeof(sysCmd), "%s", symLinkBuf);

		if(strlen(obj_IpDiagDlDiag.Interface)>0){
			beIntNameGet(obj_IpDiagDlDiag.Interface, ifName, sizeof(ifName));
			strcat(pCmd, " -n ");
			strcat(pCmd, ifName);
		}

		snprintf(strTmp, sizeof(strTmp), "%d", (reqThd->cond)->request_instance);
		strcat(pCmd, " -N ");
		strcat(pCmd, strTmp);


		strcat(pCmd, " ");
		strcat(pCmd, ip_addr);
		strcat(pCmd, " ");
		snprintf(localFilePath, sizeof(localFilePath), "%s%s", DEFAULT_DLDIAG_DOWNLOAD_DIRECTORY, strrchr(servInfoPtr.path, '/'));
		strcat(pCmd, localFilePath);

		strcat(pCmd, " ");
		snprintf(strTmp, sizeof(strTmp), "%s", servInfoPtr.path + 1);
		strcat(pCmd, strTmp);
	
		strcat(pCmd, " & ");
	}
	else{
		USP_LOG_Error("%s: Error: The protocol is not supported.\nURL:%s\n", __FUNCTION__, obj_IpDiagDlDiag.DownloadURL);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: The protocol is not supported.\nURL:%s\n", __FUNCTION__, obj_IpDiagDlDiag.DownloadURL);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: The protocol is not suported.\nURL:%s\n", __FUNCTION__, obj_IpDiagDlDiag.DownloadURL);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	USP_LOG_Info("%s() diagResultFile=%s,sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);
	//Marcus for test
	system(sysCmd);
	//waiting for the diagResultFile file
	while(stat(diagResultFile, &statBuf)){
		sleep(1);
	}
	//prepare output params
	diagResultParams = json_object_from_file(diagResultFile);

exit:
	// Save all results into the output arguments using KV_VECTOR_ functions
	(reqThd->operCompleteArgs) = USP_ARG_Create();

	if(diagResultParams){
		beIntIpAddrGet(obj_IpDiagDlDiag.Interface, ipAddressUsed, sizeof(ipAddressUsed));

		USP_ARG_Add((reqThd->operCompleteArgs), "DownloadTransports", DownloadTransports);
		USP_ARG_Add((reqThd->operCompleteArgs), "Status", json_object_get_string(json_object_object_get(diagResultParams,"DiagnosticsState")));
		USP_ARG_Add((reqThd->operCompleteArgs), "IPAddressUsed", ipAddressUsed);

		USP_ARG_Add((reqThd->operCompleteArgs), "ROMTime", json_object_get_string(json_object_object_get(diagResultParams,"ROMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "BOMTime", json_object_get_string(json_object_object_get(diagResultParams,"BOMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "EOMTime", json_object_get_string(json_object_object_get(diagResultParams,"EOMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TotalBytesReceived", json_object_get_string(json_object_object_get(diagResultParams,"TotalBytesReceived")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TestBytesReceived", json_object_get_string(json_object_object_get(diagResultParams,"TestBytesReceived")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TotalBytesSent", json_object_get_string(json_object_object_get(diagResultParams,"TotalBytesSent")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TCPOpenRequestTime", json_object_get_string(json_object_object_get(diagResultParams,"TCPOpenRequestTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TCPOpenResponseTime", json_object_get_string(json_object_object_get(diagResultParams,"TCPOpenResponseTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "X_ZYXEL_Latency", json_object_get_string(json_object_object_get(diagResultParams,"X_ZYXEL_Latency")));
		USP_ARG_Add((reqThd->operCompleteArgs), "X_ZYXEL_RoundTripTime", json_object_get_string(json_object_object_get(diagResultParams,"X_ZYXEL_RoundTripTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "X_ZYXEL_DownloadSpeed", json_object_get_string(json_object_object_get(diagResultParams,"X_ZYXEL_DownloadSpeed")));
	}

	if(err != USP_ERR_OK){
		err_msg = res->err_msg;
	}

	//NOTIFY message with OperationComplete including output params
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, (reqThd->operCompleteArgs));
	reqThd->operCompleteArgs = NULL;

	//free after use
	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}
	
	if(strlen(localFilePath) > 0){
		unlink(localFilePath);
	}
	unlink(symLinkBuf);
	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}


void *AsyncOperation_UploadDiagnostics_Thread(void *param) { // Device.IP.Diagnostics.UploadDiagnostics()

	int i = 0;

	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;

	char ipAddressUsed[32] = "";
	char UploadTransports[10] = "";
	char diagResultFile[256];
	char faultString[256] = {0};
	char ifName[33] = {0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	char thehostname[STRSIZE(SIZE_URL)] = {0};
	servInfo_t servInfoPtr;

	char *urlStr = NULL;
	int diagTcpMethod = DIAG_TCP_HTTP;

	uploadDiagParmData_t *uploadDiagParams = NULL;

	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;

	rdm_IpDiagUlDiag_t *objIPDiagUlDiag = NULL;
	rdm_IpDiagUlDiag_t obj_IpDiagUlDiag;

	oper_output_res_t result;
	oper_output_res_t *res = &result;

	struct json_object *diagResultParams = NULL;
	struct stat statBuf;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();

	memset(&result, 0, sizeof(result));
	snprintf(diagResultFile, sizeof(diagResultFile), "%s.ulDiag%d", DIAG_RESULT_PATH, (reqThd->cond)->request_instance);
	remove(diagResultFile);

	memset(&servInfoPtr, 0, sizeof(servInfoPtr));

	uploadDiagParams = (uploadDiagParmData_t *)malloc(sizeof(uploadDiagParmData_t));
	memset(uploadDiagParams, 0, sizeof(uploadDiagParmData_t));

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}

	//set default values
	obj_IpDiagUlDiag.DSCP = DEFAULT_DLDIAG_DSCP;
	obj_IpDiagUlDiag.EthernetPriority = DEFAULT_DLDIAG_ETHERNET_PRIORITY;
#if 0
	obj_IpDiagUlDiag.TimeBasedTestDuration = DEFAULT_DLDIAG_TIME_BASED_TEST_DURATION;
	obj_IpDiagUlDiag.TimeBasedTestMeasurementInterval = DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_INTERVAL;
	obj_IpDiagUlDiag.TimeBasedTestMeasurementOffset = DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_OFFSET;

	snprintf(obj_IpDiagUlDiag.ProtocolVersion, sizeof(obj_IpDiagUlDiag.ProtocolVersion), "%s", DEFAULT_DLDIAG_PROTOCOL_VERSION);
	obj_IpDiagUlDiag.NumberOfConnections = DEFAULT_DLDIAG_NUM_CONNECTIONS;
	obj_IpDiagUlDiag.EnablePerConnectionResults = DEFAULT_DLDIAG_ENABLE_PER_CONNECTION_RESULTS;
#endif
	memset(&obj_IpDiagUlDiag, 0, sizeof(obj_IpDiagUlDiag));

	// get input params
	if ( (reqThd->cond)->input_args->num_entries ) {
		for ( i = 0; i < (reqThd->cond)->input_args->num_entries; ++i ) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|", __func__, __LINE__, kv->key, kv->value);

			if ( strcmp(kv->key, "Interface") == 0 ) {
				snprintf(obj_IpDiagUlDiag.Interface, sizeof(obj_IpDiagUlDiag.Interface), "%s", kv->value);
			}
			else if ( strcmp(kv->key, "UploadURL") == 0 ) {
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				snprintf(obj_IpDiagUlDiag.UploadURL, sizeof(obj_IpDiagUlDiag.UploadURL), "%s", kv->value);
			}
			else if ( strcmp(kv->key, "DSCP") == 0 ) {
				obj_IpDiagUlDiag.DSCP = atoi(kv->value);
			}
			else if ( strcmp(kv->key, "EthernetPriority") == 0 ) {
				obj_IpDiagUlDiag.EthernetPriority = atoi(kv->value);
			}
			else if ( strcmp(kv->key, "TestFileLength") == 0 ) {
				obj_IpDiagUlDiag.TestFileLength = atoi(kv->value);
			}
#if 0
			else if (strcmp(kv->key, "TimeBasedTestDuration") == 0) {
				obj_IpDiagUlDiag.TimeBasedTestDuration = atoi(kv->value);
			}
			else if (strcmp(kv->key, "TimeBasedTestMeasurementInterval") == 0) {
				obj_IpDiagUlDiag.TimeBasedTestMeasurementInterval = atoi(kv->value);
			}
			else if (strcmp(kv->key, "TimeBasedTestMeasurementOffset") == 0) {
				obj_IpDiagUlDiag.TimeBasedTestMeasurementOffset = atoi(kv->value);
			}
			else if (strcmp(kv->key, "ProtocolVersion") == 0) {
				snprintf(obj_IpDiagUlDiag.ProtocolVersion, sizeof(obj_IpDiagUlDiag.ProtocolVersion), "%s", kv->value);
			}
			else if (strcmp(kv->key, "NumberOfConnections") == 0) {
				obj_IpDiagUlDiag.NumberOfConnections = atoi(kv->value);
			}
			else if (strcmp(kv->key, "EnablePerConnectionResults") == 0) {
				obj_IpDiagUlDiag.EnablePerConnectionResults = (strcmp(kv->value, true) == 0) ? true : false;
			}
#endif
		} // end of for
	} // end of if((reqThd->cond)->input_args->num_entries) {

	if ( strlen(obj_IpDiagUlDiag.Interface) == 0 ) {
		strcpy(uploadDiagParams->intfaddr, IPADDR_ANY);

		if ( zyUtilRetrieveSystemDefaultRoutIface(uploadDiagParams->interfaceName) != true ) {
			USP_LOG_Info("%s: no specific IP.Interface and no default route!\n", __FUNCTION__);
		}
	}

	if ( strlen(obj_IpDiagUlDiag.UploadURL) == 0 ) {
		USP_LOG_Info("%s(%d) UploadURL is empty.\n", __func__, __LINE__);

		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: UploadURL is empty.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: UploadURL is empty.", __FUNCTION__);

		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;

		goto exit;
	}

#if 1
	if ( (urlStr = strstr(obj_IpDiagUlDiag.UploadURL, "http://")) ) {
		urlStr += strlen("http://");
		diagTcpMethod = DIAG_TCP_HTTP;
		snprintf(UploadTransports, sizeof(UploadTransports), "HTTP");
	} else if ( (urlStr = strstr(obj_IpDiagUlDiag.UploadURL, "ftp://")) ) {
		urlStr += strlen("ftp://");
		diagTcpMethod = DIAG_TCP_FTP;
		snprintf(UploadTransports, sizeof(UploadTransports), "FTP");
	} else {
		// goto exit;
	}

	if ( (err = beParseUrlParams(
					urlStr,  // obj_IpDiagUlDiag.UploadURL
					uploadDiagParams->username,
					uploadDiagParams->password,
					uploadDiagParams->host,
					uploadDiagParams->port,
					uploadDiagParams->dir)) != USP_ERR_OK ) {

		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: beParseUrlParams(%d) failed", __FUNCTION__, err);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: beParseUrlParams(%d) failed", __FUNCTION__, err);

		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}

	printf( "%s(%d)[beParseUrlParams] input url=%s\n"
			"parser result:\n"
			"username=%s\n"
			"password=%s\n"
			"host=%s, port=%s\n"
			"dir=%s\n",
			__func__, __LINE__,
			obj_IpDiagUlDiag.UploadURL,
			uploadDiagParams->username,
			uploadDiagParams->password,
			uploadDiagParams->host,
			uploadDiagParams->port,
			uploadDiagParams->dir );
#endif

	// create symbolic link to identify request_instance (Device.LocalAgent.Request.i.)
	char symLinkBuf[64];
	struct stat st = {0};

	// create complete command with command options
	//Ex. /bin/zhttpput wwan0 0.0.0.0 http://124.219.31.142/fileUpload 10000000 0 0 &
	char sysCmd[256+64] = {0};

	char localFilePath[256] = {0};


	if ( strstr(obj_IpDiagUlDiag.UploadURL, "http://") != NULL ) {

		if ( (err = cwmp_parseurl(
						servInfoPtr.url,
						thehostname,
						(struct sockaddr *)&(servInfoPtr.ipAddress),
						&servInfoPtr.port, servInfoPtr.path,
						&servInfoPtr.isSecure,
						servInfoPtr.hostname,
						eMULTIP_CHOOSE_REGULAR,
						"DualStack")) != USP_ERR_OK ) {

			USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
			USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);

			faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}

		cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);

		printf( "%s(%d)[cwmp_parseurl] input url=%s\n"
				"parser result hostname=%s, ip=%s, port=%d\n"
				"path=%s\n",
				__func__, __LINE__,
				servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port,
				servInfoPtr.path );


		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);

		if ( stat(symLinkBuf, &st) == -1 ) {
			mkdir(symLinkBuf, 0700);
		}

		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/zhttpput", PATH_REQUEST, (reqThd->cond)->request_instance);

		USP_LOG_Info("%s(%d) symLinkBuf=%s\n", __FUNCTION__, __LINE__, symLinkBuf);
		symlink(PATH_ZHTTPPUT, symLinkBuf);

		// TODO: zhttpput wwan0 0.0.0.0 http://124.219.31.142/uploadContent 100000000 0 0 &
#ifdef ZYXEL_TR143_DIAG_ENHANCEMENT
		// system("killall httpdiag 2>/dev/null");

		if ( !strlen(uploadDiagParams->port) )
			strcpy(uploadDiagParams->port, DEFAULT_HTTP_PORT);

		sprintf(sysCmd, "nice -n -10 httpdiag -a %s -p %s -i %s -s up -u /%s -l %d -o 30 -t 5 &",
			uploadDiagParams->host,
			uploadDiagParams->port,
			uploadDiagParams->interfaceName,
			uploadDiagParams->dir,
			obj_IpDiagUlDiag.TestFileLength);
#else
		sprintf( sysCmd, "%s %s %s %s %d %d %d %d &",
			symLinkBuf,
			uploadDiagParams->interfaceName,
			IPADDR_ANY,
			obj_IpDiagUlDiag.UploadURL,
			obj_IpDiagUlDiag.TestFileLength,
			obj_IpDiagUlDiag.DSCP,
			obj_IpDiagUlDiag.EthernetPriority,
			(reqThd->cond)->request_instance );
#endif
	}
	else if ( strstr(obj_IpDiagUlDiag.UploadURL, "ftp://") != NULL ) {

		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d", PATH_REQUEST, (reqThd->cond)->request_instance);
		if (stat(symLinkBuf, &st) == -1) {
			mkdir(symLinkBuf, 0700);
		}

		snprintf(symLinkBuf, sizeof(symLinkBuf), "%s%d/ftpput", PATH_REQUEST, (reqThd->cond)->request_instance);
		USP_LOG_Info("%s(%d) symLinkBuf=%s\n", __FUNCTION__, __LINE__, symLinkBuf);

		symlink(PATH_FTPPUT, symLinkBuf);


		if (!strlen(uploadDiagParams->port))
			strcpy(uploadDiagParams->port, DEFAULT_FTP_PORT);

		sprintf(sysCmd, "%s -u %s -p %s -n %s -l %s -m %d -d %d -r %d -P %s -N %d %s %s - &",
			symLinkBuf,
			uploadDiagParams->username, uploadDiagParams->password,
			uploadDiagParams->interfaceName,
			obj_IpDiagUlDiag.UploadURL,
			obj_IpDiagUlDiag.TestFileLength,
			obj_IpDiagUlDiag.DSCP,
			obj_IpDiagUlDiag.EthernetPriority,
			uploadDiagParams->port,
			(reqThd->cond)->request_instance,
			uploadDiagParams->host,
			uploadDiagParams->dir);
	}
	else {
		USP_LOG_Error("%s: Error: The protocol is not supported.\nURL: %s\n", __FUNCTION__, obj_IpDiagUlDiag.UploadURL);

		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: The protocol is not supported.\nURL: %s\n", __FUNCTION__, obj_IpDiagUlDiag.UploadURL);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: The protocol is not suported.\nURL: %s\n", __FUNCTION__, obj_IpDiagUlDiag.UploadURL);

		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;

		goto exit;
	}

	USP_LOG_Info("%s() diagResultFile=%s, sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);

	// Execute the command
	system(sysCmd);

	// waiting for the diagResultFile file
	while ( stat(diagResultFile, &statBuf) ) {
		sleep(1);
	}

	// prepare output params
	diagResultParams = json_object_from_file(diagResultFile);

exit:
	// Save all results into the output arguments using KV_VECTOR_ functions
	(reqThd->operCompleteArgs) = USP_ARG_Create();

	if ( diagResultParams ) {
		beIntIpAddrGet(obj_IpDiagUlDiag.Interface, ipAddressUsed, sizeof(ipAddressUsed));

		USP_ARG_Add((reqThd->operCompleteArgs), "UploadTransports", UploadTransports);
		USP_ARG_Add((reqThd->operCompleteArgs), "Status", json_object_get_string(json_object_object_get(diagResultParams,"DiagnosticsState")));
		USP_ARG_Add((reqThd->operCompleteArgs), "IPAddressUsed", ipAddressUsed);

		USP_ARG_Add((reqThd->operCompleteArgs), "ROMTime", json_object_get_string(json_object_object_get(diagResultParams,"ROMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "BOMTime", json_object_get_string(json_object_object_get(diagResultParams,"BOMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "EOMTime", json_object_get_string(json_object_object_get(diagResultParams,"EOMTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TotalBytesReceived", json_object_get_string(json_object_object_get(diagResultParams,"TotalBytesReceived")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TestBytesSent", json_object_get_string(json_object_object_get(diagResultParams,"TestBytesSent")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TotalBytesSent", json_object_get_string(json_object_object_get(diagResultParams,"TotalBytesSent")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TCPOpenRequestTime", json_object_get_string(json_object_object_get(diagResultParams,"TCPOpenRequestTime")));
		USP_ARG_Add((reqThd->operCompleteArgs), "TCPOpenResponseTime", json_object_get_string(json_object_object_get(diagResultParams,"TCPOpenResponseTime")));
	}

	if ( err != USP_ERR_OK ) {
		err_msg = res->err_msg;
	}

	// NOTIFY message with OperationComplete including output params
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, (reqThd->operCompleteArgs));
	reqThd->operCompleteArgs = NULL;

	// free after use
	if ( (reqThd->cond) != NULL ) {
		if ( (reqThd->cond)->input_args != NULL ) {
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}

		USP_FREE((reqThd->cond));
		(reqThd->cond) = NULL;
	}

	if ( strlen(localFilePath) > 0 ) {
		unlink(localFilePath);
	}

	unlink(symLinkBuf);

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_Config_Restore_Thread(void *param){//Device.DeviceInfo.VendorConfigFile.{i}.Restore()
	int i = 0, j = 0, ret = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "Download");

	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);
	addTransferCompleteParam(reqThd);
	
	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		    USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
	}

	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "URL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					faultCode = USP_ERR_INTERNAL_ERROR;
					goto exit;
				}
				cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);
			}
		}
	}

	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	//Get the file via http get
	if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, LOCAL_CONFIG_TMP_PATH)){
		printf("%s(%d) Get file %s successfully, now is going to upgrade the system..\n", __func__, __LINE__, LOCAL_CONFIG_TMP_PATH);

		if((ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) != 0){
			USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_DISABLE).\n", __func__, __LINE__, ret);
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}

		if(zcfgFeRestoreDefault(LOCAL_CONFIG_TMP_PATH) == ZCFG_SUCCESS){
			USP_LOG_Info("%s(%d) zcfgFeRestoreDefault() return ZCFG_SUCCESS.\n", __func__, __LINE__);
			USP_LOG_Info("%s(%d) now is going to reboot...\n", __func__, __LINE__);
		}
		else{//fail to restore
			USP_LOG_Info("%s(%d) zcfgFeRestoreDefault() return ZCFG_FAIL.\n", __func__, __LINE__);
			USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to upgrade.", __FUNCTION__);
			USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to upgrade.", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	}
	else{//fail to download file
		USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, LOCAL_CONFIG_TMP_PATH);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

exit:
	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);
	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);

	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);
	
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_Config_Backup_Thread(void *param){//Device.DeviceInfo.VendorConfigFile.{i}.Backup()
	int i = 0, ret = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char username[DATETIME_LENGTH] = {0};
	char password[DATETIME_LENGTH] = {0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;
	tr369ConnCB_t *connCbPtr = NULL;
	tr369SessCB_t *sessionCb = NULL;
	transferInfo_t  *transferInfo = NULL;
	rpcUpload_t *rpcUpload = NULL;

	connCbPtr = cwmp_conn_new();

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "Upload");

	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);
	addTransferCompleteParam(reqThd);

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		    USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
	}

	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "URL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					faultCode = USP_ERR_COMMAND_FAILURE;
					goto exit;
				}
				cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);
			} else if(strcmp(kv->key, "Username") == 0){
				strncpy(username, kv->value, sizeof(username));
				strncpy(servInfoPtr.username, kv->value, sizeof(servInfoPtr.username));
			} else if(strcmp(kv->key, "Password") == 0){
				strncpy(password, kv->value, sizeof(password));
				strncpy(servInfoPtr.password, kv->value, sizeof(servInfoPtr.password));
			}
		}
	}

	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		faultCode = USP_ERR_COMMAND_FAILURE;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	if(zcfgFeConfBackup(NULL) == ZCFG_SUCCESS){
		USP_LOG_Info("%s(%d) zcfgFeConfBackup() return ZCFG_SUCCESS.\n", __func__, __LINE__);
		if(connCbPtr){
			cwmp_conn_set_client(connCbPtr, &servInfoPtr, CG_HTTP_PUT);
			/*create a session*/
			connCbPtr->sessionCb = cwmp_sess_new();
			sessionCb = connCbPtr->sessionCb;
			/*assign download information*/
			transferInfo = cwmp_transfer_new();

			rpcUpload = (rpcUpload_t*) malloc(sizeof(rpcUpload_t));
			if(rpcUpload){
				memset( rpcUpload, 0, sizeof(rpcUpload_t));
				snprintf(rpcUpload->url, sizeof(rpcUpload->url), "%s", servInfoPtr.url);
				snprintf(rpcUpload->fileType, sizeof(rpcUpload->fileType), "%s", UPLOAD_TYPE_CFG);
				snprintf(rpcUpload->username, sizeof(rpcUpload->username), "%s", username);
				snprintf(rpcUpload->password, sizeof(rpcUpload->password), "%s", password);
			}
			transferInfo->rpcUpload = rpcUpload;
			sessionCb->transferInfo = transferInfo;
			if( cwmp_sess_file_proc(connCbPtr) != USP_ERR_OK){
				USP_LOG_Info(  "cwmp_sess_file_proc fail!");
				USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "Error: fail to backup config to server.");
				USP_SNPRINTF(faultString, sizeof(faultString), "Error: fail to backup config to server.");
				faultCode = USP_ERR_COMMAND_FAILURE;
				err = USP_ERR_COMMAND_FAILURE;
			}
		}
	}
	else{//fail to backup
		USP_LOG_Info("%s(%d) zcfgFeConfBackup() return ZCFG_FAIL.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to backup config.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to backup config.", __FUNCTION__);
		faultCode = USP_ERR_COMMAND_FAILURE;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

exit:
	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);
	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);

	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_Log_Upload_Thread(void *param) { // Device.DeviceInfo.VendorLogFile.{i}.Upload()
	int i = 0, ret = 0;

	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};

	char thehostname[STRSIZE(SIZE_URL)] = {0};
	char username[DATETIME_LENGTH] = {0};
	char password[DATETIME_LENGTH] = {0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	char log_file_index[4] = "";

	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;
	tr369ConnCB_t *connCbPtr = NULL;
	tr369SessCB_t *sessionCb = NULL;
	transferInfo_t *transferInfo = NULL;
	rpcUpload_t *rpcUpload = NULL;

	connCbPtr = cwmp_conn_new();

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();

	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));

	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "Upload");

	if ( getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK ) {
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);

	addTransferCompleteParam(reqThd);

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if ( err != USP_ERR_OK )
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);

		faultCode = USP_ERR_INTERNAL_ERROR;

		goto exit;
	}

	if ( (reqThd->cond)->input_args->num_entries ) {
		for ( i = 0; i < (reqThd->cond)->input_args->num_entries; ++i ) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];

			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|", __func__, __LINE__, kv->key, kv->value);

			if ( strcmp(kv->key, "URL") == 0 ) {
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));

				if ( (err = cwmp_parseurl(
								servInfoPtr.url,
								thehostname,
								(struct sockaddr *) &(servInfoPtr.ipAddress),
								&servInfoPtr.port,
								servInfoPtr.path,
								&servInfoPtr.isSecure,
								servInfoPtr.hostname,
								eMULTIP_CHOOSE_REGULAR,
								"DualStack")) != USP_ERR_OK ) {

					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);

					faultCode = USP_ERR_COMMAND_FAILURE;

					goto exit;
				}

				cwmp_ip_ip2string( &(servInfoPtr.ipAddress), ip_addr );
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n",
						__func__, __LINE__,
						servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path);

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);

			} else if ( strcmp(kv->key, "Username") == 0 ) {
				strncpy(username, kv->value, sizeof(username));
				strncpy(servInfoPtr.username, kv->value, sizeof(servInfoPtr.username));

			} else if ( strcmp(kv->key, "Password") == 0 ) {
				strncpy(password, kv->value, sizeof(password));
				strncpy(servInfoPtr.password, kv->value, sizeof(servInfoPtr.password));
			}
		}
	}

	//check mandatory arguments
	if ( strlen(servInfoPtr.url) == 0 ) {
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);

		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);

		faultCode = USP_ERR_COMMAND_FAILURE;
		err = USP_ERR_COMMAND_FAILURE;

		goto exit;
	}

	// do upload log
	if ( connCbPtr ) {
		cwmp_conn_set_client(connCbPtr, &servInfoPtr, CG_HTTP_PUT);

		/*create a session*/
		connCbPtr->sessionCb = cwmp_sess_new();
		sessionCb = connCbPtr->sessionCb;

		/*assign download information*/
		transferInfo = cwmp_transfer_new();

		rpcUpload = (rpcUpload_t*) malloc(sizeof(rpcUpload_t));
		if ( rpcUpload ) {
			memset( rpcUpload, 0, sizeof(rpcUpload_t));

			snprintf(rpcUpload->url, sizeof(rpcUpload->url), "%s", servInfoPtr.url);

			// snprintf(rpcUpload->fileType, sizeof(rpcUpload->fileType), "%s", UPLOAD_TYPE_LOG);
			sscanf( (reqThd->cond)->parentPath, "Device.DeviceInfo.VendorLogFile.%s.", log_file_index );
			printf("[YuChih Debug] log_file_index = %s \n", log_file_index);
			snprintf(rpcUpload->fileType, sizeof(rpcUpload->fileType), "%s %s", UPLOAD_TYPE_LOG, log_file_index);

			snprintf(rpcUpload->username, sizeof(rpcUpload->username), "%s", username);
			snprintf(rpcUpload->password, sizeof(rpcUpload->password), "%s", password);
		}

		transferInfo->rpcUpload = rpcUpload;
		sessionCb->transferInfo = transferInfo;

		if (cwmp_sess_file_proc(connCbPtr) != USP_ERR_OK) {
			USP_LOG_Info("cwmp_sess_file_proc fail!");

			USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "Error: fail to upload log to server.");
			USP_SNPRINTF(faultString, sizeof(faultString), "Error: fail to upload log to server.");

			faultCode = USP_ERR_COMMAND_FAILURE;
			err = USP_ERR_COMMAND_FAILURE;
		}
	}


exit:
	if ( getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK ) {
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);

	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);

	if ( err == USP_ERR_OK ) {
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else {
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	if ( (reqThd->cond) != NULL ) {
		if ( (reqThd->cond)->input_args != NULL ) {
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}

		USP_FREE((reqThd->cond));
		(reqThd->cond) = NULL;
	}

	if ( (ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0 ) {
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

#if 1//Jessie InstallDU()
//------------------------------------------------------------------------------
// Location of the request object within the data model
#define DEVICE_SW_DU_ROOT "Device.SoftwareModules.DeploymentUnit"
char *device_swdu_root = DEVICE_SW_DU_ROOT;
#define DEVICE_SW_DU_ROOT_WITH_DOT "Device.SoftwareModules.DeploymentUnit."
char *device_swdu_root_with_dot = DEVICE_SW_DU_ROOT_WITH_DOT;
/*********************************************************************//**
**
** DEVICE_SWMODULES_DU_Add
**
** Adds a new instance to the Device.SoftwareModules.DeploymentUnit.{i}. table
**
** \param   instance - pointer to instance number to be returned
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_SWMODULES_DU_Add(int *instance)
{
    int err;
    char param[MAX_DM_PATH];

	//Only use zcfg API in non-main thread
    err = zcfg_add_object(device_swdu_root, instance);//DATA_MODEL_AddInstance() will show warning message "WARNING: Calling USP_DM_RefreshInstance from non-data model thread" and return.

    if (err != USP_ERR_OK)
    {
		USP_LOG_Error("%s() Add %s.%d. fail.", __FUNCTION__, device_swdu_root, *instance);
        return err;
    }

	//instance value is decided after zcfg_add_object
	USP_SNPRINTF(param, MAX_DM_PATH, "%s%d", DEVICE_SW_DU_ROOT_WITH_DOT, *instance);
	USP_SIGNAL_ObjectAdded(param);

	USP_LOG_Info("%s() %s.%d. is added.", __FUNCTION__, device_swdu_root, *instance);

    return USP_ERR_OK;
}

int DEVICE_SWMODULES_DU_Del(int instance)
{
    int err;
    char param[MAX_DM_PATH];

    USP_SNPRINTF(param, sizeof(param), "%s.%d", device_swdu_root, instance);
	//Only use zcfg API in non-main thread
    err = zcfg_del_object(param);//DATA_MODEL_DeleteInstance() will show warning message "WARNING: Calling USP_DM_RefreshInstance from non-data model thread" and return.

	if (err != USP_ERR_OK)
    {
        USP_LOG_Error("%s: Unable to delete %s.", __FUNCTION__, param);
        return err;
    } else USP_LOG_Info("%s() %s is deleted.", __FUNCTION__, param);

	USP_SIGNAL_ObjectDeleted(param);

    return USP_ERR_OK;
}

//------------------------------------------------------------------------------
// Location of the request object within the data model
#define DEVICE_SW_EU_ROOT "Device.SoftwareModules.ExecutionUnit"
char *device_sweu_root = DEVICE_SW_EU_ROOT;
#define DEVICE_SW_EU_ROOT_WITH_DOT "Device.SoftwareModules.ExecutionUnit."
char *device_sweu_root_with_dot = DEVICE_SW_EU_ROOT_WITH_DOT;
int DEVICE_SWMODULES_EU_Add(int *instance)
{
    int err;
    char param[MAX_DM_PATH];

	//Only use zcfg API in non-main thread
    err = zcfg_add_object(device_sweu_root, instance);//DATA_MODEL_AddInstance() will show warning message "WARNING: Calling USP_DM_RefreshInstance from non-data model thread" and return.

    if (err != USP_ERR_OK)
    {
		USP_LOG_Error("%s() Add %s.%d. fail.", __FUNCTION__, device_sweu_root, *instance);
        return err;
    }

	//instance value is decided after zcfg_add_object
	USP_SNPRINTF(param, MAX_DM_PATH, "%s%d", DEVICE_SW_EU_ROOT_WITH_DOT, *instance);
	USP_SIGNAL_ObjectAdded(param);

	USP_LOG_Info("%s() %s.%d. is added.", __FUNCTION__, device_sweu_root, *instance);

    return USP_ERR_OK;
}

int DEVICE_SWMODULES_EU_Del(int instance)
{
	int err;
	char param[MAX_DM_PATH];

	USP_SNPRINTF(param, sizeof(param), "%s.%d", device_sweu_root, instance);
	//Only use zcfg API in non-main thread
	err = zcfg_del_object(param);//DATA_MODEL_DeleteInstance() will show warning message "WARNING: Calling USP_DM_RefreshInstance from non-data model thread" and return.

	if (err != USP_ERR_OK)
    {
        USP_LOG_Error("%s: Unable to delete %s.", __FUNCTION__, param);
        return err;
    } else USP_LOG_Info("%s() %s is deleted.", __FUNCTION__, param);

	USP_SIGNAL_ObjectDeleted(param);

    return USP_ERR_OK;
}

void *AsyncOperation_InstallDU_Thread(void *param) { // Device.SoftwareModules.InstallDU()
	int i = 0, j = 0, ret = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;
	#if 1//Jessie InstallDU()
	int du_instance;
	int eu_instance;
	int fd;
	int duStateChange_faultCode = USP_ERR_OK;
	char duFile[256];
	char fileName[128];
	char duStateChange_faultString[256] = {0}, duStateChange_faultCodeToStr[8] = {0};
	char unitRefPath[64] = {0};
	char UUID[64] ={0};
	char valueSetPath[256] = {0};
	kv_vector_t *DUStateChangeEvent;
	#endif



	DUStateChangeEvent = USP_ARG_Create();

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "InstallDU");

	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);
	USP_ARG_Add(DUStateChangeEvent, "OperationPerformed", "Install");
	USP_ARG_Add(DUStateChangeEvent, "StartTime", currentTime);
	addTransferCompleteParam(reqThd);

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		    USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
	}

	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "URL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					faultCode = USP_ERR_INTERNAL_ERROR;
					duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
					goto exit;
				}
				cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);
			}
			else if(strcmp(kv->key, "UUID") == 0){
				USP_SNPRINTF(UUID, sizeof(UUID), "%s", kv->value);
			}
		}
	}

	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	if(strlen(UUID) == 0){
		zyUtilityGenerateUuid(UUID);
	}

	//create a random download filename
	fd = -1;
	snprintf(fileName, sizeof(fileName), "%s%s", DU_PATH, "XXXXXX");
	fd = mkstemp(fileName);
	USP_LOG_Info("fileName %s\n", fileName);
	if( fd != -1){
		USP_LOG_Info("close file\n");
		close(fd);
	}

	//Get the file via http get
	if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, fileName)){
		printf("%s(%d) Get file %s successfully.\n", __func__, __LINE__, fileName);

		if((ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) != 0){
			USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_DISABLE).\n", __func__, __LINE__, ret);
			faultCode = USP_ERR_INTERNAL_ERROR;
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	}
	else{//fail to download file
		USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, fileName);
		unlink(fileName);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		faultCode = USP_ERR_INTERNAL_ERROR;
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	if(DEVICE_SWMODULES_DU_Add(&du_instance) != USP_ERR_OK){
		USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
		USP_LOG_Error("%s(%d) Error: fail to create Deployment Unit instance.\n", __func__, __LINE__);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to create Deployment Unit instance.", __FUNCTION__);
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}

	//Set DU parameters
	{
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.DeploymentUnit.%d.UUID", du_instance);
		if((ret = zcfg_set_value(valueSetPath, UUID)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Deployment Unit(%d) UUID value.\n", __func__, __LINE__, du_instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Deployment Unit(%d) UUID value .", __FUNCTION__, du_instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}

		char status[16] = {"Installed"};
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.DeploymentUnit.%d.Status", du_instance);
		if((ret = zcfg_set_value(valueSetPath, status)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Deployment Unit(%d) Status value.\n", __func__, __LINE__, du_instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Deployment Unit(%d) Status value .", __FUNCTION__, du_instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}

		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.DeploymentUnit.%d.URL", du_instance);
		if((ret = zcfg_set_value(valueSetPath, servInfoPtr.url)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Deployment Unit(%d) servInfoPtr.url value.\n", __func__, __LINE__, du_instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Deployment Unit(%d) URL value .", __FUNCTION__, du_instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}

		char alias[64] = {0};
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.DeploymentUnit.%d.Alias", du_instance);
		USP_SNPRINTF(alias, sizeof(alias), DEFAULT_ALIAS_PREFIX "%d", du_instance);
		if((ret = zcfg_set_value(valueSetPath, alias)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Deployment Unit(%d) alias value.\n", __func__, __LINE__, du_instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Deployment Unit(%d) alias value .", __FUNCTION__, du_instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}
	}

	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.DeploymentUnit.%d", du_instance);
	USP_ARG_Add(DUStateChangeEvent, "DeploymentUnitRef", unitRefPath);


	//rename the random download filename to the created DeploymentUnit du_instance name
	USP_SNPRINTF(duFile, sizeof(duFile), "%s%d", DU_PATH, du_instance);
	USP_LOG_Info("rename %s to %s\n", fileName, duFile);
	rename(fileName, duFile);


	if(DEVICE_SWMODULES_EU_Add(&eu_instance) != USP_ERR_OK){
		USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
		USP_LOG_Error("%s(%d) Error: fail to create Execution Unit instance.\n", __func__, __LINE__);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to create Execution Unit instance.", __FUNCTION__);
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}

	//Set EU parameters
	{
		char status[16] ={"Idle"};
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.ExecutionUnit.%d.Status", du_instance);
		if((ret = zcfg_set_value(valueSetPath, status)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Execution Unit(%d) Status value.\n", __func__, __LINE__, du_instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Execution Unit(%d) Status value .", __FUNCTION__, du_instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}
	}

	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.ExecutionUnit.%d", du_instance);
	USP_ARG_Add(DUStateChangeEvent, "ExecutionUnitRefList", unitRefPath);

	USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Installed");

exit:
	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);
	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);

	USP_ARG_Add(DUStateChangeEvent, "CompleteTime", currentTime);
	USP_SNPRINTF(duStateChange_faultCodeToStr, sizeof(duStateChange_faultCodeToStr), "%d", duStateChange_faultCode);
	USP_ARG_Add(DUStateChangeEvent, "FaultCode", duStateChange_faultCodeToStr);
	USP_ARG_Add(DUStateChangeEvent, "FaultString", duStateChange_faultString);
	USP_SIGNAL_DataModelEvent("Device.SoftwareModules.DUStateChange!", DUStateChangeEvent);

	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_DUUninstall_Thread(void *param) { // Device.SoftwareModules.DeploymentUnit.{i}.Uninstall()
	int ret = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0};
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;

	int du_instance;
	int eu_instance;
	int duStateChange_faultCode = USP_ERR_OK;
	char duStateChange_faultString[256] = {0}, duStateChange_faultCodeToStr[8] = {0};
	char unitRefPath[64] = {0};
	kv_vector_t *DUStateChangeEvent;

	int instance;
	char path[128];
	char cmd[256];
	char *EUInstance = NULL;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();

	DUStateChangeEvent = USP_ARG_Create();
	USP_ARG_Add(DUStateChangeEvent, "OperationPerformed", "Uninstall");

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		goto exit;
	}

	USP_LOG_Info("Execute (reqThd->cond)->parentPath %s\n", (reqThd->cond)->parentPath);
	snprintf(path, sizeof(path), "%s", (reqThd->cond)->parentPath);
	EUInstance = strstr(path, device_swdu_root_with_dot);
	if( EUInstance ){
		sscanf(path, "Device.SoftwareModules.DeploymentUnit.%d.", &instance);
		USP_LOG_Info("instance %d\n", instance);
		snprintf(cmd, sizeof(cmd), "%s%d", DU_PATH, instance);
		USP_LOG_Info("unlink %s\n", cmd);
		unlink(cmd);
	}

	DEVICE_SWMODULES_DU_Del(instance);
	DEVICE_SWMODULES_EU_Del(instance);
	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.DeploymentUnit.%d", instance);
	USP_ARG_Add(DUStateChangeEvent, "DeploymentUnitRef", unitRefPath);
	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.ExecutionUnit.%d", instance);
	USP_ARG_Add(DUStateChangeEvent, "ExecutionUnitRefList", unitRefPath);
	USP_ARG_Add(DUStateChangeEvent, "CurrentState", "UnInstalled");

exit:
	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SNPRINTF(duStateChange_faultCodeToStr, sizeof(duStateChange_faultCodeToStr), "%d", duStateChange_faultCode);
	USP_ARG_Add(DUStateChangeEvent, "FaultCode", duStateChange_faultCodeToStr);
	USP_ARG_Add(DUStateChangeEvent, "FaultString", duStateChange_faultString);
	USP_SIGNAL_DataModelEvent("Device.SoftwareModules.DUStateChange!", DUStateChangeEvent);

	if((reqThd->cond) != NULL){
			USP_FREE((reqThd->cond));
			(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}

void *AsyncOperation_SoftwareModules_DeploymentUnit_Update_Thread(void *param) { //Device.SoftwareModules.DeploymentUnit.{i}.Update() 
	int i = 0, j = 0, ret = 0;
	int err = USP_ERR_OK, faultCode = USP_ERR_OK;
	char *err_msg = NULL;
	char faultString[256] = {0}, faultCodeToStr[8] = {0};
	char currentTime[DATETIME_LENGTH] = {0};
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char ip_addr[INET6_ADDRSTRLEN + 1];
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	servInfo_t servInfoPtr;
	#if 1//Jessie InstallDU()
	int du_instance;
	int eu_instance;
	int fd;
	int duStateChange_faultCode = USP_ERR_OK;
	char duFile[256];
	char fileName[128];
	char duStateChange_faultString[256] = {0}, duStateChange_faultCodeToStr[8] = {0};
	char unitRefPath[64] = {0};
	char valueSetPath[256] = {0};
	kv_vector_t *DUStateChangeEvent;
	#endif

	int instance;
	char path[128];
	char *EUInstance = NULL;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	DUStateChangeEvent = USP_ARG_Create();
	reqThd->threadID = pthread_self();
	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));

	//event transfercomplete init
	reqThd->transferCompleteOuputArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferType", "Update");

	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}

	USP_ARG_Add(DUStateChangeEvent, "OperationPerformed", "Update");
	USP_ARG_Add(DUStateChangeEvent, "StartTime", currentTime);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "StartTime", currentTime);

	addTransferCompleteParam(reqThd);

	// Exit if unable to signal that this operation is active
	err = USP_SIGNAL_OperationStatus((reqThd->cond)->request_instance, "Active");
	if (err != USP_ERR_OK)
	{
		    USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(faultString, sizeof(faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
		    USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: USP_SIGNAL_OperationStatus() failed", __FUNCTION__);
			faultCode = USP_ERR_INTERNAL_ERROR;
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
	}

	if((reqThd->cond)->input_args->num_entries) {
		for(i=0; i<(reqThd->cond)->input_args->num_entries; ++i) {
			kv_pair_t *kv = &(reqThd->cond)->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "URL") == 0){
				strncpy(servInfoPtr.url, kv->value, sizeof(servInfoPtr.url));
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) != USP_ERR_OK){
					USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(faultString, sizeof(faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
					faultCode = USP_ERR_INTERNAL_ERROR;
					duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
					goto exit;
				}
				cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
				printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

				USP_ARG_Add(reqThd->transferCompleteOuputArgs, "TransferURL", kv->value);
			}
		}
	}

	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	//create a random download filename
	snprintf(fileName, sizeof(fileName), "%s%s", DU_PATH, "XXXXXX");
	fd = mkstemp(fileName);
	USP_LOG_Info("fileName %s\n", fileName);

	//Get the file via http get
	if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, fileName)){
		printf("%s(%d) Get file %s successfully.\n", __func__, __LINE__, fileName);

		if((ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) != 0){
			USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_DISABLE).\n", __func__, __LINE__, ret);
			faultCode = USP_ERR_INTERNAL_ERROR;
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	}
	else{//fail to download file
		USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, fileName);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		faultCode = USP_ERR_INTERNAL_ERROR;
		duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	//Get the parent instance number
	USP_LOG_Info("Execute (reqThd->cond)->parentPath %s\n", (reqThd->cond)->parentPath);
	snprintf(path, sizeof(path), "%s", (reqThd->cond)->parentPath);
	EUInstance = strstr(path, device_swdu_root_with_dot);
	if( EUInstance ){
		sscanf(path, "Device.SoftwareModules.DeploymentUnit.%d.", &instance);
		USP_LOG_Info("instance %d\n", instance);
	}

	//update URL to date model
	{
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.DeploymentUnit.%d.URL", instance);
		if((ret = zcfg_set_value(valueSetPath, servInfoPtr.url)) != USP_ERR_OK) {
			USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Failed");
			USP_LOG_Error("%s(%d) Error: fail to set Deployment Unit(%d) servInfoPtr.url value.\n", __func__, __LINE__, instance);
			USP_SNPRINTF(duStateChange_faultString, sizeof(duStateChange_faultString), "%s: Error: fail to set Deployment Unit(%d) URL value .", __FUNCTION__, instance);
			duStateChange_faultCode = USP_ERR_INTERNAL_ERROR;
			goto exit;
		}
	}

	//rename the random download filename to the created DeploymentUnit instance name
	snprintf(duFile, sizeof(duFile), "%s%d", DU_PATH, instance);
	USP_LOG_Info("Delete %s(Old)\n", duFile);
	unlink(duFile);
	USP_LOG_Info("rename %s to %s(New)\n", fileName, duFile);
	rename(fileName, duFile);

	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.DeploymentUnit.%d", instance);
	USP_ARG_Add(DUStateChangeEvent, "DeploymentUnitRef", unitRefPath);
	USP_SNPRINTF(unitRefPath, sizeof(unitRefPath), "Device.SoftwareModules.ExecutionUnit.%d", instance);
	USP_ARG_Add(DUStateChangeEvent, "ExecutionUnitRefList", unitRefPath);
	USP_ARG_Add(DUStateChangeEvent, "CurrentState", "Installed");


exit:
	if(getCurrentTimeInDatetimeFormat(currentTime) != USP_ERR_OK){
		strncpy(currentTime, DEFAULT_DATETIME_VALUE, DATETIME_LENGTH);
	}
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "CompleteTime", currentTime);
	USP_SNPRINTF(faultCodeToStr, sizeof(faultCodeToStr), "%d", faultCode);

	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultCode", faultCodeToStr);
	USP_ARG_Add(reqThd->transferCompleteOuputArgs, "FaultString", faultString);

	USP_SNPRINTF(duStateChange_faultCodeToStr, sizeof(duStateChange_faultCodeToStr), "%d", duStateChange_faultCode);
	USP_ARG_Add(DUStateChangeEvent, "FaultCode", duStateChange_faultCodeToStr);
	USP_ARG_Add(DUStateChangeEvent, "FaultString", duStateChange_faultString);
	USP_SIGNAL_DataModelEvent("Device.SoftwareModules.DUStateChange!", DUStateChangeEvent);

	if(err == USP_ERR_OK){
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "PASS");
	}
	else{
		USP_SNPRINTF(res->result_str, sizeof(res->result_str), "FAIL");
		err_msg = res->err_msg;
	}

	// Save all results into the output arguments using KV_VECTOR_ functions
	reqThd->operCompleteArgs = USP_ARG_Create();
	USP_ARG_Add(reqThd->operCompleteArgs, "Results", res->result_str);

	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, reqThd->operCompleteArgs);
	reqThd->operCompleteArgs = NULL;

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);
	reqThd->transferCompleteOuputArgs = NULL;

	if((reqThd->cond) != NULL){
		if((reqThd->cond)->input_args != NULL){
			USP_FREE((reqThd->cond)->input_args);
			(reqThd->cond)->input_args = NULL;
		}
		USP_FREE((reqThd->cond));
		(reqThd->cond) = NULL;
	}

	if((ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) != 0){
		USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_ENABLE).\n", __func__, __LINE__, ret);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return NULL;
}
#endif
