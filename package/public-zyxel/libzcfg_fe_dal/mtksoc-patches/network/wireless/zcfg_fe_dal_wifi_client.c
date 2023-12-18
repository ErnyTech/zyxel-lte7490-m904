#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
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

#ifdef SUPPORT_APCLIENT
dal_param_t WIFI_CLIENT_param[]={
	{"ApCliEnable",		dalType_boolean,	0,	0,	NULL,	NULL,	0},
	{"ApCliAuthMode",	dalType_string,	        0,      0,      NULL,   NULL,   0},
	{"ApCliEncrypType",	dalType_string,         0,      0,      NULL,   NULL,   0},
	{"ApCliSsid",		dalType_string,         0,      0,      NULL,   NULL,   0},
	{"ApCliWPAPSK",		dalType_string,         0,      0,      NULL,   NULL,   0},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
	{"ActivationCode",	dalType_string,		0,      0,      NULL,   NULL,   0},
#endif
	{ NULL,	0,	0,	0,	NULL}
};

zcfgRet_t zcfgFeDalWifiClientGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *ZyWifiClientObj = NULL;
	zcfg_offset_t ZyWifiClientRdmOid = RDM_OID_AP_CLIENT;
	objIndex_t ZyWifiClientIid = {0};
	bool ZyApClientEnable = false;

	paramJobj = json_object_new_object();

	IID_INIT(ZyWifiClientIid);

	if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyWifiClientRdmOid, &ZyWifiClientIid, &ZyWifiClientObj)) == ZCFG_SUCCESS){
		ZyApClientEnable = Jgetb(ZyWifiClientObj, "ApCliEnable");
		json_object_object_add(paramJobj, "ApCliEnable", json_object_new_boolean(ZyApClientEnable));
		json_object_object_add(paramJobj, "ApCliAuthMode", JSON_OBJ_COPY(json_object_object_get(ZyWifiClientObj, "ApCliAuthMode")));
		json_object_object_add(paramJobj, "ApCliEncrypType", JSON_OBJ_COPY(json_object_object_get(ZyWifiClientObj, "ApCliEncrypType")));
		json_object_object_add(paramJobj, "ApCliSsid", JSON_OBJ_COPY(json_object_object_get(ZyWifiClientObj, "ApCliSsid")));
		json_object_object_add(paramJobj, "ApCliWPAPSK", JSON_OBJ_COPY(json_object_object_get(ZyWifiClientObj, "ApCliWPAPSK")));
		zcfgFeJsonObjFree(ZyWifiClientObj);
	}

	json_object_array_add(Jarray, paramJobj);

	return ret;
}

void zcfgFeDalShowWifiClient(struct json_object *Jarray){
	struct json_object *Jobj = NULL;
	int channel = 0;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	Jobj = json_object_array_get_idx(Jarray, 0);
	printf("%-30s %s\n","Enable:", Jgets(Jobj, "ApCliEnable"));
	printf("%-30s %s\n","ApCliAuthMode:", Jgets(Jobj, "ApCliAuthMode"));
	printf("%-30s %s\n","ApCliEncrypType:", Jgets(Jobj, "ApCliEncrypType"));
	printf("%-30s %s\n","ApCliSsid:", Jgets(Jobj, "ApCliSsid"));
	printf("%-30s %s\n","ApCliWPAPSK:", Jgets(Jobj, "ApCliWPAPSK"));
}

zcfgRet_t zcfgFeDalWifiClientEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *ZyWifiClientObj = NULL;
	objIndex_t ZyWifiClientIid = {0};
	zcfg_offset_t ZyWifiClientRdmOid = RDM_OID_AP_CLIENT;

	IID_INIT(ZyWifiClientIid);

	if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyWifiClientRdmOid, &ZyWifiClientIid, &ZyWifiClientObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "ApCliEnable")){
			json_object_object_add(ZyWifiClientObj, "ApCliEnable", json_object_new_boolean(Jgetb(Jobj, "ApCliEnable")));
		}

		if(json_object_object_get(Jobj, "ApCliAuthMode")){
			json_object_object_add(ZyWifiClientObj, "ApCliAuthMode", json_object_new_string(json_object_get_string(json_object_object_get(Jobj, "ApCliAuthMode"))));
		}

		if(json_object_object_get(Jobj, "ApCliEncrypType")){
			json_object_object_add(ZyWifiClientObj, "ApCliEncrypType", json_object_new_string(json_object_get_string(json_object_object_get(Jobj, "ApCliEncrypType"))));
		}

		if(json_object_object_get(Jobj, "ApCliSsid")){
			json_object_object_add(ZyWifiClientObj, "ApCliSsid", json_object_new_string(json_object_get_string(json_object_object_get(Jobj, "ApCliSsid"))));
		}

		if(json_object_object_get(Jobj, "ApCliWPAPSK")){
			json_object_object_add(ZyWifiClientObj, "ApCliWPAPSK", json_object_new_string(json_object_get_string(json_object_object_get(Jobj, "ApCliWPAPSK"))));
		}
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
		if(json_object_object_get(Jobj, "ActivationCode")){
			json_object_object_add(ZyWifiClientObj, "ActivationCode", json_object_new_string(json_object_get_string(json_object_object_get(Jobj, "ActivationCode"))));
		} else {
			json_object_object_add(ZyWifiClientObj, "ActivationCode", json_object_new_string(""));
		}
#endif
	} else {
		printf("get object fail\n");
	}

	if((ret = zcfgFeObjJsonSet(ZyWifiClientRdmOid, &ZyWifiClientIid, ZyWifiClientObj, NULL)) != ZCFG_SUCCESS){
		zcfgFeJsonObjFree(ZyWifiClientObj);
		printf("zcfgFeObjJsonSet RDM_OID_CELL_ZY_ESIM fail.\n");
		return ret;
	}

	if (ZyWifiClientObj) zcfgFeJsonObjFree(ZyWifiClientObj);

	return ret;
}

zcfgRet_t zcfgFeDalWifiClient(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		 ret = zcfgFeDalWifiClientEdit(Jobj, replyMsg);
	else if (!strcmp(method, "GET"))
		ret = zcfgFeDalWifiClientGet(Jobj, Jarray, replyMsg);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}
#endif
