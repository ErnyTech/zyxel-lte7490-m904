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
#include "zyExt_parameter.h"

extern tr98Object_t tr98Obj[];

//InternetGatewayDevice.X_ZYXEL_EXT.
zcfgRet_t zyExtObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
#if 0
	char mappingPathName[128] = {0};

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mappingPathName) != ZCFG_SUCCESS){
		return ZCFG_NO_SUCH_OBJECT;
	}

	printf("%s: %s\n", __FUNCTION__, mappingPathName);
#endif

	struct json_object *extObj = NULL, *paramObj;
	objIndex_t objIid;
	IID_INIT(objIid);
	zcfgRet_t rst = ZCFG_NO_SUCH_OBJECT;

	if((rst = zcfgFeObjJsonGet(RDM_OID_ZY_EXT, &objIid, &extObj)) != ZCFG_SUCCESS){
		printf("%s: retrieve X_ZYXEL_EXT object fail\n", __FUNCTION__);

		return rst;
	}

	struct tr98Parameter_s *parmList = tr98Obj[handler].parameter;
	if(!parmList){
		return ZCFG_NO_SUCH_OBJECT;
	}

	*tr98Jobj = json_object_new_object();
	while(parmList->name){
		paramObj = json_object_object_get(extObj, parmList->name);
		if(paramObj){
			json_object_object_add(*tr98Jobj, parmList->name, JSON_OBJ_COPY(paramObj));
		}
		parmList++;
	}

	json_object_put(extObj);

	return ZCFG_SUCCESS;
}


//InternetGatewayDevice.X_ZYXEL_EXT.
zcfgRet_t zyExtObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj )
{
	struct json_object *extObj = NULL, *paramObj;
	objIndex_t objIid;
	IID_INIT(objIid);

	printf("%s\n", __FUNCTION__);

	struct tr98Parameter_s *parmList = tr98Obj[handler].parameter;
	if(!parmList){
		return ZCFG_NO_SUCH_OBJECT;
	}

	tr98Jobj = zcfgFeRetrieveSpv(tr98FullPathName);

	extObj = json_object_new_object();

	while(parmList->name){
		paramObj = json_object_object_get(tr98Jobj, parmList->name);
		if(paramObj){
			json_object_object_add(extObj, parmList->name, JSON_OBJ_COPY(paramObj));
		}
		parmList++;
	}

	if(multiJobj){
		zcfgFeJsonMultiObjAppend(RDM_OID_ZY_EXT, &objIid, multiJobj, extObj);
		zcfgFeJsonMultiObjAddParam(multiJobj, RDM_OID_ZY_EXT, &objIid, "partialset", json_object_new_boolean(true));
	}else{
		zcfgRet_t rst = zcfgFeObjJsonSet(RDM_OID_ZY_EXT, &objIid, extObj, NULL);
		json_object_put(extObj);

		return rst;
	}
	json_object_put(extObj);

	return ZCFG_SUCCESS;

}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DNSRouteEntry.i
 */
zcfgRet_t zyExtDnsRtObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dnsRouteJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_DNS_RT_ENTRY, &objIid, &dnsRouteJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(dnsRouteJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(dnsRouteJobj);

	return ZCFG_SUCCESS;
}
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DNSRouteEntry.i
 */
zcfgRet_t zyExtDnsRtObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dnsRouteJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_DNS_RT_ENTRY, &objIid, &dnsRouteJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = dnsRouteJobj;
		dnsRouteJobj = NULL;
		dnsRouteJobj = zcfgFeJsonMultiObjAppend(RDM_OID_DNS_RT_ENTRY, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(dnsRouteJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(dnsRouteJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_DNS_RT_ENTRY, &objIid, dnsRouteJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(dnsRouteJobj);
			return ret;
		}
		json_object_put(dnsRouteJobj);
	}

	return ZCFG_SUCCESS;
}
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DNSRouteEntry.i
 */
zcfgRet_t zyExtDnsRtObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_DNS_RT_ENTRY, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DNSRouteEntry.i
 */
zcfgRet_t zyExtDnsRtObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSRouteEntry.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_DNS_RT_ENTRY, &objIid, NULL);;
}

zcfgRet_t zyExtDnsRtObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtDnsRtObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
		

	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtDnsRtObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DNSEntry.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DNSEntry.i
 */
zcfgRet_t zyExtDnsEntryObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dnsJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSEntry.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_DNS_ENTRY, &objIid, &dnsJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(dnsJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(dnsJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtDnsEntryObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dnsJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSEntry.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_DNS_ENTRY, &objIid, &dnsJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = dnsJobj;
		dnsJobj = NULL;
		dnsJobj = zcfgFeJsonMultiObjAppend(RDM_OID_DNS_ENTRY, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(dnsJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(dnsJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_DNS_ENTRY, &objIid, dnsJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(dnsJobj);
			return ret;
		}
		json_object_put(dnsJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtDnsEntryObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_DNS_ENTRY, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtDnsEntryObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.DNSEntry.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_DNS_ENTRY, &objIid, NULL);;
}

#if 0
zcfgRet_t zyExtDnsEntryObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtDnsEntryObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  dnsOid = 0;
	objIndex_t dnsIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(dnsIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	dnsOid = zcfgFeObjNameToObjId(tr181Obj, &dnsIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(dnsOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtDnsEntryObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  dnsOid = 0;
	objIndex_t dnsIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(dnsIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	dnsOid = zcfgFeObjNameToObjId(tr181Obj, &dnsIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(dnsOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(dnsOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, dnsOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DynamicDNS
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DynamicDNS
 */
 zcfgRet_t zyExtDDnsObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ddnsJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_D_DNS, &objIid, &ddnsJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(ddnsJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(ddnsJobj);

	return ZCFG_SUCCESS;
}
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.DynamicDNS
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.DynamicDNS
 */
zcfgRet_t zyExtDDnsObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ddnsJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_D_DNS, &objIid, &ddnsJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = ddnsJobj;
		ddnsJobj = NULL;
		ddnsJobj = zcfgFeJsonMultiObjAppend(RDM_OID_D_DNS, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(ddnsJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(ddnsJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_D_DNS, &objIid, ddnsJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(ddnsJobj);
			return ret;
		}
		json_object_put(ddnsJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtDDnsObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtDDnsObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
		

	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtDDnsObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

#ifdef ZYXEL_SFP_MODULE_SUPPORT
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.Gpon_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_Gpon_Info
 */
 zcfgRet_t zyExtGponInfoObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *gponInfoJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_GPON_INFO, &objIid, &gponInfoJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(gponInfoJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(gponInfoJobj);

	return ZCFG_SUCCESS;
}
#endif

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.System_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_System_Info
 */
 zcfgRet_t zyExtSysInfoObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sysinfoJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_SYSTEM_INFO, &objIid, &sysinfoJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(sysinfoJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(sysinfoJobj);

	return ZCFG_SUCCESS;
}
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.System_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_System_Info
 */
zcfgRet_t zyExtSysInfoObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sysinfoJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_SYSTEM_INFO, &objIid, &sysinfoJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = sysinfoJobj;
		sysinfoJobj = NULL;
		sysinfoJobj = zcfgFeJsonMultiObjAppend(RDM_OID_SYSTEM_INFO, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(sysinfoJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(sysinfoJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_SYSTEM_INFO, &objIid, sysinfoJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(sysinfoJobj);
			return ret;
		}
		json_object_put(sysinfoJobj);
	}
#ifdef CBT_CUSTOMIZATION
        bool isCBTAccountCreated = false;
        struct stat ext;
        if(!stat("/var/CBT_Account", &ext) ){
                isCBTAccountCreated = true;
        }

        rdm_ZyLogCfgGpAccount_t *accountObj = NULL;
        objIndex_t logAccountObjIid;
        IID_INIT(logAccountObjIid);
        char username[64] = {},password[64] = {},default_password[64] = {};
        while(zcfgFeObjJsonGetNext(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &logAccountObjIid, &accountObj) == ZCFG_SUCCESS) {
                if(json_object_get_string(json_object_object_get(accountObj, "Username")) != NULL ){
                        strcpy(username,json_object_get_string(json_object_object_get(accountObj, "Username")) );
                        strcpy(password,json_object_get_string(json_object_object_get(accountObj, "Password")) );
                        strcpy(default_password,json_object_get_string(json_object_object_get(accountObj, "DefaultPassword")) );
                }
                if(isCBTAccountCreated){
					if(strcmp(username,"root") && strcmp(username,"supervisor") && strcmp(username,"admin")){
							if(zcfgFeObjJsonDel(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &logAccountObjIid, NULL) != ZCFG_SUCCESS){
									printf("%s(%d): fail to delete RDM_OID_ZY_LOG_CFG_GP_ACCOUNT",__FUNCTION__,__LINE__);
							}
					}
					if(!strcmp(username,"admin")){
							if(strcmp(password,default_password)){
									json_object_object_add(accountObj, "Password",json_object_new_string(default_password));
									if(zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &logAccountObjIid, accountObj, NULL) != ZCFG_SUCCESS){
											printf("%s(%d): fail to set admin password",__FUNCTION__,__LINE__);
									}
							}
					}
					unlink("/var/CBT_Account");
                }
                zcfgFeJsonObjFree(accountObj);
        }
        
        
	rdm_RemoSrv_t *srvObj = NULL;
	objIndex_t srvObjIid;
	IID_INIT(srvObjIid);
	char srvName[32] = {},srvMode[32] = {};
			
	while(zcfgFeObjJsonGetNext(RDM_OID_REMO_SRV, &srvObjIid, &srvObj) == ZCFG_SUCCESS) {
		if(json_object_get_string(json_object_object_get(srvObj, "Name")) != NULL ){
			strcpy(srvName,json_object_get_string(json_object_object_get(srvObj, "Name")) );
			strcpy(srvMode,json_object_get_string(json_object_object_get(srvObj, "Mode")) );
		}
		if(!strcmp(srvName,"HTTP") || !strcmp(srvName,"HTTPS")){
			json_object_object_add(srvObj, "Mode",json_object_new_string("LAN_ONLY"));
			if(zcfgFeObjJsonSet(RDM_OID_REMO_SRV, &srvObjIid, srvObj, NULL) != ZCFG_SUCCESS){
				printf("%s(%d): fail to set RDM_OID_REMO_SRV",__FUNCTION__,__LINE__);
			}
		}
		zcfgFeJsonObjFree(srvObj);
	}
	unlink("/var/TR069SetMGMT");
#endif // 1
	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.System_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_System_Info
 */
zcfgRet_t zyExtSysInfoObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.System_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_System_Info
 */
int zyExtSysInfoObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
		

	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.System_Info
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_System_Info
 */
zcfgRet_t zyExtSysInfoObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.TR064
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_TR064
 */
 zcfgRet_t zyExtTr064ObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *tr064Jobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_TR064, &objIid, &tr064Jobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(tr064Jobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(tr064Jobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.TR064
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_TR064
 */
zcfgRet_t zyExtTr064ObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *tr064Jobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_TR064, &objIid, &tr064Jobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = tr064Jobj;
		tr064Jobj = NULL;
		tr064Jobj = zcfgFeJsonMultiObjAppend(RDM_OID_TR064, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(tr064Jobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(tr064Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_TR064, &objIid, tr064Jobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(tr064Jobj);
			return ret;
		}
		json_object_put(tr064Jobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.TR064
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_TR064
 */
zcfgRet_t zyExtTr064ObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.TR064
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_TR064
 */
int zyExtTr064ObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  tr064Oid = 0;
	objIndex_t tr064Iid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(tr064Iid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	tr064Oid = zcfgFeObjNameToObjId(tr181Obj, &tr064Iid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(tr064Oid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.TR064
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_TR064
 */
zcfgRet_t zyExtTr064ObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  tr064Oid = 0;
	objIndex_t tr064Iid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(tr064Iid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	tr064Oid = zcfgFeObjNameToObjId(tr181Obj, &tr064Iid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(tr064Oid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(tr064Oid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, tr064Oid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
 zcfgRet_t zyExtVlanGroupObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *vlanJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_VLAN_GROUP, &objIid, &vlanJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(vlanJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(vlanJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
zcfgRet_t zyExtVlanGroupObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *vlanJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_VLAN_GROUP, &objIid, &vlanJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = vlanJobj;
		vlanJobj = NULL;
		vlanJobj = zcfgFeJsonMultiObjAppend(RDM_OID_VLAN_GROUP, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(vlanJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(vlanJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_VLAN_GROUP, &objIid, vlanJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(vlanJobj);
			return ret;
		}
		json_object_put(vlanJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
zcfgRet_t zyExtVlanGroupObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_VLAN_GROUP, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
zcfgRet_t zyExtVlanGroupObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_VLAN_GROUP, &objIid, NULL);;
}

#if 0
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
zcfgRet_t zyExtVlanGroupObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
int zyExtVlanGroupObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  vlanOid = 0;
	objIndex_t vlanIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(vlanIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	vlanOid = zcfgFeObjNameToObjId(tr181Obj, &vlanIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(vlanOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.VlanGroup.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_VlanGroup.i
 */
zcfgRet_t zyExtVlanGroupObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  vlanOid = 0;
	objIndex_t vlanIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(vlanIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	vlanOid = zcfgFeObjNameToObjId(tr181Obj, &vlanIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(vlanOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(vlanOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, vlanOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
 zcfgRet_t zyExtEmailSrvObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *emailJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_MAIL_SERVICE, &objIid, &emailJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(emailJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(emailJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
zcfgRet_t zyExtEmailSrvObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *emailJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_MAIL_SERVICE, &objIid, &emailJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = emailJobj;
		emailJobj = NULL;
		emailJobj = zcfgFeJsonMultiObjAppend(RDM_OID_MAIL_SERVICE, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(emailJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(emailJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_SERVICE, &objIid, emailJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(emailJobj);
			return ret;
		}
		json_object_put(emailJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
zcfgRet_t zyExtEmailSrvObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_MAIL_SERVICE, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
zcfgRet_t zyExtEmailSrvObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_MAIL_SERVICE, &objIid, NULL);;
}
#if 0
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
zcfgRet_t zyExtEmailSrvObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
int zyExtEmailSrvObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  emailOid = 0;
	objIndex_t emailIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(emailIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	emailOid = zcfgFeObjNameToObjId(tr181Obj, &emailIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(emailOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailService.i
 */
zcfgRet_t zyExtEmailSrvObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  emailOid = 0;
	objIndex_t emailIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(emailIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	emailOid = zcfgFeObjNameToObjId(tr181Obj, &emailIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(emailOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(emailOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, emailOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
 zcfgRet_t zyExtEmailEventObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *emailJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &objIid, &emailJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(emailJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(emailJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
zcfgRet_t zyExtEmailEventObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *emailJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &objIid, &emailJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = emailJobj;
		emailJobj = NULL;
		emailJobj = zcfgFeJsonMultiObjAppend(RDM_OID_MAIL_EVENT_CFG, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(emailJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(emailJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_EVENT_CFG, &objIid, emailJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(emailJobj);
			return ret;
		}
		json_object_put(emailJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
zcfgRet_t zyExtEmailEventObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_MAIL_EVENT_CFG, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
zcfgRet_t zyExtEmailEventObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_MAIL_EVENT_CFG, &objIid, NULL);;
}
#if 0
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
zcfgRet_t zyExtEmailEventObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
int zyExtEmailEventObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  emailOid = 0;
	objIndex_t emailIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(emailIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	emailOid = zcfgFeObjNameToObjId(tr181Obj, &emailIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(emailOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.EmailNotification.EmailEventConfig.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_EmailNotification.EmailEventConfig.i
 */
zcfgRet_t zyExtEmailEventObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  emailOid = 0;
	objIndex_t emailIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(emailIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	emailOid = zcfgFeObjNameToObjId(tr181Obj, &emailIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(emailOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(emailOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, emailOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP
 */
 zcfgRet_t zyExtSnmpObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *snmpJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_SNMP, &objIid, &snmpJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(snmpJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(snmpJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP
 */
zcfgRet_t zyExtSnmpObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *snmpJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_SNMP, &objIid, &snmpJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = snmpJobj;
		snmpJobj = NULL;
		snmpJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_SNMP, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(snmpJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(snmpJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_SNMP, &objIid, snmpJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(snmpJobj);
			return ret;
		}
		json_object_put(snmpJobj);
	}

	return ZCFG_SUCCESS;
}
#if 0
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP
 */
zcfgRet_t zyExtSnmpObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP
 */
int zyExtSnmpObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP
 */
zcfgRet_t zyExtSnmpObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	

	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *SnmpTrapJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.%hhu", &objIid.idx[0]);
	
	if((ret = feObjJsonGet(RDM_OID_TRAP_ADDR, &objIid, &SnmpTrapJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;
	
	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(SnmpTrapJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(SnmpTrapJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *SnmpTrapJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_TRAP_ADDR, &objIid, &SnmpTrapJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = SnmpTrapJobj;
		SnmpTrapJobj = NULL;
		SnmpTrapJobj = zcfgFeJsonMultiObjAppend(RDM_OID_TRAP_ADDR, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(SnmpTrapJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(SnmpTrapJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_TRAP_ADDR, &objIid, SnmpTrapJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(SnmpTrapJobj);
			return ret;
		}
		json_object_put(SnmpTrapJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_TRAP_ADDR, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_TRAP_ADDR, &objIid, NULL);;
}

#if 0
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
int zyExtSnmpTrapObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SNMP.TrapAddr.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.X_ZYXEL_SNMP.TrapAddr.i
 */
zcfgRet_t zyExtSnmpTrapObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;
	}
	
	return ret;
}
#endif
#ifdef ISKON_CUSTOMIZATION
zcfgRet_t zyExtRemoteMgmtObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	rdm_RemoSrv_t *remoSrv = NULL;
	rdm_IpIface_t *ipIface = NULL;
	tr98Parameter_t *paramList = NULL;
	char boundWanInterface[256] = {0};
	char boundWanInterfaceName[256] = {0};
	char *pc1, *pc2;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	objIid.idx[0] = 1;

	if ((ret = feObjStructGet(RDM_OID_REMO_SRV, &objIid,  (void **)&remoSrv, updateFlag)) != ZCFG_SUCCESS)
		return ret;
	strcpy(boundWanInterface, remoSrv->BoundInterfaceList);
	zcfgFeObjStructFree(remoSrv);

	if (strlen(boundWanInterface)) {
		pc2 = &boundWanInterface[0];
		do {
			pc1 = pc2;
			pc2 = strstr(pc1, ",");
			if (pc2 != NULL) {
				*pc2 = 0;
				++pc2;
			}
			IID_INIT(objIid);
			objIid.level = 1;
			sscanf(pc1, "IP.Interface.%hhu", &objIid.idx[0]);
			if  ((ret = feObjStructGet(RDM_OID_IP_IFACE, &objIid,  (void **)&ipIface, updateFlag)) == ZCFG_SUCCESS) {
				strcat(boundWanInterfaceName, ipIface->X_ZYXEL_SrvName);
				strcat(boundWanInterfaceName, ",");
				zcfgFeObjStructFree(ipIface);
			}
			if (pc2 == NULL || pc2[0] == 0)
				break;
		} while (1);
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if (strcmp("BoundWANInterfaceList", paramList->name) == 0) {
			json_object_object_add(*tr98Jobj, paramList->name, json_object_new_string(boundWanInterfaceName));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtRemoteMgmtObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;
	char boundWanInterface[256] = {0};
	char boundWanInterfaceName[256] = {0};

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_REMO_MGMT, &objIid, &RemoteMgmtJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = RemoteMgmtJobj;
		RemoteMgmtJobj = NULL;
		RemoteMgmtJobj = zcfgFeJsonMultiObjAppend(RDM_OID_REMO_MGMT, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			if (strcmp("BoundWANInterfaceList", paramList->name) == 0) {
				rdm_IpIface_t *ipIface = NULL;
				char *pc1 = NULL, *pc2 = NULL;
				strcpy(boundWanInterfaceName, json_object_to_json_string(paramValue));
				pc2 = boundWanInterfaceName;
				if (*pc2 == '"') ++pc2;
				do {
					pc1 = pc2;
					// skip space char
					while (*pc1 == ' ')
						++pc1;
					// skip ,
					pc2 = strstr(pc1, ",");
					if (pc2 != NULL) {
						*pc2 = 0;
						++pc2;
						if (*pc2 == '"') *pc2 = 0;
					} else {
						// string end with "
						pc2 = strstr(pc1, "\"");
						if (pc2 != NULL) *pc2 = 0;
					}
					if (strlen(pc1) != 0) {
						IID_INIT(objIid);
						objIid.level = 1;
						while ((ret = feObjStructGetNext(RDM_OID_IP_IFACE, &objIid,  (void **)&ipIface, TR98_GET_WITHOUT_UPDATE)) == ZCFG_SUCCESS) {
							if (strcmp(pc1, ipIface->X_ZYXEL_SrvName) == 0) {
								sprintf(boundWanInterface, "%sIP.Interface.%d,", boundWanInterface, objIid.idx[0]);
								zcfgFeObjStructFree(ipIface);
								break;
							}
							zcfgFeObjStructFree(ipIface);
						}
						if (ret != ZCFG_SUCCESS) {
							printf("can not find interface %s\n", pc1);
							json_object_put(RemoteMgmtJobj);
							return ret;
						}
					}
					if (pc2 == NULL || pc2[0] == 0)
						break;
				} while (1);
				json_object_object_add(RemoteMgmtJobj, paramList->name, json_object_new_string(boundWanInterface));
			}
			paramList++;
			continue;
		}

		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	if(multiJobj){
		json_object_put(tmpObj);
	}
	else {
		if((ret = zcfgFeObjJsonSet(RDM_OID_REMO_MGMT, &objIid, RemoteMgmtJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(RemoteMgmtJobj);
			return ret;
		}
		json_object_put(RemoteMgmtJobj);
	}

	return ZCFG_SUCCESS;
}
#endif

int getCurrentUptime(void){
	FILE * uptimefile = NULL;
	char uptime_chr[32] = {};
	char tmp[32] ={0};
	int uptime = -1;
	int i = 0;

	if((uptimefile = fopen("/proc/uptime", "r")) == NULL){
		printf("%s(%d): fail to get Sys Uptime.",__FUNCTION__,__LINE__);
		return uptime;
	}

	 fgets(uptime_chr, sizeof(uptime_chr), uptimefile);
	 fclose(uptimefile);
	
	while(i < strlen(uptime_chr)){
		if (*(uptime_chr+i) != '.'){
			*(tmp+i) = *(uptime_chr+i);
		}else if(*(uptime_chr+i) == '.'){
			break;
		}
		i++;
	}
	uptime = atoi(tmp);
	
	return uptime;
}
#if 1 // remote access
zcfgRet_t zyExtRemoteMgmtSrvObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtSrvJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_REMO_SRV, &objIid, &RemoteMgmtSrvJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(RemoteMgmtSrvJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtRemoteMgmtSrvObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtSrvJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;
	bool isLanWan = false,isHttpService = false;
#ifdef ISKON_CUSTOMIZATION
	struct json_object *rmsObj=NULL;
	char pv[256] = {0};
	objIndex_t rmsIid;
	bool bil = false;
#endif
	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_REMO_SRV, &objIid, &RemoteMgmtSrvJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = RemoteMgmtSrvJobj;
		RemoteMgmtSrvJobj = NULL;
		RemoteMgmtSrvJobj = zcfgFeJsonMultiObjAppend(RDM_OID_REMO_SRV, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
#ifdef CBT_CUSTOMIZATION
		const char *paramStr = json_object_get_string(paramValue);
		const char *paramName = paramList->name;
		if(!strcmp(paramStr,"LAN_WAN") && !strcmp(paramName,"Mode") ){
			isLanWan = true;
		}
#endif // CBT_CUSTOMIZATION
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
			if(tr181ParamValue != NULL) {
#ifdef ISKON_CUSTOMIZATION
				if (strcmp("BoundInterfaceList", paramList->name) == 0) {
					pv[0] = 0;
					strncpy(pv, json_object_get_string(paramValue), 255);
					bil = true;
				}
#endif
				json_object_object_add(RemoteMgmtSrvJobj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}

		printf("Can't find parameter %s in TR181\n", paramList->name);

		paramList++;	
	}

	if(multiJobj){
		json_object_put(tmpObj);

#ifdef ISKON_CUSTOMIZATION
		if (bil) {
			IID_INIT(rmsIid);
			while ((ret = zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_REMO_SRV, &rmsIid, &rmsObj) == ZCFG_SUCCESS)) {
				json_object_object_add(rmsObj, "BoundInterfaceList", json_object_new_string(pv));
				ret = zcfgFeObjJsonSet(RDM_OID_REMO_SRV, &rmsIid, rmsObj, NULL);
				json_object_put(rmsObj);
			}
		}
#endif

	}
	else{
#ifdef ISKON_CUSTOMIZATION
		if (bil) {
			IID_INIT(rmsIid);
			while ((ret = zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_REMO_SRV, &rmsIid, &rmsObj) == ZCFG_SUCCESS)) {
				json_object_object_add(rmsObj, "BoundInterfaceList", json_object_new_string(pv));
				ret = zcfgFeObjJsonSet(RDM_OID_REMO_SRV, &rmsIid, rmsObj, NULL);
				json_object_put(rmsObj);
			}
		}
#endif
		if((ret = zcfgFeObjJsonSet(RDM_OID_REMO_SRV, &objIid, RemoteMgmtSrvJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(RemoteMgmtSrvJobj);
			return ret;
		}
		json_object_put(RemoteMgmtSrvJobj);
	}
	
#ifdef CBT_CUSTOMIZATION
	int currentUpTime = 0;
	char cmds[128] = {};
	if(isLanWan){
		currentUpTime = getCurrentUptime();
		sprintf(cmds,"echo %d > /var/TR069SetMGMT",currentUpTime);
		system(cmds);
	}
#endif // CBT_CUSTOMIZATION

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtRemoteMgmtSrvObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_REMO_SRV, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtRemoteMgmtSrvObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_REMO_SRV, &objIid, NULL);;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_RemoteManagement.Service.i
 */
zcfgRet_t zyExtRemoteMgmtSrvObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_RemoteManagement.Service.i
 */
int zyExtRemoteMgmtSrvObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.Service.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_RemoteManagement.Service.i
 */
zcfgRet_t zyExtRemoteMgmtSrvObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

zcfgRet_t zyExtRemoteMgmtTrustDomainObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtTrustDomainJobj = NULL;
        struct json_object *paramValue = NULL;
        tr98Parameter_t *paramList = NULL;

        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.TrustDomain.%hhu", &objIid.idx[0]);

        if((ret = feObjJsonGet(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, &RemoteMgmtTrustDomainJobj, updateFlag)) != ZCFG_SUCCESS)
                return ret;

        /*fill up tr98 object from related tr181 objects*/
        *tr98Jobj = json_object_new_object();
        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write parameter value from tr181 objects to tr98 object*/
                paramValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
                if(paramValue != NULL) {
                        json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
                        paramList++;
                        continue;
                }

                /*Not defined in tr181, give it a default value*/
                printf("Can't find parameter %s in TR181\n", paramList->name);
                paramList++;
        }

        json_object_put(RemoteMgmtTrustDomainJobj);

        return ZCFG_SUCCESS;
}

zcfgRet_t zyExtRemoteMgmtTrustDomainObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtTrustDomainJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.TrustDomain.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, &RemoteMgmtTrustDomainJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = RemoteMgmtTrustDomainJobj;
		RemoteMgmtTrustDomainJobj = NULL;
		RemoteMgmtTrustDomainJobj = zcfgFeJsonMultiObjAppend(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(RemoteMgmtTrustDomainJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, RemoteMgmtTrustDomainJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(RemoteMgmtTrustDomainJobj);
			return ret;
		}
		json_object_put(RemoteMgmtTrustDomainJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtRemoteMgmtTrustDomainObjAdd(char *tr98FullPathName, int *idx)
{
        zcfgRet_t ret;
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        ret = zcfgFeObjStructAdd(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, NULL);
        *idx = objIid.idx[0];                                                                                                                                           
        return ret;
}                                                        

zcfgRet_t zyExtRemoteMgmtTrustDomainObjDel(char *tr98FullPathName)
{
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.TrustDomain.%hhu", &objIid.idx[0]);

        return zcfgFeObjStructDel(RDM_OID_REMO_MGMT_TRUST_DOMAIN, &objIid, NULL);;
}
#endif

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpService.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_RemoteManagement.SpService.i
 */
zcfgRet_t zyExtSpRemoteMgmtSrvObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtSrvJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpService.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_SP_REMO_SRV, &objIid, &RemoteMgmtSrvJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(RemoteMgmtSrvJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSpRemoteMgmtSrvObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtSrvJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	struct json_object *RemoteMgmtSrvNameParamValue = NULL;
	tr98Parameter_t *paramList = NULL;
	//bool isLanWan = false,isHttpService = false;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpService.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_SP_REMO_SRV, &objIid, &RemoteMgmtSrvJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = RemoteMgmtSrvJobj;
		RemoteMgmtSrvJobj = NULL;
		RemoteMgmtSrvJobj = zcfgFeJsonMultiObjAppend(RDM_OID_SP_REMO_SRV, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
			RemoteMgmtSrvNameParamValue = json_object_object_get(RemoteMgmtSrvJobj, "Name");
			if(tr181ParamValue != NULL) {
				if(RemoteMgmtSrvNameParamValue != NULL)
				if( (strcmp(json_object_get_string(RemoteMgmtSrvNameParamValue),"HTTP")!=0) && (strcmp(paramList->name,"Port")==0) &&(strcmp(json_object_get_string(json_object_object_get(RemoteMgmtSrvJobj, "Port")),json_object_get_string(paramValue))!=0) ){
					printf("\nforbidden to set %s sparameter %s in TR181\n", json_object_get_string(RemoteMgmtSrvNameParamValue),paramList->name);
					return ZCFG_INVALID_PARAM_VALUE;
				}
				json_object_object_add(RemoteMgmtSrvJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_SP_REMO_SRV, &objIid, RemoteMgmtSrvJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(RemoteMgmtSrvJobj);
			return ret;
		}
		json_object_put(RemoteMgmtSrvJobj);
	}

	return ZCFG_SUCCESS;
}


zcfgRet_t zyExtSpRemoteMgmtTrustDomainObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtTrustDomainJobj = NULL;
        struct json_object *paramValue = NULL;
        tr98Parameter_t *paramList = NULL;

        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpTrustDomain.%hhu", &objIid.idx[0]);

        if((ret = feObjJsonGet(RDM_OID_SP_TRUST_DOMAIN, &objIid, &RemoteMgmtTrustDomainJobj, updateFlag)) != ZCFG_SUCCESS)
                return ret;

        /*fill up tr98 object from related tr181 objects*/
        *tr98Jobj = json_object_new_object();
        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write parameter value from tr181 objects to tr98 object*/
                paramValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
                if(paramValue != NULL) {
                        json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
                        paramList++;
                        continue;
                }

                /*Not defined in tr181, give it a default value*/
                printf("Can't find parameter %s in TR181\n", paramList->name);
                paramList++;
        }

        json_object_put(RemoteMgmtTrustDomainJobj);

        return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSpRemoteMgmtTrustDomainObjAdd(char *tr98FullPathName, int *idx)
{
        zcfgRet_t ret;
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        ret = zcfgFeObjStructAdd(RDM_OID_SP_TRUST_DOMAIN, &objIid, NULL);
        *idx = objIid.idx[0];                                                                                                                                           
        return ret;
}                                                        

zcfgRet_t zyExtSpRemoteMgmtTrustDomainObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *RemoteMgmtTrustDomainJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpTrustDomain.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_SP_TRUST_DOMAIN, &objIid, &RemoteMgmtTrustDomainJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = RemoteMgmtTrustDomainJobj;
		RemoteMgmtTrustDomainJobj = NULL;
		RemoteMgmtTrustDomainJobj = zcfgFeJsonMultiObjAppend(RDM_OID_SP_TRUST_DOMAIN, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(RemoteMgmtTrustDomainJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_SP_TRUST_DOMAIN, &objIid, RemoteMgmtTrustDomainJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(RemoteMgmtTrustDomainJobj);
			return ret;
		}
		json_object_put(RemoteMgmtTrustDomainJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSpRemoteMgmtTrustDomainObjDel(char *tr98FullPathName)
{
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement.SpTrustDomain.%hhu", &objIid.idx[0]);

        return zcfgFeObjStructDel(RDM_OID_SP_TRUST_DOMAIN, &objIid, NULL);;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.IGMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_EXT.IGMP
 */
 zcfgRet_t zyExtIgmpObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *igmpJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;
	const char *Interface = NULL;
	int len = 0;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_IGMP, &objIid, &igmpJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(igmpJobj, paramList->name);
		
		if(!strcmp(paramList->name, "Interface")) {
			Interface = json_object_get_string(paramValue);
			if(Interface) {
				char wanIpConnName[TR98_MAX_OBJ_NAME_LENGTH] = {0};
				char tr98IntfList[TR98_MAX_OBJ_NAME_LENGTH * 5] = {0};
				char interface_str[256] = {0};
				char *ipIfaceName = NULL, *tmp = NULL;
				
				sprintf(interface_str, "%s", Interface);
				ipIfaceName = strtok_r(interface_str, ",", &tmp);
				while (ipIfaceName != NULL) {
					memset(wanIpConnName, 0, TR98_MAX_OBJ_NAME_LENGTH);
					
					if (zcfgFe181To98MappingNameGet(ipIfaceName, wanIpConnName) != ZCFG_SUCCESS) {
						ipIfaceName = strtok_r(NULL, "," ,&tmp);
						continue;
					}
					strcat(tr98IntfList, wanIpConnName);
					strcat(tr98IntfList, ",");
					ipIfaceName = strtok_r(NULL, ",", &tmp);
				}
				len = strlen(tr98IntfList);
				if(tr98IntfList[len-1] == ',')
					tr98IntfList[len-1] = '\0';
				json_object_object_add(*tr98Jobj, paramList->name, json_object_new_string(tr98IntfList));
				paramList++;
				continue;
			}
		
			json_object_object_add(*tr98Jobj, paramList->name, json_object_new_string(""));
			paramList++;
			continue;
		}

		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(igmpJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.IGMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_IGMP
 */
zcfgRet_t zyExtIgmpObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *igmpJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;
	int igmpVersion = 3;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_IGMP, &objIid, &igmpJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = igmpJobj;
		igmpJobj = NULL;
		igmpJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_IGMP, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(igmpJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				if(!strcmp(paramList->name, "Interface")) {
#if 1 //Interface is a non-writable parameter
					paramList++;
					continue;
#else
					const char *Interface = NULL;
					Interface = json_object_get_string(paramValue);
					if(Interface) {
						char tr181IntfList[256] = {0};
						char ipIfaceName[32] = {0};
						char interface_str[TR98_MAX_OBJ_NAME_LENGTH * 10] = {0};
						char *wanIpConnName = NULL, *tmp = NULL;
						int len = 0;
						
						sprintf(interface_str, "%s", Interface);
						wanIpConnName = strtok_r(interface_str, ",", &tmp);
						while (wanIpConnName != NULL) {
							memset(ipIfaceName, 0, 32);
							
							if (zcfgFe98To181MappingNameGet(wanIpConnName, ipIfaceName) != ZCFG_SUCCESS) {
								wanIpConnName = strtok_r(NULL, "," ,&tmp);
								continue;
							}
							if(strstr(ipIfaceName, "PPP.Interface.")){
								zcfgFeTr181IfaceStackHigherLayerGet(ipIfaceName, ipIfaceName);
							}
							strcat(tr181IntfList, ipIfaceName);
							strcat(tr181IntfList, ",");
							wanIpConnName = strtok_r(NULL, ",", &tmp);
						}
						len = strlen(tr181IntfList);
						if(tr181IntfList[len-1] == ',')
							tr181IntfList[len-1] = '\0';
						json_object_object_add(igmpJobj, paramList->name, json_object_new_string(tr181IntfList));
					}
#endif
				}

				if(!strcmp(paramList->name, "IgmpVersion")){
					if(!(json_object_get_int(paramValue) >= 1 && json_object_get_int(paramValue) <=3)){
						json_object_object_add(igmpJobj, "IgmpVersion", json_object_new_int(igmpVersion));
					}
					else
						json_object_object_add(igmpJobj, "IgmpVersion", JSON_OBJ_COPY(paramValue));
				}
				else{
					json_object_object_add(igmpJobj, paramList->name, JSON_OBJ_COPY(paramValue));
				}

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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_IGMP, &objIid, igmpJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(igmpJobj);
			return ret;
		}
		json_object_put(igmpJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.IGMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_IGMP
 */
zcfgRet_t zyExtIgmpObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.IGMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_IGMP
 */
int zyExtIgmpObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.IGMP
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_IGMP
 */
zcfgRet_t zyExtIgmpObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  devInfoOid = 0;
	objIndex_t devInfoIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(devInfoIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

#if defined(SUNRISE_RANDOM_PASSWORD) || defined(SOL_TR69_LOGINACCOUNT) || defined(ZYXEL_SUPPORT_TR98_LOGIN_ACCOUNT)
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_LoginCfg
 */
zcfgRet_t zyExtLogCfgGpObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *LogCfgGpJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;

	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu", &objIid.idx[0]);
#ifdef SOL_TR69_LOGINACCOUNT
	objIid.idx[0] += 1; //Don't display high level
#endif

	if((ret = feObjJsonGet(RDM_OID_ZY_LOG_CFG_GP, &objIid, &LogCfgGpJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(LogCfgGpJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(LogCfgGpJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_LoginCfg
 */
zcfgRet_t zyExtLogCfgGpObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *LogCfgGpJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;
	struct json_object *tr181ParamValue = NULL, *tmpObj = NULL;

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu", &objIid.idx[0]);
#ifdef SOL_TR69_LOGINACCOUNT
	objIid.idx[0] += 1;
#endif

	if((ret = feObjJsonGet(RDM_OID_ZY_LOG_CFG_GP, &objIid, &LogCfgGpJobj, NULL)) != ZCFG_SUCCESS)
		return ret;

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		paramValue = json_object_object_get(tr98Jobj, paramList->name);

		tr181ParamValue = json_object_object_get(LogCfgGpJobj, paramList->name);
		if(paramValue && tr181ParamValue) {
			json_object_object_add(LogCfgGpJobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	if(multiJobj) {
		tmpObj = LogCfgGpJobj;
		LogCfgGpJobj = NULL;
		LogCfgGpJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_LOG_CFG_GP, &objIid, multiJobj, tmpObj);
		json_object_put(tmpObj);
	}
	else {
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP, &objIid, LogCfgGpJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(LogCfgGpJobj);
			return ret;
		}
		json_object_put(LogCfgGpJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.i.Account.i
 *
 *   Related object in TR181:
 *   Device.LoginCfg.LogGp.i.Account.i
 */
zcfgRet_t zyExtLogCfgGpAccountObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	char loginCfgGpAccoutPath[128] = "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu.Account.%hhu";
	uint32_t loginCfgGpAccoutOid = 0;
	objIndex_t loginCfgGpAccoutIid;
	struct json_object *loginCfgGpAccoutObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(loginCfgGpAccoutIid);
	loginCfgGpAccoutIid.level = 2;

	if(sscanf(tr98FullPathName, loginCfgGpAccoutPath, &loginCfgGpAccoutIid.idx[0], &loginCfgGpAccoutIid.idx[1]) != 2) return ZCFG_INVALID_OBJECT;
#ifdef SOL_TR69_LOGINACCOUNT
	loginCfgGpAccoutIid.idx[0] += 1; //Don't display high level
#endif
	loginCfgGpAccoutOid = RDM_OID_ZY_LOG_CFG_GP_ACCOUNT;

	if((ret = feObjJsonGet(loginCfgGpAccoutOid, &loginCfgGpAccoutIid, &loginCfgGpAccoutObj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 rule object*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		paramValue = json_object_object_get(loginCfgGpAccoutObj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}
		/*Not defined in tr181, give it a default value*/
		zcfgLog(ZCFG_LOG_DEBUG, "Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(loginCfgGpAccoutObj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogCfgGpAccountObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	char loginCfgGpAccoutPath[128] = "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu.Account.%hhu";
	uint32_t  loginCfgGpAccoutOid = 0;
	objIndex_t loginCfgGpAccoutIid;
	struct json_object *loginCfgGpAccoutObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	struct json_object *tmpObj = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(loginCfgGpAccoutIid);
	loginCfgGpAccoutIid.level = 2;
	if(sscanf(tr98FullPathName, loginCfgGpAccoutPath, &loginCfgGpAccoutIid.idx[0], &loginCfgGpAccoutIid.idx[1]) != 2) return ZCFG_INVALID_OBJECT;
#ifdef SOL_TR69_LOGINACCOUNT
	loginCfgGpAccoutIid.idx[0] += 1; //Don't display high level
#endif
	loginCfgGpAccoutOid = RDM_OID_ZY_LOG_CFG_GP_ACCOUNT;

	if((ret = zcfgFeObjJsonGet(loginCfgGpAccoutOid, &loginCfgGpAccoutIid, &loginCfgGpAccoutObj)) != ZCFG_SUCCESS)
		return ret;
	if(multiJobj){
		tmpObj = loginCfgGpAccoutObj;
		loginCfgGpAccoutObj = NULL;
		loginCfgGpAccoutObj = zcfgFeJsonMultiObjAppend(loginCfgGpAccoutOid, &loginCfgGpAccoutIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(loginCfgGpAccoutObj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(loginCfgGpAccoutObj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}
		paramList++;
	} /*Edn while*/

	/*Set */
	if(multiJobj){
		json_object_put(tmpObj);
	}
	else{
		if((ret = zcfgFeObjJsonSet(loginCfgGpAccoutOid, &loginCfgGpAccoutIid, loginCfgGpAccoutObj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(loginCfgGpAccoutObj);
			return ret;
		}
		json_object_put(loginCfgGpAccoutObj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogCfgGpAccountObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	char loginCfgGpAccoutPath[128] = "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu.Account";
#ifdef SUNRISE_RANDOM_PASSWORD
	struct json_object *loginCfgGpAccoutObj = NULL;
#endif
	objIndex_t loginCfgGpAccoutIid;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

#ifdef SUNRISE_RANDOM_PASSWORD
	IID_INIT(loginCfgGpAccoutIid);
	loginCfgGpAccoutIid.level = 2;
	loginCfgGpAccoutIid.idx[0] = 1;
	loginCfgGpAccoutIid.idx[1] = 3;

	if(zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginCfgGpAccoutIid, &loginCfgGpAccoutObj) == ZCFG_SUCCESS){
		json_object_put(loginCfgGpAccoutObj);
		return ZCFG_REQUEST_REJECT;
	}
#endif

	IID_INIT(loginCfgGpAccoutIid);
	loginCfgGpAccoutIid.level = 1;
	if(sscanf(tr98FullPathName, loginCfgGpAccoutPath, &loginCfgGpAccoutIid.idx[0]) != 1) return ZCFG_INVALID_OBJECT;
#ifdef SOL_TR69_LOGINACCOUNT
	loginCfgGpAccoutIid.idx[0] += 1; //Don't display high level
#endif

#ifdef SUNRISE_RANDOM_PASSWORD
	if(loginCfgGpAccoutIid.idx[0] != 1)
		return ZCFG_REQUEST_REJECT;
#endif

	if((ret = zcfgFeObjStructAdd(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginCfgGpAccoutIid, NULL)) != ZCFG_SUCCESS) {
		zcfgLog(ZCFG_LOG_DEBUG,"%s : Add LoginCfg.LogGp.%hhu.Account. Fail.\n", __FUNCTION__, loginCfgGpAccoutIid.idx[0]);
		return ret;
	}

	*idx = loginCfgGpAccoutIid.idx[1];

	return ZCFG_SUCCESS;
}

#if defined(SOL_TR69_LOGINACCOUNT) || defined(ZYXEL_SUPPORT_TR98_LOGIN_ACCOUNT)
zcfgRet_t zyExtLogCfgGpAccountObjDel(char *tr98FullPathName)
{
        objIndex_t objIid;
		char loginCfgGpAccoutPath[128] = "InternetGatewayDevice.X_ZYXEL_EXT.LoginCfg.LogGp.%hhu.Account.%hhu";

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        objIid.level = 2;
		if(sscanf(tr98FullPathName, loginCfgGpAccoutPath, &objIid.idx[0], &objIid.idx[1]) != 2)
			return ZCFG_INVALID_OBJECT;
		
#ifdef SOL_TR69_LOGINACCOUNT
		objIid.idx[0] += 1;
#endif
        return zcfgFeObjStructDel(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &objIid, NULL);;
}
#endif

#endif

/* InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting */
zcfgRet_t zyExtLogSettingObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *logSettingJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_LOG_SETTING, &objIid, &logSettingJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(logSettingJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(logSettingJobj);
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogSettingObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *logSettingJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_LOG_SETTING, &objIid, &logSettingJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = logSettingJobj;
		logSettingJobj = NULL;
		logSettingJobj = zcfgFeJsonMultiObjAppend(RDM_OID_LOG_SETTING, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(logSettingJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(logSettingJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_LOG_SETTING, &objIid, logSettingJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(logSettingJobj);
			return ret;
		}
		json_object_put(logSettingJobj);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogSettingObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtLogSettingObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtLogSettingObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(logOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, logOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.i */
zcfgRet_t zyExtLogClassifyObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *logClassifyJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu", &objIid.idx[0]);
	
	if((ret = feObjJsonGet(RDM_OID_LOG_CLASSIFY, &objIid, &logClassifyJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL)
	{
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(logClassifyJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	json_object_put(logClassifyJobj);
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogClassifyObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_LOG_CLASSIFY, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtLogClassifyObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_LOG_CLASSIFY, &objIid, NULL);;
}

zcfgRet_t zyExtLogClassifyObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtLogClassifyObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtLogClassifyObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(logOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, logOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.i.Category.i */
zcfgRet_t zyExtLogCategoryObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	char logObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu.Category.%hhu";
	uint32_t  logOid = 0;
	objIndex_t logIid;
	struct json_object *logObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(logIid);
	logIid.level = 2;
	if(sscanf(tr98FullPathName, logObjFormate, &logIid.idx[0], &logIid.idx[1]) != 2)
		return ZCFG_INVALID_OBJECT;	
	logOid = RDM_OID_LOG_CATEGORY;

	if((ret = feObjJsonGet(logOid, &logIid, &logObj, updateFlag)) != ZCFG_SUCCESS)
		return ret;
	
	/*fill up tr98 rule object*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		paramValue = json_object_object_get(logObj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}
		/*Not defined in tr181, give it a default value*/
		zcfgLog(ZCFG_LOG_DEBUG, "Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(logObj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogCategoryObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault){
	zcfgRet_t ret;
	char categoryPath[128] = "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu.Category.%hhu";
	objIndex_t categoryIid;
	struct json_object *categoryObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	struct json_object *tmpObj = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(categoryIid);
	categoryIid.level = 2;
	if(sscanf(tr98FullPathName, categoryPath, &categoryIid.idx[0], &categoryIid.idx[1]) != 2) return ZCFG_INVALID_OBJECT;

	if((ret = zcfgFeObjJsonGet(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj)) != ZCFG_SUCCESS)
		return ret;
	if(multiJobj){
		tmpObj = categoryObj;
		categoryObj = NULL;
		categoryObj = zcfgFeJsonMultiObjAppend(RDM_OID_LOG_CATEGORY, &categoryIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(categoryObj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(categoryObj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}
		paramList++;
	} /*Edn while*/

	/*Set */
	if(multiJobj){
		json_object_put(tmpObj);
	}
	else{
		if((ret = zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(categoryObj);
			return ret;
		}
		json_object_put(categoryObj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogCategoryObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	char logObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu.Category";
	uint32_t  logOid = 0;
	objIndex_t logIid;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(logIid);
	logIid.level = 1;
	if(sscanf(tr98FullPathName, logObjFormate, &logIid.idx[0]) != 1) return ZCFG_INVALID_OBJECT;	
	logOid = RDM_OID_LOG_CATEGORY;

	if((ret = zcfgFeObjStructAdd(logOid, &logIid, NULL)) != ZCFG_SUCCESS) {
		zcfgLog(ZCFG_LOG_DEBUG,"%s : Add X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu.Category. Fail.\n", __FUNCTION__, logIid.idx[0]);
		return ret;
	}

	*idx = logIid.idx[1];
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtLogCategoryObjDel(char *tr98FullPathName)
{
	zcfgRet_t ret;
	char logObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.Log_Setting.LogClassify.%hhu.Category.%hhu";
	uint32_t  logOid = 0;
	objIndex_t logIid;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(logIid);
	logIid.level = 2;
	if(sscanf(tr98FullPathName, logObjFormate, &logIid.idx[0], &logIid.idx[1]) != 2) return ZCFG_INVALID_OBJECT;	
	logOid = RDM_OID_LOG_CATEGORY;

	ret = zcfgFeObjStructDel(logOid, &logIid, NULL);
	if (ret != ZCFG_SUCCESS) {
		zcfgLog(ZCFG_LOG_DEBUG, "%s : Delete Firewall.Chain.%hhu.Rule.%hhu Fail\n", __FUNCTION__, logIid.idx[0], logIid.idx[1]);
	}

	return ret;
}

zcfgRet_t zyExtLogCategoryObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtLogCategoryObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtLogCategoryObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  logOid = 0;
	objIndex_t logIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(logIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	logOid = zcfgFeObjNameToObjId(tr181Obj, &logIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(logOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(logOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, logOid, paramList->name);
		}
		break;

	}
	
	return ret;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.MacFilter */
zcfgRet_t zyExtMacFilterObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	//char tr181Obj[128] = {0};
	//char mapObj[128] = {0};
	uint32_t  macFilterOid = 0;
	objIndex_t macFilterIid;
	struct json_object *macFilterObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;
	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(macFilterIid);
	/*if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		printf("%d:Get %s failed\n", __LINE__, tr98FullPathName);
		return ZCFG_INVALID_OBJECT;
	}*/
	//snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macFilterOid = RDM_OID_ZY_MAC_FILTER;

	if((ret = feObjJsonGet(macFilterOid, &macFilterIid, &macFilterObj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 firewall object*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		paramValue = json_object_object_get(macFilterObj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}
		/*Not defined in tr181, give it a default value*/
		zcfgLog(ZCFG_LOG_DEBUG, "Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(macFilterObj);
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.MacFilter */
zcfgRet_t zyExtMacFilterObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	//char tr181Obj[128] = {0};
	//char mapObj[128] = {0};
	uint32_t  macFilterOid = 0;
	objIndex_t macFilterIid;
	struct json_object *macFilterObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	struct json_object *tmpObj = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(macFilterIid);
	/*if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		printf("%d:Get %s failed\n", __LINE__, tr98FullPathName);
		return ZCFG_INVALID_OBJECT;
	}*/
	
	//snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macFilterOid = RDM_OID_ZY_MAC_FILTER;
	if((ret = zcfgFeObjJsonGet(macFilterOid, &macFilterIid, &macFilterObj)) != ZCFG_SUCCESS)
		return ret;
	
	if(multiJobj){
		tmpObj = macFilterObj;
		macFilterObj = NULL;
		macFilterObj = zcfgFeJsonMultiObjAppend(macFilterOid, &macFilterIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(macFilterObj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(macFilterObj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}
		paramList++;	
	} /*Edn while*/
	
	/*Set */
	if(multiJobj){
		json_object_put(tmpObj);
	}
	else{
		if((ret = zcfgFeObjJsonSet(macFilterOid, &macFilterIid, macFilterObj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(macFilterObj);
			return ret;
		}
		json_object_put(macFilterObj);
	}
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtMacFilterObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtMacFilterObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  macOid = 0;
	objIndex_t macIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(macIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macOid = zcfgFeObjNameToObjId(tr181Obj, &macIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(macOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtMacFilterObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  macOid = 0;
	objIndex_t macIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(macIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macOid = zcfgFeObjNameToObjId(tr181Obj, &macIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(macOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(macOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, macOid, paramList->name);
		}
		break;

	} /*Edn while*/
	
	return ret;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.MacFilter.WhiteList.i */
zcfgRet_t zyExtMacFilterWhiteListObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	char levelObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.MacFilter.WhiteList.%hhu";
	uint32_t  levelOid = 0;
	objIndex_t levelIid;
	struct json_object *levelObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	IID_INIT(levelIid);
	levelIid.level = 1;
	if(sscanf(tr98FullPathName, levelObjFormate, levelIid.idx) != 1) return ZCFG_INVALID_OBJECT;	
	levelOid = RDM_OID_ZY_MAC_FILTER_WHITE_LIST;

	if((ret = feObjJsonGet(levelOid, &levelIid, &levelObj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 level object*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		paramValue = json_object_object_get(levelObj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}
		/*Not defined in tr181, give it a default value*/
		zcfgLog(ZCFG_LOG_DEBUG, "Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(levelObj);
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtMacFilterWhiteListObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	char whiteListObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.MacFilter.WhiteList.%hhu";
	uint32_t  whiteListOid = 0;
	objIndex_t whiteListIid;
	struct json_object *whiteListObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	struct json_object *tmpObj = NULL;
	tr98Parameter_t *paramList = NULL;
	int index = 0;
	char macAddr[18] = {0};

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(whiteListIid);
	whiteListIid.level = 1;
	if(sscanf(tr98FullPathName, whiteListObjFormate, whiteListIid.idx) != 1) return ZCFG_INVALID_OBJECT;	
	whiteListOid = RDM_OID_ZY_MAC_FILTER_WHITE_LIST;

	if((ret = zcfgFeObjJsonGet(whiteListOid, &whiteListIid, &whiteListObj)) != ZCFG_SUCCESS)
		return ret;
	if(multiJobj){
		tmpObj = whiteListObj;
		whiteListObj = NULL;
		whiteListObj = zcfgFeJsonMultiObjAppend(whiteListOid, &whiteListIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(whiteListObj, paramList->name);
			if(tr181ParamValue != NULL) {
				// -------- check validity of MAC start --------
				if(strcmp(paramList->name, "MACAddress") == 0){
					strcpy(macAddr, json_object_get_string(paramValue));
					if(strcmp(macAddr, "") != 0){
						for(; index < 17; index++) {
							if(index % 3 != 2 && !isxdigit(macAddr[index])){
								json_object_put(whiteListObj);
								return ZCFG_INVALID_ARGUMENTS;
							}
							else if(index % 3 == 2 && macAddr[index] != ':'){
								json_object_put(whiteListObj);
								return ZCFG_INVALID_ARGUMENTS;
							}
						}
						if(macAddr[17] != '\0'){
							json_object_put(whiteListObj);
							return ZCFG_INVALID_ARGUMENTS;
						}
					}
				}
				// -------- check validity of MAC end --------
				json_object_object_add(whiteListObj, paramList->name, JSON_OBJ_COPY(paramValue));
				paramList++;
				continue;
			}
		}		
		paramList++;	
	} /*Edn while*/
	
	/*Set */
	if(multiJobj){
		json_object_put(tmpObj);
	}
	else{
		if((ret = zcfgFeObjJsonSet(whiteListOid, &whiteListIid, whiteListObj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(whiteListObj);
			return ret;
		}
		json_object_put(whiteListObj);
	}
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtMacFilterWhiteListObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_ZY_MAC_FILTER_WHITE_LIST, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtMacFilterWhiteListObjDel(char *tr98FullPathName)
{
	zcfgRet_t ret;
	char levelObjFormate[128] = "InternetGatewayDevice.X_ZYXEL_EXT.MacFilter.WhiteList.%hhu";
	uint32_t  levelOid = 0;
	objIndex_t levelIid;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	IID_INIT(levelIid);
	levelIid.level = 1;
	if(sscanf(tr98FullPathName, levelObjFormate, levelIid.idx) != 1) return ZCFG_INVALID_OBJECT;	
	levelOid = RDM_OID_ZY_MAC_FILTER_WHITE_LIST;


	ret = zcfgFeObjStructDel(levelOid, &levelIid, NULL);
	if (ret != ZCFG_SUCCESS) {
		zcfgLog(ZCFG_LOG_DEBUG, "%s : Delete X_ZYXEL_EXT.MacFilter.WhiteList.%hhu Fail\n", __FUNCTION__, levelIid.idx[0]);
	}

	return ret;
}

zcfgRet_t zyExtMacFilterWhiteListObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtMacFilterWhiteListObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  macOid = 0;
	objIndex_t macIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(macIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macOid = zcfgFeObjNameToObjId(tr181Obj, &macIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(macOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtMacFilterWhiteListObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  macOid = 0;
	objIndex_t macIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(macIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	macOid = zcfgFeObjNameToObjId(tr181Obj, &macIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(macOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(macOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, macOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.ParentalControl
 *
 *   Related object in TR181: Device.X_ZYXEL_ParentalControl
 *
 */
zcfgRet_t zyExtParenCtlObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *parenCtlJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_PAREN_CTL, &objIid, &parenCtlJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(parenCtlJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(parenCtlJobj);
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtParenCtlObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *parenCtlJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_PAREN_CTL, &objIid, &parenCtlJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = parenCtlJobj;
		parenCtlJobj = NULL;
		parenCtlJobj = zcfgFeJsonMultiObjAppend(RDM_OID_PAREN_CTL, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(parenCtlJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(parenCtlJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_PAREN_CTL, &objIid, parenCtlJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(parenCtlJobj);
			return ret;
		}
		json_object_put(parenCtlJobj);
	}
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtParenCtlObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtParenCtlObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  parenCtlOid = 0;
	objIndex_t parenCtlIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(parenCtlIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	parenCtlOid = zcfgFeObjNameToObjId(tr181Obj, &parenCtlIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(parenCtlOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtParenCtlObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  parenCtlOid = 0;
	objIndex_t parenCtlIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(parenCtlIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	parenCtlOid = zcfgFeObjNameToObjId(tr181Obj, &parenCtlIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(parenCtlOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(parenCtlOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, parenCtlOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.ParentalControl.Profile.i
 *
 *   Related object in TR181: Device.X_ZYXEL_ParentalControl.Profile.i
 *
 */
zcfgRet_t zyExtParenCtlProfObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *parenCtlProfJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.ParentalControl.Profile.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_PAREN_CTL_PROF, &objIid, &parenCtlProfJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/ 	
		paramValue = json_object_object_get(parenCtlProfJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(parenCtlProfJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtParenCtlProfObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *parenCtlProfJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.ParentalControl.Profile.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_PAREN_CTL_PROF, &objIid, &parenCtlProfJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = parenCtlProfJobj;
		parenCtlProfJobj = NULL;
		parenCtlProfJobj = zcfgFeJsonMultiObjAppend(RDM_OID_PAREN_CTL_PROF, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(parenCtlProfJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(parenCtlProfJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_PAREN_CTL_PROF, &objIid, parenCtlProfJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(parenCtlProfJobj);
			return ret;
		}
		json_object_put(parenCtlProfJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtParenCtlProfObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_PAREN_CTL_PROF, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtParenCtlProfObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.ParentalControl.Profile.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_PAREN_CTL_PROF, &objIid, NULL);;
}
#if 0
zcfgRet_t zyExtParenCtlProfObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtParenCtlProfObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  parentCtlOid = 0;
	objIndex_t parentCtlIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(parentCtlIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	parentCtlOid = zcfgFeObjNameToObjId(tr181Obj, &parentCtlIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(parentCtlOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtParenCtlProfObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  parentCtlOid = 0;
	objIndex_t parentCtlIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(parentCtlIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	parentCtlOid = zcfgFeObjNameToObjId(tr181Obj, &parentCtlIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(parentCtlOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(parentCtlOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, parentCtlOid, paramList->name);
		}
		break;
	}
	
	return ret;
}
#endif

#ifdef ZYXEL_SSH_PUBLIC_KEY_MGMT
//===========================================================================================================
//===========================================================================================================
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SSH
 *
 *   Related object in TR181: No objects.
 *
 */
zcfgRet_t zyExtSshObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sshJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	//printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_DEV_S_S_H, &objIid, &sshJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(sshJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(sshJobj);
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSshObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtSshObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  sshOid = 0;
	objIndex_t sshIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(sshIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	sshOid = zcfgFeObjNameToObjId(tr181Obj, &sshIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(sshOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtSshObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  sshOid = 0;
	objIndex_t sshIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(sshIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	sshOid = zcfgFeObjNameToObjId(tr181Obj, &sshIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(sshOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(sshOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, sshOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SSH.PublicKey.i
 *
 *   Related object in TR181: No objects.
 *
 */
zcfgRet_t zyExtSshPubKeyObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sshJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SSH.PublicKey.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, &sshJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/ 	
		paramValue = json_object_object_get(sshJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(sshJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSshPubKeyObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sshJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SSH.PublicKey.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, &sshJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = sshJobj;
		sshJobj = NULL;
		sshJobj = zcfgFeJsonMultiObjAppend(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(sshJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(sshJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, sshJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(sshJobj);
			return ret;
		}
		json_object_put(sshJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSshPubKeyObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtSshPubKeyObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.SSH.PublicKey.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_DEV_S_S_H_PUB_KEY, &objIid, NULL);;
}
//===========================================================================================================
//===========================================================================================================
#endif //ZYXEL_SSH_PUBLIC_KEY_MGMT

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.Security
 *
 *   Related object in TR181: Device.Security
 *
 */
zcfgRet_t zyExtSecObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *secJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_DEV_SEC, &objIid, &secJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(secJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(secJobj);
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSecObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtSecObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  secOid = 0;
	objIndex_t secIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(secIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	secOid = zcfgFeObjNameToObjId(tr181Obj, &secIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(secOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtSecObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  secOid = 0;
	objIndex_t secIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(secIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	secOid = zcfgFeObjNameToObjId(tr181Obj, &secIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(secOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(secOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, secOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.Security.Certificate.i
 *
 *   Related object in TR181: Device.Security.Certificate.i
 *
 */
zcfgRet_t zyExtSecCertObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *secJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.Security.Certificate.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_DEV_SEC_CERT, &objIid, &secJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/ 	
		paramValue = json_object_object_get(secJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(secJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSecCertObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *secJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.Security.Certificate.%hhu", &objIid.idx[0]);

	if((ret = zcfgFeObjJsonGet(RDM_OID_DEV_SEC_CERT, &objIid, &secJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = secJobj;
		secJobj = NULL;
		secJobj = zcfgFeJsonMultiObjAppend(RDM_OID_DEV_SEC_CERT, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(secJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(secJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_DEV_SEC_CERT, &objIid, secJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(secJobj);
			return ret;
		}
		json_object_put(secJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSecCertObjAdd(char *tr98FullPathName, int *idx)
{
	zcfgRet_t ret;
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	ret = zcfgFeObjStructAdd(RDM_OID_DEV_SEC_CERT, &objIid, NULL);
	*idx = objIid.idx[0];

	return ret;
}

zcfgRet_t zyExtSecCertObjDel(char *tr98FullPathName)
{
	objIndex_t objIid;

	printf("%s : Enter\n", __FUNCTION__);

	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.Security.Certificate.%hhu", &objIid.idx[0]);

	return zcfgFeObjStructDel(RDM_OID_DEV_SEC_CERT, &objIid, NULL);;
}
#if 0
zcfgRet_t zyExtSecCertObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtSecCertObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  secOid = 0;
	objIndex_t secIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(secIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	secOid = zcfgFeObjNameToObjId(tr181Obj, &secIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(secOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtSecCertObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  secOid = 0;
	objIndex_t secIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(secIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	secOid = zcfgFeObjNameToObjId(tr181Obj, &secIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		attrValue = zcfgFeParamAttrGetByName(secOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(secOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, secOid, paramList->name);
		}
		break;
	}
	
	return ret;
}
#endif
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.SAMBA
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_SAMBA
 */
 zcfgRet_t zyExtSambaObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sambaJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_SAMBA, &objIid, &sambaJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(sambaJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(sambaJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSambaObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *sambaJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_SAMBA, &objIid, &sambaJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = sambaJobj;
		sambaJobj = NULL;
		sambaJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_SAMBA, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(sambaJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(sambaJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_SAMBA, &objIid, sambaJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(sambaJobj);
			return ret;
		}
		json_object_put(sambaJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtSambaObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExtSambaObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  sambaOid = 0;
	objIndex_t sambaIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(sambaIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	sambaOid = zcfgFeObjNameToObjId(tr181Obj, &sambaIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(sambaOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExtSambaObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  sambaOid = 0;
	objIndex_t sambaIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(sambaIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	sambaOid = zcfgFeObjNameToObjId(tr181Obj, &sambaIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(sambaOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(sambaOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, sambaOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.UPNP
 *
 *   Related object in TR181:
 *   Device.UPnP.Device
 *	 OID:RDM_OID_UPNP_DEV	
 */
 zcfgRet_t zyExtUPnPObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *upnpJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_UPNP_DEV, &objIid, &upnpJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(upnpJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(upnpJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtUPnPObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *upnpJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_UPNP_DEV, &objIid, &upnpJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = upnpJobj;
		upnpJobj = NULL;
		upnpJobj = zcfgFeJsonMultiObjAppend(RDM_OID_UPNP_DEV, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(upnpJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(upnpJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_UPNP_DEV, &objIid, upnpJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(upnpJobj);
			return ret;
		}
		json_object_put(upnpJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.8021ag
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_8021ag
 */
zcfgRet_t zyExt8021agObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ieeeJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_IEEE8021AG, &objIid, &ieeeJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(ieeeJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(ieeeJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExt8021agObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ieeeJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IEEE8021AG, &objIid, &ieeeJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = ieeeJobj;
		ieeeJobj = NULL;
		ieeeJobj = zcfgFeJsonMultiObjAppend(RDM_OID_IEEE8021AG, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(ieeeJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(ieeeJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_IEEE8021AG, &objIid, ieeeJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(ieeeJobj);
			return ret;
		}
		json_object_put(ieeeJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExt8021agObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExt8021agObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  ieeeOid = 0;
	objIndex_t ieeeIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(ieeeIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	ieeeOid = zcfgFeObjNameToObjId(tr181Obj, &ieeeIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(ieeeOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExt8021agObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  ieeeOid = 0;
	objIndex_t ieeeIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(ieeeIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	ieeeOid = zcfgFeObjNameToObjId(tr181Obj, &ieeeIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(ieeeOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(ieeeOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, ieeeOid, paramList->name);
		}
		break;
	}
	
	return ret;
}

#ifdef ZYXEL_ETHOAM_TMSCTL
zcfgRet_t zyExt8023ahObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ieeeJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_IEEE8023AH, &objIid, &ieeeJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(ieeeJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(ieeeJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExt8023ahObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *ieeeJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IEEE8023AH, &objIid, &ieeeJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = ieeeJobj;
		ieeeJobj = NULL;
		ieeeJobj = zcfgFeJsonMultiObjAppend(RDM_OID_IEEE8023AH, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(ieeeJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(ieeeJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_IEEE8023AH, &objIid, ieeeJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(ieeeJobj);
			return ret;
		}
		json_object_put(ieeeJobj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExt8023ahObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
		if(strcmp(tr181ParamName, paramList->name) == 0) {
			found = true;
			sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
			break;
		}
		paramList++;
	}

	if(found) {
		json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
	}

	return ZCFG_SUCCESS;
}

int zyExt8023ahObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  ieeeOid = 0;
	objIndex_t ieeeIid;
	int attrValue = ZCFG_NO_SUCH_PARAMETER;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(ieeeIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	ieeeOid = zcfgFeObjNameToObjId(tr181Obj, &ieeeIid);
	
	/*fill up tr98 devInfo object*/
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		
		/*special case*/

		attrValue = zcfgFeParamAttrGetByName(ieeeOid, paramList->name);
		if(attrValue < 0 ) {
			attrValue = 0;
		}
		break;
	}
	
	return attrValue;
}

zcfgRet_t zyExt8023ahObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char tr181Obj[128] = {0};
	char mapObj[128] = {0};
	uint32_t  ieeeOid = 0;
	objIndex_t ieeeIid;
	int attrValue = 0;
	tr98Parameter_t *paramList = NULL;

	zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
	if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
		return ZCFG_INVALID_OBJECT;
	}
	
	IID_INIT(ieeeIid);
	snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
	ieeeOid = zcfgFeObjNameToObjId(tr181Obj, &ieeeIid);

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*find the matched parameter*/
		if(strcmp(paramList->name, paramName)){
			paramList++;
			continue;
		}
		/*special case*/
		
		attrValue = zcfgFeParamAttrGetByName(ieeeOid, paramList->name);
		if(attrValue < 0) {
			ret = ZCFG_INVALID_ARGUMENTS;
			break;
		}

		/*Write new parameter attribute from tr98 object to tr181 objects*/
		attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
		ret = zcfgFeMultiParamAttrAppend(ieeeOid, multiAttrJobj, paramList->name, attrValue);
		if( ret != ZCFG_SUCCESS){
			zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, ieeeOid, paramList->name);
		}
		break;
	}
	
	return ret;
}
#endif

#ifdef ONECONNECT
/* InternetGatewayDevice.X_ZYXEL_EXT.OneConnect */
zcfgRet_t zyExtOneConnectObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *oneConnectJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_ONE_CONNECT, &objIid, &oneConnectJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(oneConnectJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(oneConnectJobj);
	
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.OneConnect */
zcfgRet_t zyExtOneConnectObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *oneConnectJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_ONE_CONNECT, &objIid, &oneConnectJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = oneConnectJobj;
		oneConnectJobj = NULL;
		oneConnectJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_ONE_CONNECT, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(oneConnectJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(oneConnectJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_ONE_CONNECT, &objIid, oneConnectJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(oneConnectJobj);
			return ret;
		}
		json_object_put(oneConnectJobj);
	}
	
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.SpeedTestInfo */
zcfgRet_t zyExtSpeedTestInfoObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *speedTestInfoJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_SPEED_TEST, &objIid, &speedTestInfoJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(speedTestInfoJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(speedTestInfoJobj);
	
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.SpeedTestInfo */
zcfgRet_t zyExtSpeedTestInfoObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *speedTestInfoJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_SPEED_TEST, &objIid, &speedTestInfoJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = speedTestInfoJobj;
		speedTestInfoJobj = NULL;
		speedTestInfoJobj = zcfgFeJsonMultiObjAppend(RDM_OID_SPEED_TEST, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(speedTestInfoJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(speedTestInfoJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_SPEED_TEST, &objIid, speedTestInfoJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(speedTestInfoJobj);
			return ret;
		}
		json_object_put(speedTestInfoJobj);
	}
	
	return ZCFG_SUCCESS;
}
#endif

#ifdef ZYXEL_AP_STEERING
zcfgRet_t zyExtApSteeringObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *apSteeringJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_WIFI_AP_STEERING, &objIid, &apSteeringJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(apSteeringJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(apSteeringJobj);
	
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.OneConnect */
zcfgRet_t zyExtApSteeringObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *apSteeringJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_AP_STEERING, &objIid, &apSteeringJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = apSteeringJobj;
		apSteeringJobj = NULL;
		apSteeringJobj = zcfgFeJsonMultiObjAppend(RDM_OID_WIFI_AP_STEERING, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(apSteeringJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(apSteeringJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_WIFI_AP_STEERING, &objIid, apSteeringJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(apSteeringJobj);
			return ret;
		}
		json_object_put(apSteeringJobj);
	}
	
	return ZCFG_SUCCESS;
}
#endif //#ifdef ZYXEL_AP_STEERING

#ifdef PROXIMITY_SENSOR_SUPPORT
zcfgRet_t zyExtProximitySensorObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *proximitySensorJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_PROXIMITY_SENSOR, &objIid, &proximitySensorJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(proximitySensorJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(proximitySensorJobj);
	
	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.OneConnect */
zcfgRet_t zyExtProximitySensorObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *proximitySensorJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_PROXIMITY_SENSOR, &objIid, &proximitySensorJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = proximitySensorJobj;
		proximitySensorJobj = NULL;
		proximitySensorJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_PROXIMITY_SENSOR, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(proximitySensorJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(proximitySensorJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_PROXIMITY_SENSOR, &objIid, proximitySensorJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(proximitySensorJobj);
			return ret;
		}
		json_object_put(proximitySensorJobj);
	}
	
	return ZCFG_SUCCESS;
}
#endif //#ifdef PROXIMITY_SENSOR_SUPPORT


/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_EXT.PrintServer
 *
 *   Related object in TR181: Device.X_ZYXEL_Ipp_Cfg
 *
 */
 #ifdef ZyXEL_IPP_PRINTSERVER
zcfgRet_t zyExtIppPrintServObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *printServJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_IPP_CFG, &objIid, &printServJobj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(printServJobj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}
	
	json_object_put(printServJobj);
	
	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtIppPrintServObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault){
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *printServJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_IPP_CFG, &objIid, &printServJobj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = printServJobj;
		printServJobj = NULL;
		printServJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_IPP_CFG, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(printServJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(printServJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_IPP_CFG, &objIid, printServJobj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(printServJobj);
			return ret;
		}
		json_object_put(printServJobj);
	}
	
	return ZCFG_SUCCESS;
}
#endif

#ifdef ZYXEL_LANDING_PAGE_FEATURE
/* "InternetGatewayDevice.X_ZYXEL_EXT.LandingPage" */
zcfgRet_t zyExtLandingPageObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *landingPageJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_LANDING_PAGE, &objIid, &landingPageJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(landingPageJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(landingPageJobj);
	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.LandingPage" */
zcfgRet_t zyExtLandingPageObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *landingPageJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_LANDING_PAGE, &objIid, &landingPageJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = landingPageJobj;
		landingPageJobj = NULL;
		landingPageJobj = zcfgFeJsonMultiObjAppend(RDM_OID_LANDING_PAGE, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(landingPageJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(landingPageJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_LANDING_PAGE, &objIid, landingPageJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(landingPageJobj);
			return ret;
		}
		json_object_put(landingPageJobj);
	}
	return ZCFG_SUCCESS;
}
#endif

/* "InternetGatewayDevice.X_ZYXEL_EXT.DNS" */
zcfgRet_t zyExtDnsObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *Jobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;
	const char *X_ZYXEL_BoundInterfaceList = NULL;
	int len = 0;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_DNS, &objIid, &Jobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(Jobj, paramList->name);
		if(paramValue != NULL) {
			if(!strcmp(paramList->name, "X_ZYXEL_BoundInterfaceList")) {
					X_ZYXEL_BoundInterfaceList = json_object_get_string(paramValue);
					
					if(X_ZYXEL_BoundInterfaceList) {
						char wanIpConnName[TR98_MAX_OBJ_NAME_LENGTH] = {0};
						char tr98IntfList[TR98_MAX_OBJ_NAME_LENGTH * 5] = {0};
						char interface_str[256] = {0};
						char *ipIfaceName = NULL, *tmp = NULL;
						
						sprintf(interface_str, "%s", X_ZYXEL_BoundInterfaceList);
						ipIfaceName = strtok_r(interface_str, ",", &tmp);
						while (ipIfaceName != NULL) {
							memset(wanIpConnName, 0, TR98_MAX_OBJ_NAME_LENGTH);
							
							if (zcfgFe181To98MappingNameGet(ipIfaceName, wanIpConnName) != ZCFG_SUCCESS) {
								ipIfaceName = strtok_r(NULL, "," ,&tmp);
								continue;
							}
							strcat(tr98IntfList, wanIpConnName);
							strcat(tr98IntfList, ",");
							ipIfaceName = strtok_r(NULL, ",", &tmp);
						}
						len = strlen(tr98IntfList);
						if(tr98IntfList[len-1] == ',')
							tr98IntfList[len-1] = '\0';
						json_object_object_add(*tr98Jobj, paramList->name, json_object_new_string(tr98IntfList));
						paramList++;
						continue;
					}
					
					json_object_object_add(*tr98Jobj, paramList->name, json_object_new_string(""));
					paramList++;
					continue;
			}
			
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(Jobj);

	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.DNS" */
zcfgRet_t zyExtDnsObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *Jobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;
	const char *X_ZYXEL_BoundInterfaceList = NULL;
	int len;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_DNS, &objIid, &Jobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = Jobj;
		Jobj = NULL;
		Jobj = zcfgFeJsonMultiObjAppend(RDM_OID_DNS, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			if(!strcmp(paramList->name, "X_ZYXEL_BoundInterfaceList")) {
				X_ZYXEL_BoundInterfaceList = json_object_get_string(paramValue);
				
				if(X_ZYXEL_BoundInterfaceList) {
					char tr181IntfList[256] = {0};
					char ipIfaceName[32] = {0};
					char interface_str[TR98_MAX_OBJ_NAME_LENGTH * 10] = {0};
					char *wanIpConnName = NULL, *tmp = NULL;
					
					sprintf(interface_str, "%s", X_ZYXEL_BoundInterfaceList);
					wanIpConnName = strtok_r(interface_str, ",", &tmp);
					while (wanIpConnName != NULL) {
						sprintf(wanIpConnName, "%s", wanIpConnName);
						memset(ipIfaceName, 0, 32);
						
						if (zcfgFe98To181MappingNameGet(wanIpConnName, ipIfaceName) != ZCFG_SUCCESS) {
							wanIpConnName = strtok_r(NULL, "," ,&tmp);
							continue;
						}
						if(strstr(ipIfaceName, "PPP.Interface.")){
							zcfgFeTr181IfaceStackHigherLayerGet(ipIfaceName, ipIfaceName);
						}
						strcat(tr181IntfList, ipIfaceName);
						strcat(tr181IntfList, ",");
						wanIpConnName = strtok_r(NULL, ",", &tmp);
					}
					len = strlen(tr181IntfList);
					if(tr181IntfList[len-1] == ',')
						tr181IntfList[len-1] = '\0';
					json_object_object_add(Jobj, paramList->name, json_object_new_string(tr181IntfList));
					paramList++;
					continue;
				}
				
				json_object_object_add(Jobj, paramList->name, json_object_new_string(""));
				paramList++;
				continue;
			}
			
			tr181ParamValue = json_object_object_get(Jobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_DNS, &objIid, Jobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(Jobj);
			return ret;
		}
		json_object_put(Jobj);
	}

	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.FeatureFlag" */
zcfgRet_t zyExtFeatureFlagObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *featureFlagJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_FEATURE_FLAG, &objIid, &featureFlagJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(featureFlagJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(featureFlagJobj);

	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.FeatureFlag" */
zcfgRet_t zyExtFeatureFlagObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *featureFlagJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_FEATURE_FLAG, &objIid, &featureFlagJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = featureFlagJobj;
		featureFlagJobj = NULL;
		featureFlagJobj = zcfgFeJsonMultiObjAppend(RDM_OID_FEATURE_FLAG, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(featureFlagJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(featureFlagJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_FEATURE_FLAG, &objIid, featureFlagJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(featureFlagJobj);
			return ret;
		}
		json_object_put(featureFlagJobj);
	}

	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.GUI_CUSTOMIZATION" */
zcfgRet_t zyExtCustomizationObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *customizationJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_GUI_CUSTOMIZATION, &objIid, &customizationJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(customizationJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}
	json_object_put(customizationJobj);
	return ZCFG_SUCCESS;
}

/* "InternetGatewayDevice.X_ZYXEL_EXT.GUI_CUSTOMIZATION" */
zcfgRet_t zyExtCustomizationObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *customizationJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_GUI_CUSTOMIZATION, &objIid, &customizationJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = customizationJobj;
		customizationJobj = NULL;
		customizationJobj = zcfgFeJsonMultiObjAppend(RDM_OID_GUI_CUSTOMIZATION, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(customizationJobj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(customizationJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_GUI_CUSTOMIZATION, &objIid, customizationJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(customizationJobj);
			return ret;
		}
		json_object_put(customizationJobj);
	}
	return ZCFG_SUCCESS;
}

#ifdef ZYXEL_EASYMESH
/* InternetGatewayDevice.X_ZYXEL_EXT.EasyMesh */
zcfgRet_t zyExtEasyMeshObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *EasyMeshObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_EASY_MESH, &objIid, &EasyMeshObj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(EasyMeshObj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(EasyMeshObj);

	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.EasyMesh */
zcfgRet_t zyExtEasyMeshObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *EasyMeshObj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_EASY_MESH, &objIid, &EasyMeshObj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = EasyMeshObj;
		EasyMeshObj = NULL;
		EasyMeshObj = zcfgFeJsonMultiObjAppend(RDM_OID_EASY_MESH, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(EasyMeshObj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(EasyMeshObj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_EASY_MESH, &objIid, EasyMeshObj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(EasyMeshObj);
			return ret;
		}
		json_object_put(EasyMeshObj);
	}

	return ZCFG_SUCCESS;
}
#endif

#ifdef BUILD_SONIQ
zcfgRet_t zyExtQsteerObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *SoniqObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_WIFI_QSTEER, &objIid, &SoniqObj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(SoniqObj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(SoniqObj);

	return ZCFG_SUCCESS;
}

/* InternetGatewayDevice.X_ZYXEL_EXT.OneConnect */
zcfgRet_t zyExtQsteerObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *SoniqObj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_QSTEER, &objIid, &SoniqObj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = SoniqObj;
		SoniqObj = NULL;
		SoniqObj = zcfgFeJsonMultiObjAppend(RDM_OID_WIFI_QSTEER, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(SoniqObj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(SoniqObj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_WIFI_QSTEER, &objIid, SoniqObj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(SoniqObj);
			return ret;
		}
		json_object_put(SoniqObj);
	}

	return ZCFG_SUCCESS;
}
#endif //#ifdef BUILD_SONIQ

#ifdef ZYXEL_ONESSID
/* InternetGatewayDevice.X_ZYXEL_EXT.X_ZYXEL_ONESSID */
zcfgRet_t zyExtOneSsidObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *OneSsidObj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_WIFI_ONE_SSID, &objIid, &OneSsidObj, updateFlag)) != ZCFG_SUCCESS){
		return ret;
	}

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write parameter value from tr181 objects to tr98 object*/
		paramValue = json_object_object_get(OneSsidObj, paramList->name);
		if(paramValue != NULL){
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;
	}

	json_object_put(OneSsidObj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtOneSsidObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *OneSsidObj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_ONE_SSID, &objIid, &OneSsidObj)) != ZCFG_SUCCESS){
		return ret;
	}

	if(multiJobj){
		tmpObj = OneSsidObj;
		OneSsidObj = NULL;
		OneSsidObj = zcfgFeJsonMultiObjAppend(RDM_OID_WIFI_ONE_SSID, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL){
		/*Write new parameter value from tr98 object to tr181 objects*/
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL){
			tr181ParamValue = json_object_object_get(OneSsidObj, paramList->name);
			if(tr181ParamValue != NULL){
				json_object_object_add(OneSsidObj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_WIFI_ONE_SSID, &objIid, OneSsidObj, NULL)) != ZCFG_SUCCESS ){
			json_object_put(OneSsidObj);
			return ret;
		}
		json_object_put(OneSsidObj);
	}

	return ZCFG_SUCCESS;
}
#endif //#ifdef ZYXEL_ONESSID

#ifdef ZYXEL_TR69_DATA_USAGE
/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_DataUsage
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_DataUsage
 *   RDM_OID_ZY_DATA_USAGE
 */

zcfgRet_t zyExtDataUsageObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dataUsageJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = feObjJsonGet(RDM_OID_ZY_DATA_USAGE, &objIid, &dataUsageJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;	

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(dataUsageJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(dataUsageJobj);

	return ZCFG_SUCCESS;
}

zcfgRet_t zyExtDataUsageObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dataUsageJobj = NULL;
	struct json_object *tmpObj = NULL;
	struct json_object *paramValue = NULL;
	struct json_object *tr181ParamValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_DATA_USAGE, &objIid, &dataUsageJobj)) != ZCFG_SUCCESS)
		return ret;

	if(multiJobj){
		tmpObj = dataUsageJobj;
		dataUsageJobj = NULL;
		dataUsageJobj = zcfgFeJsonMultiObjAppend(RDM_OID_ZY_DATA_USAGE, &objIid, multiJobj, tmpObj);
	}

	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write new parameter value from tr98 object to tr181 objects*/	
		paramValue = json_object_object_get(tr98Jobj, paramList->name);
		if(paramValue != NULL) {
			tr181ParamValue = json_object_object_get(dataUsageJobj, paramList->name);
			if(tr181ParamValue != NULL) {
				json_object_object_add(dataUsageJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
		if((ret = zcfgFeObjJsonSet(RDM_OID_ZY_DATA_USAGE, &objIid, dataUsageJobj, NULL)) != ZCFG_SUCCESS ) {
			json_object_put(dataUsageJobj);
			return ret;
		}
		json_object_put(dataUsageJobj);
	}

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_DataUsage.Lan.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_DataUsage.Lan.i
 *   RDM_OID_ZY_DATA_USAGE_LAN
 */
zcfgRet_t zyExtDataUsageLanObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dataUsageLanJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_DataUsage.Lan.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_ZY_DATA_USAGE_LAN, &objIid, &dataUsageLanJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(dataUsageLanJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(dataUsageLanJobj);

	return ZCFG_SUCCESS;
}

/*
 *   TR98 Object Name : InternetGatewayDevice.X_ZYXEL_DataUsage.Wan.i
 *
 *   Related object in TR181:
 *   Device.X_ZYXEL_DataUsage.Wan.i
 *   RDM_OID_ZY_DATA_USAGE_WAN
 */
zcfgRet_t zyExtDataUsageWanObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
	zcfgRet_t ret;
	objIndex_t objIid;
	struct json_object *dataUsageWanJobj = NULL;
	struct json_object *paramValue = NULL;
	tr98Parameter_t *paramList = NULL;

	printf("%s : Enter\n", __FUNCTION__);

	/*Get related tr181 objects*/
	IID_INIT(objIid);
	objIid.level = 1;
	sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_DataUsage.Wan.%hhu", &objIid.idx[0]);

	if((ret = feObjJsonGet(RDM_OID_ZY_DATA_USAGE_WAN, &objIid, &dataUsageWanJobj, updateFlag)) != ZCFG_SUCCESS)
		return ret;

	/*fill up tr98 object from related tr181 objects*/
	*tr98Jobj = json_object_new_object();
	paramList = tr98Obj[handler].parameter;
	while(paramList->name != NULL) {
		/*Write parameter value from tr181 objects to tr98 object*/		
		paramValue = json_object_object_get(dataUsageWanJobj, paramList->name);
		if(paramValue != NULL) {
			json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
			paramList++;
			continue;
		}

		/*Not defined in tr181, give it a default value*/
		printf("Can't find parameter %s in TR181\n", paramList->name);
		paramList++;	
	}

	json_object_put(dataUsageWanJobj);

	return ZCFG_SUCCESS;
}
#endif

#if 1 // IPPT mode remote access
zcfgRet_t zyExtIPPTSrvObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtSrvJobj = NULL;
        struct json_object *paramValue = NULL;
        tr98Parameter_t *paramList = NULL;

        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.Service.%hhu", &objIid.idx[0]);

        if((ret = feObjJsonGet(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, &RemoteMgmtSrvJobj, updateFlag)) != ZCFG_SUCCESS)
                return ret;

        /*fill up tr98 object from related tr181 objects*/
        *tr98Jobj = json_object_new_object();
        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write parameter value from tr181 objects to tr98 object*/
                paramValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
                if(paramValue != NULL) {
                        json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
                        paramList++;
                        continue;
                }

                /*Not defined in tr181, give it a default value*/
                printf("Can't find parameter %s in TR181\n", paramList->name);
                paramList++;
        }

        json_object_put(RemoteMgmtSrvJobj);

        return ZCFG_SUCCESS;
}
zcfgRet_t zyExtIPPTSrvObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtSrvJobj = NULL;
        struct json_object *tmpObj = NULL;
        struct json_object *paramValue = NULL;
        struct json_object *tr181ParamValue = NULL;
        tr98Parameter_t *paramList = NULL;
        bool isLanWan = false,isHttpService = false;
        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.Service.%hhu", &objIid.idx[0]);

        if((ret = zcfgFeObjJsonGet(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, &RemoteMgmtSrvJobj)) != ZCFG_SUCCESS)
                return ret;

        if(multiJobj){
                tmpObj = RemoteMgmtSrvJobj;
                RemoteMgmtSrvJobj = NULL;
                RemoteMgmtSrvJobj = zcfgFeJsonMultiObjAppend(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, multiJobj, tmpObj);
        }

        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write new parameter value from tr98 object to tr181 objects*/
                paramValue = json_object_object_get(tr98Jobj, paramList->name);
                if(paramValue != NULL) {
                        tr181ParamValue = json_object_object_get(RemoteMgmtSrvJobj, paramList->name);
                        if(tr181ParamValue != NULL) {
                                json_object_object_add(RemoteMgmtSrvJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
                if((ret = zcfgFeObjJsonSet(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, RemoteMgmtSrvJobj, NULL)) != ZCFG_SUCCESS ) {
                        json_object_put(RemoteMgmtSrvJobj);
                        return ret;
                }
                json_object_put(RemoteMgmtSrvJobj);
        }
        return ZCFG_SUCCESS;
}

zcfgRet_t zyExtIPPTSrvObjAdd(char *tr98FullPathName, int *idx)
{
        zcfgRet_t ret;
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        ret = zcfgFeObjStructAdd(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, NULL);
        *idx = objIid.idx[0];

        return ret;
}

zcfgRet_t zyExtIPPTSrvObjDel(char *tr98FullPathName)
{
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.Service.%hhu", &objIid.idx[0]);

        return zcfgFeObjStructDel(RDM_OID_REMO_SRV_IP_PASS_THRU, &objIid, NULL);;
}

zcfgRet_t zyExtIPPTSrvObjNotify(char *tr98ObjName, char *tr181ParamName, struct json_object *tr181ParamVal, int handler, struct json_object **tr98NotifyInfo)
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
                if(strcmp(tr181ParamName, paramList->name) == 0) {
                        found = true;
                        sprintf(tr98Notify, "%s.%s", tr98ObjName, paramList->name);
                        break;
                }
                paramList++;
        }

        if(found) {
                json_object_object_add(*tr98NotifyInfo, tr98Notify, JSON_OBJ_COPY(tr181ParamVal));
        }

        return ZCFG_SUCCESS;
}
int zyExtIPPTSrvObjAttrGet(char *tr98FullPathName, int handler, char *paramName)
{
        char tr181Obj[128] = {0};
        char mapObj[128] = {0};
        uint32_t  devInfoOid = 0;
        objIndex_t devInfoIid;
        int attrValue = ZCFG_NO_SUCH_PARAMETER;
        tr98Parameter_t *paramList = NULL;

        zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);

        if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
                return ZCFG_INVALID_OBJECT;
        }

        IID_INIT(devInfoIid);
        snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
        devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

        /*fill up tr98 devInfo object*/
        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                if(strcmp(paramList->name, paramName)){
                        paramList++;
                        continue;
                }

                attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
                if(attrValue < 0 ) {
                        attrValue = 0;
                }
                break;
        }

        return attrValue;
}
zcfgRet_t zyExtIPPTSrvObjAttrSet(char *tr98FullPathName, int handler, char *paramName, int newNotify, struct json_object *multiAttrJobj)
{
        zcfgRet_t ret = ZCFG_SUCCESS;
        char tr181Obj[128] = {0};
        char mapObj[128] = {0};
        uint32_t  devInfoOid = 0;
        objIndex_t devInfoIid;
        int attrValue = 0;
        tr98Parameter_t *paramList = NULL;

        zcfgLog(ZCFG_LOG_DEBUG, "Enter %s\n", __FUNCTION__);
        if(zcfgFe98To181MappingNameGet(tr98FullPathName, mapObj) != ZCFG_SUCCESS) {
                return ZCFG_INVALID_OBJECT;
        }

        IID_INIT(devInfoIid);
        snprintf(tr181Obj, sizeof(tr181Obj),"Device.%s", mapObj);
        devInfoOid = zcfgFeObjNameToObjId(tr181Obj, &devInfoIid);

        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*find the matched parameter*/
                if(strcmp(paramList->name, paramName)){
                        paramList++;
                        continue;
                }

                attrValue = zcfgFeParamAttrGetByName(devInfoOid, paramList->name);
                if(attrValue < 0) {
                        ret = ZCFG_INVALID_ARGUMENTS;
                        break;
                }

                /*Write new parameter attribute from tr98 object to tr181 objects*/
                attrValue = zcfgFeNotifyAttrValSet(attrValue, newNotify);
                ret = zcfgFeMultiParamAttrAppend(devInfoOid, multiAttrJobj, paramList->name, attrValue);
                if( ret != ZCFG_SUCCESS){
                        zcfgLog(ZCFG_LOG_ERR, "%s(): set %d %s attribute fail\n", __FUNCTION__, devInfoOid, paramList->name);
                }
                break;

        } /*Edn while*/

        return ret;
}

zcfgRet_t zyExtIPPTTrustDomainObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtTrustDomainJobj = NULL;
        struct json_object *paramValue = NULL;
        tr98Parameter_t *paramList = NULL;

        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.TrustDomain.%hhu", &objIid.idx[0]);

        if((ret = feObjJsonGet(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, &RemoteMgmtTrustDomainJobj, updateFlag)) != ZCFG_SUCCESS)
                return ret;

        /*fill up tr98 object from related tr181 objects*/
        *tr98Jobj = json_object_new_object();
        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write parameter value from tr181 objects to tr98 object*/
                paramValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
                if(paramValue != NULL) {
                        json_object_object_add(*tr98Jobj, paramList->name, JSON_OBJ_COPY(paramValue));
                        paramList++;
                        continue;
                }

                /*Not defined in tr181, give it a default value*/
                printf("Can't find parameter %s in TR181\n", paramList->name);
                paramList++;
        }

        json_object_put(RemoteMgmtTrustDomainJobj);

        return ZCFG_SUCCESS;
}
zcfgRet_t zyExtIPPTTrustDomainObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj, char *paramfault)
{
        zcfgRet_t ret;
        objIndex_t objIid;
        struct json_object *RemoteMgmtTrustDomainJobj = NULL;
        struct json_object *tmpObj = NULL;
        struct json_object *paramValue = NULL;
        struct json_object *tr181ParamValue = NULL;
        tr98Parameter_t *paramList = NULL;

        printf("%s : Enter\n", __FUNCTION__);

        /*Get related tr181 objects*/
        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.TrustDomain.%hhu", &objIid.idx[0]);

        if((ret = zcfgFeObjJsonGet(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, &RemoteMgmtTrustDomainJobj)) != ZCFG_SUCCESS)
                return ret;

        if(multiJobj){
                tmpObj = RemoteMgmtTrustDomainJobj;
                RemoteMgmtTrustDomainJobj = NULL;
                RemoteMgmtTrustDomainJobj = zcfgFeJsonMultiObjAppend(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, multiJobj, tmpObj);
        }

        paramList = tr98Obj[handler].parameter;
        while(paramList->name != NULL) {
                /*Write new parameter value from tr98 object to tr181 objects*/
                paramValue = json_object_object_get(tr98Jobj, paramList->name);
                if(paramValue != NULL) {
                        tr181ParamValue = json_object_object_get(RemoteMgmtTrustDomainJobj, paramList->name);
                        if(tr181ParamValue != NULL) {
                                json_object_object_add(RemoteMgmtTrustDomainJobj, paramList->name, JSON_OBJ_COPY(paramValue));
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
                if((ret = zcfgFeObjJsonSet(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, RemoteMgmtTrustDomainJobj, NULL)) != ZCFG_SUCCESS ) {
                        json_object_put(RemoteMgmtTrustDomainJobj);
                        return ret;
                }
                json_object_put(RemoteMgmtTrustDomainJobj);
        }

        return ZCFG_SUCCESS;
}

zcfgRet_t zyExtIPPTTrustDomainObjAdd(char *tr98FullPathName, int *idx)
{
        zcfgRet_t ret;
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        ret = zcfgFeObjStructAdd(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, NULL);
        *idx = objIid.idx[0];
        return ret;
}

zcfgRet_t zyExtIPPTTrustDomainObjDel(char *tr98FullPathName)
{
        objIndex_t objIid;

        printf("%s : Enter\n", __FUNCTION__);

        IID_INIT(objIid);
        objIid.level = 1;
        sscanf(tr98FullPathName, "InternetGatewayDevice.X_ZYXEL_EXT.RemoteManagement_IP_PassThrough.TrustDomain.%hhu", &objIid.idx[0]);

        return zcfgFeObjStructDel(RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU, &objIid, NULL);;
}
#endif
