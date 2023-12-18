
#include <json/json.h>

#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_dal_common.h"
#include "zlog_api.h"

dal_param_t LWM2M_param[] =
{
	{"Enable", dalType_boolean, 0, 0, NULL},
	{"Endpointname", dalType_string, 0, 256, NULL},//Username	
	{"LWM2M_Server_URI_0", dalType_string, 0, 256, NULL},
	{"BootstrapServer_1", dalType_boolean, 0, 0, NULL},
	{"Security_Mode_2", dalType_int, 0, 0, NULL},
	{"Public_Key_or_Identity_3", dalType_string, 0, 256, NULL},	//ConnectionRequestUsername
	{"Secret_Key_5", dalType_string, 0, 256, NULL},	//ConnectionRequestPassword
	{"Public_Key_or_Identity_MRD", dalType_string, 0, 256, NULL},	//Jessie0408
	{"Secret_Key_MRD", dalType_string, 0, 256, NULL},	//Jessie0408
	{"Public_Key_or_Identity_User", dalType_string, 0, 256, NULL},	//Jessie0408
	{"Secret_Key_User", dalType_string, 0, 256, NULL},	//Jessie0408
	{"Identity_Secret_Key_Used", dalType_int, 0, 0, NULL},	//Jessie0408
	{"Port", dalType_int, 0, 0, NULL},//X_ZYXEL_ConnectionRequestPort
	{"Lifetime", dalType_int, 0, 0, NULL},
	{"Lifetime_1", dalType_int, 0, 0, NULL},
	{"Short_Server_ID_10", dalType_int, 0, 0, NULL},
	{"X_ZYXEL_IPv4_Protocol", dalType_boolean, 0, 0, NULL},	
	{"X_ZYXEL_IPv6_Protocol", dalType_boolean, 0, 0, NULL},
	//{"DisplaySOAP", dalType_boolean, 0, 0, NULL},
	{"BoundInterface", dalType_string, 0, 256, NULL, "Any_WAN|Multi_WAN"},
	{"BoundInterfaceList", dalType_MultiRtWanWWan, 0, 256, NULL},
	{NULL, 0, 0, 0, NULL}
};

void zcfgFeDalShowLWM2M(struct json_object *Jarray){
	struct json_object *Jobj;
	const char *mode = NULL;
	char proto[32] = {0};
	bool v4Enable, v6Enable;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	Jobj = json_object_array_get_idx(Jarray, 0);
	#if 1
	v4Enable = json_object_get_boolean(json_object_object_get(Jobj, "X_ZYXEL_IPv4_Protocol"));
	v6Enable = json_object_get_boolean(json_object_object_get(Jobj, "X_ZYXEL_IPv6_Protocol"));
	if(true==v4Enable && true==v6Enable)
		strcpy(proto, "LWM2MClient on IPv6 Only");
	else if(v4Enable)
		strcpy(proto, "LWM2MClient on IPv4 Only");
	else if(v6Enable)
		strcpy(proto, "LWM2MClient on IPv6 Only");
	else
		strcpy(proto, "LWM2MClient on IPv6 Only");
	#endif
	mode = json_object_get_string(json_object_object_get(Jobj, "BoundInterface"));
	
	printf("LWM2M Client Information:\n");
	printf("%-45s %s\n", "Enable", json_object_get_string(json_object_object_get(Jobj, "Enable")));
	printf("%-45s %s\n", "IsBoostrapServer", json_object_get_string(json_object_object_get(Jobj, "BootstrapServer_1")));
	printf("%-45s %s\n", "Server URL", json_object_get_string(json_object_object_get(Jobj, "LWM2M_Server_URI_0")));
	printf("%-45s %s\n", "Security Mode", json_object_get_string(json_object_object_get(Jobj, "Security_Mode_2")));
	printf("%-45s %s\n", "PSK ID", json_object_get_string(json_object_object_get(Jobj, "Public_Key_or_Identity_3")));
	printf("%-45s %s\n", "PSK Secret", json_object_get_string(json_object_object_get(Jobj, "Secret_Key_5")));
	printf("%-45s %s\n", "Lifetime", json_object_get_string(json_object_object_get(Jobj, "Lifetime")));
	printf("%-45s %s\n", "Endpointname", json_object_get_string(json_object_object_get(Jobj, "Endpointname")));
	printf("%-45s %s\n", "Port", json_object_get_string(json_object_object_get(Jobj, "Port")));
	printf("%-45s %d\n", "IPv4 Protocol", v4Enable);
	printf("%-45s %d\n", "IPv6 Protocol", v6Enable);
	printf("%-45s %s\n", "IP Protocol Used", proto);
}

static zcfgRet_t zcfgFeDalLWM2MSet(struct json_object *paramObj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t objSecIid;
	struct json_object *secJobj = NULL;
	struct json_object *pramJobj = NULL;
	objIndex_t objZcfgIid;
	struct json_object *zcfgJobj = NULL;
	objIndex_t objServerIid;
	struct json_object *serverJobj = NULL;
	int paramModified = 0, n = 0;
	const char *paraName = NULL;
	int serverError = 0;

	paraName = (const char *)LWM2M_param[n++].paraName;

	IID_INIT(objSecIid);
	objSecIid.level = 1;
	objSecIid.idx[0] = 1;//only one right now
	if((ret = zcfgFeObjJsonGet(RDM_OID_LWM2M_SECURITY, &objSecIid, &secJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OID_LWM2M_SECURITY fail\n", __FUNCTION__);
		//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Object RDM_OID_LWM2M_SECURITY fail", __FUNCTION__);
		return ret;
	}
	
	IID_INIT(objServerIid);
	objServerIid.level = 1;
	objServerIid.idx[0] = 1;//only one right now
	if((ret = zcfgFeObjJsonGet(RDM_OID_LWM2M_SERVER, &objServerIid, &serverJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OID_LWM2M_SERVER fail\n", __FUNCTION__);
		//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Object RDM_OID_LWM2M_SERVER fail", __FUNCTION__);
		serverError = 1;
		//it is allowed that no server in the datamodel
		ret = ZCFG_SUCCESS;
	}
	
	IID_INIT(objZcfgIid);
	if((ret = zcfgFeObjJsonGet(RDM_OIDLWM2MCLIENT, &objZcfgIid, &zcfgJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OIDLWM2MCLIENT fail\n", __FUNCTION__);
		//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Object RDM_OIDLWM2MCLIENT fail", __FUNCTION__);
		json_object_put(secJobj);
		if(0 == serverError)
			json_object_put(serverJobj);
		return ret;
	}
	
	while(paraName) {
		struct json_object *paramValue = json_object_object_get(paramObj, paraName);
		if(!paramValue) {
			paraName = (const char *)LWM2M_param[n++].paraName;
			continue;
		}
		printf("%s() paraName=%s, paramValue=%s\n", __FUNCTION__, paraName, json_object_get_string(paramValue));
		//zcfgLogPrefix(ZCFG_LOG_INFO, ZCFG_LOGPRE_LWM2M, "%s() paraName=%s, paramValue=%s", __FUNCTION__, paraName, json_object_get_string(paramValue));
		if(!strcmp(paraName, "Enable") || 
		   !strcmp(paraName, "Endpointname") || 
		   !strcmp(paraName, "Port") || 
		   !strcmp(paraName, "Lifetime") || 
		   !strcmp(paraName, "X_ZYXEL_IPv4_Protocol") || 
		   !strcmp(paraName, "X_ZYXEL_IPv6_Protocol") || 
		   !strcmp(paraName, "Public_Key_or_Identity_MRD") || 
		   !strcmp(paraName, "Secret_Key_MRD") || 
		   !strcmp(paraName, "Public_Key_or_Identity_User") || 
		   !strcmp(paraName, "Secret_Key_User") ||
		   !strcmp(paraName, "Identity_Secret_Key_Used")) {
			json_object_object_add(zcfgJobj, paraName, JSON_OBJ_COPY(paramValue));	
			paramModified = 1;			
		} else if(!strcmp(paraName, "LWM2M_Server_URI_0") || !strcmp(paraName, "BootstrapServer_1") || !strcmp(paraName, "Security_Mode_2") || !strcmp(paraName, "Public_Key_or_Identity_3") || !strcmp(paraName, "Secret_Key_5")) {
			//Check if there exists a lwm2m server object whose short server ID matches the one in lwm2m security object
			//if no, create a new lwm2m server object
			if(!strcmp(paraName, "BootstrapServer_1")){
				printf("%s:%d BootstrapServer_1 value is changed to %s from %s.\n", __func__, __LINE__, json_object_get_string(paramValue), json_object_get_string(json_object_object_get(secJobj, "BootstrapServer_1")));

				if(strcmp(json_object_get_string(json_object_object_get(secJobj, "BootstrapServer_1")), "true") == 0 && strcmp(json_object_get_string(paramValue), "false") == 0){

					struct json_object *serverTempJobj = NULL;
					objIndex_t objTempServerIid;
					int serverIndex = 0;
					bool foundShortServerIDMatch = false;
					const char *shortServerID = json_object_get_string(json_object_object_get(secJobj, "Short_Server_ID_10"));
					printf("%s:%d shortServerID %s in the /0 lwm2m security object.\n", __func__, __LINE__, shortServerID);

					IID_INIT(objTempServerIid);
					while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_LWM2M_SERVER, &objTempServerIid, &serverTempJobj) == ZCFG_SUCCESS){
					//set to Management server from bootstrap server
						printf("%s:%d Now searching sever(idx:%d) shortServerID %d.\n", __func__, __LINE__, serverIndex++, json_object_get_string(json_object_object_get(serverTempJobj, "Short_Server_ID_0")));
						if(strcmp(shortServerID, json_object_get_string(json_object_object_get(serverTempJobj, "Short_Server_ID_0"))) == 0){
							printf("%s:%d Found short ServerID match!.\n", __func__, __LINE__);
							foundShortServerIDMatch = true;
						}
					}

					if(!foundShortServerIDMatch){
						printf("%s:%d Not found short ServerID match! Create One lwm2m server object.\n", __func__, __LINE__);
						IID_INIT(objTempServerIid);
						if((ret = zcfgFeObjJsonAdd(RDM_OID_LWM2M_SERVER, &objTempServerIid, NULL)) != ZCFG_SUCCESS) {
							printf("%s: fail to add lwm2m server with ret = %d\n", __FUNCTION__, ret);
							goto Exit;
						}

						if((ret = zcfgFeObjJsonGet(RDM_OID_LWM2M_SERVER, &objTempServerIid, &serverTempJobj)) != ZCFG_SUCCESS) {
							printf("%s: Fail to get lwm2m server with ret = %d\n", __FUNCTION__, ret);
							goto Exit;
						}
						json_object_object_add(serverTempJobj, "Short_Server_ID_0", json_object_new_string(shortServerID));
						json_object_object_add(serverTempJobj, "Lifetime_1", JSON_OBJ_COPY(json_object_object_get(zcfgJobj, "Lifetime")));
						printf("%s:%d Now set new server object with serverID %s.\n", __func__, __LINE__, json_object_get_string(json_object_object_get(serverTempJobj, "Short_Server_ID_0")));
						printf("%s:%d Now set new server object with lifttime %s.\n", __func__, __LINE__, json_object_get_string(json_object_object_get(serverTempJobj, "Lifetime_1")));

						if((ret = zcfgFeObjJsonSet(RDM_OID_LWM2M_SERVER, &objTempServerIid, serverTempJobj, NULL)) != ZCFG_SUCCESS)
						{
							dbg_printf("%s: Set lwm2m server object fail with ret = %d\n", __FUNCTION__, ret);
							goto Exit;
						}
					}
				}
			}
			//end of creating new lwm2m server object

			json_object_object_add(secJobj, paraName, JSON_OBJ_COPY(paramValue));	
			paramModified = 1;			
		} else if(!strcmp(paraName, "Lifetime_1")) {
			if(0 == serverError){
				json_object_object_add(serverJobj, "Lifetime_1", JSON_OBJ_COPY(paramValue));
				paramModified = 1;
			}
		}

		paraName = (const char *)LWM2M_param[n++].paraName;
	}
	
	
	if(paramModified) {
		if(0 == serverError){
			if((ret = zcfgFeObjJsonSet(RDM_OID_LWM2M_SERVER, &objServerIid, serverJobj, NULL)) != ZCFG_SUCCESS){
				printf("%s: Write RDM_OID_LWM2M_SERVER object fail\n", __FUNCTION__);
				//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Write Object RDM_OID_LWM2M_SERVER fail", __FUNCTION__);
			}
		}
		//NOTE: beLwm2mStart() in called from RDM_OID_LWM2M_SECURITY, so RDM_OID_LWM2M_SECURITY will be set at last so that beLwm2mStart would get the latest value for other RDM_OID.
		if((ret = zcfgFeObjJsonSet(RDM_OID_LWM2M_SECURITY, &objSecIid, secJobj, NULL)) != ZCFG_SUCCESS){
			printf("%s: Write RDM_OID_LWM2M_SECURITY object fail\n", __FUNCTION__);
			//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Write Object RDM_OID_LWM2M_SECURITY fail", __FUNCTION__);
		}
		if((ret = zcfgFeObjJsonSet(RDM_OIDLWM2MCLIENT, &objZcfgIid, zcfgJobj, NULL)) != ZCFG_SUCCESS){
			printf("%s: Write RDM_OIDLWM2MCLIENT object fail\n", __FUNCTION__);
			//zcfgLogPrefix(ZCFG_LOG_ERR, ZCFG_LOGPRE_LWM2M, "%s() Write Object RDM_OIDLWM2MCLIENT fail", __FUNCTION__);
		}
	}

Exit:
	json_object_put(secJobj);
	if(0== serverError)
		json_object_put(serverJobj);
	json_object_put(zcfgJobj);

	return ret;
}

static zcfgRet_t zcfgFeDalLWM2MGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *paraName;
	objIndex_t objSecIid;
	struct json_object *secJobj = NULL;
	struct json_object *pramJobj = NULL;
	objIndex_t objZcfgIid;
	struct json_object *zcfgJobj = NULL;
	objIndex_t objServerIid;
	struct json_object *serverJobj = NULL;
	bool enable;
	int intVal;
	const char *str;
	int n = 0;
	int serverError = 0;
	// paraName = (const char *)LWM2M_param[n++].paraName;


	// yuchih@2021/10/26: get multiple instances for security object, server object, and lwm2m client global zcfg conf(enable, endpointname)
	int count = 0;  // index for security object

	// get lwm2m client enable/disable, endpoint name..., etc.
	IID_INIT(objZcfgIid);
	if((ret = zcfgFeObjJsonGet(RDM_OIDLWM2MCLIENT, &objZcfgIid, &zcfgJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OIDLWM2MCLIENT fail\n", __FUNCTION__);
		return ret;
	}

	IID_INIT(objSecIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_LWM2M_SECURITY, &objSecIid, &secJobj) == ZCFG_SUCCESS) {
		count++;
		serverError = 0;//reset serverError

		pramJobj = json_object_new_object();

		//====================================================================================================================================
		// Security object resource
		n = 0;
		paraName = (const char *)LWM2M_param[n++].paraName;
		while(paraName) {
			if( !strcmp(paraName, "LWM2M_Server_URI_0") ||
				!strcmp(paraName, "BootstrapServer_1") ||
				!strcmp(paraName, "Security_Mode_2") ||
				!strcmp(paraName, "Public_Key_or_Identity_3") ||
				!strcmp(paraName, "Secret_Key_5") ||
				!strcmp(paraName, "Short_Server_ID_10")) {
				json_object_object_add(pramJobj, paraName, JSON_OBJ_COPY(json_object_object_get(secJobj, paraName)));
			}

			paraName = (const char *)LWM2M_param[n++].paraName;
		}

		IID_INIT(objServerIid);
		objServerIid.level = 1;
		objServerIid.idx[0] = objSecIid.idx[0];
		if((ret = zcfgFeObjJsonGet(RDM_OID_LWM2M_SERVER, &objServerIid, &serverJobj)) != ZCFG_SUCCESS) {
			printf("%s: Object RDM_OID_LWM2M_SERVER fail\n", __FUNCTION__);
			serverError = 1;
			//it is allowed that no server in the datamodel
			ret = ZCFG_SUCCESS;
		}

		//====================================================================================================================================
		// Server object resource
		n = 0;
		paraName = (const char *)LWM2M_param[n++].paraName;
		while(paraName) {
			if(!strcmp(paraName, "Lifetime_1")) {
				if(0 == serverError){
					json_object_object_add(pramJobj, "Lifetime_1", JSON_OBJ_COPY(json_object_object_get(serverJobj, paraName)));
				}
			}

			paraName = (const char *)LWM2M_param[n++].paraName;
		}

		//====================================================================================================================================
		// lwm2m client object
		n = 0;
		paraName = (const char *)LWM2M_param[n++].paraName;
		while(paraName) {
			if( !strcmp(paraName, "Enable") ||
				!strcmp(paraName, "Endpointname") ||
				!strcmp(paraName, "Port") ||
				!strcmp(paraName, "X_ZYXEL_IPv4_Protocol") ||
				!strcmp(paraName, "X_ZYXEL_IPv6_Protocol") ||
				!strcmp(paraName, "Lifetime") ||
				!strcmp(paraName, "Public_Key_or_Identity_MRD") ||//Jessie0408
				!strcmp(paraName, "Secret_Key_MRD") ||//Jessie0408
				!strcmp(paraName, "Public_Key_or_Identity_User") ||//Jessie0408
				!strcmp(paraName, "Secret_Key_User") ||//Jessie0408
				!strcmp(paraName, "Identity_Secret_Key_Used")) {//Jessie0408
				json_object_object_add(pramJobj, paraName, JSON_OBJ_COPY(json_object_object_get(zcfgJobj, paraName)));
			}

			paraName = (const char *)LWM2M_param[n++].paraName;
		}

		zcfgFeJsonObjFree(secJobj);
		zcfgFeJsonObjFree(serverJobj);

		json_object_array_add(Jarray, pramJobj);

	} // end of while(zcfgFeObjJsonGetNext(RDM_OID_LWM2M_SECURITY, &objSecIid, &secJobj) == ZCFG_SUCCESS)

	zcfgFeJsonObjFree(zcfgJobj);

	return ret;
}

zcfgRet_t zcfgFeDalLWM2MDel(struct json_object *Jobj, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t lwm2mSecurityIid = {0};
	objIndex_t lwm2mServerIid = {0};
	int index = 0;

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	// Delete security instance
	ret = convertIndextoIid(index, &lwm2mSecurityIid, RDM_OID_LWM2M_SECURITY, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
	{
		return ret;
	}

	//Delete server instance same as security
	memcpy(&lwm2mServerIid, &lwm2mSecurityIid, sizeof(objIndex_t));
	if(zcfgFeObjJsonDel(RDM_OID_LWM2M_SERVER, &lwm2mServerIid, NULL) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OID_LWM2M_SERVER fail %d\n", __FUNCTION__, lwm2mServerIid.idx[lwm2mServerIid.level - 1]);
		//it is allowed that server & security out of sync
	}

	ret = zcfgFeObjJsonDel(RDM_OID_LWM2M_SECURITY, &lwm2mSecurityIid, NULL);

	return ret;
}


zcfgRet_t zcfgFeDalLWM2M(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "PUT")) {
		ret = zcfgFeDalLWM2MSet(Jobj, replyMsg);
	}
	else if(!strcmp(method, "POST")) {
		ret = zcfgFeDalLWM2MSet(Jobj, replyMsg);
	}
	else if(!strcmp(method, "GET")) {
		ret = zcfgFeDalLWM2MGet(Jobj, Jarray, NULL);
	}
	else if(!strcmp(method, "DELETE")) {
		ret = zcfgFeDalLWM2MDel(Jobj, NULL);
	}

	return ret;
}
