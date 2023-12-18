#include <ctype.h>
#include <json/json.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"

#define CWMP_SOAP_MSG_DEBUG (1 << 1)

dal_param_t CELLWAN_PSRU_param[]={
#ifdef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP // Jessie Lu@2020.08.12: for multiple APNs with IP pass through mode
{"Index", dalType_int, 0, 0, NULL},
#endif
{"Passthru_Enable", dalType_boolean, 0, 0, NULL},
{"Passthru_Mode", dalType_string, 0, "Dynamic/Fixed", NULL},
{"Passthru_MacAddr", dalType_string, 0, 0, NULL},
{"Passthru_SubnetPrefix", dalType_int, 0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}

};

zcfgRet_t zcfgFeDalCellWanPsruEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *intfZyIpPassthruObj = NULL;
	objIndex_t intfZyIpPassthruIid = {0};
	bool passthruEnable = false;
	const char *passthruMode = NULL;
	const char *passthruMacAddr = NULL;
    int passthruSubnetPrefix = 0;
	bool passthruStaticGwEn = false;
	const char *StaticGwIPAddress = NULL;

	IID_INIT(intfZyIpPassthruIid);
	intfZyIpPassthruIid.level=1;
#ifdef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP // Jessie Lu@2020.08.12: for multiple APNs with IP pass through mode
	{
		int idx = json_object_get_int(json_object_object_get(Jobj, "Index"));
		printf("[%s][%d] idx(%d)\n", __FUNCTION__, __LINE__, idx);
		if (idx > 0) {
			intfZyIpPassthruIid.idx[0] = idx;
		} else {
			intfZyIpPassthruIid.idx[0] = 1;
		}
	}
#else
	intfZyIpPassthruIid.idx[0]=1;
#endif

	passthruEnable = json_object_get_boolean(json_object_object_get(Jobj, "Passthru_Enable"));
	passthruMode = json_object_get_string(json_object_object_get(Jobj, "Passthru_Mode"));
	passthruMacAddr = json_object_get_string(json_object_object_get(Jobj, "Passthru_MacAddr"));
    passthruSubnetPrefix = json_object_get_string(json_object_object_get(Jobj, "Passthru_SubnetPrefix"));
	passthruStaticGwEn = json_object_get_boolean(json_object_object_get(Jobj, "Passthru_StaticGWIPEnable"));
	StaticGwIPAddress = json_object_get_string(json_object_object_get(Jobj, "Passthru_StaticGWIP"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, &intfZyIpPassthruObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "Passthru_Enable"))
			json_object_object_add(intfZyIpPassthruObj, "Enable", json_object_new_boolean(passthruEnable));
		if(json_object_object_get(Jobj, "Passthru_Mode"))
			json_object_object_add(intfZyIpPassthruObj, "ConnectionMode", json_object_new_string(passthruMode));
		if(json_object_object_get(Jobj, "Passthru_MacAddr"))
			json_object_object_add(intfZyIpPassthruObj, "MACAddress", json_object_new_string(passthruMacAddr));
		if(json_object_object_get(Jobj, "Passthru_SubnetPrefix"))
			json_object_object_add(intfZyIpPassthruObj, "ProxyARP_SubnetPrefix", json_object_new_int(passthruSubnetPrefix));
		if(json_object_object_get(Jobj, "Passthru_StaticGWIPEnable"))
		    json_object_object_add(intfZyIpPassthruObj, "StaticGwEn", json_object_new_boolean(passthruStaticGwEn));
		if(json_object_object_get(Jobj, "Passthru_StaticGWIP"))
		    json_object_object_add(intfZyIpPassthruObj, "StaticGwIPAddress", json_object_new_string(StaticGwIPAddress));
		zcfgFeObjJsonSet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, intfZyIpPassthruObj, NULL);
	} else {
		// if Get fail, Add a new one.
		printf("[%s][%d] Data model Get fail\n", __FUNCTION__, __LINE__);
	}

	zcfgFeJsonObjFree(intfZyIpPassthruObj);

	return ret;
}

zcfgRet_t zcfgFeDalCellWanPsruGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *intfZyIpPassthruObj = NULL;
	objIndex_t intfZyIpPassthruIid = {0};
#ifndef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP
	paramJobj = json_object_new_object();
#endif

	IID_INIT(intfZyIpPassthruIid);
#ifdef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, &intfZyIpPassthruObj) == ZCFG_SUCCESS) {
		paramJobj = json_object_new_object();

		json_object_object_add(paramJobj, "idx", json_object_new_int(intfZyIpPassthruIid.idx[0]));
		json_object_object_add(paramJobj, "Passthru_Enable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "Enable")));
		json_object_object_add(paramJobj, "Passthru_Mode", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ConnectionMode")));
		json_object_object_add(paramJobj, "Passthru_MacAddr", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "MACAddress")));
		json_object_object_add(paramJobj, "Passthru_SubnetPrefix", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ProxyARP_SubnetPrefix")));
		json_object_object_add(paramJobj, "Passthru_StaticGWIPEnable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwEn")));
		json_object_object_add(paramJobj, "Passthru_StaticGWIP", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwIPAddress")));

		json_object_array_add(Jarray, paramJobj);
		zcfgFeJsonObjFree(intfZyIpPassthruObj);
	}
#else
	intfZyIpPassthruIid.level=1;
	intfZyIpPassthruIid.idx[0]=1; 

	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, &intfZyIpPassthruObj) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "Passthru_Enable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "Enable")));
		json_object_object_add(paramJobj, "Passthru_Mode", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ConnectionMode")));
		json_object_object_add(paramJobj, "Passthru_MacAddr", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "MACAddress")));
		json_object_object_add(paramJobj, "Passthru_SubnetPrefix", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ProxyARP_SubnetPrefix")));
		json_object_object_add(paramJobj, "Passthru_StaticGWIPEnable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwEn")));
		json_object_object_add(paramJobj, "Passthru_StaticGWIP", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwIPAddress")));

		zcfgFeJsonObjFree(intfZyIpPassthruObj);
	}

	json_object_array_add(Jarray, paramJobj);
#endif

	return ret;
}

void zcfgFeDalShowCellWanPsru(struct json_object *Jarray){
	struct json_object *obj = NULL;
#ifdef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP
	int i, len = 0;
	objIndex_t cellIntfIid;
	struct json_object *cellIntfObj = NULL;
#endif

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

#ifdef ZYXEL_CELLWAN_WITH_VLAN_INTF_GROUP
		printf("%-6s %-12s %-16s %-20s %-40s %-14s\n", "Index", "Name", "Enable/Disable", "Connection Mode", "Passthrough to fixed MAC", "Subnet Prefix");
		printf("%-6s %-12s %-16s %-20s %-40s %-14s\n", "=====", "====", "==============", "===============", "========================", "=============");

		len = json_object_array_length(Jarray);
		for(i=0; i<len; i++) {
			char enable[8] = {0};
			char *cellName = NULL;

			IID_INIT(cellIntfIid);
			cellIntfIid.level=1;
			cellIntfIid.idx[0] = i+1;
			if (zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &cellIntfIid, &cellIntfObj) == ZCFG_SUCCESS) {
				cellName = json_object_get_string(json_object_object_get(cellIntfObj, "Name"));
			}

			obj = json_object_array_get_idx(Jarray, i);
			if (json_object_get_boolean(json_object_object_get(obj, "Passthru_Enable")) == true) {
				sprintf(enable, "Enabled");
			} else {
				sprintf(enable, "Disabled");
			}
			printf("%-6d %-12s %-16s %-20s %-40s %-14d\n", i+1, cellName, enable,
				json_object_get_string(json_object_object_get(obj, "Passthru_Mode")),
				json_object_get_string(json_object_object_get(obj, "Passthru_MacAddr")),
				json_object_get_int(json_object_object_get(obj, "Passthru_SubnetPrefix")) );
			if (cellName) zcfgFeJsonObjFree(cellIntfObj);
		}
#else
	obj = json_object_array_get_idx(Jarray, 0);

	printf("\nCellular IP Passthrough:\n");

	if(json_object_get_boolean(json_object_object_get(obj, "Passthru_Enable")))
		printf("IP Passthrough Enable.\n");
	else
		printf("IP Passthrough Disable.\n");

	if(!strcmp("",json_object_get_string(json_object_object_get(obj, "Passthru_Mode"))))
		printf("%-30s %-10s \n","Connection Mode:", "N/A");
	else if(!strcmp("Fixed",json_object_get_string(json_object_object_get(obj, "Passthru_Mode")))){
		printf("%-30s %-10s \n","Connection Mode:", json_object_get_string(json_object_object_get(obj, "Passthru_Mode")));
		printf("%-30s %-10s \n","Passthrough to fixed MAC:", json_object_get_string(json_object_object_get(obj, "Passthru_MacAddr")));
	}else
		printf("%-30s %-10s \n","Connection Mode:", json_object_get_string(json_object_object_get(obj, "Passthru_Mode")));

    printf("%-30s %-10d \n", "Subnet mask prefix:", json_object_get_int(json_object_object_get(obj,"Passthru_SubnetPrefix")));

#endif
}


zcfgRet_t zcfgFeDalCellWanPsru(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanPsruEdit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanPsruGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

