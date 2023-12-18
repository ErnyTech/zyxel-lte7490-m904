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


dal_param_t TR369_LOCAL_AGENT_CONTROLLER_param[] = {
	{"Index", 		dalType_int, 		0,	0,		NULL,	NULL,	dal_Edit | dal_Delete},
	{"Enable", 		dalType_boolean,	0,	0,		NULL,	NULL,	dal_Add},
	{"Alias",		dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"EndpointID",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{NULL,			0,					0,	0,		NULL,	NULL,	NULL}
};


// typedef struct rdm_DevLocalAgentController_s {   // RDM_OID_DEV_LOCAL_AGENT_CONTROLLER
//     char Alias[65];
//     char EndpointID[257];
//     char ControllerCode[129];
//     char ProvisioningCode[65];
//     bool Enable;
//     char AssignedRole[257];
//     char InheritedRole[257];
//     char Credential[257];
//     uint32_t PeriodicNotifInterval;
//     char PeriodicNotifTime[32];
//     uint32_t USPNotifRetryMinimumWaitInterval;
//     uint32_t USPNotifRetryIntervalMultiplier;
//     uint32_t BootParameterNumberOfEntries;
//     uint32_t MTPNumberOfEntries;
// } rdm_DevLocalAgentController_t;



struct json_object *tr369agentControllerObj;
objIndex_t tr369agentControllerIid = {0};

bool g_enable;
const char* g_alias;
const char* g_endpointID;

void initTR369agentControllerGlobalObjects() {
	tr369agentControllerObj = NULL;
	return;
}

void freeAlltr369agentControllerObjects() {
	if (tr369agentControllerObj) {
		json_object_put(tr369agentControllerObj);
		tr369agentControllerObj = NULL;
	}

	return ;
}

// fetch Jobj to variables
void getTR369LocalAgentControllerBasicInfo(struct json_object *Jobj) {
	g_enable = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	printf("g_enable = %d\n", g_enable);

	g_alias = json_object_get_string(json_object_object_get(Jobj, "Alias"));
	if (g_alias) {
		printf("g_alias = %s\n", g_alias);
	}

	g_endpointID = json_object_get_string(json_object_object_get(Jobj, "EndpointID"));
	if (g_endpointID) {
		printf("g_endpointID = %s\n", g_endpointID);
	}

	return;
}

// dalcmd show
void zcfgFeDalShowTR369LocalAgentController(struct json_object *Jarray) {
	int i, len = 0;
	struct json_object *obj = NULL;

	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-5s %-6s %-15s %-10s\n",
		"Index", "Enable", "Alias", "EndpointID");

	len = json_object_array_length(Jarray);
	for (i = 0; i < len; i++) {
		obj = json_object_array_get_idx(Jarray, i);

		printf( "%-5s %-6s %-15s %-30s\n",
			json_object_get_string(json_object_object_get(obj, "Index")),
			json_object_get_string(json_object_object_get(obj, "Enable")),
			json_object_get_string(json_object_object_get(obj, "Alias")),
			json_object_get_string(json_object_object_get(obj, "EndpointID")) );
	}
}

// parse input Jobj and generate tr369agentControllerObj
zcfgRet_t zcfgFeDaltr369agentControllerObjEdit(struct json_object *Jobj) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	//printf("%s(): --- Edit TR369 Local Agent: Controller Json Object --- \n%s \n ", __FUNCTION__, json_object_to_json_string(Jobj));
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &tr369agentControllerIid, &tr369agentControllerObj)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get TR369 local agent: Controller fail oid(%d)\n ", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_CONTROLLER);
		return ZCFG_INTERNAL_ERROR;
	}

	if (tr369agentControllerObj != NULL) {
		if(json_object_object_get(Jobj, "Enable"))
			json_object_object_add(tr369agentControllerObj, "Enable", json_object_new_boolean(g_enable));

		if(json_object_object_get(Jobj, "Alias"))
			json_object_object_add(tr369agentControllerObj, "Alias", json_object_new_string(g_alias));

		if(json_object_object_get(Jobj, "EndpointID"))
			json_object_object_add(tr369agentControllerObj, "EndpointID", json_object_new_string(g_endpointID));
	}

	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerEdit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *Jarray = NULL;
	int index = 0;
	zcfg_offset_t oid;

	IID_INIT(tr369agentControllerIid);
	oid = RDM_OID_DEV_LOCAL_AGENT_CONTROLLER;

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	initTR369agentControllerGlobalObjects();	// tr369agentControllerObj = NULL;

	// fetch Jobj to variables
	getTR369LocalAgentControllerBasicInfo(Jobj);

	// Get all local agent Controller array list(Jarray)
	Jarray = json_object_new_array();
	zcfgFeDalTR369LocalAgentControllerGet(NULL, Jarray, NULL);

	// get iid(tr369agentControllerIid) by index(feteched from Jobj Index property)
	ret = convertIndextoIid(index, &tr369agentControllerIid, oid, NULL, NULL, replyMsg);

	if (ret != ZCFG_SUCCESS)
		return ret;

	// parse input Jobj and generate tr369agentControllerObj
	if ((ret = zcfgFeDaltr369agentControllerObjEdit(Jobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to edit TR369 Local Agent: Controller Entry \n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &tr369agentControllerIid, tr369agentControllerObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d RDM_OID_DEV_LOCAL_AGENT_CONTROLLER\n", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_CONTROLLER);
	}

exit:
	freeAlltr369agentControllerObjects();
	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *paramJobj = NULL;
	struct json_object *tr369LocalAgentControllerJobj = NULL;
	objIndex_t objIid = {0};

	int count = 1;

	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &objIid, &tr369LocalAgentControllerJobj) == ZCFG_SUCCESS) {
		paramJobj = json_object_new_object();

		json_object_object_add(paramJobj, "Index",		json_object_new_int(count));
		json_object_object_add(paramJobj, "Enable",		JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerJobj, "Enable")));
		json_object_object_add(paramJobj, "Alias",		JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerJobj, "Alias")));
		json_object_object_add(paramJobj, "EndpointID",	JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerJobj, "EndpointID")));

		json_object_array_add(Jarray, paramJobj);

		zcfgFeJsonObjFree(tr369LocalAgentControllerJobj);

		count++;
	}

	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerAdd(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	initTR369agentControllerGlobalObjects();
	getTR369LocalAgentControllerBasicInfo(Jobj);

	IID_INIT(tr369agentControllerIid);

	if ((ret = zcfgFeObjJsonBlockedAdd(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &tr369agentControllerIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonAdd add TR369 Local Agent: Controller Entry\n ", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeDaltr369agentControllerObjEdit(Jobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to edit TR369 Local Agent: Controller Entry\n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &tr369agentControllerIid, tr369agentControllerObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d TR369 Local Agent: Controller Entry\n", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_CONTROLLER);
	}

exit:
	freeAlltr369agentControllerObjects();
	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerDelete(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	int index = 0;
	zcfg_offset_t oid;

	IID_INIT(tr369agentControllerIid);

	oid = RDM_OID_DEV_LOCAL_AGENT_CONTROLLER;
	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	ret = convertIndextoIid(index, &tr369agentControllerIid, oid, NULL, NULL, replyMsg);
	if (ret != ZCFG_SUCCESS)
		return ret;

	if ((ret = zcfgFeObjJsonDel(oid, &tr369agentControllerIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s: delete tr369 local agent: controller obj fail \n", __FUNCTION__);
		goto exit;
	}

exit:
	freeAlltr369agentControllerObjects();
	return ret;

}

zcfgRet_t zcfgFeDalTR369LocalAgentController(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "PUT")) {
		ret = zcfgFeDalTR369LocalAgentControllerEdit(Jobj, replyMsg);
	}
	else if (!strcmp(method, "POST")) {
		ret = zcfgFeDalTR369LocalAgentControllerAdd(Jobj, replyMsg);
	}
	else if (!strcmp(method, "DELETE")) {
		ret = zcfgFeDalTR369LocalAgentControllerDelete(Jobj, replyMsg);
	}
	else if (!strcmp(method, "GET")) {
		ret = zcfgFeDalTR369LocalAgentControllerGet(Jobj, Jarray, NULL);
	}
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

