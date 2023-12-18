/*
 *
 * Copyright (C) 2019-2021, Broadband Forum
 * Copyright (C) 2016-2021  CommScope, Inc
 * Copyright (C) 2020,  BT PLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file device_controller.c
 *
 * Implements the Device.Controller data model object
 *
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "common_defs.h"
#include "data_model.h"
#include "device.h"
#include "usp_api.h"
#include "dm_access.h"
#include "dm_trans.h"
#include "mtp_exec.h"
#include "msg_handler.h"
#include "text_utils.h"
#include "iso8601.h"
#include "retry_wait.h"

#ifdef ENABLE_COAP
#include "usp_coap.h"
#endif

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
//#include "zcmd_internal.h"
//#include "zcmd.h"
#include "zcmd_tool.h"
//#include "zcmd_obj_util.h"
//#include "zcmd_config_apply.h"
#include "zlog_api.h"
//#include "common_defs.h"
//#include "data_model.h"
//#include "json.h"
#include "vendor_zcfg.h"
//------------------------------------------------------------------------------
// Location of the controller table within the data model
#define DEVICE_CELL_ROOT "Device.Cellular"
#define DEVICE_INTERFACE_ROOT "Device.Cellular.Interface"
#define DEVICE_ACCESSPOINT_ROOT "Device.Cellular.AccessPoint"
static const char device_cell_root[] = DEVICE_CELL_ROOT;

//------------------------------------------------------------------------------
// Forward declarations. Note these are not static, because we need them in the symbol table for USP_LOG_Callstack() to show them
int cell_set_value(dm_req_t *req, char *buf);
int cell_set_notify_value(dm_req_t *req, char *buf);
int cell_get_value(dm_req_t *req, char *buf, int len);
int cell_add(dm_req_t *req);
int cell_add_notify(dm_req_t *req);
int cell_del(dm_req_t *req);


/*************************
* Function 	: zcfg_get_value()
* Description	: get a single parameter
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
/*get single parameter*/
int zcfg_get_value(char *objname, char *strVal, int *len){
	#if 1
	int ret = 0;
	char *paramStrVal;
	objIndex_t objIid;
	objIndex_t parentObjIid;
	int oid = 0;
	struct json_object *value = NULL;
	char objnamebuf[512] = {0};
	char objpath[512]={0};
	char name[512]={0};
	int  isPartial = 0;
	bool isIndex = 0;
	bool isIndexExit = 0;
	//printf("[%s:%d] Enter objname=%s\n",__func__, __LINE__, objname);		
	if( cwmp_cm_parse_fullpath( objname, objpath, name, &isPartial) != 0){
		printf( "invalid fullname :%s", objname );
		//continue;
	}
	//printf( "objpath :%s, name: %s,isPartial: %d\n", objpath, name, isPartial );	
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	//printf( "oid=%d\n", oid );	
	if(oid < 0 ) {
		if(isPartial){
			/* check if index node, ignor json-obj A.B of A.B.i don't have oid */
			memset(objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objpath );
			oid = zcfgFeObjNameToObjId( objnamebuf, &objIid ); 
			if( oid >= 0){
				isIndex = 1;
			}
			else{
				return -1;
			}
		}/*isPartial*/
		else{
			printf("%s() Jessi oid < 0 but is not partial\n", __FUNCTION__);
			//return -2;
		}
	}
	//printf( "222oid=%d,isIndex=%d\n", oid, isIndex );
	#endif
#if 1//test index object, mark this temporarily	
	if(oid >= 0 ) {
		/*Get parameter value and type from json object*/
		ret = zcfgFeObjJsonGet(oid, &objIid, &value);
		//printf("%s() Jessi ret=%d\n", __FUNCTION__, ret);
		paramStrVal = (char *)zcfgFeParamValGet(value, name);
		if(paramStrVal){
			sprintf(strVal,"%s", paramStrVal);
			*len = strlen(strVal);
			json_object_put(value);
		}
		else {
			json_object_put(value);
			return -1;
		}
	}
#endif
	return 0;
}

/*************************
* Function 	: zcfg_set_value()
* Description	: set a single parameter
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
/*set single parameter*/
int zcfg_set_value( char *objname, char *strVal){
	objIndex_t objIid;
	int oid = 0;
	struct json_object *value = NULL;
	char objpath[512] = {0};
	char name[512] = {0};
	char *paramStrVal;
	bool isPartial = 0;
	zcfgRet_t retzcfg = 0;
	int  ret = USP_ERR_OK;

	
	printf("objname=%s\n", objname);

	if( cwmp_cm_parse_fullpath( objname, objpath, name, &isPartial) != 0){
		return USP_ERR_INTERNAL_ERROR;
	}
	if( isPartial ){
		return USP_ERR_INTERNAL_ERROR;
	}
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	if(oid < 0 ) {
		return USP_ERR_INVALID_PATH;
	}
#if HIDDEN_FLAG
	/*check is hidden object or not*/
	if( zcfgFeObjAttrGet(oid)&OBJECT_ATTR_HIDDEN){
		return USP_ERR_INTERNAL_ERROR;
	}
#endif
	
	retzcfg = zcfgFeObjJsonGet(oid, &objIid, &value);
	printf("retzcfg=%d\n", retzcfg);
	if (retzcfg != ZCFG_SUCCESS && retzcfg != ZCFG_EMPTY_OBJECT) {
		json_object_put(value);
		value = NULL;
		return USP_ERR_INTERNAL_ERROR;
	}


	/*You can use a loop here to add all parameters into a json object*/
	retzcfg = zcfgFeJsonObjParamSet(oid, name, getZcfgJSONParameterType(objname), value, strVal);
	/*end*/

	if(retzcfg != ZCFG_SUCCESS){
		printf("zcfgFeJsonObjParamSet %s.%s fail, retzcfg = %d \n",objpath, name, retzcfg);
		if(retzcfg == ZCFG_ERROR_PARAMETER_TYPE){
			ret = USP_ERR_INVALID_TYPE;
		}
		else if(retzcfg == ZCFG_INVALID_PARAM_VALUE)
		{
			ret = USP_ERR_INVALID_TYPE;
		}
		else if( retzcfg == ZCFG_NO_SUCH_PARAMETER){
			ret = USP_ERR_UNSUPPORTED_PARAM;
		}
		else if( retzcfg == ZCFG_SET_READ_ONLY_PARAM){
			ret = USP_ERR_PARAM_READ_ONLY;
		}
		else{
			ret = USP_ERR_INTERNAL_ERROR;
		}
	}
	// cwmp_cm_set_diagnostic_creator(oid, value);
	retzcfg = zcfgFeObjJsonSet(oid, &objIid, value, NULL);
	if( retzcfg != ZCFG_SUCCESS ){
		printf("[%s]zcfgFeObjJsonSet fail, retzcfg = %d!!!\n", __FUNCTION__, retzcfg);
		ret = USP_ERR_INTERNAL_ERROR;
	}

	json_object_put(value);
	value = NULL;

	return ret;
}

/*************************
* Function 	: zcfg_add_object()
* Description	: add an index object
* Output 	: 
*************************/
int zcfg_add_object( char *objname, unsigned int *instanceNum){
	objIndex_t objIid;
	int oid = 0;
	char objnamebuf[512] = {0};
	struct json_object *value = NULL;
	zcfgRet_t retzcfg = 0;
	int ret = USP_ERR_OK;

	IID_INIT(objIid);
	snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objname);
	printf("[%s]objnamebuf = %s\n", __FUNCTION__, objnamebuf);
	oid = zcfgFeObjNameToObjId( objnamebuf, &objIid);
	if(oid < 0 ) {
		printf("[%s] oid= %d\n", __FUNCTION__, oid);
		return USP_ERR_INVALID_PATH;
	}


	// TODO: Need check instance before add
#if HIDDEN_FLAG
	/*check is hidden object or not*/
	if( zcfgFeObjAttrGet(oid)&OBJECT_ATTR_HIDDEN){
		return USP_ERR_INTERNAL_ERROR;
	}
#endif

	retzcfg = zcfgFeObjJsonAdd(oid, &objIid, NULL);

	if( retzcfg == ZCFG_SUCCESS) {
		*instanceNum = objIid.idx[objIid.level-1];

		//Set object default vale
		value = json_object_new_object();
		zcfgFeGetSchemaParameterByOid(oid, value);
		retzcfg = zcfgFeObjJsonSet(oid, &objIid, value, NULL);
		if( retzcfg != ZCFG_SUCCESS ){
			printf("[%s]zcfgFeObjJsonSet fail, retzcfg = %d!!!\n", __FUNCTION__, retzcfg);
			ret = USP_ERR_INTERNAL_ERROR;
		}
		json_object_put(value);
	}
	else if( retzcfg == ZCFG_EXCEED_MAX_INS_NUM ){
		ret = USP_ERR_CREATION_FAILURE;
	}
	else if( retzcfg == ZCFG_NOT_INSTANCE ){
		ret = USP_ERR_NOT_A_TABLE;
	}
	else if( retzcfg == ZCFG_INS_CREATE_ERR ) {
		ret = USP_ERR_OBJECT_NOT_CREATABLE;
	}
	else {
		ret = USP_ERR_INTERNAL_ERROR;
	}
	return ret;
}

/*************************
* Function 	: zcfg_del_object()
* Description	: delete an index object
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
int zcfg_del_object( char *objname ){
	objIndex_t objIid;
	int oid = 0;
	char objpath[512] = {0};
	char name[512] = {0};
	bool isPartial = 0;
	zcfgRet_t retzcfg = 0;
	int ret = USP_ERR_OK;


	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objname, &objIid);
	if(oid < 0 ) {
		return USP_ERR_INVALID_PATH;
	}
	printf("[%s]objIid.idx[objIid.level-1] = %d\n", __FUNCTION__, objIid.idx[objIid.level-1]);

	retzcfg = zcfgFeObjJsonDel(oid, &objIid, NULL); //FIXME: Run zcfgFeObjJsonDel had return ZCFG_INTERNAL_ERROR,But delete is finish....
	printf("[%s]retzcfg = %d\n", __FUNCTION__, retzcfg); 

	// if(retzcfg == ZCFG_SUCCESS) {
	// 	ret = 0;
	// }
	// else if( retzcfg == ZCFG_DELAY_APPLY ){
	// 	ret = -1;
	// }
	// else{
	// 	ret = -1;
	// }

	return ret;
}

/*********************************************************************//**
**
** getZcfgJSONParameterType
**
** Obtains the type of the specified parameter
** NOTE: This function MUST only ever be called on parameter (not object) paths that have already been validated
**
** \param   path - full data model path of the parameter
**
** \return  type of the parameter as a json_type
**
**************************************************************************/
json_type getZcfgJSONParameterType(char *path)
{
    dm_node_t *node;
    unsigned type_flags;
    json_type type;

    node = DM_PRIV_GetNodeFromPath(path, NULL, NULL);
    USP_ASSERT(node != NULL);  // because the path we queried was generated by the path resolver, so we expect it to exist
    USP_ASSERT( ((node->type != kDMNodeType_Object_MultiInstance) &&
                 (node->type != kDMNodeType_Object_SingleInstance) &&
                 (node->type != kDMNodeType_SyncOperation) &&
                 (node->type != kDMNodeType_AsyncOperation) &&
                 (node->type != kDMNodeType_Event)) );

    // Calculate the type of this parameter
    type_flags = node->registered.param_info.type_flags;
    if (type_flags & DM_INT)
    {
        type = json_type_int;
    }
	else if (type_flags & DM_UINT)
    {
        type = json_type_uint32;
    }
	else if (type_flags & DM_ULONG)
    {
        type = json_type_ulong;
    }
	else if (type_flags & DM_DATETIME)
    {
        type = json_type_time;
    }
    else if (type_flags & DM_BOOL)
    {
        type = json_type_boolean;
    }
    else
    {
        // Default, and also for DM_STRING
        type = json_type_string;
    }

    return type;
}

/*********************************************************************//**
**
** DEVICE_CELLULAR_Init
**
** Initialises this component, and registers all parameters which it implements
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CELLULAR_Init(void)
{
    int err = USP_ERR_OK;

    // Register parameters implemented by this component
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_CELL_ROOT ".RoamingEnabled", cell_get_value, cell_set_value, cell_set_notify_value, DM_BOOL);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CELL_ROOT ".RoamingStatus", cell_get_value, DM_STRING);
//#define GROUP_OBJ_TEST
#ifdef GROUP_OBJ_TEST
	printf("[%s:%d] call USP_REGISTER_GroupedObject\n",__func__, __LINE__);
	err |= USP_REGISTER_GroupedObject(1, DEVICE_INTERFACE_ROOT ".{i}", true);//group test
	err |= USP_REGISTER_Object_RefreshInstances(DEVICE_INTERFACE_ROOT ".{i}", cwmp_cm_get_recursive_names);
	printf("[%s:%d] after USP_REGISTER_Object_RefreshInstances cwmp_cm_get_recursive_names\n",__func__, __LINE__);

    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.Enable", DM_BOOL); 
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Status", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Name", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.LastChange", DM_UINT);
    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.LowerLayers", DM_STRING); 
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Upstream", DM_BOOL);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.IMEI", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.SupportedAccessTechnologies", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.PreferredAccessTechnology", DM_STRING); 
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.CurrentAccessTechnology", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.AvailableNetworks", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.NetworkRequested", DM_STRING); 
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.NetworkInUse", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.RSSI", DM_INT);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.RSRP", DM_INT);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.RSRQ", DM_INT);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.UpstreamMaxBitRate", DM_UINT);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.DownstreamMaxBitRate", DM_UINT);

    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.Status", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.IMSI", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.ICCID", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.MSISDN", DM_STRING);
    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.PINCheck", DM_STRING); 
    err |= USP_REGISTER_GroupedVendorParam_ReadWrite(1, DEVICE_INTERFACE_ROOT ".{i}.USIM.PIN", DM_STRING);

    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.BytesSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.BytesReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.PacketsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.PacketsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.ErrorsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.ErrorsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.UnicastPacketsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.UnicastPacketsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.DiscardPacketsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.DiscardPacketsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.MulticastPacketsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.MulticastPacketsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.BroadcastPacketsSent", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.BroadcastPacketsReceived", DM_ULONG);
    err |= USP_REGISTER_GroupedVendorParam_ReadOnly(1, DEVICE_INTERFACE_ROOT ".{i}.Stats.UnknownProtoPacketsReceived", DM_ULONG);


	err |= USP_REGISTER_GroupVendorHooks(1, GetMyParams, SetMyParams, AddMyObject, DelMyObject);//group test
#else
    err |= USP_REGISTER_Object(DEVICE_INTERFACE_ROOT ".{i}", NULL/*Validate Add*/, cell_add, cell_add_notify,
                                                        NULL/*validate del*/, cell_del, NULL/*Notify del*/);
	err |= USP_REGISTER_Object_RefreshInstances(DEVICE_INTERFACE_ROOT ".{i}", cwmp_cm_get_recursive_names);
	printf("[%s]RefreshInstances %s\n", __FUNCTION__, DEVICE_INTERFACE_ROOT);
    err |= USP_REGISTER_DBParam_Alias(DEVICE_INTERFACE_ROOT ".{i}.Alias", NULL);
    err |= USP_REGISTER_Param_NumEntries(DEVICE_CELL_ROOT ".InterfaceNumberOfEntries", DEVICE_INTERFACE_ROOT ".{i}");
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.Enable", cell_get_value, cell_set_value, cell_set_notify_value, DM_BOOL); 
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Status", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Name", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.LastChange", cell_get_value, DM_UINT);
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.LowerLayers", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Upstream", cell_get_value, DM_BOOL);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.IMEI", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.SupportedAccessTechnologies", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.PreferredAccessTechnology", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.CurrentAccessTechnology", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.AvailableNetworks", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.NetworkRequested", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.NetworkInUse", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.RSSI", cell_get_value, DM_INT);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.RSRP", cell_get_value, DM_INT);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.RSRQ", cell_get_value, DM_INT);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.UpstreamMaxBitRate", cell_get_value, DM_UINT);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.DownstreamMaxBitRate", cell_get_value, DM_UINT);

    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.USIM.Status", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.USIM.IMSI", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.USIM.ICCID", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.USIM.MSISDN", cell_get_value, DM_STRING);
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.USIM.PINCheck", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_INTERFACE_ROOT ".{i}.USIM.PIN", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING);

    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.BytesSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.BytesReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.PacketsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.PacketsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.ErrorsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.ErrorsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.UnicastPacketsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.UnicastPacketsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.DiscardPacketsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.DiscardPacketsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.MulticastPacketsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.MulticastPacketsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.BroadcastPacketsSent", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.BroadcastPacketsReceived", cell_get_value, DM_ULONG);
    err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_INTERFACE_ROOT ".{i}.Stats.UnknownProtoPacketsReceived", cell_get_value, DM_ULONG);
#endif
    err |= USP_REGISTER_Object(DEVICE_ACCESSPOINT_ROOT ".{i}", NULL/*Validate Add*/, cell_add, cell_add_notify,
                                                        NULL/*validate del*/, cell_del, NULL/*Notify del*/);
	err |= USP_REGISTER_Object_RefreshInstances(DEVICE_ACCESSPOINT_ROOT ".{i}", cwmp_cm_get_recursive_names);
	printf("[%s]RefreshInstances %s\n", __FUNCTION__, DEVICE_ACCESSPOINT_ROOT);
    err |= USP_REGISTER_DBParam_Alias(DEVICE_ACCESSPOINT_ROOT ".{i}.Alias", NULL);
    err |= USP_REGISTER_Param_NumEntries(DEVICE_CELL_ROOT ".AccessPointNumberOfEntries", DEVICE_ACCESSPOINT_ROOT ".{i}");
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.Enable", cell_get_value, cell_set_value, cell_set_notify_value, DM_BOOL); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.APN", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.Username", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.Password", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.Proxy", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.ProxyPort", cell_get_value, cell_set_value, cell_set_notify_value, DM_UINT); 
    err |= USP_REGISTER_VendorParam_ReadWrite(DEVICE_ACCESSPOINT_ROOT ".{i}.Interface", cell_get_value, cell_set_value, cell_set_notify_value, DM_STRING); 

    // Register unique keys for all tables
    char *interface_unique_keys[] = { "Name" };
    err |= USP_REGISTER_Object_UniqueKey(DEVICE_INTERFACE_ROOT ".{i}", interface_unique_keys, NUM_ELEM(interface_unique_keys));


    char *accesspoint_unique_keys[] = { "APN" };
    err |= USP_REGISTER_Object_UniqueKey(DEVICE_ACCESSPOINT_ROOT ".{i}", accesspoint_unique_keys, NUM_ELEM(accesspoint_unique_keys));


    // Exit if any errors occurred
    if (err != USP_ERR_OK)
    {
		printf("[%s]rrors occurred\n", __FUNCTION__);
        return USP_ERR_INTERNAL_ERROR;
    }

    // If the code gets here, then registration was successful
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** DEVICE_CELLULAR_Start
**
** Initialises the cellular array with the values of all cellular from the DB
** NOTE: If the database contains invalid data, then entries will be deleted
**       We need to do this otherwise it would be possible to set bad DB values to good,
**       but our code would not pick them up because they were not in the internal data structure
**       This function ensures that the database and the internal controller data structure it populates always match
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CELLULAR_Start(void)
{
    int i;
    int err;
    int_vector_t iv;
    int cell_instance;
    char path[MAX_DM_PATH];

    // Exit if unable to get the object instance numbers present in the cellular table
    INT_VECTOR_Init(&iv);
    err = DATA_MODEL_GetInstances(DEVICE_INTERFACE_ROOT, &iv);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // Exit, issuing a warning, if no cellular are present in database
    if (iv.num_entries == 0)
    {
        USP_LOG_Warning("%s: WARNING: No instances in %s", __FUNCTION__, DEVICE_INTERFACE_ROOT);
        err = USP_ERR_OK;
        goto exit;
    }
    printf("[%s]iv.num_entries = %d\n", __FUNCTION__, iv.num_entries);
    // Add all cellular from the cellular table to the cellular array
    for (i=0; i < iv.num_entries; i++)
    {
        cell_instance = iv.vector[i];
        err = ProcessCellularAdded(cell_instance);
        if (err != USP_ERR_OK)
        {
            // Exit if unable to delete a controller with bad parameters from the DB
            USP_SNPRINTF(path, sizeof(path), "%s.%d", DEVICE_INTERFACE_ROOT, cell_instance);
            USP_LOG_Warning("%s: Deleting %s as it contained invalid parameters.", __FUNCTION__, path);
            err = DATA_MODEL_DeleteInstance(path, 0);
            if (err != USP_ERR_OK)
            {
                goto exit;
            }
        }
    }

    err = USP_ERR_OK;

exit:
    // Destroy the vector of instance numbers for the table
    INT_VECTOR_Destroy(&iv);
    return err;
}

/*********************************************************************//**
**
** DEVICE_CELLULAR_Stop
**
** Frees up all memory associated with this module
**
** \param   None
**
** \return  None
**
**************************************************************************/
void DEVICE_CELLULAR_Stop(void)
{
    // int i;
    // controller_t *cont;

    // // Iterate over all controllers, freeing all memory used by them
    // for (i=0; i<MAX_CONTROLLERS; i++)
    // {
    //     cont = &controllers[i];
    //     if (cont->instance != INVALID)
    //     {
    //         DestroyController(cont);
    //     }
    // }
}

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_FindInstanceByEndpointId
// **
// ** Gets the instance number of the enabled controller (in Device.Controller.{i}) based on the specified endpoint_id
// **
// ** \param   endpoint_id - controller that we want to find the instance number of
// **
// ** \return  instance number of controller, or INVALID if unable to find the enabled controller
// **
// **************************************************************************/
// int DEVICE_CONTROLLER_FindInstanceByEndpointId(char *endpoint_id)
// {
//     controller_t *cont;

//     // Exit if no endpoint_id set by caller
//     if (endpoint_id == NULL)
//     {
//         return INVALID;
//     }

//     // Exit if unable to find a matching, enabled controller
//     cont = FindEnabledControllerByEndpointId(endpoint_id);
//     if (cont == NULL)
//     {
//         return INVALID;
//     }

//     // Found the matching instance
//     return cont->instance;
// }

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_FindEndpointIdByInstance
// **
// ** Gets the endpoint_id of the specified enabled controller
// **
// ** \param   instance - instance number of the controller in the Device.Controller.{i} table
// **
// ** \return  pointer to endpoint_id of the controller, or NULL if no controller found, or controller was disabled
// **
// **************************************************************************/
// char *DEVICE_CONTROLLER_FindEndpointIdByInstance(int instance)
// {
//     controller_t *cont;

//     // Exit if unable to find a matching, enabled controller
//     cont = FindControllerByInstance(instance);
//     if ((cont == NULL) || (cont->enable == false))
//     {
//         return NULL;
//     }

//     // Found the matching instance
//     return cont->endpoint_id;
// }

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_GetSubsRetryParams
// **
// ** Gets the subscription retry parameters for the specified endpoint_id
// **
// ** \param   endpoint_id - controller that we want to get the retry parameters for
// ** \param   min_wait_interval - pointer to variable in which to return the minimum wait interval
// ** \param   interval_multiplier - pointer to variable in which to return the interval multiplier
// **
// ** \return  USP_ERR_OK if successful
// **
// **************************************************************************/
// int DEVICE_CONTROLLER_GetSubsRetryParams(char *endpoint_id, unsigned *min_wait_interval, unsigned *interval_multiplier)
// {
//     controller_t *cont;

//     // Exit if unable to find a matching, enabled controller
//     cont = FindEnabledControllerByEndpointId(endpoint_id);
//     if (cont == NULL)
//     {
//         USP_LOG_Warning("%s: Unable to find enabled controller with endpoint_id=%s", __FUNCTION__, endpoint_id);
//         return USP_ERR_INTERNAL_ERROR;
//     }

//     // Found the matching instance, so copy the retry params
//     *min_wait_interval = cont->subs_retry_min_wait_interval;
//     *interval_multiplier = cont->subs_retry_interval_multiplier;
//     return USP_ERR_OK;
// }

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_GetCombinedRole
// **
// ** Gets the inherited and assigned role to use for the specified controller instance
// ** This is used when resolving paths used by subscriptions
// **
// ** \param   instance - instance number of the controller in Device.LocalAgent.Controller.{i}
// ** \param   combined_role - pointer to variable in which to return the combined role
// **
// ** \return  USP_ERR_OK if successful
// **
// **************************************************************************/
// int DEVICE_CONTROLLER_GetCombinedRole(int instance, combined_role_t *combined_role)
// {
//     controller_t *cont;

//     // Exit if unable to find a matching enabled controller
//     cont = FindControllerByInstance(instance);
//     if ((cont == NULL) || (cont->enable == false))
//     {
//         return USP_ERR_INTERNAL_ERROR;
//     }


//     // Copy across the combined role values
//     *combined_role = cont->combined_role;

//     return USP_ERR_OK;
// }

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_GetCombinedRoleByEndpointId
// **
// ** Gets the combined role to use for the specified controller endpoint_id, when
// ** processing request messages from that controller
// **
// ** \param   endpoint_id - endpoint_id of the controller
// **
// ** \return  USP_ERR_OK if successful
// **
// **************************************************************************/
// int DEVICE_CONTROLLER_GetCombinedRoleByEndpointId(char *endpoint_id, combined_role_t *combined_role)
// {
//     controller_t *cont;

//     // Exit if unable to find a matching enabled controller
//     cont = FindEnabledControllerByEndpointId(endpoint_id);
//     if (cont == NULL)
//     {
//         return USP_ERR_INTERNAL_ERROR;
//     }

//     // Copy across the combined role values
//     *combined_role = cont->combined_role;

//     return USP_ERR_OK;
// }

// #ifndef DISABLE_STOMP
// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_SetRolesFromStomp
// **
// ** Sets the controller trust role to use for all controllers connected to the specified STOMP controller
// **
// ** \param   stomp_instance - STOMP instance (in Device.STOMP.Connection table)
// ** \param   role - Role allowed for this message
// ** \param   allowed_controllers - URN pattern containing the endpoint_id of allowed controllers
// **
// ** \return  None
// **
// **************************************************************************/
// void DEVICE_CONTROLLER_SetRolesFromStomp(int stomp_instance, ctrust_role_t role, char *allowed_controllers)
// {
//     int i, j;
//     controller_t *cont;
//     controller_mtp_t *mtp;

//     // Iterate over all enabled controllers
//     for (i=0; i<MAX_CONTROLLERS; i++)
//     {
//         cont = &controllers[i];
//         if ((cont->instance != INVALID) && (cont->enable))
//         {
//             // Iterate over all enabled MTP slots for this controller
//             for (j=0; j<MAX_CONTROLLER_MTPS; j++)
//             {
//                 mtp = &cont->mtps[j];
//                 if ((mtp->instance != INVALID) && (mtp->enable))
//                 {
//                     // If this controller is connected to the specified STOMP connection, then set its inherited role
//                     if ((mtp->protocol == kMtpProtocol_STOMP) && (mtp->stomp_connection_instance == stomp_instance))
//                     {
//                         cont->combined_role.inherited = role;
//                     }
//                 }
//             }
//         }
//     }
// }
// #endif

// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_QueueBinaryMessage
// **
// ** Queues a binary message to be sent to a controller
// **
// ** \param   usp_msg_type - Type of USP message contained in pbuf. This is used for debug logging when the message is sent by the MTP.
// ** \param   endpoint_id - controller to send the message to
// ** \param   pbuf - pointer to buffer containing binary protobuf message. Ownership of this buffer passes to protocol handler, if successful
// ** \param   pbuf_len - length of buffer containing protobuf binary message
// ** \param   usp_msg_id - pointer to string containing the msg_id of the serialized USP Message
// ** \param   mrt - details of where this USP response message should be sent
// ** \param   expiry_time - time at which the USP message should be removed from the MTP send queue
// **
// ** \return  USP_ERR_OK if successful
// **
// **************************************************************************/
// int DEVICE_CONTROLLER_QueueBinaryMessage(Usp__Header__MsgType usp_msg_type, char *endpoint_id, unsigned char *pbuf, int pbuf_len, char *usp_msg_id, mtp_reply_to_t *mrt, time_t expiry_time)
// {
//     int err = USP_ERR_INTERNAL_ERROR;
//     controller_t *cont;
//     controller_mtp_t *mtp;
//     mtp_reply_to_t dest;

//     // Take a copy of the MTP destination parameters we've been given
//     // because we may modify it (and we don't want the caller to free anything we put in it, as they are owned by the data model)
//     memcpy(&dest, mrt, sizeof(dest));

//     // Exit if unable to find the specified controller
//     cont = FindEnabledControllerByEndpointId(endpoint_id);
//     if (cont == NULL)
//     {
//         USP_ERR_SetMessage("%s: Unable to find an enabled controller to send to endpoint_id=%s", __FUNCTION__, endpoint_id);
//         return USP_ERR_INTERNAL_ERROR;
//     }

//     // Exit if unable to find a controller MTP to send this message on
//     mtp = FindFirstEnabledMtp(cont, mrt->protocol);
//     if (mtp == NULL)
//     {
//         USP_ERR_SetMessage("%s: Unable to find a valid controller MTP to send to endpoint_id=%s", __FUNCTION__, endpoint_id);
//         return USP_ERR_INTERNAL_ERROR;
//     }

//     // --------------------------------------------------------------------
//     // If 'reply-to' was not specified, then use the data model to fill in where the response should be sent
//     // This is always the case for notifications, since they are not a response to any incoming USP message
//     if (mrt->is_reply_to_specified == false)
//     {
//         switch(mtp->protocol)
//         {
// #ifndef DISABLE_STOMP
//             case kMtpProtocol_STOMP:
//                 if (mtp->stomp_connection_instance == INVALID)
//                 {
//                     USP_ERR_SetMessage("%s: No Stomp connection in controller MTP to send to endpoint_id=%s", __FUNCTION__, endpoint_id);
//                     return USP_ERR_INTERNAL_ERROR;
//                 }

//                 dest.protocol = kMtpProtocol_STOMP;
//                 dest.stomp_instance = mtp->stomp_connection_instance;
//                 dest.stomp_dest = mtp->stomp_controller_queue;
//                 break;
// #endif

// #ifdef ENABLE_COAP
//             case kMtpProtocol_CoAP:
//                 dest.protocol = kMtpProtocol_CoAP;
//                 dest.coap_host = mtp->coap_controller_host;
//                 dest.coap_port = mtp->coap.port;
//                 dest.coap_resource = mtp->coap.resource;
//                 dest.coap_encryption = mtp->coap.enable_encryption;
//                 dest.coap_reset_session_hint = false;
//                 break;
// #endif

// #ifdef ENABLE_MQTT
//             case kMtpProtocol_MQTT:
//                 if (mtp->mqtt_connection_instance == INVALID)
//                 {
//                     USP_ERR_SetMessage("%s: No MQTT client in controller MTP to send to endpoint_id=%s", __FUNCTION__, endpoint_id);
//                     return USP_ERR_INTERNAL_ERROR;
//                 }

//                 dest.protocol = kMtpProtocol_MQTT;
//                 dest.mqtt_instance = mtp->mqtt_connection_instance;
//                 dest.mqtt_topic = mtp->mqtt_controller_topic;
//                 break;
// #endif
//             default:
//                 TERMINATE_BAD_CASE(mtp->protocol);
//                 break;
//         }
//     }

//     // --------------------------------------------------------------------
//     // Send the response
//     switch(dest.protocol)
//     {
// #ifndef DISABLE_STOMP
//         case kMtpProtocol_STOMP:
//         {
//             char raw_err_id_header[256];        // header's contents before colons have been escaped (to '\c')
//             char err_id_header[256];            // header's contents after colons have been escaped (to '\c')
//             char *agent_queue = DEVICE_MTP_GetAgentStompQueue(dest.stomp_instance);

//             // Form the colon escaped contents of the 'usp-err-id' header
//             USP_SNPRINTF(raw_err_id_header, sizeof(raw_err_id_header), "%s/%s", endpoint_id, usp_msg_id);
//             TEXT_UTILS_ReplaceCharInString(raw_err_id_header, ':', "\\c", err_id_header, sizeof(err_id_header));

//             err = DEVICE_STOMP_QueueBinaryMessage(usp_msg_type, dest.stomp_instance, dest.stomp_dest, agent_queue, pbuf, pbuf_len, err_id_header, expiry_time);
//         }
//             break;
// #endif

// #ifdef ENABLE_COAP
//         case kMtpProtocol_CoAP:
//             err = COAP_CLIENT_QueueBinaryMessage(usp_msg_type, cont->instance, mtp->instance, pbuf, pbuf_len, &dest, expiry_time);
//             break;
// #endif

// #ifdef ENABLE_MQTT
//         case kMtpProtocol_MQTT:
//         {
//             char *response_topic = DEVICE_MTP_GetAgentMqttResponseTopic(dest.mqtt_instance);
//             err = DEVICE_MQTT_QueueBinaryMessage(usp_msg_type, dest.mqtt_instance, dest.mqtt_topic, response_topic, pbuf, pbuf_len);
//         }
//             break;
// #endif
//         default:
//             TERMINATE_BAD_CASE(mrt->protocol);
//             break;
//     }

//     return err;
// }

// #ifndef DISABLE_STOMP
// /*********************************************************************//**
// **
// ** DEVICE_CONTROLLER_NotifyStompConnDeleted
// **
// ** Called when a STOMP connection is deleted
// ** This code unpicks all references to the STOMP connection existing in the Controller MTP table
// **
// ** \param   stomp_instance - instance in Device.STOMP.Connection which has been deleted
// **
// ** \return  None
// **
// **************************************************************************/
// void DEVICE_CONTROLLER_NotifyStompConnDeleted(int stomp_instance)
// {
//     int i;
//     int j;
//     controller_t *cont;
//     controller_mtp_t *mtp;
//     char path[MAX_DM_PATH];

//     // Iterate over all controllers
//     for (i=0; i<MAX_CONTROLLERS; i++)
//     {
//         // Iterate over all MTP slots for this controller, clearing out all references to the deleted STOMP connection
//         cont = &controllers[i];
//         if (cont->instance != INVALID)
//         {
//             for (j=0; j<MAX_CONTROLLER_MTPS; j++)
//             {
//                 mtp = &cont->mtps[j];
//                 if ((mtp->instance != INVALID) && (mtp->protocol == kMtpProtocol_STOMP) && (mtp->stomp_connection_instance == stomp_instance))
//                 {
//                     USP_SNPRINTF(path, sizeof(path), "Device.LocalAgent.Controller.%d.MTP.%d.STOMP.Reference", cont->instance, mtp->instance);
//                     DATA_MODEL_SetParameterValue(path, "", 0);
//                 }
//             }
//         }
//     }
// }
// #endif

/*********************************************************************//**
**
** ProcessCellularAdded
**
** Reads the parameters for the specified Cellular from the database and processes them
**
** \param   cell_instance - instance number of the cellular in the cellular table
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int ProcessCellularAdded(int cell_instance)
{
    // controller_t *cont;
    int err;
//     int i;
//     int_vector_t iv;
//     int mtp_instance;
//     time_t cur_time;
//     time_t base;
//     char path[MAX_DM_PATH];
//     char reference[MAX_DM_PATH];

//     // Exit if unable to add another controller
//     cont = FindUnusedController();
//     if (cont == NULL)
//     {
//         return USP_ERR_RESOURCES_EXCEEDED;
//     }

//     // Initialise to defaults
//     INT_VECTOR_Init(&iv);
//     memset(cont, 0, sizeof(controller_t));
//     cont->instance = cell_instance;
//     cont->combined_role.inherited = ROLE_DEFAULT;
//     cont->combined_role.assigned = ROLE_DEFAULT;

//     for (i=0; i<MAX_CONTROLLER_MTPS; i++)
//     {
//         cont->mtps[i].instance = INVALID;
//     }

//     // Exit if unable to determine whether this controller was enabled or not
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.Enable", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetBool(path, &cont->enable);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if unable to get the periodic base time for this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.PeriodicNotifTime", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetDateTime(path, &base);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }
//     cont->periodic_base = RETRY_WAIT_UseRandomBaseIfUnknownTime(base);


//     // Exit if unable to get the periodic interval for this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.PeriodicNotifInterval", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetUnsigned(path, &cont->periodic_interval);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Calculate the time at which this notification next fires
//     cur_time = time(NULL);
//     cont->next_time_to_fire = CalcNextPeriodicTime(cur_time, cont->periodic_base, cont->periodic_interval);

//     // Update the time at which the next periodic notification should fire
//     UpdateFirstPeriodicNotificationTime();

//     // Exit if unable to get the minimum subs retry interval for this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.USPNotifRetryMinimumWaitInterval", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetUnsigned(path, &cont->subs_retry_min_wait_interval);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if unable to get the subs retry interval multiplier for this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.USPNotifRetryIntervalMultiplier", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetUnsigned(path, &cont->subs_retry_interval_multiplier);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if unable to get the endpoint ID of this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.EndpointID", device_cont_root, cont_instance);
//     err = DM_ACCESS_GetString(path, &cont->endpoint_id);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if the endpoint ID of this controller is not unique
//     err = ValidateEndpointIdUniqueness(cont->endpoint_id, cont_instance);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if unable to get the assigned role of this controller
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.AssignedRole", device_cont_root, cont_instance);
//     err = DATA_MODEL_GetParameterValue(path, reference, sizeof(reference), 0);
//     if (err != USP_ERR_OK)
//     {
//         return err;
//     }

//     // Exit if the value was incorrectly set
//     err = UpdateAssignedRole(cont, reference);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit if unable to get the object instance numbers present in this controller's MTP table
//     USP_SNPRINTF(path, sizeof(path), "%s.%d.MTP", device_cont_root, cont_instance);
//     err = DATA_MODEL_GetInstances(path, &iv);
//     if (err != USP_ERR_OK)
//     {
//         goto exit;
//     }

//     // Exit, issuing a warning, if no MTPs for this controller are present in database
//     if (iv.num_entries == 0)
//     {
//         USP_LOG_Warning("%s: WARNING: No MTP instances for %s.%d", __FUNCTION__, device_cont_root, cont_instance);
//         err = USP_ERR_OK;
//         goto exit;
//     }

//     // Iterate over all MTPs, getting their parameters into the controller structure
//     // Or deleting them from the database, if they contain invalid parameters
//     // NOTE: We need to delete them to prevent them being modified to good values, which then this code does not pickup (because they are not in our internal array)
//     for (i=0; i < iv.num_entries; i++)
//     {
//         mtp_instance = iv.vector[i];

//         err = ProcessControllerMtpAdded(cont, mtp_instance);
//         if (err != USP_ERR_OK)
//         {
//             // Exit if unable to delete a controller MTP with bad parameters from the DB
//             USP_SNPRINTF(path, sizeof(path), "%s.%d.MTP.%d", device_cont_root, cont_instance, mtp_instance);
//             USP_LOG_Warning("%s: Deleting %s as it contained invalid parameters.", __FUNCTION__, path);
//             err = DATA_MODEL_DeleteInstance(path, 0);
//             if (err != USP_ERR_OK)
//             {
//                 goto exit;
//             }
//         }
//     }

    // If the code gets here, then we successfully retrieved all data about the controller (even if some of the MTPs were not added)
    err = USP_ERR_OK;

// exit:
//     if (err != USP_ERR_OK)
//     {
//         DestroyController(cont);
//     }

//     INT_VECTOR_Destroy(&iv);
    return err;
}

/*********************************************************************//**
**
** cell_set_value
**
** Validates that the value is unique across all registered cellular
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - value that the controller would like to set the parameter to
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int cell_set_value(dm_req_t *req, char *buf)
{
	int ret = USP_ERR_OK;
    printf("[%s]Enter\n", __FUNCTION__);
    printf("[%s]%s = %s\n", __FUNCTION__, req->path, buf);
	if((ret = zcfg_set_value(req->path, buf) )!=USP_ERR_OK)
	{
        printf("[%s]zcfg_set_value fail!!!\n", __FUNCTION__);
	}
	return ret;
}

/*********************************************************************//**
** 
** cell_set_notify_value
**
** Function called when cellular is modified
** This function updates the value of the value stored in the controller array
**
** \param   req - pointer to structure identifying the path
** \param   buf - new value of this parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int cell_set_notify_value(dm_req_t *req, char *buf)
{
    printf("[%s]Enter\n", __FUNCTION__);
    printf("[%s]%s = %s\n", __FUNCTION__, req->path, buf);
  
	//uspd_set(req->path, buf);
	return USP_ERR_OK;
}

/*********************************************************************//**
**
** cell_get_value
**
** Gets the value of req->path
** First lookup into the local USPD Database
** else call usp_get method for the requested path
**
** \param   req - pointer to structure identifying the path
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int cell_get_value(dm_req_t *req, char *buf, int len)
{
    printf("[%s]Enter\n", __FUNCTION__);
	if(zcfg_get_value(req->path, buf, &len) != 0)
	{
		printf("[%s]zcfg_get_value fail!!!\n", __FUNCTION__);
		return USP_ERR_INTERNAL_ERROR;
	}

	return USP_ERR_OK;
}

int cell_add(dm_req_t *req)
{
    printf("[%s]Enter\n", __FUNCTION__);
	int_vector_t iv;
	unsigned int instanceNum;

	DATA_MODEL_GetInstances(req->path, &iv);
	printf("[%s]%s, iv.num_entries = %d \n", __FUNCTION__, req->path, iv.num_entries);

	if(zcfg_add_object(req->path, &instanceNum) !=0)
	{
		printf("[%s]zcfg_add_object fail!!!\n", __FUNCTION__);
		return USP_ERR_INTERNAL_ERROR;
	}

	return USP_ERR_OK;
}


int cell_add_notify(dm_req_t *req)
{
    printf("[%s]Enter\n", __FUNCTION__);
	int err = USP_ERR_OK;
#if 0
	char path[MAX_DM_PATH];

	USP_SNPRINTF(path, sizeof(path), "%s.Alias", req->path);
	err = add_object_aliase(path);
#endif
	return err;
}

int cell_del(dm_req_t *req)
{
    printf("[%s]Enter\n", __FUNCTION__);
    printf("[%s]req->pat = %s\n", __FUNCTION__, req->path);
	if(zcfg_del_object(req->path) !=0)
	{
		printf("[%s]zcfg_del_object fail!!!\n", __FUNCTION__);
		return USP_ERR_INTERNAL_ERROR;
	}

	return USP_ERR_OK;
}
