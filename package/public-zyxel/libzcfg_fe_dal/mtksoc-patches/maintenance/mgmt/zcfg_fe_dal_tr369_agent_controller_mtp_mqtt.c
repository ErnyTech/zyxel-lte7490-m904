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


dal_param_t TR369_LOCAL_AGENT_CONTROLLER_MTP_MQTT_param[] = {
	{"Index",					dalType_int,		0,	0,	NULL,	NULL,	dal_Edit | dal_Delete},
	{"Parent_Index",			dalType_int,		0,	0,	NULL,	NULL,	dal_Add | dal_Edit | dal_Delete},
	{"Reference",				dalType_string,		0,	0,	NULL,	NULL,	NULL},
	{"Topic",					dalType_string,		0,	0,	NULL,	NULL,	NULL},
	// {"PublishRetainResponse",	dalType_boolean,	0,	0,	NULL,	NULL,	NULL},
	// {"PublishRetainNotify",		dalType_boolean,	0,	0,	NULL,	NULL,	NULL},
	{NULL,						0,					0,	0,	NULL,	NULL,	NULL}
};

// typedef struct rdm_LocalAgentControllerMTPMQTT_s {   // RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T 
//     char Reference[257];
//     char Topic[65536];
//     bool PublishRetainResponse;
//     bool PublishRetainNotify;
// } rdm_LocalAgentControllerMTPMQTT_t;


struct json_object *tr369agentControllerMtpMqttObj;
objIndex_t tr369agentControllerMtpMqttIid = {0};

int g_parent_index;

const char* g_reference;
const char* g_topic;

void initTR369agentControllerMtpMqttGlobalObjects() {
	tr369agentControllerMtpMqttObj = NULL;
	return;
}

void freeAlltr369agentControllerMtpMqttObjects() {
	if (tr369agentControllerMtpMqttObj) {
		json_object_put(tr369agentControllerMtpMqttObj);
		tr369agentControllerMtpMqttObj = NULL;
	}

	return ;
}

// fetch Jobj to variables
void getTR369LocalAgentControllerMtpMqttBasicInfo(struct json_object *Jobj) {
	g_parent_index = json_object_get_int(json_object_object_get(Jobj, "Parent_Index"));
	printf("g_parent_index = %d\n", g_parent_index);

	g_reference = json_object_get_string(json_object_object_get(Jobj, "Reference"));
	if (g_reference) {
		printf("g_reference = %s\n", g_reference);
	}

	g_topic = json_object_get_string(json_object_object_get(Jobj, "Topic"));
	if (g_topic) {
		printf("g_topic = %s\n", g_topic);
	}

	return;
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpMqttEdit(struct json_object *Jobj, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *tr369LocalAgentControllerMtpMqttObj = NULL;
	objIndex_t tr369LocalAgentControllerMtpMqttIid = {0};

	const char *reference = NULL;
	const char *topic = NULL;
	// bool publishRetainResponse = false;
	// bool publishRetainNotify = false;

	IID_INIT(tr369LocalAgentControllerMtpMqttIid);


	int parent_index = json_object_get_int(json_object_object_get(Jobj, "Parent_Index"));
	printf("[%s][%d] Parent_Index (%d)\n", __FUNCTION__, __LINE__, parent_index);

	tr369LocalAgentControllerMtpMqttIid.level = 1;
	if (parent_index > 0) {
		tr369LocalAgentControllerMtpMqttIid.idx[0] = parent_index;
	} else {
		tr369LocalAgentControllerMtpMqttIid.idx[0] = 1;
	}

	int idx = json_object_get_int(json_object_object_get(Jobj, "Index"));
	printf("[%s][%d] Index (%d)\n", __FUNCTION__, __LINE__, idx);

	tr369LocalAgentControllerMtpMqttIid.level = 2;
	if (idx > 0) {
		tr369LocalAgentControllerMtpMqttIid.idx[1] = idx;
	} else {
		tr369LocalAgentControllerMtpMqttIid.idx[1] = 1;
	}

	reference 				= json_object_get_string( json_object_object_get(Jobj, "Reference"));
	topic 					= json_object_get_string( json_object_object_get(Jobj, "Topic"));
	// publishRetainResponse	= json_object_get_boolean(json_object_object_get(Jobj, "PublishRetainResponse"));
	// publishRetainNotify		= json_object_get_boolean(json_object_object_get(Jobj, "PublishRetainNotify"));

	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T, &tr369LocalAgentControllerMtpMqttIid, &tr369LocalAgentControllerMtpMqttObj)) == ZCFG_SUCCESS) {

		if(json_object_object_get(Jobj, "Reference"))
			json_object_object_add(tr369LocalAgentControllerMtpMqttObj, "Reference", json_object_new_string(reference));

		if(json_object_object_get(Jobj, "Topic"))
			json_object_object_add(tr369LocalAgentControllerMtpMqttObj, "Topic", json_object_new_string(topic));

		// if(json_object_object_get(Jobj, "PublishRetainResponse"))
		// 	json_object_object_add(tr369LocalAgentControllerMtpMqttObj, "PublishRetainResponse", json_object_new_boolean(publishRetainResponse));

		// if(json_object_object_get(Jobj, "PublishRetainNotify"))
		// 	json_object_object_add(tr369LocalAgentControllerMtpMqttObj, "PublishRetainNotify", json_object_new_boolean(publishRetainNotify));

		zcfgFeObjJsonSet(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T, &tr369LocalAgentControllerMtpMqttIid, tr369LocalAgentControllerMtpMqttObj, NULL);
	} else {
		// if Get fail, Add a new one.
		printf("[%s][%d] Data model Get fail\n", __FUNCTION__, __LINE__);
	}

	zcfgFeJsonObjFree(tr369LocalAgentControllerMtpMqttObj);

	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpMqttGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *paramJobj = NULL;
	struct json_object *tr369LocalAgentControllerObj = NULL;
	struct json_object *tr369LocalAgentControllerMtpMqttObj = NULL;
	// objIndex_t tr369LocalAgentControllerMtpMqttIid = {0};

	objIndex_t child_objIid = {0}, parent_objIid = {0};
	int parent_index = 0, index = 0, current_parent_index = 0, i = 0;
	int parent_exist[256] = {0};

	IID_INIT(parent_objIid);

	// check if the parent node exists
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_CONTROLLER, &parent_objIid, &tr369LocalAgentControllerObj) == ZCFG_SUCCESS) {
		parent_exist[parent_objIid.idx[0] - 1] = 1;
		zcfgFeJsonObjFree(tr369LocalAgentControllerObj);
	}

	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T, &child_objIid, &tr369LocalAgentControllerMtpMqttObj) == ZCFG_SUCCESS) {
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
		json_object_object_add(paramJobj, "Reference",		JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerMtpMqttObj, "Reference")));
		json_object_object_add(paramJobj, "Topic",			JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerMtpMqttObj, "Topic")));
		// json_object_object_add(paramJobj, "PublishRetainResponse",	JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerMtpMqttObj, "PublishRetainResponse")));
		// json_object_object_add(paramJobj, "PublishRetainNotify",		JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentControllerMtpMqttObj, "PublishRetainNotify")));

		json_object_array_add(Jarray, paramJobj);
		zcfgFeJsonObjFree(tr369LocalAgentControllerMtpMqttObj);
	}

	return ret;
}

void zcfgFeDalShowTR369LocalAgentControllerMtpMqtt(struct json_object *Jarray) {
	struct json_object *obj = NULL;
	int i, len = 0;

	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-14s %-6s %-32s %-30s\n","Parent_Index", "Index", "Reference                     ", "Topic                      ");
	printf("%-14s %-6s %-32s %-30s\n","============", "=====", "==============================", "===========================");

	len = json_object_array_length(Jarray);

	for (i = 0; i < len; i++) {
		obj = json_object_array_get_idx(Jarray, i);
		if (obj) {
			printf("%-14s %-6s %-32s %-30s\n",
				json_object_get_string(json_object_object_get(obj, "Parent_Index")),
				json_object_get_string(json_object_object_get(obj, "Index")),
				json_object_get_string(json_object_object_get(obj, "Reference")),
				json_object_get_string(json_object_object_get(obj, "Topic")) );
		}
		obj = NULL;
	}
}


zcfgRet_t zcfgFeDalTR369LocalAgentControllerMtpMqtt(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "PUT"))
		ret = zcfgFeDalTR369LocalAgentControllerMtpMqttEdit(Jobj, NULL);
	else if (!strcmp(method, "GET"))
		ret = zcfgFeDalTR369LocalAgentControllerMtpMqttGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

