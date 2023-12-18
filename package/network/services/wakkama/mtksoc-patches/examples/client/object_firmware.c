/*******************************************************************************
 *
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
	char package_uri[257];
} fw_instance_t;

bool checkFwValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, fw_instance_t * targetP, char * zcfgParamValue)
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

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    targetP = (fw_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        *dataArrayP = lwm2m_data_new(6);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 6;
        (*dataArrayP)[0].id = 3;
        (*dataArrayP)[1].id = 5;
        (*dataArrayP)[2].id = 6;
        (*dataArrayP)[3].id = 7;
        (*dataArrayP)[4].id = 8;
        (*dataArrayP)[5].id = 9;
    }

    i = 0;
    do
    {
        switch ((*dataArrayP)[i].id)
        {
        case RES_M_PACKAGE:
        case RES_M_UPDATE:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

        case RES_M_PACKAGE_URI:
			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);
			if(ret == ZCFG_SUCCESS){
                
                if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, lwm2mfwupdateObj->Package_URI_1))
                {
                    setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
                }

				lwm2m_data_encode_string(lwm2mfwupdateObj->Package_URI_1, *dataArrayP + i);
				// printf("%s(%d) Now read the URI %s.\n", __func__, __LINE__, lwm2mfwupdateObj->Package_URI_1);
			}
            result = COAP_205_CONTENT;
            break;
        case RES_M_STATE:
            // firmware update state (int)
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            lwm2m_data_encode_int(data->state, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        case RES_M_UPDATE_RESULT:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            lwm2m_data_encode_int(data->result, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        case RES_O_PKG_NAME:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            lwm2m_data_encode_string(data->pkg_name, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        case RES_O_PKG_VERSION:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            lwm2m_data_encode_string(data->pkg_version, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

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
                    lwm2m_data_encode_int(0, subTlvP + ri);
                }
            }
            result = COAP_205_CONTENT;
            break;
        }

        case RES_M_UPDATE_METHOD:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            lwm2m_data_encode_int(data->delivery_method, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_firmware_write(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  lwm2m_write_type_t writeType)
{
    int i, j;
	int ret = ZCFG_SUCCESS;
	char omaID[16] = {0};
	int res = 0, err = 0;
	servInfo_t servInfoPtr;
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
			// printf("The URI(%d) is %s.\n", dataArray[i].value.asBuffer.length, (char*)dataArray[i].value.asBuffer.buffer);
			dataArray[i].value.asBuffer.buffer[dataArray[i].value.asBuffer.length] = '\0';
			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_LWM2M_FW_UPDATE, &objIid, (void **)&lwm2mfwupdateObj);
			if(ret == ZCFG_SUCCESS){
				snprintf(lwm2mfwupdateObj->Package_URI_1, dataArray[i].value.asBuffer.length + 1, "%s", (char*)dataArray[i].value.asBuffer.buffer);
				if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_FW_UPDATE, &objIid, lwm2mfwupdateObj, NULL)) != ZCFG_SUCCESS){
					printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
				}

				//Parsing the URI
				snprintf(servInfoPtr.url, sizeof(servInfoPtr.url), "%s",  (char*)dataArray[i].value.asBuffer.buffer);
				if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "DualStack")) == COAP_NO_ERROR){
					cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);
					// printf("%s(%d) input url=%s, parser result hostname=%s, ip=%s, port=%d, path=%s\n", __func__, __LINE__, servInfoPtr.url, thehostname, ip_addr, servInfoPtr.port, servInfoPtr.path );

					//Get the file via http get
					if(zHttpCustomrequest_post(servInfoPtr.url, thehostname, servInfoPtr.port, servInfoPtr.path, FIRMWARE_STORE_PATH)){
						//downloaded state(=2)
						data->state = 2;
						printf("%s(%d) Get file %s successfully.\n", __func__, __LINE__, FIRMWARE_STORE_PATH);
					}//end of zHttpCustomrequest_post
					else{//fail to download file
						printf("%s(%d) Fail to get file %s.\n", __func__, __LINE__, FIRMWARE_STORE_PATH);
							//back to idle state
							data->state = 0;
						//4: Connection lost during downloading process.
						data->result = 4;
					}
				}//end of CWMP Parsing
				else{
					printf("%s: cwmp_parseurl(%d) failed\n", __FUNCTION__, err);
					//back to idle state
					data->state = 0;
					//Invalid URI.
					data->result = 7;
				}
				zcfgFeObjStructFree(lwm2mfwupdateObj);
			}
			else{
				printf("%s(%d) Fail with zcfgFeObjStructGet(ret = %d).\n", __func__, __LINE__, ret);
			}

            // URL for download the firmware
            result = COAP_204_CHANGED;
            break;

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
		//2: downloaded state(=2)
        if (data->state == 2)
        {
			//3: updating state(=3)
			data->state = 3;
            // trigger your firmware download and update logic
            fprintf(stdout, "\n\t FIRMWARE UPDATE\r\n\n");
			//if(zcfgFeFwUpgrade(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS){
			if(zcfgFeFwUpgradeWrite(FIRMWARE_STORE_PATH) == ZCFG_SUCCESS){ //no reboot after updating
				printf("%s(%d) zcfgFeFwUpgrade() return ZCFG_SUCCESS.\n", __func__, __LINE__);
				printf("%s(%d) Now is going to reboot...\n", __func__, __LINE__);
				//0: back to idle state
				data->state = 0;
				//1: Firmware updated successfully.
				data->result = 1;
				zcfgFeReqReboot(NULL);
			}
			else{//fail to upgrade
				printf("%s(%d) zcfgFeFwUpgrade() return ZCFG_FAIL.\n", __func__, __LINE__);
				//0: back to idle state
				data->state = 0;
				//8: Firmware update failed.
				data->result = 8;
			}
            return COAP_204_CHANGED;
        }
        else
        {
            // firmware update already running
            return COAP_400_BAD_REQUEST;
        }
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
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
	objIndex_t objIid;
	int ret = 0;

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
        firmwareObj->instanceList = LWM2M_LIST_ADD(firmwareObj->instanceList, fwInstance);

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        firmwareObj->readFunc    = prv_firmware_read;
        firmwareObj->writeFunc   = prv_firmware_write;
        firmwareObj->executeFunc = prv_firmware_execute;
        firmwareObj->userData    = lwm2m_malloc(sizeof(firmware_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            firmware_data_t *data = (firmware_data_t*)(firmwareObj->userData);

            data->state = 0;
            //data->state = 1;
            data->result = 0;
			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_DEV_INFO, &objIid, (void **)&devinfoObj);
			if(ret == ZCFG_SUCCESS){
				strncpy(data->pkg_name, devinfoObj->SoftwareVersion, sizeof(data->pkg_name));
				strncpy(data->pkg_version, devinfoObj->SoftwareVersion, sizeof(data->pkg_version));
				zcfgFeObjStructFree(devinfoObj);
			}
			else{
				strcpy(data->pkg_name, "lwm2mclient");
				strcpy(data->pkg_version, "1.0");
			}

            /* Only support HTTP based protocols */
            data->protocol_support[0] = 2;
            data->protocol_support[1] = LWM2M_FIRMWARE_PROTOCOL_NULL;
            data->protocol_support[2] = LWM2M_FIRMWARE_PROTOCOL_NULL;
            data->protocol_support[3] = LWM2M_FIRMWARE_PROTOCOL_NULL;

           /* Only support pull method */
           data->delivery_method = 0;
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
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(fwInstance);
    }
    lwm2m_free(objectP);
}

