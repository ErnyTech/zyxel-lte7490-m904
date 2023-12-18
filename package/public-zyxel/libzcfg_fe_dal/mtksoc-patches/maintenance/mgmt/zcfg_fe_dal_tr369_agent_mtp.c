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


dal_param_t TR369_LOCAL_AGENT_MTP_param[] = {
	{"Index", 		dalType_int, 		0, 	0,	 	NULL,	NULL,	dal_Edit | dal_Delete},
	{"Enable", 		dalType_boolean,	0,	0,		NULL,	NULL,	dal_Add},
	{"Alias",		dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"Status",		dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"Protocol",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"EnableMDNS",	dalType_boolean,	0,	0,		NULL,	NULL,	NULL},
	{NULL,			0,					0,	0,		NULL,	NULL,	NULL}
};


// typedef struct rdm_DevLocalAgentMtp_s {   // RDM_OID_DEV_LOCAL_AGENT_M_T_P 
//     char Alias[65];
//     bool Enable;
//     char Status[33];
//     char Protocol[257];
//     bool EnableMDNS;
// } rdm_DevLocalAgentMtp_t;


struct json_object *tr369agentMtpObj;
objIndex_t tr369agentMtpIid = {0};

bool g_enable;
const char* g_alias;
const char* g_protocol;

void initTR369agentMtpGlobalObjects() {
	tr369agentMtpObj = NULL;
	return;
}

void freeAlltr369agentMtpObjects() {
	if (tr369agentMtpObj) {
		json_object_put(tr369agentMtpObj);
		tr369agentMtpObj = NULL;
	}

	return ;
}

// fetch Jobj to variables
void getTR369LocalAgentMtpBasicInfo(struct json_object *Jobj) {
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
void zcfgFeDalShowTR369LocalAgentMtp(struct json_object *Jarray) {
	int i, len = 0;
	struct json_object *obj = NULL;

	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-5s %-6s %-15s %-10s\n",
		"Index", "Enable", "Alias", "Prootocol");

	len = json_object_array_length(Jarray);
	for (i = 0; i < len; i++) {
		obj = json_object_array_get_idx(Jarray, i);

		printf( "%-5s %-6s %-15s %-10s\n",
			json_object_get_string(json_object_object_get(obj, "Index")),
			json_object_get_string(json_object_object_get(obj, "Enable")),
			json_object_get_string(json_object_object_get(obj, "Alias")),
			json_object_get_string(json_object_object_get(obj, "Protocol")) );
	}
}

zcfgRet_t zcfgFeDaltr369agentMtpObjEdit(struct json_object *Jobj) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	//printf("%s(): --- Edit TR369 Local Agent MTP Json Object --- \n%s \n ", __FUNCTION__, json_object_to_json_string(Jobj));
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &tr369agentMtpIid, &tr369agentMtpObj)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get TR369 local agent: MTP fail oid(%d)\n ", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_M_T_P);
		return ZCFG_INTERNAL_ERROR;
	}

	if (tr369agentMtpObj != NULL) {
		if(json_object_object_get(Jobj, "Enable"))
			json_object_object_add(tr369agentMtpObj, "Enable", json_object_new_boolean(g_enable));

		if(json_object_object_get(Jobj, "Alias"))
			json_object_object_add(tr369agentMtpObj, "Alias", json_object_new_string(g_alias));

		if(json_object_object_get(Jobj, "Protocol"))
			json_object_object_add(tr369agentMtpObj, "Protocol", json_object_new_string(g_protocol));
	}

	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentMtpEdit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *Jarray = NULL;
	int index = 0;
	zcfg_offset_t oid;

	IID_INIT(tr369agentMtpIid);
	oid = RDM_OID_DEV_LOCAL_AGENT_M_T_P;

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	initTR369agentMtpGlobalObjects();	// tr369agentMtpObj = NULL;

	// fetch Jobj to variables
	getTR369LocalAgentMtpBasicInfo(Jobj);

	// Get all local agent MTP array list
	Jarray = json_object_new_array();
	zcfgFeDalTR369LocalAgentMtpGet(NULL, Jarray, NULL);

	// get iid(tr369agentMtpIid) by index(feteched from Jobj Index property)
	ret = convertIndextoIid(index, &tr369agentMtpIid, oid, NULL, NULL, replyMsg);

	if (ret != ZCFG_SUCCESS)
		return ret;

	// parse input Jobj and generate tr369agentMtpObj
	if ((ret = zcfgFeDaltr369agentMtpObjEdit(Jobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to edit TR369 Local Agent: MTP Entry \n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &tr369agentMtpIid, tr369agentMtpObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d RDM_OID_DEV_LOCAL_AGENT_M_T_P\n", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_M_T_P);
	}

exit:
	freeAlltr369agentMtpObjects();
	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentMtpGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	struct json_object *paramJobj = NULL;
	struct json_object *tr369LocalAgentMtpJobj = NULL;
	objIndex_t objIid = {0};

	int count = 1;

	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &objIid, &tr369LocalAgentMtpJobj) == ZCFG_SUCCESS) {
		paramJobj = json_object_new_object();

		json_object_object_add(paramJobj, "Index",    json_object_new_int(count));
		json_object_object_add(paramJobj, "Enable",   JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpJobj, "Enable")));
		json_object_object_add(paramJobj, "Alias",    JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpJobj, "Alias")));
		json_object_object_add(paramJobj, "Protocol", JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentMtpJobj, "Protocol")));

		json_object_array_add(Jarray, paramJobj);

		zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);

		count++;
	}

	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgentMtpAdd(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	initTR369agentMtpGlobalObjects();
	getTR369LocalAgentMtpBasicInfo(Jobj);

	IID_INIT(tr369agentMtpIid);

	if ((ret = zcfgFeObjJsonBlockedAdd(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &tr369agentMtpIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s():(ERROR) zcfgFeObjJsonAdd add TR369 Local Agent: MTP Entry\n ", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeDaltr369agentMtpObjEdit(Jobj)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to edit TR369 Local Agent: MTP Entry\n", __FUNCTION__);
		goto exit;
	}

	if ((ret = zcfgFeObjJsonSet(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &tr369agentMtpIid, tr369agentMtpObj, NULL)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) fail to set object %d TR369 Local Agent: MTP Entry\n", __FUNCTION__, RDM_OID_DEV_LOCAL_AGENT_M_T_P);
	}

exit:
	freeAlltr369agentMtpObjects();
	return ret;
}

zcfgRet_t zcfgFeDalTR369LocalAgentMtpDelete(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	int index = 0;
	zcfg_offset_t oid;

	IID_INIT(tr369agentMtpIid);

	oid = RDM_OID_DEV_LOCAL_AGENT_M_T_P;
	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	ret = convertIndextoIid(index, &tr369agentMtpIid, oid, NULL, NULL, replyMsg);
	if (ret != ZCFG_SUCCESS)
		return ret;

	if ((ret = zcfgFeObjJsonDel(oid, &tr369agentMtpIid, NULL)) != ZCFG_SUCCESS) {
		printf("%s: delete tr369 local agent mtp obj fail \n", __FUNCTION__);
		goto exit;
	}

exit:
	freeAlltr369agentMtpObjects();
	return ret;

}

zcfgRet_t zcfgFeDalTR369LocalAgentMtp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!strcmp(method, "PUT")) {
		ret = zcfgFeDalTR369LocalAgentMtpEdit(Jobj, replyMsg);
	}
	// ToDo: currently agent MTP just one instance
	/*
	else if (!strcmp(method, "POST")) {
		ret = zcfgFeDalTR369LocalAgentMtpAdd(Jobj, replyMsg);
	}
	else if (!strcmp(method, "DELETE")) {
		ret = zcfgFeDalTR369LocalAgentMtpDelete(Jobj, replyMsg);
	}
	*/
	else if (!strcmp(method, "GET")) {
		ret = zcfgFeDalTR369LocalAgentMtpGet(Jobj, Jarray, NULL);
	}
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

