#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"

//#define JSON_OBJ_COPY(json_object) json_tokener_parse(json_object_to_json_string(json_object))

dal_param_t WIFI_OTHERS_param[]={
	//WiFi Radio
	{"Band",						dalType_string,		0,	0,	NULL,	"2.4GHz|5GHz",	dal_Edit},
	{"wlRtsThrshld",				dalType_int,		0,	2347,	NULL,	NULL,	0},
	{"wlFrgThrshld",				dalType_int, 		256,	2346,	NULL,	NULL,	0},
	{"AutoChannelTimer",			dalType_int,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlCsScanTimer",				dalType_int,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlTxPwrPcnt",					dalType_int,		0,	0,	NULL,	"20|40|60|80|100",	0},
	{"wlBcnIntvl",					dalType_int,		50,	1000,	NULL,	NULL,	0},
	{"wlDtmIntvl",					dalType_int,		1,	255,	NULL,	NULL,	0},
	{"wlMode",						dalType_wlanMode,	0,	0,	NULL,	NULL,	0},
	{"wlProtect",					dalType_string,		0,	0,	NULL,	"auto|off",	0},
	//{"wlRifsAdvert",				dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlPreamble",					dalType_string, 	0,	0,	NULL,	"long|short",	dalcmd_Forbid},
	{"wlRxChainPwrSave",			dalType_int, 		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	//{"wlObssCox",					dalType_boolean, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlFrameburst",				dalType_boolean,	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlRegMode",					dalType_int,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlDfsPreIsm",					dalType_int, 		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlTpcDb",						dalType_int, 		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"wlDfsPostIsm",				dalType_int, 		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"OperatingChannelBandwidth",	dalType_string, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	//WiFi AP Sec
	{"wlPmf",						dalType_string, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	//WiFi AP WPS
	{"wps_v2",						dalType_boolean, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"atf",							dalType_boolean, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"InActiveChan",				dalType_string, 	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"singleSKU", 					dalType_string, 	0,	0, 	NULL,	NULL,	dalcmd_Forbid},
#ifdef LTE_WLAN_AUTO_SWITCH_OFF
	{"wlAutoOffEnable",				dalType_int,		0,	0,	NULL, 	NULL,	0},
	{"wlAutoOffIntvl",				dalType_int,		0,	0,	NULL,	"15|30|45|60",	0},
#endif
	{NULL,		0,	0,	0,	NULL,	NULL,	0},
};

struct json_object *wifiRadioOthersObj = NULL;
struct json_object *wifiApWpsOthersObj = NULL;
struct json_object *wifiSecOthersObj = NULL;

objIndex_t wifiRadioOthersIid = {0};
objIndex_t wifiApWpsOthersIid = {0};
objIndex_t wifiSecOthersIid = {0};

//WiFi Radio
unsigned int wlRtsThrshld;
unsigned int wlFrgThrshld;
unsigned int AutoChannelTimer;
unsigned int wlCsScanTimer;
unsigned int wlTxPwrPcnt;
unsigned int wlBcnIntvl;
unsigned int wlDtmIntvl;
const char* wlMode;
const char* wlProtect;
//const char* wlRifsAdvert;
const char* wlPreamble;
int wlRxChainPwrSave;
//bool wlObssCox;
bool wlFrameburst;
int wlRegMode;
int wlDfsPreIsm;
int wlTpcDb;
int wlDfsPostIsm;
const char* OperatingChannelBandwidth;
//WiFi AP Sec
const char* wlPmf;
//WiFi AP WPS
bool wps_v2;
bool atf;
const char* InActiveChan = NULL;
const char* singleSKU = NULL;
#ifdef LTE_WLAN_AUTO_SWITCH_OFF
int wlAutoOffEnable;
unsigned int wlAutoOffIntvl;
#endif

void zcfgFeDalShowWifiOthers(struct json_object *Jarray){
	int i, len = 0;
	struct json_object *obj;
	char channel[16] = {0};
	
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		if(obj != NULL){
			printf("%-20s : %s\n","Band",json_object_get_string(json_object_object_get(obj, "Band")));
			printf("-----------------------------------------------\n");
			printf("%-30s : %s\n","RTS/CTS Threshold",json_object_get_string(json_object_object_get(obj, "wlRtsThrshld")));
			printf("%-30s : %s\n","Fragmentation Threshold",json_object_get_string(json_object_object_get(obj, "wlFrgThrshld")));
			printf("%-30s : %s\n","Beacon Interval",json_object_get_string(json_object_object_get(obj, "wlBcnIntvl")));
			printf("%-30s : %s\n","DTIM Interval",json_object_get_string(json_object_object_get(obj, "wlDtmIntvl")));
			printf("%-30s : %s\n","802.11 Mode",json_object_get_string(json_object_object_get(obj, "wlMode")));
			printf("%-30s : %s\n","Output Power",json_object_get_string(json_object_object_get(obj, "wlTxPwrPcnt")));
			printf("%-30s : %s\n","802.11 Protection",json_object_get_string(json_object_object_get(obj, "wlProtect")));
			printf("%-30s : %s\n","Preamble",json_object_get_string(json_object_object_get(obj, "wlPreamble")));
			printf("%-30s : %s\n","Protected Management Frames",json_object_get_string(json_object_object_get(obj, "wlPmf")));
			printf("\n");
		}
	}
	
}

void prepareJObj(struct json_object *Jobj){
	char *band=NULL;
	int ssidIndex;
	struct json_object *SsidObj = NULL;
	struct json_object *RadioObj = NULL;
	objIndex_t SsidIid={0};	
	objIndex_t RadioIid={0};	
	const char *tmpLowerLayer = NULL;
	uint8_t tmpRadioIndex;

	if(json_object_object_get(Jobj, "Band")){
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_RADIO, &RadioIid, &RadioObj) == ZCFG_SUCCESS){
			if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "Band")),json_object_get_string(json_object_object_get(RadioObj, "OperatingFrequencyBand")))){
				break;
			}
			zcfgFeJsonObjFree(RadioObj);
		}
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &SsidIid, &SsidObj) == ZCFG_SUCCESS){
			tmpLowerLayer = json_object_get_string(json_object_object_get(SsidObj, "LowerLayers"));
			sscanf(tmpLowerLayer, "WiFi.Radio.%hhu", &tmpRadioIndex);
			if(RadioIid.idx[0] == tmpRadioIndex){
				ssidIndex = SsidIid.idx[0];
				break;
			}
			zcfgFeJsonObjFree(SsidObj);
		}
		json_object_object_add(Jobj, "Index", json_object_new_int(ssidIndex));
		json_object_object_del(Jobj, "Band");
		zcfgFeJsonObjFree(SsidObj);
		zcfgFeJsonObjFree(RadioObj);
	}
}

zcfgRet_t zcfgFeDalWifiOthersGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t SsidIid={0};
	objIndex_t RadioIid={0};
	objIndex_t APSecIid={0};
	char *wlRtsThrshld=NULL, *wlFrgThrshld=NULL, *wlBcnIntvl=NULL, *wlDtmIntvl=NULL, *wlMode=NULL, *wlProtect=NULL, *wlPreamble=NULL, *wlPmf=NULL;
	bool enablePMF = false;
	int pmfMethod = 0;
	int wlTxPwrPcnt=0;
	struct json_object *SsidObj = NULL;
	struct json_object *RadioObj = NULL;
	struct json_object *APSecObj = NULL;
	struct json_object *pramJobj = NULL;
	const char *tmpLowerLayer = NULL;
	int index;
	uint8_t tmpRadioIndex;
#ifdef LTE_WLAN_AUTO_SWITCH_OFF
	int autoOffIntvl;
#endif

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	SsidIid.level = 1;
	SsidIid.idx[0] = index;
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_SSID, &SsidIid, &SsidObj)) != ZCFG_SUCCESS)
		return ret;
	tmpLowerLayer = json_object_get_string(json_object_object_get(SsidObj, "LowerLayers"));
	sscanf(tmpLowerLayer, "WiFi.Radio.%hhu", &tmpRadioIndex);
	RadioIid.level = 1;
	RadioIid.idx[0] = tmpRadioIndex;
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_RADIO, &RadioIid, &RadioObj)) != ZCFG_SUCCESS)
		return ret;

	APSecIid.level = 1;
	APSecIid.idx[0] = index;
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_ACCESS_POINT_SEC, &APSecIid, &APSecObj)) != ZCFG_SUCCESS)
		return ret;

	pramJobj = json_object_new_object();
	json_object_object_add(pramJobj, "Band", JSON_OBJ_COPY(json_object_object_get(RadioObj, "OperatingFrequencyBand")));
	json_object_object_add(pramJobj, "wlRtsThrshld", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_RTS_CTS_Threshold")));
	json_object_object_add(pramJobj, "wlFrgThrshld", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Fragment_Threshold")));
	json_object_object_add(pramJobj, "wlBcnIntvl", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Beacon_Interval")));
	json_object_object_add(pramJobj, "wlDtmIntvl", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_DTIM_Interval")));
	json_object_object_add(pramJobj, "wlMode", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Wireless_Mode")));
	json_object_object_add(pramJobj, "wlTxPwrPcnt", JSON_OBJ_COPY(json_object_object_get(RadioObj, "TransmitPower")));
	json_object_object_add(pramJobj, "wlProtect", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Protection")));
	json_object_object_add(pramJobj, "wlPreamble", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Preamble")));
	json_object_object_add(pramJobj, "InActiveChan", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_InActiveChannels")));
	json_object_object_add(pramJobj, "atf", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_AirtimeFairness")));
#ifdef LTE_WLAN_AUTO_SWITCH_OFF
	json_object_object_add(pramJobj, "wlAutoOffEnable", JSON_OBJ_COPY(json_object_object_get(RadioObj, "X_ZYXEL_Radio_Pwrsave_Enable")));
	autoOffIntvl = json_object_get_int(json_object_object_get(RadioObj, "X_ZYXEL_Radio_Pwrsave_Quiet_Time"));
	json_object_object_add(pramJobj, "wlAutoOffIntvl", json_object_new_int(autoOffIntvl/60));	
#endif

	enablePMF = json_object_get_boolean(json_object_object_get(APSecObj, "EnableManagementFrameProtection"));
	pmfMethod = json_object_get_int(json_object_object_get(APSecObj, "X_ZYXEL_PmfMethod"));
	if (enablePMF == false) {
		json_object_object_add(pramJobj, "wlPmf", JSON_OBJ_COPY(json_object_new_string("disabled")));
	}
	else { //enablePMF = true
		if (pmfMethod == 0) {
			json_object_object_add(pramJobj, "wlPmf", JSON_OBJ_COPY(json_object_new_string("capable")));
		}
		else {
			json_object_object_add(pramJobj, "wlPmf", JSON_OBJ_COPY(json_object_new_string("required")));
		}
	}

	json_object_array_add(Jarray, pramJobj);

	if(SsidObj)
		zcfgFeJsonObjFree(SsidObj);
	if(RadioObj)
		zcfgFeJsonObjFree(RadioObj);
	if(APSecObj)
		zcfgFeJsonObjFree(APSecObj);
	return ret;
}

zcfgRet_t setAllWlOthersObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	
	if(wifiApWpsOthersObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_WPS, &wifiApWpsOthersIid, wifiApWpsOthersObj, NULL);
	if(wifiRadioOthersObj) zcfgFeObjJsonBlockedSet(RDM_OID_WIFI_RADIO, &wifiRadioOthersIid, wifiRadioOthersObj, NULL);
	
	return ret;
}

zcfgRet_t freeAllWlOthersObjects(){
	
	zcfgRet_t ret = ZCFG_SUCCESS;
	
	if(wifiRadioOthersObj) json_object_put(wifiRadioOthersObj); 
	if(wifiApWpsOthersObj) json_object_put(wifiApWpsOthersObj);
	if(wifiSecOthersObj) json_object_put(wifiSecOthersObj);
	
	return ret;
}

void getBasicWlOthersInfo(struct json_object *Jobj){
	//WiFi Radio
	wlRtsThrshld = json_object_get_int(json_object_object_get(Jobj, "wlRtsThrshld"));
	wlFrgThrshld = json_object_get_int(json_object_object_get(Jobj, "wlFrgThrshld"));
	AutoChannelTimer = json_object_get_int(json_object_object_get(Jobj, "AutoChannelTimer"));
	wlCsScanTimer = json_object_get_int(json_object_object_get(Jobj, "wlCsScanTimer"));
	wlTxPwrPcnt = json_object_get_int(json_object_object_get(Jobj, "wlTxPwrPcnt"));
	wlBcnIntvl = json_object_get_int(json_object_object_get(Jobj, "wlBcnIntvl"));
	wlDtmIntvl = json_object_get_int(json_object_object_get(Jobj, "wlDtmIntvl"));
	wlMode = json_object_get_string(json_object_object_get(Jobj, "wlMode"));
	wlProtect = json_object_get_string(json_object_object_get(Jobj, "wlProtect"));
	//wlRifsAdvert = json_object_get_string(json_object_object_get(Jobj, "wlRifsAdvert"));
	wlPreamble = json_object_get_string(json_object_object_get(Jobj, "wlPreamble"));
	wlRxChainPwrSave = json_object_get_int(json_object_object_get(Jobj, "wlRxChainPwrSave"));
	//wlObssCox = json_object_get_boolean(json_object_object_get(Jobj, "wlObssCox"));;
	wlFrameburst = json_object_get_boolean(json_object_object_get(Jobj, "wlFrameburst"));
	wlRegMode = json_object_get_int(json_object_object_get(Jobj, "wlRegMode"));
	wlDfsPreIsm = json_object_get_int(json_object_object_get(Jobj, "wlDfsPreIsm"));
	wlTpcDb = json_object_get_int(json_object_object_get(Jobj, "wlTpcDb"));
	wlDfsPostIsm = json_object_get_int(json_object_object_get(Jobj, "wlDfsPostIsm"));
	OperatingChannelBandwidth = json_object_get_string(json_object_object_get(Jobj, "OperatingChannelBandwidth"));
	//WiFi AP Sec
	wlPmf = json_object_get_string(json_object_object_get(Jobj, "wlPmf"));
	//WiFi AP WPS
	wps_v2 = json_object_get_boolean(json_object_object_get(Jobj, "wps_v2"));
	atf = json_object_get_boolean(json_object_object_get(Jobj, "atf"));
	InActiveChan = json_object_get_string(json_object_object_get(Jobj, "InActiveChan"));
	singleSKU = json_object_get_string(json_object_object_get(Jobj, "singleSKU"));

#ifdef LTE_WLAN_AUTO_SWITCH_OFF
	wlAutoOffEnable = json_object_get_int(json_object_object_get(Jobj, "wlAutoOffEnable"));
	wlAutoOffIntvl = json_object_get_int(json_object_object_get(Jobj, "wlAutoOffIntvl"));
#endif
	
	return;
}

void initGlobalWlOthersObjects(){
	
	wifiRadioOthersObj = NULL;
	wifiApWpsOthersObj = NULL;
	wifiSecOthersObj = NULL;
	
	IID_INIT(wifiRadioOthersIid);
	IID_INIT(wifiApWpsOthersIid);
	IID_INIT(wifiSecOthersIid);
}

zcfgRet_t editwifiRadioOthersObjects(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;
	uint8_t iidIdx =0;
	const char* tmpOperatingFrequencyBand;

	objIndex_t SsidIid={0};
	struct json_object *SsidObj = NULL;
	const char *tmpLowerLayer = NULL;
	uint8_t tmpRadioIndex;	
	
	SsidIid.level = 1;
	SsidIid.idx[0] = json_object_get_int(json_object_object_get(Jobj, "Index"));
	if((ret = zcfgFeObjJsonGet(RDM_OID_WIFI_SSID, &SsidIid, &SsidObj)) != ZCFG_SUCCESS)
		return ret;
	tmpLowerLayer = json_object_get_string(json_object_object_get(SsidObj, "LowerLayers"));
	sscanf(tmpLowerLayer, "WiFi.Radio.%hhu", &tmpRadioIndex);
	IID_INIT(wifiRadioOthersIid);
	wifiRadioOthersIid.level = 1;
	wifiRadioOthersIid.idx[0] = tmpRadioIndex;
	zcfgFeJsonObjFree(SsidObj);
	
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_RADIO, &wifiRadioOthersIid, &wifiRadioOthersObj)) != ZCFG_SUCCESS) {
		return ret;
	}

	if(json_object_object_get(Jobj, "wlRtsThrshld"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_RTS_CTS_Threshold", json_object_new_int(wlRtsThrshld));
	if(json_object_object_get(Jobj, "wlFrgThrshld"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Fragment_Threshold", json_object_new_int(wlFrgThrshld));
	if(json_object_object_get(Jobj, "AutoChannelTimer"))
		json_object_object_add(wifiRadioOthersObj, "AutoChannelRefreshPeriod", json_object_new_int(AutoChannelTimer));
	if(json_object_object_get(Jobj, "wlBcnIntvl"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Beacon_Interval", json_object_new_int(wlBcnIntvl));
	if(json_object_object_get(Jobj, "wlDtmIntvl"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_DTIM_Interval", json_object_new_int(wlDtmIntvl));
	if(json_object_object_get(Jobj, "wlMode"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Wireless_Mode", json_object_new_string(wlMode));
	if(json_object_object_get(Jobj, "wlTxPwrPcnt"))
		json_object_object_add(wifiRadioOthersObj, "TransmitPower", json_object_new_int(wlTxPwrPcnt));
	if(json_object_object_get(Jobj, "wlProtect")){
		if(!strcmp(wlProtect, "off"))
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Protection", json_object_new_boolean(false));
		else
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Protection", json_object_new_boolean(true));
	}
	if(json_object_object_get(Jobj, "wlPreamble"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Preamble", json_object_new_string(wlPreamble));
	//if (!strcmp(wlRifsAdvert, "off"))
	//	json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_RIFS_Advertisement", json_object_new_boolean(false));
	//else
	//	json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_RIFS_Advertisement", json_object_new_boolean(true));
	if(json_object_object_get(Jobj, "wlFrameburst"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Frameburst", json_object_new_boolean(wlFrameburst));
	//json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_OBSS_coex", json_object_new_boolean(wlObssCox));
	tmpOperatingFrequencyBand = json_object_object_get(wifiRadioOthersObj, "OperatingFrequencyBand");
	if(!strcmp(tmpOperatingFrequencyBand,"5GHz"))
	{
		if(json_object_object_get(Jobj, "wlDfsPreIsm"))
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_DfsPreIsm", json_object_new_int(wlDfsPreIsm));
		if(json_object_object_get(Jobj, "wlDfsPostIsm"))
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_DfsPostIsm", json_object_new_int(wlDfsPostIsm));
		if(json_object_object_get(Jobj, "wlTpcDb"))
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_TpcDb", json_object_new_int(wlTpcDb));
	}
	if(json_object_object_get(Jobj, "atf"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_AirtimeFairness", json_object_new_boolean(atf));
	if(json_object_object_get(Jobj, "InActiveChan")){
		if(InActiveChan) {
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_InActiveChannels", json_object_new_string(InActiveChan));
		}
		else {
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_InActiveChannels", json_object_new_string(""));
		}
	}
/* __ZyXEL__, Melissa, 20161111, Costomization for VMG1312-T20A Brazil */
	if(json_object_object_get(Jobj, "singleSKU")){
		if(singleSKU) {
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_SingleSKU", json_object_new_string(singleSKU));
		}
		else {
			json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_SingleSKU", json_object_new_string(""));
		}
	}

#ifdef LTE_WLAN_AUTO_SWITCH_OFF
if(json_object_object_get(Jobj, "wlAutoOffEnable"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Radio_Pwrsave_Enable", json_object_new_int(wlAutoOffEnable));	
	if(json_object_object_get(Jobj, "wlAutoOffIntvl"))
		json_object_object_add(wifiRadioOthersObj, "X_ZYXEL_Radio_Pwrsave_Quiet_Time", json_object_new_int(wlAutoOffIntvl*60));
#endif

	return ret;
}

zcfgRet_t editWifiOthersSecObject(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	char lowerlayer[16] = {0}, main_radio[2] = {0}, cur_radio[2] = {0};
	struct json_object *wifiSsidObj = NULL;
	objIndex_t wifiSsidIid;

	// Search Main interface radio
	IID_INIT(wifiSsidIid);
	wifiSsidIid.level = 1;
	wifiSsidIid.idx[0] = json_object_get_int(json_object_object_get(Jobj,"Index"));
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiSsidIid, &wifiSsidObj)) != ZCFG_SUCCESS) {
		return ret;
	}
	strcpy(lowerlayer, json_object_get_string(json_object_object_get(wifiSsidObj, "LowerLayers")));
	sscanf(lowerlayer, "WiFi.Radio.%1s", main_radio);
	zcfgFeJsonObjFree(wifiSsidObj);

	IID_INIT(wifiSecOthersIid);
	wifiSecOthersIid.level = 1;
	wifiSecOthersIid.idx[0] = json_object_get_int(json_object_object_get(Jobj,"Index"))-1;
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecOthersIid, &wifiSecOthersObj) == ZCFG_SUCCESS) {
		// Search Current interface radio
		IID_INIT(wifiSsidIid);
		wifiSsidIid.level = 1;
		wifiSsidIid.idx[0] = wifiSecOthersIid.idx[0];
		if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiSsidIid, &wifiSsidObj)) != ZCFG_SUCCESS) {
			zcfgFeJsonObjFree(wifiSecOthersObj);
			return ret;
		}
		strcpy(lowerlayer, json_object_get_string(json_object_object_get(wifiSsidObj, "LowerLayers")));
		sscanf(lowerlayer, "WiFi.Radio.%1s", cur_radio);
		zcfgFeJsonObjFree(wifiSsidObj);

		if (!strcmp(cur_radio, main_radio)) { //Current interface radio = Main interface radio, update PMF value according band
			if (json_object_object_get(Jobj, "wlPmf")) {
				if (!strcmp(wlPmf, "disabled")) {
					json_object_object_add(wifiSecOthersObj, "EnableManagementFrameProtection", json_object_new_boolean(false));
					json_object_object_add(wifiSecOthersObj, "X_ZYXEL_PmfMethod", json_object_new_int(0));
				}
				else if (!strcmp(wlPmf, "capable")) {
					json_object_object_add(wifiSecOthersObj, "EnableManagementFrameProtection", json_object_new_boolean(true));
					json_object_object_add(wifiSecOthersObj, "X_ZYXEL_PmfMethod", json_object_new_int(0));
				}
				else { //wlPmf = required
					json_object_object_add(wifiSecOthersObj, "EnableManagementFrameProtection", json_object_new_boolean(true));
					json_object_object_add(wifiSecOthersObj, "X_ZYXEL_PmfMethod", json_object_new_int(1));
				}
			}
			zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecOthersIid, wifiSecOthersObj, NULL);
			zcfgFeJsonObjFree(wifiSecOthersObj);
		}
		else {
			zcfgFeJsonObjFree(wifiSecOthersObj);
			break;
		}
	}
	return ret;
}

zcfgRet_t editWifiOthersApWpsObject(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;
	uint8_t iidIdx =0;
	
	IID_INIT(wifiApWpsOthersIid);
	wifiApWpsOthersIid.level = 1;
	wifiApWpsOthersIid.idx[0] = json_object_get_int(json_object_object_get(Jobj,"Index"));
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_WPS, &wifiApWpsOthersIid, &wifiApWpsOthersObj)) != ZCFG_SUCCESS) {
		return ret;
	}
	
	if(json_object_object_get(Jobj, "wps_v2"))
		json_object_object_add(wifiApWpsOthersObj, "X_ZYXEL_WPS_V2", json_object_new_boolean(wps_v2));
	
	return ret;
}

zcfgRet_t zcfgFeDalWifiOthersEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	printf("%s %s %d \n",json_object_to_json_string(Jobj),__func__,__LINE__);
	
	
	initGlobalWlOthersObjects();

	getBasicWlOthersInfo(Jobj);
	
	if((ret = editWifiOthersApWpsObject(Jobj)) != ZCFG_SUCCESS){
		printf("editWifiOthersApWpsObject fail", __LINE__);
		goto dalwlan_edit_fail;
	}
	
	if((ret = editWifiOthersSecObject(Jobj)) != ZCFG_SUCCESS){
		printf("editWifiOthersSecObject fail", __LINE__);
		goto dalwlan_edit_fail;
	}
		
	 
	if((ret = editwifiRadioOthersObjects(Jobj)) != ZCFG_SUCCESS){
		printf("editwifiRadioOthersObjects fail", __LINE__);
		goto dalwlan_edit_fail;
	}
			
	setAllWlOthersObjects(Jobj);

dalwlan_edit_fail:
	freeAllWlOthersObjects();
	
	return ret;
}

zcfgRet_t zcfgFeDalWifiOthers(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	
	prepareJObj(Jobj);
	
	//if(!strcmp(method, "POST"))
		//ret = zcfgFeDalWifiOthersAdd(Jobj, NULL);
	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalWifiOthersEdit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalWifiOthersGet(Jobj, Jarray, replyMsg);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}


