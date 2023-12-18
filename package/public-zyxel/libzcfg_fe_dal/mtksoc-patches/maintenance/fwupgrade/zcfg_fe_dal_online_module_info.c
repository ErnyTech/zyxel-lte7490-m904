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

dal_param_t ONLINE_Module_param[] = {
    { NULL,	0,	0,	0,	NULL}
};

zcfgRet_t zcfgFeDalOnlineModuleInfoGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid = {0};
	struct json_object *paramJobj = NULL;
	struct json_object *Obj = NULL;

	paramJobj = json_object_new_object();
	IID_INIT(iid);

	if ((ret = zcfgFeObjJsonGet(RDM_OID_ONLINE_MODULE_UPGRADE, &iid, &Obj)) == ZCFG_SUCCESS) {
		json_object_object_add(paramJobj, "ModuleVersion",      JSON_OBJ_COPY(json_object_object_get(Obj, "ModuleVersion")));
		json_object_object_add(paramJobj, "ModuleReleaseDate",  JSON_OBJ_COPY(json_object_object_get(Obj, "ModuleReleaseDate")));
		json_object_object_add(paramJobj, "ModuleSize",         JSON_OBJ_COPY(json_object_object_get(Obj, "ModuleSize")));
		zcfgFeJsonObjFree(Obj);
	}

	json_object_array_add(Jarray, paramJobj);

	return ret;
}

zcfgRet_t zcfgFeDalOnlineModuleInfo(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "GET"))
		ret = zcfgFeDalOnlineModuleInfoGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

