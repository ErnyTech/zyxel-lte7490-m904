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

dal_param_t CELLWAN_BAND_param[]={
{"INTF_Preferred_Access_Technology", dalType_string, 0, 0, NULL},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SERVICE_DOMAIN
{"INTF_Preferred_Service_Domain", dalType_string, 0, 0, NULL},
#endif
{"INTF_Preferred_Bands", dalType_string, 0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}

};

zcfgRet_t zcfgFeDalCellWanBandEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t intfIid = {0};
	struct json_object *intfObj = NULL;
	const char *preferredAccessTechnology = NULL;
	const char *preferredServiceDomain = NULL;
	const char *preferredBands = NULL;

	IID_INIT(intfIid);
	intfIid.level=1;
	intfIid.idx[0]=1;

	preferredAccessTechnology = json_object_get_string(json_object_object_get(Jobj, "INTF_Preferred_Access_Technology"));
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SERVICE_DOMAIN	
	preferredServiceDomain = json_object_get_string(json_object_object_get(Jobj, "INTF_Preferred_Service_Domain"));
#endif	
	preferredBands = json_object_get_string(json_object_object_get(Jobj, "INTF_Preferred_Bands"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &intfIid, &intfObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "INTF_Preferred_Access_Technology"))
			json_object_object_add(intfObj, "PreferredAccessTechnology", json_object_new_string(preferredAccessTechnology));
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SERVICE_DOMAIN			
		if(json_object_object_get(Jobj, "INTF_Preferred_Service_Domain"))
			json_object_object_add(intfObj, "PreferredServiceDomain", json_object_new_string(preferredServiceDomain));			
#endif			
		if(json_object_object_get(Jobj, "INTF_Preferred_Bands"))
			json_object_object_add(intfObj, "X_ZYXEL_PreferredBands", json_object_new_string(preferredBands));
		zcfgFeObjJsonSet(RDM_OID_CELL_INTF, &intfIid, intfObj, NULL);
	}

	zcfgFeJsonObjFree(intfObj);

	return ret;
}	

zcfgRet_t zcfgFeDalCellWanBandGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *intfObj = NULL;
	objIndex_t intfIid = {0};
	paramJobj = json_object_new_object();

	IID_INIT(intfIid);
	intfIid.level=1;
	intfIid.idx[0]=1;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &intfIid, &intfObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "INTF_Supported_Access_Technologies", JSON_OBJ_COPY(json_object_object_get(intfObj, "SupportedAccessTechnologies")));
		json_object_object_add(paramJobj, "INTF_Preferred_Access_Technology", JSON_OBJ_COPY(json_object_object_get(intfObj, "PreferredAccessTechnology")));
		json_object_object_add(paramJobj, "INTF_Current_Access_Technology", JSON_OBJ_COPY(json_object_object_get(intfObj, "CurrentAccessTechnology")));
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SERVICE_DOMAIN		
		json_object_object_add(paramJobj, "INTF_Supported_Service_Domain", JSON_OBJ_COPY(json_object_object_get(intfObj, "SupportedServiceDomain")));
		json_object_object_add(paramJobj, "INTF_Preferred_Service_Domain", JSON_OBJ_COPY(json_object_object_get(intfObj, "PreferredServiceDomain")));		
#endif		
		json_object_object_add(paramJobj, "INTF_Supported_Bands", JSON_OBJ_COPY(json_object_object_get(intfObj, "X_ZYXEL_SupportedBands")));
		json_object_object_add(paramJobj, "INTF_Preferred_Bands", JSON_OBJ_COPY(json_object_object_get(intfObj, "X_ZYXEL_PreferredBands")));
		json_object_object_add(paramJobj, "INTF_Current_Band", JSON_OBJ_COPY(json_object_object_get(intfObj, "X_ZYXEL_CurrentBand")));

		zcfgFeJsonObjFree(intfObj);
	}

	json_object_array_add(Jarray, paramJobj);
	return ret;
}

void zcfgFeDalShowCellWanBand(struct json_object *Jarray){
	struct json_object *obj = NULL;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	obj = json_object_array_get_idx(Jarray, 0);
	
	printf("\nAccess Technologies:\n");
	if(!strcmp("",json_object_get_string(json_object_object_get(obj, "INTF_Supported_Access_Technologies"))))
		printf("%-30s %-10s\n","Supported Access Technologies:", "N/A");
	else
		printf("%-30s %-10s\n","Supported Access Technologies:", json_object_get_string(json_object_object_get(obj, "INTF_Supported_Access_Technologies")));
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SERVICE_DOMAIN		
	printf("\nService Domain:\n");
	if(!strcmp("",json_object_get_string(json_object_object_get(obj, "INTF_Supported_Service_Domain"))))
		printf("%-30s %-10s\n","Supported Service Domain:", "N/A");
	else
		printf("%-30s %-10s\n","Supported Service Domain:", json_object_get_string(json_object_object_get(obj, "INTF_Supported_Service_Domain")));		
#endif
	printf("\n Preferred Access Technology: \n");
	if(!strcmp("",json_object_get_string(json_object_object_get(obj, "INTF_Preferred_Access_Technology"))))
		printf("%-30s %-10s\n","Preferred Access Technologies:", "N/A");
	else
		printf("%-30s %-10s\n","Preferred Access Technologies:", json_object_get_string(json_object_object_get(obj, "INTF_Preferred_Access_Technology")));
}


zcfgRet_t zcfgFeDalCellWanBand(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanBandEdit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanBandGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

