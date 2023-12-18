#include <json/json.h>
#include <ctype.h>
#include <sys/stat.h>
#include "zcfg_common.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcmd_schema.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_msg.h"
#include "zcfg_debug.h"
#include "zcfg_fe_tr98.h"
#include "faultMgmt_parameter.h"

extern tr98Object_t tr98Obj[];


//===========================================================================================================
//===========================================================================================================
/*
 *   TR98 Object Name : InternetGatewayDevice.FaultMgmt.
 *
 *   Related object in TR181: Device.FaultMgmt.
 *
 */
zcfgRet_t faultmgmtObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *faultmgmtJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	//printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_FAULT_MGMT, &objIid, &faultmgmtJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		//printf("%s() paramList->name=%s\n", __FUNCTION__, paramList->name);
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(faultmgmtJobj, paramList->name);
		if(paramValue != NULL){
			//printf("%s() json_object_get_string(paramValue)=%s\n", __FUNCTION__, json_object_get_string(paramValue));
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(faultmgmtJobj);
	
	return ZCFG_SUCCESS;
}

zcfgRet_t faultmgmtObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
{
	bool found = false;
	char tr98Notify[256] = "";
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	//printf("%s : Enter tr181ParamName=%s\n", __FUNCTION__, tr181ParamName);
	if(*tr98NotifyInfo == NULL) {
		*tr98NotifyInfo = json_object_new_object();
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		//printf("%s() paramList->name=%s\n", __FUNCTION__, paramList->name);
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			//printf("%s : tr98Notify=%s\n", __FUNCTION__, tr98Notify);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int faultmgmtObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  faultmgmtOid = 0;
	objIndex_t faultmgmtIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	//printf("%s : Enter tr98FullPathName=%s,paramName=%s\n", __FUNCTION__, tr98FullPathName, paramName);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	//printf("%s : mapObj=%s\n", __FUNCTION__, mapObj);	
	IID_INIT(faultmgmtIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	faultmgmtOid = zcfgFeObjNameToObjId(tr181Obj, &faultmgmtIid);
	//printf("%s : faultmgmtOid=%d\n", __FUNCTION__, faultmgmtOid);	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		//printf("%s : paramList->name=%s\n", __FUNCTION__, paramList->name);
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(faultmgmtOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t faultmgmtObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  faultmgmtOid = 0;
	objIndex_t faultmgmtIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(faultmgmtIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	faultmgmtOid = zcfgFeObjNameToObjId(tr181Obj, &faultmgmtIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(faultmgmtOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(faultmgmtOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, faultmgmtOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.FaultMgmt.CurrentAlarm.i
 *
 *   Related object in TR181: Device.FaultMgmt.CurrentAlarm.i
 *
 */
zcfgRet_t faultmgmtCurrentAlarmObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *faultmgmtJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	//printf("%s : Enter tr98FullPathName=%s\n", __FUNCTION__, tr98FullPathName);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.FaultMgmt.CurrentAlarm.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, &faultmgmtJobj, updateFlag)) != ZCFG_SUCCESS){
		printf("%s : return\n", __FUNCTION__);
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		//printf("%s :  paramList->name=%s\n", __FUNCTION__, paramList->name);
		/*Write parameter value from tr181 objects to tr98 object*/ 	
		paramValue = json_object_object_get(faultmgmtJobj, paramList->name);
		if(paramValue != NULL){
			//printf("%s : json_object_get_string(paramValue)=%s\n", __FUNCTION__, json_object_get_string(paramValue));
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(faultmgmtJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t faultmgmtCurrentAlarmObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *faultmgmtJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	//printf("%s : Enter tr98FullPathName=%s\n", __FUNCTION__, tr98FullPathName);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.FaultMgmt.CurrentAlarm.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, &faultmgmtJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = faultmgmtJobj;
		faultmgmtJobj = NULL;
		faultmgmtJobj = zcfgFeJsonMultiObjAppend(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		//printf("%s : paramList->name=%s\n", __FUNCTION__, paramList->name);
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(faultmgmtJobj, paramList->name);
			if(tr181ParamValue != NULL){
				//printf("%s() json_object_get_string tr181ParamValue=%s\n", __FUNCTION__, json_object_get_string(tr181ParamValue));
				json_object_object_add(faultmgmtJobj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}

		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	if(multiJobj){
		json_object_put(tmpObj);
	}
	else{
		if((ret = zcfgFeObjJsonSet(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, faultmgmtJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(faultmgmtJobj);
			return ret;
		}
		json_object_put(faultmgmtJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t faultmgmtCurrentAlarmObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	//printf("%s : Enter tr98FullPathName=%s\n", __FUNCTION__, tr98FullPathName);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, NULL);
	*idx = objIid.idx[0];
	//printf("%s : objIid.idx[0]=%d\n", __FUNCTION__, objIid.idx[0]);
	return ret;
}

zcfgRet_t faultmgmtCurrentAlarmObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	//printf("%s : Enter tr98FullPathName=%s\n", __FUNCTION__, tr98FullPathName);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.FaultMgmt.CurrentAlarm.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_FAULT_MGMT_CURRENT_ALARM, &objIid, NULL);;
}

zcfgRet_t faultmgmtCurrentAlarmObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
{
	bool found = false;
	char tr98Notify[256] = "";
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(*tr98NotifyInfo == NULL) {
		*tr98NotifyInfo = json_object_new_object();
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		//printf("%s() paramList->name=%s\n", __FUNCTION__, paramList->name);
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			//printf("%s() tr181ParamName=%s\n", __FUNCTION__, tr181ParamName);
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			//printf("%s() tr98Notify=%s\n", __FUNCTION__, tr98Notify);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int faultmgmtCurrentAlarmObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  faultmgmtCurrentAlarmOid = 0;
	objIndex_t faultmgmtCurrentAlarmIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s tr98FullPathName=%s\n", __FUNCTION__, tr98FullPathName);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(faultmgmtCurrentAlarmIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	//printf("%s() tr181Obj=%s\n", __FUNCTION__, tr181Obj);
	faultmgmtCurrentAlarmOid = zcfgFeObjNameToObjId(tr181Obj, &faultmgmtCurrentAlarmIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		//printf("%s() paramList->name=%s\n", __FUNCTION__, paramList->name);
		if(strcmp(paramList->name, paramName)){
			//printf("%s() paramName=%s\n", __FUNCTION__, paramName);
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(faultmgmtCurrentAlarmOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t faultmgmtCurrentAlarmObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  faultmgmtCurrentAlarmOid = 0;
	objIndex_t faultmgmtCurrentAlarmIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(faultmgmtCurrentAlarmIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	faultmgmtCurrentAlarmOid = zcfgFeObjNameToObjId(tr181Obj, &faultmgmtCurrentAlarmIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(faultmgmtCurrentAlarmOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(faultmgmtCurrentAlarmOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, faultmgmtCurrentAlarmOid, paramList->name);
		}
		break;
	}
	
	return ret;
}