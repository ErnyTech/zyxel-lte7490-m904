
#include <json/json.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_webgui_flag.h"

dal_param_t LOG_SETTING_param[]={
	{"Enable",								dalType_boolean,0,	0,	NULL},
#ifdef TELIA_CUSTOMIZATION
#else	
	{"LogMode",								dalType_string,	0,	0,	NULL,	"Remote|Local|LocalRemote",	NULL},
#endif	
	{"LogServer",							dalType_string,	0,	0,	NULL},
	{"UDPPort",								dalType_int,	0,	0,	NULL},
	{"ATLogEnable",							dalType_boolean,0,	0,	NULL},
#ifdef ZYXEL_IMPROVE_SYSTEM_LOGS
	{"EnableDebug",							dalType_boolean,0,	0,	NULL},
#endif
	{"LogEnable",							dalType_boolean,0,	0,	NULL},
	{"LogSericeRef",						dalType_string,	0,	0,	NULL},
	{"LogEmailSubject",						dalType_string,	0,	0,	NULL},
	{"AlarmEmailSubject",					dalType_string,	0,	0,	NULL},
	{"LogEmailTo",							dalType_string,	0,	0,	NULL},
	{"AlarmEmailTo",						dalType_string,	0,	0,	NULL},
	{"Interval",							dalType_int,	0,	0,	NULL},
	{"category",							dalType_string,	0,	0,	NULL,	NULL,	dalcmd_Forbid},
#ifdef ZYXEL_NAT_LOG
	{"NatEmailSubject",						dalType_string,	0,	0,	NULL},
	{"NatEmailTo",							dalType_string,	0,	0,	NULL},
#endif
	{"WAN-DHCP",							dalType_boolean,0,	0,	NULL},
	{"DHCPServer",							dalType_boolean,0,	0,	NULL},
	{"PPPoE",								dalType_boolean,0,	0,	NULL},
	{"TR-069",								dalType_boolean,0,	0,	NULL},
	{"HTTP",								dalType_boolean,0,	0,	NULL},
	{"UPNP",								dalType_boolean,0,	0,	NULL},
	{"System",								dalType_boolean,0,	0,	NULL},
	{"xDSL",								dalType_boolean,0,	0,	NULL},
	{"ACL",									dalType_boolean,0,	0,	NULL},
	{"Wireless",							dalType_boolean,0,	0,	NULL},
	{"Voice",								dalType_boolean,0,	0,	NULL},
	{"Account",								dalType_boolean,0,	0,	NULL},
	{"Attack",								dalType_boolean,0,	0,	NULL},
	{"Firewall",							dalType_boolean,0,	0,	NULL},
	{"MACFilter",							dalType_boolean,0,	0,	NULL},
	{"esmd",								dalType_boolean,0,	0,	NULL},
	{"zcmd",								dalType_boolean,0,	0,	NULL},
	{"MQTT",								dalType_boolean,0,	0,	NULL},
#ifdef ZYXEL_LWM2M_CLIENT	
	{"LWM2M",								dalType_boolean,0,	0,	NULL},	
#endif	
	{NULL,									dalType_int,	0,	0,	NULL},

};

bool Enable;
bool ATLogEnable;
#ifdef ZYXEL_IMPROVE_SYSTEM_LOGS
bool EnableDebug;
#endif
const char* LogMode;
const char* LogServer;
unsigned int UDPPort;
unsigned int Interval;
const char* category;
bool LogEnable, MailEnable;
const char* LogEmailSubject;
const char* AlarmEmailSubject;
const char* CurrentIPEmailSubject;
const char* LogEmailTo;
const char* AlarmEmailTo;
const char* CurrentIPEmailTo;
const char* LogSericeRef;

struct json_object *logSettingJobj;
struct json_object *categoryJobj;

objIndex_t logSettingIid;
objIndex_t categoryIid;

#ifdef ZYXEL_NAT_LOG
const char* NatEmailSubject;
const char* NatEmailTo;
bool isnatlog;
#endif


void freeAllLogSettingObjects(){
	if(logSettingJobj) json_object_put(logSettingJobj);
	if(categoryJobj) json_object_put(categoryJobj);

	return;
}

void initLogSettingGlobalObjects(){
	logSettingJobj = NULL;
	categoryJobj = NULL;

	return;
}

void getLogSettingBasicInfo(struct json_object *Jobj){
	Enable = json_object_get_boolean(json_object_object_get(Jobj, "Enable"));
	ATLogEnable = json_object_get_boolean(json_object_object_get(Jobj, "ATLogEnable"));
	#ifdef ZYXEL_IMPROVE_SYSTEM_LOGS
	EnableDebug = json_object_get_boolean(json_object_object_get(Jobj, "EnableDebug"));
	#endif
	LogMode = json_object_get_string(json_object_object_get(Jobj, "LogMode"));
	LogServer = json_object_get_string(json_object_object_get(Jobj, "LogServer"));
	UDPPort = json_object_get_int(json_object_object_get(Jobj, "UDPPort"));
	Interval = json_object_get_int(json_object_object_get(Jobj, "Interval"));

	return;
}

void getCategoryBasicInfo(struct json_object *Jobj){
	category = json_object_get_string(json_object_object_get(Jobj, "category"));
	return;
}

void getMailEventBasicInfo(struct json_object *Jobj){
	if(NULL !=json_object_object_get(Jobj, "MailEnable")){
		MailEnable = json_object_get_boolean(json_object_object_get(Jobj, "MailEnable"));
		LogEmailSubject = json_object_get_string(json_object_object_get(Jobj, "MailSubject"));
		AlarmEmailSubject = json_object_get_string(json_object_object_get(Jobj, "AlarmSubject"));
		CurrentIPEmailSubject = json_object_get_string(json_object_object_get(Jobj, "CurrentIPSubject"));
		LogEmailTo = json_object_get_string(json_object_object_get(Jobj, "MailTo"));
		AlarmEmailTo = json_object_get_string(json_object_object_get(Jobj, "AlarmTo"));
		CurrentIPEmailTo = json_object_get_string(json_object_object_get(Jobj, "CurrentIPTo"));
		LogSericeRef = json_object_get_string(json_object_object_get(Jobj, "MailRef"));
	}else{
		LogEnable = json_object_get_boolean(json_object_object_get(Jobj, "LogEnable"));
		LogEmailSubject = json_object_get_string(json_object_object_get(Jobj, "LogEmailSubject"));
		AlarmEmailSubject = json_object_get_string(json_object_object_get(Jobj, "AlarmEmailSubject"));
		LogEmailTo = json_object_get_string(json_object_object_get(Jobj, "LogEmailTo"));
		AlarmEmailTo = json_object_get_string(json_object_object_get(Jobj, "AlarmEmailTo"));
		LogSericeRef = json_object_get_string(json_object_object_get(Jobj, "LogSericeRef"));
	}

	#ifdef ZYXEL_NAT_LOG
	NatEmailSubject = json_object_get_string(json_object_object_get(Jobj, "NatEmailSubject"));
	NatEmailTo = json_object_get_string(json_object_object_get(Jobj, "NatEmailTo"));
	#endif
	return;
}


zcfgRet_t zcfgFeDaleMailEventLogCfg(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *emailLogObj = NULL;
	objIndex_t Iid = {0};
	bool emailLog = false, curemailstatus = false;
	while(zcfgFeObjJsonGetNext(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailLogObj) == ZCFG_SUCCESS){
		if(!strcmp("MAILSEND_EVENT_SEND_LOG",json_object_get_string(json_object_object_get(emailLogObj, "Event")))){
			zcfgFeJsonObjFree(emailLogObj);
			emailLog = true;
			break;			
		}
		zcfgFeJsonObjFree(emailLogObj);
	}
	if(emailLog){
		if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailLogObj) == ZCFG_SUCCESS){
			curemailstatus = json_object_get_boolean(json_object_object_get(emailLogObj, "Enable"));

			if(json_object_object_get(Jobj, "MailEnable")){ //come from webgui Vue
				if(MailEnable){
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(MailEnable));
					if(json_object_object_get(Jobj, "MailTo"))
						json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(LogEmailTo));
					if(json_object_object_get(Jobj, "MailSubject"))
						json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(LogEmailSubject));
					if(json_object_object_get(Jobj, "MailRef"))
						json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(MailEnable));
					json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(""));
				}
			}else if(json_object_object_get(Jobj, "LogEnable")){
				if(LogEnable){
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(LogEnable));
					if(json_object_object_get(Jobj, "LogEmailTo"))
						json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(LogEmailTo));
					if(json_object_object_get(Jobj, "LogEmailSubject"))
						json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(LogEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(LogEnable));
					json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(""));
				}
			}
			else{
				if(curemailstatus){
					if(json_object_object_get(Jobj, "AlarmEmailTo") || json_object_object_get(Jobj, "AlarmTo") )
						json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(LogEmailTo));
					if(json_object_object_get(Jobj, "AlarmEmailSubject") || json_object_object_get(Jobj, "AlarmSubject") )
						json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(LogEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef") || json_object_object_get(Jobj, "MailRef") )
						json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(""));
				}
			}
		}
	}
	else{
		IID_INIT(Iid);
		if(zcfgFeObjJsonBlockedAdd(RDM_OID_MAIL_EVENT_CFG, &Iid, NULL) == ZCFG_SUCCESS){
			if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailLogObj) == ZCFG_SUCCESS){
				json_object_object_add(emailLogObj, "Event", json_object_new_string("MAILSEND_EVENT_SEND_LOG"));
				json_object_object_add(emailLogObj, "EmailBody", json_object_new_string("User send a email with log file."));
				if(json_object_object_get(Jobj, "LogEnable"))
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(LogEnable));
				else if(json_object_object_get(Jobj, "MailEnable"))
					json_object_object_add(emailLogObj, "Enable", json_object_new_boolean(MailEnable));
				if(json_object_object_get(Jobj, "LogEmailTo") || json_object_object_get(Jobj, "MailTo"))
					json_object_object_add(emailLogObj, "EmailTo", json_object_new_string(LogEmailTo));
				if(json_object_object_get(Jobj, "LogEmailSubject") || json_object_object_get(Jobj, "MailSubject"))
					json_object_object_add(emailLogObj, "EmailSubject", json_object_new_string(LogEmailSubject));
				if(json_object_object_get(Jobj, "LogSericeRef") || json_object_object_get(Jobj, "MailRef"))
					json_object_object_add(emailLogObj, "SericeRef", json_object_new_string(LogSericeRef));
	}
			}
		}
	if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_EVENT_CFG, &Iid, emailLogObj, NULL)) != ZCFG_SUCCESS)
		{
			printf("%s:(ERROR) Set email event fail\n", __FUNCTION__);
			return ZCFG_INTERNAL_ERROR;
		}
	zcfgFeJsonObjFree(emailLogObj);	

	return ret;
}


zcfgRet_t zcfgFeDaleMailEventAlarmCfg(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *emailAlarmObj = NULL;
	objIndex_t Iid = {0};
	bool emailAlarm = false, curemailstatus = false;
	while(zcfgFeObjJsonGetNext(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailAlarmObj) == ZCFG_SUCCESS){
		if(!strcmp("MAILSEND_EVENT_SEND_ALARM",json_object_get_string(json_object_object_get(emailAlarmObj, "Event")))){
			zcfgFeJsonObjFree(emailAlarmObj);
			emailAlarm = true;
			break;			
		}
		zcfgFeJsonObjFree(emailAlarmObj);
	}
	if(emailAlarm){
		if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailAlarmObj) == ZCFG_SUCCESS){
			curemailstatus = json_object_get_boolean(json_object_object_get(emailAlarmObj, "Enable"));
			if(json_object_object_get(Jobj, "MailEnable")){ //come from webgui Vue
				if(MailEnable){
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(MailEnable));
					if(json_object_object_get(Jobj, "AlarmTo"))
						json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(AlarmEmailTo));
					if(json_object_object_get(Jobj, "AlarmSubject"))
						json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(AlarmEmailSubject));
					if(json_object_object_get(Jobj, "MailRef"))
						json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(MailEnable));
					json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(""));
				}
			}else if(json_object_object_get(Jobj, "LogEnable")){
				if(LogEnable){
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(LogEnable));
					if(json_object_object_get(Jobj, "AlarmEmailTo"))
						json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(AlarmEmailTo));
					if(json_object_object_get(Jobj, "AlarmEmailSubject"))
						json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(AlarmEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(LogEnable));
					json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(""));
				}
			}
			else{
				if(curemailstatus){
					if(json_object_object_get(Jobj, "AlarmEmailTo"))
						json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(AlarmEmailTo));
					if(json_object_object_get(Jobj, "AlarmEmailSubject"))
						json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(AlarmEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(""));
				}
			}
		}
	}
	else{
		IID_INIT(Iid);
		if(zcfgFeObjJsonBlockedAdd(RDM_OID_MAIL_EVENT_CFG, &Iid, NULL) == ZCFG_SUCCESS){
			if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailAlarmObj) == ZCFG_SUCCESS){
				json_object_object_add(emailAlarmObj, "Event", json_object_new_string("MAILSEND_EVENT_SEND_ALARM"));
				json_object_object_add(emailAlarmObj, "EmailBody", json_object_new_string("Your device is under attack."));
				if(json_object_object_get(Jobj, "LogEnable"))
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(LogEnable));
				else if(json_object_object_get(Jobj, "MailEnable"))
					json_object_object_add(emailAlarmObj, "Enable", json_object_new_boolean(MailEnable));
				if(json_object_object_get(Jobj, "AlarmEmailTo") || json_object_object_get(Jobj, "AlarmTo") )
					json_object_object_add(emailAlarmObj, "EmailTo", json_object_new_string(AlarmEmailTo));
				if(json_object_object_get(Jobj, "AlarmEmailSubject")|| json_object_object_get(Jobj, "AlarmSubject"))
					json_object_object_add(emailAlarmObj, "EmailSubject", json_object_new_string(AlarmEmailSubject));
				if(json_object_object_get(Jobj, "LogSericeRef")|| json_object_object_get(Jobj, "MailRef"))
					json_object_object_add(emailAlarmObj, "SericeRef", json_object_new_string(LogSericeRef));
	}
			}
		}
	if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_EVENT_CFG, &Iid, emailAlarmObj, NULL)) != ZCFG_SUCCESS)
		{
			printf("%s:(ERROR) Set email event fail\n", __FUNCTION__);
			return ZCFG_INTERNAL_ERROR;
		}
	zcfgFeJsonObjFree(emailAlarmObj);	
	return ret;
}
zcfgRet_t zcfgFeDaleMailEventCurrentIPCfg(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *emailCurrentIPObj = NULL;
	objIndex_t Iid = {0};
	bool emailCurrentIP = false, curemailstatus = false;
	while(zcfgFeObjJsonGetNext(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailCurrentIPObj) == ZCFG_SUCCESS){
		if(!strcmp("MAILSEND_EVENT_SEND_CURRENT_IP",json_object_get_string(json_object_object_get(emailCurrentIPObj, "Event")))){
			zcfgFeJsonObjFree(emailCurrentIPObj);
			emailCurrentIP = true;
			break;			
		}
		zcfgFeJsonObjFree(emailCurrentIPObj);
	}
	if(emailCurrentIP){
		if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailCurrentIPObj) == ZCFG_SUCCESS){
			curemailstatus = json_object_get_boolean(json_object_object_get(emailCurrentIPObj, "Enable"));
			if(json_object_object_get(Jobj, "MailEnable")){ //come from webgui Vue
				if(MailEnable){
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(MailEnable));
					if(json_object_object_get(Jobj, "CurrentIPTo"))
						json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(CurrentIPEmailTo));
					if(json_object_object_get(Jobj, "CurrentIPSubject"))
						json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(CurrentIPEmailSubject));
					if(json_object_object_get(Jobj, "MailRef"))
						json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(MailEnable));
					json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(""));
				}
			}else if(json_object_object_get(Jobj, "LogEnable")){
				if(LogEnable){
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(LogEnable));
					if(json_object_object_get(Jobj, "CurrenIPEmailTo"))
						json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(CurrentIPEmailTo));
					if(json_object_object_get(Jobj, "CurrentIPEmailSubject"))
						json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(CurrentIPEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(LogEnable));
					json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(""));
				}
			}
			else{
				if(curemailstatus){
					if(json_object_object_get(Jobj, "CurrentIPEmailTo"))
						json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(CurrentIPEmailTo));
					if(json_object_object_get(Jobj, "CurrentIPEmailSubject"))
						json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(CurrentIPEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(""));
				}
			}
		}
	}
	else{
		IID_INIT(Iid);
		if(zcfgFeObjJsonBlockedAdd(RDM_OID_MAIL_EVENT_CFG, &Iid, NULL) == ZCFG_SUCCESS){
			if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailCurrentIPObj) == ZCFG_SUCCESS){
				json_object_object_add(emailCurrentIPObj, "Event", json_object_new_string("MAILSEND_EVENT_SEND_CURRENT_IP"));
				json_object_object_add(emailCurrentIPObj, "EmailBody", json_object_new_string(""));
				if(json_object_object_get(Jobj, "LogEnable"))
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(LogEnable));
				else if(json_object_object_get(Jobj, "MailEnable"))
					json_object_object_add(emailCurrentIPObj, "Enable", json_object_new_boolean(MailEnable));
				if(json_object_object_get(Jobj, "CurrentIPEmailTo") || json_object_object_get(Jobj, "CurrentIPTo") )
					json_object_object_add(emailCurrentIPObj, "EmailTo", json_object_new_string(CurrentIPEmailTo));
				if(json_object_object_get(Jobj, "CurrentIPEmailSubject")|| json_object_object_get(Jobj, "CurrentIPSubject"))
					json_object_object_add(emailCurrentIPObj, "EmailSubject", json_object_new_string(CurrentIPEmailSubject));
				if(json_object_object_get(Jobj, "LogSericeRef")|| json_object_object_get(Jobj, "MailRef"))
					json_object_object_add(emailCurrentIPObj, "SericeRef", json_object_new_string(LogSericeRef));
			}
		}
	}
	if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_EVENT_CFG, &Iid, emailCurrentIPObj, NULL)) != ZCFG_SUCCESS)
	{
		printf("%s:(ERROR) Set email event fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
	zcfgFeJsonObjFree(emailCurrentIPObj);	
	return ret;
}
#ifdef ZYXEL_NAT_LOG
zcfgRet_t zcfgFeDaleMailEventNatLogCfg(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *emailNatObj = NULL;
	objIndex_t Iid = {0};
	bool emailNat = false, curemailstatus = false;
	while(zcfgFeObjJsonGetNext(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailNatObj) == ZCFG_SUCCESS){
		if(!strcmp("MAILSEND_EVENT_SEND_NAT_LOG",json_object_get_string(json_object_object_get(emailNatObj, "Event")))){
			zcfgFeJsonObjFree(emailNatObj);
			emailNat = true;
			break;			
		}
		zcfgFeJsonObjFree(emailNatObj);
	}
	if(emailNat){
		if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailNatObj) == ZCFG_SUCCESS){
			curemailstatus = json_object_get_boolean(json_object_object_get(emailNatObj, "Enable"));
			if(json_object_object_get(Jobj, "LogEnable")){
				if(LogEnable){
					json_object_object_add(emailNatObj, "Enable", json_object_new_boolean(LogEnable));
					if(json_object_object_get(Jobj, "NatEmailTo"))
						json_object_object_add(emailNatObj, "EmailTo", json_object_new_string(NatEmailTo));
					if(json_object_object_get(Jobj, "NatEmailSubject"))
						json_object_object_add(emailNatObj, "EmailSubject", json_object_new_string(NatEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailNatObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailNatObj, "Enable", json_object_new_boolean(LogEnable));
					json_object_object_add(emailNatObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailNatObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailNatObj, "SericeRef", json_object_new_string(""));
				}
			}
			else{
				if(curemailstatus){
					if(json_object_object_get(Jobj, "NatEmailTo"))
						json_object_object_add(emailNatObj, "EmailTo", json_object_new_string(NatEmailTo));
					if(json_object_object_get(Jobj, "NatEmailSubject"))
						json_object_object_add(emailNatObj, "EmailSubject", json_object_new_string(NatEmailSubject));
					if(json_object_object_get(Jobj, "LogSericeRef"))
						json_object_object_add(emailNatObj, "SericeRef", json_object_new_string(LogSericeRef));
				}
				else{
					json_object_object_add(emailNatObj, "EmailTo", json_object_new_string(""));
					json_object_object_add(emailNatObj, "EmailSubject", json_object_new_string(""));
					json_object_object_add(emailNatObj, "SericeRef", json_object_new_string(""));
				}
			}
		}
	}
	else{
		IID_INIT(Iid);
		if(zcfgFeObjJsonBlockedAdd(RDM_OID_MAIL_EVENT_CFG, &Iid, NULL) == ZCFG_SUCCESS){
			if(zcfgFeObjJsonGet(RDM_OID_MAIL_EVENT_CFG, &Iid, &emailNatObj) == ZCFG_SUCCESS){
				json_object_object_add(emailNatObj, "Event", json_object_new_string("MAILSEND_EVENT_SEND_ALARM"));
				json_object_object_add(emailNatObj, "EmailBody", json_object_new_string("Your device is under attack."));
				if(json_object_object_get(Jobj, "LogEnable"))
					json_object_object_add(emailNatObj, "Enable", json_object_new_boolean(LogEnable));
				if(json_object_object_get(Jobj, "NatEmailTo"))
					json_object_object_add(emailNatObj, "EmailTo", json_object_new_string(NatEmailTo));
				if(json_object_object_get(Jobj, "NatEmailSubject"))
					json_object_object_add(emailNatObj, "EmailSubject", json_object_new_string(NatEmailSubject));
				if(json_object_object_get(Jobj, "LogSericeRef"))
					json_object_object_add(emailNatObj, "SericeRef", json_object_new_string(LogSericeRef));
	}
			}
		}
	if((ret = zcfgFeObjJsonSet(RDM_OID_MAIL_EVENT_CFG, &Iid, emailNatObj, NULL)) != ZCFG_SUCCESS)
		{
			printf("%s:(ERROR) Set email event fail\n", __FUNCTION__);
			return ZCFG_INTERNAL_ERROR;
		}
	zcfgFeJsonObjFree(emailNatObj);
	return ret;
}
#endif
zcfgRet_t zcfgFeDalLogSettingcfg(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *obj = NULL, *objtmp = NULL;
	getLogSettingBasicInfo(Jobj);
	IID_INIT(logSettingIid);
	obj = json_object_new_array();
	zcfgFeDalLogSettingGet(Jobj, obj, NULL);
	objtmp = json_object_array_get_idx(obj, 0);
	if((ret = zcfgFeObjJsonGet(RDM_OID_LOG_SETTING, &logSettingIid, &logSettingJobj)) != ZCFG_SUCCESS) {
		printf("%s:(ERROR) Get logsetting Obj fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}

	if(logSettingJobj != NULL){
		if(json_object_object_get(Jobj, "Enable"))
		json_object_object_add(logSettingJobj, "Enable", json_object_new_boolean(Enable));

		if(json_object_object_get(Jobj, "ATLogEnable"))
			json_object_object_add(logSettingJobj, "AT_LOG_Enable", json_object_new_boolean(ATLogEnable));

#ifdef ZYXEL_IMPROVE_SYSTEM_LOGS
		if(json_object_object_get(Jobj, "EnableDebug")){
		json_object_object_add(logSettingJobj, "SYS_LOG_DEBUG_Enable", json_object_new_boolean(EnableDebug));
		}
#endif		
#ifdef TELIA_CUSTOMIZATION
		if(Enable)
			json_object_object_add(logSettingJobj, "LogMode", json_object_new_string("2"));
		else
			json_object_object_add(logSettingJobj, "LogMode", json_object_new_string("1"));
			
#else	
		if(json_object_object_get(Jobj, "LogMode")){
			if(!strcmp("Local",LogMode))
				json_object_object_add(logSettingJobj, "LogMode", json_object_new_string("0"));
			else if(!strcmp("Remote",LogMode))
				json_object_object_add(logSettingJobj, "LogMode", json_object_new_string("1"));
			else if(!strcmp("LocalRemote",LogMode))
				json_object_object_add(logSettingJobj, "LogMode", json_object_new_string("2"));
		}
#endif		
		if(json_object_object_get(Jobj, "LogMode")){
			if(strcmp("Local",LogMode)){
				if(json_object_object_get(Jobj, "LogServer"))
		json_object_object_add(logSettingJobj, "LogServer", json_object_new_string(LogServer));
				if(json_object_object_get(Jobj, "UDPPort"))
		json_object_object_add(logSettingJobj, "UDPPort", json_object_new_int(UDPPort));
			}
		}
		else{
			if(strcmp("Local",json_object_get_string(json_object_object_get(objtmp, "LogMode")))){
				if(json_object_object_get(Jobj, "LogServer"))
					json_object_object_add(logSettingJobj, "LogServer", json_object_new_string(LogServer));
				if(json_object_object_get(Jobj, "UDPPort"))
					json_object_object_add(logSettingJobj, "UDPPort", json_object_new_int(UDPPort));
			}
		}
		if(json_object_object_get(Jobj, "Interval"))
		json_object_object_add(logSettingJobj, "Interval", json_object_new_int(Interval));

		if((ret = zcfgFeObjJsonSet(RDM_OID_LOG_SETTING, &logSettingIid, logSettingJobj, NULL)) != ZCFG_SUCCESS)
		{
			printf("%s:(ERROR) Set Log setting fail oid(%d)\n", __FUNCTION__, RDM_OID_LOG_SETTING);
			return ZCFG_INTERNAL_ERROR;
		}
	}

	return ret;
}

zcfgRet_t zcfgFeDalCategory(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	getCategoryBasicInfo(Jobj);
	char *logCat = NULL, *tmp_ptr = NULL;
	char *intf;
	int classIdx;
	int categoryIdx;
	bool enable;
	char catTemp[128] = {0};

	//printf("Input category (%s)",category);

	logCat = strtok_r(category, ",", &tmp_ptr);
	while(logCat != NULL){
		//printf("logCat : %s \n", logCat);
		sscanf(logCat, "%d_%d", &classIdx, &categoryIdx);
		//printf("classIdx : %d, categoryIdx :%d \n", classIdx, categoryIdx);

		IID_INIT(categoryIid);
		categoryIid.level = 2;
		categoryIid.idx[0] = (classIdx+1);
		categoryIid.idx[1] = (categoryIdx+1);

		if((ret = zcfgFeObjJsonGet(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryJobj)) != ZCFG_SUCCESS) {
			printf("%s: Get Log category Obj fail\n", __FUNCTION__);
			return ZCFG_INTERNAL_ERROR;
		}

		if(categoryJobj != NULL){
			enable = json_object_get_boolean(json_object_object_get(categoryJobj, "Enable"));
			if(enable == false){
				json_object_object_add(categoryJobj, "Enable", json_object_new_boolean(true));
			}else{
				json_object_object_add(categoryJobj, "Enable", json_object_new_boolean(false));
			}
			// notify log settings modification
			if((ret = zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryJobj, NULL)) != ZCFG_SUCCESS)
			{
				printf("%s: Set log category fail\n", __FUNCTION__);
				return ZCFG_INTERNAL_ERROR;
			}
		}

		logCat = strtok_r(NULL, ",", &tmp_ptr);
	}

	return ret;
}

zcfgRet_t zcfgFeDaleMailEvent(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	getMailEventBasicInfo(Jobj);

	if((ret = zcfgFeDaleMailEventLogCfg(Jobj)) != ZCFG_SUCCESS) {
		printf("%s: Set log category fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
	if((ret = zcfgFeDaleMailEventAlarmCfg(Jobj)) != ZCFG_SUCCESS) {
		printf("%s: Set log category fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
	if((ret = zcfgFeDaleMailEventCurrentIPCfg(Jobj)) != ZCFG_SUCCESS) {
		printf("%s: Set log category fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
#ifdef ZYXEL_NAT_LOG
	if((ret = zcfgFeDaleMailEventNatLogCfg(Jobj)) != ZCFG_SUCCESS) {
		printf("%s: Set log category fail\n", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
#endif
	return ret;
}

zcfgRet_t zcfgFeDalLogSettingEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	initLogSettingGlobalObjects();
	if(json_object_object_get(Jobj, "category")){
	if((ret = zcfgFeDalCategory(Jobj)) != ZCFG_SUCCESS){
		printf("%s: config Category fail\n", __FUNCTION__);
		goto exit;
	}
	}
	else{
		zcfgFeDalCLICategory(Jobj);
	}

	if((ret = zcfgFeDalLogSettingcfg(Jobj)) != ZCFG_SUCCESS){
		printf("%s: config LogSetting fail\n", __FUNCTION__);
		goto exit;
	}

	if((ret = zcfgFeDaleMailEvent(Jobj)) != ZCFG_SUCCESS){
		printf("%s: config MailEvent fail\n", __FUNCTION__);
		goto exit;
	}

exit:
	freeAllLogSettingObjects();
	return ret;
}

zcfgRet_t zcfgFeDalCLICategory(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *categoryObj = NULL;
	objIndex_t categoryIid = {0};
	if(json_object_object_get(Jobj, "WAN-DHCP")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"WAN-DHCP")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "WAN-DHCP")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);///////////////for cli
	if(json_object_object_get(Jobj, "DHCPServer")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"DHCP Server")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "DHCPServer")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "DHCP Server")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"DHCP Server")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "DHCP Server")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "PPPoE")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"PPPoE")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "PPPoE")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
#ifndef	WIND_ITALY_CUSTOMIZATION			
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "TR-069")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"TR-069")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "TR-069")));
				zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
#endif	
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "HTTP")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"HTTP")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "HTTP")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "UPNP")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"UPNP")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "UPNP")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "System")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"System")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "System")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "xDSL")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"xDSL")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "xDSL")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "ACL")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"ACL")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "ACL")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Wireless")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Wireless")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Wireless")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Voice")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Voice")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Voice")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
//MTKSOC Patches
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Cellular WAN")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Cellular WAN")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Cellular WAN")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "MQTT")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"MQTT")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "MQTT")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
#ifdef ZYXEL_LWM2M_CLIENT	
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "LWM2M")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"LWM2M")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "LWM2M")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}	
#endif	
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Account")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Account")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Account")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Attack")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Attack")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Attack")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "Firewall")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"Firewall")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "Firewall")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);//////////////for cli
	if(json_object_object_get(Jobj, "MACFilter")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"MAC Filter")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "MACFilter")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "MAC Filter")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"MAC Filter")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "MAC Filter")));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}

	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "ESMD")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"ESMD")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "ESMD")));
				zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "zcmd")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"zcmd")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "zcmd")));
				zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}
	IID_INIT(categoryIid);
	if(json_object_object_get(Jobj, "MESH")){
		while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &categoryIid, &categoryObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(categoryObj, "Describe")),"MESH")){
				json_object_object_add(categoryObj, "Enable", JSON_OBJ_COPY(json_object_object_get(Jobj, "MESH")));
				zcfgFeObjJsonSet(RDM_OID_LOG_CATEGORY, &categoryIid, categoryObj, NULL);
				zcfgFeJsonObjFree(categoryObj);
			}
			zcfgFeJsonObjFree(categoryObj);
		}
	}


	return ret;	
}

zcfgRet_t zcfgFeDalLogSettingGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *LogJobj = NULL, *MailJobj = NULL, *CategoryObj = NULL, *sysJobj = NULL, *sysNameJobj = NULL, *securJobj = NULL;
	 sysJobj = json_object_new_array();
	 securJobj = json_object_new_array();
//MTKSOC Patches
	 sysNameJobj = json_object_new_object();
	objIndex_t LogIid = {0}, MailIid = {0}, CategoryIid = {0};
	paramJobj = json_object_new_object();
	if(zcfgFeObjJsonGet(RDM_OID_LOG_SETTING, &LogIid, &LogJobj) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "Enable", JSON_OBJ_COPY(json_object_object_get(LogJobj, "Enable")));
#ifdef ZYXEL_IMPROVE_SYSTEM_LOGS
		json_object_object_add(paramJobj, "EnableDebug", JSON_OBJ_COPY(json_object_object_get(LogJobj, "SYS_LOG_DEBUG_Enable")));
#endif		
		if(!strcmp("0",json_object_get_string(json_object_object_get(LogJobj, "LogMode"))))
			json_object_object_add(paramJobj, "LogMode", json_object_new_string("Local"));
		else if(!strcmp("1",json_object_get_string(json_object_object_get(LogJobj, "LogMode"))))
			json_object_object_add(paramJobj, "LogMode", json_object_new_string("Remote"));
		else if(!strcmp("2",json_object_get_string(json_object_object_get(LogJobj, "LogMode"))))
			json_object_object_add(paramJobj, "LogMode", json_object_new_string("LocalRemote"));
		json_object_object_add(paramJobj, "LogServer", JSON_OBJ_COPY(json_object_object_get(LogJobj, "LogServer")));
		json_object_object_add(paramJobj, "UDPPort", JSON_OBJ_COPY(json_object_object_get(LogJobj, "UDPPort")));
		json_object_object_add(paramJobj, "Interval", JSON_OBJ_COPY(json_object_object_get(LogJobj, "Interval")));
		json_object_object_add(paramJobj, "ATLogEnable", JSON_OBJ_COPY(json_object_object_get(LogJobj, "AT_LOG_Enable")));
		zcfgFeJsonObjFree(LogJobj);
	}
	while(zcfgFeObjJsonGetNext(RDM_OID_MAIL_EVENT_CFG, &MailIid, &MailJobj) == ZCFG_SUCCESS){
		if(!strcmp("MAILSEND_EVENT_SEND_LOG",json_object_get_string(json_object_object_get(MailJobj, "Event")))){
			json_object_object_add(paramJobj, "MailEnable", JSON_OBJ_COPY(json_object_object_get(MailJobj, "Enable")));
			json_object_object_add(paramJobj, "MailTo", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailTo")));
			json_object_object_add(paramJobj, "MailSubject", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailSubject")));
			json_object_object_add(paramJobj, "MailRef", JSON_OBJ_COPY(json_object_object_get(MailJobj, "SericeRef")));
		}
		if(!strcmp("MAILSEND_EVENT_SEND_ALARM",json_object_get_string(json_object_object_get(MailJobj, "Event")))){
			json_object_object_add(paramJobj, "AlarmTo", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailTo")));
			json_object_object_add(paramJobj, "AlarmSubject", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailSubject")));
		}
		if(!strcmp("MAILSEND_EVENT_SEND_CURRENT_IP",json_object_get_string(json_object_object_get(MailJobj, "Event")))){
			json_object_object_add(paramJobj, "CurrentIPTo", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailTo")));
			json_object_object_add(paramJobj, "CurrentIPSubject", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailSubject")));
		}
#ifdef ZYXEL_NAT_LOG
		if(!strcmp("MAILSEND_EVENT_SEND_ALARM",json_object_get_string(json_object_object_get(MailJobj, "Event")))){
			json_object_object_add(paramJobj, "NatTo", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailTo")));
			json_object_object_add(paramJobj, "NatSubject", JSON_OBJ_COPY(json_object_object_get(MailJobj, "EmailSubject")));
		}
#endif
		zcfgFeJsonObjFree(MailJobj);
	}
	while(zcfgFeObjJsonGetNext(RDM_OID_LOG_CATEGORY, &CategoryIid, &CategoryObj) == ZCFG_SUCCESS){
#ifdef	WIND_ITALY_CUSTOMIZATION		
		if(!strcmp(json_object_get_string(json_object_object_get(CategoryObj, "Describe")),"TR-069")){
			zcfgFeJsonObjFree(CategoryObj);
			continue;
		}		
#endif 		
		json_object_object_add(paramJobj, json_object_get_string(json_object_object_get(CategoryObj, "Describe")), JSON_OBJ_COPY(json_object_object_get(CategoryObj, "Enable")));
		/*	when iid first element is 1, it is systemLog.
			when iid first element is 2, it is securityLog*/
		if(1==CategoryIid.idx[0]){
			json_object_array_add(sysJobj,json_object_new_string(json_object_get_string(json_object_object_get(CategoryObj, "Describe")) ) );
//MTKSOC Patches
			json_object_object_add(sysNameJobj, json_object_get_string(json_object_object_get(CategoryObj, "Name")), JSON_OBJ_COPY(json_object_object_get(CategoryObj, "Describe")));
		}else if(2==CategoryIid.idx[0]){
			json_object_array_add(securJobj,json_object_new_string(json_object_get_string(json_object_object_get(CategoryObj, "Describe")) ) );
		}
		zcfgFeJsonObjFree(CategoryObj);
	}
	

	json_object_object_add(paramJobj, "systemLog", sysJobj);
	json_object_object_add(paramJobj, "securityLog", securJobj);
//MTKSOC Patches
	json_object_object_add(paramJobj, "systemLogName", sysNameJobj);
	json_object_array_add(Jarray, paramJobj);
	return ret;
}

void zcfgFeDalShowLogSetting(struct json_object *Jarray){
	struct json_object *obj = NULL;
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	obj = json_object_array_get_idx(Jarray, 0);
	printf("Syslog Setting \n");
#if 1//def TELIA_CUSTOMIZATION	
	printf("%-2s %-20s %-10s \n", "", "Local Syslog Logging:", json_object_get_string(json_object_object_get(obj, "Enable")));
#else	
	printf("%-2s %-20s %-10s \n", "", "Local Syslog Logging:", json_object_get_string(json_object_object_get(obj, "LogEnable")));
#endif
	printf("%-2s %-20s %-10s \n", "", "Mode:", json_object_get_string(json_object_object_get(obj, "LogMode")));
	printf("%-2s %-20s %-10s \n", "", "Syslog Server:", json_object_get_string(json_object_object_get(obj, "LogServer")));
	printf("%-2s %-20s %-10s \n", "", "UDP Port:", json_object_get_string(json_object_object_get(obj, "UDPPort")));
	printf("%-2s %-20s %-10s \n", "", "AT Log Enabled:", json_object_get_string(json_object_object_get(obj, "ATLogEnable")));
#ifndef TELIA_CUSTOMIZATION	
	printf("E-mail Log Settings \n");
	printf("%-2s %-20s %-10s \n", "", "E-mail Log Settings:", json_object_get_string(json_object_object_get(obj, "MailEnable")));
	printf("%-2s %-20s %-10s \n", "", "Mail Account:", json_object_get_string(json_object_object_get(obj, "MailRef")));
	printf("%-2s %-20s %-10s \n", "", "System Log Mail Subject:", json_object_get_string(json_object_object_get(obj, "MailSubject")));
	printf("%-2s %-20s %-10s \n", "", "Security Log Mail Subject:", json_object_get_string(json_object_object_get(obj, "AlarmSubject")));
#endif	
#ifdef ZYXEL_NAT_LOG
	printf("%-2s %-20s %-10s \n", "", "Nat Log Mail Subject:", json_object_get_string(json_object_object_get(obj, "NatSubject")));
#endif
	printf("%-2s %-20s %-10s \n", "", "Send Log to:", json_object_get_string(json_object_object_get(obj, "MailTo")));
	printf("%-2s %-20s %-10s \n", "", "Send Alarm to:", json_object_get_string(json_object_object_get(obj, "AlarmTo")));
#ifdef ZYXEL_NAT_LOG
	printf("%-2s %-20s %-10s \n", "", "Send Nat to:", json_object_get_string(json_object_object_get(obj, "NatTo")));
#endif
	printf("%-2s %-20s %-10s \n", "", "Alarm Interval:", json_object_get_string(json_object_object_get(obj, "Interval")));
	printf("Active Log \n");
	printf("%-2s System Log \n","");
#ifdef TELIA_CUSTOMIZATION
	printf("%-4s %-12s %-10s\n", "", "WAN-DHCP", json_object_get_string(json_object_object_get(obj, "WAN-DHCP")));
	printf("%-4s %-12s %-10s\n", "", "DHCP Server", json_object_get_string(json_object_object_get(obj, "DHCP Server")));
	printf("%-4s %-12s %-10s\n", "", "TR-069", json_object_get_string(json_object_object_get(obj, "TR-069")));
	printf("%-4s %-12s %-10s\n", "", "HTTP", json_object_get_string(json_object_object_get(obj, "HTTP")));
	printf("%-4s %-12s %-10s\n", "", "UPNP", json_object_get_string(json_object_object_get(obj, "UPNP")));
	printf("%-4s %-12s %-10s\n", "", "System", json_object_get_string(json_object_object_get(obj, "System")));
	printf("%-4s %-12s %-10s\n", "", "ACL", json_object_get_string(json_object_object_get(obj, "ACL")));
	printf("%-4s %-12s %-10s\n", "", "Wireless", json_object_get_string(json_object_object_get(obj, "Wireless")));
//MTKSOC Patches
	printf("%-4s %-12s %-10s\n", "", "Cellular WAN", json_object_get_string(json_object_object_get(obj, "Cellular WAN")));

#else		
	printf("%-4s %-12s %-10s\n", "", "WAN-DHCP", json_object_get_string(json_object_object_get(obj, "WAN-DHCP")));
	printf("%-4s %-12s %-10s\n", "", "DHCP Server", json_object_get_string(json_object_object_get(obj, "DHCP Server")));
	printf("%-4s %-12s %-10s\n", "", "PPPoE", json_object_get_string(json_object_object_get(obj, "PPPoE")));
	printf("%-4s %-12s %-10s\n", "", "TR-069", json_object_get_string(json_object_object_get(obj, "TR-069")));
	printf("%-4s %-12s %-10s\n", "", "HTTP", json_object_get_string(json_object_object_get(obj, "HTTP")));
	printf("%-4s %-12s %-10s\n", "", "UPNP", json_object_get_string(json_object_object_get(obj, "UPNP")));
	printf("%-4s %-12s %-10s\n", "", "System", json_object_get_string(json_object_object_get(obj, "System")));
	printf("%-4s %-12s %-10s\n", "", "xDSL", json_object_get_string(json_object_object_get(obj, "xDSL")));
	printf("%-4s %-12s %-10s\n", "", "ACL", json_object_get_string(json_object_object_get(obj, "ACL")));
	printf("%-4s %-12s %-10s\n", "", "Wireless", json_object_get_string(json_object_object_get(obj, "Wireless")));
	printf("%-4s %-12s %-10s\n", "", "Voice", json_object_get_string(json_object_object_get(obj, "Voice")));
//MTKSOC Patches
	printf("%-4s %-12s %-10s\n", "", "Cellular WAN", json_object_get_string(json_object_object_get(obj, "Cellular WAN")));
	printf("%-4s %-12s %-10s\n", "", "MQTT", json_object_get_string(json_object_object_get(obj, "MQTT")));
#ifdef ZYXEL_LWM2M_CLIENT	
	printf("%-4s %-12s %-10s\n", "", "LWM2M", json_object_get_string(json_object_object_get(obj, "LWM2M")));
#endif	
#endif	
	printf("%-2s Security Log \n","");
	printf("%-4s %-12s %-10s\n", "", "Account", json_object_get_string(json_object_object_get(obj, "Account")));
	printf("%-4s %-12s %-10s\n", "", "Attack", json_object_get_string(json_object_object_get(obj, "Attack")));
	printf("%-4s %-12s %-10s\n", "", "Firewall", json_object_get_string(json_object_object_get(obj, "Firewall")));
	printf("%-4s %-12s %-10s\n", "", "MAC Filter", json_object_get_string(json_object_object_get(obj, "MAC Filter")));
}


zcfgRet_t zcfgFeDalLogSetting(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "PUT")) {
		ret = zcfgFeDalLogSettingEdit(Jobj, NULL);
	}else if(!strcmp(method,"GET")){
		ret = zcfgFeDalLogSettingGet(Jobj, Jarray, NULL);
	}

	return ret;
}

