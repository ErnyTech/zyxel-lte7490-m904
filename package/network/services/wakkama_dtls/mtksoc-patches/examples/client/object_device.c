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
 * This object is single instance only, and is mandatory to all LWM2M device as it describe the object such as its
 * manufacturer, model, etc...
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

#define PRV_MANUFACTURER      "Open Mobile Alliance"
#define PRV_MODEL_NUMBER      "Lightweight M2M Client"
#define PRV_SERIAL_NUMBER     "345000123"
#define PRV_FIRMWARE_VERSION  "1.0"
#define PRV_POWER_SOURCE_1    1
#define PRV_POWER_SOURCE_2    5
#define PRV_POWER_VOLTAGE_1   3800
#define PRV_POWER_VOLTAGE_2   5000
#define PRV_POWER_CURRENT_1   125
#define PRV_POWER_CURRENT_2   900
#define PRV_BATTERY_LEVEL     100
#define PRV_MEMORY_FREE       15
#define PRV_ERROR_CODE        0
#define PRV_TIME_ZONE         "Europe/Berlin"
#define PRV_BINDING_MODE      "U"

#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128

// Resource Id's:
#define RES_O_MANUFACTURER          0
#define RES_O_MODEL_NUMBER          1
#define RES_O_SERIAL_NUMBER         2
#define RES_O_FIRMWARE_VERSION      3
#define RES_M_REBOOT                4
#define RES_O_FACTORY_RESET         5
// #define RES_O_AVL_POWER_SOURCES     6
// #define RES_O_POWER_SOURCE_VOLTAGE  7
// #define RES_O_POWER_SOURCE_CURRENT  8
// #define RES_O_BATTERY_LEVEL         9
#define RES_O_MEMORY_FREE           10
#define RES_M_ERROR_CODE            11
#define RES_O_RESET_ERROR_CODE      12
#define RES_O_CURRENT_TIME          13
#define RES_O_UTC_OFFSET            14
#define RES_O_TIMEZONE              15
#define RES_M_BINDING_MODES         16
// since TS 20141126-C:
#define RES_O_DEVICE_TYPE           17
#define RES_O_HARDWARE_VERSION      18
#define RES_O_SOFTWARE_VERSION      19
// #define RES_O_BATTERY_STATUS        20
#define RES_O_MEMORY_TOTAL          21
#define RES_O_EXTDEVINFO            22



typedef struct
{
    int64_t free_memory;
    int64_t error;
    int64_t time;
    // uint8_t battery_level;
    char time_offset[PRV_OFFSET_MAXLEN];
} device_data_t;


static bool checkDeviceValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, device_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_O_MANUFACTURER:
        if(strcmp(targetP->manufacturer, zcfgParamValue))
        {
            strncpy(targetP->manufacturer, zcfgParamValue, sizeof(targetP->manufacturer));
            return true;
        }
        break;

    case RES_O_MODEL_NUMBER:
        if(strcmp(targetP->model_number, zcfgParamValue))
        {
            strncpy(targetP->model_number, zcfgParamValue, sizeof(targetP->model_number));
            return true;
        }
        break;

    case RES_O_SERIAL_NUMBER:
        if(strcmp(targetP->serial_number, zcfgParamValue))
        {
            strncpy(targetP->serial_number, zcfgParamValue, sizeof(targetP->serial_number));
            return true;
        }
        break;

    case RES_O_FIRMWARE_VERSION:
        if(strcmp(targetP->firmware_version, zcfgParamValue))
        {
            strncpy(targetP->firmware_version, zcfgParamValue, sizeof(targetP->firmware_version));
            return true;
        }
        break;

    // case RES_O_AVL_POWER_SOURCES:
    //     if(strcmp(targetP->avl_power_sources, zcfgParamValue))
    //     {
    //         strncpy(targetP->avl_power_sources, zcfgParamValue, sizeof(targetP->avl_power_sources));
    //         return true;
    //     }
    //     break;
    // case RES_O_POWER_SOURCE_VOLTAGE:
    //     if(strcmp(targetP->power_source_voltage, zcfgParamValue))
    //     {
    //         strncpy(targetP->power_source_voltage, zcfgParamValue, sizeof(targetP->power_source_voltage));
    //         return true;
    //     }
    //     break;
    // case RES_O_POWER_SOURCE_CURRENT:
    //     if(strcmp(targetP->power_source_current, zcfgParamValue))
    //     {
    //         strncpy(targetP->power_source_current, zcfgParamValue, sizeof(targetP->power_source_current));
    //         return true;
    //     }
    //     break;
    // case RES_O_BATTERY_LEVEL:
    //     if(strcmp(targetP->battery_level, zcfgParamValue))
    //     {
    //         strncpy(targetP->battery_level, zcfgParamValue, sizeof(targetP->battery_level));
    //         return true;
    //     }
    //     break;
    case RES_O_MEMORY_FREE:
        if(strcmp(targetP->memory_free, zcfgParamValue))
        {
            strncpy(targetP->memory_free, zcfgParamValue, sizeof(targetP->memory_free));
            return true;
        }
        break;

    case RES_M_ERROR_CODE:
    {
        if(LWM2M_LIST_FIND(targetP->error_code, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->error_code = LWM2M_LIST_ADD(targetP->error_code, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->error_code, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }    

    case RES_O_CURRENT_TIME:
        if(strcmp(targetP->current_time, zcfgParamValue))
        {
            strncpy(targetP->current_time, zcfgParamValue, sizeof(targetP->current_time));
            return true;
        }
        break;

    case RES_O_UTC_OFFSET:
        if(strcmp(targetP->utc_offset, zcfgParamValue))
        {
            strncpy(targetP->utc_offset, zcfgParamValue, sizeof(targetP->utc_offset));
            return true;
        }
        break;

    case RES_O_TIMEZONE:
        if(strcmp(targetP->timezone, zcfgParamValue))
        {
            strncpy(targetP->timezone, zcfgParamValue, sizeof(targetP->timezone));
            return true;
        }
        break;

    case RES_M_BINDING_MODES:
        if(strcmp(targetP->binding_modes, zcfgParamValue))
        {
            strncpy(targetP->binding_modes, zcfgParamValue, sizeof(targetP->binding_modes));
            return true;
        }
        break;

    // case RES_O_DEVICE_TYPE:
    //     if(strcmp(targetP->device_type, zcfgParamValue))
    //     {
    //         strncpy(targetP->device_type, zcfgParamValue, sizeof(targetP->device_type));
    //         return true;
    //     }
    //     break;
    case RES_O_HARDWARE_VERSION:
        if(strcmp(targetP->hardware_version, zcfgParamValue))
        {
            strncpy(targetP->hardware_version, zcfgParamValue, sizeof(targetP->hardware_version));
            return true;
        }
        break;

    case RES_O_SOFTWARE_VERSION:
        if(strcmp(targetP->software_version, zcfgParamValue))
        {
            strncpy(targetP->software_version, zcfgParamValue, sizeof(targetP->software_version));
            return true;
        }
        break;

    // case RES_O_BATTERY_STATUS:
    //     if(strcmp(targetP->battery_status, zcfgParamValue))
    //     {
    //         strncpy(targetP->battery_status, zcfgParamValue, sizeof(targetP->battery_status));
    //         return true;
    //     }
    //     break;
    case RES_O_MEMORY_TOTAL:
        if(strcmp(targetP->memory_total, zcfgParamValue))
        {
            strncpy(targetP->memory_total, zcfgParamValue, sizeof(targetP->memory_total));
            return true;
        }
        break;

    // case RES_O_EXTDEVINFO:
    //     if(strcmp(targetP->extdevinfo, zcfgParamValue))
    //     {
    //         strncpy(targetP->extdevinfo, zcfgParamValue, sizeof(targetP->extdevinfo));
    //         return true;
    //     }
    //     break;

    default:
        break;
    }
    return false;
}

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

static uint8_t prv_device_read(lwm2m_context_t *contextP,
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
    device_instance_t * targetP;


    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    targetP = (device_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_MANUFACTURER,
                RES_O_MODEL_NUMBER,
                RES_O_SERIAL_NUMBER,
                RES_O_FIRMWARE_VERSION,
                //E: RES_M_REBOOT,
                //E: RES_O_FACTORY_RESET,
                // RES_O_AVL_POWER_SOURCES,
                // RES_O_POWER_SOURCE_VOLTAGE,
                // RES_O_POWER_SOURCE_CURRENT,
                // RES_O_BATTERY_LEVEL,
                RES_O_MEMORY_FREE,
                RES_M_ERROR_CODE,
                //E: RES_O_RESET_ERROR_CODE,
                RES_O_CURRENT_TIME,
                RES_O_UTC_OFFSET,
                RES_O_TIMEZONE,
                RES_M_BINDING_MODES,
				// RES_O_DEVICE_TYPE,
				RES_O_HARDWARE_VERSION,
				RES_O_SOFTWARE_VERSION,
				// RES_O_BATTERY_STATUS,
				RES_O_MEMORY_TOTAL,
				// RES_O_EXTDEVINFO
				
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
		memset(desValue, 0, sizeof(desValue));
        snprintf(omaID, 16, "%d_%d", objectP->objID, (*dataArrayP)[i].id);
        switch ((*dataArrayP)[i].id)
        {
        case RES_O_MANUFACTURER:
        case RES_O_MODEL_NUMBER:
		case RES_O_SERIAL_NUMBER:
		case RES_O_FIRMWARE_VERSION:
		case RES_O_UTC_OFFSET:
		case RES_O_TIMEZONE:
		case RES_M_BINDING_MODES:
		//case RES_O_DEVICE_TYPE:
		case RES_O_HARDWARE_VERSION:
		case RES_O_SOFTWARE_VERSION:
		//case RES_O_EXTDEVINFO:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_string(desValue, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

		//case RES_O_AVL_POWER_SOURCES:
		//case RES_O_POWER_SOURCE_VOLTAGE:
		//case RES_O_POWER_SOURCE_CURRENT:
		//case RES_O_BATTERY_LEVEL:
		case RES_O_MEMORY_FREE:
		//case RES_O_BATTERY_STATUS:
		case RES_O_MEMORY_TOTAL:
			if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

		case RES_O_CURRENT_TIME:
		{
			if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_int(dateTimeToUnixTime(desValue), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;
        }
        //Multi int 
        case RES_M_ERROR_CODE: //OPAL not support, always set 0 for resource instance 0
            subTlvP = lwm2m_data_new(1);
            subTlvP[0].id = 0;
            lwm2m_data_encode_instances(subTlvP, 1, *dataArrayP + i);
            strcpy(desValue, "0");
            if(checkDeviceValueChange((*dataArrayP)[i].id, subTlvP[0].id, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  subTlvP[0].id);
            }
            lwm2m_data_encode_int(0/*No error*/, subTlvP);
            result = COAP_205_CONTENT;
            break;

        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_device_discover(lwm2m_context_t *contextP,
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
            RES_O_MANUFACTURER,
            RES_O_MODEL_NUMBER,
            RES_O_SERIAL_NUMBER,
            RES_O_FIRMWARE_VERSION,
            RES_M_REBOOT,
            RES_O_FACTORY_RESET,
            // RES_O_AVL_POWER_SOURCES,
            // RES_O_POWER_SOURCE_VOLTAGE,
            // RES_O_POWER_SOURCE_CURRENT,
            // RES_O_BATTERY_LEVEL,
            RES_O_MEMORY_FREE,
            RES_M_ERROR_CODE,
            RES_O_RESET_ERROR_CODE,
            RES_O_CURRENT_TIME,
            RES_O_UTC_OFFSET,
            RES_O_TIMEZONE,
            RES_M_BINDING_MODES
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
            case RES_O_MANUFACTURER:
            case RES_O_MODEL_NUMBER:
            case RES_O_SERIAL_NUMBER:
            case RES_O_FIRMWARE_VERSION:
            case RES_M_REBOOT:
            case RES_O_FACTORY_RESET:
            // case RES_O_AVL_POWER_SOURCES:
            // case RES_O_POWER_SOURCE_VOLTAGE:
            // case RES_O_POWER_SOURCE_CURRENT:
            // case RES_O_BATTERY_LEVEL:
            case RES_O_MEMORY_FREE:
            case RES_M_ERROR_CODE:
            case RES_O_RESET_ERROR_CODE:
            case RES_O_CURRENT_TIME:
            case RES_O_UTC_OFFSET:
            case RES_O_TIMEZONE:
            case RES_M_BINDING_MODES:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_device_write(lwm2m_context_t *contextP,
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
        case RES_O_CURRENT_TIME:
            strncpy(param, dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
            if(prv_set_zcfg_value(omaID, -1, -1, LWM2M_TYPE_TIME, param) == ZCFG_SUCCESS)
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        case RES_O_UTC_OFFSET:
        case RES_O_TIMEZONE:
            strncpy(param, dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
            if(prv_set_zcfg_value(omaID, -1, -1, dataArray[i].type, param) == ZCFG_SUCCESS)
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
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_device_execute(lwm2m_context_t *contextP,
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
    case RES_M_REBOOT:
        fprintf(stdout, "\n\t REBOOT\r\n\n");
		zcfgFeReqReboot(NULL);//Jessie Device
        g_reboot = 1;
        return COAP_204_CHANGED;
    case RES_O_FACTORY_RESET:
        fprintf(stdout, "\n\t FACTORY RESET\r\n\n");
		zcfgFeRestoreDefault( NULL );//Jessie Device
        return COAP_204_CHANGED;
    case RES_O_RESET_ERROR_CODE:
        fprintf(stdout, "\n\t RESET ERROR CODE\r\n\n");
        ((device_data_t*)(objectP->userData))->error = 0;
        return COAP_204_CHANGED;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

void display_device_object(lwm2m_object_t * object)
{
    device_data_t * data = (device_data_t *)object->userData;
    fprintf(stdout, "  /%u: Device object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    time: %lld, time_offset: %s\r\n",
                (long long) data->time, data->time_offset);
    }
}

lwm2m_object_t * get_object_device()
{
    /*
     * The get_object_device function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * deviceObj;

    deviceObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != deviceObj)
    {

        device_instance_t * deviceInstance;

        memset(deviceObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 3 is the standard ID for the mandatory object "Object device".
         */
        deviceObj->objID = LWM2M_DEVICE_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        deviceInstance = (device_instance_t *)lwm2m_malloc(sizeof(device_instance_t));
        if (NULL == deviceInstance)
        {
            lwm2m_free(deviceObj);
            return NULL;
        }

        memset(deviceInstance, 0, sizeof(device_instance_t));
        deviceInstance->instanceId = 0;
        memset(deviceInstance->manufacturer, 0, 257);
        memset(deviceInstance->model_number, 0, 257);
        memset(deviceInstance->serial_number, 0, 257);
        memset(deviceInstance->firmware_version, 0, 257);
        // memset(deviceInstance->avl_power_sources, 0, 257);
        // memset(deviceInstance->power_source_voltage, 0, 257);
        // memset(deviceInstance->power_source_current, 0, 257);
        // memset(deviceInstance->battery_level, 0, 257);
        memset(deviceInstance->memory_free, 0, 257);
        memset(deviceInstance->current_time, 0, 257);
        memset(deviceInstance->utc_offset, 0, 257);
        memset(deviceInstance->timezone, 0, 257);
        memset(deviceInstance->binding_modes, 0, 257);
        // memset(deviceInstance->device_type, 0, 257);
        memset(deviceInstance->hardware_version, 0, 257);
        memset(deviceInstance->software_version, 0, 257);
        // memset(deviceInstance->battery_status, 0, 257);
        memset(deviceInstance->memory_total, 0, 257);
        // memset(deviceInstance->extdevinfo, 0, 257);
        deviceObj->instanceList = LWM2M_LIST_ADD(deviceObj->instanceList, deviceInstance);
        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        deviceObj->readFunc     = prv_device_read;
        deviceObj->discoverFunc = prv_device_discover;
        deviceObj->writeFunc    = prv_device_write;
        deviceObj->executeFunc  = prv_device_execute;
        deviceObj->userData = lwm2m_malloc(sizeof(device_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables 
         */
        if (NULL != deviceObj->userData)
        {
            // ((device_data_t*)deviceObj->userData)->battery_level = PRV_BATTERY_LEVEL;
            ((device_data_t*)deviceObj->userData)->free_memory   = PRV_MEMORY_FREE;
            ((device_data_t*)deviceObj->userData)->error = PRV_ERROR_CODE;
            ((device_data_t*)deviceObj->userData)->time  = 1367491215;
            strcpy(((device_data_t*)deviceObj->userData)->time_offset, "+01:00");
        }
        else
        {
            lwm2m_free(deviceObj->instanceList);
            lwm2m_free(deviceObj);
            deviceObj = NULL;
        }
    }

    return deviceObj;
}

void free_object_device(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    while (objectP->instanceList != NULL)
    {
        device_instance_t * deviceInstance = (device_instance_t *)objectP->instanceList;
        while (deviceInstance->error_code != NULL)
        {
            resource_ins_list_t * resourceIns = deviceInstance->error_code;
            deviceInstance->error_code = deviceInstance->error_code->next;
            lwm2m_free(resourceIns);
        }
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(deviceInstance);
    }
    lwm2m_free(objectP);
}

uint8_t device_change(lwm2m_data_t * dataArray,
                      lwm2m_object_t * objectP)
{
    uint8_t result;

    switch (dataArray->id)
    {
    // case RES_O_BATTERY_LEVEL:
    //         {
    //             int64_t value;
    //             if (1 == lwm2m_data_decode_int(dataArray, &value))
    //             {
    //                 if ((0 <= value) && (100 >= value))
    //                 {
    //                     ((device_data_t*)(objectP->userData))->battery_level = value;
    //                     result = COAP_204_CHANGED;
    //                 }
    //                 else
    //                 {
    //                     result = COAP_400_BAD_REQUEST;
    //                 }
    //             }
    //             else
    //             {
    //                 result = COAP_400_BAD_REQUEST;
    //             }
    //         }
    //         break;
        case RES_M_ERROR_CODE:
            if (1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->error))
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        case RES_O_MEMORY_FREE:
            if (1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->free_memory))
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
