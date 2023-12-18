/*******************************************************************************
 i
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Julien Vermillard - initial implementation
 *    Fabien Fleutot - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Gregory Lemercier - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *
 *******************************************************************************/

/*
 * This object is single instance only, and provide firmware upgrade functionality.
 * Object ID is 5.
 */

/*
 * resources:
 * 0 package                   write
 * 1 package url               write
 * 2 update                    exec
 * 3 state                     read
 * 5 update result             read
 * 6 package name              read
 * 7 package version           read
 * 8 update protocol support   read
 * 9 update delivery method    read
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#if 1//Jessie Device
#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"


#include "zcmd_schema.h"
#include "zcfg_msg.h"
#include "zcmd_tool.h"
#include "zlog_api.h"
#include "json-c/json_object.h"
#include "connection.h"
#include "http_utils.h"
#define WAKKAMA_MAPPING_TABLE "/etc/zcfg_wakkama_mapping_table"
#endif

#define FIRMWARE_STORE_PATH         "/tmp/firmwareUpgrade"

// ---- private object "Firmware" specific defines ----
// Resource Id's:
#define RES_M_PACKAGE                   0
#define RES_M_PACKAGE_URI               1
#define RES_M_UPDATE                    2
#define RES_M_STATE                     3
#define RES_M_UPDATE_RESULT             5
#define RES_O_PKG_NAME                  6
#define RES_O_PKG_VERSION               7
#define RES_O_UPDATE_PROTOCOL           8
#define RES_M_UPDATE_METHOD             9

#define LWM2M_FIRMWARE_PROTOCOL_NUM     4
#define LWM2M_FIRMWARE_PROTOCOL_NULL    ((uint8_t)-1)


//#ifdef ZYXEL_LTE_FIRMWARE_UPGRADE_BY_DFOTA
char dfotaDownloadUrl[STRSIZE(SIZE_URL)];//extern?
#include "dfota.h"
#define DFOTA_S_PORT "/dev/ttyUSB2"
#define DFOTA_DOWNLOAD_TRY 10
//#endif

//for open() function
//#ifdef ZYXEL_LTE_FIRMWARE_UPGRADE_BY_QFIREHOSE
#include <fcntl.h>
#define LOCAL_QFIREHOSE_PATH "/tmp/QFirehoseUpgrade.list"
//#endif

//#ifdef ZYXEL_LTE_FIRMWARE_UPGRADE_BY_DFOTA
#define LOCAL_DFOTA_PATH "/tmp/DFOATUpgrade.zip"
//#endif

//#ifdef ZYXEL_LTE_FIRMWARE_UPGRADE_BY_QFIREHOSE
int checkQfirehoseFile(const char *fileName, const int fileSize, const char *fileChecksum);
int fileSizeLastCheck = 0, fileStalledTimes = 0;
//#endif

typedef struct
{
    uint8_t state;
    uint8_t result;
    char pkg_name[256];
    char pkg_version[256];
    uint8_t protocol_support[LWM2M_FIRMWARE_PROTOCOL_NUM];
    uint8_t delivery_method;
} firmware_data_t;

typedef struct _fw_instance_
{
    struct _fw_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id

	//observe save data
	char package_uri[257];
    char state[257];
    char update_result[257];
    char pkg_name[257];
    char pkg_version[257];
	resource_ins_list_t *update_protocol;
	char delivery_method[257];
} fw_instance_t;

servInfo_t servInfoPtr;
char pTheHostname[STRSIZE(SIZE_URL)]={0};



//#ifdef ZYXEL_LTE_FIRMWARE_UPGRADE_BY_QFIREHOSE
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

	//printf("[LwM2M-QFirehose]fileSizeNowCheck is %d, fileSizeLastCheck is %d.\n", fileSizeNowCheck, fileSizeLastCheck);
	if(fileSizeNowCheck >= fileSize){
		printf("[LwM2M-QFirehose] The file %s is downloaded, now going to check its checksum value.\n", fileName);

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
				printf("[LwM2M-QFirehose] checksum of file %s is %s, fileChecksum is %s.\n", fileName, checksumDownload, fileChecksum);
				if(strcasecmp(checksumDownload, fileChecksum) == 0){
					printf("[LwM2M-QFirehose] Checksum matched.\n");
					if(pfp != NULL){
						pclose(pfp);
						pfp = NULL;
					}
					return 1;
				}
				else{
					printf("[LwM2M-QFirehose] Checksum does not match.\n");
					if(pfp != NULL){
						pclose(pfp);
						pfp = NULL;
					}
					return -1;
				}
				
			}
			else{
				printf("[LwM2M-QFirehose]%s(%d): No md5 outputs of file %s.\n", __FUNCTION__, __LINE__, fileName);
				if(pfp != NULL){
					pclose(pfp);
					pfp = NULL;
				}
				return -1;
			}
		}
	}
	else if(fileSizeNowCheck > fileSizeLastCheck){
		printf("[LwM2M-QFirehose] %d > %d, The file is still downloading...\n", fileSizeNowCheck, fileSizeLastCheck);
		fileSizeLastCheck = fileSizeNowCheck;
	}
	else{
		fileStalledTimes++;
		printf("[LwM2M-Qfirehose] No files are downloaded(%d/%d), stalled(%d)...?\n", fileSizeNowCheck, fileSizeLastCheck, fileStalledTimes);
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
//#endif

static bool checkFwValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, fw_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_M_PACKAGE_URI:
        if(strcmp(targetP->package_uri, zcfgParamValue))
        {
            strncpy(targetP->package_uri, zcfgParamValue, sizeof(targetP->package_uri));
            return true;
        }
        break;

    case RES_M_STATE:
        if(strcmp(targetP->state, zcfgParamValue))
        {
            strncpy(targetP->state, zcfgParamValue, sizeof(targetP->state));
            return true;
        }
        break;

    case RES_M_UPDATE_RESULT:
        if(strcmp(targetP->update_result, zcfgParamValue))
        {
            strncpy(targetP->update_result, zcfgParamValue, sizeof(targetP->update_result));
            return true;
        }
        break;

    case RES_O_PKG_NAME:
        if(strcmp(targetP->pkg_name, zcfgParamValue))
        {
            strncpy(targetP->pkg_name, zcfgParamValue, sizeof(targetP->pkg_name));
            return true;
        }
        break;

    case RES_O_PKG_VERSION:
        if(strcmp(targetP->pkg_version, zcfgParamValue))
        {
            strncpy(targetP->pkg_version, zcfgParamValue, sizeof(targetP->pkg_version));
            return true;
        }
        break;

    case RES_O_UPDATE_PROTOCOL:
	{
        if(LWM2M_LIST_FIND(targetP->update_protocol, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->update_protocol = LWM2M_LIST_ADD(targetP->update_protocol, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->update_protocol, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }  

	case RES_M_UPDATE_METHOD:
        if(strcmp(targetP->delivery_method, zcfgParamValue))
        {
            strncpy(targetP->delivery_method, zcfgParamValue, sizeof(targetP->delivery_method));
            return true;
        }
        break;

    default:
        break;
    }
    return false;
}

static uint8_t prv_firmware_read(lwm2m_context_t *contextP,
                                 uint16_t instanceId,
                                 int * numDataP,
                                 lwm2m_data_t ** dataArrayP,
                                 lwm2m_object_t * objectP)
{
    int i;
    uint8_t result;
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);
	objIndex_t objIid;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
	int ret = 0;
    fw_instance_t * targetP;
	char desValue[512] = {0};
    char omaID[16] = {0};

    /* unused parameter */
    (void)contextP;
printf("%s() Jessie instanceId=%d\n", __FUNCTION__, instanceId);
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    targetP = (fw_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;
printf("%s() Jessie lwm2m_list_find found\n", __FUNCTION__);
    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_M_PACKAGE_URI,
                RES_M_STATE,
				RES_M_UPDATE_RESULT,
				RES_O_PKG_NAME,
				RES_O_PKG_VERSION,
				RES_O_UPDATE_PROTOCOL,
				RES_M_UPDATE_METHOD
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
		snprintf(omaID, 16, "%d_%d", objectP->objID, (*dataArrayP)[i].id);
        switch ((*dataArrayP)[i].id)
        {
printf("%s() Jessie *dataArrayP)[i].id=%d\n", __FUNCTION__, (*dataArrayP)[i].id);			
        case RES_M_PACKAGE:
        case RES_M_UPDATE:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

		// String
        case RES_M_PACKAGE_URI:
		case RES_O_PKG_NAME:
		case RES_O_PKG_VERSION:
		{
			if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
			if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
			{
				printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
			}

			if(checkFwValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
			{
				setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
			}

			lwm2m_data_encode_string(desValue, *dataArrayP + i);
			result = COAP_205_CONTENT;
			break;
		}
		
		// Integer
        case RES_M_STATE:
		case RES_M_UPDATE_RESULT:
		{
			if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;

			if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
			{
				printf("[%s] prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
			}

			if(checkFwValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
			{
				setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
			}

			lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);

			result = COAP_205_CONTENT;
			break;
		}

        case RES_O_UPDATE_PROTOCOL:
        {
            lwm2m_data_t * subTlvP;
            size_t count;
            size_t ri;
            int num = 0;

            while ((num < LWM2M_FIRMWARE_PROTOCOL_NUM) &&
                    (data->protocol_support[num] != LWM2M_FIRMWARE_PROTOCOL_NULL))
                num++;

            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
            {
                count = (*dataArrayP)[i].value.asChildren.count;
                subTlvP = (*dataArrayP)[i].value.asChildren.array;
            }
            else
            {
                count = num;
                if (!count) count = 1;
                subTlvP = lwm2m_data_new(count);
                for (ri = 0; ri < count; ri++) subTlvP[ri].id = ri;
                lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);
            }

            if (num)
            {
                for (ri = 0; ri < count; ri++)
                {
                    if (subTlvP[ri].id >= num) return COAP_404_NOT_FOUND;
					sprintf(desValue, "%d", data->protocol_support[subTlvP[ri].id]);
					if(checkFwValueChange((*dataArrayP)[i].id, ri, targetP, desValue))
					{
						setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  ri);
					}    
                    lwm2m_data_encode_int(data->protocol_support[subTlvP[ri].id],
                                          subTlvP + ri);
                }
            }
            else
            {
                /* If no protocol is provided, use CoAP as default (per spec) */
                for (ri = 0; ri < count; ri++)
                {
                    if (subTlvP[ri].id != 0) return COAP_404_NOT_FOUND;
					sprintf(desValue, "%d", 0);
					if(checkFwValueChange((*dataArrayP)[i].id, ri, targetP, desValue))
					{
						setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  ri);
					}    
                    lwm2m_data_encode_int(0, subTlvP + ri);
                }
            }
            result = COAP_205_CONTENT;
            break;
        }

        case RES_M_UPDATE_METHOD:
		{
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
			sprintf(desValue, "%d", data->delivery_method);
			if(checkFwValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
			{
				setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
			}
            lwm2m_data_encode_int(data->delivery_method, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;
		}

        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_firmware_discover(lwm2m_context_t *contextP,
                                   uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    server_instance_t * targetP;
    uint8_t result;
    int i;

	//printf("%s(%d) instanceId %d, numDataP %d.\n", __FUNCTION__, __LINE__, instanceId, numDataP);
    /* unused parameter */
    (void)contextP;

    result = COAP_205_CONTENT;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
			RES_M_PACKAGE,
				RES_M_PACKAGE_URI,
				RES_M_UPDATE,
				RES_M_STATE,
				RES_M_UPDATE_RESULT,
				RES_O_PKG_NAME,
				RES_O_PKG_VERSION,
				RES_O_UPDATE_PROTOCOL,
				RES_M_UPDATE_METHOD,
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
				case RES_M_PACKAGE_URI:
				case RES_M_UPDATE:
				case RES_M_STATE:
				case RES_M_UPDATE_RESULT:
				case RES_O_PKG_NAME:
				case RES_O_PKG_VERSION:
				case RES_O_UPDATE_PROTOCOL:
				case RES_M_UPDATE_METHOD:
                break;

            default:
                result = COAP_404_NOT_FOUND;
                break;
            }
        }
    }

    return result;
}

/* State_3
0: Idle (before downloading or after successful updating)
1: Downloading (The data sequence is on the way)
2: Downloaded
3: Updating
*/

/* Update_Result_5
0: Initial value. Once the updating process is initiated (Download /Update), this Resource MUST be reset to Initial value.
1: Firmware updated successfully.
2: Not enough flash memory for the new firmware package.
3: Out of RAM during downloading process.
4: Connection lost during downloading process.
5: Integrity check failure for new downloaded package.
6: Unsupported package type.
7: Invalid URI.
8: Firmware update failed.
9: Unsupported protocol. A LwM2M client indicates the failure to retrieve the firmware image using the URI provided in the Package URI resource by writing the value 9 to the /5/0/5 (Update Result resource) when the URI contained a URI scheme unsupported by the client. Consequently, the LwM2M Client is unable to retrieve the firmware image using the URI provided by the LwM2M Server in the Package URI when it refers to an unsupported protocol.
10: Firmware update cancelled. A Cancel operation has been executed successfully.
11: Firmware update deferred.
*/
int updateResponseState(rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj, objIndex_t objIid, int lwm2mfwupdateObjState_3, int lwm2mfwupdateObjResult_5, firmware_data_t *data, int dataState, char *path_remove_file){
	int ret;
	printf("%s() Jessie lwm2mfwupdateObjState_3=%d, lwm2mfwupdateObjResult_5=%d, dataState=%d\n", __FUNCTION__, lwm2mfwupdateObjState_3, lwm2mfwupdateObjResult_5, dataState);
	zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Firmware Update State_3 %d, Result_5 %d, dataState %d.", lwm2mfwupdateObjState_3, lwm2mfwupdateObjResult_5, dataState);

	if( lwm2mfwupdateObj ){
		lwm2mfwupdateObj->State_3 = (lwm2mfwupdateObjState_3 >= 0 ? lwm2mfwupdateObjState_3 : lwm2mfwupdateObj->State_3);
		lwm2mfwupdateObj->Update_Result_5 = (lwm2mfwupdateObjResult_5 >= 0 ? lwm2mfwupdateObjResult_5 : lwm2mfwupdateObj->Update_Result_5);

		ret = 0;
		printf("%s() Jessie call zcfgFeObjStructSet\n", __FUNCTION__);
		if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_FW_UPDATE, &objIid, lwm2mfwupdateObj, NULL)) != ZCFG_SUCCESS){
			printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
			zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s Fail with zcfgFeObjStructSet(ret = %d).\n", __FUNCTION__, ret);
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
	}
	
	if( data ){
		data->state = dataState;
	}

	//The device should remove the downloaded firmware image when the state is reset to Idle (0).
	if(path_remove_file){
		printf("%s() Jessie call unlink path_remove_file %s\n", __FUNCTION__, path_remove_file);
		unlink(path_remove_file);
	}

	return COAP_204_CHANGED;
}

void thread_update_file(void)
{
	objIndex_t objIid;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
	int ret;
    uint8_t result;
	int moduleDFOTA = 0;
	int moduleQFIREHOSE = 0;
	int routerFwUpgrade = 0;
	int tryDFOTADLCount, dfotaRet = 0;
	FILE* fp = NULL, *pfp = NULL, *pfp_m = NULL;
	char commands[256] = {0}, qFireFileName[128] = {0}, tmp[256] = {0}, qFileFileChecksum[33] = {0};
	char *p_start = NULL, *p_end = NULL;
	int fileState = 0, fileTryDownloadTimes = 0, giveUpDownload = 0, fileSize = 0, waitResetCount = 0;
	int err = (-1);
	int rebootNeeded = 1;

	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);
	if(ret == ZCFG_SUCCESS){
		if(lwm2mfwupdateObj->State_3 == 2)//2: Downloaded
		{
			//3: Updating
			if(updateResponseState(lwm2mfwupdateObj, objIid, 3, 0, NULL, 3, NULL) != COAP_204_CHANGED){//Update_Result_5 value is not updated yet, use -1
				goto END_OF_RES_M_UPDATE;
			}

			routerFwUpgrade = 0;
			moduleDFOTA = 0;
			moduleQFIREHOSE = 0;
			err = (-1);

			printf("%s Jessie servInfoPtr.path=%s\n", __FUNCTION__, servInfoPtr.path);
			if(servInfoPtr.path && strstr(servInfoPtr.path, ".bin")){//to be refined
				routerFwUpgrade = 1;
			} else if(servInfoPtr.path && strstr(servInfoPtr.path, ".zip")){//to be refined
				moduleDFOTA = 1;
			} else if(servInfoPtr.path && strstr(servInfoPtr.path, ".txt")){//to be refined .list? .txt?
				moduleQFIREHOSE = 1;
			}

			if(routerFwUpgrade){
				fprintf(stdout, "\n\t FIRMWARE UPDATE\r\n\n");
				if(zcfgFeFwUpgradeWrite(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS){
					printf("zcfgFeFwUpgradeWrite success.\n");
					zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "zcfgFeFwUpgradeWrite success.");
					
					/* If performing the Update or Cancel operation was successful, the state changes to 
					0: Idle (before downloading or after successful updating)
					1: Firmware updated successfully.
					*/
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 1, NULL, 1, FIRMWARE_STORE_PATH) != COAP_204_CHANGED){
						goto END_OF_RES_M_UPDATE;
					}

					//? wait until next Notify about State_3 Update_Result_5 is sent?

					result = COAP_204_CHANGED;
					goto END_OF_RES_M_UPDATE;
				}
				else{
					printf("%s zcfgFeFwUpgradeWrite() FIRMWARE UPDATE return fail.\n", __FUNCTION__);
					zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s zcfgFeFwUpgradeWrite() FIRMWARE UPDATE return fail.", __FUNCTION__);
					//0: Idle (before downloading or after successful updating)
					//8: Firmware update failed
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 8, NULL, 8, FIRMWARE_STORE_PATH) != COAP_204_CHANGED){
						goto END_OF_RES_M_UPDATE;
					}

					rebootNeeded = 0;
					result = COAP_500_INTERNAL_SERVER_ERROR;
					goto END_OF_RES_M_UPDATE;
				}
			} else if(moduleDFOTA){
				fprintf(stdout, "\n\t DFOTA MODULE FIRMWARE UPDATE\r\n\n");
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
				printf("DFOTA now update with url : %s.\n", dfotaDownloadUrl);

				if(strlen(dfotaDownloadUrl) > 0){
					while( tryDFOTADLCount >= 0 ){
						tryDFOTADLCount--;
						dfotaRet = updateModuleByDFOTA(DFOTA_S_PORT, dfotaDownloadUrl);
						if(0 == dfotaRet){
							printf( "****DFOTA Module Upgrade Success.****\n" );
							zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "DFOTA Module Upgrade Success.");
							//0: Idle (before downloading or after successful updating)
							//1: Firmware updated successfully.
							if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 1, NULL, 1, NULL) != COAP_204_CHANGED){
								goto END_OF_RES_M_UPDATE;
							}
							
							result = COAP_204_CHANGED;

							break;
						}
						else if((tryDFOTADLCount > 0) && ((dfotaRet == 601) || (dfotaRet == 701))){//601:FTP unknown error, 701: HTTP(S) unknown error
							//try at least tryDFOTADLCount times to download the file via HTTP/FTP link
							printf( "**** DFOTA download fail(%d). \nTry it again(Remaining times = %d).****\n" , dfotaRet, tryDFOTADLCount);
							sleep(1);

							result = COAP_500_INTERNAL_SERVER_ERROR;
							continue;
						}
						else{
							if((tryDFOTADLCount <= 0) && ((dfotaRet == 601) || (dfotaRet == 701))){//601:FTP unknown error, 701: HTTP(S) unknown error
								zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "****DFOTA download fail(%d)****\n", dfotaRet);
								printf( "****DFOTA download fail(%d)****\n" );
								//0: Idle (before downloading or after successful updating)
								//4: Connection lost during downloading process
								if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 4, NULL, 4, NULL) != COAP_204_CHANGED){
									goto END_OF_RES_M_UPDATE;
								}

								result = COAP_500_INTERNAL_SERVER_ERROR;
							
								break;
							}
							else{
								zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "****DFOTA Module Upgrade Fail(%d)!!****\n", dfotaRet);
								printf( "****DFOTA Module Upgrade Fail!!****\n" );
								//0: Idle (before downloading or after successful updating)
								//8: Firmware update failed
								if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 8, NULL, 8, NULL) != COAP_204_CHANGED){
									goto END_OF_RES_M_UPDATE;
								}

								result = COAP_500_INTERNAL_SERVER_ERROR;
								break;
							}
						}
					}//end while( tryDFOTADLCount >= 0 )
				} else {
					printf( "****DFOTA Module Upgrade Fail!!Invalid download url(%s)****\n" , dfotaDownloadUrl);
					zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "Invalid download url(%s). DFOTA Module Upgrade Fail.", dfotaDownloadUrl);
					//0: Idle (before downloading or after successful updating)
					//7: Invalid URI
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 7, NULL, 7, NULL) != COAP_204_CHANGED){
						goto END_OF_RES_M_UPDATE;
					}

					result = COAP_500_INTERNAL_SERVER_ERROR;
				}

				//unlink(LOCAL_DFOTA_PATH);//router board does not download the .zip file, we just pass the url to LTE module, and LTE module will download the .zip file by itself

			} else if(moduleQFIREHOSE){
				fprintf(stdout, "\n\t QFIREHOSE MODULE FIRMWARE UPDATE\r\n\n");

				if((fp = fopen(FIRMWARE_STORE_PATH, "r")) == NULL){
					printf( "****QFIREHOSE Module Upgrade Fail!! %s not found****\n", FIRMWARE_STORE_PATH);
					zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s is not found. QFIREHOSE Module Upgrade Fail.", FIRMWARE_STORE_PATH);
					err = (-1);
					//0: Idle (before downloading or after successful updating)
					//8: Firmware update failed
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 8, NULL, 8, NULL) != COAP_204_CHANGED){
						goto END_OF_RES_M_UPDATE;
					}

					result = COAP_500_INTERNAL_SERVER_ERROR;
				}
				else{
					printf("Now update with Qfirehose.\n");
					zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "QFIREHOSE Module Upgrade is ready to start!");
					memset(commands, 0, sizeof(commands));
					giveUpDownload = 0;
					while(fgets(commands, 255, fp) != NULL){
						if(giveUpDownload) break;

						if(commands[strlen(commands) - 1] == '\n'){
							commands[strlen(commands) - 1] = '\0';
						}

						printf("\n%s\n", commands);
						zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Commands %s", commands);
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
										zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "QFirehose Module Upgrade Success.");
										err = 0;
										//0: Idle (before downloading or after successful updating)
										//1: Firmware updated successfully
										if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 1, NULL, 1, FIRMWARE_STORE_PATH) != COAP_204_CHANGED){
											goto END_OF_RES_M_UPDATE;
										}

										result = COAP_204_CHANGED;

										goto END_OF_RES_M_UPDATE;
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
					zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Now wait module reset.");
					sleep(10);
					waitResetCount = 0;
					//wait for reset at last to 1 min
					while(1){
						if(waitResetCount > 10){
							printf("waitResetCount > 10, not wait module reset.\n");
							zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "waitResetCount > 10, not wait module reset.");
							break;
						}
						if(open("/dev/ttyUSB2", O_RDWR | O_NDELAY) == -1){
							printf("waitResetCount(%d) , still wait module reset....\n", waitResetCount);
							zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "waitResetCount(%d) , still wait module reset.", waitResetCount);
							sleep(5);
							waitResetCount++;
						}
						else{
							printf("Module had been reset successfully.\n");
							zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Module had been reset successfully.");
							sleep(12);
							break;
						}
					}
				}
				
				unlink(FIRMWARE_STORE_PATH);
				//unlink qfirehose fw?
			}
		}
		else
		{
			// firmware update already running
			printf("%s(%d) The state(= %d) is not 2(Downloaded).\n", __FUNCTION__, __LINE__, lwm2mfwupdateObj->State_3);
			zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "The state(%d) is not 2(Downloaded).\n", lwm2mfwupdateObj->State_3);
			result = COAP_500_INTERNAL_SERVER_ERROR;
			goto END_OF_RES_M_UPDATE;
		}
	}
	else{
		printf("%s(%d) zcfgFeObjStructGet() return ZCFG_FAIL.\n", __FUNCTION__, __LINE__);
		zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "Fail with zcfgFeObjStructSet(ret = %d).\n", ret);
		result = COAP_500_INTERNAL_SERVER_ERROR;
		goto END_OF_RES_M_UPDATE;
	}
END_OF_RES_M_UPDATE:
	if(lwm2mfwupdateObj != NULL){
		zcfgFeObjStructFree(lwm2mfwupdateObj);
		lwm2mfwupdateObj = NULL;
	}
	#if 1
	if(fp != NULL){
		fclose(fp);
		fp = NULL;
	}

	if(pfp != NULL){
		pclose(pfp);
		pfp = NULL;
	}
	#endif
	if(1 == rebootNeeded){
		system("sync");
		system("sync");
		system("sync");
		printf("Rebooting...\n");
		zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "Rebooting...");
		zcfgFeReqReboot(NULL);
	}
}
void thread_download_file(void)
{
	objIndex_t objIid;
	int ret;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
#if 1//QFIREHOSE
	FILE* fp = NULL, *pfp = NULL, *pfp_m = NULL;
	char commands[256] = {0}, qFireFileName[128] = {0}, tmp[256] = {0}, qFileFileChecksum[33] = {0};
	char *p_start = NULL, *p_end = NULL;
	int fileState = 0, fileTryDownloadTimes = 0, giveUpDownload = 0, fileSize = 0, waitResetCount = 0;
	int err = 0;
#endif
	printf("%s Now on thread download files.\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);
	if(ret == ZCFG_SUCCESS){
		 printf("%s input url=%s, parser result hostname=%s, port=%d, path=%s\n", __FUNCTION__, servInfoPtr.url, pTheHostname, servInfoPtr.port, servInfoPtr.path );

		if(zHttpCustomrequest_post(servInfoPtr.url, pTheHostname, servInfoPtr.port, servInfoPtr.path, FIRMWARE_STORE_PATH)){
			printf("%s Get file %s to %s successfully.\n", __FUNCTION__, servInfoPtr.url, FIRMWARE_STORE_PATH);
			zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Get file %s to %s successfully.\n", servInfoPtr.url, FIRMWARE_STORE_PATH);

			#if 1
			printf("%s Jessie servInfoPtr.path=%s\n", __FUNCTION__, servInfoPtr.path);
			if(servInfoPtr.path && strstr(servInfoPtr.path, ".txt")){//to be refined .list? .txt?
				fprintf(stdout, "\n\t QFIREHOSE MODULE FIRMWARE DOWNLOAD\r\n\n");
	
					if((fp = fopen(FIRMWARE_STORE_PATH, "r")) == NULL){
						printf( "****QFIREHOSE Module DOWNLOAD Fail!! %s not found****\n", FIRMWARE_STORE_PATH);
						zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s is not found. QFIREHOSE Module DOWNLOAD Fail.", FIRMWARE_STORE_PATH);
						err = (-1);
						//6: Unsupported package type
						if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 6, NULL, 6, NULL) != COAP_204_CHANGED){
							goto FAIL_DOWNLOAD_FILE;
						}
					}
					else{
						printf("Now download with Qfirehose.\n");
						zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "QFIREHOSE Module DOWNLOAD is ready to download!");
						memset(commands, 0, sizeof(commands));
						giveUpDownload = 0;
						while(fgets(commands, 255, fp) != NULL){
							if(giveUpDownload) break;

							if(commands[strlen(commands) - 1] == '\n'){
								commands[strlen(commands) - 1] = '\0';
							}

							printf("\n%s\n", commands);
							zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Commands %s", commands);
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
								zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "The file's infomation is %s(%d/%s).", qFireFileName, fileSize, qFileFileChecksum);
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
										zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "The file %s is downloaded. Go to next.", qFireFileName);
										break;
									}
									//got error when downloading, maybe the network or the server is down
									else{
										system("killall wget");
										printf("Retry state: fileTryDownloadTimes is %d.\n", fileTryDownloadTimes);
										zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "Retry state: fileTryDownloadTimes is %d.", fileTryDownloadTimes);
										if(fileTryDownloadTimes <= 3){
											goto Retry_Download_file;
										}
										else{
											printf( "****QFIREHOSE Module Download Fail!!The file %s can't be dwonloaded.****\n", qFireFileName);
											zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "QFIREHOSE file %s can't be dwonloaded.", qFireFileName);
											err = (-1);
											giveUpDownload = 1;
											//5: Integrity check failure for new downloaded package
											if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 5, NULL, 5, FIRMWARE_STORE_PATH) != COAP_204_CHANGED){
												goto FAIL_DOWNLOAD_FILE;
											}

											break;
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
			#endif

			//2: Downloaded
			if(updateResponseState(lwm2mfwupdateObj, objIid, 2, 0, NULL, 2, NULL) != COAP_204_CHANGED){
				goto FAIL_DOWNLOAD_FILE;
			}
			
		}//end of zHttpCustomrequest_post
		else{//fail to download file
			printf("%s(%d) Fail to download file %s.\n", __FUNCTION__, __LINE__, FIRMWARE_STORE_PATH);
			zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s(%d) Fail to download file %s.\n", __FUNCTION__, __LINE__, FIRMWARE_STORE_PATH);
			//4: Connection lost during downloading process
			if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 4, NULL, 4, NULL) != COAP_204_CHANGED){
				goto FAIL_DOWNLOAD_FILE;
			}
		}
	}
	else{
		printf("%s(%d) Error: Fail on zcfgFeObjStructGet.\n", __FUNCTION__, __LINE__);
		zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s Fail zcfgFeObjStructGet\n", __FUNCTION__);
		//0: Idle (before downloading or after successful updating)
		if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 0, NULL, 0, NULL) != COAP_204_CHANGED){
			goto FAIL_DOWNLOAD_FILE;
		}
	}
FAIL_DOWNLOAD_FILE:
	if(lwm2mfwupdateObj != NULL){
		zcfgFeObjStructFree(lwm2mfwupdateObj);
		lwm2mfwupdateObj = NULL;
	}

	#if 1
	if(fp != NULL){
		fclose(fp);
		fp = NULL;
	}

	if(pfp != NULL){
		pclose(pfp);
		pfp = NULL;
	}
	#endif	
}

static uint8_t prv_firmware_write(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  lwm2m_write_type_t writeType)
{
    int i, j;
	int ret = ZCFG_SUCCESS, pRet = 0;
	pthread_t p_id;
	char omaID[16] = {0};
	int res = 0, err = 0;
	char ip_addr[INET6_ADDRSTRLEN + 1];
	char thehostname[STRSIZE(SIZE_URL)]={0};
	char desValue[512] = {0};
	lwm2m_data_t * subTlvP;
	size_t count;
    uint8_t result;
	objIndex_t objIid;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);

    /* unused parameter */
    (void)contextP;

    // All write types are treated the same here
    (void)writeType;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

	snprintf(omaID, 16, "%d_%d", objectP->objID, (dataArray)[i].id);

    do
    {
        /* No multiple instance resources */
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;

        }

        switch (dataArray[i].id)
        {
        case RES_M_PACKAGE:
            // inline firmware binary
            result = COAP_204_CHANGED;
            break;

        case RES_M_PACKAGE_URI:
            result = COAP_204_CHANGED;

			printf("The URI(%d) is %s.\n", dataArray[i].value.asBuffer.length, (char*)dataArray[i].value.asBuffer.buffer);
			
			if(dataArray[i].value.asBuffer.length == 0){
				//Writing an empty string to Package URI Resource or setting the Package Resource to NULL (‘\0’), 
				//resets the Firmware Update State Machine
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);

				if(ret == ZCFG_SUCCESS){
					memset(lwm2mfwupdateObj->Package_URI_1, 0, sizeof(lwm2mfwupdateObj->Package_URI_1));
					//0: Idle (before downloading or after successful updating)
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 0, NULL, 0, NULL) != COAP_204_CHANGED){
						goto END_OF_RES_M_PACKAGE_URI;
					}

					/*
					   Writing an empty string to Package URI Resource or setting the Package Resource to NULL (‘\0’), resets the Firmware Update State Machine: the State Resource value is set to Idle and the Update Result Resource value is set to 0.
					   The device should remove the downloaded firmware image when the state is reset to Idle.
					   */
					unlink(FIRMWARE_STORE_PATH);
					/*If QFIREHOSE, unlink all files listed in the list? 
					mkdir /tmp/RG502QEAAAR01A04M4G_01.001V01.01.001V01
					mkdir /tmp/RG502QEAAAR01A04M4G_01.001V01.01.001V01/update
					mkdir /tmp/RG502QEAAAR01A04M4G_01.001V01.01.001V01/update/firehose
					*/
				}
				else{
					printf("%s(%d) Error: Fail with zcfgFeObjStructGet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
					zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s(%d) Error: Fail with zcfgFeObjStructGet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
					result = COAP_500_INTERNAL_SERVER_ERROR;
					goto END_OF_RES_M_PACKAGE_URI;
				}
			}
			else{
				//dataArray[i].value.asBuffer.length doesn't count the empty string
				//dataArray[i].value.asBuffer.buffer[dataArray[i].value.asBuffer.length] = '\0';
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);
				if(ret == ZCFG_SUCCESS){
					snprintf(lwm2mfwupdateObj->Package_URI_1, sizeof(lwm2mfwupdateObj->Package_URI_1), "%s", (char*)dataArray[i].value.asBuffer.buffer);
					if(dataArray[i].value.asBuffer.length >= sizeof(lwm2mfwupdateObj->Package_URI_1)){
						//no enoungh buffer for URI string
						lwm2mfwupdateObj->Package_URI_1[sizeof(lwm2mfwupdateObj->Package_URI_1) - 1] = '\0';
					}
					else{
						//end with an empty string
						lwm2mfwupdateObj->Package_URI_1[dataArray[i].value.asBuffer.length] = '\0';
					}

					if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_FW_UPDATE, &objIid, lwm2mfwupdateObj, NULL)) != ZCFG_SUCCESS){
						printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
						zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
						result = COAP_500_INTERNAL_SERVER_ERROR;
						goto END_OF_RES_M_PACKAGE_URI;
					}

					//Parsing the URI
					snprintf(servInfoPtr.url, sizeof(servInfoPtr.url), "%s",  (char*)dataArray[i].value.asBuffer.buffer);
					snprintf(dfotaDownloadUrl, sizeof(dfotaDownloadUrl), "%s",  (char*)dataArray[i].value.asBuffer.buffer);
					if(dataArray[i].value.asBuffer.length >= sizeof(servInfoPtr.url)){
						//no enoungh buffer for URI string
						servInfoPtr.url[sizeof(servInfoPtr.url) - 1] = '\0';
					}
					else{
						//end with an empty string
						servInfoPtr.url[dataArray[i].value.asBuffer.length] = '\0';
					}

					if(dataArray[i].value.asBuffer.length >= sizeof(dfotaDownloadUrl)){
						//no enoungh buffer for URI string
						dfotaDownloadUrl[sizeof(dfotaDownloadUrl) - 1] = '\0';
					}
					else{
						//end with an empty string
						dfotaDownloadUrl[dataArray[i].value.asBuffer.length] = '\0';
					}

					if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) == COAP_NO_ERROR){
						cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
						 printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __FUNCTION__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

						//store the upgrade package name,  skiping the '/' in the first place
						//snprintf(lwm2mfwupdateObj->PkgName_6, sizeof(lwm2mfwupdateObj->PkgName_6), "%s", (servInfoPtr.path + 1));

						//1: Downloading
						if(updateResponseState(lwm2mfwupdateObj, objIid, 1, 0, NULL, 1, NULL) != COAP_204_CHANGED){
							goto END_OF_RES_M_PACKAGE_URI;
						}

						snprintf(pTheHostname, sizeof(pTheHostname), "%s", thehostname);
						
						#if 1
						if(servInfoPtr.path && strstr(servInfoPtr.path, ".zip")){//to be refined
							//Do not download DFOTA file, pass url to LTE module directly
							//When in Downloaded state, and the executable Resource Update is triggered, the state changes to Updating if the update starts immediately.
							//so change lwm2mfwupdateObj->State_3 to (2: downloaded) so that execute could start if execute is ordered from server
							//2: Downloaded
							if(updateResponseState(lwm2mfwupdateObj, objIid, 2, 0, NULL, 0, NULL) != COAP_204_CHANGED){
								goto END_OF_RES_M_PACKAGE_URI;
							}
						} else 
						#endif
						{
							pRet=pthread_create(&p_id,NULL,(void *) thread_download_file,NULL);
							if(pRet != 0){
								printf("%s(%d) Fail on pthread_create.");
								//0: Idle (before downloading or after successful updating)
								if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 0, NULL, 0, NULL) != COAP_204_CHANGED){
									goto END_OF_RES_M_PACKAGE_URI;
								}
							}
							else{
								printf("%s(%d) create a thread to download file now. Main program goes on.\n", __FUNCTION__, __LINE__);
							}
						}
					}
					else{
						printf("%s: cwmp_parseurl(%d) failed\n", __FUNCTION__, err);
						zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s: cwmp_parseurl(%d) failed\n", __FUNCTION__, err);
						//7: Invalid URI
						if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 7, NULL, 7, NULL) != COAP_204_CHANGED){
							goto END_OF_RES_M_PACKAGE_URI;
						}
					}

					if(lwm2mfwupdateObj != NULL){
						zcfgFeObjStructFree(lwm2mfwupdateObj);
						lwm2mfwupdateObj = NULL;
					}
				}
				else{
					printf("%s(%d) Fail with zcfgFeObjStructGet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
					zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s(%d) Fail with zcfgFeObjStructGet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
					//0: Idle (before downloading or after successful updating)
					if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 0, NULL, 0, NULL) != COAP_204_CHANGED){
						goto END_OF_RES_M_PACKAGE_URI;
					}
				}
			}

END_OF_RES_M_PACKAGE_URI:
			if(lwm2mfwupdateObj != NULL){
				zcfgFeObjStructFree(lwm2mfwupdateObj);
				lwm2mfwupdateObj = NULL;
			}
            break;// end of RES_M_PACKAGE_URI

        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_firmware_execute(lwm2m_context_t *contextP,
                                    uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);
	objIndex_t objIid;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
	int ret = 0;
    uint8_t result;
	int pRet = 0;
	pthread_t p_id;

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    // for execute callback, resId is always set.
    switch (resourceId)
    {
    case RES_M_UPDATE:
		result = COAP_204_CHANGED;

		pRet=pthread_create(&p_id,NULL,(void *) thread_update_file,NULL);
		if(pRet != 0){
			printf("%s(%d) Fail on pthread_create.");
			//0: Idle (before downloading or after successful updating)
			if(updateResponseState(lwm2mfwupdateObj, objIid, 0, 0, NULL, 0, NULL) != COAP_204_CHANGED){
				return COAP_500_INTERNAL_SERVER_ERROR;
			}
		}
		else{
			printf("%s(%d) create a thread to update firmware now. Main program goes on.\n", __FUNCTION__, __LINE__);
		}

		break;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
	return result;
}

void display_firmware_object(lwm2m_object_t * object)
{
    firmware_data_t * data = (firmware_data_t *)object->userData;
    fprintf(stdout, "  /%u: Firmware object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    state: %u, result: %u\r\n", data->state,
                data->result);
    }
}

lwm2m_object_t * get_object_firmware(void)
{
    /*
     * The get_object_firmware function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * firmwareObj;
	rdm_DevInfo_t *devinfoObj = NULL;
	objIndex_t lwm2mFWupdateObjIid, objIid, cellIntfObjIid;
	int ret = 0;
	rdm_Lwm2mFwUpdate_t *lwm2mfwupdateObj = NULL;
	rdm_CellIntf_t *cellIntfObj = NULL;

    firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != firmwareObj)
    {

        fw_instance_t * fwInstance;
        
        memset(firmwareObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns its unique ID
         * The 5 is the standard ID for the optional object "Object firmware".
         */
        firmwareObj->objID = LWM2M_FIRMWARE_UPDATE_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        fwInstance = (fw_instance_t *)lwm2m_malloc(sizeof(fw_instance_t));
        if (NULL == fwInstance)
        {
            lwm2m_free(firmwareObj);
            return NULL;
        }

        memset(fwInstance, 0, sizeof(fw_instance_t));
        fwInstance->instanceId = 0;
        memset(fwInstance->package_uri, 0, 257);
		memset(fwInstance->state, 0, 257);
        memset(fwInstance->update_result, 0, 257);
        memset(fwInstance->pkg_name, 0, 257);
        memset(fwInstance->pkg_version, 0, 257);
		memset(fwInstance->delivery_method, 0, 257);
        firmwareObj->instanceList = LWM2M_LIST_ADD(firmwareObj->instanceList, fwInstance);

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        firmwareObj->readFunc    = prv_firmware_read;
        firmwareObj->writeFunc   = prv_firmware_write;
        firmwareObj->discoverFunc   = prv_firmware_discover;
        firmwareObj->executeFunc = prv_firmware_execute;
        firmwareObj->userData    = lwm2m_malloc(sizeof(firmware_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            firmware_data_t *data = (firmware_data_t*)(firmwareObj->userData);


			IID_INIT(lwm2mFWupdateObjIid);
			ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &lwm2mFWupdateObjIid, (void **)&lwm2mfwupdateObj);
			if(ret == ZCFG_SUCCESS){
				data->state = lwm2mfwupdateObj->State_3;
				data->result = lwm2mfwupdateObj->Update_Result_5;
			}
			else{
				data->state = 0;
				data->result = 0;
			}
			#if 1
			snprintf(data->pkg_name, sizeof(data->pkg_name), "");
			snprintf(lwm2mfwupdateObj->PkgName_6, sizeof(lwm2mfwupdateObj->PkgName_6), "");

			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_DEV_INFO, &objIid, (void **)&devinfoObj);
			if(ret == ZCFG_SUCCESS){
				IID_INIT(cellIntfObjIid);
				cellIntfObjIid.level = 1;
				cellIntfObjIid.idx[0] = 1;
				ret = zcfgFeObjStructGet(RDM_OID_CELL_INTF, &cellIntfObjIid, (void **)&cellIntfObj);
				if(ret == ZCFG_SUCCESS){
					snprintf(data->pkg_version, sizeof(data->pkg_version), "%s/%s", devinfoObj->SoftwareVersion, cellIntfObj->X_ZYXEL_ModuleSoftwareVersion);
					snprintf(lwm2mfwupdateObj->PkgVersion_7, sizeof(lwm2mfwupdateObj->PkgVersion_7), "%s/%s", devinfoObj->SoftwareVersion, cellIntfObj->X_ZYXEL_ModuleSoftwareVersion);


					if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_FW_UPDATE, &lwm2mFWupdateObjIid, lwm2mfwupdateObj, NULL)) != ZCFG_SUCCESS){
						printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __FUNCTION__, __LINE__, ret);
						zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s Fail with zcfgFeObjStructSet RDM_OID_LWM2M_FW_UPDATE(ret = %d).\n", __FUNCTION__, ret);
					}

					zcfgFeObjStructFree(cellIntfObj);
				}

				zcfgFeObjStructFree(devinfoObj);
			}
			#endif
            /* Only support HTTP based protocols */
            data->protocol_support[0] = 2;
            data->protocol_support[1] = LWM2M_FIRMWARE_PROTOCOL_NULL;
            data->protocol_support[2] = LWM2M_FIRMWARE_PROTOCOL_NULL;
            data->protocol_support[3] = LWM2M_FIRMWARE_PROTOCOL_NULL;

           /* Only support pull method */
           data->delivery_method = 0;
		   if(lwm2mfwupdateObj != NULL){
			   zcfgFeObjStructFree(lwm2mfwupdateObj);
			   lwm2mfwupdateObj = NULL;
		   }
        }
        else
        {
            lwm2m_free(firmwareObj);
            firmwareObj = NULL;
        }
    }

    return firmwareObj;
}

void free_object_firmware(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    while (objectP->instanceList != NULL)
    {
        fw_instance_t * fwInstance = (fw_instance_t *)objectP->instanceList;
		while (fwInstance->update_protocol != NULL)
        {
            resource_ins_list_t * resourceIns = fwInstance->update_protocol;
            fwInstance->update_protocol = fwInstance->update_protocol->next;
            lwm2m_free(resourceIns);
        }
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(fwInstance);
    }
    lwm2m_free(objectP);
}

