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


dal_param_t L2TPv3_param[]={
	{"Index", 			dalType_int,	0,	0,		NULL,	NULL,	dal_Edit},
	{"Enable",              	dalType_boolean,    0,  0,  NULL},
	{"LocalIPAddress", 			dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"LocalTunnelAddress", 			dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"RemoteIPAddress", 		dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"RemoteTunnelAddress", 		dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"LocalTunnelID", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"RemoteTunnelID", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"LocalSessionID", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"RemoteSessionID", 		dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"SourcePort", 				dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"DestinationPort", 		dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{NULL,						0,				0,	0,		NULL,	NULL,	NULL}
};

struct json_object *L2TPv3Obj;

objIndex_t L2TPv3Iid = {0};

bool g_enable = false;
const char* g_LocalIPAddress = NULL;
const char* g_LocalTunnelAddress = NULL;
const char* g_RemoteIPAddress = NULL;
const char* g_RemoteTunnelAddress = NULL;
int g_LocalTunnelID = 0;
int g_RemoteTunnelID = 0;
int g_LocalSessionID = 0;
int g_RemoteSessionID = 0;
int g_SourcePort = 0;
int g_DestinationPort = 0;

void initL2TPv3EntryGlobalObjects(){
	L2TPv3Obj = NULL;
	return;
}

void freeAllL2TPv3Objects(){
	if(L2TPv3Obj) {
		json_object_put(L2TPv3Obj);
		L2TPv3Obj = NULL;
	}

	return;
}

void getL2TPv3BasicInfo(struct json_object *Jobj){

	g_enable = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	printf("g_enable=%s\n", g_enable ? "true":"false");
	
	g_LocalIPAddress = json_object_get_string(json_object_object_get(Jobj, "LocalIPAddress"));
	if(g_LocalIPAddress)
		printf("g_LocalIPAddress=%s\n", g_LocalIPAddress);

	g_LocalTunnelAddress = json_object_get_string(json_object_object_get(Jobj, "LocalTunnelAddress"));
	if(g_LocalTunnelAddress)
		printf("g_LocalTunnelAddress=%s\n", g_LocalTunnelAddress);

	g_RemoteIPAddress = json_object_get_string(json_object_object_get(Jobj, "RemoteIPAddress"));
	if(g_RemoteIPAddress)
		printf("g_RemoteIPAddress=%s\n", g_RemoteIPAddress);

	g_RemoteTunnelAddress = json_object_get_string(json_object_object_get(Jobj, "RemoteTunnelAddress"));
	if(g_RemoteTunnelAddress)
		printf("g_RemoteTunnelAddress=%s\n", g_RemoteTunnelAddress);

	g_LocalTunnelID = json_object_get_int(json_object_object_get(Jobj, "LocalTunnelID"));
	printf("g_LocalTunnelID=%d\n", g_LocalTunnelID);
	
	g_RemoteTunnelID = json_object_get_int(json_object_object_get(Jobj, "RemoteTunnelID"));
	printf("g_RemoteTunnelID=%d\n", g_RemoteTunnelID);
	
	g_LocalSessionID = json_object_get_int(json_object_object_get(Jobj, "LocalSessionID"));
	printf("g_LocalSessionID=%d\n", g_LocalSessionID);
	
	g_RemoteSessionID = json_object_get_int(json_object_object_get(Jobj, "RemoteSessionID"));
	printf("g_RemoteSessionID=%d\n", g_RemoteSessionID);

	g_SourcePort = json_object_get_int(json_object_object_get(Jobj, "SourcePort"));
	printf("g_SourcePort=%d\n", g_SourcePort);

	g_DestinationPort = json_object_get_int(json_object_object_get(Jobj, "DestinationPort"));
	printf("g_DestinationPort=%d\n", g_DestinationPort);

	return;
}

void zcfgFeDalShowL2TPv3(struct json_object *Jarray){
	struct json_object *obj = NULL;
	int i = 0, len = 0;
	
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	len = json_object_array_length(Jarray);

	printf("L2TPv3 Information:\n");
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		printf("APN No.%d:\n", (i + 1));
		printf("Enable               : %s\n",json_object_get_string(json_object_object_get(obj, "Enable")));
		printf("LocalIPAddress       = %s\n",json_object_get_string(json_object_object_get(obj, "LocalIPAddress")));
		printf("LocalTunnelAddress   = %s\n",json_object_get_string(json_object_object_get(obj, "LocalTunnelAddress")));
		printf("RemoteIPAddress      = %s\n",json_object_get_string(json_object_object_get(obj, "RemoteIPAddress")));
		printf("RemoteTunnelAddress  = %s\n",json_object_get_string(json_object_object_get(obj, "RemoteTunnelAddress")));
		printf("LocalTunnelID        = %s\n",json_object_get_string(json_object_object_get(obj, "LocalTunnelID")));
		printf("RemoteTunnelID       = %s\n",json_object_get_string(json_object_object_get(obj, "RemoteTunnelID")));
		printf("LocalSessionID       = %s\n",json_object_get_string(json_object_object_get(obj, "LocalSessionID")));
		printf("RemoteSessionID      = %s\n",json_object_get_string(json_object_object_get(obj, "RemoteSessionID")));
		printf("SourcePort           = %s\n",json_object_get_string(json_object_object_get(obj, "SourcePort")));
		printf("DestinationPort      = %s\n\n",json_object_get_string(json_object_object_get(obj, "DestinationPort")));
	}
}

zcfgRet_t zcfgFeDalL2TPv3ObjEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_L2_T_PV3, &L2TPv3Iid, &L2TPv3Obj)) != ZCFG_SUCCESS){
		printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get L2TPv3 object fail oid(%d)\n ",__FUNCTION__,RDM_OID_L2_T_PV3);
		return ZCFG_INTERNAL_ERROR;
	}

	if(L2TPv3Obj != NULL){
		if(json_object_object_get(Jobj, "Enable"))
			json_object_object_add(L2TPv3Obj, "Enable", json_object_new_boolean(g_enable));
		if(json_object_object_get(Jobj, "LocalIPAddress"))
			json_object_object_add(L2TPv3Obj, "LocalIPAddress", json_object_new_string(g_LocalIPAddress));
		if(json_object_object_get(Jobj, "LocalTunnelAddress"))
			json_object_object_add(L2TPv3Obj, "LocalTunnelAddress", json_object_new_string(g_LocalTunnelAddress));
		if(json_object_object_get(Jobj, "RemoteIPAddress"))
			json_object_object_add(L2TPv3Obj, "RemoteIPAddress", json_object_new_string(g_RemoteIPAddress));
		if(json_object_object_get(Jobj, "RemoteTunnelAddress"))
			json_object_object_add(L2TPv3Obj, "RemoteTunnelAddress", json_object_new_string(g_RemoteTunnelAddress));
		if(json_object_object_get(Jobj, "LocalTunnelID"))
			json_object_object_add(L2TPv3Obj, "LocalTunnelID", json_object_new_int(g_LocalTunnelID));
		if(json_object_object_get(Jobj, "RemoteTunnelID"))
			json_object_object_add(L2TPv3Obj, "RemoteTunnelID", json_object_new_int(g_RemoteTunnelID));
		if(json_object_object_get(Jobj, "LocalSessionID"))
			json_object_object_add(L2TPv3Obj, "LocalSessionID", json_object_new_int(g_LocalSessionID));
		if(json_object_object_get(Jobj, "RemoteSessionID"))
			json_object_object_add(L2TPv3Obj, "RemoteSessionID", json_object_new_int(g_RemoteSessionID));
		if(json_object_object_get(Jobj, "SourcePort"))
			json_object_object_add(L2TPv3Obj, "SourcePort", json_object_new_int(g_SourcePort));
		if(json_object_object_get(Jobj, "DestinationPort"))
			json_object_object_add(L2TPv3Obj, "DestinationPort", json_object_new_int(g_DestinationPort));
	}
	
	return ret;
}

zcfgRet_t zcfgFeDalL2TPv3Edit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	IID_INIT(L2TPv3Iid);
	int index = 0;

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	initL2TPv3EntryGlobalObjects();

	getL2TPv3BasicInfo(Jobj);
	L2TPv3Iid.level = 1;
	ret = convertIndextoIid(index, &L2TPv3Iid, RDM_OID_L2_T_PV3, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;


	if((ret = zcfgFeDalL2TPv3ObjEdit(Jobj)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to edit L2TPv3 Entry \n", __FUNCTION__);
		goto exit;
	}

	if((ret = zcfgFeObjJsonSet(RDM_OID_L2_T_PV3, &L2TPv3Iid, L2TPv3Obj, NULL)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to set object %d RDM_OID_L2_T_PV3\n", __FUNCTION__, RDM_OID_L2_T_PV3);
	}

exit:
	freeAllL2TPv3Objects();
	return ret;
}

zcfgRet_t zcfgFeDalL2TPv3Get(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL;
	struct json_object *L2TPv3Jobj = NULL;
	objIndex_t L2TPv3objIid = {0};

	IID_INIT(L2TPv3objIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_L2_T_PV3, &L2TPv3objIid, &L2TPv3Jobj) == ZCFG_SUCCESS){

		paramJobj = json_object_new_object();
		json_object_object_add(paramJobj, "Enable", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "Enable")));
		json_object_object_add(paramJobj, "LocalIPAddress", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "LocalIPAddress")));
		json_object_object_add(paramJobj, "LocalTunnelAddress", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "LocalTunnelAddress")));
		json_object_object_add(paramJobj, "RemoteIPAddress", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "RemoteIPAddress")));
		json_object_object_add(paramJobj, "RemoteTunnelAddress", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "RemoteTunnelAddress")));
		json_object_object_add(paramJobj, "LocalTunnelID", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "LocalTunnelID")));
		json_object_object_add(paramJobj, "RemoteTunnelID", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "RemoteTunnelID")));
		json_object_object_add(paramJobj, "LocalSessionID", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "LocalSessionID")));
		json_object_object_add(paramJobj, "RemoteSessionID", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "RemoteSessionID")));
		json_object_object_add(paramJobj, "SourcePort", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "SourcePort")));
		json_object_object_add(paramJobj, "DestinationPort", JSON_OBJ_COPY(json_object_object_get(L2TPv3Jobj, "DestinationPort")));
		json_object_array_add(Jarray, paramJobj);
	
		zcfgFeJsonObjFree(L2TPv3Jobj);
	}
	
	return ret;
}


zcfgRet_t zcfgFeDalL2TPv3Add(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	//Fixed fileds, no need to add new object
	return ret;
}

zcfgRet_t zcfgFeDalL2TPv3Delete(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	//Fixed fileds, no need to add new object
	return ret;
}

zcfgRet_t zcfgFeDalL2TPv3(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	if(!strcmp(method, "PUT")){
		ret = zcfgFeDalL2TPv3Edit(Jobj, replyMsg);
	}
	else if(!strcmp(method, "POST")) {
		ret = zcfgFeDalL2TPv3Add(Jobj, replyMsg);
	}
	else if(!strcmp(method, "DELETE")) {
		ret = zcfgFeDalL2TPv3Delete(Jobj, replyMsg);
	}
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalL2TPv3Get(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}
