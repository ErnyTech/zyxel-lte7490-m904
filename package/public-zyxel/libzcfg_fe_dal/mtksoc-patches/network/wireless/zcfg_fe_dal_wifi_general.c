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

#define AutoChannel 0
//#define JSON_OBJ_COPY(json_object) json_tokener_parse(json_object_to_json_string(json_object))

dal_param_t WIFI_GENERAL_param[]={
	{"Band",				dalType_string,		0,	0,	NULL,	"2.4GHz|5GHz",	dal_Edit},
#ifdef ZYXEL_ONESSID
	{"oneSsidEnable",		dalType_boolean,	0,	0,	NULL,		NULL,			0},
#endif
	//WiFi SSID
	//{"Index",				dalType_int,		1,	8,	NULL,		NULL,		dal_Edit},
	{"MainSSID",			dalType_boolean,	0,	0,	NULL,		NULL,		dal_Edit},
	{"GuestSSID",			dalType_int,		0,	0,	NULL,		NULL,			0},
	{"wlEnable",			dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"SSID",				dalType_string,	 	0,	0,	NULL,		NULL,			0},
	{"X_ZYXEL_Multicast_Fwd",dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"upRate",				dalType_int,		0,	0,	NULL,		NULL,			0},
	{"downRate",			dalType_int,		0,	0,	NULL,		NULL,			0},
	//WiFi Radio
	{"div_wifiAdvance",		dalType_boolean,	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"bandwidth",			dalType_int,		0,	0,	NULL,		NULL,			0},
	{"extcha",				dalType_int,		0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"channel",				dalType_int,		0,	0,	NULL,		NULL,			0},
	{"PossibleChannels",		dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},	
	{"X_ZYXEL_UpperChannels",	dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"X_ZYXEL_LowerChannels",	dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"X_ZYXEL_AC80_Channels",	dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"X_ZYXEL_Wireless_Mode",	dalType_string,		0,	0,	NULL,	NULL,	dalcmd_Forbid},
	{"X_ZYXEL_OBSS_coex",		dalType_boolean,	0,	0,	NULL,	NULL,	dalcmd_Forbid},
	//WiFi Access Point
	{"MaxAssociatedDevices",dalType_int,		1,	64,	NULL,		NULL,			0},
	{"wlHide",				dalType_boolean, 	0,	0,	NULL,		NULL,			0},
	{"wlGwMode",			dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"wlEnableGuest",		dalType_boolean, 	0,	0,	NULL,		NULL,			0},
	//WiFi Access Point Sec
	{"ModesSupported",		dalType_string,		0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"securityLevel",		dalType_string,		0,	0,	NULL,		"NoSecurity|Basic|MoreSecure",	0},
	{"secMode",				dalType_int,		0,	0,	NULL,		"43|149|255",	dalcmd_Forbid},
	{"wep",					dalType_string,		0,	0,	NULL,		NULL,			0}, // 64bit|128bit if need enumeration
	{"WEPKey",				dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_AutoGenWepKey",dalType_boolean,	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEPKey2",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEPKey3",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEPKey4",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEP64Key1",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEP64Key2",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEP64Key3",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	// {"X_ZYXEL_WEP64Key4",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"AutoGenWepKey",dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"WEPKey2",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEPKey3",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEPKey4",		dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEP64Key1",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEP64Key2",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEP64Key3",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"WEP64Key4",	dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"wepid",				dalType_string,		0,	0,	NULL,		"key1|key2|key3|key4",			0},
#ifdef ZYXEL_WIFI_SUPPORT_WPA_MIX_MODE
	{"wpaMode",				dalType_string,		0,	0,	NULL,		"wpawpa2psk|wpa2psk|wpawpa2",	0},
#else
	{"wpaMode",				dalType_string,		0,	0,	NULL,		"wpa2psk|wpa2",			0},
#endif
	// {"X_ZYXEL_AutoGenPSK",	dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"AutoGenPSK",	dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"psk_value",			dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"RekeyingInterval",	dalType_int,		0,	0,	NULL,		NULL,			0},
	{"RadiusServerIPAddr",	dalType_string, 	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"RadiusServerPort",	dalType_int,		0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"RadiusSecret",		dalType_string, 	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"X_ZYXEL_Preauth",		dalType_boolean,	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"X_ZYXEL_ReauthInterval",dalType_int,		0,	0,	NULL,		NULL,			0},
#ifdef ZYXEL_WIFI_SUPPORT_WPA_MIX_MODE
	{"encryp",				dalType_string, 	0,	0,	NULL,		"tkipaes|aes",		0},
#else
	{"encryp",				dalType_string, 	0,	0,	NULL,		"aes",			0},
#endif
	{"X_ZYXEL_PassphraseType",dalType_int,		0,	0,	NULL,		NULL,			dalcmd_Forbid},
	//DHCPV4 SRV SUBNET
	{"div_wifiSubnet",		dalType_boolean,	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"wlSubnetState",		dalType_boolean,	0,	0,	NULL,		NULL,			0},
	{"Interface",			dalType_string, 	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{"MinAddress",			dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"MaxAddress",			dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"SubnetMask",			dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"LANIP",				dalType_string, 	0,	0,	NULL,		NULL,			0},
	{"subnetObjAction",		dalType_string, 	0,	0,	NULL,		NULL,			dalcmd_Forbid},
	{NULL,		0,	0,	0,	NULL},
};

struct json_object *wifiRadioObj = NULL;
struct json_object *wifiSsidObj = NULL;
struct json_object *wifiApWpsObj = NULL;
struct json_object *wifiApObj = NULL;
struct json_object *wifiSecObj = NULL;
struct json_object *wifiSubnetObj = NULL;

#ifdef ZYXEL_ONESSID
struct json_object *wifiOneSsidObj = NULL;
objIndex_t wifiOneSsidIid = {0};
#endif

objIndex_t wifiRadioIid = {0};
objIndex_t wifiSsidIid = {0};
objIndex_t wifiApWpsIid = {0};
objIndex_t wifiApIid = {0};
objIndex_t wifiSecIid = {0};
objIndex_t wifiSubnetIid = {0};

#ifdef WIFI_ONESSID_SECURITY_SYNC

struct json_object *syncWifiSsidObj = NULL;
objIndex_t syncWifiSsidIid = {0};
struct json_object *syncWifiSecObj = NULL;
objIndex_t syncWifiSecIid = {0};
bool bSyncSameSSID;

#endif //WIFI_ONESSID_SECURITY_SYNC

unsigned int SLIDER_POS_NONE = 43;
unsigned int SLIDER_POS_BASIC = 149;
unsigned int SLIDER_POS_MORESECURE = 255;
const char* X_ZYXEL_Auto_WepKey26;
const char* X_ZYXEL_Auto_WepKey10;
const char* X_ZYXEL_Auto_PSK;

#ifdef ZYXEL_ONESSID
bool oneSsidEnable;
#endif

//WiFi SSID
bool wlEnable;
const char* SSID;
bool X_ZYXEL_Multicast_Fwd;
unsigned int upRate;
unsigned int downRate;
//WiFi Radio
bool div_wifiAdvance;
unsigned int bandwidth;
unsigned int extcha;
unsigned int channel;
//WiFi Access Point
unsigned int MaxAssociatedDevices;
bool wlHide;
const char* wlGwMode;
bool wlEnableGuest;
//WiFi Access Point Sec
const char *secLv;
int secMode;
const char* wep;
const char* WEPKey;
bool X_ZYXEL_AutoGenWepKey;
const char* X_ZYXEL_WEPKey2;
const char* X_ZYXEL_WEPKey3;
const char* X_ZYXEL_WEPKey4;
const char* X_ZYXEL_WEP64Key1;
const char* X_ZYXEL_WEP64Key2;
const char* X_ZYXEL_WEP64Key3;
const char* X_ZYXEL_WEP64Key4;
const char* wepid;
const char* wpaMode;
bool X_ZYXEL_AutoGenPSK;
const char* psk_value;
unsigned int RekeyingInterval;
const char* RadiusServerIPAddr;
unsigned int RadiusServerPort;
const char* RadiusSecret;
bool X_ZYXEL_Preauth;
unsigned int X_ZYXEL_ReauthInterval;
const char* encryp;
unsigned int X_ZYXEL_PassphraseType;
//DHCPV4 SRV SUBNET
bool div_wifiSubnet;
bool wlSubnetState;
const char* Interface;
const char* MinAddress;
const char* MaxAddress;
const char* SubnetMask;
const char* LANIP;
const char* subnetObjAction;

time_t t1, t2;

bool isChannelInList(int, char *);

//convert string to lower case
void stolower(char *str, char *buf, size_t size)
{

	int n;

	for(n = 0; n < size && str[n] != '\0'; n++) {
		buf[n] = tolower(str[n]);
	}
	buf[n] = '\0';

	return;

}

zcfgRet_t zcfgFeDalWifiGeneralGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	objIndex_t subnetIid;
	struct json_object *obj = NULL;
	struct json_object *obj2 = NULL;
	struct json_object *subnetObj = NULL;
	char path[64] = {0};
	const char *RadioReference = NULL;
	const char *SSIDReference = NULL;
	struct json_object *RadioMultiObj = NULL;
	struct json_object *AccessPointMultiObj = NULL;
	objIndex_t radioIid;
	struct json_object *pramJobj = NULL;
	char *modeEn;
	int subnetInterface = 0;
#ifdef ZYXEL_ONESSID
	struct json_object *OneSsidObj = NULL;

	OneSsidObj = json_object_new_object();
	IID_INIT(iid);
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ONE_SSID, &iid, &obj) == ZCFG_SUCCESS) {
		json_object_object_add(OneSsidObj, "oneSsidEnable", JSON_OBJ_COPY(json_object_object_get(obj, "Enable")));
	}
#endif

	RadioMultiObj = json_object_new_object();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_WIFI_RADIO, &iid, &obj) == ZCFG_SUCCESS) {
		sprintf(path, "WiFi.Radio.%u", iid.idx[0]);
		json_object_object_add(RadioMultiObj, path, obj);
	}

	AccessPointMultiObj = json_object_new_object();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT, &iid, &obj) == ZCFG_SUCCESS) {
		//sprintf(path, "WiFi.AccessPoint.%u", iid.idx[0]);
		SSIDReference = json_object_get_string(json_object_object_get(obj, "SSIDReference"));
		json_object_object_add(AccessPointMultiObj, SSIDReference, obj);

		if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &iid, &obj2)== ZCFG_SUCCESS){
			json_object_object_add(obj, "Security", obj2);
		}
	}

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &iid, &obj) == ZCFG_SUCCESS) {
		pramJobj = json_object_new_object();
		json_object_object_add(pramJobj, "Index", json_object_new_int(iid.idx[0]));
		json_object_object_add(pramJobj, "wlEnable", JSON_OBJ_COPY(json_object_object_get(obj, "Enable")));
		json_object_object_add(pramJobj, "SSID", JSON_OBJ_COPY(json_object_object_get(obj, "SSID")));
		json_object_object_add(pramJobj, "MainSSID", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_MainSSID")));
		json_object_object_add(pramJobj, "X_ZYXEL_Multicast_Fwd", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_Multicast_Fwd")));
		json_object_object_add(pramJobj, "upRate", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_UpstreamRate")));
		json_object_object_add(pramJobj, "downRate", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_DownstreamRate")));
		json_object_object_add(pramJobj, "BSSID", JSON_OBJ_COPY(json_object_object_get(obj, "BSSID")));
		json_object_object_add(pramJobj, "X_ZYXEL_Attribute", JSON_OBJ_COPY(json_object_object_get(obj, "X_ZYXEL_Attribute")));
#ifdef ZYXEL_ONESSID
		json_object_object_add(pramJobj, "oneSsidEnable", JSON_OBJ_COPY(json_object_object_get(OneSsidObj, "oneSsidEnable")));
#endif

		RadioReference = json_object_get_string(json_object_object_get(obj, "LowerLayers"));
		obj2 = json_object_object_get(RadioMultiObj, RadioReference);
		if(obj2){
			json_object_object_add(pramJobj, "band", JSON_OBJ_COPY(json_object_object_get(obj2, "OperatingFrequencyBand")));
			json_object_object_add(pramJobj, "AutoChannelEnable", JSON_OBJ_COPY(json_object_object_get(obj2, "AutoChannelEnable")));
			json_object_object_add(pramJobj, "channel", JSON_OBJ_COPY(json_object_object_get(obj2, "Channel")));
			json_object_object_add(pramJobj, "bandwidth", JSON_OBJ_COPY(json_object_object_get(obj2, "OperatingChannelBandwidth")));
			json_object_object_add(pramJobj, "PossibleChannels", JSON_OBJ_COPY(json_object_object_get(obj2, "PossibleChannels")));
			json_object_object_add(pramJobj, "X_ZYXEL_UpperChannels", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_UpperChannels")));
			json_object_object_add(pramJobj, "X_ZYXEL_LowerChannels", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_LowerChannels")));
			json_object_object_add(pramJobj, "X_ZYXEL_AC80_Channels", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_AC80_Channels")));
			json_object_object_add(pramJobj, "X_ZYXEL_Wireless_Mode", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Wireless_Mode")));
			json_object_object_add(pramJobj, "X_ZYXEL_OBSS_coex", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_OBSS_coex")));
			{
			        char *band = json_object_get_string(json_object_object_get(obj2, "OperatingFrequencyBand"));;
			        char *chList = json_object_get_string(json_object_object_get(obj2, "PossibleChannels"));
			        char *chListUpper = json_object_get_string(json_object_object_get(obj2, "X_ZYXEL_UpperChannels"));;
			        char *chListLower = json_object_get_string(json_object_object_get(obj2, "X_ZYXEL_LowerChannels"));;
					char *sideBand = json_object_get_string(json_object_object_get(obj2, "ExtensionChannel"));
				unsigned int bw = json_object_get_int(json_object_object_get(obj2, "OperatingChannelBandwidth"));
				unsigned int ch = 0;
	
		                if(json_object_get_boolean(json_object_object_get(obj2, "AutoChannelEnable")))
		                        ch = AutoChannel;
		                else
                		        ch = json_object_get_int(json_object_object_get(obj2, "Channel"));


		                if(!strcmp(band,"2.4GHz")){
							if(!strcmp(sideBand, "BelowControlChannel")){
								json_object_object_add(pramJobj, "extcha", json_object_new_int(2));
							}
							else if(!strcmp(sideBand, "AboveControlChannel")){
								json_object_object_add(pramJobj, "extcha", json_object_new_int(1));
							}
                		}
		                else{ // 5G
					char *chListAC80 = json_object_get_string(json_object_object_get(obj2, "X_ZYXEL_AC80_Channels"));

		                        if(bw==20 && isChannelInList(ch,chList)){
                                		json_object_object_add(pramJobj, "extcha", json_object_new_int(0));
		                        }
                		        else if(bw==40 && isChannelInList(ch,chListLower)){
		                                json_object_object_add(pramJobj, "extcha", json_object_new_int(1));
                		        }
		                        else if(bw==40 && isChannelInList(ch,chListUpper)){
                                		json_object_object_add(pramJobj, "extcha", json_object_new_int(2));
		                        }
                		        else if(bw==80 && isChannelInList(ch,chListAC80)){
		                                json_object_object_add(pramJobj, "extcha", json_object_new_int(0));
                		        }
                		}
			}
		}

		//json_object_object_add(pramJobj, "SecurityMode", json_object_new_string("None"));
		sprintf(path, "WiFi.SSID.%u", iid.idx[0]);
		SSIDReference = (const char*)path;
		obj2 = json_object_object_get(AccessPointMultiObj, SSIDReference);
		if(obj2){
			json_object_object_add(pramJobj, "MaxAssociatedDevices", JSON_OBJ_COPY(json_object_object_get(obj2, "MaxAssociatedDevices")));
			
			if (json_object_get_boolean(json_object_object_get(obj2, "SSIDAdvertisementEnabled")))
			{
				json_object_object_add(pramJobj, "wlHide", json_object_new_boolean(false));
			}
			else
			{
				json_object_object_add(pramJobj, "wlHide", json_object_new_boolean(true));
			}

			json_object_object_add(pramJobj, "IsolationEnable", JSON_OBJ_COPY(json_object_object_get(obj2, "IsolationEnable")));
			json_object_object_add(pramJobj, "X_ZYXEL_BssIdIsolation", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_BssIdIsolation")));

			obj2 = json_object_object_get(obj2, "Security");
			if(obj2){
				json_object_object_add(pramJobj, "ModesSupported" , JSON_OBJ_COPY(json_object_object_get(obj2, "ModesSupported")));
				json_object_object_add(pramJobj, "SecurityMode", JSON_OBJ_COPY(json_object_object_get(obj2, "ModeEnabled")));
				modeEn = json_object_get_string(json_object_object_get(obj2, "ModeEnabled"));
				if(!strcmp("None",modeEn)){
					#ifdef TELIA_CUSTOMIZATION
					json_object_object_add(pramJobj, "CurrentPSKValue", json_object_new_string("None"));
					#else
					json_object_object_add(pramJobj, "PskDisplay", json_object_new_string("None"));
					#endif
				}
				else if(!strcmp("WPA2-Personal",modeEn)|| !strcmp("WPA-WPA2-Personal",modeEn)){
					if(json_object_get_boolean(json_object_object_get(obj2, "X_ZYXEL_AutoGenPSK"))){
						#ifdef TELIA_CUSTOMIZATION
						json_object_object_add(pramJobj, "CurrentPSKValue", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Auto_PSK")));
						#else
						json_object_object_add(pramJobj, "PskDisplay", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Auto_PSK")));
						#endif
					}
					else if(strlen(json_object_get_string(json_object_object_get(obj2, "KeyPassphrase")) ) > 0){
						#ifdef TELIA_CUSTOMIZATION
						json_object_object_add(pramJobj, "CurrentPSKValue", JSON_OBJ_COPY(json_object_object_get(obj2, "KeyPassphrase")));
						#else
						json_object_object_add(pramJobj, "PskDisplay", JSON_OBJ_COPY(json_object_object_get(obj2, "KeyPassphrase")));
						#endif
					}
					else{
						#ifdef TELIA_CUSTOMIZATION
						json_object_object_add(pramJobj, "CurrentPSKValue", JSON_OBJ_COPY(json_object_object_get(obj2, "PreSharedKey")));
						#else
						json_object_object_add(pramJobj, "PskDisplay", JSON_OBJ_COPY(json_object_object_get(obj2, "PreSharedKey")));
						#endif
					}
				}
				else{
					#ifdef TELIA_CUSTOMIZATION
					json_object_object_add(pramJobj, "CurrentPSKValue", json_object_new_string("N/A"));
					#else
					json_object_object_add(pramJobj, "PskDisplay", json_object_new_string("N/A"));
					#endif
				}
				json_object_object_add(pramJobj, "AutoGenPSK", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_AutoGenPSK")));
				json_object_object_add(pramJobj, "AutoGenPSKValue", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Auto_PSK")));
				#ifdef TELIA_CUSTOMIZATION
				;
				#else
				json_object_object_add(pramJobj, "CurrentPSKValue", JSON_OBJ_COPY(json_object_object_get(obj2, "KeyPassphrase")));
				#endif
				json_object_object_add(pramJobj, "RekeyingInterval", JSON_OBJ_COPY(json_object_object_get(obj2, "RekeyingInterval")));
				json_object_object_add(pramJobj, "encryp", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Encryption")));
				json_object_object_add(pramJobj, "RadiusServerIPAddr", JSON_OBJ_COPY(json_object_object_get(obj2, "RadiusServerIPAddr")));
				json_object_object_add(pramJobj, "RadiusServerPort", JSON_OBJ_COPY(json_object_object_get(obj2, "RadiusServerPort")));
				json_object_object_add(pramJobj, "RadiusSecret", JSON_OBJ_COPY(json_object_object_get(obj2, "RadiusSecret")));
				json_object_object_add(pramJobj, "X_ZYXEL_Preauth", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_Preauth")));
				json_object_object_add(pramJobj, "X_ZYXEL_ReauthInterval", JSON_OBJ_COPY(json_object_object_get(obj2, "X_ZYXEL_ReauthInterval")));				

			}
		}

		// SSID Subnet
		IID_INIT(subnetIid);
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DHCPV4_SRV_SUBNET, &subnetIid, &subnetObj) == ZCFG_SUCCESS){
			sscanf(json_object_get_string(json_object_object_get(subnetObj, "Interface")), "WiFi.SSID.%u", &subnetInterface);
			
			
			if(subnetInterface == iid.idx[0]){
				json_object_object_add(pramJobj, "wlSubnetState", JSON_OBJ_COPY(json_object_object_get(subnetObj, "Enable")));
				json_object_object_add(pramJobj, "MinAddress", JSON_OBJ_COPY(json_object_object_get(subnetObj, "MinAddress")));
				json_object_object_add(pramJobj, "MaxAddress", JSON_OBJ_COPY(json_object_object_get(subnetObj, "MaxAddress")));
				json_object_object_add(pramJobj, "SubnetMask", JSON_OBJ_COPY(json_object_object_get(subnetObj, "SubnetMask")));
				json_object_object_add(pramJobj, "LANIP", JSON_OBJ_COPY(json_object_object_get(subnetObj, "LANIP")));
			}
		}

		json_object_array_add(Jarray, pramJobj);
		zcfgFeJsonObjFree(obj);
	}

#ifdef ZYXEL_ONESSID
	zcfgFeJsonObjFree(OneSsidObj);
#endif
	zcfgFeJsonObjFree(RadioMultiObj);
	zcfgFeJsonObjFree(AccessPointMultiObj);
	zcfgFeJsonObjFree(subnetObj);
	
	return ret;
}

void zcfgFeDalShowWifiGeneral(struct json_object *Jarray){

	int i, len = 0;
	struct json_object *obj;
	char channel[16] = {0};
	
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-7s %-8s %-35s %-10s %-10s %-10s %-12s %-20s %-20s\n",
		    "Index", "Band", "SSID", "Enable", "Bandwidth", "Channel", "MaxDevices", "SecurityMode", "PskValue");
	
	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		if(obj != NULL){
			if(json_object_get_boolean(json_object_object_get(obj, "AutoChannelEnable")))
				strcpy(channel, "Auto");
			else
				sprintf(channel, "%llu", json_object_get_int(json_object_object_get(obj, "channel")));

			/*
				Bug #48715: 
				Security issue: "Cfg wlan get" command will show hidden preset and hidden backhaul SSID and password.
				Expectation: Need to remove this in the cfg wlan get command.
				This is for all Multy Pro supported devices with CLI command support.
				Solution: Hide Preset and Backhaul Wifi information. Skip if X_ZYXEL_Attribute is not equal to 0.
			*/
			if(json_object_get_int(json_object_object_get(obj, "X_ZYXEL_Attribute"))==0){
			printf("%-7llu %-8s %-35s %-10u %-10s %-10s %-12llu %-20s %-20s\n",
			    json_object_get_int(json_object_object_get(obj, "Index")),
			    json_object_get_string(json_object_object_get(obj, "band")),
			    json_object_get_string(json_object_object_get(obj, "SSID")),
			    json_object_get_boolean(json_object_object_get(obj, "wlEnable")),
#ifdef TELIA_CUSTOMIZATION //To show "20/40MHz" instead of "40MHz" when bandwidth coexistence is enabled in 2.4GHz
                            (!strcmp("2.4GHz",json_object_get_string(json_object_object_get(obj, "band"))) && \ 
			     !strcmp("40MHz",json_object_get_string(json_object_object_get(obj, "bandwidth"))) && \
                             json_object_get_boolean(json_object_object_get(obj, "X_ZYXEL_OBSS_coex"))) ? \
				"20/40MHz": json_object_get_string(json_object_object_get(obj, "bandwidth")),
#else
			    json_object_get_string(json_object_object_get(obj, "bandwidth")),
#endif
			    channel,
			    json_object_get_int(json_object_object_get(obj, "MaxAssociatedDevices")),
			    json_object_get_string(json_object_object_get(obj, "SecurityMode")),
#ifdef TELIA_CUSTOMIZATION
				json_object_get_string(json_object_object_get(obj, "CurrentPSKValue")));
#else				
				json_object_get_string(json_object_object_get(obj, "PskDisplay")));
#endif			
			}
		}
	}
	
}


zcfgRet_t setAllWlObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

#ifdef ZYXEL_ONESSID
	if(wifiOneSsidObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ONE_SSID, &wifiOneSsidIid, wifiOneSsidObj, NULL);
#endif
	if(wifiApObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT, &wifiApIid, wifiApObj, NULL);
	if(wifiSecObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecIid, wifiSecObj, NULL);
	if(wifiRadioObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_RADIO, &wifiRadioIid, wifiRadioObj, NULL);
	if(wifiSsidObj)zcfgFeObjJsonBlockedSet(RDM_OID_WIFI_SSID, &wifiSsidIid, wifiSsidObj, NULL);
	if(wifiSubnetObj) zcfgFeObjJsonBlockedSet(RDM_OID_DHCPV4_SRV_SUBNET, &wifiSubnetIid, wifiSubnetObj, NULL);
#ifdef WIFI_ONESSID_SECURITY_SYNC
	if(syncWifiSecObj) zcfgFeObjJsonBlockedSet(RDM_OID_WIFI_ACCESS_POINT_SEC, &syncWifiSecIid, syncWifiSecObj, NULL);
#endif //WIFI_ONESSID_SECURITY_SYNC
	return ret;
}

zcfgRet_t freeAllWlObjects(){
	zcfgRet_t ret = ZCFG_SUCCESS;

#ifdef ZYXEL_ONESSID
	if(wifiOneSsidObj) json_object_put(wifiOneSsidObj);
#endif

	if(wifiRadioObj) json_object_put(wifiRadioObj); 
	if(wifiSsidObj) json_object_put(wifiSsidObj);
	if(wifiApWpsObj) json_object_put(wifiApWpsObj);
	if(wifiApObj) json_object_put(wifiApObj);
	if(wifiSecObj) json_object_put(wifiSecObj);
#ifdef WIFI_ONESSID_SECURITY_SYNC
	if(syncWifiSecObj) json_object_put(syncWifiSecObj);
#endif //WIFI_ONESSID_SECURITY_SYNC
	if(wifiSubnetObj) json_object_put(wifiSubnetObj);

	return ret;
}


void getBasicWlInfo(struct json_object *Jobj){

#ifdef ZYXEL_ONESSID
/*
	if(!json_object_object_get(Jobj, "oneSsidEnable")){
		oneSsidEnable = json_object_get_boolean(json_object_object_get(Jobj, "oneSsidEnable"));
	}
*/
    if(json_object_object_get(Jobj, "oneSsidEnable") != NULL){
        oneSsidEnable = json_object_get_boolean(json_object_object_get(Jobj, "oneSsidEnable"));
    }    
    else {
        oneSsidEnable = json_object_get_boolean(json_object_object_get(wifiOneSsidObj, "oneSsidEnable"));
    }
#endif

	wlEnable = json_object_get_boolean(json_object_object_get(Jobj, "wlEnable"));
	SSID = json_object_get_string(json_object_object_get(Jobj, "SSID"));
	X_ZYXEL_Multicast_Fwd = json_object_get_boolean(json_object_object_get(Jobj, "X_ZYXEL_Multicast_Fwd"));
	upRate = json_object_get_int(json_object_object_get(Jobj, "upRate"));
	downRate = json_object_get_int(json_object_object_get(Jobj, "downRate"));
	
	//WiFi Radio
	div_wifiAdvance = json_object_get_boolean(json_object_object_get(Jobj, "div_wifiAdvance"));
	bandwidth = json_object_get_int(json_object_object_get(Jobj, "bandwidth"));
	extcha = json_object_get_int(json_object_object_get(Jobj, "extcha"));
	if(json_object_get_string(json_object_object_get(Jobj, "channel")) && !strcasecmp("auto",json_object_get_string(json_object_object_get(Jobj, "channel"))))
		channel = 0; // if "auto" then change it to 0
	else
		channel = json_object_get_int(json_object_object_get(Jobj, "channel"));
	
	//WiFi Access Point
	MaxAssociatedDevices = json_object_get_int(json_object_object_get(Jobj, "MaxAssociatedDevices"));
	wlHide = json_object_get_boolean(json_object_object_get(Jobj, "wlHide"));
	wlGwMode = json_object_get_string(json_object_object_get(Jobj, "wlGwMode"));
	wlEnableGuest= json_object_get_boolean(json_object_object_get(Jobj, "wlEnableGuest"));
	
	//WiFi Access Point Sec
	// dalcmd: securityLevel, GUI: secMode
	if(json_object_object_get(Jobj, "securityLevel")){
		secLv = json_object_get_string(json_object_object_get(Jobj, "securityLevel"));
		if(!strcmp(secLv,"NoSecurity"))
			json_object_object_add(Jobj, "secMode", json_object_new_int(43));
		else if(!strcmp(secLv,"Basic"))
			json_object_object_add(Jobj, "secMode", json_object_new_int(149));
		else if(!strcmp(secLv,"MoreSecure"))
			json_object_object_add(Jobj, "secMode", json_object_new_int(255));
		else
			printf("Security level has wrong value.\n");
	}
	secMode = json_object_get_int(json_object_object_get(Jobj, "secMode"));
	wep = json_object_get_string(json_object_object_get(Jobj, "wep"));
	X_ZYXEL_AutoGenWepKey = json_object_get_boolean(json_object_object_get(Jobj, "AutoGenWepKey"));
	WEPKey = json_object_get_string(json_object_object_get(Jobj, "WEPKey"));
	X_ZYXEL_WEPKey2 = json_object_get_string(json_object_object_get(Jobj, "WEPKey2"));
	X_ZYXEL_WEPKey3 = json_object_get_string(json_object_object_get(Jobj, "WEPKey3"));
	X_ZYXEL_WEPKey4 = json_object_get_string(json_object_object_get(Jobj, "WEPKey4"));
	X_ZYXEL_WEP64Key1 = json_object_get_string(json_object_object_get(Jobj, "WEP64Key1"));
	X_ZYXEL_WEP64Key2 = json_object_get_string(json_object_object_get(Jobj, "WEP64Key2"));
	X_ZYXEL_WEP64Key3 = json_object_get_string(json_object_object_get(Jobj, "WEP64Key3"));
	X_ZYXEL_WEP64Key4 = json_object_get_string(json_object_object_get(Jobj, "WEP64Key4"));
	wepid = json_object_get_string(json_object_object_get(Jobj, "wepid"));
	wpaMode = json_object_get_string(json_object_object_get(Jobj, "wpaMode"));
	X_ZYXEL_AutoGenPSK = json_object_get_boolean(json_object_object_get(Jobj, "AutoGenPSK"));
	psk_value = json_object_get_string(json_object_object_get(Jobj, "psk_value"));
	RekeyingInterval = json_object_get_int(json_object_object_get(Jobj, "RekeyingInterval"));
	RadiusServerIPAddr = json_object_get_string(json_object_object_get(Jobj, "RadiusServerIPAddr"));
	RadiusServerPort = json_object_get_int(json_object_object_get(Jobj, "RadiusServerPort"));
	RadiusSecret = json_object_get_string(json_object_object_get(Jobj, "RadiusSecret"));
	X_ZYXEL_Preauth = json_object_get_boolean(json_object_object_get(Jobj, "X_ZYXEL_Preauth"));
	X_ZYXEL_ReauthInterval = json_object_get_int(json_object_object_get(Jobj, "X_ZYXEL_ReauthInterval"));
	encryp = json_object_get_string(json_object_object_get(Jobj, "encryp"));
	X_ZYXEL_PassphraseType = json_object_get_int(json_object_object_get(Jobj, "X_ZYXEL_PassphraseType")); 
	//DHCPV4 SRV SUBNET
	wlSubnetState = json_object_get_boolean(json_object_object_get(Jobj, "wlSubnetState"));
	div_wifiSubnet = json_object_get_boolean(json_object_object_get(Jobj, "div_wifiSubnet"));
	Interface = json_object_get_string(json_object_object_get(Jobj, "Interface"));
	MinAddress = json_object_get_string(json_object_object_get(Jobj, "MinAddress"));
	MaxAddress = json_object_get_string(json_object_object_get(Jobj, "MaxAddress"));
	SubnetMask = json_object_get_string(json_object_object_get(Jobj, "SubnetMask"));
	LANIP = json_object_get_string(json_object_object_get(Jobj, "LANIP"));
	subnetObjAction = json_object_get_string(json_object_object_get(Jobj, "subnetObjAction"));
	
	return;
}

void initGlobalWlObjects(){

	wifiRadioObj = NULL;
	wifiSsidObj = NULL;
	wifiApWpsObj = NULL;
	wifiApObj = NULL;
	wifiSecObj = NULL;
#ifdef WIFI_ONESSID_SECURITY_SYNC
	syncWifiSecObj = NULL;
#endif //WIFI_ONESSID_SECURITY_SYNC
	wifiSubnetObj = NULL;
	
#ifdef ZYXEL_ONESSID
	wifiOneSsidObj = NULL;
	IID_INIT(wifiOneSsidIid);
#endif

	IID_INIT(wifiRadioIid);
	IID_INIT(wifiSsidIid);
	IID_INIT(wifiApWpsIid);
	IID_INIT(wifiApIid);
	IID_INIT(wifiSecIid);
	IID_INIT(wifiSubnetIid);

}

zcfgRet_t editWifiRadioObjects(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;
	uint8_t iidIdx =0;
	const char *band = NULL;

	if (channel == 0)
	{
		json_object_object_add(wifiRadioObj, "AutoChannelEnable", json_object_new_boolean(true));
	}else
	{
		json_object_object_add(wifiRadioObj, "AutoChannelEnable", json_object_new_boolean(false));
		json_object_object_add(wifiRadioObj, "Channel", json_object_new_int(channel));
	}
	
	band = json_object_get_string(json_object_object_get(wifiRadioObj, "OperatingFrequencyBand"));

	if(bandwidth == 0){
		json_object_object_add(wifiRadioObj, "X_ZYXEL_OBSS_coex", json_object_new_boolean(true));

		if(!strcmp(band, "2.4GHz")){
			json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("40MHz"));
			if(extcha == 2)
				json_object_object_add(wifiRadioObj, "ExtensionChannel", json_object_new_string("BelowControlChannel"));
			else
				json_object_object_add(wifiRadioObj, "ExtensionChannel", json_object_new_string("AboveControlChannel"));
		}
	}
	else{
		json_object_object_add(wifiRadioObj, "X_ZYXEL_OBSS_coex", json_object_new_boolean(false));
		if (bandwidth == 20)
			json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("20MHz"));
		else if (bandwidth == 40)
			json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("40MHz"));
		else if (bandwidth == 80)
			json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("80MHz"));
		if (bandwidth == 40)
		{
			if(extcha == 2)
				json_object_object_add(wifiRadioObj, "ExtensionChannel", json_object_new_string("BelowControlChannel"));
			else
				json_object_object_add(wifiRadioObj, "ExtensionChannel", json_object_new_string("AboveControlChannel"));
		}
	}
	
	return ret;
}

zcfgRet_t editWifiSsidObjects(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;
	uint8_t iidIdx =0;
#ifdef WIFI_ONESSID_SECURITY_SYNC

	int getIndex = wifiSsidIid.idx[0] ;
	int syncIndex = DualbandSync(getIndex);
		
	IID_INIT(syncWifiSsidIid);
	syncWifiSsidIid.level = 1;
	syncWifiSsidIid.idx[0] = syncIndex;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &syncWifiSsidIid, &syncWifiSsidObj)) != ZCFG_SUCCESS) {
		return ret;
	}
	char *syncSSID = json_object_get_string(json_object_object_get(syncWifiSsidObj, "SSID"));
	if (!strcmp(syncSSID, SSID)){
		bSyncSameSSID = true;
		printf("\n%s: Wireless network security object synchronization for Same SSID.\n", __FUNCTION__);
		printf("%s: WiFi Interface[%d]-%s synchronization from Interface[%d]-%s !!!! \n", __FUNCTION__, syncIndex, syncSSID, getIndex, SSID);
	}
	else{
		bSyncSameSSID = false;
	}
	
#endif //WIFI_ONESSID_SECURITY_SYNC

#ifdef ZYXEL_ONESSID
	json_object_object_add(wifiOneSsidObj, "Enable", json_object_new_boolean(oneSsidEnable));
#endif

	json_object_object_add(wifiSsidObj, "Enable", json_object_new_boolean(wlEnable));
	json_object_object_add(wifiSsidObj, "X_ZYXEL_Multicast_Fwd", json_object_new_boolean(X_ZYXEL_Multicast_Fwd));
	json_object_object_add(wifiSsidObj, "SSID", json_object_new_string(SSID));
	
	if(upRate)
		json_object_object_add(wifiSsidObj, "X_ZYXEL_UpstreamRate", json_object_new_int(upRate));
	else
		json_object_object_add(wifiSsidObj, "X_ZYXEL_UpstreamRate", json_object_new_int(0));
	
	if(downRate)
		json_object_object_add(wifiSsidObj, "X_ZYXEL_DownstreamRate", json_object_new_int(downRate));
	else
		json_object_object_add(wifiSsidObj, "X_ZYXEL_DownstreamRate", json_object_new_int(0));

	return ret;
}

#ifdef WIFI_ONESSID_SECURITY_SYNC
int DualbandSync(int wifiIdx){
/* 
	1. Security Level
	2. Security Mode
	3. Generate password automatically
	4. password
	5. WPA Compatible
	6. Encryption
	7. Group Key Update Timer.
*/
	if (wifiIdx < 5){
		return (wifiIdx+4);
	}
	else{
		return (wifiIdx-4);
	}
}
#endif //WIFI_ONESSID_SECURITY_SYNC

zcfgRet_t editWiFiPMFtoCapable(int wifiSec_idx) {

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *wifiOtherSsidObj = NULL;
	struct json_object *wifiOtherSecObj = NULL;
	objIndex_t wifiOtherSsidIid;
	objIndex_t wifiOtherSecIid;
	char lowerlayer[16] = {0}, cur_radio[2] = {0}, tmp_radio[2] = {0};

	// Search current interface radio
	IID_INIT(wifiOtherSsidIid);
	wifiOtherSsidIid.level = 1;
	wifiOtherSsidIid.idx[0] = wifiSecIid.idx[0];
	if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiOtherSsidIid, &wifiOtherSsidObj)) != ZCFG_SUCCESS) {
		return ret;
	}
	strcpy(lowerlayer, json_object_get_string(json_object_object_get(wifiOtherSsidObj, "LowerLayers")));
	sscanf(lowerlayer, "WiFi.Radio.%1s", cur_radio);
	zcfgFeJsonObjFree(wifiOtherSsidObj);

	IID_INIT(wifiOtherSecIid);
	wifiOtherSecIid.level = 1;
	while (zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiOtherSecIid, &wifiOtherSecObj) == ZCFG_SUCCESS) {
		// Search other interface radio
		IID_INIT(wifiOtherSsidIid);
		wifiOtherSsidIid.level = 1;
		wifiOtherSsidIid.idx[0] = wifiOtherSecIid.idx[0];
		if ((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiOtherSsidIid, &wifiOtherSsidObj)) != ZCFG_SUCCESS) {
			zcfgFeJsonObjFree(wifiOtherSecObj);
			return ret;
		}
		strcpy(lowerlayer, json_object_get_string(json_object_object_get(wifiOtherSsidObj, "LowerLayers")));
		sscanf(lowerlayer, "WiFi.Radio.%1s", tmp_radio);
		zcfgFeJsonObjFree(wifiOtherSsidObj);

		if (!strcmp(tmp_radio, cur_radio)) {
			json_object_object_add(wifiOtherSecObj, "EnableManagementFrameProtection", json_object_new_boolean(true));
			json_object_object_add(wifiOtherSecObj, "X_ZYXEL_PmfMethod", json_object_new_int(0));
			zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiOtherSecIid, wifiOtherSecObj, NULL);
		}
		zcfgFeJsonObjFree(wifiOtherSecObj);
	}
	return ret;
}

zcfgRet_t editWifiApObject(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;	
	uint8_t iidIdx =0;

	json_object_object_add(wifiApObj, "MaxAssociatedDevices", json_object_new_int(MaxAssociatedDevices));
	if(wlHide)
		json_object_object_add(wifiApObj, "SSIDAdvertisementEnabled", json_object_new_boolean(false));
	else
		json_object_object_add(wifiApObj, "SSIDAdvertisementEnabled", json_object_new_boolean(true));
	if(wlEnableGuest)
	{
		if(!strcmp(wlGwMode, "0"))
		{
			json_object_object_add(wifiApObj, "IsolationEnable", json_object_new_boolean(true));
			json_object_object_add(wifiApObj, "X_ZYXEL_BssIdIsolation", json_object_new_boolean(true));
		}
		else
		{
			json_object_object_add(wifiApObj, "IsolationEnable", json_object_new_boolean(false));
			json_object_object_add(wifiApObj, "X_ZYXEL_BssIdIsolation", json_object_new_boolean(true));
		}
	}
	else
	{
		json_object_object_add(wifiApObj, "IsolationEnable", json_object_new_boolean(false));
		json_object_object_add(wifiApObj, "X_ZYXEL_BssIdIsolation", json_object_new_boolean(false));
	}
	
	return ret;
}

zcfgRet_t editWifiSecObject(struct json_object *Jobj){

	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *iidArray = NULL;
	uint8_t iidIdx =0;
	char *psk_lowercase = NULL;

	// Index: from dalcmd
	if(json_object_object_get(Jobj, "Index")){
		wifiSecIid.idx[0] = json_object_get_int(json_object_object_get(Jobj, "Index"));
		if(wifiSecIid.idx[0] != 0)
			wifiSecIid.level = 1;
	}

	if(secMode == SLIDER_POS_NONE)
		json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("None"));
	else if(secMode == SLIDER_POS_BASIC)
	{
		if (!strcmp(wep,"128bit"))
		{
			json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("WEP-128"));
			json_object_object_add(wifiSecObj, "X_ZYXEL_WepModeSelect", json_object_new_string("WEP-128"));
		}
		else
		{
			json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("WEP-64"));
			json_object_object_add(wifiSecObj, "X_ZYXEL_WepModeSelect", json_object_new_string("WEP-64"));
		}
		
		json_object_object_add(wifiSecObj, "X_ZYXEL_WEPAuthentication", json_object_new_string("None"));
		json_object_object_add(wifiSecObj, "X_ZYXEL_WEPEncryption", json_object_new_string("WEPEncryption"));
		json_object_object_add(wifiSecObj, "X_ZYXEL_AutoGenWepKey", json_object_new_boolean(X_ZYXEL_AutoGenWepKey));
		
		if(X_ZYXEL_AutoGenWepKey)
		{
			json_object_object_add(wifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
			
			if (!strcmp(wep,"128bit"))
			{
				X_ZYXEL_Auto_WepKey26 = json_object_get_string(json_object_object_get(wifiSecObj, "X_ZYXEL_Auto_WepKey26"));
				json_object_object_add(wifiSecObj, "WEPKey", json_object_new_string(X_ZYXEL_Auto_WepKey26));
			}
			else
			{
				X_ZYXEL_Auto_WepKey10 = json_object_get_string(json_object_object_get(wifiSecObj, "X_ZYXEL_Auto_WepKey10"));
				json_object_object_add(wifiSecObj, "WEPKey", json_object_new_string(X_ZYXEL_Auto_WepKey10));
			}
		}
		else
		{
			if (!strcmp(wep,"128bit"))
			{
				if(json_object_object_get(Jobj, "WEPKey"))
					json_object_object_add(wifiSecObj, "WEPKey", json_object_new_string(WEPKey));
				if(json_object_object_get(Jobj, "WEPKey2"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEPKey2", json_object_new_string(X_ZYXEL_WEPKey2));
				if(json_object_object_get(Jobj, "WEPKey3"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEPKey3", json_object_new_string(X_ZYXEL_WEPKey3));
				if(json_object_object_get(Jobj, "WEPKey4"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEPKey4", json_object_new_string(X_ZYXEL_WEPKey4));
			}
			else
			{
				if(json_object_object_get(Jobj, "WEP64Key1"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEP64Key1", json_object_new_string(X_ZYXEL_WEP64Key1));
				if(json_object_object_get(Jobj, "WEP64Key2"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEP64Key2", json_object_new_string(X_ZYXEL_WEP64Key2));
				if(json_object_object_get(Jobj, "WEP64Key3"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEP64Key3", json_object_new_string(X_ZYXEL_WEP64Key3));
				if(json_object_object_get(Jobj, "WEP64Key4"))
					json_object_object_add(wifiSecObj, "X_ZYXEL_WEP64Key4", json_object_new_string(X_ZYXEL_WEP64Key4));
			}
			
			json_object_object_add(wifiSecObj, "wepid", json_object_new_string(wepid));
			
			if(strlen(wepid) == 0)
				json_object_object_add(wifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
			else
			{
				json_object_object_add(wifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(wepid[3]-'0'));
				//The value of wepid[3] is 1~4 but character, so it needs to be changed to number
				if(wepid[3]-'0' <= 0 || wepid[3]-'0' > 4)
					json_object_object_add(wifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
			}
		}
	}
	else if(secMode == SLIDER_POS_MORESECURE)
	{
		if(!strcmp(wpaMode,"wpa2psk") || !strcmp(wpaMode,"wpawpa2psk"))
		{
			if(!strcmp(wpaMode,"wpa2psk")){
				json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("WPA2-Personal"));
				if (wlEnable == true) { //interface enable
					json_object_object_add(wifiSecObj, "EnableManagementFrameProtection", json_object_new_boolean(true));
					json_object_object_add(wifiSecObj, "X_ZYXEL_PmfMethod", json_object_new_int(0));
					/* Set PMF with capable when enable WPA2-PSK for other interfaces with the same band */
					if ((ret = editWiFiPMFtoCapable(wifiSecIid.idx[0])) != ZCFG_SUCCESS) {
						return ret;
					}
				}
			}
			else{
				json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("WPA-WPA2-Personal"));
			}
			// AutoGenPSK be from user (Jobj) or default (wifiSecObj)
			if(json_object_object_get(Jobj,"AutoGenPSK")){
				json_object_object_add(wifiSecObj, "X_ZYXEL_AutoGenPSK", json_object_new_boolean(X_ZYXEL_AutoGenPSK));
			}
			else{
				X_ZYXEL_AutoGenPSK = json_object_get_boolean(json_object_object_get(wifiSecObj, "X_ZYXEL_AutoGenPSK"));
				psk_value = json_object_get_string(json_object_object_get(wifiSecObj, "X_ZYXEL_Auto_PSK"));
			}
			
			if(X_ZYXEL_AutoGenPSK)
			{
				X_ZYXEL_Auto_PSK = json_object_get_string(json_object_object_get(wifiSecObj, "X_ZYXEL_Auto_PSK"));
				json_object_object_add(wifiSecObj, "KeyPassphrase", json_object_new_string(X_ZYXEL_Auto_PSK));
				json_object_object_add(wifiSecObj, "PreSharedKey", json_object_new_string(""));
			}
			else {
				if(strlen(psk_value) == 64) {
					psk_lowercase = (char*)malloc(strlen(psk_value)+1);
					memset(psk_lowercase, 0, strlen(psk_value)+1);
					stolower(psk_value, psk_lowercase, strlen(psk_value));
					json_object_object_add(wifiSecObj, "KeyPassphrase", json_object_new_string(""));
					json_object_object_add(wifiSecObj, "PreSharedKey", json_object_new_string(psk_lowercase));
				}
				else {
					json_object_object_add(wifiSecObj, "KeyPassphrase", json_object_new_string(psk_value));
				}
			}
		}
		else if(!strcmp(wpaMode,"wpa2"))
		{
			json_object_object_add(wifiSecObj, "ModeEnabled", json_object_new_string("WPA2-Enterprise"));
			json_object_object_add(wifiSecObj, "X_ZYXEL_Preauth", json_object_new_boolean(X_ZYXEL_Preauth));
			json_object_object_add(wifiSecObj, "X_ZYXEL_ReauthInterval", json_object_new_int(X_ZYXEL_ReauthInterval));
			
			json_object_object_add(wifiSecObj, "RadiusServerIPAddr", json_object_new_string(RadiusServerIPAddr));
			json_object_object_add(wifiSecObj, "RadiusServerPort", json_object_new_int(RadiusServerPort));
			json_object_object_add(wifiSecObj, "RadiusSecret", json_object_new_string(RadiusSecret));
		}

		if(json_object_object_get(Jobj, "encryp")){
			if(!strcmp(encryp, "tkipaes")){
				json_object_object_add(wifiSecObj, "X_ZYXEL_Encryption", json_object_new_string("tkipaes"));
			}
			if(!strcmp(encryp, "aes")){
				json_object_object_add(wifiSecObj, "X_ZYXEL_Encryption", json_object_new_string("aes"));
			}
		}

		// RekeyingInterval be from user (Jobj) or default
		if(json_object_object_get(Jobj, "RekeyingInterval")){
			json_object_object_add(wifiSecObj, "RekeyingInterval", json_object_new_int(RekeyingInterval));
		}
	}
#ifdef WIFI_ONESSID_SECURITY_SYNC
    //Sync WiFi Security object
	if (bSyncSameSSID == true){
		int getIndex = wifiSecIid.idx[0] ;
		int syncIndex = DualbandSync(getIndex);
			
		IID_INIT(syncWifiSecIid);
		syncWifiSecIid.level = 1;
		syncWifiSecIid.idx[0] = syncIndex;
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &syncWifiSecIid, &syncWifiSecObj)) != ZCFG_SUCCESS) {
			return ret;
		}

		if(secMode == SLIDER_POS_NONE)
			json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("None"));
		else if(secMode == SLIDER_POS_BASIC)
		{
			if (!strcmp(wep,"128bit"))
			{
				json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("WEP-128"));
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_WepModeSelect", json_object_new_string("WEP-128"));
			}
			else
			{
				json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("WEP-64"));
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_WepModeSelect", json_object_new_string("WEP-64"));
			}
			
			json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEPAuthentication", json_object_new_string("None"));
			json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEPEncryption", json_object_new_string("WEPEncryption"));
			
			json_object_object_add(syncWifiSecObj, "X_ZYXEL_AutoGenWepKey", json_object_new_boolean(X_ZYXEL_AutoGenWepKey));
			
			if(X_ZYXEL_AutoGenWepKey)
			{
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
				
				if (!strcmp(wep,"128bit"))
				{
					X_ZYXEL_Auto_WepKey26 = json_object_get_string(json_object_object_get(syncWifiSecObj, "X_ZYXEL_Auto_WepKey26"));
					json_object_object_add(syncWifiSecObj, "WEPKey", json_object_new_string(X_ZYXEL_Auto_WepKey26));
				}
				else
				{
					X_ZYXEL_Auto_WepKey10 = json_object_get_string(json_object_object_get(syncWifiSecObj, "X_ZYXEL_Auto_WepKey10"));
					json_object_object_add(syncWifiSecObj, "WEPKey", json_object_new_string(X_ZYXEL_Auto_WepKey10));
				}
			}
			else
			{
				if (!strcmp(wep,"128bit"))
				{
					json_object_object_add(syncWifiSecObj, "WEPKey", json_object_new_string(WEPKey));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEPKey2", json_object_new_string(X_ZYXEL_WEPKey2));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEPKey3", json_object_new_string(X_ZYXEL_WEPKey3));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEPKey4", json_object_new_string(X_ZYXEL_WEPKey4));
				}
				else
				{
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEP64Key1", json_object_new_string(X_ZYXEL_WEP64Key1));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEP64Key2", json_object_new_string(X_ZYXEL_WEP64Key2));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEP64Key3", json_object_new_string(X_ZYXEL_WEP64Key3));
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_WEP64Key4", json_object_new_string(X_ZYXEL_WEP64Key4));
				}
				
				json_object_object_add(syncWifiSecObj, "wepid", json_object_new_string(wepid));
				
				if(strlen(wepid) == 0)
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
				else
				{
					json_object_object_add(syncWifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(wepid[3]-'0'));
					//The value of wepid[3] is 1~4 but character, so it needs to be changed to number
					if(wepid[3]-'0' <= 0 || wepid[3]-'0' > 4)
						json_object_object_add(syncWifiSecObj, "X_ZYXEL_DefaultKeyID", json_object_new_int(1));
				}
			}
		}
		else if(secMode == SLIDER_POS_MORESECURE)
		{
			if(!strcmp(wpaMode,"wpa2psk")|| !strcmp(wpaMode,"wpawpa2psk"))
			{
				if(!strcmp(wpaMode,"wpa2psk")){
					json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("WPA2-Personal"));
				}
				else{
					json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("WPA-WPA2-Personal"));
				}
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_AutoGenPSK", json_object_new_boolean(X_ZYXEL_AutoGenPSK));
			
				if(X_ZYXEL_AutoGenPSK)
				{
					X_ZYXEL_Auto_PSK = json_object_get_string(json_object_object_get(syncWifiSecObj, "X_ZYXEL_Auto_PSK"));
					json_object_object_add(syncWifiSecObj, "KeyPassphrase", json_object_new_string(X_ZYXEL_Auto_PSK));
					json_object_object_add(syncWifiSecObj, "PreSharedKey", json_object_new_string(""));
				}
				else {
					if(strlen(psk_value) == 64) {
						psk_lowercase = (char*)malloc(strlen(psk_value)+1);
						memset(psk_lowercase, 0, strlen(psk_value)+1);
						stolower(psk_value, psk_lowercase, strlen(psk_value));
						json_object_object_add(syncWifiSecObj, "KeyPassphrase", json_object_new_string(""));
						json_object_object_add(syncWifiSecObj, "PreSharedKey", json_object_new_string(psk_lowercase));
					}
					else {
						json_object_object_add(syncWifiSecObj, "KeyPassphrase", json_object_new_string(psk_value));
					}
				}
			}
			else if(!strcmp(wpaMode,"wpa2"))
			{

				json_object_object_add(syncWifiSecObj, "ModeEnabled", json_object_new_string("WPA2-Enterprise"));
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_Preauth", json_object_new_boolean(X_ZYXEL_Preauth));
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_ReauthInterval", json_object_new_int(X_ZYXEL_ReauthInterval));
				
				json_object_object_add(syncWifiSecObj, "RadiusServerIPAddr", json_object_new_string(RadiusServerIPAddr));
				json_object_object_add(syncWifiSecObj, "RadiusServerPort", json_object_new_int(RadiusServerPort));
				json_object_object_add(syncWifiSecObj, "RadiusSecret", json_object_new_string(RadiusSecret));
			}
			
			if(!strcmp(encryp, "aes")){
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_Encryption", json_object_new_string("aes"));
			}
			if(!strcmp(encryp, "tkipaes")){
				json_object_object_add(syncWifiSecObj, "X_ZYXEL_Encryption", json_object_new_string("tkipaes"));
			}
			json_object_object_add(syncWifiSecObj, "RekeyingInterval", json_object_new_int(RekeyingInterval));
		}
	}
#endif //WIFI_ONESSID_SECURITY_SYNC
	return ret;
}

zcfgRet_t cfgWifiSubnetObject(struct json_object *Jobj, const char* action){

	zcfgRet_t ret = ZCFG_SUCCESS;
	
	if(!strcmp(action, "DELETE"))
	{
		zcfgFeObjJsonDel(RDM_OID_DHCPV4_SRV_SUBNET, &wifiSubnetIid, NULL);
			return ret;
	}
	else if(!strcmp(action, "POST"))
	{
		zcfgFeObjJsonAdd(RDM_OID_DHCPV4_SRV_SUBNET, &wifiSubnetIid, NULL);
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DHCPV4_SRV_SUBNET, &wifiSubnetIid, &wifiSubnetObj)) != ZCFG_SUCCESS) {
			return ret;
		}
	}
	else if(!strcmp(action, "PUT"))
	{
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DHCPV4_SRV_SUBNET, &wifiSubnetIid, &wifiSubnetObj)) != ZCFG_SUCCESS) {
			return ret;
		}
	}
	
	json_object_object_add(wifiSubnetObj, "Enable", json_object_new_boolean(wlSubnetState));
	json_object_object_add(wifiSubnetObj, "MinAddress", json_object_new_string(MinAddress));
	json_object_object_add(wifiSubnetObj, "MaxAddress", json_object_new_string(MaxAddress));
	json_object_object_add(wifiSubnetObj, "SubnetMask", json_object_new_string(SubnetMask));
	json_object_object_add(wifiSubnetObj, "LANIP", json_object_new_string(LANIP));
	json_object_object_add(wifiSubnetObj, "Interface", json_object_new_string(Interface));

	return ret;
}

/*
zcfgRet_t dalcmdWifiGeneralEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int Index;
	struct json_object *paramObj = NULL;
	const char *RadioReference = NULL, *SSIDReference = NULL;
	uint8_t SSIDIdx;
	const char *band = NULL;
	int64_t intValue;
	
	Index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	IID_INIT(wifiSsidIid);
	wifiSsidIid.level = 1;
	wifiSsidIid.idx[0] = Index;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiSsidIid, &wifiSsidObj)) != ZCFG_SUCCESS) {
		return ZCFG_NO_SUCH_OBJECT;
	}
	
	RadioReference = json_object_get_string(json_object_object_get(wifiSsidObj, "LowerLayers"));
	IID_INIT(wifiRadioIid);
	wifiRadioIid.level = 1;
	sscanf(RadioReference, "WiFi.Radio.%hhu", &(wifiRadioIid.idx[0]));
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_RADIO, &wifiRadioIid, &wifiRadioObj)) != ZCFG_SUCCESS) {
		return ZCFG_INTERNAL_ERROR;
	}

	IID_INIT(wifiApIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT, &wifiApIid, &wifiApObj) == ZCFG_SUCCESS) {
		SSIDReference = json_object_get_string(json_object_object_get(wifiApObj, "SSIDReference"));
		sscanf(SSIDReference, "WiFi.SSID.%hhu", &SSIDIdx);
		if(SSIDIdx == wifiSsidIid.idx[0]){
			break;
		}
		json_object_put(wifiApObj);
		wifiApObj = NULL;
	}
	if(wifiApObj == NULL)
		return ZCFG_INTERNAL_ERROR;

	// Get Sec Obj
	IID_INIT(wifiSecIid);
	wifiSecIid.level = 1;
	wifiSecIid.idx[0] = Index;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecIid, &wifiSecObj)) != ZCFG_SUCCESS) {
		return ZCFG_NO_SUCH_OBJECT;
	}
	
	//wlEnable
	if((paramObj = json_object_object_get(Jobj, "wlEnable")) != NULL)
		json_object_object_add(wifiSsidObj, "Enable", JSON_OBJ_COPY(paramObj));

	//SSID
	if((paramObj = json_object_object_get(Jobj, "SSID")) != NULL)
		json_object_object_add(wifiSsidObj, "SSID", JSON_OBJ_COPY(paramObj));

	//bandwidth
	if((paramObj = json_object_object_get(Jobj, "bandwidth")) != NULL){
		band = json_object_get_string(json_object_object_get(wifiApObj, "OperatingFrequencyBand"));

		intValue = json_object_get_int(paramObj);
		if(intValue == 0){
			json_object_object_add(wifiRadioObj, "X_ZYXEL_OBSS_coex", json_object_new_boolean(true));
			if(!strcmp(band, "2.4GHz")){
				json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("40MHz"));
			}
		}
		else{
			json_object_object_add(wifiRadioObj, "X_ZYXEL_OBSS_coex", json_object_new_boolean(false));
			if (intValue == 20)
				json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("20MHz"));
			else if (intValue == 40)
				json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("40MHz"));
			else if (strcmp(band, "2.4GHz") && intValue == 80)
				json_object_object_add(wifiRadioObj, "OperatingChannelBandwidth", json_object_new_string("80MHz"));
			else{
				if(replyMsg != NULL)
					strcpy(replyMsg, "Wrong bandwidth value");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
	}

	//channel
	if((paramObj = json_object_object_get(Jobj, "channel")) != NULL){
		intValue = json_object_get_int(paramObj);
		if (intValue == 0){
			json_object_object_add(wifiRadioObj, "AutoChannelEnable", json_object_new_boolean(true));
		}
		else{
			json_object_object_add(wifiRadioObj, "AutoChannelEnable", json_object_new_boolean(false));
			json_object_object_add(wifiRadioObj, "Channel", json_object_new_int(intValue));
		}
	}

	//MaxAssociatedDevices
	if((paramObj = json_object_object_get(Jobj, "MaxAssociatedDevices")) != NULL)
		json_object_object_add(wifiApObj, "MaxAssociatedDevices", JSON_OBJ_COPY(paramObj));

	// Security related
	if(validateWifiSecObj(Jobj,NULL) != ZCFG_SUCCESS){
		json_object_put(wifiSecObj);
		wifiSecObj = NULL;
		printf("Security related parameters failed validation..\n");
	}
	else{
		if((ret = editWifiSecObject(Jobj)) != ZCFG_SUCCESS){
			json_object_put(wifiSecObj);
			wifiSecObj = NULL;
			printf("Fail to set security related parameters..\n");
		}
	}
	
	//if(wifiApWpsObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_WPS, &wifiApWpsIid, wifiApWpsObj, NULL);
	if(wifiApObj) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT, &wifiApIid, wifiApObj, NULL);
	if(wifiSecObj!=NULL) zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecIid, wifiSecObj, NULL);	
	if(wifiSsidObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_WIFI_SSID, &wifiSsidIid, wifiSsidObj, NULL);
	if(wifiRadioObj) zcfgFeObjJsonSet(RDM_OID_WIFI_RADIO, &wifiRadioIid, wifiRadioObj, NULL);

	return ret;
}
*/

zcfgRet_t zcfgFeDalWifiGeneralEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *tmpRadioObj = NULL;
	objIndex_t tmpRadioIid;
	struct json_object *tmpSSIDObj = NULL;
	objIndex_t tmpSSIDIid;
	const char *band = NULL;
	const char *OperaBand = NULL;
	bool found = false;
	uint8_t radio = 0, tmpRadio = 0;
	bool mainSSID = false;
	int guestSSID = 0;
	int tmpGuestIdx = 0;
	const char *tmpLowerLayer = NULL;
	initGlobalWlObjects();
	getBasicWlInfo(Jobj);

	/* Check CLI wlan edit input. */
	if(json_object_object_get(Jobj, "Band")){
		band = json_object_get_string(json_object_object_get(Jobj,"Band"));
		IID_INIT(tmpRadioIid);
		/* Check device if it supports the band that input from CLI. */
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_RADIO, &tmpRadioIid, &tmpRadioObj) == ZCFG_SUCCESS){
			OperaBand = json_object_get_string(json_object_object_get(tmpRadioObj, "OperatingFrequencyBand"));
			if(!strcmp(band, OperaBand)) {
				found = true;
				json_object_put(tmpRadioObj);
				break;
			}
			json_object_put(tmpRadioObj);
		}
		if(found == false){
			printf("This device doesn't support %s.\n", band);
			goto dalwlan_edit_fail;
		}
		/* Accordong Band, MainSSID or GuestSSID to add Index to Jobj. */
		if (!strcmp(band, "2.4GHz"))
			radio = 1;
		else if (!strcmp(band, "5GHz"))
			radio = 2;
		mainSSID = json_object_get_boolean(json_object_object_get(Jobj, "MainSSID"));
		if (mainSSID == false){
			guestSSID = json_object_get_int(json_object_object_get(Jobj, "GuestSSID"));
		}
		IID_INIT(tmpSSIDIid);
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &tmpSSIDIid, &tmpSSIDObj) == ZCFG_SUCCESS){
			if ((mainSSID == json_object_get_boolean(json_object_object_get(tmpSSIDObj, "X_ZYXEL_MainSSID")))
				&& (json_object_get_int(json_object_object_get(tmpSSIDObj, "X_ZYXEL_Attribute")) == 0)){
				tmpLowerLayer = json_object_get_string(json_object_object_get(tmpSSIDObj, "LowerLayers"));
				sscanf(tmpLowerLayer, "WiFi.Radio.%hhu", &tmpRadio);
				if (radio == tmpRadio){
					if (mainSSID){ //wlan edit main ssid
						json_object_object_add(Jobj, "Index", json_object_new_int(tmpSSIDIid.idx[0]));
						json_object_put(tmpSSIDObj);
						break;
					}
					else { //wlan edit guest ssid
						tmpGuestIdx++;
						if (guestSSID == tmpGuestIdx){ //enter guest ssid can be found in datamodel
							json_object_object_add(Jobj, "Index", json_object_new_int(tmpSSIDIid.idx[0]));
							json_object_put(tmpSSIDObj);
							break;
						}
					}
				}
			}
			json_object_put(tmpSSIDObj);
		}
	}
	if((ret = getWlObj(Jobj,NULL)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;

	if((ret = validateWlObj(Jobj,NULL)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;

	prepareWlObj(Jobj);
	
	if((ret = editWifiSsidObjects(Jobj)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;

	if((ret = editWifiSecObject(Jobj)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;
	
	/*if((ret = editWifiApWpsObject(Jobj)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;*/
	
	if((ret = editWifiApObject(Jobj)) != ZCFG_SUCCESS)
		goto dalwlan_edit_fail;
	
	if(div_wifiSubnet)
	{
		if((!strcmp(subnetObjAction, "POST")) || (!strcmp(subnetObjAction, "PUT")) || (!strcmp(subnetObjAction, "DELETE"))) {
			if((ret = cfgWifiSubnetObject(Jobj, subnetObjAction)) != ZCFG_SUCCESS)
				goto dalwlan_edit_fail;	
		}
	}
	
	if(div_wifiAdvance)
	{
		if((ret = editWifiRadioObjects(Jobj)) != ZCFG_SUCCESS)
			goto dalwlan_edit_fail;
	}
	
	setAllWlObjects(Jobj);

dalwlan_edit_fail:
	freeAllWlObjects();
	
	return ret;
}

zcfgRet_t getWlObj(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int ssidIndex;

	// radio obj
	objIndex_t tmpSsidIid;
	struct json_object *tmpSsidObj = NULL;
	const char *tmpLowerLayer = NULL;
	uint8_t radioIndex;
	uint8_t tmpRadioIndex;

	// subnet obj
	objIndex_t tmpSubnetIid;
	struct json_object *tmpSubnetObj = NULL;
	const char *tmpIntf = NULL;
	uint8_t subnetIdx;
	uint8_t tmpSubnetIdx;
	
	if(!json_object_object_get(Jobj,"Index")){
		printf("Error : %s needs parameter \"Index\".\n",__FUNCTION__);
		return ZCFG_NO_SUCH_PARAMETER;
	}

#ifdef ZYXEL_ONESSID
	IID_INIT(wifiOneSsidIid);
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ONE_SSID, &wifiOneSsidIid, &wifiOneSsidObj) != ZCFG_SUCCESS){
		printf("%s : Get RDM_OID_WIFI_ONE_SSID object failed.\n",__FUNCTION__);
		return ZCFG_NO_SUCH_OBJECT;
	}
#endif

	// get index
	ssidIndex = json_object_get_int(json_object_object_get(Jobj,"Index"));

	// ssid/ap/sec 
	wifiSsidIid.level = 1;
	wifiSsidIid.idx[0] = ssidIndex;
	wifiApIid.level = 1;
	wifiApIid.idx[0] = ssidIndex;
	wifiSecIid.level = 1;
	wifiSecIid.idx[0] = ssidIndex;

	// ssid/ap/sec
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_SSID, &wifiSsidIid, &wifiSsidObj) != ZCFG_SUCCESS){
		printf("%s : Get RDM_OID_WIFI_SSID object failed.\n",__FUNCTION__);
		return ZCFG_NO_SUCH_OBJECT;
	}
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT, &wifiApIid, &wifiApObj) != ZCFG_SUCCESS){
		printf("%s : Get RDM_OID_WIFI_ACCESS_POINT object failed.\n",__FUNCTION__);
		return ZCFG_NO_SUCH_OBJECT;
	}
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_SEC, &wifiSecIid, &wifiSecObj) != ZCFG_SUCCESS){
		printf("%s : Get RDM_OID_WIFI_ACCESS_POINT_SEC object failed.\n",__FUNCTION__);
		return ZCFG_NO_SUCH_OBJECT;
	}

	// radio
	if(json_object_get_boolean(json_object_object_get(wifiSsidObj, "X_ZYXEL_MainSSID"))){
		// get radio iid
		IID_INIT(tmpSsidIid);
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &tmpSsidIid, &tmpSsidObj) == ZCFG_SUCCESS) {
			tmpLowerLayer = json_object_get_string(json_object_object_get(tmpSsidObj, "LowerLayers"));
			sscanf(tmpLowerLayer, "WiFi.Radio.%hhu", &tmpRadioIndex);
			if(tmpRadioIndex == ssidIndex){
				break;
			}
			json_object_put(tmpSsidObj);
			tmpSsidObj = NULL;
		}
		radioIndex = tmpRadioIndex;
		json_object_put(tmpSsidObj);
		tmpSsidObj = NULL;
		wifiRadioIid.level = 1;
		wifiRadioIid.idx[0] = radioIndex;

		// get radio obj
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_RADIO, &wifiRadioIid, &wifiRadioObj)) != ZCFG_SUCCESS) {
			printf("%s : Get RDM_OID_WIFI_RADIO object failed.\n",__FUNCTION__);
			return ZCFG_NO_SUCH_OBJECT;
		}

	}
	else{ // subnet
	}

	return ZCFG_SUCCESS;
}
bool isChannelInList(int num, char *list){
	char tmpList[100] = {0};
	char tmpChannel[5] = {0};
	sprintf(tmpList,",%d,%s,",AutoChannel,list);
	sprintf(tmpChannel,",%d,",num);
	if(strstr(tmpList,tmpChannel))
		return true;
	else
		return false;
}
zcfgRet_t validateWlObj(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	// ap: make sure when "wlEnableGuest" is set to 1 then "wlGwMode" must be set too
	if(json_object_object_get(Jobj, "wlEnableGuest") && json_object_get_boolean(json_object_object_get(Jobj, "wlEnableGuest")) && !json_object_object_get(Jobj, "wlGwMode")){
		printf("\"wlEnableGuest\" and \"wlGwMode\" should be set in pair.\n");
		return ZCFG_INVALID_PARAM_VALUE;
	}

	// radio & subnet
	if(json_object_get_boolean(json_object_object_get(wifiSsidObj, "X_ZYXEL_MainSSID"))){
		if((ret = validateWifiRadioObj(Jobj,NULL)) != ZCFG_SUCCESS){
			return ret;
		}	
	}
	else{
		if((ret = validateWifiSubnetObj(Jobj,NULL)) != ZCFG_SUCCESS){
			return ret;
		}	
	}
	
	// sec obj
	if((ret = validateWifiSecObj(Jobj,NULL)) != ZCFG_SUCCESS){
		printf("Security related parameters failed validation..\n");
		return ret;
	}	
	/*
	*/
	return ret;
}

zcfgRet_t validateWifiRadioObj(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	// radio
	bool isMainSSID;
	bool hasBandwidth;
	bool hasChannel;
	const char *wl_mode = NULL;
	const char *curBand = NULL;
	const char *band;
	const char *chList;
	const char *chListUpper;
	const char *chListLower;
	const char *chListAC80;	

	// radio: get mainSSID status, X_ZYXEL_Wireless_Mode
	isMainSSID = json_object_get_boolean(json_object_object_get(wifiSsidObj, "X_ZYXEL_MainSSID"));
	wl_mode = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_Wireless_Mode"));
	curBand = json_object_get_string(json_object_object_get(wifiRadioObj, "OperatingFrequencyBand"));

	// radio: get bandwidth and channel status
	if(json_object_object_get(Jobj, "bandwidth"))
		hasBandwidth = true;
	else
		hasBandwidth = false;
	if(json_object_object_get(Jobj, "channel"))
		hasChannel = true;
	else
		hasChannel = false;

	// radio: check "bandwidth" and "channel" show or not
	if(!hasBandwidth && !hasChannel){
		// pass
	}
	else if(!isMainSSID && (hasBandwidth || hasChannel)){
		printf("Guest SSID does not support Bandwidth and Channel setting.\n");
		return ZCFG_INVALID_PARAM_VALUE;
	}
	else if((hasBandwidth && !hasChannel) || (!hasBandwidth && hasChannel)){
		printf("Bandwidth and Channel should be set together.\n");
		return ZCFG_INVALID_PARAM_VALUE;
	}
	else{ // main_SSID and hasBandwidth and hasChannel

		// check band and mode
		if(!strcmp(curBand,"2.4GHz") && (strstr(wl_mode,"n") != NULL)){
			if(!(bandwidth == 20 || bandwidth == 40 || bandwidth == 0)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20, 40, and 0");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!strcmp(curBand,"2.4GHz") && (strstr(wl_mode,"n") == NULL)){
			if(!(bandwidth == 20)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!strcmp(curBand,"5GHz") && (strstr(wl_mode,"n") != NULL) && (strstr(wl_mode,"ac") != NULL)){
			if(!(bandwidth == 20 || bandwidth == 40 || bandwidth == 80)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20, 40, and 80");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!strcmp(curBand,"5GHz") && (strstr(wl_mode,"n") != NULL) && (strstr(wl_mode,"ac") == NULL)){
			if(!(bandwidth == 20 || bandwidth == 40)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20 and 40");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!strcmp(curBand,"5GHz") && (strstr(wl_mode,"n") == NULL) && (strstr(wl_mode,"ac") != NULL)){
			if(!(bandwidth == 20 || bandwidth == 80)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20 and 80");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!strcmp(curBand,"5GHz") && (strstr(wl_mode,"n") == NULL) && (strstr(wl_mode,"ac") == NULL)){
			if(!(bandwidth == 20)){
				printf("In %s %s mode only support \"bandwidth\" = %s.\n",curBand,wl_mode,"20");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else{
			// pass
		}

		// check if channel number correct
		band = json_object_get_string(json_object_object_get(wifiRadioObj, "OperatingFrequencyBand"));
		if(!strcmp(band,"2.4GHz")){
			chList = json_object_get_string(json_object_object_get(wifiRadioObj, "PossibleChannels"));
			chListUpper = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_UpperChannels"));
			chListLower = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_LowerChannels"));

			if(!isChannelInList(channel,chList)){
				printf("%s : available channels are %s\n",__FUNCTION__,chList);
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(((bandwidth == 40) || (bandwidth == 0)) && !(isChannelInList(channel,chListLower) || isChannelInList(channel,chListUpper))){
				printf("%s : available channels are %s,%s\n",__FUNCTION__,chListLower,chListUpper);
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else{
				// pass
			}			

		}
		else{ // 5G
			chList = json_object_get_string(json_object_object_get(wifiRadioObj, "PossibleChannels"));
			chListUpper = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_UpperChannels"));
			chListLower = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_LowerChannels"));
			chListAC80 = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_AC80_Channels"));

			if((bandwidth == 20) && !isChannelInList(channel,chList)){
				printf("%s : available channels are %s\n",__FUNCTION__,chList);
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if((bandwidth == 40) && !(isChannelInList(channel,chListLower) || isChannelInList(channel,chListUpper))){
				printf("%s : available channels are %s,%s\n",__FUNCTION__,chListLower,chListUpper);
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if((bandwidth == 80) && !isChannelInList(channel,chListAC80)){
				printf("%s : available channels are %s\n",__FUNCTION__,chListAC80);
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else{
				// pass
			}			
		}
	}

	return ret;
}
zcfgRet_t validateWifiSubnetObj(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool newSubnetState;

	// subnet: check if sunbet setting is correct
	if(json_object_object_get(Jobj,"wlSubnetState")){
		newSubnetState = json_object_get_boolean(json_object_object_get(Jobj,"wlSubnetState"));
		if(newSubnetState){
			if(!(json_object_object_get(Jobj,"MinAddress")&&
				 json_object_object_get(Jobj,"MaxAddress")&&
				 json_object_object_get(Jobj,"SubnetMask")&&
				 json_object_object_get(Jobj,"LANIP"))){
				printf("When enabling subnet setting, all subnet related parameters should be entered.\n");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
		if(!newSubnetState){
			if(json_object_object_get(Jobj,"MinAddress")||
				 json_object_object_get(Jobj,"MaxAddress")||
				 json_object_object_get(Jobj,"SubnetMask")||
				 json_object_object_get(Jobj,"LANIP")){
				printf("When disabling subnet setting, no subnet related parameter is allowed.\n");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
	}
	else{
		if(json_object_object_get(Jobj,"MinAddress")||
			 json_object_object_get(Jobj,"MaxAddress")||
			 json_object_object_get(Jobj,"SubnetMask")||
			 json_object_object_get(Jobj,"LANIP")){
			printf("Please set \"wlSubnetState\" to 1 or 0.\n");
			ret = ZCFG_INVALID_PARAM_VALUE;
		}
	}
	return ret;
}
zcfgRet_t validateWifiSecObj(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t wifiWPSIid;
	struct json_object *wifiWPSObj = NULL;
	int Index;
	bool autoGenMoreSecurePasswd;
	bool wpsEnabled = false;
	bool isMainSSID;

	if(!json_object_object_get(Jobj, "securityLevel") && !json_object_object_get(Jobj, "secMode")){
		if(json_object_object_get(Jobj, "wep")||
			json_object_object_get(Jobj, "WEPKey")||
			json_object_object_get(Jobj, "AutoGenWepKey")||
			json_object_object_get(Jobj, "WEPKey2")||
			json_object_object_get(Jobj, "WEPKey3")||
			json_object_object_get(Jobj, "WEPKey4")||
			json_object_object_get(Jobj, "WEP64Key1")||
			json_object_object_get(Jobj, "WEP64Key2")||
			json_object_object_get(Jobj, "WEP64Key3")||
			json_object_object_get(Jobj, "WEP64Key4")||
			json_object_object_get(Jobj, "wepid")||
			json_object_object_get(Jobj, "wpaMode")||
			json_object_object_get(Jobj, "AutoGenPSK")||
			json_object_object_get(Jobj, "psk_value")||
			json_object_object_get(Jobj, "encryp")||
			json_object_object_get(Jobj, "RekeyingInterval")){
			printf("\"securityLevel\" has to be set.\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}
		else
			return ZCFG_SUCCESS;
	}

	// get parameters
	Index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	isMainSSID = json_object_get_boolean(json_object_object_get(wifiSsidObj, "X_ZYXEL_MainSSID"));

	IID_INIT(wifiWPSIid);
	wifiWPSIid.level = 1;
	wifiWPSIid.idx[0] = Index;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_WIFI_ACCESS_POINT_WPS, &wifiWPSIid, &wifiWPSObj)) == ZCFG_SUCCESS) {
		if(json_object_get_boolean(json_object_object_get(wifiWPSObj, "Enable"))){
			wpsEnabled = true;
		}
		json_object_put(wifiWPSObj);
	}
	else{
		printf("%s : Get WPS object fail.\n",__FUNCTION__);
		return ZCFG_NO_SUCH_OBJECT;
	}


	// secMode must be set to proper value
	if(!json_object_object_get(Jobj, "secMode")){
		return ZCFG_INVALID_PARAM_VALUE;
	}

	if(secMode == SLIDER_POS_NONE){
		return ZCFG_SUCCESS;
	}
	else if(secMode == SLIDER_POS_BASIC){

		// if WPS is on, return false
		if(wpsEnabled){
			printf("WEP security does not support WPS. You should disable WPS first.\n");
			return ZCFG_REQUEST_REJECT;
		}

		// wep must be set to proper value
		if(!json_object_object_get(Jobj, "wep")){
			printf("wep must be set.\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}
		else if(strcmp(wep,"128bit") && strcmp(wep,"64bit")){
			printf("wep must be set to \"64bit\" or \"128bit\".\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}

		// AutoGenWepKey must be set
		if(!json_object_object_get(Jobj, "AutoGenWepKey")){
			printf("AutoGenWepKey must be set.\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}

		// if user-defined passwd be set, wepid must match key's order
		if(X_ZYXEL_AutoGenWepKey == false){

			// wepid must be set to proper value
			if(strcmp(wepid,"key1") && strcmp(wepid,"key2") && strcmp(wepid,"key3") && strcmp(wepid,"key4")){
				printf("wepid must be set to \"key1\", \"key2\", \"key3\", or \"key4\".\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}

			if(!strcmp(wep,"128bit") && !strcmp(wepid,"key1") && !json_object_object_get(Jobj, "WEPKey")){
				printf("\"key1\" and \"WEPKey\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"128bit") && !strcmp(wepid,"key2") && !json_object_object_get(Jobj, "WEPKey2")){
				printf("\"key2\" and \"WEPKey2\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"128bit") && !strcmp(wepid,"key3") && !json_object_object_get(Jobj, "WEPKey3")){
				printf("\"key3\" and \"WEPKey3\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"128bit") && !strcmp(wepid,"key4") && !json_object_object_get(Jobj, "WEPKey4")){
				printf("\"key4\" and \"WEPKey4\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"64bit") && !strcmp(wepid,"key1") && !json_object_object_get(Jobj, "WEP64Key1")){
				printf("\"key1\" and \"WEP64Key1\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"64bit") && !strcmp(wepid,"key2") && !json_object_object_get(Jobj, "WEP64Key2")){
				printf("\"key2\" and \"WEP64Key2\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"64bit") && !strcmp(wepid,"key3") && !json_object_object_get(Jobj, "WEP64Key3")){
				printf("\"key3\" and \"WEP64Key3\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!strcmp(wep,"64bit") && !strcmp(wepid,"key4") && !json_object_object_get(Jobj, "WEP64Key4")){
				printf("\"key4\" and \"WEP64Key4\" should be set together.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			else{
				dbg_printf("%s : wep=%s wepid=%s ..\n",__FUNCTION__,wep,wepid);
				return ZCFG_SUCCESS;
			}
		}
	}
	else if(secMode == SLIDER_POS_MORESECURE){

		// wpaMode must be set
		if(!json_object_object_get(Jobj, "wpaMode")){
			printf("wpaMode must be set in \"MoreSecure\" mode.\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}
	
		// wpaMode must be set to a proper value
#ifdef ZYXEL_WIFI_SUPPORT_WPA_MIX_MODE
		if(strcmp(wpaMode,"wpa2psk")&& strcmp(wpaMode,"wpawpa2psk") ){
			printf("wpaMode must be set to \"wpa2psk\" or \"wpawpa2psk\".\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}
#else
               if(strcmp(wpaMode,"wpa2psk") && strcmp(wpaMode,"wpa2")){
                       printf("wpaMode must be set to \"wpa2psk\" or \"wpa2\".\n");
                       return ZCFG_INVALID_PARAM_VALUE;
               }
#endif

		// AutoGenPSK is optional
		if(!json_object_object_get(Jobj, "AutoGenPSK")){
			if(json_object_object_get(Jobj, "psk_value")){
				printf("\"psk_value\" should be set when auto-generated password is disabled.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else{
			// When AutoGenPSK is set, psk_value must be set according to AutoGenPSK's value
			autoGenMoreSecurePasswd = json_object_get_boolean(json_object_object_get(Jobj, "AutoGenPSK"));
			if(!autoGenMoreSecurePasswd && !json_object_object_get(Jobj, "psk_value")){
				printf("\"psk_value\" should be set when auto-generated password is disabled.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}

		// encryp must be set
		if(json_object_object_get(Jobj, "encryp")){
#ifdef ZYXEL_WIFI_SUPPORT_WPA_MIX_MODE
			if(strcmp(encryp, "tkipaes") && strcmp(encryp, "aes")){
				printf("encryp must be set to \"tkipaes\" or \"aes\".\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
#else
			if(strcmp(encryp, "aes")){
				printf("encryp must be set to \"aes\".\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
#endif
		}

		return ZCFG_SUCCESS;
	}
	else{
		return ZCFG_INVALID_PARAM_VALUE;
	}
	return ZCFG_SUCCESS;
}

void prepareWlObj(struct json_object *Jobj){
#ifdef ZYXEL_ONESSID
	if(!json_object_object_get(Jobj, "oneSsidEnable")){
		oneSsidEnable = json_object_get_boolean(json_object_object_get(wifiOneSsidObj, "Enable"));
	}
#endif

	// ssid obj
	if(!json_object_object_get(Jobj, "wlEnable")){
		wlEnable = json_object_get_boolean(json_object_object_get(wifiSsidObj, "Enable"));
	}
	if(!json_object_object_get(Jobj, "SSID")){
		SSID = json_object_get_string(json_object_object_get(wifiSsidObj, "SSID"));
	}
	if(!json_object_object_get(Jobj, "X_ZYXEL_Multicast_Fwd")){
		X_ZYXEL_Multicast_Fwd = json_object_get_boolean(json_object_object_get(wifiSsidObj, "X_ZYXEL_Multicast_Fwd"));
	}
	if(!json_object_object_get(Jobj, "upRate")){
		upRate = json_object_get_int(json_object_object_get(wifiSsidObj, "X_ZYXEL_UpstreamRate"));
	}
	if(!json_object_object_get(Jobj, "downRate")){
		downRate = json_object_get_int(json_object_object_get(wifiSsidObj, "X_ZYXEL_DownstreamRate"));
	}

	// ap obj
	if(!json_object_object_get(Jobj,"MaxAssociatedDevices")){
		MaxAssociatedDevices = json_object_get_int(json_object_object_get(wifiApObj, "MaxAssociatedDevices"));
	}
	if(!json_object_object_get(Jobj,"wlHide")){
		if(json_object_get_boolean(json_object_object_get(wifiApObj, "SSIDAdvertisementEnabled"))){
			wlHide = false;
			json_object_object_add(Jobj, "wlHide", json_object_new_boolean(false));
		}
		else{
			wlHide = true;
			json_object_object_add(Jobj, "wlHide", json_object_new_boolean(true));
		}
	}
	if(!json_object_object_get(Jobj,"wlEnableGuest")){
		if(json_object_get_boolean(json_object_object_get(wifiApObj, "X_ZYXEL_BssIdIsolation"))){
			wlEnableGuest = true;
			json_object_object_add(Jobj, "wlEnableGuest", json_object_new_boolean(true));
		}
		else{
			wlEnableGuest = false;
			json_object_object_add(Jobj, "wlEnableGuest", json_object_new_boolean(false));
		}
		if(json_object_get_boolean(json_object_object_get(wifiApObj, "IsolationEnable"))){
			//strcpy(wlGwMode,"0");
			wlGwMode = "0";
			json_object_object_add(Jobj, "wlGwMode", json_object_new_string("0"));
		}
		else{
			//strcpy(wlGwMode,"1");
			wlGwMode = "1";
			json_object_object_add(Jobj, "wlGwMode", json_object_new_string("1"));
		}
	}

	// radio obj
	prepareWifiRadioObj(Jobj);

	// subnet obj
	prepareWifiSubnetObj(Jobj);
}
void prepareWifiRadioObj(struct json_object *Jobj){

  	// radio obj: prepare extcha
	const char *band;
	const char *chList;
	const char *chListUpper;
	const char *chListLower;
	const char *chListAC80;	

	band = json_object_get_string(json_object_object_get(wifiRadioObj, "OperatingFrequencyBand"));

	// Prepare div_wifiAdvance
	if(!json_object_object_get(Jobj,"div_wifiAdvance")){
		if(json_object_get_boolean(json_object_object_get(wifiSsidObj,"X_ZYXEL_MainSSID")))
			div_wifiAdvance = true;
		else
			div_wifiAdvance = false;
		json_object_object_add(Jobj, "div_wifiAdvance", json_object_new_boolean(div_wifiAdvance));
	}	

	// If guest SSIDs, return
	if(!div_wifiAdvance){
		dbg_printf("%s : not main SSID, no need to prepare Radio obj..\n",__FUNCTION__);
		return;
	}

	// Prepare bandwidth
	if(!json_object_object_get(Jobj, "bandwidth")){
		bandwidth = json_object_get_int(json_object_object_get(wifiRadioObj, "OperatingChannelBandwidth"));
		/* special case: In 2.4G, 
		 *	1. if X_ZYXEL_OBSS_coex == true && bandwidth == 40 then bandwidth is 20/40
		 *	2. if X_ZYXEL_OBSS_coex == false && bandwidth == 40 then bandwidth is 40
		*/
		if(!strcmp(band,"2.4GHz") && (bandwidth == 40) && json_object_get_boolean(json_object_object_get(wifiRadioObj, "X_ZYXEL_OBSS_coex"))){
			bandwidth = 0;
			dbg_printf("%s : In 2.4G, OBSS coexistence is on, so bandwidth is 20/40\n",__FUNCTION__);
		}
		json_object_object_add(Jobj, "bandwidth", json_object_new_int(bandwidth));
	}

	// Prepare channel
	if(!json_object_object_get(Jobj, "channel")){
		if(json_object_get_boolean(json_object_object_get(wifiRadioObj, "AutoChannelEnable")))
			channel = AutoChannel;
		else		
			channel = json_object_get_int(json_object_object_get(wifiRadioObj, "Channel"));
		json_object_object_add(Jobj, "channel", json_object_new_int(channel));
	}

	// Prepare extcha
	if(!json_object_object_get(Jobj, "extcha")){
		if(!strcmp(band,"2.4GHz")){
			chList = json_object_get_string(json_object_object_get(wifiRadioObj, "PossibleChannels"));
			chListUpper = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_UpperChannels"));
			chListLower = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_LowerChannels"));
			if(bandwidth==20 && isChannelInList(channel,chList)){
				extcha = 0;
				json_object_object_add(Jobj, "extcha", json_object_new_int(0));
			}
			else if(bandwidth==40 && isChannelInList(channel,chListLower)){
				extcha = 1;
				json_object_object_add(Jobj, "extcha", json_object_new_int(1));
			}
			else if(bandwidth==40 && isChannelInList(channel,chListUpper)){
				extcha = 2;
				json_object_object_add(Jobj, "extcha", json_object_new_int(2));
			}
			else if(bandwidth==0 && isChannelInList(channel,chListLower)){
				extcha = 1;
				json_object_object_add(Jobj, "extcha", json_object_new_int(1));
			}
			else if(bandwidth==0 && isChannelInList(channel,chListUpper)){
				extcha = 2;
				json_object_object_add(Jobj, "extcha", json_object_new_int(2));
			}
			else{
				printf("%s: ERROR!\n",__FUNCTION__);
			}
		}
		else{ // 5G
			chList = json_object_get_string(json_object_object_get(wifiRadioObj, "PossibleChannels"));
			chListUpper = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_UpperChannels"));
			chListLower = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_LowerChannels"));
			chListAC80 = json_object_get_string(json_object_object_get(wifiRadioObj, "X_ZYXEL_AC80_Channels"));
			if(bandwidth==20 && isChannelInList(channel,chList)){
				extcha = 0;
				json_object_object_add(Jobj, "extcha", json_object_new_int(0));
			}
			else if(bandwidth==40 && isChannelInList(channel,chListLower)){
				extcha = 1;
				json_object_object_add(Jobj, "extcha", json_object_new_int(1));
			}
			else if(bandwidth==40 && isChannelInList(channel,chListUpper)){
				extcha = 2;
				json_object_object_add(Jobj, "extcha", json_object_new_int(2));
			}
			else if(bandwidth==80 && isChannelInList(channel,chListAC80)){
				extcha = 0;
				json_object_object_add(Jobj, "extcha", json_object_new_int(0));
			}
			else{
				printf("%s: ERROR!\n",__FUNCTION__);
			}
		}
	}
}
void prepareWifiSubnetObj(struct json_object *Jobj){
	int index = 0;
	objIndex_t subnetIid;
	struct json_object *subnetObj = NULL;

	const char *subnetIntf = NULL;
	uint8_t SubnetIdx;
	bool foundSubnet = false;
	bool curSubnetState;
	bool newSubnetState;
	const char *ssidIntf[20]={0};

	// get index
	index = json_object_get_int(json_object_object_get(Jobj,"Index"));
	
	// get RDM subnet obj
	IID_INIT(subnetIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DHCPV4_SRV_SUBNET, &subnetIid, &subnetObj) == ZCFG_SUCCESS) {
		subnetIntf = json_object_get_string(json_object_object_get(subnetObj, "Interface"));
		sscanf(subnetIntf, "WiFi.SSID.%hhu", &SubnetIdx);
		if(SubnetIdx == index){
			foundSubnet = true;
			break;
		}
		json_object_put(subnetObj);
		subnetObj = NULL;
	}

	// Get current and new subnet state
	if(foundSubnet)
		curSubnetState = json_object_get_boolean(json_object_object_get(subnetObj,"Enable"));
	else
		curSubnetState = false;

	newSubnetState = json_object_get_boolean(json_object_object_get(Jobj,"wlSubnetState"));

	// Prepare action
	if(!json_object_object_get(Jobj,"subnetObjAction")){
		if(!curSubnetState && !newSubnetState){
			json_object_object_add(Jobj, "subnetObjAction", json_object_new_string(""));
			subnetObjAction = "";
		}
		if(!curSubnetState && newSubnetState){
			json_object_object_add(Jobj, "subnetObjAction", json_object_new_string("POST"));
			subnetObjAction = "POST";
		}
		if(curSubnetState && !newSubnetState){
			json_object_object_add(Jobj, "subnetObjAction", json_object_new_string("DELETE"));
			subnetObjAction = "DELETE";
		}
		if(curSubnetState && newSubnetState){
			json_object_object_add(Jobj, "subnetObjAction", json_object_new_string("PUT"));
			subnetObjAction = "PUT";
		}
	}
	else{
		// special case
		if(subnetObjAction == NULL){
			json_object_object_add(Jobj, "subnetObjAction", json_object_new_string(""));
			subnetObjAction = "";
		}
	}

	// Prepare iid
	if(!strcmp(subnetObjAction,"DELETE") || !strcmp(subnetObjAction,"PUT")){
		wifiSubnetIid.level = 1;
		wifiSubnetIid.idx[0] = subnetIid.idx[0];
	}

	// Prepare Interface
	if(!json_object_object_get(Jobj,"Interface")){
		if(newSubnetState){
			sprintf(ssidIntf, "WiFi.SSID.%u", index);
			json_object_object_add(Jobj, "Interface", json_object_new_string(ssidIntf));
			Interface = json_object_get_string(json_object_object_get(Jobj,"Interface"));
		}
	}

	// Prepare div_wifiSubnet
	if(!json_object_object_get(Jobj,"div_wifiSubnet")){
		if(json_object_get_boolean(json_object_object_get(wifiSsidObj,"X_ZYXEL_MainSSID"))){
			json_object_object_add(Jobj, "div_wifiSubnet", json_object_new_boolean(false));
			div_wifiSubnet = false;
		}
		else{
			json_object_object_add(Jobj, "div_wifiSubnet", json_object_new_boolean(true));
			div_wifiSubnet = true;
		}
	}	

	json_object_put(subnetObj);
}

void zcfgDalGetIid(struct json_object *Jobj, char *iidName, objIndex_t *iid)
{
	if(!strcmp(iidName,"wlSsidIid") && !strcmp(iidName,"wlApIid") && !strcmp(iidName,"wlSecIid")){
		iid->level = 1;
		iid->idx[0] = json_object_get_int(json_object_object_get(Jobj,"Index"));
	}
	else{
		printf("Iid name %s is not yet supported.\n",iidName);
	}	
	/*
	struct json_object *iidArray = NULL;
	uint8_t iidIdx;

	iidArray = json_object_object_get(Jobj, iidName);
	for (iidIdx = 0 ; iidIdx < 6 ; iidIdx++) {
		iid->idx[iidIdx] = json_object_get_int(json_object_array_get_idx(iidArray, iidIdx));
		if (iid->idx[iidIdx] != 0) {
			iid->level = iidIdx + 1;
		}
	}
	*/
	return;
}

zcfgRet_t zcfgFeDalWifiGeneral(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int arrLen=0;
	int idx=0;
	struct json_object *obj = NULL;

	// for new gui
	if ((json_object_get_type(Jobj) == json_type_array) && (!strcmp(method, "PUT")))
	{
		arrLen = json_object_array_length(Jobj);
		for (idx = 0; idx < arrLen; idx++)
		{
			obj = json_object_array_get_idx(Jobj,idx);
			ret = zcfgFeDalWifiGeneralEdit(obj,NULL);
			if (ret!=ZCFG_SUCCESS)
				return ret;
		}
	}
	else
	{
		//if(!strcmp(method, "POST"))
			//ret = zcfgFeDalWifiGeneralAdd(Jobj, NULL);
		if(!strcmp(method, "PUT")) {
			ret = zcfgFeDalWifiGeneralEdit(Jobj, NULL);
		}
		else if(!strcmp(method, "GET"))
			ret = zcfgFeDalWifiGeneralGet(Jobj, Jarray, NULL);
		else
			printf("Unknown method:%s\n", method);
	}
	return ret;
}


