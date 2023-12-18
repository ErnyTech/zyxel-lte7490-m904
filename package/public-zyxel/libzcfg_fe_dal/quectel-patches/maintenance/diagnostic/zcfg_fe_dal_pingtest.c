
#include <json/json.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_webgui_flag.h"
#include "zlog_api.h"

#define IP_DIAG_IPPING 0
#define IP_DIAG_TRACE_RT 1
#define DNS_DIAG_NS_LOOKUP 2

#ifdef TELIA_CUSTOMIZATION
#define DIAG_CANCEL 3	//Add Cancel button to cancel function for long running processes
#endif

dal_param_t PING_TEST_param[]={
	{"type",								dalType_int,	0,	0,	NULL},
	{"ProtocolVersion",						dalType_string,	0,	0,	NULL},
	{"Host",								dalType_string,	0,	0,	NULL},
	{"DiagnosticsState",					dalType_string,	0,	0,	NULL},
	{"NumberOfRepetitions",					dalType_int,	0,	0,	NULL},
	{"HostName",							dalType_string,	0,	0,	NULL},
	{"DNSServer",							dalType_string,	0,	0,	NULL},
	{NULL,									0,				0,	0,	NULL},
};
#ifdef ZYXEL_NUTTCP

dal_param_t NUTTCP_param[] = {
	{"ServerIp",	dalType_string,	0,	0,	NULL,NULL,NULL},
	{"DiagnosticsState",dalType_string,0,0,NULL,NULL,NULL},
	{"Receive",	dalType_boolean,	0,	0,NULL,NULL,	NULL},
	{"Transmit",	dalType_boolean,	0,	0,NULL,NULL,	NULL},
	{"UDP",       dalType_boolean,    0,      0,     NULL,NULL, NULL},
	{"PORT",       dalType_int,    0,      0,      NULL,NULL, NULL},
	{"Interval",       dalType_int,    0,      0,  NULL,NULL,    NULL},
	{"Length",       dalType_int,    0,      0,   NULL,NULL,   NULL},
	{"Timeout",       dalType_int,    0,      0, NULL,NULL,     NULL},
	{"NumberOfStream",       dalType_int,    0,      0,   NULL,NULL,   NULL},
	{"OWD",       dalType_boolean,    0,      0,  NULL,NULL,    NULL},
	{"Active",       dalType_boolean,    0,      0, NULL,NULL,     NULL},
	{"Clear",       dalType_boolean,    0,      0,  NULL,NULL,    NULL},
	{"Rate",      dalType_string, 0,      0,   NULL,NULL,   NULL},
	{"Result",      dalType_string, 0,      0,  NULL,NULL,    NULL},
	{NULL,      0, 0,      0,      NULL,NULL,NULL},
};
#endif
unsigned int type;
const char* ProtocolVersion;
const char* Host;
const char* DiagnosticsState;
unsigned int NumberOfRepetitions;
const char* HostName;
const char* DNSServer;
bool invalidInput = false;

struct json_object *diagIPPingJobj;
struct json_object *diagTraceRTobj;
struct json_object *dnsNSLookUpobj;

objIndex_t IpDiagIPPingIid;
objIndex_t IpDiagTraceRTIid;
objIndex_t DNSDiagNSLookUpIid;

void freeAllPingTestObjects(){
	if(diagIPPingJobj) json_object_put(diagIPPingJobj);
	if(diagTraceRTobj) json_object_put(diagTraceRTobj);
	if(dnsNSLookUpobj) json_object_put(dnsNSLookUpobj);
	return;
}

void initPingTestGlobalObjects(){
	diagIPPingJobj = NULL;
	diagTraceRTobj = NULL;
	dnsNSLookUpobj = NULL;
	return;
}

void getPingTestType(struct json_object *Jobj){
	if( NULL != json_object_object_get(Jobj, "type") ){
		type = json_object_get_int(json_object_object_get(Jobj, "type"));
	}
}

void getIpDiagIPPingBasicInfo(struct json_object *Jobj){
	if( NULL != json_object_object_get(Jobj, "ProtocolVersion") ){
		ProtocolVersion = json_object_get_string(json_object_object_get(Jobj, "ProtocolVersion"));
	}
	if( NULL != json_object_object_get(Jobj, "Host") ){
		Host = json_object_get_string(json_object_object_get(Jobj, "Host"));
		invalidCharFilter((char *)Host, 41);
		if(DomainFilter(Host) == true ||
			IPFilter(Host) == true ||
			IPv6Filter(Host) == true){
			invalidInput = false;
		}
		else
			invalidInput = true;
	}
	if( NULL != json_object_object_get(Jobj, "DiagnosticsState") ){
		DiagnosticsState = json_object_get_string(json_object_object_get(Jobj, "DiagnosticsState"));
	}
	if( NULL != json_object_object_get(Jobj, "NumberOfRepetitions") ){
		NumberOfRepetitions = json_object_get_int(json_object_object_get(Jobj, "NumberOfRepetitions"));
	}
}

void getIpDiagTraceRTBasicInfo(struct json_object *Jobj){
	if( NULL != json_object_object_get(Jobj, "ProtocolVersion") ){
		ProtocolVersion = json_object_get_string(json_object_object_get(Jobj, "ProtocolVersion"));
	}
	if( NULL != json_object_object_get(Jobj, "Host") ){
		Host = json_object_get_string(json_object_object_get(Jobj, "Host"));
		invalidCharFilter((char *)Host, 41);
		if(DomainFilter(Host) == true ||
			IPFilter(Host) == true ||
			IPv6Filter(Host) == true){
			invalidInput = false;
		}
		else
			invalidInput = true;
	}
	if( NULL != json_object_object_get(Jobj, "DiagnosticsState") ){
		DiagnosticsState = json_object_get_string(json_object_object_get(Jobj, "DiagnosticsState"));
	}
	if( NULL != json_object_object_get(Jobj, "NumberOfRepetitions") ){
		NumberOfRepetitions = json_object_get_int(json_object_object_get(Jobj, "NumberOfRepetitions"));
	}
}

void getDNSDiagNSLookUpgBasicInfo(struct json_object *Jobj){
	if( NULL != json_object_object_get(Jobj, "HostName") ){
		HostName = json_object_get_string(json_object_object_get(Jobj, "HostName"));
	}else if( NULL != json_object_object_get(Jobj, "Host") ){
		HostName = json_object_get_string(json_object_object_get(Jobj, "Host"));
	}
	invalidCharFilter((char *)HostName, 41);
	if(DomainFilter(HostName) == true ||
			IPFilter(HostName) == true ||
			IPv6Filter(HostName) == true){
			invalidInput = false;
	}
	else
		invalidInput = true;

	if( NULL != json_object_object_get(Jobj, "DiagnosticsState") ){
		DiagnosticsState = json_object_get_string(json_object_object_get(Jobj, "DiagnosticsState"));
	}
	if( NULL != json_object_object_get(Jobj, "DiagnosticsState") ){
		DNSServer = json_object_get_string(json_object_object_get(Jobj, "DNSServer"));
	}
}

zcfgRet_t zcfgFeDalIpDiagIPPingEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	ZLOG_INFO("Enter.. \n ");
	getIpDiagIPPingBasicInfo(Jobj);
	if(invalidInput){
		printf("\ninvalid input...\n");
		return ZCFG_INTERNAL_ERROR;
	}

	IID_INIT(IpDiagIPPingIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, &diagIPPingJobj)) != ZCFG_SUCCESS) {
		ZLOG_INFO("(ERROR) Get ip daig ip ping Obj fail\n");
		return ZCFG_INTERNAL_ERROR;
	}

	if(diagIPPingJobj != NULL){
		json_object_object_add(diagIPPingJobj, "ProtocolVersion", json_object_new_string(ProtocolVersion));
		json_object_object_add(diagIPPingJobj, "Host", json_object_new_string(Host));
		json_object_object_add(diagIPPingJobj, "DiagnosticsState", json_object_new_string(DiagnosticsState));
		json_object_object_add(diagIPPingJobj, "NumberOfRepetitions", json_object_new_int(NumberOfRepetitions));

		//if((ret = zcfgFeObjJsonSet(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, diagIPPingJobj, NULL)) != ZCFG_SUCCESS)
		const char *ipping;
		ipping = json_object_to_json_string(diagIPPingJobj);
		if((ret = zcfgFeObjWrite(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, ipping, REQUEST_SET_OBJ | ZCFG_PARTIAL_OBJECT_SET, NULL)) != ZCFG_SUCCESS)
		{
			ZLOG_INFO("(ERROR) Set ip daig ip ping fail\n");
			return ZCFG_INTERNAL_ERROR;
		}
	}

	return ret;
}

zcfgRet_t zcfgFeDalIpDiagTraceRTEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	ZLOG_INFO("Enter.. \n ");
	getIpDiagTraceRTBasicInfo(Jobj);
	if(invalidInput){
		printf("\ninvalid input...\n");
		return ZCFG_INTERNAL_ERROR;
	}

	IID_INIT(IpDiagTraceRTIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_TRACE_RT, &IpDiagTraceRTIid, &diagTraceRTobj)) != ZCFG_SUCCESS) {
		ZLOG_INFO("(ERROR) Get ip daig trace RT Obj fail\n");
		return ZCFG_INTERNAL_ERROR;
	}

	if(diagTraceRTobj != NULL){
		json_object_object_add(diagTraceRTobj, "ProtocolVersion", json_object_new_string(ProtocolVersion));
		json_object_object_add(diagTraceRTobj, "Host", json_object_new_string(Host));
		json_object_object_add(diagTraceRTobj, "DiagnosticsState", json_object_new_string(DiagnosticsState));
		json_object_object_add(diagTraceRTobj, "NumberOfRepetitions", json_object_new_int(NumberOfRepetitions));

		if((ret = zcfgFeObjJsonSet(RDM_OID_IP_DIAG_TRACE_RT, &IpDiagTraceRTIid, diagTraceRTobj, NULL)) != ZCFG_SUCCESS)
		{
			ZLOG_INFO("(ERROR) Set ip daig trace RT fail\n");
			return ZCFG_INTERNAL_ERROR;
		}
	}

	return ret;
}

zcfgRet_t zcfgFeDalDNSDiagNSLookUpEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	ZLOG_INFO("Enter.. \n ");
	getDNSDiagNSLookUpgBasicInfo(Jobj);
	if(invalidInput){
		printf("\ninvalid input...\n");
		return ZCFG_INTERNAL_ERROR;
	}


	IID_INIT(DNSDiagNSLookUpIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_DNS_DIAG_NS_LOOKUP_DIAG, &DNSDiagNSLookUpIid, &dnsNSLookUpobj)) != ZCFG_SUCCESS) {
		ZLOG_INFO("(ERROR) Get DNS diag NS Lookup Obj fail\n");
		return ZCFG_INTERNAL_ERROR;
	}

	if(dnsNSLookUpobj != NULL){
		if( (NULL != json_object_object_get(Jobj, "HostName")) || (NULL != json_object_object_get(Jobj, "Host"))){
			json_object_object_add(dnsNSLookUpobj, "HostName", json_object_new_string(HostName));
		}
		if( NULL != json_object_object_get(Jobj, "DiagnosticsState")){
			json_object_object_add(dnsNSLookUpobj, "DiagnosticsState", json_object_new_string(DiagnosticsState));
		}
		if( NULL != json_object_object_get(Jobj, "DNSServer")){
			json_object_object_add(dnsNSLookUpobj, "DNSServer", json_object_new_string(DNSServer));
		}
		if((ret = zcfgFeObjJsonSet(RDM_OID_DNS_DIAG_NS_LOOKUP_DIAG, &DNSDiagNSLookUpIid, dnsNSLookUpobj, NULL)) != ZCFG_SUCCESS)
		{
			ZLOG_INFO("(ERROR) Set DNS diag NS Lookup fail\n");
			return ZCFG_INTERNAL_ERROR;
		}
	}

	return ret;
}
#ifdef TELIA_CUSTOMIZATION //Add Cancel button to cancel function for long running processes
zcfgRet_t zcfgFeDalDiagCancelEdit(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	ZLOG_INFO("Enter.. \n ");
	
	//ping
	getIpDiagIPPingBasicInfo(Jobj);
	
	IID_INIT(IpDiagIPPingIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, &diagIPPingJobj)) != ZCFG_SUCCESS) {
		ZLOG_INFO("(ERROR) Get ip daig ip ping Obj fail\n");
		return ZCFG_INTERNAL_ERROR;
	}
	
	if(diagIPPingJobj != NULL){
		if( NULL != json_object_object_get(Jobj, "DiagnosticsState")){
			json_object_object_add(diagIPPingJobj, "DiagnosticsState", json_object_new_string(DiagnosticsState));
		}

		//if((ret = zcfgFeObjJsonSet(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, diagIPPingJobj, NULL)) != ZCFG_SUCCESS)
		const char *ipping;
		ipping = json_object_to_json_string(diagIPPingJobj);
		if((ret = zcfgFeObjWrite(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, ipping, REQUEST_SET_OBJ | ZCFG_PARTIAL_OBJECT_SET, NULL)) != ZCFG_SUCCESS)
		{
			ZLOG_INFO("(ERROR) Set ip daig ip ping fail\n");
			return ZCFG_INTERNAL_ERROR;
		}
	}
	
	//traceroute
	getIpDiagTraceRTBasicInfo(Jobj);
	
	IID_INIT(IpDiagTraceRTIid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_TRACE_RT, &IpDiagTraceRTIid, &diagTraceRTobj)) != ZCFG_SUCCESS) {
		ZLOG_INFO("(ERROR) Get ip daig trace RT Obj fail\n");
		return ZCFG_INTERNAL_ERROR;
	}
	
	if(diagTraceRTobj != NULL){
		if( NULL != json_object_object_get(Jobj, "DiagnosticsState")){
			json_object_object_add(diagTraceRTobj, "DiagnosticsState", json_object_new_string(DiagnosticsState));
		}

		if((ret = zcfgFeObjJsonSet(RDM_OID_IP_DIAG_TRACE_RT, &IpDiagTraceRTIid, diagTraceRTobj, NULL)) != ZCFG_SUCCESS)
		{
			ZLOG_INFO("(ERROR) Set ip daig trace RT fail\n");
			return ZCFG_INTERNAL_ERROR;
		}
	}
	return ret;
}
#endif //TELIA_CUSTOMIZATION

#ifdef PACKAGE_SpeedTest
#define ZY_EXT_SPEED_TEST 8
zcfgRet_t zcfgFeDalZyExtSpeedTestGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char* diagsState;
	bool speedTestStart = false;
	objIndex_t zyExtSpeedTestIid = {0};
	struct json_object *paramJobj = NULL, *zyExtSpeedTestObj = NULL;
	paramJobj = json_object_new_object();

	ZLOG_INFO("%s : Enter\n", __func__);

	IID_INIT(zyExtSpeedTestIid);
	if((ret =  zcfgFeObjJsonGet(RDM_OID_SPEED_TEST, &zyExtSpeedTestIid, &zyExtSpeedTestObj)) == ZCFG_SUCCESS){
		speedTestStart = json_object_get_boolean(json_object_object_get(zyExtSpeedTestObj, "Start"));
		if(speedTestStart)
			diagsState = "Requested";
		else
			diagsState = "Complete";
		json_object_object_add(paramJobj, "DiagnosticsState", json_object_new_string(diagsState));
		zcfgFeJsonObjFree(zyExtSpeedTestObj);
	}

	json_object_array_add(Jarray, paramJobj);
	return ret;
}

zcfgRet_t zcfgFeDalZyExtSpeedTestEdit(struct json_object *Jobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char* diagsState;
	bool speedTestStart = false;
	objIndex_t zyExtSpeedTestIid = {0};
	struct json_object *zyExtSpeedTestObj = NULL;

	ZLOG_INFO("%s : Enter\n", __func__);

	IID_INIT(zyExtSpeedTestIid);
	diagsState = json_object_get_string(json_object_object_get(Jobj, "DiagnosticsState"));
	if(!strcmp(diagsState, "Requested"))
		speedTestStart = true;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_SPEED_TEST, &zyExtSpeedTestIid, &zyExtSpeedTestObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "DiagnosticsState"))
			json_object_object_add(zyExtSpeedTestObj, "Start", json_object_new_boolean(speedTestStart));
		zcfgFeObjJsonSet(RDM_OID_SPEED_TEST, &zyExtSpeedTestIid, zyExtSpeedTestObj, NULL);
	}

	zcfgFeJsonObjFree(zyExtSpeedTestObj);

	return ret;
}
#endif //PACKAGE_SpeedTest

zcfgRet_t zcfgFeDalPingTestEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	ZLOG_INFO("input --- %s \n ",json_object_to_json_string(Jobj));

	initPingTestGlobalObjects();
	getPingTestType(Jobj);

	if(type == IP_DIAG_IPPING ){
		if((ret = zcfgFeDalIpDiagIPPingEdit(Jobj)) != ZCFG_SUCCESS){
			ZLOG_INFO("config I pDiag IP Ping fail\n");
			goto exit;
		}
	}else if(type == IP_DIAG_TRACE_RT){
		if((ret = zcfgFeDalIpDiagTraceRTEdit(Jobj)) != ZCFG_SUCCESS){
			ZLOG_INFO("config Ip Diag Trace RT fail\n");
			goto exit;
		}
	}else if(type == DNS_DIAG_NS_LOOKUP){
		if((ret = zcfgFeDalDNSDiagNSLookUpEdit(Jobj)) != ZCFG_SUCCESS){
			ZLOG_INFO("config DNS Diag NSL ookUp fail\n");
			goto exit;
		}
	}
#ifdef TELIA_CUSTOMIZATION	//Add Cancel button to cancel function for long running processes
	else if(type == DIAG_CANCEL){
		if((ret = zcfgFeDalDiagCancelEdit(Jobj)) != ZCFG_SUCCESS){
			ZLOG_INFO("cancel diagnostic fail\n");
			goto exit;
		}
	}
#endif
#ifdef PACKAGE_SpeedTest
	else if(type == ZY_EXT_SPEED_TEST){
		if((ret = zcfgFeDalZyExtSpeedTestEdit(Jobj)) != ZCFG_SUCCESS){
			ZLOG_INFO("config Zy Ext Speed Test fail\n");
			return ret;
		}
	}
#endif
exit:
	freeAllPingTestObjects();
	return ret;
}

zcfgRet_t zcfgFeDalPingTestGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	 ZLOG_INFO("input --- %s \n ",json_object_to_json_string(Jobj));
	struct json_object *paramJobj = NULL;
	paramJobj = json_object_new_object();

	getPingTestType(Jobj);

	if(type == IP_DIAG_IPPING ){
		IID_INIT(IpDiagIPPingIid);
		if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_IPPING, &IpDiagIPPingIid, &diagIPPingJobj)) != ZCFG_SUCCESS) {
			ZLOG_INFO("(ERROR) Get ip daig ip ping Obj fail\n");
			ret = ZCFG_INTERNAL_ERROR;
			goto exit;
		}
		json_object_object_add(paramJobj, "DiagnosticsState", JSON_OBJ_COPY(json_object_object_get(diagIPPingJobj, "DiagnosticsState")));
	}else if(type == IP_DIAG_TRACE_RT){
		IID_INIT(IpDiagTraceRTIid);
		if((ret = zcfgFeObjJsonGet(RDM_OID_IP_DIAG_TRACE_RT, &IpDiagTraceRTIid, &diagTraceRTobj)) != ZCFG_SUCCESS) {
			ZLOG_INFO("(ERROR) Get ip daig trace RT Obj fail\n");
			ret = ZCFG_INTERNAL_ERROR;
			goto exit;
		}
		json_object_object_add(paramJobj, "DiagnosticsState", JSON_OBJ_COPY(json_object_object_get(diagTraceRTobj, "DiagnosticsState")));
	}else if(type == DNS_DIAG_NS_LOOKUP){
		IID_INIT(DNSDiagNSLookUpIid);
		if((ret = zcfgFeObjJsonGet(RDM_OID_DNS_DIAG_NS_LOOKUP_DIAG, &DNSDiagNSLookUpIid, &dnsNSLookUpobj)) != ZCFG_SUCCESS) {
			ZLOG_INFO("(ERROR) Get DNS diag NS Lookup Obj fail\n");
			ret = ZCFG_INTERNAL_ERROR;
			goto exit;
		}
		json_object_object_add(paramJobj, "DiagnosticsState", JSON_OBJ_COPY(json_object_object_get(dnsNSLookUpobj, "DiagnosticsState")));
	}
#ifdef PACKAGE_SpeedTest
	else if(type == ZY_EXT_SPEED_TEST){
		if((ret = zcfgFeDalZyExtSpeedTestGet(Jobj, Jarray, replyMsg)) != ZCFG_SUCCESS){
			ZLOG_INFO("(ERROR) Get Zy Ext Speed test Obj fail\n");
		}
		return ret;
	}
#endif
	json_object_array_add(Jarray, paramJobj);
exit:
	freeAllPingTestObjects();
	return ret;
}

zcfgRet_t zcfgFeDalPingTest(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "PUT")) {
		ret = zcfgFeDalPingTestEdit(Jobj, NULL);
	}

	if(!strcmp(method, "GET")) {
		ret = zcfgFeDalPingTestGet(Jobj, Jarray, NULL);
	}

	return ret;
}
#ifdef ZYXEL_NUTTCP
void zcfgFeDalNutTcpShow(struct json_object *Jarray){
	printf("\n");
}

objIndex_t NutTcpIid;

struct json_object *NutTcpObj;

zcfgRet_t zcfgFeDalNutTcpSet(struct json_object *Jobj){
        zcfgRet_t ret = ZCFG_SUCCESS;
#if 1
	bool recv, tx, owd, udp, act, clr;
	const char* ip;
	const char* rate;
	const char* result;
	const char* state;
	int port,Interval,len,timout,num;
	IID_INIT(NutTcpIid);
        if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_DIAG_NUT_TCP_DIAG, &NutTcpIid, &NutTcpObj)) != ZCFG_SUCCESS){
                printf("%s():(ERROR) zcfgFeObjJsonGetWithoutUpdate get NutTcp fail oid(%d)\n ",__FUNCTION__,RDM_OID_IP_DIAG_NUT_TCP_DIAG);
                return ZCFG_INTERNAL_ERROR;
        }

        if(NutTcpObj != NULL){
		if(json_object_object_get(Jobj, "ServerIp")){
			ip = json_object_get_string(json_object_object_get(Jobj,"ServerIp"));
	         	json_object_object_add(NutTcpObj, "ServerIp", json_object_new_string(ip));
		}
		if(json_object_object_get(Jobj, "DiagnosticsState")){
			state = json_object_get_string(json_object_object_get(Jobj,"DiagnosticsState"));
			json_object_object_add(NutTcpObj, "DiagnosticsState", json_object_new_string(state));
		}
		if(json_object_object_get(Jobj, "Receive")){
			recv = json_object_get_boolean(json_object_object_get(Jobj,"Receive"));
			json_object_object_add(NutTcpObj, "Receive", json_object_new_boolean(recv));
		}
		if(json_object_object_get(Jobj, "Transmit")){
			tx = json_object_get_boolean(json_object_object_get(Jobj,"Transmit"));
			json_object_object_add(NutTcpObj, "Transmit", json_object_new_boolean(tx));
		}
		if(json_object_object_get(Jobj, "Port")){
			port = json_object_get_int(json_object_object_get(Jobj,"Port"));
			json_object_object_add(NutTcpObj, "Port", json_object_new_int(port));
		}
		if(json_object_object_get(Jobj, "UDP")){
			udp = json_object_get_boolean(json_object_object_get(Jobj,"UDP"));
			json_object_object_add(NutTcpObj, "UDP", json_object_new_boolean(udp));
		}
		if(json_object_object_get(Jobj, "Rate")){
			rate = json_object_get_string(json_object_object_get(Jobj,"Rate"));
			json_object_object_add(NutTcpObj, "Rate", json_object_new_string(rate));
		}
		if(json_object_object_get(Jobj, "Interval")){
			Interval = json_object_get_int(json_object_object_get(Jobj,"Interval"));
			json_object_object_add(NutTcpObj, "Interval", json_object_new_int(Interval));
		}
		if(json_object_object_get(Jobj, "Length")){
			len = json_object_get_int(json_object_object_get(Jobj,"Length"));
			json_object_object_add(NutTcpObj, "Length", json_object_new_int(len));
		}
		if(json_object_object_get(Jobj, "Timeout")){
			timout = json_object_get_int(json_object_object_get(Jobj,"Timeout"));
			json_object_object_add(NutTcpObj, "Timeout", json_object_new_int(timout));
		}
		if(json_object_object_get(Jobj, "NumberOfStream")){
			num = json_object_get_int(json_object_object_get(Jobj,"NumberOfStream"));
			json_object_object_add(NutTcpObj, "NumberOfStream", json_object_new_int(num));
		}
		if(json_object_object_get(Jobj, "OWD")){
			owd = json_object_get_boolean(json_object_object_get(Jobj,"OWD"));
			json_object_object_add(NutTcpObj, "OWD", json_object_new_boolean(owd));
		}
		if(json_object_object_get(Jobj, "Clear")){
			clr = json_object_get_boolean(json_object_object_get(Jobj,"Clear"));
			json_object_object_add(NutTcpObj, "Clear", json_object_new_boolean(clr));
		}
		if(json_object_object_get(Jobj, "Active")){
			act = json_object_get_boolean(json_object_object_get(Jobj,"Active"));
			json_object_object_add(NutTcpObj, "Active", json_object_new_boolean(act));
		}
		zcfgFeObjJsonBlockedSet(RDM_OID_IP_DIAG_NUT_TCP_DIAG, &NutTcpIid, NutTcpObj, NULL);
        }
	zcfgFeJsonObjFree(NutTcpObj);
#endif
        return ret;
}
zcfgRet_t zcfgFeDalNutTcpGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
        zcfgRet_t ret = ZCFG_SUCCESS;
	rdm_IpDiagNutTcpDiag_t *nutObj = NULL;
        struct json_object *paramJobj = NULL;
        struct json_object *NutTcpJobj = NULL;
        objIndex_t objIid = {0};
        int count = 1;
	IID_INIT(NutTcpIid);
        ret = zcfgFeObjStructGet(RDM_OID_IP_DIAG_NUT_TCP_DIAG, &NutTcpIid, (void **)&nutObj);
	if (ret  == ZCFG_SUCCESS){
#if 1
		printf("ServerIp is %s\n",nutObj->ServerIp);
		printf("DiagnosticsState = %s,\n Result = %s\n",nutObj->DiagnosticsState,nutObj->Result);
#endif

		int abc = RDM_OID_IP_DIAG_NUT_TCP_DIAG;
//		printf("111OID is %d, %x, obj = %s\n", abc,abc,json_object_to_json_string(NutTcpJobj));
	}else
		printf("get fail\n\n\n");
	if(nutObj != NULL){
        	zcfgFeObjStructFree(nutObj);
        }
	zcfgFeJsonObjFree(NutTcpJobj);
        return 0;
}
zcfgRet_t zcfgFeDalNutTcp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
//	printf("%s():--- method is %s Json Object --- \n%s \n ",__FUNCTION__,method,json_object_to_json_string(Jobj));
	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;
	if(!strcmp(method, "PUT")) {
		ret = zcfgFeDalNutTcpSet(Jobj);
	}
	if(!strcmp(method, "GET")) {
		ret = zcfgFeDalNutTcpGet(Jobj, Jarray, NULL);
	}
	return ret;
}
#endif
