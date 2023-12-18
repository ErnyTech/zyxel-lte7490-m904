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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
 * This object is single instance only, and is optional to LWM2M device, it describe the objects such as its
 * IMEI, MSISDN, etc...
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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
#define WAKKAMA_MAPPING_TABLE "/etc/zcfg_wakkama_mapping_table"
#endif

#define PRV_DEVICENAME      "Open Mobile Alliance"
#define PRV_TOOL_VERSION    "Lightweight M2M Client"
#define PRV_IMEI      		"001090123456789"
#define PRV_IMSI  			"001090123456789"
#define PRV_MSISDN  		"001090123456789"

#define PRV_IMSI_MAXLEN   16
#define PRV_TLV_BUFFER_SIZE 128

// Resource Id's:
#define RES_M_DEVICENAME          	0
#define RES_M_TOOL_VERSION          1
#define RES_M_IMEI         			2
#define RES_M_IMSI      			3
#define RES_M_MSISDN                4


typedef struct
{
    char devicename[PRV_IMSI_MAXLEN];
    char toolversion[PRV_IMSI_MAXLEN];
    char imei[PRV_IMSI_MAXLEN];
    char imsi[PRV_IMSI_MAXLEN];
    char msisdn[PRV_IMSI_MAXLEN];
} poweruplog_data_t;

static bool checkPwrUpLogValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, pwr_up_log_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_M_DEVICENAME:
        if(strcmp(targetP->devicename, zcfgParamValue))
        {
            strncpy(targetP->devicename, zcfgParamValue, sizeof(targetP->devicename));
            return true;
        }
        break;

    case RES_M_TOOL_VERSION:
        if(strcmp(targetP->tool_version, zcfgParamValue))
        {
            strncpy(targetP->tool_version, zcfgParamValue, sizeof(targetP->tool_version));
            return true;
        }
        break;

    case RES_M_IMEI:
        if(strcmp(targetP->imei, zcfgParamValue))
        {
            strncpy(targetP->imei, zcfgParamValue, sizeof(targetP->imei));
            return true;
        }
        break;

    case RES_M_IMSI:
        if(strcmp(targetP->imsi, zcfgParamValue))
        {
            strncpy(targetP->imsi, zcfgParamValue, sizeof(targetP->imsi));
            return true;
        }
        break;

    case RES_M_MSISDN:
        if(strcmp(targetP->msisdn, zcfgParamValue))
        {
            strncpy(targetP->msisdn, zcfgParamValue, sizeof(targetP->msisdn));
            return true;
        }
        break;

    default:
        break;
    }
    return false;
}

#if 0
// basic check that the time offset value is at ISO 8601 format
// bug: +12:30 is considered a valid value by this function
static int prv_check_time_offset(char * buffer,
                                 int length)
{
    int min_index;

    if (length != 3 && length != 5 && length != 6) return 0;
    if (buffer[0] != '-' && buffer[0] != '+') return 0;
    switch (buffer[1])
    {
    case '0':
        if (buffer[2] < '0' || buffer[2] > '9') return 0;
        break;
    case '1':
        if (buffer[2] < '0' || (buffer[0] == '-' && buffer[2] > '2') || (buffer[0] == '+' && buffer[2] > '4')) return 0;
        break;
    default:
        return 0;
    }
    switch (length)
    {
    case 3:
        return 1;
    case 5:
        min_index = 3;
        break;
    case 6:
        if (buffer[3] != ':') return 0;
        min_index = 4;
        break;
    default:
        // never happen
        return 0;
    }
    if (buffer[min_index] < '0' || buffer[min_index] > '5') return 0;
    if (buffer[min_index+1] < '0' || buffer[min_index+1] > '9') return 0;

    return 1;
}
#endif

static uint8_t prv_set_value(lwm2m_data_t * dataP,
                             poweruplog_data_t * devDataP)
{
    lwm2m_data_t * subTlvP;
    size_t count;
    size_t i;
    // a simple switch structure is used to respond at the specified resource asked
    switch (dataP->id)
    {
    case RES_M_DEVICENAME:
        if (dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
        lwm2m_data_encode_string(PRV_DEVICENAME, dataP);
        return COAP_205_CONTENT;

    case RES_M_TOOL_VERSION:
        if (dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
        lwm2m_data_encode_string(PRV_TOOL_VERSION, dataP);
        return COAP_205_CONTENT;

    case RES_M_IMEI:
        if (dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
        lwm2m_data_encode_string(PRV_IMEI, dataP);
        return COAP_205_CONTENT;

    case RES_M_IMSI:
        if (dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
        lwm2m_data_encode_string(RES_M_IMSI, dataP);
        return COAP_205_CONTENT;
      
    case RES_M_MSISDN:
        if (dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
        lwm2m_data_encode_string(RES_M_MSISDN, dataP);
        return COAP_205_CONTENT;

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_poweruplog_read(lwm2m_context_t *contextP,
                               uint16_t instanceId,
                               int * numDataP,
                               lwm2m_data_t ** dataArrayP,
                               lwm2m_object_t * objectP)
{
    uint8_t result;
    int i, j;
    int ret = ZCFG_SUCCESS;
    char omaID[16] = {0};
    char desValue[512] = {0};
    lwm2m_data_t * subTlvP;
    size_t count;
    pwr_up_log_instance_t * targetP;

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    targetP = (pwr_up_log_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_M_DEVICENAME,
				RES_M_TOOL_VERSION,
				RES_M_IMEI,
				RES_M_IMSI,
				RES_M_MSISDN
				
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
        case RES_M_DEVICENAME:
        case RES_M_TOOL_VERSION:
		case RES_M_IMEI:
		case RES_M_IMSI:
		case RES_M_MSISDN:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }
                            
            if(checkPwrUpLogValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_string(desValue, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_poweruplog_discover(lwm2m_context_t *contextP,
                                   uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
				RES_M_DEVICENAME,
				RES_M_TOOL_VERSION,
				RES_M_IMEI,
				RES_M_IMSI,
				RES_M_MSISDN
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
            case RES_M_DEVICENAME:
			case RES_M_TOOL_VERSION:
			case RES_M_IMEI:
			case RES_M_IMSI:
			case RES_M_MSISDN:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_poweruplog_write(lwm2m_context_t *contextP,
                                uint16_t instanceId,
                                int numData,
                                lwm2m_data_t * dataArray,
                                lwm2m_object_t * objectP,
                                lwm2m_write_type_t writeType)
{
    int i, j;
    uint8_t result;
    char param[512] = {0};
    char omaID[16] = {0};
    char valstr[32] = {0};
    int64_t iValue;
    uint64_t uValue;


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

    do
    {
        // /* No multiple instance resources */
        // if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        // {
        //     result = COAP_404_NOT_FOUND;
        //     continue;
        // }
        snprintf(omaID, 16, "%d_%d", objectP->objID, (dataArray)[i].id);
        switch (dataArray[i].id)
        {
		case RES_M_DEVICENAME:
		case RES_M_TOOL_VERSION:
		case RES_M_IMEI:
		case RES_M_IMSI:
		case RES_M_MSISDN:
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_poweruplog_execute(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  uint16_t resourceId,
                                  uint8_t * buffer,
                                  int length,
                                  lwm2m_object_t * objectP)
{
    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    switch (resourceId)
    {
    case RES_M_DEVICENAME:
	case RES_M_TOOL_VERSION:
	case RES_M_IMEI:
	case RES_M_IMSI:
	case RES_M_MSISDN:
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

void display_poweruplog_object(lwm2m_object_t * object)
{
    poweruplog_data_t * data = (poweruplog_data_t *)object->userData;
    fprintf(stdout, "  /%u: poweruplog object:\r\n", object->objID);
    if (NULL != data)
    {
        //fprintf(stdout, "    time: %lld, time_offset: %s\r\n",(long long) data->time, data->time_offset);
    }
}

lwm2m_object_t * get_object_poweruplog()
{
    /*
     * The get_object_poweruplog function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * poweruplogObj;

    poweruplogObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != poweruplogObj)
    {

        pwr_up_log_instance_t * pwrUpLogInstance;

        memset(poweruplogObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 3 is the standard ID for the mandatory object "Object device".
         */
        poweruplogObj->objID = LWM2M_POWERUPLOG_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        pwrUpLogInstance = (pwr_up_log_instance_t *)lwm2m_malloc(sizeof(pwr_up_log_instance_t));
        if (NULL == pwrUpLogInstance)
        {
            lwm2m_free(poweruplogObj);
            return NULL;
        }

        memset(pwrUpLogInstance, 0, sizeof(pwr_up_log_instance_t));
        pwrUpLogInstance->instanceId = 0;
        memset(pwrUpLogInstance->devicename, 0, 257);
        memset(pwrUpLogInstance->tool_version, 0, 257);
        memset(pwrUpLogInstance->imei, 0, 257);
        memset(pwrUpLogInstance->imsi, 0, 257);
        memset(pwrUpLogInstance->msisdn, 0, 257);
        poweruplogObj->instanceList = LWM2M_LIST_ADD(poweruplogObj->instanceList, pwrUpLogInstance);

        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        poweruplogObj->readFunc     = prv_poweruplog_read;
        poweruplogObj->discoverFunc = prv_poweruplog_discover;
        poweruplogObj->writeFunc    = prv_poweruplog_write;
        poweruplogObj->executeFunc  = prv_poweruplog_execute;
        poweruplogObj->userData = lwm2m_malloc(sizeof(poweruplog_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables 
         */
        if (NULL != poweruplogObj->userData)
        {
			strcpy(((poweruplog_data_t*)poweruplogObj->userData)->devicename, "NR7102");
            strcpy(((poweruplog_data_t*)poweruplogObj->userData)->toolversion, "lwm2mclient-1.0.0");
			strcpy(((poweruplog_data_t*)poweruplogObj->userData)->imei, "001010123456789");
			strcpy(((poweruplog_data_t*)poweruplogObj->userData)->imsi, "001010123456789");
			strcpy(((poweruplog_data_t*)poweruplogObj->userData)->msisdn, "001010123456789");
        }
        else
        {
            lwm2m_free(poweruplogObj->instanceList);
            lwm2m_free(poweruplogObj);
            poweruplogObj = NULL;
        }
    }

    return poweruplogObj;
}

void free_object_poweruplog(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    while (objectP->instanceList != NULL)
    {
        pwr_up_log_instance_t * pwrUpLogInstance = (pwr_up_log_instance_t *)objectP->instanceList;
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(pwrUpLogInstance);
    }
    lwm2m_free(objectP);
}
#if 0
uint8_t poweruplog_change(lwm2m_data_t * dataArray,
                      lwm2m_object_t * objectP)
{
    uint8_t result;

    switch (dataArray->id)
    {
    case RES_O_BATTERY_LEVEL:
            {
                int64_t value;
                if (1 == lwm2m_data_decode_int(dataArray, &value))
                {
                    if ((0 <= value) && (100 >= value))
                    {
                        ((poweruplog_data_t*)(objectP->userData))->battery_level = value;
                        result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                    }
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }
            }
            break;
        case RES_M_ERROR_CODE:
            if (1 == lwm2m_data_decode_int(dataArray, &((poweruplog_data_t*)(objectP->userData))->error))
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        case RES_O_MEMORY_FREE:
            if (1 == lwm2m_data_decode_int(dataArray, &((poweruplog_data_t*)(objectP->userData))->free_memory))
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;
        }
    
    return result;
}
#endif