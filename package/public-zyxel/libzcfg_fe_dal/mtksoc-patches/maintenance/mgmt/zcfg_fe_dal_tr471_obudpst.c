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


dal_param_t TR471_OBUDPST_param[] = {
	{"DiagnosticsState",	dalType_string,		0,	0,		NULL,	"Requested|Canceled",	NULL},
	{"Interface",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"Role",	dalType_string,		0,	0,		NULL,	"Receiver|Sender",	NULL},
	{"Host",	dalType_string,		0,	0,		NULL,	NULL,	NULL},
	{"Port",	dalType_int,		1,	65535,		NULL,	NULL,	NULL},
	{"JumboFramesPermitted",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
	{"DSCP",	dalType_int,		0,	63,		NULL,	NULL,	NULL},
	{"ProtocolVersion",	dalType_string,		0,	0,		NULL,	"Any|IPv4|IPv6",	NULL},
	{"UDPPayloadMin",	dalType_int,		35,	8972,		NULL,	NULL,	NULL},
	{"UDPPayloadMax",	dalType_int,		35,	8972,		NULL,	NULL,	NULL},
	{"UDPPayloadContent",	dalType_string,		0,	0,		NULL,	"random",	NULL},
	{"PortMin",	dalType_int,		49152,	65535,		NULL,	NULL,	NULL},
	{"PortMax",	dalType_int,		49152,	65535,		NULL,	NULL,	NULL},
	{"PortOptionalMin",	dalType_int,		0,	0,		NULL,	"0|1024 - 49151",	NULL},
	{"PortOptionalMax",	dalType_int,		0,	0,		NULL,	"0|1024 - 49151",	NULL},
	{"TestType",	dalType_string,		0,	0,		NULL,	"Search|Fixed",	NULL},
	{"IPDVEnable",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
	{"IPRREnable",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
	{"RIPREnable",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
	{"StartSendingRate",	dalType_int,		500,	10000000,		NULL,	NULL,	NULL},
	{"NumberTestSubIntervals",	dalType_int,		1,	100,		NULL,	NULL,	NULL},
	{"NumberFirstModeTestSubIntervals",	dalType_int,		0,	100,		NULL,	NULL,	NULL},
	{"TestSubInterval",	dalType_int,		100,	6000,		NULL,	NULL,	NULL},
	{"StatusFeedbackInterval",	dalType_int,		5,	250,		NULL,	NULL,	NULL},
	{"TimeoutNoTestTraffic",	dalType_int,		500,	1000,		NULL,	NULL,	NULL},
	{"TimeoutNoStatusMessage",	dalType_int,		500,	1000,		NULL,	NULL,	NULL},
	{"Tmax",	dalType_int,		50,	3000,		NULL,	NULL,	NULL},
	{"TmaxRTT",	dalType_int,		50,	3000,		NULL,	NULL,	NULL},
	{"SeqErrThresh",	dalType_int,		0,	100,		NULL,	NULL,	NULL},
	{"ReordDupIgnoreEnable",	dalType_boolean,		0,	0,		NULL,	NULL,	NULL},
	{"LowerThresh",	dalType_int,		5,	250,		NULL,	NULL,	NULL},
	{"UpperThresh",	dalType_int,		5,	250,		NULL,	NULL,	NULL},
	{"HighSpeedDelta",	dalType_int,		0,	0,		NULL,	"min:2",	NULL},
	{"SlowAdjThresh",	dalType_int,		0,	0,		NULL,	"min:2",	NULL},
	{"HSpeedThresh",	dalType_int,		0,	0,		NULL,	"min:1",	NULL},
#if 0 // not supported yet
	{"NumberOfConnections",	dalType_int,		0,	0,		NULL,	NULL,	NULL},
	{"EthernetPriority",	dalType_int,		0,	0,		NULL,	NULL,	NULL},
	{"PreambleDuration",	dalType_int,		0,	0,		NULL,	NULL,	NULL},
	{"TimeoutNoTraffic",	dalType_int,		0,	0,		NULL,	NULL,	NULL},
	{"TimestampResolution",	dalType_int,		0,	0,		NULL,	NULL,	NULL},
#endif
	{NULL,			0,					0,	0,		NULL,	NULL,	NULL}
};

/*
typedef struct rdm_IpDiagIpLayerCapMetrics_s {   // RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS 
	char DiagnosticsState[33];
	uint32_t IPLayerMaxConnections;
	uint32_t IPLayerMaxIncrementalResult;
	char IPLayerCapSupportedSoftwareVersion[65];
	char IPLayerCapSupportedControlProtocolVersion[65];
	char IPLayerCapSupportedMetrics[33];
	char Interface[257];
	char Role[17];
	char Host[257];
	uint32_t Port;
	bool JumboFramesPermitted;
	uint32_t NumberOfConnections;
	uint32_t EthernetPriority;
	uint32_t DSCP;
	char ProtocolVersion[9];
	uint32_t UDPPayloadMin;
	uint32_t UDPPayloadMax;
	char UDPPayloadContent[33];
	uint32_t PortMin;
	uint32_t PortMax;
	uint32_t PortOptionalMin;
	uint32_t PortOptionalMax;
	char TestType[9];
	bool IPDVEnable;
	bool IPRREnable;
	bool RIPREnable;
	uint32_t PreambleDuration;
	uint64_t StartSendingRate;
	uint32_t NumberTestSubIntervals;
	uint32_t NumberFirstModeTestSubIntervals;
	uint32_t TestSubInterval;
	uint32_t StatusFeedbackInterval;
	uint32_t TimeoutNoTestTraffic;
	uint32_t TimeoutNoStatusMessage;
	uint32_t Tmax;
	uint32_t TmaxRTT;
	uint32_t TimestampResolution;
	uint32_t SeqErrThresh;
	bool ReordDupIgnoreEnable;
	uint32_t LowerThresh;
	uint32_t UpperThresh;
	uint32_t HighSpeedDelta;
	uint32_t SlowAdjThresh;
	uint32_t HSpeedThresh;
	char BOMTime[32];
	char EOMTime[32];
	uint32_t TmaxUsed;
	uint32_t TestInterval;
	char MaxIPLayerCapacity[97];
	char TimeOfMax[32];
	char MaxETHCapacityNoFCS[97];
	char MaxETHCapacityWithFCS[97];
	char MaxETHCapacityWithFCSVLAN[97];
	char LossRatioAtMax[97];
	char RTTRangeAtMax[97];
	char PDVRangeAtMax[97];
	char MinOnewayDelayAtMax[97];
	char ReorderedRatioAtMax[97];
	char ReplicatedRatioAtMax[97];
	char InterfaceEthMbpsAtMax[97];
	char IPLayerCapacitySummary[97];
	char LossRatioSummary[97];
	char RTTRangeSummary[97];
	char PDVRangeSummary[97];
	char MinOnewayDelaySummary[97];
	char MinRTTSummary[97];
	char ReorderedRatioSummary[97];
	char ReplicatedRatioSummary[97];
	char InterfaceEthMbpsSummary[97];
	uint32_t IncrementalResultNumberOfEntries;
	uint32_t ModalResultNumberOfEntries;
	uint32_t TmaxRTTUsed;
	uint32_t TimestampResolutionUsed;
	char X_ZYXEL_Creator[23];
} rdm_IpDiagIpLayerCapMetrics_t;
*/

void zcfgFeDalShowTR471Obudpst(struct json_object *Jarray) {
	struct json_object *Jobj;


	if (json_object_get_type(Jarray) != json_type_array) {
		printf("wrong Jobj format!\n");
		return;
	}

	Jobj = json_object_array_get_idx(Jarray, 0);
	printf("TR-471 Obudpst Information:\n");
	printf("%-45s %s\n", "DiagnosticsState", json_object_get_string(json_object_object_get(Jobj, "DiagnosticsState")));
	printf("%-45s %s\n", "Host", json_object_get_string(json_object_object_get(Jobj, "Host")));
	printf("%-45s %s\n", "Role", json_object_get_string(json_object_object_get(Jobj, "Role")));
	printf("%-45s %s\n", "Interface", json_object_get_string(json_object_object_get(Jobj, "Interface")));
	printf("%-45s %s\n", "JumboFramesPermitted", json_object_get_string(json_object_object_get(Jobj, "JumboFramesPermitted")));
	printf("%-45s %s\n", "DSCP", json_object_get_string(json_object_object_get(Jobj, "DSCP")));
	printf("%-45s %s\n", "ProtocolVersion", json_object_get_string(json_object_object_get(Jobj, "ProtocolVersion")));
	printf("%-45s %s\n", "UDPPayloadMin", json_object_get_string(json_object_object_get(Jobj, "UDPPayloadMin")));
	printf("%-45s %s\n", "UDPPayloadMax", json_object_get_string(json_object_object_get(Jobj, "UDPPayloadMax")));
	printf("%-45s %s\n", "UDPPayloadContent", json_object_get_string(json_object_object_get(Jobj, "UDPPayloadContent")));
	printf("%-45s %s\n", "PortMin", json_object_get_string(json_object_object_get(Jobj, "PortMin")));
	printf("%-45s %s\n", "PortMax", json_object_get_string(json_object_object_get(Jobj, "PortMax")));
	printf("%-45s %s\n", "PortOptionalMin", json_object_get_string(json_object_object_get(Jobj, "PortOptionalMin")));
	printf("%-45s %s\n", "PortOptionalMax", json_object_get_string(json_object_object_get(Jobj, "PortOptionalMax")));
	printf("%-45s %s\n", "TestType", json_object_get_string(json_object_object_get(Jobj, "TestType")));
	printf("%-45s %s\n", "IPDVEnable", json_object_get_string(json_object_object_get(Jobj, "IPDVEnable")));
	printf("%-45s %s\n", "IPRREnable", json_object_get_string(json_object_object_get(Jobj, "IPRREnable")));
	printf("%-45s %s\n", "RIPREnable", json_object_get_string(json_object_object_get(Jobj, "RIPREnable")));
	printf("%-45s %s\n", "StartSendingRate", json_object_get_string(json_object_object_get(Jobj, "StartSendingRate")));
	printf("%-45s %s\n", "NumberTestSubIntervals", json_object_get_string(json_object_object_get(Jobj, "NumberTestSubIntervals")));
	printf("%-45s %s\n", "NumberFirstModeTestSubIntervals", json_object_get_string(json_object_object_get(Jobj, "NumberFirstModeTestSubIntervals")));
	printf("%-45s %s\n", "TestSubInterval", json_object_get_string(json_object_object_get(Jobj, "TestSubInterval")));
	printf("%-45s %s\n", "StatusFeedbackInterval", json_object_get_string(json_object_object_get(Jobj, "StatusFeedbackInterval")));
	printf("%-45s %s\n", "TimeoutNoTestTraffic", json_object_get_string(json_object_object_get(Jobj, "TimeoutNoTestTraffic")));
	printf("%-45s %s\n", "TimeoutNoStatusMessage", json_object_get_string(json_object_object_get(Jobj, "TimeoutNoStatusMessage")));
	printf("%-45s %s\n", "Tmax", json_object_get_string(json_object_object_get(Jobj, "Tmax")));
	printf("%-45s %s\n", "TmaxRTT", json_object_get_string(json_object_object_get(Jobj, "TmaxRTT")));
	printf("%-45s %s\n", "SeqErrThresh", json_object_get_string(json_object_object_get(Jobj, "SeqErrThresh")));
	printf("%-45s %s\n", "ReordDupIgnoreEnable", json_object_get_string(json_object_object_get(Jobj, "ReordDupIgnoreEnable")));
	printf("%-45s %s\n", "LowerThresh", json_object_get_string(json_object_object_get(Jobj, "LowerThresh")));
	printf("%-45s %s\n", "UpperThresh", json_object_get_string(json_object_object_get(Jobj, "UpperThresh")));
	printf("%-45s %s\n", "HighSpeedDelta", json_object_get_string(json_object_object_get(Jobj, "HighSpeedDelta")));
	printf("%-45s %s\n", "SlowAdjThresh", json_object_get_string(json_object_object_get(Jobj, "SlowAdjThresh")));
	printf("%-45s %s\n", "HSpeedThresh", json_object_get_string(json_object_object_get(Jobj, "HSpeedThresh")));
}


static zcfgRet_t zcfgFeDalTR471ObudpstGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t objIid;
	struct json_object *getAllReqTR471Objs = NULL;

	struct json_object *ipLayerMetricsObj = NULL, *ipLayerMetricsArray = NULL;
	struct json_object *ipLayerModalResultObj = NULL, *ipLayerModalResultArray = NULL;
	struct json_object *ipLayerIncrementalResultObj = NULL, *ipLayerIncrementalResultArray = NULL;
	bool getData = FALSE;

	//getAllReqTR471Objs = json_object_new_object();

	IID_INIT(objIid);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS, &objIid, &getAllReqTR471Objs)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS retrieve fail\n", __FUNCTION__);
		return ret;
	}
	json_object_array_add(Jarray, getAllReqTR471Objs);

	//possible result output
	ipLayerModalResultArray = json_object_new_array();
	getData = FALSE;
	IID_INIT(objIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_IP_DIAG_IP_LAYER_MODAL, &objIid, &ipLayerModalResultObj) == ZCFG_SUCCESS) {
		json_object_array_add(ipLayerModalResultArray, ipLayerModalResultObj);
		getData = true;
	}
	if(getData){
		json_object_object_add(getAllReqTR471Objs, "ipLayerCapMetricsModalResult", ipLayerModalResultArray);
	}

	//possible result output
	ipLayerIncrementalResultArray = json_object_new_array();
	getData = FALSE;
	IID_INIT(objIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_IP_DIAG_IP_LAYER_INCRE_RES, &objIid, &ipLayerIncrementalResultObj) == ZCFG_SUCCESS) {
		json_object_array_add(ipLayerIncrementalResultArray, ipLayerIncrementalResultObj);
		getData = true;
	}
	if(getData){
		json_object_object_add(getAllReqTR471Objs, "ipLayerCapMetricsIncrementalResult", ipLayerIncrementalResultArray);
	}

	return ret;
}


static zcfgRet_t zcfgFeDalTR471ObudpstSet(struct json_object *paramObj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *paraName;
	objIndex_t objIid;
	struct json_object *tr471ObudpstJobj = NULL;
	int paramModified = 0, n = 0;

	paraName = (const char *)TR471_OBUDPST_param[n++].paraName;

	IID_INIT(objIid);
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS, &objIid, &tr471ObudpstJobj)) != ZCFG_SUCCESS) {
		printf("%s: Object RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS retrieve fail\n", __FUNCTION__);
		return ret;
	}

	while (paraName) {
		struct json_object *paramValue = json_object_object_get(paramObj, paraName);
		if (!paramValue) {
			paraName = (const char *)TR471_OBUDPST_param[n++].paraName;
			continue;
		}

		if (!strcmp(paraName, "DiagnosticsState")) {
			const char *state = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "DiagnosticsState", json_object_new_string(state));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "Interface")) {
			const char *Interface = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "Interface", json_object_new_string(Interface));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "Role")) {
			const char *role = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "Role", json_object_new_string(role));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "Host")) {
			const char *host = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "Host", json_object_new_string(host));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "Port")) {
			const int Port = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "Port", json_object_new_int(Port));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "NumberFirstModeTestSubIntervals")) {
			const int NumberFirstModeTestSubIntervals = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "NumberFirstModeTestSubIntervals", json_object_new_int(NumberFirstModeTestSubIntervals));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "JumboFramesPermitted")) {
			const bool JumboFramesPermitted = json_object_get_boolean(paramValue);
			json_object_object_add(tr471ObudpstJobj, "JumboFramesPermitted", json_object_new_boolean(JumboFramesPermitted));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "DSCP")) {
			const int DSCP = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "DSCP", json_object_new_int(DSCP)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "ProtocolVersion")) {
			const char *ProtocolVersion = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "ProtocolVersion", json_object_new_string(ProtocolVersion));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "UDPPayloadMin")) {
			const int UDPPayloadMin = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "UDPPayloadMin", json_object_new_int(UDPPayloadMin)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "UDPPayloadMax")) {
			const int UDPPayloadMax = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "UDPPayloadMax", json_object_new_int(UDPPayloadMax)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "UDPPayloadContent")) {
			const char *UDPPayloadContent = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "UDPPayloadContent", json_object_new_string(UDPPayloadContent));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "PortMin")) {
			const int PortMin = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "PortMin", json_object_new_int(PortMin)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "PortMax")) {
			const int PortMax = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "PortMax", json_object_new_int(PortMax)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "PortOptionalMin")) {
			const int PortOptionalMin = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "PortOptionalMin", json_object_new_int(PortOptionalMin)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "PortOptionalMax")) {
			const int PortOptionalMax = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "PortOptionalMax", json_object_new_int(PortOptionalMax)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "TestType")) {
			const char *TestType = json_object_get_string(paramValue);
			json_object_object_add(tr471ObudpstJobj, "TestType", json_object_new_string(TestType));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "IPDVEnable")) {
			const bool IPDVEnable = json_object_get_boolean(paramValue);
			json_object_object_add(tr471ObudpstJobj, "IPDVEnable", json_object_new_boolean(IPDVEnable));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "IPRREnable")) {
			const bool IPRREnable = json_object_get_boolean(paramValue);
			json_object_object_add(tr471ObudpstJobj, "IPRREnable", json_object_new_boolean(IPRREnable));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "RIPREnable")) {
			const bool RIPREnable = json_object_get_boolean(paramValue);
			json_object_object_add(tr471ObudpstJobj, "RIPREnable", json_object_new_boolean(RIPREnable));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "StartSendingRate")) {
			const int StartSendingRate = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "StartSendingRate", json_object_new_int(StartSendingRate)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "NumberTestSubIntervals")) {
			const int NumberTestSubIntervals = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "NumberTestSubIntervals", json_object_new_int(NumberTestSubIntervals)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "TestSubInterval")) {
			const int TestSubInterval = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "TestSubInterval", json_object_new_int(TestSubInterval)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "StatusFeedbackInterval")) {
			const int StatusFeedbackInterval = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "StatusFeedbackInterval", json_object_new_int(StatusFeedbackInterval)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "TimeoutNoTestTraffic")) {
			const int TimeoutNoTestTraffic = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "TimeoutNoTestTraffic", json_object_new_int(TimeoutNoTestTraffic)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "TimeoutNoStatusMessage")) {
			const int TimeoutNoStatusMessage = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "TimeoutNoStatusMessage", json_object_new_int(TimeoutNoStatusMessage)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "Tmax")) {
			const int Tmax = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "Tmax", json_object_new_int(Tmax)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "TmaxRTT")) {
			const int TmaxRTT = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "TmaxRTT", json_object_new_int(TmaxRTT)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "SeqErrThresh")) {
			const int SeqErrThresh = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "SeqErrThresh", json_object_new_int(SeqErrThresh)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "ReordDupIgnoreEnable")) {
			const bool ReordDupIgnoreEnable = json_object_get_boolean(paramValue);
			json_object_object_add(tr471ObudpstJobj, "ReordDupIgnoreEnable", json_object_new_boolean(ReordDupIgnoreEnable));
			paramModified = 1;
		}
		else if (!strcmp(paraName, "LowerThresh")) {
			const int LowerThresh = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "LowerThresh", json_object_new_int(LowerThresh)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "UpperThresh")) {
			const int UpperThresh = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "UpperThresh", json_object_new_int(UpperThresh)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "HighSpeedDelta")) {
			const int HighSpeedDelta = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "HighSpeedDelta", json_object_new_int(HighSpeedDelta)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "SlowAdjThresh")) {
			const int SlowAdjThresh = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "SlowAdjThresh", json_object_new_int(SlowAdjThresh)); 
			paramModified = 1;
		}
		else if (!strcmp(paraName, "HSpeedThresh")) {
			const int HSpeedThresh = json_object_get_int(paramValue);
			json_object_object_add(tr471ObudpstJobj, "HSpeedThresh", json_object_new_int(HSpeedThresh)); 
			paramModified = 1;
		}
		else {
			json_object_object_add(tr471ObudpstJobj, paraName, JSON_OBJ_COPY(paramValue));
			paramModified = 1;
		}

		paraName = (const char *)TR471_OBUDPST_param[n++].paraName;
	}

	if (paramModified) {
		if((ret = zcfgFeObjJsonSet(RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS, &objIid, tr471ObudpstJobj, NULL)) != ZCFG_SUCCESS)
			printf("%s: Write object RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS fail\n", __FUNCTION__);
	}

	json_object_put(tr471ObudpstJobj);

	return ret;
}


zcfgRet_t zcfgFeDalTR471Obudpst(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if (!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if (!strcmp(method, "PUT")) {
		ret = zcfgFeDalTR471ObudpstSet(Jobj, replyMsg);
	}
	else if (!strcmp(method, "POST")) {
		// ret = zcfgFeDalTR471ObudpstSet(Jobj, replyMsg);
	}
	else if (!strcmp(method, "GET")) {
		ret = zcfgFeDalTR471ObudpstGet(Jobj, Jarray, NULL);
	}
	else if (!strcmp(method, "DELETE")) {

	}

	return ret;
}
