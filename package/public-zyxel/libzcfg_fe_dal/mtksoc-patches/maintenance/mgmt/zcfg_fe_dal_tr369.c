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


dal_param_t TR369_LOCAL_AGENT_param[] = {
	{"EndpointID",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_BoundInterface",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_BoundInterfaceList",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_ActiveBoundInterface",	dalType_string,		0,	0,		NULL,	NULL,	dalcmd_Forbid},
#if 1//Jessie debug_on_off
	{"X_ZYXEL_Prototrace",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
#endif
	{NULL,			0,					0,	0,		NULL,	NULL,	NULL}
};


// typedef struct rdm_LocalAgent_s {   // RDM_OID_LOCAL_AGENT 
//     char EndpointID[257];
//     char SoftwareVersion[65];
//     uint32_t UpTime;
//     char SupportedProtocols[258];
//     char SupportedFingerprintAlgorithms[258];
//     char SupportedThresholdOperator[258];
//     char AdvertisedDeviceSubtypes[258];
//     uint32_t MaxSubscriptionChangeAdoptionTime;
//     uint32_t X_ZYXEL_Feature;
//     bool X_ZYXEL_CheckCertificate;
//     bool X_ZYXEL_CheckCertificateCN;
//     bool X_ZYXEL_Prototrace;
//     uint32_t X_ZYXEL_Prototrace_level;
//     bool X_ZYXEL_IPv4_Protocol;
//     bool X_ZYXEL_IPv6_Protocol;
//     char X_ZYXEL_BoundInterface[33];
//     char X_ZYXEL_BoundInterfaceList[129];
//     char X_ZYXEL_ActiveBoundInterface[33];
//     uint32_t CertificateNumberOfEntries;
// } rdm_LocalAgent_t;


void zcfgFeDalShowTR369LocalAgent(struct json_object *Jarray) {
	struct json_object *Jobj;
	char proto[32] = {0};


	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}
	Jobj = json_object_array_get_idx(Jarray, 0);


	printf("TR-369 Local Agent Information:\n");
	printf("%-45s %s\n", "EndpointID", json_object_get_string(json_object_object_get(Jobj, "EndpointID")));
	printf("%-45s %s\n", "X_ZYXEL_BoundInterface", json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_BoundInterface")));
	printf("%-45s %s\n", "X_ZYXEL_BoundInterfaceList", json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_BoundInterfaceList")));
	printf("%-45s %s\n", "X_ZYXEL_ActiveBoundInterface", json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_ActiveBoundInterface")));
#if 1//Jessie debug_on_off
	printf("%-45s %s\n", "X_ZYXEL_Prototrace", json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_Prototrace")));
#endif
}


static zcfgRet_t zcfgFeDalTR369LocalAgentGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *paraName;
	objIndex_t objIid;
	struct json_object *tr369LocalAgentJobj = NULL;
	struct json_object *pramJobj = NULL;
	// int intVal;
	const char *str;
	int n = 0;
#if 1//Jessie debug_on_off
	bool proto = 0;
#endif
	paraName = (const char *)TR369_LOCAL_AGENT_param[n++].paraName;

	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_LOCAL_AGENT, &objIid, &tr369LocalAgentJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object retrieve fail\n", __FUNCTION__);
		return ret;
	}

	pramJobj = json_object_new_object();
	while (paraName) {
		if (!strcmp(paraName, "EndpointID")) {
			str = json_object_get_string(json_object_object_get(tr369LocalAgentJobj, "EndpointID"));
			json_object_object_add(pramJobj, paraName, json_object_new_string(str));
		}
		else if (!strcmp(paraName, "X_ZYXEL_BoundInterface")) {
			str = json_object_get_string(json_object_object_get(tr369LocalAgentJobj, "X_ZYXEL_BoundInterface"));
			json_object_object_add(pramJobj, paraName, json_object_new_string(str));
		}
		else if (!strcmp(paraName, "X_ZYXEL_BoundInterfaceList")) {
			str = json_object_get_string(json_object_object_get(tr369LocalAgentJobj, "X_ZYXEL_BoundInterfaceList"));
			json_object_object_add(pramJobj, paraName, json_object_new_string(str));
		}
		else if (!strcmp(paraName, "X_ZYXEL_ActiveBoundInterface")) {
			str = json_object_get_string(json_object_object_get(tr369LocalAgentJobj, "X_ZYXEL_ActiveBoundInterface"));
			json_object_object_add(pramJobj, paraName, json_object_new_string(str));
		}
#if 1//Jessie debug_on_off
		else if (!strcmp(paraName, "X_ZYXEL_Prototrace")) {
			proto = json_object_get_boolean(json_object_object_get(tr369LocalAgentJobj, "X_ZYXEL_Prototrace"));
			json_object_object_add(pramJobj, paraName, json_object_new_boolean(proto));
		}
#endif
		else {
			json_object_object_add(pramJobj, paraName, JSON_OBJ_COPY(json_object_object_get(tr369LocalAgentJobj, paraName)));
		}

		paraName = (const char *)TR369_LOCAL_AGENT_param[n++].paraName;
	}

	json_object_put(tr369LocalAgentJobj);

	json_object_array_add(Jarray, pramJobj);

	return ret;
}


static zcfgRet_t zcfgFeDalTR369LocalAgentSet(struct json_object *paramObj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *paraName;
	objIndex_t objIid;
	struct json_object *tr369LocalAgentJobj = NULL;
	int paramModified = 0, n = 0;

	paraName = (const char *)TR369_LOCAL_AGENT_param[n++].paraName;

	IID_INIT(objIid);
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_LOCAL_AGENT, &objIid, &tr369LocalAgentJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object retrieve fail\n", __FUNCTION__);
		return ret;
	}

	while (paraName) {
		struct json_object *paramValue = json_object_object_get(paramObj, paraName);
		if (!paramValue) {
			paraName = (const char *)TR369_LOCAL_AGENT_param[n++].paraName;
			continue;
		}

		if (!strcmp(paraName, "EndpointID")) {
			const char *mode = json_object_get_string(paramValue);
			json_object_object_add(tr369LocalAgentJobj, "EndpointID", json_object_new_string(mode));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "X_ZYXEL_BoundInterface")) {
			const char *mode = json_object_get_string(paramValue);
			json_object_object_add(tr369LocalAgentJobj, "X_ZYXEL_BoundInterface", json_object_new_string(mode));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "X_ZYXEL_ActiveBoundInterface")) {
			const char *mode = json_object_get_string(paramValue);
			json_object_object_add(tr369LocalAgentJobj, "X_ZYXEL_ActiveBoundInterface", json_object_new_string(mode));
			paramModified = 1;
		}
#if 1//Jessie debug_on_off
		else if (!strcmp(paraName, "X_ZYXEL_Prototrace")) {
			const bool mode = json_object_get_boolean(paramValue);
			printf("%s() X_ZYXEL_Prototrace (mode) %d\n", __FUNCTION__, mode);
			json_object_object_add(tr369LocalAgentJobj, "X_ZYXEL_Prototrace", json_object_new_boolean(mode));
			paramModified = 1;
		}
#endif
		else {
			json_object_object_add(tr369LocalAgentJobj, paraName, JSON_OBJ_COPY(paramValue));
			paramModified = 1;
		}

		paraName = (const char *)TR369_LOCAL_AGENT_param[n++].paraName;
	}

	if (paramModified) {
		if((ret = zcfgFeObjJsonSet(RDM_OID_LOCAL_AGENT, &objIid, tr369LocalAgentJobj, NULL)) != ZCFG_SUCCESS)
			printf("%s: Write object fail\n", __FUNCTION__);
	}

	json_object_put(tr369LocalAgentJobj);

	return ret;
}


zcfgRet_t zcfgFeDalTR369LocalAgent(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if (!strcmp(method, "PUT")) {
		ret = zcfgFeDalTR369LocalAgentSet(Jobj, replyMsg);
	}
	else if (!strcmp(method, "POST")) {
		// ret = zcfgFeDalTR369LocalAgentSet(Jobj, replyMsg);
	}
	else if (!strcmp(method, "GET")) {
		ret = zcfgFeDalTR369LocalAgentGet(Jobj, Jarray, NULL);
	}
	else if (!strcmp(method, "DELETE")) {

	}

	return ret;
}
