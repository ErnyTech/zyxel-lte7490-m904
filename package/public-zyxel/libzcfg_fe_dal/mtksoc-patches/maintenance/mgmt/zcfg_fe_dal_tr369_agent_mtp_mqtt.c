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


dal_param_t TR369_LOCAL_AGENT_MTP_MQTT_param[] = {
	{"Index",					dalType_int,		0,	0,	NULL},
	{"Reference",				dalType_string,		0,	0,	NULL},
	{"ResponseTopicConfigured",	dalType_string,		0,	0,	NULL},
	{"ResponseTopicDiscovered",	dalType_string,		0,	0,	NULL},
	{"PublishQoS",				dalType_int,		0,	0,	NULL},
	{NULL,						0,					0,	0,	NULL}
};

// typedef struct rdm_DevLocalAgentMtpMqtt_s {   // RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T 
//     char Reference[257];
//     char ResponseTopicConfigured[65536];
//     char ResponseTopicDiscovered[65536];
//     uint32_t PublishQoS;
// } rdm_DevLocalAgentMtpMqtt_t;


zcfgRet_t zcfgFeDalTR369LocalAgentMtpMqttEdit(struct json_object *Jobj, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *tr369LocalAgentMtpMqttObj = NULL;
	objIndex_t tr369LocalAgentMtpMqttIid = {0};

	const char *reference = NULL;
	const char *responseTopicConfigured = NULL;
	const char *responseTopicDiscovered = NULL;
    int publishQoS = 0;

	IID_INIT(tr369LocalAgentMtpMqttIid);

	tr369LocalAgentMtpMqttIid.level = 1;

	int idx = json_object_get_int(json_object_object_get(Jobj, "Index"));
	printf("[%s][%d] idx(%d)\n", __FUNCTION__, __LINE__, idx);

	if (idx > 0) {
		tr369LocalAgentMtpMqttIid.idx[0] = idx;
	} else {
		tr369LocalAgentMtpMqttIid.idx[0] = 1;
	}

	reference 				= json_object_get_string(json_object_object_get(Jobj, "Reference"));
	responseTopicConfigured = json_object_get_string(json_object_object_get(Jobj, "ResponseTopicConfigured"));
	responseTopicDiscovered = json_object_get_string(json_object_object_get(Jobj, "ResponseTopicDiscovered"));
	publishQoS 				= json_object_get_int(   json_object_object_get(Jobj, "PublishQoS"));

	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T, &tr369LocalAgentMtpMqttIid, &tr369LocalAgentMtpMqttObj)) == ZCFG_SUCCESS) {

		if(json_object_object_get(Jobj, "Reference"))
			json_object_object_add(tr369LocalAgentMtpMqttObj, "Reference", json_object_new_string(reference));

		if(json_object_object_get(Jobj, "ResponseTopicConfigured"))
			json_object_object_add(tr369LocalAgentMtpMqttObj, "ResponseTopicConfigured", json_object_new_string(responseTopicConfigured));

		if(json_object_object_get(Jobj, "ResponseTopicDiscovered"))
			json_object_object_add(tr369LocalAgentMtpMqttObj, "ResponseTopicDiscovered", json_object_new_string(responseTopicDiscovered));

		if(json_object_object_get(Jobj, "PublishQoS"))
			json_object_object_add(tr369LocalAgentMtpMqttObj, "PublishQoS", json_object_new_int(publishQoS));

		zcfgFeObjJsonSet(RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T, &tr369LocalAgentMtpMqttIid, tr369LocalAgentMtpMqttObj, NULL);
	} else {
		// if Get fail, Add a new one.
		printf("[%s][%d] Data model Get fail\n", __FUNCTION__, __LINE__);
	}

	zcfgFeJsonObjFree(tr369LocalAgentMtpMqttObj);

	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentMtpMqttGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *paramJobj = NULL, *tr369LocalAgentMtpMqttObj = NULL;
	objIndex_t tr369LocalAgentMtpMqttIid = {0};

	IID_INIT(tr369LocalAgentMtpMqttIid);

	paramJobj = json_object_new_object();

	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T, &tr369LocalAgentMtpMqttIid, &tr369LocalAgentMtpMqttObj) == ZCFG_SUCCESS) {
		paramJobj = json_object_new_object();

		json_object_object_add(paramJobj, "idx",					 json_object_new_int(tr369LocalAgentMtpMqttIid.idx[0]));
		json_object_object_add(paramJobj, "Reference",				 JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpMqttObj, "Reference")));
		json_object_object_add(paramJobj, "ResponseTopicConfigured", JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpMqttObj, "ResponseTopicConfigured")));
		json_object_object_add(paramJobj, "ResponseTopicDiscovered", JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpMqttObj, "ResponseTopicDiscovered")));
		json_object_object_add(paramJobj, "PublishQoS",				 JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpMqttObj, "PublishQoS")));

		json_object_array_add(Jarray, paramJobj);
		zcfgFeJsonObjFree(tr369LocalAgentMtpMqttObj);
	}

	return ret;
}

void zcfgFeDalShowTR369LocalAgentMtpMqtt(struct json_object *Jarray) {
	struct json_object *obj = NULL;
	int i, len = 0;

	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-6s %-32s %-30s %-10s\n", "Index", "Reference                     ", "ResponseTopicConfigured    ", "PublishQoS");
	printf("%-6s %-32s %-30s %-10s\n", "=====", "==============================", "===========================", "==========");

	len = json_object_array_length(Jarray);

	for (i = 0; i < len; i++) {
		obj = json_object_array_get_idx(Jarray,i);
		if(obj){
#if 0
			printf("%-6s %-32s %-30s %-30s %-10s\n",
				json_object_get_string(json_object_object_get(obj, "idx")),
				json_object_get_string(json_object_object_get(obj, "Reference")),
				json_object_get_string(json_object_object_get(obj, "ResponseTopicConfigured")),
				json_object_get_string(json_object_object_get(obj, "ResponseTopicDiscovered")),
				json_object_get_string(json_object_object_get(obj, "PublishQoS")) );
#else
			printf("%-6s %-32s %-30s %-10s\n",
				json_object_get_string(json_object_object_get(obj, "idx")),
				json_object_get_string(json_object_object_get(obj, "Reference")),
				json_object_get_string(json_object_object_get(obj, "ResponseTopicConfigured")),
				json_object_get_string(json_object_object_get(obj, "PublishQoS")) );
#endif
		}
		obj = NULL;
	}
}


zcfgRet_t zcfgFeDalTR369LocalAgentMtpMqtt(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "PUT"))
		ret = zcfgFeDalTR369LocalAgentMtpMqttEdit(Jobj, NULL);
	else if (!strcmp(method, "GET"))
		ret = zcfgFeDalTR369LocalAgentMtpMqttGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

