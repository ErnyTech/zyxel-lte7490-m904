#include <ctype.h>
#include <json/json.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"


dal_param_t CELLWAN_MODULE_TEMP_param[]={		//RDM_OID_TEMP_STAT_TEMP_SENSOR
{"module_temperature", dalType_string,	0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}
};


zcfgRet_t zcfgFeDal_cellWanModuleTemperature_Get(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	rdm_TempStatTempSensor_t *tempSensorObj = NULL;
	enum json_type jsonType;
	struct json_object *paramJobj = NULL;
	objIndex_t objIid;
	int result = 0;
	const char* cmdType = NULL;

	IID_INIT(objIid);
	objIid.level=1;
	objIid.idx[0]=1;//hard code because there is LTE module pa0 temperature available right now.
	if(zcfgFeObjStructGet(RDM_OID_TEMP_STAT_TEMP_SENSOR, &objIid, (void **)&tempSensorObj) != ZCFG_SUCCESS){
		if(replyMsg) strcpy(replyMsg, "Request reject");
		return ZCFG_REQUEST_REJECT;
	}

	jsonType = json_object_get_type(Jobj);
	if(jsonType == json_type_object){
		cmdType = json_object_get_string(json_object_object_get(Jobj, "cmdtype"));

		if(!strcmp(cmdType, "CLI")){
			paramJobj = json_object_new_object();
			if(paramJobj){
				json_object_object_add(paramJobj, "module_temperature", json_object_new_int(tempSensorObj->Value));
				json_object_array_add(Jarray, paramJobj);
			}
		}

		if(tempSensorObj != NULL){
			zcfgFeObjStructFree(tempSensorObj);
		}
	}
	return ret;
}

void zcfgFeDalShowCellWanModuleTemp(struct json_object *Jarray){
	struct json_object *obj = NULL;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	obj = json_object_array_get_idx(Jarray, 0);
	if(obj)
		printf("%s%s \n","module_temperature:", json_object_get_string(json_object_object_get(obj, "module_temperature")));
		
}


zcfgRet_t zcfgFeDalCellWanModuleTemp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	if(!strcmp(method, "GET"))
		ret = zcfgFeDal_cellWanModuleTemperature_Get(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

