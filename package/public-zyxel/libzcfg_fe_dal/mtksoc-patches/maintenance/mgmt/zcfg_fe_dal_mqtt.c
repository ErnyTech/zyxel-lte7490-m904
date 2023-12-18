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

dal_param_t DEV_MQTT_NODE_param[] = {
        {"X_ZYXEL_BoundIfList",                 dalType_string, 0,      0,      NULL,   NULL,   NULL},
	{NULL,	0,	0,	0,	NULL,	NULL,	NULL}
};


dal_param_t DEV_MQTT_CLIENT_param[]={
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
	{"Index", 					dalType_int, 	0, 	0,	 	NULL,	NULL,	dal_Edit | dal_Delete | dal_Publish},
#else
	{"Index", 					dalType_int, 	0, 	0,	 	NULL,	NULL,	dal_Edit | dal_Delete},
#endif
	{"Enable", 					dalType_boolean,0,	0,		NULL,	NULL,	dal_Add},
	{"BrokerAddress", 			dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"BrokerPort", 				dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_QoS", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	//{"WillEnable", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	//{"WillTopic", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	//{"WillValue", 			dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"ClientID", 		        dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"TransportProtocol", 		dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"ForceReconnect", 			dalType_boolean,0,	0,		NULL,	NULL,	NULL},
	//{"KeepAliveTime", 		dalType_int,	0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_Certificate", 	dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"X_ZYXEL_CA_Certificate", 	dalType_string,	0,	0,		NULL,	NULL,	NULL},
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
	{"Topic", 	dalType_string,	0,	0,		NULL,	NULL,	dal_Publish},
	{"Message", 	dalType_string,	0,	0,		NULL,	NULL,	dal_Publish},
#endif
	{"Username", 	dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"Password", 	dalType_string,	0,	0,		NULL,	NULL,	NULL},
	{"ProtocolVersion", 	dalType_string,	0,	0,		NULL,	NULL,	NULL},	
	{NULL,						0,				0,	0,		NULL,	NULL,	NULL}
};

struct json_object *mqttClientObj;
objIndex_t mqttClientIid = {0};
bool g_enable;
const char* g_brokerAddr;
const char* g_brokerPort;
int g_X_ZYXEL_QoS;
bool g_will_enable;
const char* g_will_topic;
const char* g_will_value;
const char* g_transport_protocol;
const char* g_client_id;
bool g_force_reconnect;
int g_keepalivetime;
const char* Iface;
const char* g_certificate;
const char* g_ca_certificate;
const char* g_username;
const char* g_password;
const char* g_protocol_version;
void initMqttEntryGlobalObjects(){
	mqttClientObj = NULL;
	return;
}

void freeAllmqttClientObjects(){
	if(mqttClientObj) {
		json_object_put(mqttClientObj);
		mqttClientObj = NULL;
	}
	return ;
}

void getMqttBasicInfo(struct json_object *Jobj){
	g_enable = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	printf("g_enable=%d\n", g_enable);
	
	g_brokerAddr = json_object_get_string(json_object_object_get(Jobj, "BrokerAddress"));
	if(g_brokerAddr)
		printf("g_brokerAddr=%s\n", g_brokerAddr);
	
	g_brokerPort = json_object_get_int(json_object_object_get(Jobj, "BrokerPort"));
		printf("g_brokerPort=%d\n", g_brokerPort);
	
	g_X_ZYXEL_QoS = json_object_get_int(json_object_object_get(Jobj, "X_ZYXEL_QoS"));
	printf("g_X_ZYXEL_QoS=%d\n", g_X_ZYXEL_QoS);

	/*
	g_will_enable = json_object_get_boolean(json_object_object_get(Jobj, "WillEnable"));
	printf("g_will_enable=%d\n", g_will_enable);
	
	g_will_topic = json_object_get_string(json_object_object_get(Jobj, "WillTopic"));
	if(g_will_topic)
		printf("g_will_topic=%s\n", g_will_topic);
	
	g_will_value = json_object_get_string(json_object_object_get(Jobj, "WillValue"));
	if(g_will_value)
		printf("g_will_value=%s\n", g_will_value);
		*/
	g_transport_protocol = json_object_get_string(json_object_object_get(Jobj, "TransportProtocol"));
	if(g_transport_protocol){
		printf("g_transport_protocol=%s\n", g_transport_protocol);
	}
	
	g_client_id = json_object_get_string(json_object_object_get(Jobj, "ClientID"));
	if(g_client_id){
		printf("g_client_id=%s\n", g_client_id);
	}

	g_force_reconnect = json_object_get_boolean(json_object_object_get(Jobj, "ForceReconnect"));
	printf("g_force_reconnect=%d\n", g_force_reconnect);
	/*
	g_keepalivetime = json_object_get_int(json_object_object_get(Jobj, "KeepAliveTime"));
	printf("g_keepalivetime=%d\n", g_keepalivetime);
	*/
	
	//X_ZYXEL_Certificate
	g_certificate = json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_Certificate"));
	if(g_certificate){
		printf("g_certificate=%s\n", g_certificate);
	}
	
	//X_ZYXEL_CA_Certificate
	g_ca_certificate = json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_CA_Certificate"));
	if(g_ca_certificate){
		printf("g_ca_certificate=%s\n", g_ca_certificate);
	}

	//Username
	g_username = json_object_get_string(json_object_object_get(Jobj, "Username"));
	if(g_username){
		printf("g_username=%s\n", g_username);
	}

	//Password
	g_password = json_object_get_string(json_object_object_get(Jobj, "Password"));
	if(g_password){
		printf("g_password=%s\n", g_password);
	}

	//ProtocolVersion
	g_protocol_version = json_object_get_string(json_object_object_get(Jobj, "ProtocolVersion"));
	if(g_protocol_version){
		printf("g_protocol_version=%s\n", g_protocol_version);
	}

	return;
}
void zcfgFeDalShowMQTT(struct json_object *Jarray){
	int i, len = 0;
	struct json_object *obj = NULL;
	int force_reconnect = 0;
	
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-5s %-6s %-32s %-10s %-17s %-14s %-12s %-16s %-10s %-10s %-10s %-10s %-10s\n",
			"Index", "Enable", "BrokerAddress", "BrokerPort", "TransportProtocol", "ForceReconnect", "X_ZYXEL_QoS", "ClientID", "Certificate", "CA", "Username", "Password", "ProtocolVersion");
	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray,i);
		printf("%-5s %-6s %-32s %-10s %-17s %-14s %-12s %-16s %-10s %-10s %-10s %-10s %-10s\n",
			json_object_get_string(json_object_object_get(obj, "Index")),
			json_object_get_string(json_object_object_get(obj, "Enable")),
			json_object_get_string(json_object_object_get(obj, "BrokerAddress")),
			json_object_get_string(json_object_object_get(obj, "BrokerPort")),
			json_object_get_string(json_object_object_get(obj, "TransportProtocol")),
			force_reconnect?"true":"false",//always show false
			json_object_get_string(json_object_object_get(obj, "X_ZYXEL_QoS")),
			json_object_get_string(json_object_object_get(obj, "ClientID")),
			json_object_get_string(json_object_object_get(obj, "X_ZYXEL_Certificate")),
			json_object_get_string(json_object_object_get(obj, "X_ZYXEL_CA_Certificate")),
			json_object_get_string(json_object_object_get(obj, "Username")),
			json_object_get_string(json_object_object_get(obj, "Password")),
			json_object_get_string(json_object_object_get(obj, "ProtocolVersion"))
			);
	}
}

zcfgRet_t zcfgFeDalmqttClientObjEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	//printf("%s():--- Edit MQTTEntry Json Object --- \n%s \n ",__FUNCTION__,json_object_to_json_string(Jobj));
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_MQTT_CLIENT, &mqttClientIid, &mqttClientObj)) != ZCFG_SUCCESS){
		printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get MQTT fail oid(%d)\n ",__FUNCTION__,RDM_OID_MQTT_CLIENT);
		return ZCFG_INTERNAL_ERROR;
	}
	
	if(mqttClientObj != NULL){
		if(json_object_object_get(Jobj, "Enable"))
			json_object_object_add(mqttClientObj, "Enable", json_object_new_boolean(g_enable));
		if(json_object_object_get(Jobj, "BrokerAddress"))
			json_object_object_add(mqttClientObj, "BrokerAddress", json_object_new_string(g_brokerAddr));
		if(json_object_object_get(Jobj, "BrokerPort"))
			json_object_object_add(mqttClientObj, "BrokerPort", json_object_new_int(g_brokerPort));
		if(json_object_object_get(Jobj, "X_ZYXEL_QoS"))
			json_object_object_add(mqttClientObj, "X_ZYXEL_QoS", json_object_new_int(g_X_ZYXEL_QoS));
		/*
		if(json_object_object_get(Jobj, "WillEnable"))
			json_object_object_add(mqttClientObj, "WillEnable", json_object_new_boolean(g_will_enable));
		if(json_object_object_get(Jobj, "WillTopic"))
			json_object_object_add(mqttClientObj, "WillTopic", json_object_new_string(g_will_topic));
		if(json_object_object_get(Jobj, "WillValue"))
			json_object_object_add(mqttClientObj, "WillValue", json_object_new_string(g_will_value));
			*/
		if(json_object_object_get(Jobj, "TransportProtocol"))
			json_object_object_add(mqttClientObj, "TransportProtocol", json_object_new_string(g_transport_protocol));
		if(json_object_object_get(Jobj, "ClientID"))
			json_object_object_add(mqttClientObj, "ClientID", json_object_new_string(g_client_id));
		if(json_object_object_get(Jobj, "ForceReconnect"))
			json_object_object_add(mqttClientObj, "ForceReconnect", json_object_new_boolean(g_force_reconnect));
		/*
		if(json_object_object_get(Jobj, "KeepAliveTime"))
			json_object_object_add(mqttClientObj, "KeepAliveTime", json_object_new_int(g_keepalivetime));
			*/
		if(json_object_object_get(Jobj, "X_ZYXEL_Certificate"))
			json_object_object_add(mqttClientObj, "X_ZYXEL_Certificate", json_object_new_string(g_certificate));
		
		if(json_object_object_get(Jobj, "X_ZYXEL_CA_Certificate"))
			json_object_object_add(mqttClientObj, "X_ZYXEL_CA_Certificate", json_object_new_string(g_ca_certificate));

		if(json_object_object_get(Jobj, "Username"))
			json_object_object_add(mqttClientObj, "Username", json_object_new_string(g_username));

		if(json_object_object_get(Jobj, "Password"))
			json_object_object_add(mqttClientObj, "Password", json_object_new_string(g_password));

		if(json_object_object_get(Jobj, "ProtocolVersion"))
			json_object_object_add(mqttClientObj, "ProtocolVersion", json_object_new_string(g_protocol_version));
	}
	return ret;
}

zcfgRet_t zcfgFeDalMQTTEdit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *Jarray = NULL;
	int index = 0;
	zcfg_offset_t oid;
	IID_INIT(mqttClientIid);
	oid = RDM_OID_MQTT_CLIENT;
	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	initMqttEntryGlobalObjects();

	getMqttBasicInfo(Jobj);

	Jarray = json_object_new_array();

	zcfgFeDalMQTTGet(NULL,Jarray, NULL);

	ret = convertIndextoIid(index, &mqttClientIid, oid, NULL, NULL, replyMsg);

	if(ret != ZCFG_SUCCESS)
		return ret;	

	if((ret = zcfgFeDalmqttClientObjEdit(Jobj)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to edit MQTT Entry \n", __FUNCTION__);
		goto exit;
	}

	if((ret = zcfgFeObjJsonSet(RDM_OID_MQTT_CLIENT, &mqttClientIid, mqttClientObj, NULL)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to set object %d RDM_OID_MQTT_CLIENT\n", __FUNCTION__, RDM_OID_MQTT_CLIENT);
	}

exit:
	freeAllmqttClientObjects();
	return ret;
}

	
zcfgRet_t zcfgFeDalMQTTGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL;
	struct json_object *MqttJobj = NULL;
	objIndex_t objIid = {0};
	int count = 1;
	int force_reconnect = 0;
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_MQTT_CLIENT, &objIid, &MqttJobj) == ZCFG_SUCCESS){
		paramJobj = json_object_new_object();
		json_object_object_add(paramJobj, "Index", json_object_new_int(objIid.idx[0]));
		json_object_object_add(paramJobj, "Enable", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "Enable")));
		json_object_object_add(paramJobj, "BrokerAddress", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "BrokerAddress")));
		json_object_object_add(paramJobj, "BrokerPort", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "BrokerPort")));
		json_object_object_add(paramJobj, "X_ZYXEL_QoS", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "X_ZYXEL_QoS")));
		/*
		json_object_object_add(paramJobj, "WillEnable", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "WillEnable")));
		json_object_object_add(paramJobj, "WillTopic", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "WillTopic")));
		json_object_object_add(paramJobj, "WillValue", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "WillValue")));
		*/
		json_object_object_add(paramJobj, "TransportProtocol", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "TransportProtocol")));
		json_object_object_add(paramJobj, "ClientID", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "ClientID")));
		json_object_object_add(paramJobj, "ForceReconnect", json_object_new_boolean(force_reconnect));//always show false
		/*
		json_object_object_add(paramJobj, "KeepAliveTime", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "KeepAliveTime")));
		*/
		json_object_object_add(paramJobj, "X_ZYXEL_Certificate", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "X_ZYXEL_Certificate")));
		json_object_object_add(paramJobj, "X_ZYXEL_CA_Certificate", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "X_ZYXEL_CA_Certificate")));

		json_object_object_add(paramJobj, "Username", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "Username")));
		json_object_object_add(paramJobj, "Password", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "Password")));
		json_object_object_add(paramJobj, "ProtocolVersion", JSON_OBJ_COPY(json_object_object_get(MqttJobj, "ProtocolVersion")));

		json_object_array_add(Jarray, paramJobj);
	
		zcfgFeJsonObjFree(MqttJobj);
		count++;
	}
	return ret;
}

#if 1
int getMTPMQTTRef( objIndex_t mtpIndex, int *mtpMQTTRefIndex ){
	int ret = -1;
	objIndex_t mqttIid;
	struct json_object *mqttObj = NULL;
	const char *mqttRef = NULL;

	if(!mtpMQTTRefIndex){
		zcfgLog(ZCFG_LOG_ERR, "%s mtpMQTTRefIndex is NULL, return error.", __FUNCTION__);
		return -1;
	}

	IID_INIT(mqttIid);
	mqttIid.idx[0] = mtpIndex.idx[0];
	mqttIid.level = 1;

	ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T, &mqttIid, &mqttObj);

	if(ret != ZCFG_SUCCESS)
	{
		zcfgLog(ZCFG_LOG_ERR, "%s Get RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T fail.", __FUNCTION__);
		return ret;
	}

	mqttRef = json_object_get_string(json_object_object_get(mqttObj, "Reference"));
	if(mqttRef && strstr(mqttRef, "Device.MQTT.Client.") ){
		sscanf(mqttRef, "Device.MQTT.Client.%d", mtpMQTTRefIndex);
	}
	zcfgFeJsonObjFree(mqttObj);

	return 0;
}

int getControllerMTPMQTTRef( objIndex_t mtpIndex, int *mtpMQTTRefIndex ){
	int ret = -1;
	objIndex_t mqttIid;
	struct json_object *mqttObj = NULL;
	const char *mqttRef = NULL;

	if(!mtpMQTTRefIndex){
		zcfgLog(ZCFG_LOG_ERR, "%s mtpMQTTRefIndex is NULL, return error.", __FUNCTION__);
		return -1;
	}

	IID_INIT(mqttIid);
	mqttIid.level = 2;
	mqttIid.idx[0] = mtpIndex.idx[0];
	mqttIid.idx[1] = 1;

	ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T, &mqttIid, &mqttObj);

	if(ret != ZCFG_SUCCESS)
	{
		printf("%s Get RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T fail.\n", __FUNCTION__);
		zcfgLog(ZCFG_LOG_ERR, "%s Get RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T fail.", __FUNCTION__);
		return ret;
	}

	mqttRef = json_object_get_string(json_object_object_get(mqttObj, "Reference"));
	if(mqttRef && strstr(mqttRef, "Device.MQTT.Client.") ){
		sscanf(mqttRef, "Device.MQTT.Client.%d", mtpMQTTRefIndex);
	}

	zcfgFeJsonObjFree(mqttObj);

	return 0;
}
#endif

//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
/*!
*  replace string
*  @param[IN/OUT]	source
*  @param[IN]	find_str //keyword
*  @param[IN]	rep_str //replace
*/
void str_replace (char *source, char* find_str, char*rep_str){
	char result[65] = {0};
	char* str_temp = NULL;
	char* str_location = NULL;
	int rep_str_len = 0;
	int find_str_len = 0;
	int gap = 0;

	strcpy(result, source);
	find_str_len = strlen(find_str);
	rep_str_len = strlen(rep_str);
	str_temp = source;
	str_location = strstr(str_temp, find_str);

	while(str_location != NULL){
		gap += (str_location - str_temp);
		result[gap] = '\0';
		strcat(result, rep_str);
		gap += rep_str_len;
		str_temp = str_location + find_str_len;
		strcat(result, str_temp);
		str_location = strstr(str_temp, find_str);
	}

	result[strlen(result)] = '\0';
	memset(source, 0 , strlen(source));
	strcpy(source, result);
}

zcfgRet_t zcfgFeDalMQTTPublish(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int idx = -1;
	char *topic = NULL;
	char *msg = NULL;
	char cmd[2048];

	struct json_object *tr369LocalAgentMtpJobj = NULL;
	objIndex_t objIid = {0};
	char *protocol_str = NULL;
	char *enable_str = NULL;
	int refIndex = -1;

	char serialNumber[14] = "";

	if(json_object_object_get(Jobj, "Index")){
		idx =  json_object_get_int(json_object_object_get(Jobj, "Index"));
	}
	if(json_object_object_get(Jobj, "Topic")){
		topic =  json_object_get_string(json_object_object_get(Jobj, "Topic"));
	}
	if(json_object_object_get(Jobj, "Message")){
		msg =  json_object_get_string(json_object_object_get(Jobj, "Message"));
	}

	if( topic && msg ){
		zcfgLog(ZCFG_LOG_INFO, "%s() Enter", __FUNCTION__);
	} else {
		return ZCFG_INTERNAL_ERROR;
	}

	if(topic){
		if(zyUtilIGetSerialNumber(serialNumber) == ZCFG_SUCCESS){
			str_replace(topic, SERIAL_NUMBER_AUTO_REPLACE, serialNumber);
		}
	}

	if( !zyUtilIsAppRunning("obuspa") ){//hard code process name here
		snprintf(cmd, sizeof(cmd), "zpublishcmd publish %d %s %s", idx, topic, msg);
		zcfgLog(ZCFG_LOG_INFO, " %s ", cmd);
		system(cmd);
		return ZCFG_SUCCESS;
	}

	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &objIid, &tr369LocalAgentMtpJobj) == ZCFG_SUCCESS) {
		protocol_str = json_object_get_string(json_object_object_get(tr369LocalAgentMtpJobj, "Protocol"));
		if(protocol_str){

			if( !strcmp(protocol_str, "MQTT") ){
				enable_str = json_object_get_string(json_object_object_get(tr369LocalAgentMtpJobj, "Enable"));
				if(enable_str){

					if( !strcmp(enable_str, "true") || !strcmp(enable_str, "1") ){
						if(zyUtilIsAppRunning("obuspa")){//hard code process name here

							getMTPMQTTRef(objIid, &refIndex);

							//printf("%s() obuspa is running. idx %d,refIndex %d\n", __FUNCTION__, idx, refIndex);
							if( idx == refIndex ){
								//use Device.LocalAgent.MTP.1.MQTT.Reference (e.g. Device.MQTT.Client.2) to send publish message to MQTT broker
								snprintf(cmd, sizeof(cmd), "ztr369cmd publish %s %s", topic, msg);
								zcfgLog(ZCFG_LOG_INFO, "%s command_1 %s", __FUNCTION__, cmd);
								system(cmd);
								zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
								break;
							} else {
								snprintf(cmd, sizeof(cmd), "zpublishcmd publish %d %s %s", idx, topic, msg);
								zcfgLog(ZCFG_LOG_INFO, "%s command_2 %s", __FUNCTION__, cmd);
								system(cmd);
								zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
								break;
							}
						} else {
							snprintf(cmd, sizeof(cmd), "zpublishcmd publish %d %s %s", idx, topic, msg);
							zcfgLog(ZCFG_LOG_INFO, "%s command_3 %s", __FUNCTION__, cmd);
							system(cmd);
							zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
							break;
						}
					} else {
						zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
						continue;
					}
				}
			} else {
				zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
				continue;
			}
		}

		zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
	}

	return ret;
}
#endif

zcfgRet_t zcfgFeDalMQTTAdd(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	initMqttEntryGlobalObjects();
	getMqttBasicInfo(Jobj);

	IID_INIT(mqttClientIid);
	
	if((ret = zcfgFeObjJsonBlockedAdd(RDM_OID_MQTT_CLIENT, &mqttClientIid, NULL)) != ZCFG_SUCCESS){
		printf("%s():(ERROR) zcfgFeObjJsonAdd add MQTT Entry \n ",__FUNCTION__);
		goto exit;
	}

	if((ret = zcfgFeDalmqttClientObjEdit(Jobj)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to edit MQTT Entry \n", __FUNCTION__);
		goto exit;
	}

	if((ret = zcfgFeObjJsonSet(RDM_OID_MQTT_CLIENT, &mqttClientIid, mqttClientObj, NULL)) != ZCFG_SUCCESS){
		printf("%s:(ERROR) fail to set object %d MQTT Entry\n", __FUNCTION__, RDM_OID_MQTT_CLIENT);
	}

exit:
	freeAllmqttClientObjects();
	return ret;
}

zcfgRet_t zcfgFeDalMQTTDelete(struct json_object *Jobj, char *replyMsg)
{
	struct json_object *tr369LocalAgentMtpJobj = NULL;
	objIndex_t objIid = {0};
	struct json_object *tr369ControllerMtpJobj = NULL;
	objIndex_t objControllerMTPIid = {0};
	char *protocol_str = NULL;
	char *enable_str = NULL;
	int refIndex = -1;
	int controllerRefIndex = -1;

	zcfgRet_t ret = ZCFG_SUCCESS;
	int index = 0;
	zcfg_offset_t oid;
	IID_INIT(mqttClientIid);

	oid = RDM_OID_MQTT_CLIENT;
	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	ret = convertIndextoIid(index, &mqttClientIid, oid, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;
	//printf("%s() index %d\n", __FUNCTION__, index);
	#if 1//MQTT Client could not be deleted since this object is referenced by either local agent mtp or local agent controller mtp
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &objIid, &tr369LocalAgentMtpJobj) == ZCFG_SUCCESS) {
		protocol_str = json_object_get_string(json_object_object_get(tr369LocalAgentMtpJobj, "Protocol"));
		if(protocol_str){
			if( !strcmp(protocol_str, "MQTT") ){
					getMTPMQTTRef(objIid, &refIndex);

					//printf("%s() index %d,refIndex %d\n", __FUNCTION__, index, refIndex);
					zcfgLog(ZCFG_LOG_INFO, "%s index %d,refIndex %d", __FUNCTION__, index, refIndex);
					if( index == refIndex ){
						ret = ZCFG_INTERNAL_ERROR;//This object could not be deleted since this object is referenced.
						zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
						break;

					}
			}
		}

		zcfgFeJsonObjFree(tr369LocalAgentMtpJobj);
	}
	if(ret != ZCFG_SUCCESS)
		goto exit;


	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P, &objControllerMTPIid, &tr369ControllerMtpJobj) == ZCFG_SUCCESS) {
		protocol_str = json_object_get_string(json_object_object_get(tr369ControllerMtpJobj, "Protocol"));
		if(protocol_str){
			if( !strcmp(protocol_str, "MQTT") ){
					getControllerMTPMQTTRef(objControllerMTPIid, &controllerRefIndex);

					//printf("%s() (controller mtp) index %d,controllerRefIndex %d\n", __FUNCTION__, index, controllerRefIndex);
					zcfgLog(ZCFG_LOG_INFO, "%s (controller mtp) index %d,controllerRefIndex %d", __FUNCTION__, index, controllerRefIndex);
					if( index == controllerRefIndex ){
						ret = ZCFG_INTERNAL_ERROR;//This object could not be deleted since this object is referenced.
						zcfgFeJsonObjFree(tr369ControllerMtpJobj);
						break;
					}
			}
		}

		zcfgFeJsonObjFree(tr369ControllerMtpJobj);
	}
	if(ret != ZCFG_SUCCESS)
		goto exit;
	#endif

	if((ret = zcfgFeObjJsonDel(oid, &mqttClientIid, NULL)) != ZCFG_SUCCESS){
		printf("%s: delete mqtt obj fail \n", __FUNCTION__);
		goto exit;
	} else printf("%s: delete mqtt obj %d success \n", __FUNCTION__, index);

exit:
	freeAllmqttClientObjects();
	return ret;
	
}

zcfgRet_t zcfgFeDalMQTT(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT")){
		ret = zcfgFeDalMQTTEdit(Jobj, replyMsg);
	}
	else if(!strcmp(method, "POST")) {
		ret = zcfgFeDalMQTTAdd(Jobj, replyMsg);
	}
	else if(!strcmp(method, "DELETE")) {
		ret = zcfgFeDalMQTTDelete(Jobj, replyMsg);
	}
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalMQTTGet(Jobj, Jarray, NULL);
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
	else if(!strcmp(method, "PUBLISH")){
		ret = zcfgFeDalMQTTPublish(Jobj, Jarray, NULL);
	}
#endif
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

/*-----------------------MQTT Node -------------*/
objIndex_t mqttIid;
struct json_object *MQTTNodeObj;

void zcfgFeDalShowMQTTNode(struct json_object *Jarray){
#if 0
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	printf("%s: %s \n","X_ZYXEL_BoundIfName",json_object_get_string(json_object_object_get(Jarray,"X_ZYXEL_BoundIfName")));
#endif
}

zcfgRet_t zcfgFeDalMQTTNodeGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
        zcfgRet_t ret = ZCFG_SUCCESS;
        
	const char* Iface;
	
	IID_INIT(mqttIid);
        if( (ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_MQTT,&mqttIid,&MQTTNodeObj)) != ZCFG_SUCCESS){
                printf("%s():(ERROR) zcfgFeObjStructGet 1111 get MQTT fail oid(%d), ret = %d\n",__FUNCTION__,RDM_OID_MQTT, ret);
                return ZCFG_INTERNAL_ERROR;
        }
	if(MQTTNodeObj != NULL){
		if(json_object_object_get(MQTTNodeObj,"X_ZYXEL_BoundIfList")){
			Iface = json_object_get_string(json_object_object_get(MQTTNodeObj,"X_ZYXEL_BoundIfList"));
               		printf("X_ZYXEL_BoundIfList = %s\n",Iface);
		}
	        zcfgFeJsonObjFree(MQTTNodeObj);
        }
        return ret;
}

zcfgRet_t zcfgFeDalMQTTNodeEdit(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char* Iface;
		
	IID_INIT(mqttIid);
	if( (ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_MQTT,&mqttIid,&MQTTNodeObj)) != ZCFG_SUCCESS){
		printf(" %s() : (ERROR) zcfgFeObjJsonGetWithoutUpdate get MQTT object fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
	if(MQTTNodeObj != NULL ){
		if(json_object_object_get(Jobj,"X_ZYXEL_BoundIfList")){
			Iface = json_object_get_string(json_object_object_get(Jobj,"X_ZYXEL_BoundIfList"));
			json_object_object_add(MQTTNodeObj, "X_ZYXEL_BoundIfList", json_object_new_string(Iface));
			zcfgFeObjJsonBlockedSet(RDM_OID_MQTT, &mqttIid, MQTTNodeObj, NULL);
		}
		zcfgFeJsonObjFree(MQTTNodeObj);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t zcfgFeDalMQTTNode(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
        zcfgRet_t ret = ZCFG_SUCCESS;
        if(!strcmp(method, "PUT")){
                ret = zcfgFeDalMQTTNodeEdit(Jobj, replyMsg);
        }
        else if(!strcmp(method, "GET"))
                ret = zcfgFeDalMQTTNodeGet(Jobj, Jarray, NULL);
        else
                printf("Unknown method:%s\n", method);

        return ret;
}

