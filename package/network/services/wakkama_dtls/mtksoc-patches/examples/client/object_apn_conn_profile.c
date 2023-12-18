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
#define RES_O_PROFILE_NAME                          0
#define RES_O_APN                                   1
#define RES_O_AUTO_SELECT_APN                       2
#define RES_O_ENABLE_STATUS                         3
#define RES_O_AUTHEN_TYPE                           4
#define RES_O_USER_NAME                             5
#define RES_O_SECRET                                6
#define RES_O_RECONNECT_SCHEDULE                    7
#define RES_M_VALIDITY_MCC_MNC                      8
#define RES_M_CONNECTION_ESTABLISH_TIME             9
#define RES_M_CONNECTION_ESTABLISH_RESULT           10
#define RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE     11
#define RES_M_CONNECTION_END_TIME                   12
#define RES_O_TOTAL_BYTES_SENT                      13
#define RES_O_TOTAL_BYTES_RECEIVED                  14
#define RES_M_IP_ADDR                               15
#define RES_M_PREFIX_LEN                            16
#define RES_M_SUBNET_MASK                           17
#define RES_M_GATEWAY                               18
#define RES_M_PRI_DNS_ADDR                          19
#define RES_M_SEC_DNS_ADDR                          20
#define RES_O_QCI                                   21
#define RES_O_VENDOR_SPECIFIC_EXTENSIONS            22
#define RES_O_TOTAL_PACKETS_SENT                    23
#define RES_O_PDN_TYPE                              24
#define RES_O_APN_RATE_CTL                          25
#define RES_O_SERVING_PLMN_RATE_CTL                 26
#define RES_O_UPLINK_TIME_UNIT                      27
#define RES_O_APN_RATE_CTL_4_EXCEPTION_DATA         28
#define RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT   29
#define RES_M_SUP_RAT_TYPES                         30
#define RES_M_RDS_APP_ID                            31
#define RES_M_RDS_DESTINATION_PORT                  32
#define RES_M_RDS_SRC_PORT                          33

static bool checkApnConnProfileValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, apn_conn_profile_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_O_PROFILE_NAME:
        if(strcmp(targetP->profile_name, zcfgParamValue))
        {
            strncpy(targetP->profile_name, zcfgParamValue, sizeof(targetP->profile_name));
            return true;
        }
        break;

    case RES_O_APN:
        if(strcmp(targetP->apn, zcfgParamValue))
        {
            strncpy(targetP->apn, zcfgParamValue, sizeof(targetP->apn));
            return true;
        }
        break;
        
    case RES_O_AUTO_SELECT_APN:
        if(strcmp(targetP->auto_select_apn, zcfgParamValue))
        {
            strncpy(targetP->auto_select_apn, zcfgParamValue, sizeof(targetP->auto_select_apn));
            return true;
        }
        break;

    case RES_O_ENABLE_STATUS:
        if(strcmp(targetP->enable_status, zcfgParamValue))
        {
            strncpy(targetP->enable_status, zcfgParamValue, sizeof(targetP->enable_status));
            return true;
        }
        break;

    case RES_O_AUTHEN_TYPE:
        if(strcmp(targetP->authen_type, zcfgParamValue))
        {
            strncpy(targetP->authen_type, zcfgParamValue, sizeof(targetP->authen_type));
            return true;
        }
        break;

    case RES_O_USER_NAME:
        if(strcmp(targetP->user_name, zcfgParamValue))
        {
            strncpy(targetP->user_name, zcfgParamValue, sizeof(targetP->user_name));
            return true;
        }
        break;

    case RES_O_SECRET:
        if(strcmp(targetP->secret, zcfgParamValue))
        {
            strncpy(targetP->secret, zcfgParamValue, sizeof(targetP->secret));
            return true;
        }
        break;

    case RES_O_RECONNECT_SCHEDULE:
        if(strcmp(targetP->reconnect_schedule, zcfgParamValue))
        {
            strncpy(targetP->reconnect_schedule, zcfgParamValue, sizeof(targetP->reconnect_schedule));
            return true;
        }
        break;

    case RES_M_VALIDITY_MCC_MNC:
    {
        if(LWM2M_LIST_FIND(targetP->validity_mcc_mnc, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->validity_mcc_mnc = LWM2M_LIST_ADD(targetP->validity_mcc_mnc, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->validity_mcc_mnc, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_CONNECTION_ESTABLISH_TIME:
    {
        if(LWM2M_LIST_FIND(targetP->connection_establish_time, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->connection_establish_time = LWM2M_LIST_ADD(targetP->connection_establish_time, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->connection_establish_time, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_CONNECTION_ESTABLISH_RESULT:
    {
        if(LWM2M_LIST_FIND(targetP->connection_establish_result, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->connection_establish_result = LWM2M_LIST_ADD(targetP->connection_establish_result, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->connection_establish_result, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE:
    {
        if(LWM2M_LIST_FIND(targetP->connection_establish_reject_cause, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->connection_establish_reject_cause = LWM2M_LIST_ADD(targetP->connection_establish_reject_cause, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->connection_establish_reject_cause, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_CONNECTION_END_TIME:
    {
        if(LWM2M_LIST_FIND(targetP->connection_end_time, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->connection_end_time = LWM2M_LIST_ADD(targetP->connection_end_time, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->connection_end_time, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_O_TOTAL_BYTES_SENT:
        if(strcmp(targetP->total_bytes_sent, zcfgParamValue))
        {
            strncpy(targetP->total_bytes_sent, zcfgParamValue, sizeof(targetP->total_bytes_sent));
            return true;
        }
        break;

    case RES_O_TOTAL_BYTES_RECEIVED:
        if(strcmp(targetP->total_bytes_received, zcfgParamValue))
        {
            strncpy(targetP->total_bytes_received, zcfgParamValue, sizeof(targetP->total_bytes_received));
            return true;
        }
        break;

    case RES_M_IP_ADDR:
    {
        if(LWM2M_LIST_FIND(targetP->ip_addr, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->ip_addr = LWM2M_LIST_ADD(targetP->ip_addr, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->ip_addr, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }  

    case RES_M_PREFIX_LEN:
    {
        if(LWM2M_LIST_FIND(targetP->prefix_len, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->prefix_len = LWM2M_LIST_ADD(targetP->prefix_len, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->prefix_len, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_SUBNET_MASK:
    {
        if(LWM2M_LIST_FIND(targetP->subnet_mask, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->subnet_mask = LWM2M_LIST_ADD(targetP->subnet_mask, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->subnet_mask, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_GATEWAY:
    {
        if(LWM2M_LIST_FIND(targetP->gateway, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->gateway = LWM2M_LIST_ADD(targetP->gateway, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->gateway, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_PRI_DNS_ADDR:
    {
        if(LWM2M_LIST_FIND(targetP->pri_dns_addr, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->pri_dns_addr = LWM2M_LIST_ADD(targetP->pri_dns_addr, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->pri_dns_addr, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_SEC_DNS_ADDR:
    {
        if(LWM2M_LIST_FIND(targetP->sec_dns_addr, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->sec_dns_addr = LWM2M_LIST_ADD(targetP->sec_dns_addr, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->sec_dns_addr, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_O_QCI:
        if(strcmp(targetP->qci, zcfgParamValue))
        {
            strncpy(targetP->qci, zcfgParamValue, sizeof(targetP->qci));
            return true;
        }
        break;

    case RES_O_VENDOR_SPECIFIC_EXTENSIONS:
        if(strcmp(targetP->vendor_specific_extensions, zcfgParamValue))
        {
            strncpy(targetP->vendor_specific_extensions, zcfgParamValue, sizeof(targetP->vendor_specific_extensions));
            return true;
        }
        break;

    case RES_O_TOTAL_PACKETS_SENT:
        if(strcmp(targetP->total_packets_sent, zcfgParamValue))
        {
            strncpy(targetP->total_packets_sent, zcfgParamValue, sizeof(targetP->total_packets_sent));
            return true;
        }
        break;
    case RES_O_PDN_TYPE:
        if(strcmp(targetP->pdn_type, zcfgParamValue))
        {
            strncpy(targetP->pdn_type, zcfgParamValue, sizeof(targetP->pdn_type));
            return true;
        }
        break;

    case RES_O_APN_RATE_CTL:
        if(strcmp(targetP->apn_rate_ctl, zcfgParamValue))
        {
            strncpy(targetP->apn_rate_ctl, zcfgParamValue, sizeof(targetP->apn_rate_ctl));
            return true;
        }
        break;

    case RES_O_SERVING_PLMN_RATE_CTL:
        if(strcmp(targetP->serving_plmn_rate_ctl, zcfgParamValue))
        {
            strncpy(targetP->serving_plmn_rate_ctl, zcfgParamValue, sizeof(targetP->serving_plmn_rate_ctl));
            return true;
        }
        break;

    case RES_O_UPLINK_TIME_UNIT:
        if(strcmp(targetP->uplink_time_unit, zcfgParamValue))
        {
            strncpy(targetP->uplink_time_unit, zcfgParamValue, sizeof(targetP->uplink_time_unit));
            return true;
        }
        break;

    case RES_O_APN_RATE_CTL_4_EXCEPTION_DATA:
        if(strcmp(targetP->apn_rate_ctl_4_exception_data, zcfgParamValue))
        {
            strncpy(targetP->apn_rate_ctl_4_exception_data, zcfgParamValue, sizeof(targetP->apn_rate_ctl_4_exception_data));
            return true;
        }
        break;

    case RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT:
        if(strcmp(targetP->apn_exception_data_uplink_time_unit, zcfgParamValue))
        {
            strncpy(targetP->apn_exception_data_uplink_time_unit, zcfgParamValue, sizeof(targetP->apn_exception_data_uplink_time_unit));
            return true;
        }
        break;        

    case RES_M_SUP_RAT_TYPES:
    {
        if(LWM2M_LIST_FIND(targetP->sup_rat_types, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->sup_rat_types = LWM2M_LIST_ADD(targetP->sup_rat_types, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->sup_rat_types, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_RDS_APP_ID:
    {
        if(LWM2M_LIST_FIND(targetP->rds_app_id, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->rds_app_id = LWM2M_LIST_ADD(targetP->rds_app_id, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->rds_app_id, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_RDS_DESTINATION_PORT:
    {
        if(LWM2M_LIST_FIND(targetP->rds_destination_port, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->rds_destination_port = LWM2M_LIST_ADD(targetP->rds_destination_port, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->rds_destination_port, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_RDS_SRC_PORT:
    {
        if(LWM2M_LIST_FIND(targetP->rds_src_port, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->rds_src_port = LWM2M_LIST_ADD(targetP->rds_src_port, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->rds_src_port, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    default:
        break;
    }
    return false;
}

static uint8_t prv_apn_conn_read(lwm2m_context_t *contextP,
                               uint16_t instanceId,
                               int * numDataP,
                               lwm2m_data_t ** dataArrayP,
                               lwm2m_object_t * objectP)
{
    uint8_t result;
    int i, j;
    bool en = false;
    int ret = ZCFG_SUCCESS;
    char omaID[16] = {0};
    char desValue[512] = {0};
    lwm2m_data_t * subTlvP;
    size_t count;
    apn_conn_profile_instance_t * targetP;

    
    /* unused parameter */
    (void)contextP;

    targetP = (apn_conn_profile_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_O_PROFILE_NAME,
            RES_O_APN,
            RES_O_AUTO_SELECT_APN,
            RES_O_ENABLE_STATUS,
            RES_O_AUTHEN_TYPE,
            RES_O_USER_NAME,
            RES_O_SECRET,
            RES_O_RECONNECT_SCHEDULE,
            RES_M_VALIDITY_MCC_MNC,
            RES_M_CONNECTION_ESTABLISH_TIME,
            RES_M_CONNECTION_ESTABLISH_RESULT,
            RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE,
            RES_M_CONNECTION_END_TIME,
            RES_O_TOTAL_BYTES_SENT,
            RES_O_TOTAL_BYTES_RECEIVED,
            RES_M_IP_ADDR,
            RES_M_PREFIX_LEN,
            RES_M_SUBNET_MASK,
            RES_M_GATEWAY,
            RES_M_PRI_DNS_ADDR,
            RES_M_SEC_DNS_ADDR,
            RES_O_QCI,
            RES_O_VENDOR_SPECIFIC_EXTENSIONS,
            RES_O_TOTAL_PACKETS_SENT,
            RES_O_PDN_TYPE,
            RES_O_APN_RATE_CTL,
            RES_O_SERVING_PLMN_RATE_CTL,
            RES_O_UPLINK_TIME_UNIT,
            RES_O_APN_RATE_CTL_4_EXCEPTION_DATA,
            RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT,
            RES_M_SUP_RAT_TYPES,
            RES_M_RDS_APP_ID,
            RES_M_RDS_DESTINATION_PORT,
            RES_M_RDS_SRC_PORT
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
        // String
        case RES_O_PROFILE_NAME:
        case RES_O_APN:
        case RES_O_USER_NAME:
        case RES_O_SECRET:
        case RES_O_RECONNECT_SCHEDULE:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkApnConnProfileValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_string(desValue, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;
        
        //Object Link
        case RES_O_VENDOR_SPECIFIC_EXTENSIONS:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            uint16_t objectId = 0;
            uint16_t objectInstanceId = 0;
            if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkApnConnProfileValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
            }

            prv__zcfg_decode_objlink(desValue, &objectId, &objectInstanceId);
            lwm2m_data_encode_objlink(objectId, objectId, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        //Int
        case RES_O_AUTHEN_TYPE:
        case RES_O_TOTAL_BYTES_SENT:
        case RES_O_TOTAL_BYTES_RECEIVED:
        case RES_O_QCI:
        case RES_O_TOTAL_PACKETS_SENT:
        case RES_O_PDN_TYPE:
        case RES_O_APN_RATE_CTL:
        case RES_O_APN_RATE_CTL_4_EXCEPTION_DATA:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkApnConnProfileValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        //uint
        case RES_O_SERVING_PLMN_RATE_CTL:
        case RES_O_UPLINK_TIME_UNIT:
        case RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkApnConnProfileValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
            }
            
            lwm2m_data_encode_uint(strtoul(desValue, NULL, 0), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        //bool
        case RES_O_AUTO_SELECT_APN:
        case RES_O_ENABLE_STATUS:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkApnConnProfileValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
            }
            
            lwm2m_data_encode_bool((atoi(desValue)) ? true : false, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        //multi time
        case RES_M_CONNECTION_ESTABLISH_TIME:
        case RES_M_CONNECTION_END_TIME:
        {
            setResourceInsToData(omaID, instanceId, &subTlvP, &count);
            lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

            for (j = 0; j < count; j++)
            {
                if((ret = prv_get_zcfg_value(omaID, instanceId, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkApnConnProfileValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  j);
                }
                
                lwm2m_data_encode_int(dateTimeToUnixTime(desValue), &subTlvP[j]);
            }
            result = COAP_205_CONTENT;
            break;
        }

        //Multi String
        case RES_M_VALIDITY_MCC_MNC:
        case RES_M_IP_ADDR:
        case RES_M_PREFIX_LEN:
        case RES_M_SUBNET_MASK:
        case RES_M_GATEWAY:
        case RES_M_PRI_DNS_ADDR:
        case RES_M_SEC_DNS_ADDR:
        case RES_M_RDS_APP_ID:
        {
            setResourceInsToData(omaID, instanceId, &subTlvP, &count);
            lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

            for (j = 0; j < count; j++)
            {

                if((ret = prv_get_zcfg_value(omaID, instanceId, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkApnConnProfileValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  j);
                }

                lwm2m_data_encode_string(desValue, &subTlvP[j]);

            }
            result = COAP_205_CONTENT;
            break;
        }

        //Multi Int
        case RES_M_CONNECTION_ESTABLISH_RESULT:
        case RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE:
        case RES_M_RDS_DESTINATION_PORT:
        case RES_M_RDS_SRC_PORT:
        {
            setResourceInsToData(omaID, instanceId, &subTlvP, &count);
            lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

            for (j = 0; j < count; j++)
            {
                if((ret = prv_get_zcfg_value(omaID, instanceId, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkApnConnProfileValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  j);
                }
                
                lwm2m_data_encode_int(atoi(desValue), &subTlvP[j]);
            }
            result = COAP_205_CONTENT;
            break;
        }

        //Multi uint
        case RES_M_SUP_RAT_TYPES:
        {
            setResourceInsToData(omaID, instanceId, &subTlvP, &count);
            lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

            for (j = 0; j < count; j++)
            {
                if((ret = prv_get_zcfg_value(omaID, instanceId, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkApnConnProfileValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  j);
                }
                
                lwm2m_data_encode_uint(strtoul(desValue, NULL, 0), &subTlvP[j]);
            }
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

// TODO
static uint8_t prv_apn_conn_discover(lwm2m_context_t *contextP,
                                   uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;

    /* unused parameter */
    (void)contextP;

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_O_PROFILE_NAME,
            RES_O_APN,
            RES_O_AUTO_SELECT_APN,
            RES_O_ENABLE_STATUS,
            RES_O_AUTHEN_TYPE,
            RES_O_USER_NAME,
            RES_O_SECRET,
            RES_O_RECONNECT_SCHEDULE,
            RES_M_VALIDITY_MCC_MNC,
            RES_M_CONNECTION_ESTABLISH_TIME,
            RES_M_CONNECTION_ESTABLISH_RESULT,
            RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE,
            RES_M_CONNECTION_END_TIME,
            RES_O_TOTAL_BYTES_SENT,
            RES_O_TOTAL_BYTES_RECEIVED,
            RES_M_IP_ADDR,
            RES_M_PREFIX_LEN,
            RES_M_SUBNET_MASK,
            RES_M_GATEWAY,
            RES_M_PRI_DNS_ADDR,
            RES_M_SEC_DNS_ADDR,
            RES_O_QCI,
            RES_O_VENDOR_SPECIFIC_EXTENSIONS,
            RES_O_TOTAL_PACKETS_SENT,
            RES_O_PDN_TYPE,
            RES_O_APN_RATE_CTL,
            RES_O_SERVING_PLMN_RATE_CTL,
            RES_O_UPLINK_TIME_UNIT,
            RES_O_APN_RATE_CTL_4_EXCEPTION_DATA,
            RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT,
            RES_M_SUP_RAT_TYPES,
            RES_M_RDS_APP_ID,
            RES_M_RDS_DESTINATION_PORT,
            RES_M_RDS_SRC_PORT
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
                case RES_O_PROFILE_NAME:
                case RES_O_APN:
                case RES_O_AUTO_SELECT_APN:
                case RES_O_ENABLE_STATUS:
                case RES_O_AUTHEN_TYPE:
                case RES_O_USER_NAME:
                case RES_O_SECRET:
                case RES_O_RECONNECT_SCHEDULE:
                case RES_M_VALIDITY_MCC_MNC:
                case RES_M_CONNECTION_ESTABLISH_TIME:
                case RES_M_CONNECTION_ESTABLISH_RESULT:
                case RES_M_CONNECTION_ESTABLISH_REJECT_CAUSE:
                case RES_M_CONNECTION_END_TIME:
                case RES_O_TOTAL_BYTES_SENT:
                case RES_O_TOTAL_BYTES_RECEIVED:
                case RES_M_IP_ADDR:
                case RES_M_PREFIX_LEN:
                case RES_M_SUBNET_MASK:
                case RES_M_GATEWAY:
                case RES_M_PRI_DNS_ADDR:
                case RES_M_SEC_DNS_ADDR:
                case RES_O_QCI:
                case RES_O_VENDOR_SPECIFIC_EXTENSIONS:
                case RES_O_TOTAL_PACKETS_SENT:
                case RES_O_PDN_TYPE:
                case RES_O_APN_RATE_CTL:
                case RES_O_SERVING_PLMN_RATE_CTL:
                case RES_O_UPLINK_TIME_UNIT:
                case RES_O_APN_RATE_CTL_4_EXCEPTION_DATA:
                case RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT:
                case RES_M_SUP_RAT_TYPES:
                case RES_M_RDS_APP_ID:
                case RES_M_RDS_DESTINATION_PORT:
                case RES_M_RDS_SRC_PORT:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_apn_conn_write(lwm2m_context_t *contextP,
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
    bool bValue = false;
    lwm2m_data_t * subTlvP;
    size_t count;


    /* unused parameter */
    (void)contextP;

    // All write types are treated the same here
    (void)writeType;

    i = 0;
    do
    {
		memset(param, 0, sizeof(param));
        snprintf(omaID, 16, "%d_%d", objectP->objID, (dataArray)[i].id);
        switch (dataArray[i].id)
        {
        //String
        case RES_O_PROFILE_NAME:
        case RES_O_APN:
        case RES_O_USER_NAME:
        case RES_O_SECRET:
        case RES_O_RECONNECT_SCHEDULE:
            strncpy(param, dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
            if(prv_set_zcfg_value(omaID, instanceId, -1, dataArray[i].type, param) == ZCFG_SUCCESS)
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        //Int
        case RES_O_AUTHEN_TYPE:
        case RES_O_PDN_TYPE:
            lwm2m_data_decode_int(&dataArray[i], &iValue);
            sprintf(param, "%d", iValue);
            if(prv_set_zcfg_value(omaID, instanceId, -1, LWM2M_TYPE_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE,but this case type should be LWM2M_TYPE_INTEGER, I dont know why
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        //uint
        case RES_O_SERVING_PLMN_RATE_CTL:
        case RES_O_UPLINK_TIME_UNIT:
        case RES_O_APN_EXCEPTION_DATA_UPLINK_TIME_UNIT:
            lwm2m_data_decode_uint(&dataArray[i], &uValue);
            sprintf(param, "%u", uValue);
            if(prv_set_zcfg_value(omaID, instanceId, -1, LWM2M_TYPE_UNSIGNED_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE but this case type should be LWM2M_TYPE_UNSIGNED_INTEGER, I dont know why
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        //bool
        case RES_O_AUTO_SELECT_APN:
        case RES_O_ENABLE_STATUS:
            lwm2m_data_decode_bool(&dataArray[i], &bValue);
            sprintf(param, "%d", (bValue == true) ? 1 : 0);
            if(prv_set_zcfg_value(omaID, instanceId, -1, LWM2M_TYPE_BOOLEAN, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE but this case type should be LWM2M_TYPE_BOOLEAN, I dont know why
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        //multi String
        case RES_M_VALIDITY_MCC_MNC:
        case RES_M_IP_ADDR:
        case RES_M_PREFIX_LEN:
        case RES_M_SUBNET_MASK:
        case RES_M_GATEWAY:
        case RES_M_PRI_DNS_ADDR:
        case RES_M_SEC_DNS_ADDR:
        case RES_M_RDS_APP_ID:
        {
            if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
            {
                count = dataArray[i].value.asChildren.count;
                subTlvP = dataArray[i].value.asChildren.array;
                for (j = 0; j < count; j++)
                {
                    if(prv_add_zcfg_resource_ins(omaID, instanceId, subTlvP[j].id) != ZCFG_SUCCESS)
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                    strncpy(param, subTlvP[j].value.asBuffer.buffer, subTlvP[j].value.asBuffer.length);
                    if(prv_set_zcfg_value(omaID, instanceId, subTlvP[j].id, subTlvP[j].type, param) == ZCFG_SUCCESS)
                    {
                        result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        //multi Int
        case RES_M_RDS_DESTINATION_PORT:
        case RES_M_RDS_SRC_PORT:
        {
            if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
            {
                count = dataArray[i].value.asChildren.count;
                subTlvP = dataArray[i].value.asChildren.array;
                for (j = 0; j < count; j++)
                {
                    if(prv_add_zcfg_resource_ins(omaID, instanceId, subTlvP[j].id) != ZCFG_SUCCESS)
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                    lwm2m_data_decode_int(&subTlvP[j], &iValue);
                    sprintf(param, "%d", iValue);
                    if(prv_set_zcfg_value(omaID, instanceId, subTlvP[j].id, LWM2M_TYPE_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE,but this case type should be LWM2M_TYPE_INTEGER, I dont know why
                    {
                        result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        //multi uint
        case RES_M_SUP_RAT_TYPES:
        {
            if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
            {
                count = dataArray[i].value.asChildren.count;
                subTlvP = dataArray[i].value.asChildren.array;
                for (j = 0; j < count; j++)
                {
                    if(prv_add_zcfg_resource_ins(omaID, instanceId, subTlvP[j].id) != ZCFG_SUCCESS)
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                    lwm2m_data_decode_uint(&subTlvP[j], &uValue);
                    sprintf(param, "%u", uValue);
                    if(prv_set_zcfg_value(omaID, instanceId, subTlvP[j].id, LWM2M_TYPE_UNSIGNED_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE,but this case type should be LWM2M_TYPE_INTEGER, I dont know why
                    {
                        result = COAP_204_CHANGED;
                    }
                    else
                    {
                        result = COAP_400_BAD_REQUEST;
                        break;
                    }
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}
// TODO
static uint8_t prv_apn_conn_execute(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  uint16_t resourceId,
                                  uint8_t * buffer,
                                  int length,
                                  lwm2m_object_t * objectP)
{
    /* unused parameter */
    (void)contextP;

    if (length != 0) return COAP_400_BAD_REQUEST;

    switch (resourceId)
    {
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

static uint8_t prv_apn_conn_delete(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  lwm2m_object_t * objectP)
{
    apn_conn_profile_instance_t * apnConnInstance;
    char omaID[16] = {0};

    /* unused parameter */
    (void)contextP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, instanceId, &apnConnInstance);
    if (NULL == apnConnInstance) return COAP_404_NOT_FOUND;


    snprintf(omaID, 16, "%d", objectP->objID);

    prv_del_zcfg_value(omaID, instanceId);

    while (apnConnInstance->validity_mcc_mnc != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->validity_mcc_mnc;
        apnConnInstance->validity_mcc_mnc = apnConnInstance->validity_mcc_mnc->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->connection_establish_time != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->connection_establish_time;
        apnConnInstance->connection_establish_time = apnConnInstance->connection_establish_time->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->connection_establish_result != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->connection_establish_result;
        apnConnInstance->connection_establish_result = apnConnInstance->connection_establish_result->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->connection_establish_reject_cause != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->connection_establish_reject_cause;
        apnConnInstance->connection_establish_reject_cause = apnConnInstance->connection_establish_reject_cause->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->connection_end_time != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->connection_end_time;
        apnConnInstance->connection_end_time = apnConnInstance->connection_end_time->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->ip_addr != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->ip_addr;
        apnConnInstance->ip_addr = apnConnInstance->ip_addr->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->prefix_len != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->prefix_len;
        apnConnInstance->prefix_len = apnConnInstance->prefix_len->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->subnet_mask != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->subnet_mask;
        apnConnInstance->subnet_mask = apnConnInstance->subnet_mask->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->gateway != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->gateway;
        apnConnInstance->gateway = apnConnInstance->gateway->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->pri_dns_addr != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->pri_dns_addr;
        apnConnInstance->pri_dns_addr = apnConnInstance->pri_dns_addr->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->sec_dns_addr != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->sec_dns_addr;
        apnConnInstance->sec_dns_addr = apnConnInstance->sec_dns_addr->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->sup_rat_types != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->sup_rat_types;
        apnConnInstance->sup_rat_types = apnConnInstance->sup_rat_types->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->rds_app_id != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->rds_app_id;
        apnConnInstance->rds_app_id = apnConnInstance->rds_app_id->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->rds_destination_port != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->rds_destination_port;
        apnConnInstance->rds_destination_port = apnConnInstance->rds_destination_port->next;
        lwm2m_free(resIns);
    }
    while (apnConnInstance->rds_src_port != NULL)
    {
        resource_ins_list_t * resIns = apnConnInstance->rds_src_port;
        apnConnInstance->rds_src_port = apnConnInstance->rds_src_port->next;
        lwm2m_free(resIns);
    }

    lwm2m_free(apnConnInstance);

    return COAP_202_DELETED;
}

static uint8_t prv_apn_conn_create(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP)
{
    apn_conn_profile_instance_t *apnConnInstance;
    uint8_t result;
    char omaID[16] = {0};

    apnConnInstance = (apn_conn_profile_instance_t *)lwm2m_malloc(sizeof(apn_conn_profile_instance_t));
    if (NULL == apnConnInstance) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(apnConnInstance, 0, sizeof(apn_conn_profile_instance_t));

    apnConnInstance->instanceId = instanceId;
    memset(apnConnInstance->profile_name, 0, 257);
    memset(apnConnInstance->apn, 0, 257);
    memset(apnConnInstance->auto_select_apn, 0, 257);
    memset(apnConnInstance->enable_status, 0, 257);
    memset(apnConnInstance->authen_type, 0, 257);
    memset(apnConnInstance->user_name, 0, 257);
    memset(apnConnInstance->secret, 0, 257);
    memset(apnConnInstance->reconnect_schedule, 0, 257);
    memset(apnConnInstance->total_bytes_sent, 0, 257);
    memset(apnConnInstance->total_bytes_received, 0, 257);
    memset(apnConnInstance->qci, 0, 257);
    memset(apnConnInstance->vendor_specific_extensions, 0, 257);
    memset(apnConnInstance->total_packets_sent, 0, 257);
    memset(apnConnInstance->pdn_type, 0, 257);
    memset(apnConnInstance->apn_rate_ctl, 0, 257);
    memset(apnConnInstance->serving_plmn_rate_ctl, 0, 257);
    memset(apnConnInstance->uplink_time_unit, 0, 257);
    memset(apnConnInstance->apn_rate_ctl_4_exception_data, 0, 257);
    memset(apnConnInstance->apn_exception_data_uplink_time_unit, 0, 257);
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, apnConnInstance);

    snprintf(omaID, 16, "%d", objectP->objID);
    prv_add_zcfg_object(omaID, instanceId);





    result = prv_apn_conn_write(contextP, instanceId, numData, dataArray, objectP, LWM2M_WRITE_REPLACE_RESOURCES);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_apn_conn_delete(contextP, instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;


}

void display_apn_conn_object(lwm2m_object_t * object)
{

    fprintf(stdout, "  /%u: APN Connection Profile object:\r\n", object->objID);

}

lwm2m_object_t * get_object_apn_conn()
{
    /*
     * The get_object_device function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * apnConnObj;
    char omaID[16] = {0};
    int newCount = 0;
	objIndex_t objIid;
    struct json_object *value = NULL;
    uint32_t oid;

	#if 0//Jessie QQQ
	rdm_CellAccessPoint_t *CellAccessPointObj = NULL;
	objIndex_t objIid;
	int ret = 0;

	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_CELL_ACCESS_POINT, &objIid, (void **)&CellAccessPointObj);
	if(ret == ZCFG_SUCCESS){
		strncpy(data->pkg_name, CellAccessPointObj->SoftwareVersion, sizeof(data->pkg_name));
		strncpy(data->pkg_version, CellAccessPointObj->SoftwareVersion, sizeof(data->pkg_version));
		zcfgFeObjStructFree(CellAccessPointObj);
	}
	else{
		strcpy(data->pkg_name, "lwm2mclient");
		strcpy(data->pkg_version, "1.0");
	}

	#endif//Jessie QQQ

    apnConnObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != apnConnObj)
    {
        memset(apnConnObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 11 is the standard ID for the mandatory object "Object APN Connection Profile".
         */
        apnConnObj->objID = LWM2M_APN_CONNECTION_OBJECT_ID;
        apnConnObj->versionMajor = 1;
        apnConnObj->versionMinor = 1;

        /*
         * and its unique instance
         *
         */
        snprintf(omaID, 16, "%d", apnConnObj->objID);

        IID_INIT(objIid);
        while(zcfgFeObjJsonGetNext(RDM_OID_CELL_ACCESS_POINT, &objIid, &value) == ZCFG_SUCCESS)
        {    
            newCount++;
            json_object_put(value);
        }
        printf("[%s:%d]update object %d instance to %d\n", __func__, __LINE__, apnConnObj->objID, newCount);
        updateInsInZcfg(omaID, -1, newCount);
        setObjInsListToObj(omaID, apnConnObj);
        
        apn_conn_profile_instance_t * apnInstance = (apn_conn_profile_instance_t *)apnConnObj->instanceList;
        while(apnInstance != NULL)
        {
            memset(apnInstance->profile_name, 0, 257);
            memset(apnInstance->apn, 0, 257);
            memset(apnInstance->auto_select_apn, 0, 257);
            memset(apnInstance->enable_status, 0, 257);
            memset(apnInstance->authen_type, 0, 257);
            memset(apnInstance->user_name, 0, 257);
            memset(apnInstance->secret, 0, 257);
            memset(apnInstance->reconnect_schedule, 0, 257);
            memset(apnInstance->total_bytes_sent, 0, 257);
            memset(apnInstance->total_bytes_received, 0, 257);
            memset(apnInstance->qci, 0, 257);
            memset(apnInstance->vendor_specific_extensions, 0, 257);
            memset(apnInstance->total_packets_sent, 0, 257);
            memset(apnInstance->pdn_type, 0, 257);
            memset(apnInstance->apn_rate_ctl, 0, 257);
            memset(apnInstance->serving_plmn_rate_ctl, 0, 257);
            memset(apnInstance->uplink_time_unit, 0, 257);
            memset(apnInstance->apn_rate_ctl_4_exception_data, 0, 257);
            memset(apnInstance->apn_exception_data_uplink_time_unit, 0, 257);
            apnInstance = apnInstance->next;
        }
        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        apnConnObj->readFunc     = prv_apn_conn_read;
        apnConnObj->discoverFunc = prv_apn_conn_discover;
        apnConnObj->writeFunc    = prv_apn_conn_write;
        apnConnObj->executeFunc  = prv_apn_conn_execute;
        apnConnObj->createFunc   = prv_apn_conn_create;
        apnConnObj->deleteFunc   = prv_apn_conn_delete;
        apnConnObj->userData = NULL;

    }

    return apnConnObj;
}

void free_object_apn_conn(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    while (objectP->instanceList != NULL)
    {
        apn_conn_profile_instance_t * apnConnInstance = (apn_conn_profile_instance_t *)objectP->instanceList;
        while (apnConnInstance->validity_mcc_mnc != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->validity_mcc_mnc;
            apnConnInstance->validity_mcc_mnc = apnConnInstance->validity_mcc_mnc->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->connection_establish_time != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->connection_establish_time;
            apnConnInstance->connection_establish_time = apnConnInstance->connection_establish_time->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->connection_establish_result != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->connection_establish_result;
            apnConnInstance->connection_establish_result = apnConnInstance->connection_establish_result->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->connection_establish_reject_cause != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->connection_establish_reject_cause;
            apnConnInstance->connection_establish_reject_cause = apnConnInstance->connection_establish_reject_cause->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->connection_end_time != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->connection_end_time;
            apnConnInstance->connection_end_time = apnConnInstance->connection_end_time->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->ip_addr != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->ip_addr;
            apnConnInstance->ip_addr = apnConnInstance->ip_addr->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->prefix_len != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->prefix_len;
            apnConnInstance->prefix_len = apnConnInstance->prefix_len->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->subnet_mask != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->subnet_mask;
            apnConnInstance->subnet_mask = apnConnInstance->subnet_mask->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->gateway != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->gateway;
            apnConnInstance->gateway = apnConnInstance->gateway->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->pri_dns_addr != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->pri_dns_addr;
            apnConnInstance->pri_dns_addr = apnConnInstance->pri_dns_addr->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->sec_dns_addr != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->sec_dns_addr;
            apnConnInstance->sec_dns_addr = apnConnInstance->sec_dns_addr->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->sup_rat_types != NULL)
    {
            resource_ins_list_t * resIns = apnConnInstance->sup_rat_types;
            apnConnInstance->sup_rat_types = apnConnInstance->sup_rat_types->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->rds_app_id != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->rds_app_id;
            apnConnInstance->rds_app_id = apnConnInstance->rds_app_id->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->rds_destination_port != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->rds_destination_port;
            apnConnInstance->rds_destination_port = apnConnInstance->rds_destination_port->next;
            lwm2m_free(resIns);
        }
        while (apnConnInstance->rds_src_port != NULL)
        {
            resource_ins_list_t * resIns = apnConnInstance->rds_src_port;
            apnConnInstance->rds_src_port = apnConnInstance->rds_src_port->next;
            lwm2m_free(resIns);
        }
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(apnConnInstance);
    }

    lwm2m_free(objectP);
}

