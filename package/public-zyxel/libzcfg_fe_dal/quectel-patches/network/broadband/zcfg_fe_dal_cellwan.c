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

dal_param_t CELLWAN_param[]={
{"CELL_Roaming_Enable", dalType_boolean, 0, 0, NULL},
#ifdef PACKAGE_lte_srv_diag	
{"CELL_Network_Monitor_Enable", dalType_boolean, 0, 0, NULL},//PACKAGE_lte_srv_diag
#endif
#if 1 // proxy arp
{"ProxyARP_Enable", dalType_boolean, 0, 0, NULL}, // ProxyARP feature
#endif
#ifdef ZYXEL_FQ_CODEL
{"CELL_FQ_CODEL_Enable",dalType_boolean, 0, 0, NULL},	
#endif
#ifndef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
{"AP_ManualAPN", dalType_boolean, 0, 0, NULL},
{"AP_APN", dalType_string, 0, 0, NULL},
{"AP_Username", dalType_string, 0, 0, NULL},
{"AP_Password", dalType_string, 0, 0, NULL},
{"AP_Auth_Type", dalType_string, 0, "None/PAP/CHAP/Auto", NULL},
{"AP_PDP_Type", dalType_string, 0, "IPv4/IPv6/IPv4v6", NULL},
#endif
{"CELL_Disable_UMTS", dalType_boolean,0,"1 for disable 3G",NULL},
{ NULL,	0,	0,	0,	NULL}

};

zcfgRet_t setCellObj(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *cellObj = NULL;
	objIndex_t cellIid = {0};
	IID_INIT(cellIid);
	bool enable = false;
	bool u_enable = false;
	enable = json_object_get_boolean(json_object_object_get(Jobj, "CELL_Roaming_Enable"));

	u_enable = json_object_get_boolean(json_object_object_get(Jobj, "CELL_Disable_UMTS"));
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "CELL_Roaming_Enable"))
			json_object_object_add(cellObj, "RoamingEnabled", json_object_new_boolean(enable));
		if(json_object_object_get(Jobj, "CELL_Disable_UMTS"))
			json_object_object_add(cellObj, "X_ZYXEL_DisableUMTS", json_object_new_boolean(u_enable));
		zcfgFeObjJsonSet(RDM_OID_CELL, &cellIid, cellObj, NULL);
	}
#ifdef PACKAGE_lte_srv_diag
	enable = json_object_get_boolean(json_object_object_get(Jobj, "CELL_Network_Monitor_Enable"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "CELL_Network_Monitor_Enable"))
			json_object_object_add(cellObj, "NetworkMonitorEnabled", json_object_new_boolean(enable));
		zcfgFeObjJsonSet(RDM_OID_CELL, &cellIid, cellObj, NULL);
	}
#endif

#if 1 // proxy arp
    // YuChih@20201119: ProxyARP feature
	enable = json_object_get_boolean(json_object_object_get(Jobj, "ProxyARP_Enable"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "ProxyARP_Enable"))
			json_object_object_add(cellObj, "X_ZYXEL_ProxyARP_Enabled", json_object_new_boolean(enable));
		zcfgFeObjJsonSet(RDM_OID_CELL, &cellIid, cellObj, NULL);
	}
#endif

#ifdef ZYXEL_FQ_CODEL//Aaron	
	unsigned int fq_codel_upload = 0;
	const char *modeType = NULL;
	const char *PingServerAdr = NULL;
	unsigned int fq_baseline = 0;
	unsigned int fq_threshold = 0;
	unsigned int fq_limit = 0;

	enable = json_object_get_boolean(json_object_object_get(Jobj, "CELL_FQ_CODEL_Enable"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "CELL_FQ_CODEL_Enable"))
			json_object_object_add(cellObj, "X_ZYXEL_FqCodelEnabled", json_object_new_boolean(enable));
		zcfgFeObjJsonSet(RDM_OID_CELL, &cellIid, cellObj, NULL);		
	}	
#endif

	zcfgFeJsonObjFree(cellObj);

	return ret;
}
#ifndef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
zcfgRet_t setAccessPointObj(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *accessPointObj = NULL;
	objIndex_t accessPointIid = {0};
	bool manualAPN = false;
	const char *apn = NULL;
	const char *username = NULL;
	const char *password = NULL;
	const char *authType = NULL;
	const char *pdpType = NULL;

	IID_INIT(accessPointIid);
	accessPointIid.level=1;
	accessPointIid.idx[0]=1; 

	manualAPN = json_object_get_boolean(json_object_object_get(Jobj, "AP_ManualAPN"));
	apn = json_object_get_string(json_object_object_get(Jobj, "AP_APN"));
	username = json_object_get_string(json_object_object_get(Jobj, "AP_Username"));
	password = json_object_get_string(json_object_object_get(Jobj, "AP_Password"));
	authType = json_object_get_string(json_object_object_get(Jobj, "AP_Auth_Type"));
	pdpType = json_object_get_string(json_object_object_get(Jobj, "AP_PDP_Type"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_ACCESS_POINT, &accessPointIid, &accessPointObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "AP_ManualAPN"))
			json_object_object_add(accessPointObj, "X_ZYXEL_ManualAPN", json_object_new_boolean(manualAPN));
		if(json_object_object_get(Jobj, "AP_APN"))
			json_object_object_add(accessPointObj, "APN", json_object_new_string(apn));
		if(json_object_object_get(Jobj, "AP_Username"))
			json_object_object_add(accessPointObj, "Username", json_object_new_string(username));
		if(json_object_object_get(Jobj, "AP_Password"))
			json_object_object_add(accessPointObj, "Password", json_object_new_string(password));
		if(json_object_object_get(Jobj, "AP_Auth_Type"))
			json_object_object_add(accessPointObj, "X_ZYXEL_AuthenticationType", json_object_new_string(authType));
		if(json_object_object_get(Jobj, "AP_PDP_Type"))
			json_object_object_add(accessPointObj, "X_ZYXEL_PDP_Type", json_object_new_string(pdpType));
		zcfgFeObjJsonSet(RDM_OID_CELL_ACCESS_POINT, &accessPointIid, accessPointObj, NULL);
	}

	zcfgFeJsonObjFree(accessPointObj);

	return ret;
}
#endif
zcfgRet_t zcfgFeDalCellWanEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if((ret = setCellObj(Jobj)) != ZCFG_SUCCESS){
		printf("Set Cell Obj Fail!\n");
	}

#ifndef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
	if((ret = setAccessPointObj(Jobj)) != ZCFG_SUCCESS){
		printf("Set Access Point Obj Fail!\n");
	}
#endif

	return ret;	
}	

zcfgRet_t zcfgFeDalCellWanGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *cellObj = NULL, *accessPointObj = NULL;
	objIndex_t cellIid = {0}, accessPointIid = {0};
	paramJobj = json_object_new_object();

	IID_INIT(cellIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "CELL_Roaming_Enable", JSON_OBJ_COPY(json_object_object_get(cellObj, "RoamingEnabled")));
		json_object_object_add(paramJobj, "CELL_Disable_UMTS", JSON_OBJ_COPY(json_object_object_get(cellObj, "X_ZYXEL_DisableUMTS")));
		zcfgFeJsonObjFree(cellObj);
	}

#ifdef PACKAGE_lte_srv_diag
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "CELL_Network_Monitor_Enable", JSON_OBJ_COPY(json_object_object_get(cellObj, "NetworkMonitorEnabled")));
		zcfgFeJsonObjFree(cellObj);
	}
#endif

#if 1 // proxy arp
    // YuChih@20201119: ProxyARP feature
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "ProxyARP_Enable", JSON_OBJ_COPY(json_object_object_get(cellObj, "X_ZYXEL_ProxyARP_Enabled")));
		zcfgFeJsonObjFree(cellObj);
	}
#endif

#ifdef ZYXEL_FQ_CODEL//Aaron	
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL, &cellIid, &cellObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "CELL_FQ_CODEL_Enable", JSON_OBJ_COPY(json_object_object_get(cellObj, "X_ZYXEL_FqCodelEnabled")));	
		zcfgFeJsonObjFree(cellObj);
	}	
#endif
#ifndef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
	IID_INIT(accessPointIid);
	accessPointIid.level=1;
	accessPointIid.idx[0]=1;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_ACCESS_POINT, &accessPointIid, &accessPointObj)) == ZCFG_SUCCESS){
		/* json_object_object_add(paramJobj, "AP_Enable", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Enable"))); */
		json_object_object_add(paramJobj, "AP_APN", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "APN")));
		json_object_object_add(paramJobj, "AP_Username", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Username")));
		json_object_object_add(paramJobj, "AP_Password", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Password")));
		json_object_object_add(paramJobj, "AP_ManualAPN", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_ManualAPN")));
		json_object_object_add(paramJobj, "AP_Auth_Type", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_AuthenticationType")));
		json_object_object_add(paramJobj, "AP_PDP_Type", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_PDP_Type")));
		zcfgFeJsonObjFree(accessPointObj);
	}
#endif

	json_object_array_add(Jarray, paramJobj);
	return ret;
}

void zcfgFeDalShowCellWan(struct json_object *Jarray){
	struct json_object *obj = NULL;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	obj = json_object_array_get_idx(Jarray, 0);
	
	printf("\nCellular WAN Settings:\n");
	printf("%-30s %-10s \n","Data Roaming:", json_object_get_string(json_object_object_get(obj, "CELL_Roaming_Enable")));
#ifdef PACKAGE_lte_srv_diag
	printf("%-30s %-10s \n","Network Monitoring:", json_object_get_string(json_object_object_get(obj, "CELL_Network_Monitor_Enable")));
#endif
#ifdef ZYXEL_FQ_CODEL//Aaron
	printf("%-30s %-10s \n","FQ CODEL:", json_object_get_string(json_object_object_get(obj, "CELL_FQ_CODEL_Enable")));
#endif
	printf("%-30s %-10s \n","Disable UMTS:", json_object_get_string(json_object_object_get(obj, "CELL_Disable_UMTS")));
#ifndef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
	if(json_object_get_boolean(json_object_object_get(obj, "AP_ManualAPN")))
		printf("%-30s %-10s\n","APN Mode:", "Manual");
	else
		printf("%-30s %-10s\n","APN Mode:", "Automatic");
	printf("%-30s %-10s \n","APN:", json_object_get_string(json_object_object_get(obj, "AP_APN")));
	printf("%-30s %-10s \n","APN Username:", json_object_get_string(json_object_object_get(obj, "AP_Username")));
	printf("%-30s %-10s \n","APN Password:", json_object_get_string(json_object_object_get(obj, "AP_Password")));
	printf("%-30s %-10s \n","APN Authentication Type:", json_object_get_string(json_object_object_get(obj, "AP_Auth_Type")));
	printf("%-30s %-10s \n","APN PDP Type:", json_object_get_string(json_object_object_get(obj, "AP_PDP_Type")));
#endif
	
}


zcfgRet_t zcfgFeDalCellWan(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanEdit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

