#include <json/json.h>

#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_msg.h"

#define CWMP_SOAP_MSG_DEBUG (1 << 1)
#define isEmptyIns(obj) json_object_get_boolean(json_object_object_get(obj, "emptyIns"))


dal_param_t NETWORK_MAP_param[] = 
{
	{"Internet_Blocking_Enable",	    dalType_boolean,	0,	0,	NULL},
	{"HostName",						dalType_string,		0,	0,	NULL},
	{"IPAddress",						dalType_string,		0,	0,	NULL},
	{"IPLinkLocalAddress6",				dalType_string,		0,	0,	NULL},
	{"PhysAddress",						dalType_string,		0,	0,	NULL},
	{"AddressSource",					dalType_string,		0,	0,	NULL},
	{"X_ZYXEL_ConnectionType",			dalType_string,		0,	0,	NULL},
	{NULL,		0,	0,	0,	NULL}
};

bool isSameMac(const char *mac1, const char *mac2){
	int i;
	
	for(i=0;i<18;i++){
		if((*mac1 == *mac2) || (*mac1 == *mac2 + 32) || (*mac1 == *mac2 - 32)){
			mac1++;
			mac2++;
			continue;
		}
		else if(*mac1 == ':' || *mac1 == '-'){
			mac1++;
			mac2++;
			continue;
		}
		else 
			return false;
		
		mac1++;
		mac2++;
	}
	return true;
}

bool genIid(struct json_object **iid, int iid1, int iid2, int iid3, int iid4, int iid5, int iid6){
	
	*iid = json_object_new_array();
	json_object_array_add(*iid, json_object_new_int(iid1));
	json_object_array_add(*iid, json_object_new_int(iid2));
	json_object_array_add(*iid, json_object_new_int(iid3));
	json_object_array_add(*iid, json_object_new_int(iid4));
	json_object_array_add(*iid, json_object_new_int(iid5));
	json_object_array_add(*iid, json_object_new_int(iid6));
	
}

void getDHCPStatus(char *status, char *tr181Path, const char *type, const char *ipIfacePath, struct json_object *dhcp4PoolJarray, struct json_object *dhcp4ClientJarray){
	int len, i;
	const char *Interface = NULL, *DHCPStatus = NULL;
	struct json_object *dhcp4PoolJobj, *dhcp4ClientJobj;

	strcpy(status, "Disable");
	strcpy(tr181Path, "");

	if(!strcmp(type, "LAN")){
		len = json_object_array_length(dhcp4PoolJarray);
		for(i=0;i<len;i++){
			dhcp4PoolJobj = json_object_array_get_idx(dhcp4PoolJarray, i);
			Interface = Jgets(dhcp4PoolJobj, "Interface");
			if(Interface == NULL)
				continue;

			if(!strcmp(ipIfacePath, Interface)){
				sprintf(tr181Path, "Device.DHCPv4.Server.Pool.%d", i+1);
				if(Jgetb(dhcp4PoolJobj, "Enable"))
					strcpy(status, "Server");
				break;
			}
		}
	}
	else{
		len = json_object_array_length(dhcp4ClientJarray);
		for(i=0;i<len;i++){
			dhcp4ClientJobj = json_object_array_get_idx(dhcp4ClientJarray, i);
			Interface = Jgets(dhcp4ClientJobj, "Interface");
			if(Interface == NULL)
				continue;

			if(!strcmp(ipIfacePath, Interface)){
				sprintf(tr181Path, "Device.DHCPv4.Client.%d", i+1);
				DHCPStatus = Jgets(dhcp4ClientJobj, "DHCPStatus");
				if(DHCPStatus == NULL)
					strcpy(status, "");
				else
					strcpy(status, DHCPStatus);
				break;
			}
		}
	}
}

/*
 * return Status get PPP uptime every time.
 *  @param[IN]	   inputKey: PPP obj index
 *  @param[OUT] varValue: PPP lastConnectionUpTime
 */
int getPPPUptime(unsigned char idx){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *pppIfaceObj = NULL;
	objIndex_t pppIfaceIid = {0};
	int X_ZYXEL_LastConnectionUpTime = 0;

	IID_INIT(pppIfaceIid);
	pppIfaceIid.idx[0] = idx;
	pppIfaceIid.level = 1;
	if((ret = zcfgFeObjJsonGet(RDM_OID_PPP_IFACE, &pppIfaceIid, &pppIfaceObj)) == ZCFG_SUCCESS){
		X_ZYXEL_LastConnectionUpTime = json_object_get_int(json_object_object_get(pppIfaceObj, "X_ZYXEL_LastConnectionUpTime"));
		zcfgFeJsonObjFree(pppIfaceObj);
	}

	return X_ZYXEL_LastConnectionUpTime;
}

/*
 * return WLAN and ETH Port list in Jason Array.
 * Format:Jarray.i.NAME = Name
 *		 Jarray.i.SSID = Name
 *           Jarray.i.IntfPath : Ethernet.Interface.i|WiFi.SSID.i
 *           Jarray.i.X_ZYXEL_LanPort
 *           Jarray.i.Status : Up|Down
 *           Jarray.i.Channel :
 *           Jarray.i.AutoChannelEnable :
 *           Jarray.i.OperatingStandards : b,g,n,a,ac
 *           Jarray.i.ModeEnabled : None|WEP-64|WEP-128|WPA-Personal|WPA2-Personal|WPA-WPA2-Personal
 *           Jarray.i.WPSEnable : true|false
 *           Jarray.i.X_ZYXEL_MainSSID : true|false
 */
zcfgRet_t getWifiInfo(struct json_object **Jarray){
	objIndex_t iid, secIid;
	struct json_object *Jobj = NULL;
	struct json_object *wifiRadioJarray, *wifiRadioObj, *wifiRadioStObj, *wifiSsidJarray, *wifiSsidObj;
	struct json_object *wifiApJarray, *wifiApObj, *wifiApSecObj, *wifiApWpsObj, *wifiSecObj;
	char intfPath[64] = {0};
	const char *LowerLayers, *SSID, *SSIDReference;
	unsigned char idx;
	int len, i;

	//update RDM_OID_WIFI_RADIO and RDM_OID_WIFI_RADIO_ST
	wifiRadioJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_WIFI_RADIO, &iid, &wifiRadioObj) == ZCFG_SUCCESS) {
		json_object_array_add(wifiRadioJarray, wifiRadioObj);
		zcfgFeObjJsonGet(RDM_OID_WIFI_RADIO_ST, &iid, &wifiRadioStObj);
		json_object_object_add(wifiRadioObj, "Stats", wifiRadioStObj);
	}

	zcfgFeWholeObjJsonGet("Device.WiFi.AccessPoint", &wifiApJarray);
	zcfgFeWholeObjJsonGet("Device.WiFi.SSID", &wifiSsidJarray);
	
	*Jarray = json_object_new_array();

	len = json_object_array_length(wifiApJarray);
	for(i=0;i<len;i++){
		Jobj = json_object_new_object();
		secIid.level = 1;
		secIid.idx[0] = i + 1;
		if(zcfgFeObjJsonGet(RDM_OID_WIFI_ACCESS_POINT_SEC, &secIid, &wifiSecObj) == ZCFG_SUCCESS){
			if(strlen(json_object_get_string(json_object_object_get(wifiSecObj, "KeyPassphrase")) ) > 0){
				json_object_object_add(Jobj, "wifiPassword", JSON_OBJ_COPY(json_object_object_get(wifiSecObj, "KeyPassphrase")));
			}
			else{
				json_object_object_add(Jobj, "wifiPassword", JSON_OBJ_COPY(json_object_object_get(wifiSecObj, "PreSharedKey")));
			}
			zcfgFeJsonObjFree(wifiSecObj);
		}
		
		wifiApObj = json_object_array_get_idx(wifiApJarray, i);
		wifiApSecObj = Jget(wifiApObj, "Security");
		wifiApWpsObj = Jget(wifiApObj, "WPS");
		SSIDReference = Jgets(wifiApObj, "SSIDReference");
		sscanf(SSIDReference, "WiFi.SSID.%hhu", &idx);
		wifiSsidObj = json_object_array_get_idx(wifiSsidJarray, idx-1);
		
		sprintf(intfPath, "WiFi.SSID.%u", idx);
		SSID = json_object_get_string(json_object_object_get(wifiSsidObj, "SSID"));
		LowerLayers = json_object_get_string(json_object_object_get(wifiSsidObj, "LowerLayers"));
		sscanf(LowerLayers, "WiFi.Radio.%hhu", &idx);
		wifiRadioObj = json_object_array_get_idx(wifiRadioJarray, idx-1);
		wifiRadioStObj = Jget(wifiRadioObj, "Stats");

#if	0
		BrPortPath = json_object_get_string(json_object_object_get(mappingObj, intfPath));
		if(BrPortPath == NULL)
			BrPortPath = "";
#endif
		
		//Jobj = json_object_new_object();
		json_object_object_add(Jobj, "SSID", json_object_new_string(SSID));
		//json_object_object_add(Jobj, "BrPortPath", json_object_new_string(BrPortPath));
		json_object_object_add(Jobj, "IntfPath", json_object_new_string(intfPath));
		replaceParam(Jobj, "Enable", wifiSsidObj, "Enable");
		replaceParam(Jobj, "MACAddress", wifiSsidObj, "MACAddress");
		replaceParam(Jobj, "X_ZYXEL_MainSSID", wifiSsidObj, "X_ZYXEL_MainSSID");

		replaceParam(Jobj, "OperatingFrequencyBand", wifiRadioObj, "OperatingFrequencyBand");
		replaceParam(Jobj, "OperatingChannelBandwidth", wifiRadioObj, "OperatingChannelBandwidth");
		replaceParam(Jobj, "Channel", wifiRadioObj, "Channel");
		replaceParam(Jobj, "AutoChannelEnable", wifiRadioObj, "AutoChannelEnable");
		replaceParam(Jobj, "OperatingStandards", wifiRadioObj, "OperatingStandards");
		replaceParam(Jobj, "X_ZYXEL_Wireless_Mode", wifiRadioObj, "X_ZYXEL_Wireless_Mode");
		replaceParam(Jobj, "X_ZYXEL_Rate", wifiRadioStObj, "X_ZYXEL_Rate");
		replaceParam(Jobj, "ModeEnabled", wifiApSecObj, "ModeEnabled");
		replaceParam(Jobj, "WPSEnable", wifiApWpsObj, "Enable");
			
		json_object_array_add(*Jarray, Jobj);
	}
	
	json_object_put(wifiRadioJarray);
	json_object_put(wifiApJarray);
	json_object_put(wifiSsidJarray);
	return ZCFG_SUCCESS;
}


/*
 * return WAN and LAN list in Jason Array.
 * Format:Jarray.i.NAME = Name
 *           Jarray.i.IpIfacePath = IP.Interface.i
 *           Jarray.i.wanIdx
 *           Jarray.i.wanpppIdx
 *           Jarray.i.X_ZYXEL_Type = WAN| LAN
 *           Jarray.i.LinkType = ATM|PTM|ETH|PON|USB (For WAN only)
 *           Jarray.i.X_ZYXEL_ConnectionType = IP_Routed|IP_Bridged (For WAN only)
 *           Jarray.i.Encapsulation = IPoE|IPoA|PPPoE|PPPoA (For WAN only, NUL if LinkType is USB))
 *           Jarray.i.BindToIntfGrp = true|false (For WAN only)
 *           Jarray.i.Group_WAN_IpIface = IP.Interface.i,IP.Interface.i,IP.Interface.i,... (For LAN only)
 *           Jarray.i.BridgingBrPath = Bridging.Bridge.i (For LAN only)
 *
 *		 Jarray.i.IPAddress = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.SubnetMask = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.MACAddress = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.PrimaryDNSserver = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.SecondaryDNSserver = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.IPAddress = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.DHCP = Bridging.Bridge.i (For LAN only)
 *		 Jarray.i.LowerlayerUp (For PPPoE or PPPoA case)
 *		 Jarray.i.pppConnectionStatus (For PPP)
 *		 Jarray.i.wanpppIdx (For PPP)
 *		 Jarray.i.DHCPtr181Path
 */
zcfgRet_t getWanLanList_ext(struct json_object **Jarray){
	struct json_object *ipIfaceJarray, *pppIfaceJarray, *v4AddrJarray, *v6AddrJarray, *v6PrefixJarray, *vlanTermJarray, *atmLinkJarray, *bridgeBrJarray, *dhcp4PoolJarray, *dhcp4ClientJarray, *dnsClientJarray = NULL;
	struct json_object *ipIfaceObj, *pppIfaceObj, *v4AddrObj, *v6AddrObj, *v6PrefixObj, *vlanTermObj, *ethLinkObj = NULL, *atmLinkObj, *bridgeBrObj, *dnsClientObj, *cellIntfZyIpPassthruObj = NULL;
	struct json_object *Jobj = NULL;
	struct json_object *addrJarray, *dnsv4Array = NULL, *dnsv6Array = NULL;
	char ifacePath[32] = {0}, BridgingBrPath[32] = {0};
	int len, len2, i, j;
	unsigned char idx;
	const char *X_ZYXEL_BridgeName, *X_ZYXEL_SrvName, *X_ZYXEL_ConnectionType, *X_ZYXEL_IfName, *X_ZYXEL_Group_WAN_IpIface, *LowerLayers;
#ifdef ZYXEL_CHECK_UNTAG //amet test for find out untag vlan interface group
	const char *port_LowerLayers;
#endif
	const char *X_ZYXEL_Type, *dns_tmp;
	char intfGrpWANList[512] = {0};
	char *intf = NULL, *tmp_ptr = NULL;
	const char *IpIfacePath = NULL;
	char DHCPStatus[16] = {0}, DHCPtr181Path[32] = {0}, ipv6dns[128] = {0}, ipv4dns[32] = {0};
	objIndex_t iid;	
	bool Enable = false;
   	int X_ZYXEL_LastConnectionUpTime = 0;
	struct json_object *ipIfaceObject;

	zcfgFeWholeObjJsonGet("Device.IP.Interface.", &ipIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.PPP.Interface.", &pppIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.Ethernet.VLANTermination.", &vlanTermJarray);
	zcfgFeWholeObjJsonGet("Device.ATM.Link.", &atmLinkJarray);
	zcfgFeWholeObjJsonGet("Device.Bridging.Bridge.", &bridgeBrJarray);
	zcfgFeWholeObjJsonGet("Device.DHCPv4.Server.Pool.", &dhcp4PoolJarray);
	zcfgFeWholeObjJsonGet("Device.DHCPv4.Client.", &dhcp4ClientJarray);
	zcfgFeWholeObjJsonGet("Device.DNS.Client.Server.", &dnsClientJarray);

	if(ipIfaceJarray==NULL||pppIfaceJarray==NULL||vlanTermJarray==NULL||bridgeBrJarray==NULL)
		return ZCFG_INTERNAL_ERROR;

	*Jarray = json_object_new_array();

	len = json_object_array_length(ipIfaceJarray);
	for(i=0;i<len;i++){
		ipIfaceObj = json_object_array_get_idx(ipIfaceJarray, i);
		Enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "Enable"));
		X_ZYXEL_SrvName = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_SrvName"));
		X_ZYXEL_ConnectionType = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConnectionType"));
		LowerLayers = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
		X_ZYXEL_Group_WAN_IpIface = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
		sprintf(ifacePath, "IP.Interface.%u", i+1);
		if(X_ZYXEL_SrvName==NULL || X_ZYXEL_ConnectionType==NULL || LowerLayers==NULL){
			continue;
		}
		
		Jobj = json_object_new_object();
		Jaddb(Jobj, "Enable", Enable);
		Jadds(Jobj, "IpIfacePath", ifacePath);
		Jaddi(Jobj, "wanIdx", i+1);
		Jaddb(Jobj, "BindToIntfGrp", false);
		Jadds(Jobj, "X_ZYXEL_Type", "WAN");
		Jadds(Jobj, "Encapsulation", "");
		Jadds(Jobj, "MACAddress", "");
		Jadds(Jobj, "LowerlayerUp", "");
		Jadds(Jobj, "pppConnectionStatus", "");
		Jaddi(Jobj, "wanpppIdx", 0);
		Jadds(Jobj, "DHCPtr181Path", "");
		replaceParam(Jobj, "Status", ipIfaceObj, "Status");
		replaceParam(Jobj, "X_ZYXEL_ConnectionType", ipIfaceObj, "X_ZYXEL_ConnectionType");
		replaceParam(Jobj, "X_ZYXEL_SrvName", ipIfaceObj, "X_ZYXEL_SrvName");
		replaceParam(Jobj, "Name", ipIfaceObj, "Name");
		replaceParam(Jobj, "Type", ipIfaceObj, "Type");
		replaceParam(Jobj, "IPv4Enable", ipIfaceObj, "IPv4Enable");
		replaceParam(Jobj, "IPv6Enable", ipIfaceObj, "IPv6Enable");
		replaceParam(Jobj, "X_ZYXEL_IfName", ipIfaceObj, "X_ZYXEL_IfName");
		replaceParam(Jobj, "IPv6Origin", ipIfaceObj, "X_ZYXEL_IPv6Origin");
		replaceParam(Jobj, "IPv6PrefixDelegateWAN", ipIfaceObj, "X_ZYXEL_IPv6PrefixDelegateWAN");
		replaceParam(Jobj, "X_ZYXEL_DefaultGatewayIface", ipIfaceObj, "X_ZYXEL_DefaultGatewayIface");
		Jadds(Jobj, "X_ZYXEL_IPv6LocalAddress", "");
		
#if 1	/* __ZyXEL__, GraceXiao, 20171225, #32394 The IPv6 Link Local Address can not display in Status webpage. */
		//update X_ZYXEL_IPv6LocalAddress
		IID_INIT(iid);
		iid.level = 1;
		iid.idx[0] = i+1;
		if(zcfgFeObjJsonGet(RDM_OID_IP_IFACE, &iid, &ipIfaceObject) == ZCFG_SUCCESS) {
			replaceParam(Jobj, "X_ZYXEL_IPv6LocalAddress", ipIfaceObject, "X_ZYXEL_IPv6LocalAddress");
			json_object_put(ipIfaceObject);
		}
#endif


		//the first item is for LAN
		if(i > 0){
			IID_INIT(iid);
			iid.level = 1;
			iid.idx[0] = i;
			if(zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &iid, &cellIntfZyIpPassthruObj) == ZCFG_SUCCESS) {
				replaceParam(Jobj, "IP_PassThru_Enable", cellIntfZyIpPassthruObj, "Enable");
				json_object_put(cellIntfZyIpPassthruObj);
			}
		}

		/*Address info*/
		addrJarray =  json_object_new_array();
		json_object_object_add(Jobj, "IPv4Address", addrJarray);
		v4AddrJarray = Jget(ipIfaceObj, "IPv4Address");
		if(v4AddrJarray != NULL){
			len2 = json_object_array_length(v4AddrJarray);
			for(j=0;j<len2;j++){
				v4AddrObj = json_object_array_get_idx(v4AddrJarray, j);
				if(isEmptyIns(v4AddrObj))
					continue;
				Jaddi(v4AddrObj, "ipIfaceIdx", i+1);
				json_object_array_add(addrJarray, JSON_OBJ_COPY(v4AddrObj));
				break;
			}
		}

		addrJarray =  json_object_new_array();
		json_object_object_add(Jobj, "IPv6Address", addrJarray);
		v6AddrJarray = Jget(ipIfaceObj, "IPv6Address");
		if(v6AddrJarray != NULL){
			len2 = json_object_array_length(v6AddrJarray);
			for(j=0;j<len2;j++){
				v6AddrObj = json_object_array_get_idx(v6AddrJarray, j);
				if(isEmptyIns(v6AddrObj))
					continue;
				Jaddi(v6AddrObj, "ipIfaceIdx", i+1);
				json_object_array_add(addrJarray, JSON_OBJ_COPY(v6AddrObj));
			}
		}

		/* V6 prefix info */
		addrJarray = json_object_new_array();
		json_object_object_add(Jobj, "IPv6Prefix", addrJarray);
		v6PrefixJarray = Jget(ipIfaceObj, "IPv6Prefix");
		if(v6PrefixJarray != NULL) {
			len2 = json_object_array_length(v6PrefixJarray);
			for(j = 0; j < len2; j++) {
				v6PrefixObj = json_object_array_get_idx(v6PrefixJarray, j);
				if(isEmptyIns(v6PrefixObj))
					continue;
				json_object_array_add(addrJarray, JSON_OBJ_COPY(v6PrefixObj));
			}
		}
		
		/* DNS client server */
		dnsv4Array = json_object_new_array();		
		dnsv6Array = json_object_new_array();	//	need FIX
		json_object_object_add(Jobj, "dnsv4Server", dnsv4Array);
		json_object_object_add(Jobj, "dnsv6Server", dnsv6Array);
		if(dnsClientJarray != NULL){
			len2 = json_object_array_length(dnsClientJarray);
			for(j=0; j<len2; j++){
				dnsClientObj = json_object_array_get_idx(dnsClientJarray, j);
				if(Jgets(dnsClientObj, "Interface") != NULL){
					if(!strcmp(Jgets(dnsClientObj, "Interface"),ifacePath)){
	
						dns_tmp = Jgets(dnsClientObj, "DNSServer");
						if(strchr(dns_tmp, ':')){	//ipv6 dns server
	
							strcpy(ipv6dns, dns_tmp);
							Jadds(dnsClientObj, "v6dns", ipv6dns);
							json_object_array_add(dnsv6Array, JSON_OBJ_COPY(dnsClientObj));
						}
						else{						//ipv4 dns server
	
							strcpy(ipv4dns, dns_tmp);
							Jadds(dnsClientObj, "v4dns", ipv4dns);
							json_object_array_add(dnsv4Array, JSON_OBJ_COPY(dnsClientObj));
						}
					}
				}
			}
		}

		int count = 0;
		const char *HigherLayer = "IP.Interface.";
		while(count < 4){
			count++;

			if(LowerLayers == NULL){
				json_object_put(Jobj);
				Jobj = NULL;
				break;
			}

			if(!strncmp(HigherLayer, "IP.Interface.", 13)){
				if(!strncmp(LowerLayers, "PPP.Interface.", 14)){
					sscanf(LowerLayers, "PPP.Interface.%hhu", &idx);
					pppIfaceObj = json_object_array_get_idx(pppIfaceJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(pppIfaceObj, "LowerLayers"));
					X_ZYXEL_LastConnectionUpTime = getPPPUptime(idx);
					replaceParam(Jobj, "pppConnectionStatus", pppIfaceObj, "ConnectionStatus");
					Jaddi(Jobj, "wanpppIdx", idx);
                    			Jaddi(Jobj, "pppoeConnectionUpTime", X_ZYXEL_LastConnectionUpTime);
				}
				else if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
				}
				else if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					if(ethLinkObj){
						zcfgFeJsonObjFree(ethLinkObj);
					}
					IID_INIT(iid);
					iid.level = 1;
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &(iid.idx[0]));
					zcfgFeObjJsonGet(RDM_OID_ETH_LINK, &iid, &ethLinkObj);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
//MTKSOC Patches
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
					replaceParam(Jobj, "MACAddress", ethLinkObj, "MACAddress");
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9) && !strcmp("IP_Routed", X_ZYXEL_ConnectionType)){
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp(HigherLayer, "PPP.Interface.", 14)){
				if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoE"));
					replaceParam(Jobj, "LowerlayerUp", vlanTermObj, "Status");
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9)){
					sscanf(LowerLayers, "ATM.Link.%hhu", &idx);
					atmLinkObj = json_object_array_get_idx(atmLinkJarray, idx-1);
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					replaceParam(Jobj, "LowerlayerUp", atmLinkObj, "Status");
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25)){
				if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					if(ethLinkObj){
						zcfgFeJsonObjFree(ethLinkObj);
					}
					IID_INIT(iid);
					iid.level = 1;
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &(iid.idx[0]));
					zcfgFeObjJsonGet(RDM_OID_ETH_LINK, &iid, &ethLinkObj);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
					replaceParam(Jobj, "MACAddress", ethLinkObj, "MACAddress");
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.Link.", HigherLayer, 14)){
				if(!strncmp("ATM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					break;
				}
				else if(!strncmp("PTM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PTM"));
					break;
				}
				else if(!strncmp("Ethernet.Interface.", LowerLayers, 19)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					break;
				}
				else if(!strncmp("Optical.Interface.", LowerLayers, 18)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PON"));
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					break;
				}
//MTKSOC Patches
				else if(!strncmp("Cellular.Interface.", LowerLayers, 19)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("CELL"));
					json_object_object_add(Jobj, "cellIntfPath", json_object_new_string(LowerLayers));
					break;
				}
				else if(!strncmp("Bridging.Bridge.", LowerLayers, 16)){
					sscanf(LowerLayers, "Bridging.Bridge.%hhu.Port.1", &idx);
					bridgeBrObj = json_object_array_get_idx(bridgeBrJarray, idx-1);
#ifdef ZYXEL_CHECK_UNTAG //amet test for untag vlan
					port_LowerLayers = json_object_get_string(json_object_object_get(bridgeBrObj, "Port"));
					port_LowerLayers = strstr(port_LowerLayers,"Ethernet.Interface.1");
					if(port_LowerLayers == NULL){
						port_LowerLayers = "";
						Jadds(Jobj, "untag_check", "false");
					}else{
						Jadds(Jobj, "untag_check", "true");
					}
#endif
					X_ZYXEL_BridgeName = json_object_get_string(json_object_object_get(bridgeBrObj, "X_ZYXEL_BridgeName"));
					if(X_ZYXEL_BridgeName == NULL){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					if(X_ZYXEL_Group_WAN_IpIface == NULL)
						X_ZYXEL_Group_WAN_IpIface = "";
						
					sprintf(BridgingBrPath, "Bridging.Bridge.%u", idx);
					json_object_object_add(Jobj, "X_ZYXEL_SrvName", json_object_new_string(X_ZYXEL_BridgeName));
					Jadds(Jobj, "X_ZYXEL_Type", "LAN");
					json_object_object_add(Jobj, "BridgingBrPath", json_object_new_string(BridgingBrPath));
					json_object_object_add(Jobj, "Group_WAN_IpIface", json_object_new_string(X_ZYXEL_Group_WAN_IpIface));
					strcat(intfGrpWANList, X_ZYXEL_Group_WAN_IpIface);
					strcat(intfGrpWANList, ",");
					break;
				}
				else if(!strcmp("", LowerLayers)){
					if(ethLinkObj){
						zcfgFeJsonObjFree(ethLinkObj);
					}
					IID_INIT(iid);
					iid.level = 1;
					sscanf(HigherLayer, "Ethernet.Link.%hhu", &(iid.idx[0]));
					zcfgFeObjJsonGet(RDM_OID_ETH_LINK, &iid, &ethLinkObj);
					if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
		}

		if(Jobj != NULL){
			X_ZYXEL_Type = Jgets(Jobj, "X_ZYXEL_Type");
			getDHCPStatus(DHCPStatus, DHCPtr181Path, X_ZYXEL_Type, ifacePath, dhcp4PoolJarray, dhcp4ClientJarray);
			Jadds(Jobj, "DHCPStatus", DHCPStatus);
			Jadds(Jobj, "DHCPtr181Path", DHCPtr181Path);
			json_object_array_add(*Jarray, Jobj);
		}
	}

	if(strcmp(intfGrpWANList, "")){
		intf = strtok_r(intfGrpWANList, ",", &tmp_ptr);
		while(intf != NULL){
			if(*intf != '\0'){
				len = json_object_array_length(*Jarray);
				for(i=0;i<len;i++){
					Jobj = json_object_array_get_idx(*Jarray, i);
					IpIfacePath = json_object_get_string(json_object_object_get(Jobj, "IpIfacePath"));
					if(!strcmp(IpIfacePath, intf)){
						json_object_object_add(Jobj, "BindToIntfGrp", json_object_new_boolean(true));
					}
				}
			}
			intf = strtok_r(NULL, ",", &tmp_ptr);
		}
	}

	json_object_put(ipIfaceJarray);
	json_object_put(pppIfaceJarray);
	json_object_put(vlanTermJarray);
	json_object_put(atmLinkJarray);
	json_object_put(bridgeBrJarray);
	json_object_put(dhcp4PoolJarray);
	json_object_put(dhcp4ClientJarray);
	json_object_put(ethLinkObj);
	json_object_put(dnsClientJarray);
	
	return ZCFG_SUCCESS;
}
zcfgRet_t zcfgFeDalStatusGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	struct json_object *statusJobj = NULL;
	struct json_object *firewallInfoObj = NULL, *firewallLevelObj = NULL, *gponObj = NULL, *wwanObj = NULL;
	struct json_object *ssidJarray = NULL;
	struct json_object *wanLanJarray = NULL;
	struct json_object *ethIfaceJarray = NULL, *ethIfaceObj = NULL;
	struct json_object *dslChannelJarray = NULL, *dslChannelObj = NULL;
	struct json_object *optIntfJarray = NULL, *optIbtfObj = NULL;
	struct json_object *wwanBackupJarray = NULL, *wwanBackupObj = NULL;
#ifdef ZYXEL_GUIVUE_USB_INFO_SHOW_SYSINFO
	struct json_object *usbHostDevParJarray = NULL, *usbHostDevParObj = NULL;
#endif
	struct json_object *voipProfJarray = NULL, *voipProfObj = NULL;
	struct json_object *voipProSIPfJarray = NULL, *voipProSIPfObj = NULL;
	struct json_object *voipLineJarray = NULL, *voipLineObj = NULL;
	struct json_object *gponstatusObj = NULL;
//MTKSOC Patches
	struct json_object *cellIntfObj = NULL;
	struct json_object *cellIntfZyIpPassthruObj = NULL;
	struct json_object *cellAccessPointJarray = NULL, *cellAccessPointObj = NULL;
	objIndex_t iid;	
	struct json_object *devInfoObj = NULL, *sysInfoObj = NULL, *hostObj = NULL; 
	zcfgRet_t ret = ZCFG_SUCCESS;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_DEV_INFO, &iid, &devInfoObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;
	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_SYSTEM_INFO, &iid, &sysInfoObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;
	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_FIREWALL, &iid, &firewallInfoObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_HOSTS, &iid, &hostObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;
	zcfgFeJsonObjFree(hostObj);

	IID_INIT(iid);
	iid.level = 1;
	iid.idx[0] = 1;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_FIREWALL_LEVEL, &iid, &firewallLevelObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;
	replaceParam(firewallInfoObj, "X_ZYXEL_DefaultConfig", firewallLevelObj, "X_ZYXEL_DefaultConfig");
	zcfgFeJsonObjFree(firewallLevelObj);


	ethIfaceJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_ETH_IFACE, &iid, &ethIfaceObj) == ZCFG_SUCCESS) {
		json_object_array_add(ethIfaceJarray, ethIfaceObj);
	}

	dslChannelJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DSL_CHANNEL, &iid, &dslChannelObj) == ZCFG_SUCCESS) {
		json_object_array_add(dslChannelJarray, dslChannelObj);
	}

	optIntfJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_OPT_INTF, &iid, &optIbtfObj) == ZCFG_SUCCESS) {
		json_object_array_add(optIntfJarray, optIbtfObj);
	}

	wwanBackupJarray = json_object_new_array();
	IID_INIT(iid);
	if(zcfgFeObjJsonGet(RDM_OID_ZY_W_W_A_N_BACKUP, &iid, &wwanBackupObj) == ZCFG_SUCCESS) {
		json_object_array_add(wwanBackupJarray, wwanBackupObj);
	}

#ifdef ZYXEL_GUIVUE_USB_INFO_SHOW_SYSINFO
	usbHostDevParJarray = json_object_new_array();
	objIndex_t usbIid = {0};
	usbIid.level = 3;
	usbIid.idx[0] = 1;
	usbIid.idx[1] = 2;
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_USB_USBHOSTS_HOST_DEV_PAR, &usbIid, &usbHostDevParObj) == ZCFG_SUCCESS){	
		json_object_array_add(usbHostDevParJarray, usbHostDevParObj);
	}
#endif

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_GPON_INFO, &iid, &gponstatusObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ONU, &iid, &gponObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_W_W_A_N_STAT, &iid, &wwanObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;

//MTKSOC Patches
	IID_INIT(iid);
	iid.level = 1;
	iid.idx[0] = 1;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF, &iid, &cellIntfObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &iid, &cellIntfZyIpPassthruObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalStatusGetFail;

	cellAccessPointJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_CELL_ACCESS_POINT, &iid, &cellAccessPointObj) == ZCFG_SUCCESS) {
		json_object_array_add(cellAccessPointJarray, cellAccessPointObj);
	}

	voipProfJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_VOICE_PROF, &iid, &voipProfObj) == ZCFG_SUCCESS) {
		Jaddi(voipProfObj, "idx", iid.idx[1]);
		json_object_array_add(voipProfJarray, voipProfObj);
	}

	voipProSIPfJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_VOICE_PROF_SIP, &iid, &voipProSIPfObj) == ZCFG_SUCCESS) {
		json_object_array_add(voipProSIPfJarray, voipProSIPfObj);
	}

	voipLineJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_VOICE_LINE, &iid, &voipLineObj) == ZCFG_SUCCESS) {
		Jaddi(voipLineObj, "idx0", iid.idx[0]);
		Jaddi(voipLineObj, "idx1", iid.idx[1]);
		Jaddi(voipLineObj, "idx2", iid.idx[2]);
		json_object_array_add(voipLineJarray, voipLineObj);
	}

	getWifiInfo(&ssidJarray);
	getWanLanList_ext(&wanLanJarray);
	
	statusJobj = json_object_new_object();
	json_object_array_add(Jarray, statusJobj);
	json_object_object_add(statusJobj, "DeviceInfo", devInfoObj);
	json_object_object_add(statusJobj, "SystemInfo", sysInfoObj);
	json_object_object_add(statusJobj, "FirewallInfo", firewallInfoObj);
	json_object_object_add(statusJobj, "LanPortInfo", ethIfaceJarray);
	json_object_object_add(statusJobj, "WiFiInfo", ssidJarray);
	json_object_object_add(statusJobj, "WanLanInfo", wanLanJarray);
	json_object_object_add(statusJobj, "DslChannelInfo", dslChannelJarray);
	json_object_object_add(statusJobj, "OptIntfInfo", optIntfJarray);
	json_object_object_add(statusJobj, "WWANBackupInfo", wwanBackupJarray);
#ifdef ZYXEL_GUIVUE_USB_INFO_SHOW_SYSINFO
	json_object_object_add(statusJobj, "USBStatsInfo", usbHostDevParJarray);
#endif	
	json_object_object_add(statusJobj, "GponStatsInfo", gponstatusObj);
	json_object_object_add(statusJobj, "GponInfo", gponObj);
	json_object_object_add(statusJobj, "WWANStatsInfo", wwanObj);
//MTKSOC Patches
	json_object_object_add(statusJobj, "CellIntfInfo", cellIntfObj);
	json_object_object_add(statusJobj, "CellIntfZyIpPassthruInfo", cellIntfZyIpPassthruObj);
	json_object_object_add(statusJobj, "CellAccessPointInfo", cellAccessPointJarray);
	json_object_object_add(statusJobj, "VoipProfInfo", voipProfJarray);
	json_object_object_add(statusJobj, "VoipProfSIPInfo", voipProSIPfJarray);
	json_object_object_add(statusJobj, "VoipLineInfo", voipLineJarray);
	
	return ZCFG_SUCCESS;

zcfgFeDalStatusGetFail:
	json_object_put(devInfoObj);
	json_object_put(sysInfoObj);
	json_object_put(firewallInfoObj);
	json_object_put(firewallLevelObj);
	json_object_put(hostObj);
	json_object_put(gponstatusObj);
//MTKSOC Patches
	json_object_put(cellIntfObj);
	json_object_put(cellIntfZyIpPassthruObj);
	return ret;

}

/*
 *  Function Name: zcfgFeDalCardPageStatusGet
 *  Description: Only get useful parameter or object to GUI.
 *
 */
zcfgRet_t zcfgFeDalCardPageStatusGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *statusJobj = NULL;
	struct json_object *devInfoObj = NULL, *wwanBackupObj = NULL;
	struct json_object *wanLanJarray = NULL;
	struct json_object *ethnewObj = NULL, *ethIfaceObj = NULL;
	struct json_object *ssidJarray = NULL, *MainSSIDobj = NULL, *GuestSSIDObj = NULL, *newssidJarray = NULL;
	struct json_object *dhcpv4SerObj = NULL;
	struct json_object *partctlObj = NULL;
	struct json_object *wwanObj = NULL;
	struct json_object *ethIfaceJarray = NULL, *ethwanIfaceObj = NULL;
	struct json_object *dslChannelJarray = NULL, *dslChannelObj = NULL;
	struct json_object *gponstatusObj = NULL;
//MTKSOC Patches
#ifdef ZYXEL_CHECK_UNTAG // add by amet for dhcp untag vlan check
	struct json_object *dhcptmpObj = NULL;
	int idx = 0;
	const char *dhcpPath = NULL;
#endif
	struct json_object *cellIntfObj = NULL;
	struct json_object *dhcpv4SERObj = NULL;
	struct json_object *cellIntfSimObj = NULL;
	struct json_object *cellIntfZyIpPassthruObj = NULL;
	objIndex_t iid;
#ifdef ZYXEL_WEB_GUI_SHOW_BLUETOOTH
    struct json_object *btIntfObj = NULL;
    struct json_object *btIntfJarray = NULL;
#endif
	int count = 0, maxBitRate = 0, len = 0, i;
	const char *lanport = NULL, *lanMacAddr = NULL, *duplexMode = NULL, *guestOperatingFrequencyBand = NULL, *mainOperatingFrequencyBand = NULL;
	//bool lanport_enable = NULL;//fix the bug: LAN MAC address disappear on homepage (System Info card page)
	char ethConnectionStatus[16] = {0};
	bool ethPortExist = false, MainSSID = false, Guest24GSSIDEnable = false, Main24GSSIDEnable = false, Guest5GSSIDEnable = false, Main5GSSIDEnable = false, GuestSSIDEnable = false, support24G = false, support5G = false;
	
	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_DEV_INFO, &iid, &devInfoObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;
	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_W_W_A_N_BACKUP, &iid, &wwanBackupObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_ZY_W_W_A_N_STAT, &iid, &wwanObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;

	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_GPON_INFO, &iid, &gponstatusObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;

#ifdef ZYXEL_WEB_GUI_SHOW_BLUETOOTH
    // update RDM_OID_BT_INTF
    btIntfJarray = json_object_new_array();
    IID_INIT(iid);
    while(zcfgFeObjJsonGetNext(RDM_OID_BT_INTF, &iid, &btIntfObj) == ZCFG_SUCCESS) {
        json_object_array_add(btIntfJarray, btIntfObj);
    }
#endif


//MTKSOC Patches
	IID_INIT(iid);
	iid.level = 1;
	iid.idx[0] = 1;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF, &iid, &cellIntfObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF_USIM, &iid, &cellIntfSimObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;
	if((ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &iid, &cellIntfZyIpPassthruObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;

	Jadds(devInfoObj, "PackageVersion", json_object_get_string (json_object_object_get(wwanBackupObj, "PackageVersion")));
	Jadds(devInfoObj, "ModuleSoftwareVersion", json_object_get_string (json_object_object_get(cellIntfObj, "X_ZYXEL_ModuleSoftwareVersion")));
    zcfgFeJsonObjFree(wwanBackupObj);

	ethIfaceJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_ETH_IFACE, &iid, &ethwanIfaceObj) == ZCFG_SUCCESS) {
		json_object_array_add(ethIfaceJarray, ethwanIfaceObj);
	}

	dslChannelJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DSL_CHANNEL, &iid, &dslChannelObj) == ZCFG_SUCCESS) {
		json_object_array_add(dslChannelJarray, dslChannelObj);
	}

	ethnewObj = json_object_new_object();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_ETH_IFACE, &iid, &ethIfaceObj) == ZCFG_SUCCESS) {
		lanport = json_object_get_string (json_object_object_get(ethIfaceObj, "X_ZYXEL_LanPort"));
		//lanport_enable = json_object_get_boolean (json_object_object_get(ethIfaceObj, "Enable"));
		if(!strncmp (lanport, "LAN", 3)) {
			//if(1 == lanport_enable) {
			lanMacAddr = json_object_get_string (json_object_object_get(ethIfaceObj, "MACAddress"));
			Jadds (ethnewObj, "LanMac", lanMacAddr);
			//}
		}
		else if (!strcmp (lanport, "ETHWAN")) {
			ethPortExist = true;
			maxBitRate = json_object_get_int (json_object_object_get(ethIfaceObj, "MaxBitRate"));
			duplexMode = json_object_get_string (json_object_object_get(ethIfaceObj, "DuplexMode"));
			if (maxBitRate != 0) {
				sprintf (ethConnectionStatus, "%d/%s", maxBitRate, duplexMode);
			}
			else {
				strcat (ethConnectionStatus, "Connection down");
			}
			Jadds (ethnewObj, "EthConnectionStatus", ethConnectionStatus);
		}
		Jaddb (ethnewObj, "ethPortExist", ethPortExist);
		zcfgFeJsonObjFree (ethIfaceObj);
	}

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_PAREN_CTL_PROF, &iid, &partctlObj) == ZCFG_SUCCESS){
		count++;
		zcfgFeJsonObjFree(partctlObj);		
	}

	getWifiInfo(&ssidJarray);
	//Just GET Main SSID object to GUI
	newssidJarray = json_object_new_array();
	len = json_object_array_length (ssidJarray);
	for (i = 0; i < len; i++) {
		MainSSIDobj = json_object_array_get_idx(ssidJarray, i);
		MainSSID = json_object_get_boolean (json_object_object_get(MainSSIDobj, "X_ZYXEL_MainSSID"));
		if (MainSSID) {
			json_object_array_add(newssidJarray, JSON_OBJ_COPY(MainSSIDobj));
			mainOperatingFrequencyBand = json_object_get_string (json_object_object_get(MainSSIDobj, "OperatingFrequencyBand"));
			if (!strcmp (mainOperatingFrequencyBand, "2.4GHz")) {
				Main24GSSIDEnable = json_object_get_boolean (json_object_object_get(MainSSIDobj, "Enable"));
			}
			else if (!strcmp (mainOperatingFrequencyBand, "5GHz")) {
				Main5GSSIDEnable = json_object_get_boolean (json_object_object_get(MainSSIDobj, "Enable"));
			}
			GuestSSIDObj = json_object_array_get_idx(ssidJarray, i + 1);
			if (GuestSSIDObj != NULL) {
				json_object_array_add(newssidJarray, JSON_OBJ_COPY(GuestSSIDObj));
				guestOperatingFrequencyBand = json_object_get_string (json_object_object_get(GuestSSIDObj, "OperatingFrequencyBand"));
				if (!strcmp (guestOperatingFrequencyBand, "2.4GHz")) {
					support24G = true;
					Guest24GSSIDEnable = json_object_get_boolean (json_object_object_get(GuestSSIDObj, "Enable"));
				}
				else if (!strcmp (guestOperatingFrequencyBand, "5GHz")) {
					support5G = true;
					Guest5GSSIDEnable = json_object_get_boolean (json_object_object_get(GuestSSIDObj, "Enable"));
				}
			}
		}
	}
	zcfgFeJsonObjFree(ssidJarray);

	//Consider if some project only support 2.4G or 5G only in the future.
	if (support24G && support5G) {	
		if (Main24GSSIDEnable && Main5GSSIDEnable && Guest24GSSIDEnable && Guest5GSSIDEnable) {
			GuestSSIDEnable = true;
		} 
	}
	else if (!support24G && support5G) {
		if (Main5GSSIDEnable && Guest5GSSIDEnable) {
			GuestSSIDEnable = true;
		}
	}
	else if (support24G && !support5G) {
		if (Main24GSSIDEnable && Guest24GSSIDEnable) {
			GuestSSIDEnable = true;
		}
	}

	getWanLanList_ext(&wanLanJarray);
	statusJobj = json_object_new_object();
	json_object_array_add(Jarray, statusJobj);
	json_object_object_add(statusJobj, "WanLanInfo", wanLanJarray);
	json_object_object_add(statusJobj, "DeviceInfo", devInfoObj);
	json_object_object_add(statusJobj, "LanPortInfo", ethnewObj);
	json_object_object_add(statusJobj, "WiFiInfo", newssidJarray);
#ifdef ZYXEL_CHECK_UNTAG //add by amet for test dhcpPool
	len = 0;
	idx = 1;
	len = json_object_array_length(wanLanJarray);
	for(i = 0; i < len ; i++){
		dhcptmpObj = json_object_array_get_idx(wanLanJarray, i);
		if(Jgets(dhcptmpObj, "untag_check") == NULL){
			continue;
		}
		if(strcmp(Jgets(dhcptmpObj, "untag_check"),"true") == 0 ){
			dhcpPath = Jgets(dhcptmpObj, "DHCPtr181Path");
			sscanf(dhcpPath, "Device.DHCPv4.Server.Pool.%hhu", &idx);
			break;
		}
	}
        IID_INIT(iid);
        iid.level = 1;
        iid.idx[0] = idx;
        if((ret = zcfgFeObjJsonGet(RDM_OID_DHCPV4_SRV_POOL, &iid, &dhcpv4SerObj)) != ZCFG_SUCCESS)
                goto zcfgFeDalCardPageStatusGetFail;
#else
	IID_INIT(iid);
	iid.level = 1;
	iid.idx[0] = 1;
	if((ret = zcfgFeObjJsonGet(RDM_OID_DHCPV4_SRV_POOL, &iid, &dhcpv4SerObj)) != ZCFG_SUCCESS)
		goto zcfgFeDalCardPageStatusGetFail;
#endif
#ifdef ZYXEL_WEB_GUI_ENABLE_DHCP_SRV
	IID_INIT(iid);
	if((ret = zcfgFeObjJsonGet(RDM_OID_DHCPV4_SRV, &iid, &dhcpv4SERObj)) != ZCFG_SUCCESS)
		goto  zcfgFeDalCardPageStatusGetFail;
	replaceParam(dhcpv4SerObj, "Enable", dhcpv4SERObj, "Enable");
#endif
#ifdef ZYXEL_WEB_GUI_SHOW_BLUETOOTH
    json_object_object_add(statusJobj, "BTIntfInfo", btIntfJarray);
#endif

	json_object_object_add(statusJobj, "Dhcp4SerPoolInfo", dhcpv4SerObj);
	json_object_object_add(statusJobj, "WWANStatsInfo", wwanObj);
	json_object_object_add(statusJobj, "LanInfo", ethIfaceJarray);
	json_object_object_add(statusJobj, "DslChannelInfo", dslChannelJarray);	
	json_object_object_add(statusJobj, "GponStatsInfo", gponstatusObj);
//MTKSOC Patches
	json_object_object_add(statusJobj, "CellIntfInfo", cellIntfObj);
	json_object_object_add(statusJobj, "CellIntfSimfInfo", cellIntfSimObj);
	json_object_object_add(statusJobj, "CellIntfZyIpPassthruInfo", cellIntfZyIpPassthruObj);
	Jaddi(statusJobj,"partctlNum",count);
	Jaddb(statusJobj,"GuestSSIDEnable",GuestSSIDEnable);

	return ZCFG_SUCCESS;

zcfgFeDalCardPageStatusGetFail:	
	json_object_put(devInfoObj);
	json_object_put(dhcpv4SerObj);
	json_object_put(wwanBackupObj);	
	json_object_put(wwanObj);
	json_object_put(gponstatusObj);
//MTKSOC Patches
	json_object_put(cellIntfObj);
	json_object_put(cellIntfSimObj);
	json_object_put(cellIntfZyIpPassthruObj);
	json_object_put(dhcpv4SERObj);
	return ret;
}

/*
 *  Function Name: zcfgFeDalLanPortInfoGet
 *  Description: Only get useful parameter or object to GUI.
 *
 */
zcfgRet_t zcfgFeDalLanPortInfoGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	struct json_object *statusJobj = NULL;
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *ethIfaceJarray = NULL, *ethIfaceObj = NULL;
	objIndex_t iid;	

	ethIfaceJarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_ETH_IFACE, &iid, &ethIfaceObj) == ZCFG_SUCCESS) {
		json_object_array_add(ethIfaceJarray, ethIfaceObj);
	}

	statusJobj = json_object_new_object();
	json_object_array_add(Jarray, statusJobj);
	json_object_object_add(statusJobj, "LanPortInfo", ethIfaceJarray);
	
	return ZCFG_SUCCESS;
}

void zcfgFeDalShowNetworkMAP(struct json_object *Jarray){
	int i, len = 0, len1 = 0;
	struct json_object *obj = NULL;
	const char *Name = NULL;
	char chartmp[64] = {0};
	const char *IPv6Address = NULL;
	struct json_object *networkMAPInfoJobj = NULL;
	struct json_object *hostsJarray;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	networkMAPInfoJobj = json_object_array_get_idx(Jarray, 0);
	if(networkMAPInfoJobj == NULL)
		return;
	hostsJarray = Jget(networkMAPInfoJobj,"lanhosts");
	if(hostsJarray == NULL)
		return;
	
	printf("%-20s %-17s %-30s %-20s %-16s %-5s\n",
		    "Name", "IP Address", "IPv6 Address", "MAC Address", "Address Source", "Connection Type");
	len = json_object_array_length(hostsJarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(hostsJarray, i);

		//if PhysAddress is empty, skip it.
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "PhysAddress")),""))
			continue;
		
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "HostName")),"")){
			Name = "Unknown";
			strcpy(chartmp,Name);
		}
		else{
			Name = json_object_get_string(json_object_object_get(obj, "HostName"));
			len1 = strlen(Name);
			strcpy(chartmp,Name);
			if(chartmp[len1-1] == '\n')
				chartmp[len1-1] = '\0';
		}
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPAddress6")),"")){
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPLinkLocalAddress6")),"")){
				IPv6Address = "N/A";
			}
			else{
				IPv6Address = json_object_get_string(json_object_object_get(obj, "IPLinkLocalAddress6"));
			}
		}
		else 
			IPv6Address = json_object_get_string(json_object_object_get(obj, "IPAddress6"));
		
		printf("%-20s %-17s %-30s %-20s %-16s %-5s \n",
			chartmp,
			json_object_get_string(json_object_object_get(obj, "IPAddress")),
			IPv6Address,
			json_object_get_string(json_object_object_get(obj, "PhysAddress")),
			json_object_get_string(json_object_object_get(obj, "AddressSource")),
			json_object_get_string(json_object_object_get(obj, "X_ZYXEL_ConnectionType")));
	}
}


/*
*	All parameters of RDM_OID_HOSTS and the following parameters
*	DeviceIcon
*	curHostName
*	DeviceName
*	dhcp4PoolExist
*	dhcp4PoolIid
*	dhcp4StaticAddrUsedByOtherHost
*	dhcp4StaticAddrExist
*	dhcp4StaticAddrNum
*	dhcp4StaticAddrIid
*	dhcp4StaticAddrEnable
*	dhcp4StaticAddr
*
*	Parameters For Elisa and CBT
*	Customized X_ZYXEL_ConnectionType
*	SupportedFrequencyBands
*	WiFiname
*/
zcfgRet_t zcfgFeDalLanHostsGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *obj = NULL;
	struct json_object *iconJobj, *iconJarray;
	struct json_object *ssidJobj, *ssidJarray;
	struct json_object *dhcp4PoolJobj, *dhcp4PoolJarray, *dhcp4PoolIid;
	struct json_object *dhcp4StaticAddrJobj, *dhcp4StaticAddrJarray, *dhcp4StaticAddrIid;
	struct json_object *radioJobj, *radioJarray;
	objIndex_t iid;
	struct json_object *stbJobj = NULL;
	int i, j, len, len2;
	const char *MacAddress, *IPAddress, *PhysAddress, *Layer1Interface, *Layer3Interface, *LowerLayers, *SupportedFrequencyBands, *Interface, *Yiaddr, *currIcon = NULL;
	char X_ZYXEL_ConnectionType[64] = {0};
	

	zcfgFeWholeObjJsonGet("Device.X_ZYXEL_Change_Icon_Name.", &iconJarray);
	zcfgFeWholeObjJsonGet("Device.X_ZYXEL_STB_VENDOR_ID.", &stbJobj);
	zcfgFeWholeObjJsonGet("Device.DHCPv4.Server.Pool.", &dhcp4PoolJarray);
#if (defined(CBT_CUSTOMIZATION) || defined (ELISA_NETWORKMAP_CUSTOMIZATION) || defined(ZYXEL_WEB_NETWORK_MAP_NEW_STYLE))
	zcfgFeWholeObjJsonGet("Device.WiFi.SSID.", &ssidJarray);
	zcfgFeWholeObjJsonGet("Device.WiFi.Radio.", &radioJarray);
#endif
	//update hosts list first
	IID_INIT(iid);
	if(zcfgFeObjJsonGet(RDM_OID_HOSTS, &iid, &obj) == ZCFG_SUCCESS){
		zcfgFeJsonObjFree(obj);
	}
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_HOSTS_HOST, &iid, &obj) == ZCFG_SUCCESS){
		PhysAddress = Jgets(obj, "PhysAddress");
		IPAddress = Jgets(obj, "IPAddress");
		Layer3Interface = Jgets(obj, "Layer3Interface");
		Layer3Interface = strstr(Layer3Interface, "IP."); //Layer3Interface value may be "IP.Interface.1" or "Device.IP.Interface.1" skip "Device."
		
		if(Layer3Interface == NULL)//consider the host under extender
			Layer3Interface = "";

		if(PhysAddress == NULL || IPAddress == NULL || Layer3Interface == NULL)
			continue;	
		if( (strlen(PhysAddress) < 2) || (validateIPv4(IPAddress) == false) )//It's not legal or empty.
			continue;
		Jadds(obj, "SupportedFrequencyBands", "");
		Jadds(obj, "WiFiname", "");
		Jadds(obj, "DeviceIcon", "");
		Jaddb(obj, "Internet_Blocking_Enable", false);
		replaceParam(obj, "DeviceName", obj, "HostName");
		Jadds(obj, "curHostName", "");
		
		if(!strcmp("", Jgets(obj,"DeviceName")))
			Jadds(obj, "DeviceName", "Unknown");

		if(!strcmp("", Jgets(obj,"HostName")))
			Jadds(obj, "HostName", "Unknown");
		
		//DHCPv4 static address info
		Jaddb(obj, "dhcp4PoolExist", false);
		genIid(&dhcp4PoolIid, 0, 0, 0, 0, 0, 0);
		json_object_object_add(obj, "dhcp4PoolIid", dhcp4PoolIid);
		Jaddb(obj, "dhcp4StaticAddrExist", false);
		genIid(&dhcp4StaticAddrIid, 0, 0, 0, 0, 0, 0);
		json_object_object_add(obj, "dhcp4StaticAddrIid", dhcp4StaticAddrIid);
		Jaddb(obj, "dhcp4StaticAddrEnable", false);
		Jadds(obj, "dhcp4StaticAddr", "");
		Jaddi(obj, "dhcp4StaticAddrNum", 0);
		Jaddb(obj, "dhcp4StaticAddrUsedByOtherHost", false);
		
		len = json_object_array_length(dhcp4PoolJarray);
		for(i=0;i<len;i++){
			dhcp4PoolJobj = json_object_array_get_idx(dhcp4PoolJarray, i);
			if(dhcp4PoolJobj == NULL)
				break;
			Interface = Jgets(dhcp4PoolJobj, "Interface");
			if(Interface == NULL || *Interface == '\0')
				continue;
			Interface = strstr(Interface, "IP."); //Interface value may be "IP.Interface.1" or "Device.IP.Interface.1" skip "Device."

			if(!strcmp(Interface, Layer3Interface)){
				Jaddb(obj, "dhcp4PoolExist", true);
				genIid(&dhcp4PoolIid, i+1, 0, 0, 0, 0, 0);
				json_object_object_add(obj, "dhcp4PoolIid", dhcp4PoolIid);
				
				json_object_object_add(obj, "staticIP", json_object_new_string(""));

				dhcp4StaticAddrJarray = Jget(dhcp4PoolJobj, "StaticAddress");
				if(dhcp4StaticAddrJarray == NULL){
					break;
				}

				len2 = json_object_array_length(dhcp4StaticAddrJarray);
				Jaddi(obj, "dhcp4StaticAddrNum", len2);
				for(j=0;j<len2;j++){
					dhcp4StaticAddrJobj = json_object_array_get_idx(dhcp4StaticAddrJarray, j);
					MacAddress = Jgets(dhcp4StaticAddrJobj, "Chaddr");
					if(MacAddress && isSameMac(MacAddress, PhysAddress)){
						Jaddb(obj, "dhcp4StaticAddrExist", true);
						replaceParam(obj, "dhcp4StaticAddr", dhcp4StaticAddrJobj, "Yiaddr");
						genIid(&dhcp4StaticAddrIid, i+1, j+1, 0, 0, 0, 0);
						json_object_object_add(obj, "dhcp4StaticAddrIid", dhcp4StaticAddrIid);
						replaceParam(obj, "dhcp4StaticAddrEnable", dhcp4StaticAddrJobj, "Enable");
						break;
					}
					Yiaddr = Jgets(dhcp4StaticAddrJobj, "Yiaddr");
					if(Yiaddr && !strcmp(Yiaddr, IPAddress)){ //different MAC but IP is the same
						Jaddb(obj, "dhcp4StaticAddrUsedByOtherHost", true);
						break;
					}
				}
				break;
			}
		}
		
		//ICON info
		len = json_object_array_length(iconJarray);
		for(i=0;i<len;i++){
			iconJobj = json_object_array_get_idx(iconJarray, i);
			if(iconJobj == NULL)
				break;
			MacAddress = Jgets(iconJobj, "MacAddress");
			if(MacAddress == NULL)
				continue;
			if(isSameMac(PhysAddress, MacAddress)){
				replaceParam(obj, "DeviceIcon", iconJobj, "DeviceIcon");
				replaceParam(obj, "Internet_Blocking_Enable", iconJobj, "Internet_Blocking_Enable");
				if(strcmp("", Jgets(iconJobj, "HostName")))
					replaceParam(obj, "curHostName", iconJobj, "HostName");
			}
		}
		currIcon = json_object_get_string(json_object_object_get(obj, "DeviceIcon"));
		if(!strcmp(currIcon, "iPC") || !strcmp(currIcon, "")){
			json_object_object_add(obj, "icon", json_object_new_string("ipc"));
		}else if(!strcmp(currIcon, "iGame")){
			json_object_object_add(obj, "icon", json_object_new_string("igame"));
		}else if(!strcmp(currIcon, "iPrinter")){
			json_object_object_add(obj, "icon", json_object_new_string("iprinter"));
		}else if(!strcmp(currIcon, "iStb")){
			json_object_object_add(obj, "icon", json_object_new_string("istb"));
		}else if(!strcmp(currIcon, "iCamera")){
			json_object_object_add(obj, "icon", json_object_new_string("icamera"));
		}else if(!strcmp(currIcon, "iServer")){
			json_object_object_add(obj, "icon", json_object_new_string("iserver"));
		}else if(!strcmp(currIcon, "iTelephone")){
			json_object_object_add(obj, "icon", json_object_new_string("itelephone"));
		}else if(!strcmp(currIcon, "iRouter")){
			json_object_object_add(obj, "icon", json_object_new_string("irouter"));
		}else if(!strcmp(currIcon, "iOthers")){
			json_object_object_add(obj, "icon", json_object_new_string("iothers"));
		}else if(!strcmp(currIcon, "iStereo")){
			json_object_object_add(obj, "icon", json_object_new_string("istereo"));
		}else if(!strcmp(currIcon, "iMobile")){
			json_object_object_add(obj, "icon", json_object_new_string("imobile"));
		}else if(!strcmp(currIcon, "iTV")){
			json_object_object_add(obj, "icon", json_object_new_string("itv"));
		}
		
#if (defined(CBT_CUSTOMIZATION) || defined (ELISA_NETWORKMAP_CUSTOMIZATION) || defined(ZYXEL_WEB_NETWORK_MAP_NEW_STYLE))
		Layer1Interface = Jgets(obj, "Layer1Interface");
		if(strstr(Layer1Interface, "SSID")){
			if(strstr(Layer1Interface, "Device.WiFi."))
				sscanf(Layer1Interface, "Device.WiFi.SSID.%d", &i);
			else
				sscanf(Layer1Interface, "WiFi.SSID.%d", &i);
			
			ssidJobj = json_object_array_get_idx(ssidJarray, i-1);
			replaceParam(obj, "WiFiname", ssidJobj, "SSID");
			LowerLayers = Jgets(ssidJobj, "LowerLayers");
			
			if(LowerLayers != NULL)
				sscanf(LowerLayers, "WiFi.Radio.%hhu", &i);		
			
			radioJobj = json_object_array_get_idx(radioJarray, i-1);
			SupportedFrequencyBands = Jgets(radioJobj, "OperatingFrequencyBand");

			if(SupportedFrequencyBands != NULL)
					Jadds(obj, "SupportedFrequencyBands", SupportedFrequencyBands);
#ifdef ELISA_NETWORKMAP_CUSTOMIZATION
			sprintf(X_ZYXEL_ConnectionType, "%s %s", Jgets(obj, "X_ZYXEL_ConnectionType"), SupportedFrequencyBands);
			Jadds(obj, "X_ZYXEL_ConnectionType", X_ZYXEL_ConnectionType);
#else
			Jadds(obj, "X_ZYXEL_ConnectionType", "Wi-Fi");
#endif
		}
#ifdef ELISA_NETWORKMAP_CUSTOMIZATION
		else if(strstr(Layer1Interface, "Radio")){
			sscanf(Layer1Interface, "Device.WiFi.Radio.%hhu", &i);
			radioJobj = json_object_array_get_idx(radioJarray, i-1);
			SupportedFrequencyBands = Jgets(radioJobj, "SupportedFrequencyBands");
			Jadds(obj, "SupportedFrequencyBands", SupportedFrequencyBands);
			
			sprintf(X_ZYXEL_ConnectionType, "%s %s", Jgets(obj, "X_ZYXEL_ConnectionType"), SupportedFrequencyBands);
			Jadds(obj, "X_ZYXEL_ConnectionType", X_ZYXEL_ConnectionType);
		}
#endif
#endif
		Layer1Interface = Jgets(obj, "Layer1Interface");
		if(strstr(Layer1Interface, "Device"))
			json_object_array_add(Jarray, obj);

		
	}
	zcfgFeJsonObjFree(iconJarray);
	zcfgFeJsonObjFree(stbJobj);
	zcfgFeJsonObjFree(dhcp4PoolJarray);
#if (defined(CBT_CUSTOMIZATION) || defined (ELISA_NETWORKMAP_CUSTOMIZATION) || defined(ZYXEL_WEB_NETWORK_MAP_NEW_STYLE))
	zcfgFeJsonObjFree(ssidJarray);
	zcfgFeJsonObjFree(radioJarray);
#endif

	return ret;
}

zcfgRet_t zcfgFeDalNetworkMAPEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *obj = NULL;
	objIndex_t Iid = {0};
	const char *macAddr = NULL, *curmacAddr = NULL, *GUIicon = NULL, *hostName = NULL;
	char icon[16] = {0};
	bool changeIconRuleExist = false, Internet_Blocking_Enable = false;
	macAddr = json_object_get_string(json_object_object_get(Jobj,"MacAddress"));
	GUIicon = json_object_get_string(json_object_object_get(Jobj,"DeviceIcon"));
	hostName = json_object_get_string(json_object_object_get(Jobj,"HostName"));
	Internet_Blocking_Enable = json_object_get_boolean(json_object_object_get(Jobj,"Internet_Blocking_Enable"));

	if(!strcmp(GUIicon, "ipc")){
		strcat(icon, "iPC");
	}else if(!strcmp(GUIicon, "igame")){
		strcat(icon, "iGame");
	}else if(!strcmp(GUIicon, "iprinter")){
		strcat(icon, "iPrinter");
	}else if(!strcmp(GUIicon, "istb")){
		strcat(icon, "iStb");
	}else if(!strcmp(GUIicon, "icamera")){
		strcat(icon, "iCamera");
	}else if(!strcmp(GUIicon, "iserver")){
		strcat(icon, "iServer");
	}else if(!strcmp(GUIicon, "itelephone")){
		strcat(icon, "iTelephone");
	}else if(!strcmp(GUIicon, "irouter")){
		strcat(icon, "iRouter");
	}else if(!strcmp(GUIicon, "iothers")){
		strcat(icon, "iOthers");
	}else if(!strcmp(GUIicon, "istereo")){
		strcat(icon, "iStereo");
	}else if(!strcmp(GUIicon, "imobile")){
		strcat(icon, "iMobile");
	}else if(!strcmp(GUIicon, "itv")){
		strcat(icon, "iTV");
	}
	while(zcfgFeObjJsonGetNext(RDM_OID_CHANGE_ICON_NAME, &Iid, &obj) == ZCFG_SUCCESS){
		curmacAddr = json_object_get_string(json_object_object_get(obj,"MacAddress"));
		if(isSameMac(macAddr, curmacAddr)){
			changeIconRuleExist = true;
			if(json_object_object_get(Jobj, "DeviceIcon")){
				json_object_object_add(obj, "DeviceIcon", json_object_new_string(icon));
			}
			if(json_object_object_get(Jobj, "HostName")){
				json_object_object_add(obj, "HostName", json_object_new_string(hostName));
			}
			if(json_object_object_get(Jobj, "MacAddress")){
				json_object_object_add(obj, "MacAddress", json_object_new_string(macAddr));
			}
			if(json_object_object_get(Jobj, "Internet_Blocking_Enable")){
				json_object_object_add(obj, "Internet_Blocking_Enable", json_object_new_boolean(Internet_Blocking_Enable));
			}
			zcfgFeObjJsonSet(RDM_OID_CHANGE_ICON_NAME, &Iid, obj, NULL);
			zcfgFeJsonObjFree(obj);
			break;			
		}
		zcfgFeJsonObjFree(obj);
	}
	IID_INIT(Iid);
	if(!changeIconRuleExist){
		ret = zcfgFeObjJsonAdd(RDM_OID_CHANGE_ICON_NAME, &Iid, NULL);
		if(ret == ZCFG_SUCCESS){
			ret = zcfgFeObjJsonGet(RDM_OID_CHANGE_ICON_NAME, &Iid, &obj);
			if(json_object_object_get(Jobj, "DeviceIcon")){
				json_object_object_add(obj, "DeviceIcon", json_object_new_string(icon));
			}
			if(json_object_object_get(Jobj, "HostName")){
				json_object_object_add(obj, "HostName", json_object_new_string(hostName));
			}
			if(json_object_object_get(Jobj, "MacAddress")){
				json_object_object_add(obj, "MacAddress", json_object_new_string(macAddr));
			}
			if(json_object_object_get(Jobj, "Internet_Blocking_Enable")){
				json_object_object_add(obj, "Internet_Blocking_Enable", json_object_new_boolean(Internet_Blocking_Enable));
			}
			zcfgFeObjJsonSet(RDM_OID_CHANGE_ICON_NAME, &Iid, obj, NULL);
			zcfgFeJsonObjFree(obj);
		}
	}
		
	return ret;
}


/*
[{
  "lanhosts":[{},...],
  "wanInfo":{"wanStatus": 1|0, "wanIfaceExist" : 1|0}
}]
*/
zcfgRet_t zcfgFeDalNetworkMAPGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	struct json_object *ipIfaceObj;
	struct json_object *v4AddrObj = NULL;
	struct json_object *v6AddrObj = NULL;
	struct json_object *networkMAPInfoJobj = NULL;
	struct json_object *hostsJarray;
	struct json_object *wanInfoJobj;
	struct json_object *stbJobj;
	const char *X_ZYXEL_IfName = NULL, *Status = NULL, *IPAddress = NULL, *addrStatus = NULL;
	char wantype[9] = {0};
	bool X_ZYXEL_DefaultGatewayIface;
	int wanstatus = 0, wanIfaceExist = 0;
	objIndex_t iid = {0};
	objIndex_t ipIfaceIid = {0};
	objIndex_t v4AddrIid = {0};
	objIndex_t v6AddrIid = {0};
	
	while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &ipIfaceIid, &ipIfaceObj) == ZCFG_SUCCESS){
		X_ZYXEL_IfName = json_object_get_string(json_object_object_get(ipIfaceObj,"X_ZYXEL_IfName"));
		if(X_ZYXEL_IfName == NULL){
			zcfgFeJsonObjFree(ipIfaceObj);
			continue;
		}
		if(strstr(X_ZYXEL_IfName, "br")){
			zcfgFeJsonObjFree(ipIfaceObj);
			continue;
		}
		wanIfaceExist = 1;
		Status = json_object_get_string(json_object_object_get(ipIfaceObj, "Status"));
		if(Status == NULL){
			Status = "";
		}
		X_ZYXEL_DefaultGatewayIface = json_object_get_boolean(json_object_object_get(ipIfaceObj,"X_ZYXEL_DefaultGatewayIface"));
#if (!defined(INTERNET_LED_DEPEND_ON_DEFAULT_GW))
		X_ZYXEL_DefaultGatewayIface = true;
#endif
		if(X_ZYXEL_DefaultGatewayIface && (!strcmp(Status, "Up") || !strcmp(Status, "LowerLayerDown"))){
			bool bIPv6Exist = false;
			char sysCmd[128] = "\0";

			IID_INIT(v4AddrIid);
			v4AddrIid.level = 2;
			v4AddrIid.idx[0] = ipIfaceIid.idx[0];
			v4AddrIid.idx[1] = 1;
			if(zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj) == ZCFG_SUCCESS){
				IPAddress = json_object_get_string(json_object_object_get(v4AddrObj,"IPAddress"));
				Status = json_object_get_string(json_object_object_get(v4AddrObj, "Status"));
				if(Status == NULL){
					Status = "";
				}
				if(IPAddress != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
					wanstatus = 1;
					if(strlen(wantype) == 0){
						strcpy(wantype,"ipv4");
					}
					else if(!strcmp(wantype,"ipv6")){
						wantype[0] = '\0';
						strcpy(wantype,"ipv4ipv6");
					}
					zcfgFeJsonObjFree(v4AddrObj);
					zcfgFeJsonObjFree(ipIfaceObj);
				}
				zcfgFeJsonObjFree(v4AddrObj);
			}

#ifdef OI_CUSTOMIZATION
			sprintf(sysCmd, "ifconfig | grep -A2 \"nas\\|ppp\" | grep \"inet6 addr\" | grep \"Scope:Global\" > /var/wan6ipcheck");
			system(sysCmd);
			FILE *fp = fopen("/var/wan6ipcheck", "r");
			if(fp) {
				char wan6ip[512] = {0};
				fread(wan6ip, 1, sizeof(wan6ip) - 1, fp);
				if(strchr(wan6ip, ':'))
					bIPv6Exist = true;
				fclose(fp);
			}

			if ( bIPv6Exist == false )
			{
				IID_INIT(v6AddrIid);
				while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj) == ZCFG_SUCCESS) {
					if ( v6AddrIid.idx[0] == ipIfaceIid.idx[0] ) {
						int bEnable = json_object_get_int(json_object_object_get(v6AddrObj, "Enable"));
						if ( bEnable ) {
							Jaddi(v6AddrObj,"Enable",false);
							zcfgFeObjJsonSet(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, v6AddrObj, NULL);
						}
					}

					zcfgFeJsonObjFree(v6AddrObj);
				}
			}
			else
#endif /* OI_CUSTOMIZATION */
			{
				IID_INIT(v6AddrIid);
				v6AddrIid.level = 2;
				v6AddrIid.idx[0] = ipIfaceIid.idx[0];
				v6AddrIid.idx[1] = 1;
				if(zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj) == ZCFG_SUCCESS){
					IPAddress = json_object_get_string(json_object_object_get(v6AddrObj,"IPAddress"));
					Status = json_object_get_string(json_object_object_get(v6AddrObj, "Status"));
					if(Status == NULL){
						Status = "";
					}
					if(IPAddress != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
						wanstatus = 1;
						if(strlen(wantype) == 0){
							strcpy(wantype,"ipv6");
						}
						else if(!strcmp(wantype,"ipv4")){
							wantype[0] = '\0';
							strcpy(wantype,"ipv4ipv6");
						}
						zcfgFeJsonObjFree(v6AddrObj);
						zcfgFeJsonObjFree(ipIfaceObj);
					}
					zcfgFeJsonObjFree(v6AddrObj);
				}
			}
		}
		
		zcfgFeJsonObjFree(ipIfaceObj);
	}



	networkMAPInfoJobj = json_object_new_object();
	json_object_array_add(Jarray, networkMAPInfoJobj);
	wanInfoJobj = json_object_new_object();
	json_object_object_add(networkMAPInfoJobj, "wanInfo", wanInfoJobj);
	Jaddi(wanInfoJobj,"wanStatus",wanstatus);
	Jaddi(wanInfoJobj,"wanIfaceExist",wanIfaceExist);
   	Jadds(wanInfoJobj,"wanType",wantype);
	
	hostsJarray = json_object_new_array();
	json_object_object_add(networkMAPInfoJobj, "lanhosts", hostsJarray);
	zcfgFeDalLanHostsGet(NULL, hostsJarray, NULL);


	IID_INIT(iid);
	if(zcfgFeObjJsonGet(RDM_OID_S_T_B_VENDOR_I_D, &iid, &stbJobj) == ZCFG_SUCCESS){
		json_object_object_add(networkMAPInfoJobj, "STBVendorID", stbJobj);
	}

	return ZCFG_SUCCESS;
}


zcfgRet_t zcfgFeDalNetworkMAP(const char *method, struct json_object *Jobj, struct json_object *Jarray,char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "GET")){
		ret = zcfgFeDalNetworkMAPGet(Jobj, Jarray, NULL);
	}
	else if(!strcmp(method, "PUT")){
		ret = zcfgFeDalNetworkMAPEdit(Jobj, NULL);
	}

	return ret;
}

zcfgRet_t zcfgFeDalIPoE(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg, const char *action)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	void *recvBuf = NULL;
	zcfgMsg_t *sendMsgHdr = NULL;
	objIndex_t v4ClientIid = {0};

	if (Jobj != NULL && json_object_object_get(Jobj,"index"))
	{
		char *index = NULL;
		char ipIfacePath[20] = {0};

		index = json_object_get_string(json_object_object_get(Jobj,"index"));
		strcpy(ipIfacePath, "IP.Interface.");
		if (index)
			strcat(ipIfacePath, index);
		if (!getSpecificObj(RDM_OID_DHCPV4_CLIENT, "Interface", json_type_string, ipIfacePath, &v4ClientIid, NULL))
		{
			return ZCFG_INTERNAL_ERROR;
		}
	}
	else
	{
		return ZCFG_INTERNAL_ERROR;
	}
	//create and send MSG
	sendMsgHdr = (zcfgMsg_t *)malloc(sizeof(zcfgMsg_t));
	if (sendMsgHdr == NULL)
	{
		return ZCFG_INTERNAL_ERROR;
	}
	if(strcmp(action,"Release") == 0){
		sendMsgHdr->type = ZCFG_MSG_WAN_IP_RELEASE;
	}
	else if(strcmp(action,"Renew") == 0){
		sendMsgHdr->type = ZCFG_MSG_WAN_IP_RENEW;
	}
	sendMsgHdr->oid = RDM_OID_DHCPV4_CLIENT;
	sendMsgHdr->length = 0;
	sendMsgHdr->srcEid = ZCFG_EID_DALCMD;
	sendMsgHdr->dstEid = ZCFG_EID_ESMD;
	memcpy(sendMsgHdr->objIid, &v4ClientIid, sizeof(objIndex_t));
	ret = zcfgMsgSendAndGetReply(sendMsgHdr, (zcfgMsg_t **)&recvBuf, 30000);
	if (ret == ZCFG_TIMEOUT)
		return ZCFG_SUCCESS;
	return ret;
}

zcfgRet_t zcfgFeDalPPPoE(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg, const char *action)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	void *recvBuf = NULL;
	zcfgMsg_t *sendMsgHdr = NULL;
	objIndex_t pppIid = {0};

	if (Jobj != NULL && json_object_object_get(Jobj,"wanpppIdx"))
	{
		int wanpppIdx = 0;
		wanpppIdx = json_object_get_int(json_object_object_get(Jobj,"wanpppIdx"));
		if (wanpppIdx)
		{
			pppIid.level = 1;
			pppIid.idx[0] = wanpppIdx;
		}
		else return ZCFG_INTERNAL_ERROR;
	}
	else
	{
		return ZCFG_INTERNAL_ERROR;
	}
	//create and send MSG
	sendMsgHdr = (zcfgMsg_t *)malloc(sizeof(zcfgMsg_t));
	if (sendMsgHdr == NULL)
	{
		return ZCFG_INTERNAL_ERROR;
	}
	if(strcmp(action,"Disconnect") == 0){
		sendMsgHdr->type = ZCFG_MSG_WAN_PPP_DISCONNECT;
	}
	else if(strcmp(action,"Connect") == 0){
		sendMsgHdr->type = ZCFG_MSG_WAN_PPP_CONNECT;
	}
	sendMsgHdr->oid = RDM_OID_PPP_IFACE;
	sendMsgHdr->length = 0;
	sendMsgHdr->srcEid = ZCFG_EID_DALCMD;
	sendMsgHdr->dstEid = ZCFG_EID_ESMD;
	memcpy(sendMsgHdr->objIid, &pppIid, sizeof(objIndex_t));
	ret = zcfgMsgSendAndGetReply(sendMsgHdr, (zcfgMsg_t **)&recvBuf, 30000);
	if (ret == ZCFG_TIMEOUT)
		return ZCFG_SUCCESS;
	return ret;
}

zcfgRet_t zcfgFeDalStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray,char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "GET")){
		ret = zcfgFeDalStatusGet(Jobj, Jarray, NULL);
	}
	else if(!strcmp(method, "PUT")){
		char *action = NULL;
		if (Jobj != NULL && json_object_object_get(Jobj,"action")) {
			action = json_object_get_string(json_object_object_get(Jobj,"action"));
			if(!strcmp(action, "Renew")||!strcmp(action, "Release")){
				ret = zcfgFeDalIPoE(Jobj, Jarray, replyMsg, action);
			}
			else if(!strcmp(action, "Connect")||!strcmp(action, "Disconnect")){
				ret = zcfgFeDalPPPoE(Jobj, Jarray, replyMsg, action);
			}
		}
	}

	return ret;
}

zcfgRet_t zcfgFeDalCardPageStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray,char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;
	if(!strcmp(method, "GET")){
		ret = zcfgFeDalCardPageStatusGet(Jobj, Jarray, NULL);
	}

	return ret;
}

zcfgRet_t zcfgFeDalLanPortInfo(const char *method, struct json_object *Jobj, struct json_object *Jarray,char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "GET")){
		ret = zcfgFeDalLanPortInfoGet(Jobj, Jarray, NULL);
	}

	return ret;
}



