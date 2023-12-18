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

#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128


// Resource Id's:
#define RES_O_SMSC_ADDR                             0
#define RES_O_DISABLE_RADIO_PERIOD                  1
#define RES_O_MODULE_ACTIVATION_CODE                2
#define RES_O_VENDOR_SPECIFIC_EXTENSIONS            3
#define RES_O_PSM_TIMER                             4
#define RES_O_ACTIVE_TIMER                          5
#define RES_O_EDRX_PARAM_4_IU_MODE                  7
#define RES_O_EDRX_PARAM_4_WBS1_MODE                8
#define RES_O_EDRX_PARAM_4_NBS1_MODE                9
#define RES_O_EDRX_PARAM_4_AGB_MODE                 10
#define RES_M_ACTIVATED_PROFILE_NAMES               11
#define RES_O_SUP_POWER_SAVING_MODES                12
#define RES_O_ACTIVE_POWER_SAVING_MODES             13
#define RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE   14

bool checkCellConnValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, cell_conn_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_O_SMSC_ADDR:
        if(strcmp(targetP->smsc_addr, zcfgParamValue))
        {
            strncpy(targetP->smsc_addr, zcfgParamValue, sizeof(targetP->smsc_addr));
            return true;
        }
        break;

    case RES_O_DISABLE_RADIO_PERIOD:
        if(strcmp(targetP->disable_radio_period, zcfgParamValue))
        {
            strncpy(targetP->disable_radio_period, zcfgParamValue, sizeof(targetP->disable_radio_period));
            return true;
        }
        break;

    case RES_O_MODULE_ACTIVATION_CODE:
        if(strcmp(targetP->module_activation_code, zcfgParamValue))
        {
            strncpy(targetP->module_activation_code, zcfgParamValue, sizeof(targetP->module_activation_code));
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

    case RES_O_PSM_TIMER:
        if(strcmp(targetP->psm_timer, zcfgParamValue))
        {
            strncpy(targetP->psm_timer, zcfgParamValue, sizeof(targetP->psm_timer));
            return true;
        }
        break;

    case RES_O_ACTIVE_TIMER:
        if(strcmp(targetP->active_timer, zcfgParamValue))
        {
            strncpy(targetP->active_timer, zcfgParamValue, sizeof(targetP->active_timer));
            return true;
        }
        break;

    case RES_O_EDRX_PARAM_4_IU_MODE:
        if(strcmp(targetP->edrx_param_4_iu_mode, zcfgParamValue))
        {
            strncpy(targetP->edrx_param_4_iu_mode, zcfgParamValue, sizeof(targetP->edrx_param_4_iu_mode));
            return true;
        }
        break;

    case RES_O_EDRX_PARAM_4_WBS1_MODE:
        if(strcmp(targetP->edrx_param_4_wbs1_mode, zcfgParamValue))
        {
            strncpy(targetP->edrx_param_4_wbs1_mode, zcfgParamValue, sizeof(targetP->edrx_param_4_wbs1_mode));
            return true;
        }
        break;

    case RES_O_EDRX_PARAM_4_NBS1_MODE:
        if(strcmp(targetP->edrx_param_4_nbs1_mode, zcfgParamValue))
        {
            strncpy(targetP->edrx_param_4_nbs1_mode, zcfgParamValue, sizeof(targetP->edrx_param_4_nbs1_mode));
            return true;
        }
        break;

    case RES_O_EDRX_PARAM_4_AGB_MODE:
        if(strcmp(targetP->edrx_param_4_agb_mode, zcfgParamValue))
        {
            strncpy(targetP->edrx_param_4_agb_mode, zcfgParamValue, sizeof(targetP->edrx_param_4_agb_mode));
            return true;
        }
        break;

    case RES_M_ACTIVATED_PROFILE_NAMES:
    {
        if(LWM2M_LIST_FIND(targetP->activated_profile_names, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->activated_profile_names = LWM2M_LIST_ADD(targetP->activated_profile_names, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->activated_profile_names, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }   

    case RES_O_SUP_POWER_SAVING_MODES:
        if(strcmp(targetP->sup_power_saving_modes, zcfgParamValue))
        {
            strncpy(targetP->sup_power_saving_modes, zcfgParamValue, sizeof(targetP->sup_power_saving_modes));
            return true;
        }
        break;

    case RES_O_ACTIVE_POWER_SAVING_MODES:
        if(strcmp(targetP->active_power_saving_modes, zcfgParamValue))
        {
            strncpy(targetP->active_power_saving_modes, zcfgParamValue, sizeof(targetP->active_power_saving_modes));
            return true;
        }
        break;

    case RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE:
        if(strcmp(targetP->release_assistance_indication_usage, zcfgParamValue))
        {
            strncpy(targetP->release_assistance_indication_usage, zcfgParamValue, sizeof(targetP->release_assistance_indication_usage));
            return true;
        }
        break;








 

    default:
        break;
    }
    return false;
}

int mappingZcfgObject(char *omaId, uint32_t *oid, char *param)
{
    int ret = ZCFG_SUCCESS;
    FILE *zcfgWakkamaFp = NULL;
    char fileStr[512] = {0};
    char * Id;

    if((zcfgWakkamaFp = fopen(WAKKAMA_MAPPING_TABLE, "r")) == NULL) {
        printf("fail to open %s file !!\n", WAKKAMA_MAPPING_TABLE);
        return ZCFG_INTERNAL_ERROR;
    }

    while (fscanf(zcfgWakkamaFp, "%[^\n] ", fileStr) != EOF) {
        Id = strtok(fileStr, ":");   // Get oma ID string
        strcpy(param, strtok(NULL, ":")); // Get zcfg param string
        *oid = atoi(strtok(NULL, ":"));  // Get oid number
        if(!strcmp(Id, omaId))
        {
             // printf("[%d:%d] %s:%s:%d\n", __FUNCTION__, __LINE__,Id, param, *oid);
            fclose(zcfgWakkamaFp);
            return ret;
        }
    }
    
    fclose(zcfgWakkamaFp);
    return ZCFG_NO_SUCH_OBJECT;
}

int64_t dateTimeToUnixTime(char *datetime)
{
    struct tm tm_time;
    int64_t unixtime;
	
	memset(&tm_time, 0, sizeof(struct tm));
	if(datetime){
		//printf("%s() datetime=%s\n", __FUNCTION__, datetime);
		strptime(datetime, "%Y-%m-%dT%H:%M:%SZ\n", &tm_time);
	}
	unixtime = timegm(&tm_time);
	
    if(unixtime == -1)
    {
        printf("[%s:%d]tm to time_t is fail, set default time(1970-01-01T00:00:00z)", __func__, __LINE__);
        unixtime = 0;
    }
    return unixtime;
}

json_type getZcfgJSONParameterType(lwm2m_data_type_t type)
{
    switch(type)
    {   
    case LWM2M_TYPE_INTEGER:
        return json_type_int;
        break;
    case LWM2M_TYPE_TIME:
        return json_type_time;
        break;
    case LWM2M_TYPE_UNSIGNED_INTEGER:
        return json_type_uint32;
        break;

    case LWM2M_TYPE_FLOAT:
        return json_type_double;
        break;   

    case LWM2M_TYPE_BOOLEAN:
        return json_type_boolean;
        break;

    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
    default:
        return json_type_string;
        break;

    }

    return json_type_string;
}

bool setChangeValueList(int objectId, int instanceId,  int resourceId,  int resourceInstanceId)
{
    change_value_list_t * changeValue;
    bool found = false;
    int idx = 0;
    while(LWM2M_LIST_FIND(changeValueList, idx) != NULL)
    {
        idx++;
    }
    changeValue = (change_value_list_t *)lwm2m_malloc(sizeof(change_value_list_t));
    if (NULL != changeValue)
    {
        memset(changeValue, 0, sizeof(change_value_list_t));
        changeValue->instanceId = idx;
        changeValue->uri.objectId = objectId;
        changeValue->uri.instanceId = (instanceId < 0) ? 0 : instanceId;
        changeValue->uri.resourceId = resourceId;
    #ifndef LWM2M_VERSION_1_0
        changeValue->uri.resourceInstanceId = (resourceInstanceId < 0) ? 0 : resourceInstanceId;
    #endif
        
    }

    change_value_list_t * checkList = (change_value_list_t *)changeValueList;
    while(checkList != NULL)
    {
        if(!memcmp(&checkList->uri, &changeValue->uri, sizeof(lwm2m_uri_t)))
        {
            found = true;
            break;
        }
        checkList = checkList->next;
    }
    
    if (NULL != changeValue)
    {
        if(found == false)
        {
            changeValueList = LWM2M_LIST_ADD(changeValueList, changeValue);
        }
        else
        {
            //printf("[%s:%d]/%d/%d/%d/%d  resource is exist in change value list", __func__, __LINE__, objectId, instanceId,  resourceId,  resourceInstanceId);
            lwm2m_free(changeValue);
            return false;
        }
    }
    return true;
}


bool setObjInsListToObj(char *omaId, lwm2m_object_t *object)
{

    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    while(zcfgFeObjJsonGetNext(oid, &objIid, &value) == ZCFG_SUCCESS)
    {
        if(zcfgFeParamValGet(value, "emptyIns") == NULL)
        {
            printf("[%s:%d]objIid.idx %d is exist\n", __func__, __LINE__,objIid.idx[objIid.level - 1]);
            switch(object->objID)
            {
                case LWM2M_APN_CONNECTION_OBJECT_ID:
                {
                    apn_conn_profile_instance_t * instance = (apn_conn_profile_instance_t *)lwm2m_malloc(sizeof(apn_conn_profile_instance_t));
                    if (NULL == instance)
                    {
                        return false;
                    }
                    memset(instance, 0, sizeof(apn_conn_profile_instance_t));
                    instance->instanceId = objIid.idx[objIid.level - 1] - 1;
                    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);
                    break;
                }

                case LWM2M_SERVER_OBJECT_ID:
                {
                    server_instance_t * instance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
                    if (NULL == instance)
                    {
                        return false;
                    }
                    memset(instance, 0, sizeof(server_instance_t));
                    instance->instanceId = objIid.idx[objIid.level - 1] - 1;
                    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);
                    break;
                }

                // case LWM2M_SECURITY_OBJECT_ID:
                // case LWM2M_SERVER_OBJECT_ID:
                default:
                {
                    lwm2m_list_t * instance = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
                    if (NULL == instance)
                    {
                        return false;
                    }
                    memset(instance, 0, sizeof(lwm2m_list_t));
                    instance->id = objIid.idx[objIid.level - 1] - 1;
                    object->instanceList = LWM2M_LIST_ADD(object->instanceList, instance);
                    break;
                }
            }
        }
        json_object_put(value);
    }
    if(objIid.level < 1)
    {
        printf("%s() this object no instance\n", __FUNCTION__);
        return false;
    }
    
    printf("[%s:%d]ObjInstance total = %d\n", __func__, __LINE__, objIid.idx[objIid.level - 1]);
    return true;
}


bool setResourceInsToData(char *omaId, int objInsId, lwm2m_data_t **data, size_t *dataCount)
{

    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    lwm2m_list_t * instance;
    int totalIns = 0;
    int idx[256] = {0};
    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    // if(objInsId >= 0)
    // {
    //     objIid.idx[objIid.level++] = objInsId + 1;
    // }
    while(zcfgFeObjJsonGetNext(oid, &objIid, &value) == ZCFG_SUCCESS)
    {
        if(objIid.level > 1)
        {
            if(objInsId != (objIid.idx[0] - 1))
            {
                continue;
            }
        }
        // for(int i = 0; i < objIid.level; i++)      
        //     printf("[%s:%d]objIid.idx[%d] = %d\n", __func__, __LINE__, i, objIid.idx[i]);
        // printf("\n");
          

        if(!zcfgFeParamValGet(value, "emptyIns"))
        {
            idx[totalIns++] = objIid.idx[objIid.level - 1] - 1;
        }
        json_object_put(value);
    }

    *data = lwm2m_data_new(totalIns);
    for (int i = 0; i < totalIns; i++)
    {
        (*data)[i].id = idx[i];
    }
    *dataCount = totalIns;
    printf("[%s:%d]ResourceInstance total = %d\n", __func__, __LINE__, totalIns);
    return true;
}

bool updateInsInZcfg(char *omaId, int objInsId, int newCount)
{
    objIndex_t objIid;
    struct json_object *value = NULL;
    char param[64] = {0};
    uint32_t oid;
    int maxInstance = 0;
    int curCount = 0;
    int ret = ZCFG_SUCCESS;

    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    while(zcfgFeObjJsonGetNext(oid, &objIid, &value) == ZCFG_SUCCESS)
    {
        if(objIid.level > 1)
        {
            if(objInsId != (objIid.idx[0] - 1))
            {
                continue;
            }
        }      
        if(!zcfgFeParamValGet(value, "emptyIns"))
        {
            curCount++;
        }
        maxInstance++;
        json_object_put(value);
    }
    printf("[%s:%d]maxInstance = %d, curCount = %d, newCount = %d, objInsId = %d, oid = %d\n", __func__, __LINE__, maxInstance, curCount, newCount, objInsId, oid);

    for(int i = 0; i < maxInstance - curCount; i++)
    {
        IID_INIT(objIid);
        if(objInsId >= 0)
        {
            objIid.idx[objIid.level++] = objInsId + 1;
        }
        if((ret = zcfgFeObjJsonAdd(oid, &objIid, NULL)) != ZCFG_SUCCESS)
        {
            printf("[%s]zcfgFeObjJsonAdd fail, ret = %d!!!\n", __FUNCTION__, ret);
        }
    }
    if(newCount > maxInstance)
    {
        for(int i = 0; i < newCount - maxInstance; i++)
        {
            IID_INIT(objIid);
            if(objInsId >= 0)
            {
                objIid.idx[objIid.level++] = objInsId + 1;
            }
            if((ret = zcfgFeObjJsonAdd(oid, &objIid, NULL)) != ZCFG_SUCCESS)
            {
                printf("[%s]zcfgFeObjJsonAdd fail, ret = %d!!!\n", __FUNCTION__, ret);
            }
        } 
    }
    else if(maxInstance > newCount)
    {
        for(int i = 0; i < maxInstance - newCount; i++)
        {
            IID_INIT(objIid);
            if(objInsId >= 0)
            {
                objIid.idx[objIid.level++] = objInsId + 1;
            }
            objIid.idx[objIid.level++] = maxInstance - i;
            ret = zcfgFeObjJsonDel(oid, &objIid, NULL); //FIXME: Run zcfgFeObjJsonDel had return ZCFG_INTERNAL_ERROR,But delete is finish....
            printf("[%s]retzcfg = %d\n", __FUNCTION__, ret); 
        }

    }
    return true;
}


bool prv__zcfg_decode_objlink(char * src, uint16_t *objectId, uint16_t *objectInstanceId)
{
    char *token;
    if(strlen(src) == 0)
    {
        *objectId = 0;   
        *objectInstanceId = 0;  
        return false;
    }

    token = strtok(src,":");
    if(token == NULL)
    {
        return false;
    }
    *objectId = atoi(token);

    token = strtok(NULL,":");
    if(token == NULL)
    { 
        return false;
    }
    *objectInstanceId = atoi(token); 

    return true;
}


/**
 * @brief get resource value in zcfg database.
 * 
 * @param omaId oma ID string(e.g. 0_0 is LWM2M Security object LWM2M  Server URI resource)
 * @param instanceId object instance id, if object is single object instanceId set -1
 * @param resInstance resource instance num, if resource is single resource resInstance set 1.
 * @param desValue output resource value
 * @return int zcfgRet_t
 */
int prv_get_zcfg_value(char *omaId, int instanceId, int resInstance, char *desValue)
{
    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    if(instanceId >= 0)
    {
        objIid.idx[objIid.level++] = instanceId + 1;
    }
    if(strcmp(param,"value") == 0)
    {
        objIid.idx[objIid.level++] = resInstance + 1; 
    }

    /*Get parameter value and type from json object*/
    ret = zcfgFeObjJsonGet(oid, &objIid, &value);
    if(ret == ZCFG_SUCCESS)
    {
        strcpy(desValue, (char *)zcfgFeParamValGet(value, param));
    }
    else
    {
        printf("*********%s() Jessi ret=%d\n", __FUNCTION__, ret);     
    }
    json_object_put(value);
    value = NULL;
    return ret;
}

/**
 * @brief set resource value in zcfg database.
 * 
 * @param omaId oma ID string(e.g. 0_0 is LWM2M Security object LWM2M  Server URI resource)
 * @param instanceId object instance id, if object is single object instanceId set -1
 * @param resInstance resource instance num, if resource is single resource resInstance set 1.
 * @param desValue input resource value
 * @return int zcfgRet_t
 */
int prv_set_zcfg_value(char *omaId, int instanceId, int resInstance, lwm2m_data_type_t type, char *desValue)
{
    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    if(instanceId >= 0)
    {
        objIid.idx[objIid.level++] = instanceId + 1;
    }
    if(strcmp(param,"value") == 0)
    {
        objIid.idx[objIid.level++] = resInstance + 1; 
    }

    ret = zcfgFeObjJsonGet(oid, &objIid, &value);
	if(ret != ZCFG_SUCCESS){
		printf("zcfgFeObjJsonGet fail, ret = %d!!!\n", ret);
        return ret;
	}

	/*You can use a loop here to add all parameters into a json object*/
	ret = zcfgFeJsonObjParamSet(oid, param, getZcfgJSONParameterType(type), value, desValue);
	/*end*/

	if(ret != ZCFG_SUCCESS){
		printf("zcfgFeJsonObjParamSet fail, ret = %d!!!\n", ret);
        return ret;
	}
	// cwmp_cm_set_diagnostic_creator(oid, value);
	ret = zcfgFeObjJsonSet(oid, &objIid, value, NULL);
	if( ret != ZCFG_SUCCESS ){
		printf("[%s]zcfgFeObjJsonSet fail, ret = %d!!!\n", __FUNCTION__, ret);
        return ret;
	}

	json_object_put(value);
	value = NULL;

	return ret;
}

/**
 * @brief create object in zcfg database.
 * 
 * @param omaId  oma ID string(e.g. 0 is LWM2M Security object)
 * @param instanceId object instance id, if object is single object instanceId set -1
 * @return int int zcfgRet_t
 */
int prv_add_zcfg_object(char *omaId, int instanceId)
{
    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);

    for(int i = 0; i < instanceId + 1; i++)
    {
        IID_INIT(objIid);

        if(instanceId >= 0)
        {
            objIid.idx[objIid.level++] = i + 1;
        }

        if(zcfgFeObjJsonQueryExist(oid, &objIid) != 0) {
            IID_INIT(objIid);

            ret = zcfgFeObjJsonAdd(oid, &objIid, NULL);

            if( ret == ZCFG_SUCCESS) {

                //Set object default vale
                value = json_object_new_object();
                zcfgFeGetSchemaParameterByOid(oid, value);
                ret = zcfgFeObjJsonSet(oid, &objIid, value, NULL);
                if( ret != ZCFG_SUCCESS ){
                    printf("[%s]zcfgFeObjJsonSet fail, ret = %d!!!\n", __FUNCTION__, ret);
                    return ret;
                }
                json_object_put(value);
            }
            else {
                printf("[%s]zcfgFeObjJsonAdd fail, ret = %d!!!\n", __FUNCTION__, ret);
                return ret;
            }
        }
    }
	return ret;
}

/**
 * @brief create resource instance to zcfg database.
 * 
 * @param omaId oma ID string(e.g. 0_0 is LWM2M Security object LWM2M  Server URI resource)
 * @param instanceId object instance id, if object is single object instanceId set -1
 * @param resInstance resource instance num.
 * @return int zcfgRet_t
 */
int prv_add_zcfg_resource_ins(char *omaId, int instanceId, int resInstance)
{
    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);

    for(int i = 0; i < resInstance + 1; i++)
    {
        IID_INIT(objIid);
        if(instanceId >= 0)
        {
            objIid.idx[objIid.level++] = instanceId + 1;
        }

        if(strcmp(param,"value") == 0)
        {
            objIid.idx[objIid.level++] = i + 1; 
        }

        if(zcfgFeObjJsonQueryExist(oid, &objIid) != 0) {
            printf("*********************[%s:%d] resource instance %d isn't exist!create now!!\n", __func__, __LINE__, i);
            IID_INIT(objIid);
            if(instanceId >= 0)
            {
                objIid.idx[objIid.level++] = instanceId + 1;
            }

            ret = zcfgFeObjJsonAdd(oid, &objIid, NULL);

            if( ret == ZCFG_SUCCESS) {

                //Set object default vale
                value = json_object_new_object();
                zcfgFeGetSchemaParameterByOid(oid, value);
                ret = zcfgFeObjJsonSet(oid, &objIid, value, NULL);
                if( ret != ZCFG_SUCCESS ){
                    printf("[%s]zcfgFeObjJsonSet fail, ret = %d!!!\n", __FUNCTION__, ret);
                    return ret;
                }
                json_object_put(value);
            }
            else {
                printf("[%s]zcfgFeObjJsonAdd fail, ret = %d!!!\n", __FUNCTION__, ret);
                return ret;
            }
        } 
    }
	return ret;
}

/**
 * @brief Delete object in zcfg database.
 * 
 * @param omaId  oma ID string(e.g. 0 is LWM2M Security object)
 * @param instanceId object instance id, if object is single object instanceId set -1
 * @return int int zcfgRet_t
 */
int prv_del_zcfg_value(char *omaId, int instanceId)
{

    int ret = ZCFG_SUCCESS;
    char param[64] = {0};
    uint32_t oid;
	objIndex_t objIid;
	struct json_object *value = NULL;
    mappingZcfgObject(omaId, &oid, param);
    IID_INIT(objIid);
    if(instanceId >= 0)
    {
        objIid.idx[objIid.level++] = instanceId + 1;
    }

	ret = zcfgFeObjJsonDel(oid, &objIid, NULL); //FIXME: Run zcfgFeObjJsonDel had return ZCFG_INTERNAL_ERROR,But delete is finish....
	printf("[%s]retzcfg = %d\n", __FUNCTION__, ret); 

	return ZCFG_SUCCESS;
}


static uint8_t prv_cell_conn_read(lwm2m_context_t *contextP,
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
    cell_conn_instance_t * targetP;
    
    /* unused parameter */
    (void)contextP;



    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    targetP = (cell_conn_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;    

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_O_SMSC_ADDR,
            RES_O_DISABLE_RADIO_PERIOD,
            RES_O_MODULE_ACTIVATION_CODE,
            RES_O_VENDOR_SPECIFIC_EXTENSIONS,
            RES_O_PSM_TIMER,
            RES_O_ACTIVE_TIMER,
            RES_O_EDRX_PARAM_4_IU_MODE,
            RES_O_EDRX_PARAM_4_WBS1_MODE,
            RES_O_EDRX_PARAM_4_NBS1_MODE,
            RES_O_EDRX_PARAM_4_AGB_MODE,
            RES_M_ACTIVATED_PROFILE_NAMES,
            RES_O_SUP_POWER_SAVING_MODES,
            RES_O_ACTIVE_POWER_SAVING_MODES,
            RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE 
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
        case RES_O_SMSC_ADDR:
        case RES_O_MODULE_ACTIVATION_CODE:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkCellConnValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_string(desValue, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        // Object Link
        case RES_O_VENDOR_SPECIFIC_EXTENSIONS:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            uint16_t objectId = 0;
            uint16_t objectInstanceId = 0;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkCellConnValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            prv__zcfg_decode_objlink(desValue, &objectId, &objectInstanceId);
            lwm2m_data_encode_objlink(objectId, objectId, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        // Int
        case RES_O_DISABLE_RADIO_PERIOD:
        case RES_O_PSM_TIMER:
        case RES_O_ACTIVE_TIMER:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkCellConnValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        // Opaque
        case RES_O_EDRX_PARAM_4_IU_MODE:
        case RES_O_EDRX_PARAM_4_WBS1_MODE:
        case RES_O_EDRX_PARAM_4_NBS1_MODE:
        case RES_O_EDRX_PARAM_4_AGB_MODE:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkCellConnValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }
            
            lwm2m_data_encode_opaque((uint8_t*)desValue, strlen(desValue), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        // Uint
        case RES_O_SUP_POWER_SAVING_MODES:
        case RES_O_ACTIVE_POWER_SAVING_MODES:
        case RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE:
            if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkCellConnValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
            }
            
            lwm2m_data_encode_uint(strtoul(desValue, NULL, 0), *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        // Multi object link
        case RES_M_ACTIVATED_PROFILE_NAMES: 
        {
            uint16_t objectId = 0;
            uint16_t objectInstanceId = 0;

            setResourceInsToData(omaID, -1, &subTlvP, &count);
            lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

            for (j = 0; j < count; j++)
            {
                if((ret = prv_get_zcfg_value(omaID, -1, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkCellConnValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  j);
                }      

                prv__zcfg_decode_objlink(desValue, &objectId, &objectInstanceId);
                lwm2m_data_encode_objlink(objectId, objectInstanceId, &subTlvP[j]);
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
static uint8_t prv_cell_conn_discover(lwm2m_context_t *contextP,
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
            RES_O_SMSC_ADDR,
            RES_O_DISABLE_RADIO_PERIOD,
            RES_O_MODULE_ACTIVATION_CODE,
            RES_O_VENDOR_SPECIFIC_EXTENSIONS,
            RES_O_PSM_TIMER,
            RES_O_ACTIVE_TIMER,
            RES_O_EDRX_PARAM_4_IU_MODE,
            RES_O_EDRX_PARAM_4_WBS1_MODE,
            RES_O_EDRX_PARAM_4_NBS1_MODE,
            RES_O_EDRX_PARAM_4_AGB_MODE,
            RES_M_ACTIVATED_PROFILE_NAMES,
            RES_O_SUP_POWER_SAVING_MODES,
            RES_O_ACTIVE_POWER_SAVING_MODES,
            RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE
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
            case RES_O_SMSC_ADDR:
            case RES_O_DISABLE_RADIO_PERIOD:
            case RES_O_MODULE_ACTIVATION_CODE:
            case RES_O_VENDOR_SPECIFIC_EXTENSIONS:
            case RES_O_PSM_TIMER:
            case RES_O_ACTIVE_TIMER:
            case RES_O_EDRX_PARAM_4_IU_MODE:
            case RES_O_EDRX_PARAM_4_WBS1_MODE:
            case RES_O_EDRX_PARAM_4_NBS1_MODE:
            case RES_O_EDRX_PARAM_4_AGB_MODE:
            case RES_M_ACTIVATED_PROFILE_NAMES:
            case RES_O_SUP_POWER_SAVING_MODES:
            case RES_O_ACTIVE_POWER_SAVING_MODES:
            case RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_cell_conn_write(lwm2m_context_t *contextP,
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
        snprintf(omaID, 16, "%d_%d", objectP->objID, (dataArray)[i].id);
        switch (dataArray[i].id)
        {
        // String
        case RES_O_SMSC_ADDR:
        case RES_O_MODULE_ACTIVATION_CODE:

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

        // Int
        case RES_O_DISABLE_RADIO_PERIOD:
        case RES_O_PSM_TIMER:
        case RES_O_ACTIVE_TIMER:
            lwm2m_data_decode_int(&dataArray[i], &iValue);
            sprintf(param, "%d", iValue);
            if(prv_set_zcfg_value(omaID, -1, -1, LWM2M_TYPE_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE,but this case type should be LWM2M_TYPE_INTEGER, I dont know why
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        // Opaque
        case RES_O_EDRX_PARAM_4_IU_MODE:
        case RES_O_EDRX_PARAM_4_WBS1_MODE:
        case RES_O_EDRX_PARAM_4_NBS1_MODE:
        case RES_O_EDRX_PARAM_4_AGB_MODE:
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

        // Uint
        case RES_O_ACTIVE_POWER_SAVING_MODES:
        case RES_O_RELEASE_ASSISTANCE_INDICATION_USAGE:
            lwm2m_data_decode_uint(&dataArray[i], &uValue);
            sprintf(param, "%u", uValue);
            if(prv_set_zcfg_value(omaID, -1, -1, LWM2M_TYPE_UNSIGNED_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE but this case type should be LWM2M_TYPE_UNSIGNED_INTEGER, I dont know why
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
// TODO
static uint8_t prv_cell_conn_execute(lwm2m_context_t *contextP,
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
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

void display_cell_conn_object(lwm2m_object_t * object)
{

    fprintf(stdout, "  /%u: Cellular Connectivity object:\r\n", object->objID);

}

lwm2m_object_t * get_object_cell_conn()
{
    /*
     * The get_object_device function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * cellConnObj;

    cellConnObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != cellConnObj)
    {

        cell_conn_instance_t * cellConnInstance;

        memset(cellConnObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         * The 10 is the standard ID for the mandatory object "Object Cellular Connectivity".
         */
        cellConnObj->objID = LWM2M_CELL_CONN_OBJECT_ID;
        cellConnObj->versionMajor = 1;
        cellConnObj->versionMinor = 1;

        /*
         * and its unique instance
         *
         */
        cellConnInstance = (cell_conn_instance_t *)lwm2m_malloc(sizeof(cell_conn_instance_t));
        if (NULL == cellConnInstance)
        {
            lwm2m_free(cellConnObj);
            return NULL;
        }

        memset(cellConnInstance, 0, sizeof(cell_conn_instance_t));
        cellConnInstance->instanceId = 0;
        memset(cellConnInstance->smsc_addr, 0, 257);
        memset(cellConnInstance->disable_radio_period, 0, 257);
        memset(cellConnInstance->module_activation_code, 0, 257);
        memset(cellConnInstance->vendor_specific_extensions, 0, 257);
        memset(cellConnInstance->psm_timer, 0, 257);
        memset(cellConnInstance->active_timer, 0, 257);
        memset(cellConnInstance->edrx_param_4_iu_mode, 0, 257);
        memset(cellConnInstance->edrx_param_4_wbs1_mode, 0, 257);
        memset(cellConnInstance->edrx_param_4_nbs1_mode, 0, 257);
        memset(cellConnInstance->edrx_param_4_agb_mode, 0, 257);
        memset(cellConnInstance->sup_power_saving_modes, 0, 257);
        memset(cellConnInstance->active_power_saving_modes, 0, 257);
        memset(cellConnInstance->release_assistance_indication_usage, 0, 257);
        cellConnObj->instanceList = LWM2M_LIST_ADD(cellConnObj->instanceList, cellConnInstance);
        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        cellConnObj->readFunc     = prv_cell_conn_read;
        cellConnObj->discoverFunc = prv_cell_conn_discover;
        cellConnObj->writeFunc    = prv_cell_conn_write;
        cellConnObj->executeFunc  = prv_cell_conn_execute;
        cellConnObj->userData = NULL;

    }

    return cellConnObj;
}

void free_object_cell_conn(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    while (objectP->instanceList != NULL)
    {
        cell_conn_instance_t * cellConnInstance = (cell_conn_instance_t *)objectP->instanceList;
        while (cellConnInstance->activated_profile_names != NULL)
        {
            resource_ins_list_t * resourceIns = cellConnInstance->activated_profile_names;
            cellConnInstance->activated_profile_names = cellConnInstance->activated_profile_names->next;
            lwm2m_free(resourceIns);
        }
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(cellConnInstance);
    }

    lwm2m_free(objectP);
}

