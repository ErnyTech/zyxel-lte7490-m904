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


dal_param_t TR369_LOCAL_AGENT_CONTROLLER_MTP_param[] = {
	{"Index",			dalType_int,		0,	0,		NULL,	NULL,	dal_Edit | dal_Delete},
	{"Parent_Index",	dalType_int,		0,	0,		NULL,	NULL,	dal_Add | dal_Edit | dal_Delete},
	{"Enable",			dalType_boolean,	0,	0,		NULL,	NULL,	dal_Add},
	{"Alias",			dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"Protocol",		dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{NULL,				0,					0,	0,		NULL,	NULL,	NULL}
};

// #define RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P     		120216 /*Device.LocalAgent.Controller.i.MTP.i*/
// typedef struct rdm_LocalAgentControllerMTP_s {   // RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P
//     char Alias[65];
//     bool Enable;
//     char Protocol[257];
// } rdm_LocalAgentControllerMTP_t;



struct json_object *tr369agentControllerMtpObj;
objIndex_t tr369agentControllerMtpIid = {0};

int g_parent_index;
bool g_enable;
const char* g_alias;
const char* g_protocol;

void initTR369agentControllerMtpGlobalObjects() {
	tr369agentControllerMtpObj = NULL;
	return;
}

void freeAlltr369agentControllerMtpObjects() {
	if (tr369agentControllerMtpObj) {
		json_object_put(tr369agentControllerMtpObj);
		tr369agentControllerMtpObj = NULL;
	}

	return ;
}

// fetch Jobj to variables
void getTR369LocalAgentControllerMtpBasicInfo(struct json_object *Jobj) {
	g_parent_index = json_object_get_int(json_object_object_get(Jobj, "Parent_Index"));
	printf("g_parent_index = %d\n", g_parent_index);

	g_enable = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	printf("g_enable = %d\n", g_enable);

	g_alias = json_object_get_string(json_object_object_get(Jobj, "Alias"));
	if (g_alias) {
		printf("g_alias = %s\n", g_alias);
	}

	g_protocol = json_object_get_string(json_object_object_get(Jobj, "Protocol"));
	if (g_protocol) {
		printf("g_protocol = %s\n", g_protocol);
	}

	return;
}

// dalcmd show
void zcfgFeDalShowTR369LocalAgentControllerMtp(struct json_object *Jarray) {
	int i, len = 0;
	struct json_object *obj = NULL;

	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-12s %-5s %-6s %-15s %-10s\n",
		"Parent_Index", "Index", "Enable", "Alias", "Prootocol");

	len = json_object_array_length(Jarray);
	for (i = 0; i < len; i++) {
		obj = json_object_array_get_idx(Jarray, i);

		printf( "%-12s %-5s %-6s %-15s %-10s\n",
			json_object_get_string(json_object_object_get(obj, "Parent_Index")),
			json_object_get_string(json_object_object_get(obj, "Index")),
			json_object_get_string(json_object_object_get(obj, "Enable")),
			json_object_get_string(json_object_object_get(obj, "Alias")),
			json_object_get_string(json_object_object_get(obj, "Protocol")) );
	}
}

zcfgRet_t zcfgFeDaltr369agentControllerMtpObjEdit(struct json_object *Jobj) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	//printf("%s(): --- Edit TR369 Local Agent: Controller MTP Json Object --- \n%s \n ", __FUNCTION__, json_object_to_json_string(Jobj));
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &tr369agentControllerMtpIid, &tr369agentControllerMtpObj)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get TR369 local agent: Controller MTP fail oid(%d)\n ", __FUNCTION__, RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P);
		return ZCFG_INTERNAL_ERROR;
	}

	if (tr369agentControllerMtpObj != NULL) {
		if(json_object_object_get(Jobj, "Enable"))
			json_object_object_add(tr369agentControllerMtpObj, "Enable", json_object_new_boolean(g_enable));

		if(json_object_object_get(Jobj, "Alias"))
			json_object_object_add(tr369agentControllerMtpObj, "Alias", json_object_new_string(g_alias));

		if(json_object_object_get(Jobj, "Protocol"))
			json_object_object_add(tr369agentControllerMtpObj, "Protocol", json_object_new_string(g_protocol));
	}

	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpEdit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	// struct json_object *Jarray = NULL;

	int index = 0, parent_index = 0;;
	zcfg_offset_t oid;

	IID_INIT(tr369agentControllerMtpIid);
	oid = RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P;

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	parent_index = json_object_get_int(json_object_object_get(Jobj, "Parent_Index"));

	initTR369agentControllerMtpGlobalObjects();	// tr369agentControllerMtpObj = NULL;

	// fetch Jobj to variables
	getTR369LocalAgentControllerMtpBasicInfo(Jobj);


	//Get the actual index in level 1
	tr369agentControllerMtpIid.level = 1;
	ret = convertIndextoIid(parent_index, &tr369agentControllerMtpIid, RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;

	//Get the actual index in level 2
	tr369agentControllerMtpIid.level = 2;
	ret = convertIndextoIid(index, &tr369agentControllerMtpIid, RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;


	// parse input Jobj and generate tr369agentControllerMtpObj
	if ((ret = zcfgFeDaltr369agentControllerMtpObjEdit(Jobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to edit TR369 Local Agent: Controller MTP Entry \n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &tr369agentControllerMtpIid, tr369agentControllerMtpObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P\n", __FUNCTION__, RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P);
	}

exit:
	freeAlltr369agentControllerMtpObjects();
	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *paramJobj = NULL;
	struct json_object *tr369agentControllerMtpObj = NULL;
	struct json_object *tr369agentControllerObj = NULL;

	objIndex_t child_objIid = {0}, parent_objIid = {0};
	int parent_index = 0, index = 0, current_parent_index = 0, i = 0;
	int parent_exist[256] = {0};

	IID_INIT(parent_objIid);

	// check if the parent node exists
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &parent_objIid, &tr369agentControllerObj) == ZCFG_SUCCESS) {
		parent_exist[parent_objIid.idx[0] - 1] = 1;
		zcfgFeJsonObjFree(tr369agentControllerObj);
	}

	IID_INIT(child_objIid);
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &child_objIid, &tr369agentControllerMtpObj) == ZCFG_SUCCESS) {

		// The index displayed does not contain deleted nodes.
		// Get the correct parent_index to be displayed.
		if (current_parent_index < child_objIid.idx[0]) {
			for (i = current_parent_index; i < child_objIid.idx[0]; i++) {
				if(parent_exist[i] != 0){
					parent_index++;
				}
			}
			index = 1;
			current_parent_index = child_objIid.idx[0];
		}

		paramJobj = json_object_new_object();
		json_object_object_add(paramJobj, "Parent_Index",	json_object_new_int(parent_index));
		json_object_object_add(paramJobj, "Index",			json_object_new_int(index++));
		json_object_object_add(paramJobj, "Enable",			JSON_OBJ_COPY(json_object_object_get(tr369agentControllerMtpObj, "Enable")));
		json_object_object_add(paramJobj, "Alias",			JSON_OBJ_COPY(json_object_object_get(tr369agentControllerMtpObj, "Alias")));
		json_object_object_add(paramJobj, "Protocol",		JSON_OBJ_COPY(json_object_object_get(tr369agentControllerMtpObj, "Protocol")));

		json_object_array_add(Jarray, paramJobj);

		zcfgFeJsonObjFree(tr369agentControllerMtpObj);
	}

	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpAdd(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	initTR369agentControllerMtpGlobalObjects();
	getTR369LocalAgentControllerMtpBasicInfo(Jobj);

	IID_INIT(tr369agentControllerMtpIid);

	// //MQTT.Client.i.Publish_Object.i
	// mqttClientPublishIid.level = 1;
	// mqttClientPublishIid.idx[0] = g_parent_index;

	// LocalAgent.Controller.i.MTP.i
	tr369agentControllerMtpIid.level = 1;
	tr369agentControllerMtpIid.idx[0] = g_parent_index;

	if ((ret = zcfgFeObjJsonBlockedAdd(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &tr369agentControllerMtpIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonAdd add TR369 Local Agent: Controller MTP Entry\n ", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeDaltr369agentControllerMtpObjEdit(Jobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to edit TR369 Local Agent: Controller MTP Entry\n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &tr369agentControllerMtpIid, tr369agentControllerMtpObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d TR369 Local Agent: Controller MTP Entry\n", __FUNCTION__, RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P);
	}

exit:
	freeAlltr369agentControllerMtpObjects();
	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpDelete(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	int index = 0, parent_index = 0;

	IID_INIT(tr369agentControllerMtpIid);

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	parent_index = json_object_get_int(json_object_object_get(Jobj, "Parent_Index"));

	//Get the actual index in level 1
	tr369agentControllerMtpIid.level = 1;
	ret = convertIndextoIid(parent_index, &tr369agentControllerMtpIid, RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;

	//Get the actual index in level 2
	tr369agentControllerMtpIid.level = 2;
	ret = convertIndextoIid(index, &tr369agentControllerMtpIid, RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;

	if((ret = zcfgFeObjJsonDel(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &tr369agentControllerMtpIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s: delete tr369 local agent: controller mtp obj fail \n", __FUNCTION__);
		goto exit;
	}

exit:
	freeAlltr369agentControllerMtpObjects();
	return ret;

}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "PUT")) {
		ret = zcfgFeDalTR369LocalAgentControllerMtpEdit(Jobj, replyMsg);
	}
	else if (!strcmp(method, "POST")) {
		ret = zcfgFeDalTR369LocalAgentControllerMtpAdd(Jobj, replyMsg);
	}
	else if (!strcmp(method, "DELETE")) {
		ret = zcfgFeDalTR369LocalAgentControllerMtpDelete(Jobj, replyMsg);
	}
	else if (!strcmp(method, "GET")) {
		ret = zcfgFeDalTR369LocalAgentControllerMtpGet(Jobj, Jarray, NULL);
	}
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

