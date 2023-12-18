#include "vendor_operate_async.h"

extern asyncOperReqThread_t asyncOperReqThread[MAX_ASYNC_REQUEST_NUM];

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
	objIndex_t ipIfIid;
	rdm_IpIfaceV4Addr_t *ipv4Addr = NULL;
	objIndex_t v4AddrIid = {0};

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

BOOL zHttpCustomrequest_post(CgNetURL *url,  char *ipaddr, int port, char *requestpath, char *fileStorePath)
{

	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;

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
			else if(strcmp(kv->key, "AutoActivate") == 0){
				if(strcmp(kv->value, "true") == 0){
					AutoActivate = 1;
				}
				else{
					AutoActivate = 0;
				}
			}
		}
	}

	//check mandatory arguments
	if(strlen(servInfoPtr.url) == 0 || AutoActivate == -1){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: Missing mandatory arguments.", __FUNCTION__);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
	}

	//Get the file via http get
	if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, FIRMWARE_STORE_PATH)){
		printf("%s(%d) Get file %s successfully, now is going to upgrade the system..\n", __func__, __LINE__, FIRMWARE_STORE_PATH);

		if((ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL)) != 0){
			USP_LOG_Error("%s(%d) Error(= %d): pthread_setcancelstate(PTHREAD_CANCEL_DISABLE).\n", __func__, __LINE__, ret);
			faultCode = USP_ERR_INTERNAL_ERROR;
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}

		//start to upgrade firmware
		if(AutoActivate){
			if(zcfgFeFwUpgrade(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS){
				USP_LOG_Info("%s(%d) zcfgFeFwUpgrade() return ZCFG_SUCCESS.\n", __func__, __LINE__);
				USP_LOG_Info("%s(%d) The AutoActive is true, now is going to reboot...\n", __func__, __LINE__);
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
		else{//no need to reboot case
			if(zcfgFeFwUpgradeWrite(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS){
				USP_LOG_Info("%s(%d) zcfgFeFwUpgradeWrite() return ZCFG_SUCCESS.\n", __func__, __LINE__);
				USP_LOG_Info("%s(%d) The AutoActive is false, no need to reboot now.\n", __func__, __LINE__);
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
	}
	else{//fail to download file
		USP_LOG_Info("%s(%d) Fail to get file %s.\n", __func__, __LINE__, FIRMWARE_STORE_PATH);
		USP_SNPRINTF(res->err_msg, sizeof(res->err_msg), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		USP_SNPRINTF(faultString, sizeof(faultString), "%s: Error: fail to download the file %s.", __FUNCTION__, servInfoPtr.path);
		faultCode = USP_ERR_INTERNAL_ERROR;
		err = USP_ERR_COMMAND_FAILURE;
		goto exit;
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

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);

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
		err = DATA_MODEL_DeleteInstance(path, 0);
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
		}

		if(reqThd->transferCompleteOuputArgs != NULL){
			USP_ARG_Destroy(reqThd->transferCompleteOuputArgs);
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
	char dataBlockSize[256] = {0};
	char dscp[256] = {0};

	char ifName[33] = {0};
	oper_output_res_t result;
	oper_output_res_t *res = &result;
	char ipAddressUsed[32] = "";
	asyncOperReqThread_t *reqThd = (asyncOperReqThread_t*)param;
	char diagResultFile[256];
	struct stat statBuf;
	struct json_object *diagResultParams = NULL;
	char tmpString[32];
	
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[(reqThd->cond)->request_instance].threadID = pthread_self();
	memset(&result, 0, sizeof(result));
	snprintf(diagResultFile, sizeof(diagResultFile), "%s.ping%d", DIAG_RESULT_PATH, (reqThd->cond)->request_instance);
	remove(diagResultFile);
	
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
		strcat(pCmd, " -W ");
		strcat(pCmd, timeout);
	}
	
	snprintf(tmpString, sizeof(tmpString), " %s >%s&", host, DIAG_RESULT_PATH);
	strcat(pCmd, tmpString);
	system(sysCmd);

	USP_LOG_Info("%s() diagResultFile=%s,sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);
	
	//waiting for the DIAG_RESULT_PATH file	
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
	struct stat statBuf;
	struct json_object *diagResultParams = NULL;
	char tmpString[32];
	
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
	snprintf(tmpString, sizeof(tmpString), " &>%s&", DIAG_RESULT_PATH);
	strcat(pCmd, tmpString);
	system(sysCmd);

	USP_LOG_Info("%s() diagResultFile=%s,sysCmd=%s\n", __FUNCTION__, diagResultFile, sysCmd);
	
	//waiting for the DIAG_RESULT_PATH file	
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
	//waiting for the DIAG_RESULT_PATH file	
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
	}

	if(err != USP_ERR_OK){
		err_msg = res->err_msg;
	}

	//NOTIFY message with OperationComplete including output params
	USP_SIGNAL_OperationComplete((reqThd->cond)->request_instance, err, err_msg, (reqThd->operCompleteArgs));

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

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);

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

	USP_SIGNAL_DataModelEvent("Device.LocalAgent.TransferComplete!", reqThd->transferCompleteOuputArgs);

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
