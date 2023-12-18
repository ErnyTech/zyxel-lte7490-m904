#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
#include <time.h>
#include <string.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_Qos.h"

typedef struct s_QoS {
	bool X_ZYXEL_Enabled;
	char *X_ZYXEL_AutoMapType;
	uint32_t X_ZYXEL_UpRate;
	uint32_t X_ZYXEL_DownRate;
}s_QoS;

dal_param_t QosGeneral_param[] = {
	{"Enable",						dalType_boolean, 	0, 0, 			NULL,		NULL,												NULL},
	{"AutoMapType",					dalType_string, 	0, 0, 			NULL,		"None|EthernetPriority|IPPrecedence|PacketLength|Blocked",	NULL},
	{"UpRate",						dalType_int, 		0, 1024000, 	NULL,		NULL,												NULL},
	{"DownRate",					dalType_int, 		0, 1024000, 	NULL,		NULL,												NULL},
	{NULL,							0,					0, 0,			NULL,		NULL,												NULL},
};

void getQosGeneralBasicInfo(struct json_object *Jobj, s_QoS *QoS_Info) {
	QoS_Info->X_ZYXEL_Enabled = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	QoS_Info->X_ZYXEL_AutoMapType = (char*)json_object_get_string(json_object_object_get(Jobj, "AutoMapType"));
	if(json_object_object_get(Jobj, "UpRate") != NULL)
		QoS_Info->X_ZYXEL_UpRate = json_object_get_int(json_object_object_get(Jobj, "UpRate"));
	if(json_object_object_get(Jobj, "DownRate") != NULL)
		QoS_Info->X_ZYXEL_DownRate = json_object_get_int(json_object_object_get(Jobj, "DownRate"));
	return;
}


void zcfgFeDalShowQosGeneral(struct json_object *Jarray){
	struct json_object *obj;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	obj = json_object_array_get_idx(Jarray, 0);
		printf("Qos General Configuration\n");
		printf("%-50s %d \n","Qos :", json_object_get_boolean(json_object_object_get(obj, "Enable")));
		printf("%-50s %s \n","WAN Managed Upstream Bandwidth :", json_object_get_string(json_object_object_get(obj, "UpRate")));
		printf("%-50s %s \n","LAN Managed Downstream Bandwidth :", json_object_get_string(json_object_object_get(obj, "DownRate")));
		printf("%-50s %s \n","Upstream Traffic Priority Assigned by :", json_object_get_string(json_object_object_get(obj, "AutoMapType")));
	}

zcfgRet_t zcfgFeDalQosGeneralGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	struct json_object *obj = NULL;
	struct json_object *pramJobj = NULL;
	const char* AutoMapType;

	IID_INIT(iid);
	if(zcfgFeObjJsonGet(RDM_OID_QOS, &iid, &obj) == ZCFG_SUCCESS) {
		pramJobj = json_object_new_object();
		json_object_object_add(pramJobj, "Enable", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_Enabled")));
		AutoMapType = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_AutoMapType"));
		if(!strcmp(AutoMapType,"802.1P"))
			json_object_object_add(pramJobj, "AutoMapType", json_object_new_string("Ethernet Priority"));
		else if(!strcmp(AutoMapType,"DSCP"))
			json_object_object_add(pramJobj, "AutoMapType", json_object_new_string("IP Precedence"));
		else if(!strcmp(AutoMapType,"Packet_Length"))
			json_object_object_add(pramJobj, "AutoMapType", json_object_new_string("Packet Length"));
		else if(!strcmp(AutoMapType,"Blocked")) // Jessie Lu@2020.04.09: Disable forwarding
			json_object_object_add(pramJobj, "AutoMapType", json_object_new_string("Blocked"));
		else		
			json_object_object_add(pramJobj, "AutoMapType", json_object_new_string(AutoMapType));
		json_object_object_add(pramJobj, "UpRate", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_UpRate")));
		json_object_object_add(pramJobj, "DownRate", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_DownRate")));
		json_object_array_add(Jarray, pramJobj);
		zcfgFeJsonObjFree(obj);
	}
	
	return ret;
}

zcfgRet_t zcfgFeDalQosGeneralEdit(struct json_object *Jobj, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;
	s_QoS QoS_Info;
	json_object *qosObj = NULL;
	objIndex_t qosIid = {0};
	IID_INIT(qosIid);

	memset(&QoS_Info, 0, sizeof(s_QoS));
	getQosGeneralBasicInfo(Jobj, &QoS_Info);
	ret = zcfgFeObjJsonGet(RDM_OID_QOS, &qosIid, &qosObj);	
	if(ret == ZCFG_SUCCESS) {
		if(json_object_object_get(Jobj, "Enable")!= NULL)
			json_object_object_add(qosObj, "X_ZYXEL_Enabled", json_object_new_boolean(QoS_Info.X_ZYXEL_Enabled));
		if(json_object_object_get(Jobj, "AutoMapType")!= NULL){
			if(!strcmp(QoS_Info.X_ZYXEL_AutoMapType,"EthernetPriority"))
				json_object_object_add(qosObj, "X_ZYXEL_AutoMapType", json_object_new_string("802.1P"));
			else if(!strcmp(QoS_Info.X_ZYXEL_AutoMapType,"IPPrecedence"))
				json_object_object_add(qosObj, "X_ZYXEL_AutoMapType", json_object_new_string("DSCP"));
			else if(!strcmp(QoS_Info.X_ZYXEL_AutoMapType,"PacketLength"))
				json_object_object_add(qosObj, "X_ZYXEL_AutoMapType", json_object_new_string("Packet_Length"));
			else if(!strcmp(QoS_Info.X_ZYXEL_AutoMapType,"Blocked")) // Jessie Lu@2020.04.09: Disable forwarding
				json_object_object_add(qosObj, "X_ZYXEL_AutoMapType", json_object_new_string("Blocked"));
			else
				json_object_object_add(qosObj, "X_ZYXEL_AutoMapType", json_object_new_string(QoS_Info.X_ZYXEL_AutoMapType));
		}
		if(json_object_object_get(Jobj, "UpRate")!= NULL)
			json_object_object_add(qosObj, "X_ZYXEL_UpRate", json_object_new_int(QoS_Info.X_ZYXEL_UpRate));
		if(json_object_object_get(Jobj, "DownRate")!= NULL)
			json_object_object_add(qosObj, "X_ZYXEL_DownRate", json_object_new_int(QoS_Info.X_ZYXEL_DownRate));
		zcfgFeObjJsonBlockedSet(RDM_OID_QOS, &qosIid, qosObj, NULL);
		zcfgFeJsonObjFree(qosObj);
	}

	else{
		printf("%s: Object retrieve fail\n", __FUNCTION__);
		return ret;
	}

	return ret;
}

zcfgRet_t zcfgFeDalQosGeneral(const char *method, struct json_object *Jobj, struct json_object *Jarray,char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;
	
	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalQosGeneralEdit(Jobj, replyMsg);
	else if(!strcmp(method, "GET")) 
		ret = zcfgFeDalQosGeneralGet(Jobj, Jarray, NULL);
	else 
		printf("Unknown method:%s\n", method);


	return ret;
}
