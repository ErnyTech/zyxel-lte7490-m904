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

dal_param_t LANSETUP_param[]={
	{"Name", dalType_string,	1,	128,	NULL, NULL, dal_Edit},
	//LAN IP
	{"IPAddress", dalType_v4Addr,	0,	0,	NULL},
	{"SubnetMask", dalType_v4Mask,	0,	0,	NULL},
	//IP Addressing Values
	{"DHCP_MinAddress", dalType_v4Addr,	0,	0,	NULL},
	{"DHCP_MaxAddress", dalType_v4Addr,	0,	0,	NULL},
	{"DHCP_AutoReserveLanIp", dalType_boolean,	0,	0,	NULL},
	{"DHCP_RelayAddr", dalType_v4Addr,	0,	0,	NULL},
	//DHCP Server State
	{"EnableDHCP", dalType_boolean,	0,	0,	NULL},
	{"DHCPType", dalType_string,	0,	0,	NULL, "DHCPServer|DHCPRelay"},
	//DHCP Server Lease Time
	{"DHCP_LeaseTime", dalType_int,	0,	0,	NULL},
	//DNS Values
	{"DNS_Type", dalType_string,	0,	0,	NULL, "DNSProxy|FromISP|Static"},
	{"DNS_Servers1", dalType_v4Addr,	0,	0,	NULL},
	{"DNS_Servers2", dalType_v4Addr,	0,	0,	NULL},
	//LAN IPv6 Mode Setup
	{"IPv6_LanEnable", dalType_boolean,	0,	0,	NULL},
	//Link Local Address Type
	{"IPv6_LinkLocalAddressType", dalType_string,	0,	0,	NULL, "EUI64|Manual"},
	{"IPv6_LinkLocalAddress", dalType_v6Addr,	0,	0,	NULL},
	//Lan Global Identifier Type
	{"IPv6_IdentifierType", 	dalType_string,	0,	0,	NULL, "EUI64|Manual"},
	{"IPv6_Identifier", dalType_string,	0,	0,	NULL},
	//LAN IPv6 Prefix Setup
	{"IPv6_PrefixType", dalType_string,	0,	0,	NULL, "Static|DelegateFromWan"},
	{"IPv6_PrefixWan",			dalType_RtWan,	0,	0,	NULL, "Default"},
	{"IPv6_Prefix", 			dalType_v6AddrPrefix,	0,	0,	NULL},
	//LAN IPv6 Address Assign Setup
	{"IPv6_LanAddrAssign", dalType_string,	0,	0,	NULL, "Stateless|Stateful"},
	//LAN IPv6 DNS Assign Setup
	{"IPv6_DNSAssign", dalType_string,	0,	0,	NULL, "RA|DHCP|RA_DHCP"},
	//IPv6 Address Values
	{"Ipv6_MinAddress", dalType_string,	0,	0,	NULL},
	{"Ipv6_MaxAddress", dalType_string,	0,	0,	NULL},
	{"Ipv6_DNSSuffix", dalType_string,	0,	0,	NULL},
//{"Ipv6Interface", dalType_string,	0,	0,	NULL},
	//IPv6 DNS Values
	{"Ipv6_DNSServer1", dalType_v6Addr,	0,	0,	NULL, "None|ISP|Proxy"},
	{"Ipv6_DNSServer2", dalType_v6Addr,	0,	0,	NULL, "None|ISP|Proxy"},
	{"Ipv6_DNSServer3", dalType_v6Addr,	0,	0,	NULL, "None|ISP|Proxy"},
	//DNS Query Scenario
	{"DNSQueryScenario", dalType_string,	0,	0,	NULL, "IPv4IPv6|IPv6Only|IPv4Only|IPv6First|IPv4First"},
	//Other
	{"IGMPSnoopingMode", dalType_string,	0,	0,	NULL, "Disabled|Standard|Blocking",	dalcmd_Forbid},
	{"MLDSnoopingMode", dalType_string,	0,	0,	NULL, "Disabled|Standard|Blocking",	dalcmd_Forbid},
#ifdef MTKSOC_DHCP6S_IAPD_SUPPORT
	//LAN DHCPv6 Server IAPD Setup
	{"X_ZYXEL_IAPDEnable", dalType_boolean,	0,	0,	NULL},
	{"X_ZYXEL_ClientDUID", dalType_string,	0,	0,	NULL},
	{"X_ZYXEL_IAPDPrefixes", dalType_string,	0,	0,	NULL},
	{"X_ZYXEL_IAPDIpAddress", dalType_string,	0,	0,	NULL},
#endif
	{NULL,					0,	0,	0,	NULL},
};

//GlobalLanObjs
static struct json_object *allIpIfaceObj = NULL;
static struct json_object *curIpIfaceObj = NULL;
static char ifacePath[32] = {0};
static char manualPrefixes[32] = {0};
static const char *selbrName = NULL;

static objIndex_t allIpIfaceIid = {0};
static objIndex_t curIpIfaceIid = {0};


#ifdef DHCPV6_RELAY_SUPPORT
extern zcfgRet_t zcfgFeDal_isDhcpv6RelayEnable(char *ipIfPath);
#endif


void initGlobalLanObjs(){
	curIpIfaceObj = NULL;
	allIpIfaceObj = NULL;
	IID_INIT(curIpIfaceIid);
	IID_INIT(allIpIfaceIid);
}

void freeAllLanObjs(){
	zcfgFeJsonObjFree(allIpIfaceObj);
	//zcfgFeJsonObjFree(curIpIfaceObj); //curIpIfaceObj is a part of allIpIfaceObj, cant free it.
}

/*
 * Get Ipv6_DNSServer1 Ipv6_DNSServer2 Ipv6_DNSServer3 and curDnsList, curDnsServer
 * to construct dnsList and dnsServer
 *
 * The posiable of Ipv6_DNSServer is "ISP", "None", or IPv6 address. If it is IPv6 address, imply its Dns type is 'User Defined'.
 * e.g. curDnsList="None,User,User"   curDnsServer=",2001:123::1,2001:123::2"
 *        Ipv6_DNSServer1="2002:111::1"  Ipv6_DNSServer2=NULL  Ipv6_DNSServer3="ISP"
 *
 *      After convert, dnsList = "User,User,ISP"  dnsServer="2002:111::1,2001:123::1,"
*/
void convertIPv6DnsList(struct json_object *Jobj){
	struct json_object *radvdObj = NULL;
	objIndex_t iid = {0};
	const char *inface = NULL;
	bool found = false;
	char dnsList[32] = {0}, dnsServer[256] = {0};
	char  curDnsList[32] = {0}, curDnsServer[256] = {0};
	const char *DNSList1 = NULL, *DNSList2 = NULL, *DNSList3 = NULL;
	const char *DNSServer1 = NULL, *DNSServer2 = NULL, *DNSServer3 = NULL;
	const char *curDnsList1 = NULL, *curDnsList2 = NULL, *curDnsList3 = NULL;
	const char *curDNSServer1 = NULL, *curDNSServer2 = NULL, *curDNSServer3 = NULL;
	const char *Ipv6_DNSServer1 = NULL, *Ipv6_DNSServer2 = NULL, *Ipv6_DNSServer3 = NULL;
	char *tmp_ptr = NULL;
	
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_RT_ADV_INTF_SET, &iid, &radvdObj) == ZCFG_SUCCESS) {
		inface = json_object_get_string(json_object_object_get(radvdObj, "Interface"));
		if(!strcmp(inface, ifacePath)){
			found = true;
			strcpy(curDnsList, json_object_get_string(json_object_object_get(radvdObj, "X_ZYXEL_DNSList")));
			strcpy(curDnsServer, json_object_get_string(json_object_object_get(radvdObj, "X_ZYXEL_DNSServers")));
			zcfgFeJsonObjFree(radvdObj);
			break;
		}
		zcfgFeJsonObjFree(radvdObj);
	}

	if(found){
		curDnsList1 = strtok_r( curDnsList, ",", &tmp_ptr);
		curDnsList2 = strtok_r( NULL, ",", &tmp_ptr);
		curDnsList3 = strtok_r( NULL, ",", &tmp_ptr);

		curDNSServer1 = strtok_r( curDnsServer, ",", &tmp_ptr);
		curDNSServer2 = strtok_r( NULL, ",", &tmp_ptr);
		curDNSServer3 = strtok_r( NULL, ",", &tmp_ptr);
	}
	
	if(curDnsList1 == NULL) curDnsList1 = "FromISP";
	if(curDnsList2 == NULL) curDnsList3 = "FromISP";
	if(curDnsList3 == NULL) curDnsList3 = "FromISP";
	if(curDNSServer1 == NULL) curDNSServer1 = "";
	if(curDNSServer2 == NULL) curDNSServer2 = "";
	if(curDNSServer3 == NULL) curDNSServer3 = "";

	Ipv6_DNSServer1 = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSServer1"));
	Ipv6_DNSServer2 = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSServer2"));
	Ipv6_DNSServer3 = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSServer3"));

	DNSList1 = DNSList2 = DNSList3 = "User";
	DNSServer1 = DNSServer2 = DNSServer3 = "";
	if(Ipv6_DNSServer1 == NULL){
		DNSList1 = curDnsList1;
		DNSServer1 = curDNSServer1;
	}
	else{
		if(!strcmp(Ipv6_DNSServer1, "None") || !strcmp(Ipv6_DNSServer1, "ISP") || !strcmp(Ipv6_DNSServer1, "Proxy"))
			DNSList1 = Ipv6_DNSServer1;
		else
			DNSServer1 = Ipv6_DNSServer1;	
	}
	if(Ipv6_DNSServer2 == NULL){
		DNSList2 = curDnsList2;
		DNSServer2 = curDNSServer2;
	}
	else{
		if(!strcmp(Ipv6_DNSServer2, "None") || !strcmp(Ipv6_DNSServer2, "ISP") || !strcmp(Ipv6_DNSServer2, "Proxy"))
			DNSList2 = Ipv6_DNSServer2;
		else
			DNSServer2 = Ipv6_DNSServer2;	
	}
	if(Ipv6_DNSServer3 == NULL){
		DNSList3 = curDnsList3;
		DNSServer3 = curDNSServer3;
	}
	else{
		if(!strcmp(Ipv6_DNSServer3, "None") || !strcmp(Ipv6_DNSServer3, "ISP") || !strcmp(Ipv6_DNSServer3, "Proxy"))
			DNSList3 = Ipv6_DNSServer3;
		else
			DNSServer3 = Ipv6_DNSServer3;	
	}
	
	sprintf(dnsList, "%s,%s,%s", DNSList1, DNSList2, DNSList3);
	sprintf(dnsServer, "%s,%s,%s", DNSServer1, DNSServer2, DNSServer3);

	json_object_object_add(Jobj, "Ipv6_DNSList", json_object_new_string(dnsList));
	json_object_object_add(Jobj, "Ipv6_DNSServers", json_object_new_string(dnsServer));

	return;
}

bool isLowerLayerUSBorEmpty(struct json_object *subIpIfaceObj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	static struct json_object *pppObj = NULL;
	static struct json_object *ethlinkObj = NULL;
	objIndex_t pppIid = {0};
	objIndex_t ethlinkIid = {0};
	uint8_t index = 0;
	const char *lowLayer = NULL;
	char ipIntLowLayer[32] = {0};

	IID_INIT(pppIid);
	strcpy(ipIntLowLayer, json_object_get_string(json_object_object_get(subIpIfaceObj, "LowerLayers")));

	if(!strncmp(ipIntLowLayer, "PPP.Interface.", 14)){
		sscanf(ipIntLowLayer, "PPP.Interface.%hhu", &index);
		pppIid.idx[0] = index;
		if((ret = zcfgFeObjJsonGet(RDM_OID_PPP_IFACE, &pppIid, &pppObj)) == ZCFG_SUCCESS){
			lowLayer = json_object_get_string(json_object_object_get(pppObj, "LowerLayers"));
			if((strlen(lowLayer) == 0) ||(!strncmp(lowLayer, "USB.", 4))){
				zcfgFeJsonObjFree(pppObj);
				return true;
			}
		}

	}else if(!strncmp(ipIntLowLayer, "Ethernet.Link.", 14)){
		sscanf(ipIntLowLayer, "Ethernet.Link.%hhu", &index);
		ethlinkIid.idx[0] = index;
		if((ret = zcfgFeObjJsonGet(RDM_OID_ETH_LINK, &ethlinkIid, &ethlinkObj)) == ZCFG_SUCCESS){
			lowLayer = json_object_get_string(json_object_object_get(ethlinkObj, "LowerLayers"));
			if((strlen(lowLayer) == 0) ||(!strncmp(lowLayer, "USB.", 4))){
				zcfgFeJsonObjFree(ethlinkObj);
				return true;
			}
		}
	}
	return false;
}

bool isNatEnable(){
	struct json_object *natObj = NULL;
	objIndex_t natIid = {0};
	char natIf[32] = {0};
	const char *groupWANIf = NULL;
	bool enable = false;
	uint8_t tmpIndex = 0;
	bool nat = false;
	int i = 0;

	IID_INIT(natIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_NAT_INTF_SETTING, &natIid, &natObj) == ZCFG_SUCCESS) {
		enable = json_object_get_boolean(json_object_object_get(natObj, "Enable"));
		strcpy(natIf, json_object_get_string(json_object_object_get(natObj, "Interface")));
		if(enable == true && strlen(natIf) > 0){
			sscanf(natIf, "IP.Interface.%hhu", &tmpIndex);
			for(i = 0; i < json_object_array_length(allIpIfaceObj); i++) {
				struct json_object *subIpIfaceObj = json_object_array_get_idx(allIpIfaceObj, i);
				if(tmpIndex == (i+1)){
					groupWANIf = json_object_get_string(json_object_object_get(curIpIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
					if(!isLowerLayerUSBorEmpty(subIpIfaceObj) || strstr(natIf,groupWANIf)){					
						nat = true;
						zcfgFeJsonObjFree(natObj);
						break;
					}
				}
			}
			if(nat == true){
				zcfgFeJsonObjFree(natObj);
				break;
			}
		}
	zcfgFeJsonObjFree(natObj);
	}
	return nat;
}

zcfgRet_t setNat(){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *natObj = NULL;
	objIndex_t natIid = {0};
	const char *natIf = NULL;
	const char *groupWANIf = NULL, *curGroupWANIf = NULL;
	const char *ifName = NULL;
	char *ipInt = NULL, *single_ipInt = NULL, *rest_str = NULL;
	bool enable = false;
	int i = 0;

	IID_INIT(natIid);
	if(isNatEnable()){
		while(zcfgFeObjJsonGetNext(RDM_OID_NAT_INTF_SETTING, &natIid, &natObj) == ZCFG_SUCCESS) {
			natIf = json_object_get_string(json_object_object_get(natObj, "Interface"));
			enable = json_object_get_boolean(json_object_object_get(natObj, "Enable"));
			if(enable == true && strlen(natIf) > 0){
				bool group = false;
				i = 0;
				for(i = 0; i < json_object_array_length(allIpIfaceObj); i++) {
					struct json_object *subIpIfaceObj = json_object_array_get_idx(allIpIfaceObj, i);
					ifName = json_object_get_string(json_object_object_get(subIpIfaceObj, "X_ZYXEL_IfName"));
					curGroupWANIf = json_object_get_string(json_object_object_get(curIpIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
					if(!strncmp(ifName, "br", 2)){
						groupWANIf = json_object_get_string(json_object_object_get(subIpIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
						ipInt = (char*)malloc(strlen(groupWANIf)+1);
						memset(ipInt, 0, strlen(groupWANIf)+1);
						memcpy(ipInt, groupWANIf, strlen(groupWANIf)+1);
						single_ipInt = strtok_r( ipInt, ",", &rest_str);
						while(single_ipInt != NULL){
							if(!strcmp(natIf, single_ipInt)){
								if(!strcmp(groupWANIf, curGroupWANIf)){
									json_object_object_add(natObj, "Enable", json_object_new_boolean(false));
									json_object_object_add(natObj, "X_ZYXEL_FullConeEnabled", json_object_new_boolean(false));
									ret = zcfgFeObjJsonSet(RDM_OID_NAT_INTF_SETTING, &natIid, natObj, NULL);
								}
								group = true;
							}
							single_ipInt = strtok_r(rest_str, ",", &rest_str);
						}
						free(ipInt);
					}
				}//for

				if(group == false && !strcmp(curGroupWANIf, "")){
					json_object_object_add(natObj, "Enable", json_object_new_boolean(false));
					json_object_object_add(natObj, "X_ZYXEL_FullConeEnabled", json_object_new_boolean(false));
					ret = zcfgFeObjJsonSet(RDM_OID_NAT_INTF_SETTING, &natIid, natObj, NULL);
					group = true;
				}
			}	
			zcfgFeJsonObjFree(natObj);
		}//while
			
	}//if
	return ret;
}

zcfgRet_t setIgmpSnooping(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *igmpSnoopingObj = NULL;
	struct json_object *obj = NULL;
	const char *brtmpname = NULL;
	objIndex_t igmpSnoopingIid = {0};
	char *igmpSnooping = NULL;
	int brsnoopingenable = 0;
	const char *IGMPSnoopingMode = NULL;
	const char *brIfName = NULL;
	char *tmp = NULL;
	char newbrtmp[6] = {0};
	char igmpBrSnooping[100] = {0};
	char buff[100] = {0};
	char *brname = NULL;
	char newbrstr[100] = {0};
	objIndex_t iid = {0};
	int len = 0;

	IGMPSnoopingMode = json_object_get_string(json_object_object_get(Jobj, "IGMPSnoopingMode"));
	if(IGMPSnoopingMode == NULL)
		return ret;

	if(!strcmp(IGMPSnoopingMode, "Disabled"))
		brsnoopingenable = 0;
	else if(!strcmp(IGMPSnoopingMode, "Standard"))
		brsnoopingenable = 1;
	else //if(!strcmp(IGMPSnoopingMode, "Blocking"))
		brsnoopingenable = 2;
	

	IID_INIT(iid);
	brtmpname = json_object_get_string(json_object_object_get(Jobj, "Name"));
	if(!strcmp(brtmpname, "Default")){
		brIfName = "br0";
	}
	else{
		while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &iid, &obj) == ZCFG_SUCCESS) {
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "X_ZYXEL_SrvName")),brtmpname)){
				brIfName = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_IfName"));
				if(!strncmp(brIfName, "br", 2)) break;
			}
			zcfgFeJsonObjFree(obj);
		}		
	}
	IID_INIT(igmpSnoopingIid);
	if(zcfgFeObjJsonGet(RDM_OID_ZY_IGMP, &igmpSnoopingIid, &igmpSnoopingObj) == ZCFG_SUCCESS){
		igmpSnooping = json_object_get_string(json_object_object_get(igmpSnoopingObj, "SnoopingBridgeIfName"));	
		if(igmpSnooping != NULL){
			strcpy(buff, igmpSnooping);
			brname = strtok_r(buff, ",", &tmp);
			while(brname != NULL){
				if(strncmp(brIfName,brname,3)){
					strcat(newbrstr, brname);
					strcat(newbrstr, ",");
				}
				else{
					sprintf(newbrtmp,"%s|%d",brIfName,brsnoopingenable);
					strcat(newbrstr, newbrtmp);
					strcat(newbrstr, ",");
				}
				brname = strtok_r(tmp, ",", &tmp);
			}
			len = strlen(newbrstr);
			if(newbrstr[len-1] == ',')
				newbrstr[len-1] = '\0';
		}
		else
			sprintf(newbrstr,"%s|%d", brIfName, brsnoopingenable);

		json_object_object_add(igmpSnoopingObj, "SnoopingBridgeIfName", json_object_new_string(newbrstr));
		zcfgFeObjJsonSet(RDM_OID_ZY_IGMP, &igmpSnoopingIid, igmpSnoopingObj, NULL);
		zcfgFeJsonObjFree(igmpSnoopingObj);
	}
	zcfgFeJsonObjFree(obj);
	return ret;	
}



zcfgRet_t setmldSnooping(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *mldSnoopingObj = NULL;
	struct json_object *obj = NULL;
	objIndex_t mldSnoopingIid = {0};
	char *mldSnooping = NULL;
	int brsnoopingenable = 0;
    const char *MLDSnoopingMode = NULL;
	const char *brtmpname = NULL;
	const char *brIfName = NULL;
	char buff[100] = {0};
	char *brname = NULL;
	char *tmp = NULL;
	char newbrstr[100] = {0};
	char newbrtmp[6] = {0};
	char mldBrSnooping[100] = {0};
	objIndex_t iid = {0};
	int len = 0;

    MLDSnoopingMode = json_object_get_string(json_object_object_get(Jobj, "MLDSnoopingMode"));
    if(MLDSnoopingMode == NULL)
        return ret;
    
    if(!strcmp(MLDSnoopingMode, "Disabled"))
        brsnoopingenable = 0;
    else if(!strcmp(MLDSnoopingMode, "Standard"))
        brsnoopingenable = 1;
    else //if(!strcmp(MLDSnoopingMode, "Blocking"))
        brsnoopingenable = 2;

	IID_INIT(iid);
	brtmpname = json_object_get_string(json_object_object_get(Jobj, "Name"));
	if(!strcmp(brtmpname, "Default")){
		brIfName = "br0";
	}
	else{
		while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &iid, &obj) == ZCFG_SUCCESS) {
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "X_ZYXEL_SrvName")),brtmpname)){
				brIfName = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_IfName"));
				break;
			}
			zcfgFeJsonObjFree(obj);
		}		
	}
	IID_INIT(mldSnoopingIid);
	if(zcfgFeObjJsonGet(RDM_OID_ZY_MLD, &mldSnoopingIid, &mldSnoopingObj) == ZCFG_SUCCESS){
		mldSnooping = json_object_get_string(json_object_object_get(mldSnoopingObj, "SnoopingBridgeIfName"));	
		if(mldSnooping != NULL){
			strcpy(buff, mldSnooping);
			brname = strtok_r(buff, ",", &tmp);
			while(brname != NULL){
				if(strncmp(brIfName,brname,3)){
					strcat(newbrstr, brname);
					strcat(newbrstr, ",");
			}
			else{
					sprintf(newbrtmp, "%s|%d", brIfName, brsnoopingenable);
					strcat(newbrstr, newbrtmp);
					strcat(newbrstr, ",");
				}
				brname = strtok_r(tmp, ",", &tmp);
			}
			len = strlen(newbrstr);
			if(newbrstr[len-1] == ',')
				newbrstr[len-1] = '\0';
		}
		else
			sprintf(mldBrSnooping, "%s|%d", brIfName, brsnoopingenable);
		json_object_object_add(mldSnoopingObj, "SnoopingBridgeIfName", json_object_new_string(newbrstr));
		zcfgFeObjJsonSet(RDM_OID_ZY_MLD, &mldSnoopingIid, mldSnoopingObj, NULL);
		zcfgFeJsonObjFree(mldSnoopingObj);
	}
	zcfgFeJsonObjFree(obj);
	return ret;	
}

#ifdef ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY
zcfgRet_t checkPassword(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
 	struct json_object *accountObj = NULL;
	objIndex_t accountIid = {0};
	char loginUserName[32] = {0};	
	char loginLevel[16] = {0};
	char *curPassword = NULL;
	int authResult = -1;
	

	if(getLoginUserInfo(Jobj, loginUserName, loginLevel) != ZCFG_SUCCESS){
		printf("%s() getLoginUserInfo Fail", __FUNCTION__);
		return ZCFG_INTERNAL_ERROR;
	}
	
	IID_INIT(accountIid);
	if(!getSpecificObj(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, "Username", json_type_string, loginUserName, &accountIid, &accountObj)){
		if(replyMsg) strcpy(replyMsg, "Request reject");
		return ZCFG_REQUEST_REJECT;
	}
	
	//if(ISSET("Password")){
		if(!strcmp("", Jgets(Jobj ,"oldPw"))){
			curPassword = Jgets(accountObj, "Password");
			if(strcmp(curPassword, Jgets(Jobj ,"oldPw"))!= 0){
				if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
				json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.oldpassword_err2"));
					zcfgFeJsonObjFree(accountObj);
				return ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
			}
		}
		else{
			if(!ISSET("oldPw")){
				if(replyMsg) strcpy(replyMsg, "Need --oldPw and --verifyNewpassword to modify Password");
				zcfgFeJsonObjFree(accountObj);
				return ZCFG_DELETE_REJECT;
			}
			
			authResult = zyUtilAuthUserAccount(loginUserName, Jgets(Jobj ,"oldPw"));
			if (authResult == 1) 
			{
				curPassword = Jgets(accountObj, "Password");
				if(strcmp(curPassword, Jgets(Jobj ,"oldPw"))){
					if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
					json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.oldpassword_err2"));
					zcfgFeJsonObjFree(accountObj);
					return ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
				}
			}
			else if (authResult < 0)
			{
				if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
				json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.oldpassword_err2"));
				zcfgFeJsonObjFree(accountObj);
				return ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
			}
			//replaceParam(accountObj, "Password", Jobj, "Password");
		}
	//}
	
done:
	zcfgFeJsonObjFree(accountObj);
	return ret;
}
#endif//ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY


zcfgRet_t setLanIP(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
 	struct json_object *v4AddrObj = NULL;
	const char *IPAddress = NULL, *CurIPAddress = NULL;
	const char *SubnetMask = NULL, *CurSubnetMask = NULL;
	objIndex_t v4AddrIid = {0};

	IID_INIT(v4AddrIid);
	v4AddrIid.idx[0] = curIpIfaceIid.idx[0];
	v4AddrIid.idx[1] = 1;
	v4AddrIid.level = 2;

	ret = zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj);

	if(ret != ZCFG_SUCCESS)
	{
		return ret;
	}

	CurIPAddress = json_object_get_string(json_object_object_get(v4AddrObj, "IPAddress"));
	CurSubnetMask = json_object_get_string(json_object_object_get(v4AddrObj, "SubnetMask"));

	IPAddress = json_object_get_string(json_object_object_get(Jobj, "IPAddress"));
	SubnetMask = json_object_get_string(json_object_object_get(Jobj, "SubnetMask"));

	if (IPAddress == NULL)
	{
		IPAddress = CurIPAddress;
	}
	if (SubnetMask == NULL)
	{
		SubnetMask = CurSubnetMask;
	}

	if (!strcmp(IPAddress,CurIPAddress) && !strcmp(SubnetMask,CurSubnetMask))
	{
		goto done;
	}

	json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(true));
	json_object_object_add(v4AddrObj, "IPAddress", json_object_new_string(IPAddress));
	json_object_object_add(v4AddrObj, "SubnetMask", json_object_new_string(SubnetMask));
	zcfgFeObjJsonSet(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, v4AddrObj, NULL);

done:
	zcfgFeJsonObjFree(v4AddrObj);
	return ret;
}

zcfgRet_t setDHCPState(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *dhcpSrvObj = NULL;
	struct json_object *dhcpRelayObj = NULL;
	struct json_object *dhcpServerObj = NULL; // RDM_OID_DHCPV4_SRV
	objIndex_t dhcpSrvIid = {0};
	objIndex_t dhcpRelayIid = {0};
	objIndex_t dhcpServerIid = {0}; // RDM_OID_DHCPV4_SRV
	const char *dhcpIfacePath = NULL, *dhcpRelayPath = NULL;
	const char *dhcpRelayAddr = NULL, *curdhcpRelayAddr = NULL;
	const char *dhcpMinAdr = NULL, *curDhcpMinAdr = NULL;
	const char *dhcpMaxAdr = NULL, *curDhcpMaxAdr = NULL;
	const char *dnsServer1 = NULL, *dnsServer2 = NULL, *curDnsServer = NULL;
	const char *dnsType = NULL, *curDnsType = NULL;
	bool enableDhcp = false, curEnableDhcp = false;
	bool curRelayDhcp = false;
	bool dhcpAutoReserve = false, curDhcpAutoReserve = false;
	int leaseTime = 0, curLeaseTime;
	bool setParam = false;
	const char *DHCPType = NULL;
	bool dhcpServerEnable = false;
	char dnsServer[64] = {0};
	bool checkDhcp = false;
	// RDM_OID_DHCPV4_SRV
	IID_INIT(dhcpServerIid);
	if(zcfgFeObjJsonGet(RDM_OID_DHCPV4_SRV, &dhcpServerIid, &dhcpServerObj) == ZCFG_SUCCESS){
		dhcpServerEnable = json_object_get_boolean(json_object_object_get(dhcpServerObj, "Enable"));
	}

	// RDM_OID_DHCPV4_SRV_POOL
	IID_INIT(dhcpSrvIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_SRV_POOL, &dhcpSrvIid, &dhcpSrvObj) == ZCFG_SUCCESS) {
		dhcpIfacePath = json_object_get_string(json_object_object_get(dhcpSrvObj, "Interface"));
		if(!strcmp(ifacePath, dhcpIfacePath)){
			curEnableDhcp = json_object_get_boolean(json_object_object_get(dhcpSrvObj, "Enable"));
			curDhcpMinAdr = json_object_get_string(json_object_object_get(dhcpSrvObj, "MinAddress"));
			curDhcpMaxAdr = json_object_get_string(json_object_object_get(dhcpSrvObj, "MaxAddress"));
			curDnsServer = json_object_get_string(json_object_object_get(dhcpSrvObj, "DNSServers"));
			curDnsType = json_object_get_string(json_object_object_get(dhcpSrvObj, "X_ZYXEL_DNS_Type"));
			curLeaseTime = json_object_get_int(json_object_object_get(dhcpSrvObj, "LeaseTime"));
			curDhcpAutoReserve = json_object_get_boolean(json_object_object_get(dhcpSrvObj, "X_ZYXEL_AutoReserveLanIp"));
			break;
		}
		zcfgFeJsonObjFree(dhcpSrvObj);
	}
	if(json_object_object_get(Jobj, "EnableDHCP") != NULL){
		enableDhcp = json_object_get_boolean(json_object_object_get(Jobj, "EnableDHCP"));
	}
	else
		enableDhcp = curEnableDhcp;

#ifdef ZYXEL_WEB_GUI_ENABLE_DHCP_SRV // add by amet to edit DHCPv4 server
	if(json_object_object_get(Jobj, "Check") != NULL){
		checkDhcp = json_object_get_boolean(json_object_object_get(Jobj, "Check"));
		printf("Check from GUI is %d, enable = %d\n", checkDhcp,enableDhcp);
		if(true == checkDhcp){
			json_object_object_add(dhcpServerObj, "Enable", json_object_new_boolean(enableDhcp));
			zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_SRV, &dhcpServerIid, dhcpServerObj, NULL);
			sleep(3);
			zcfgFeJsonObjFree(dhcpServerObj);
			zcfgFeJsonObjFree(dhcpSrvObj);
			return ret;
		}
	}
#endif
	IID_INIT(dhcpRelayIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, &dhcpRelayObj) == ZCFG_SUCCESS) {
		dhcpRelayPath = json_object_get_string(json_object_object_get(dhcpRelayObj, "Interface"));
		if(!strcmp(ifacePath, dhcpRelayPath)){
			curRelayDhcp = json_object_get_boolean(json_object_object_get(dhcpRelayObj, "Enable"));
			curdhcpRelayAddr = json_object_get_string(json_object_object_get(dhcpRelayObj, "DHCPServerIPAddress"));
			break;
		}
		zcfgFeJsonObjFree(dhcpRelayObj);
	}

	DHCPType = json_object_get_string(json_object_object_get(Jobj, "DHCPType"));
	if(DHCPType == NULL){
		if(enableDhcp){
			printf("\nInvalid parameter: DHCPType\n");
			return ZCFG_INVALID_PARAM_VALUE;
		}
		else{
			if(curRelayDhcp)
				DHCPType = "DHCPRelay";
			else
				DHCPType = "DHCPServer";
		}
	}

	if(enableDhcp && !strcmp(DHCPType, "DHCPRelay")){
		dhcpRelayAddr = json_object_get_string(json_object_object_get(Jobj, "DHCP_RelayAddr"));
		if(dhcpRelayAddr == NULL){
			if(curdhcpRelayAddr == NULL || curdhcpRelayAddr[0] == '\0')
				printf("\nInvalid parameter: DHCP_RelayAddr\n");
				return ZCFG_INVALID_PARAM_VALUE;
		}

		setNat();
		
		//original enable dhcp, now disable dhcp server but not delete object
		if(curEnableDhcp){
			json_object_object_add(dhcpSrvObj, "Enable", json_object_new_boolean(false));
			zcfgFeObjJsonSet(RDM_OID_DHCPV4_SRV_POOL, &dhcpSrvIid, dhcpSrvObj, NULL);
		}

		//orignial enable dhcp relay, set dhcp relay
		if(curRelayDhcp){
			if(strcmp(dhcpRelayAddr, curdhcpRelayAddr)){
				setParam = true;
			}
		}else{//orignial disable dhcp relay, add dhcp relay, POST
			IID_INIT(dhcpRelayIid);
			zcfgFeJsonObjFree(dhcpRelayObj);
			ret = zcfgFeObjJsonAdd(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, NULL);
			if(ret == ZCFG_SUCCESS){
				zcfgFeObjJsonGet(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, &dhcpRelayObj);
				json_object_object_add(dhcpRelayObj, "Interface", json_object_new_string(ifacePath));
				setParam = true;
			}
		}
		if(setParam){
			json_object_object_add(dhcpRelayObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(dhcpRelayObj, "DHCPServerIPAddress", json_object_new_string(dhcpRelayAddr));
			zcfgFeObjJsonSet(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, dhcpRelayObj, NULL);
		}
	}
	else if(enableDhcp && !strcmp(DHCPType, "DHCPServer")){
		dhcpMinAdr = json_object_get_string(json_object_object_get(Jobj, "DHCP_MinAddress"));
		dhcpMaxAdr = json_object_get_string(json_object_object_get(Jobj, "DHCP_MaxAddress"));
		dnsServer1 = json_object_get_string(json_object_object_get(Jobj, "DNS_Servers1"));
		dnsServer2 = json_object_get_string(json_object_object_get(Jobj, "DNS_Servers2"));
		dnsType = json_object_get_string(json_object_object_get(Jobj, "DNS_Type"));
		leaseTime = json_object_get_int(json_object_object_get(Jobj, "DHCP_LeaseTime"));
		dhcpAutoReserve = json_object_get_boolean(json_object_object_get(Jobj, "DHCP_AutoReserveLanIp"));
		
		//original enable dhcp relay, disable dhcp relay server
		if(curRelayDhcp){
			ret = zcfgFeObjJsonDel(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, NULL);
			if(dnsType == NULL){
				if(!strcmp(curDnsType, "From ISP"))
					dnsType = "FromISP";
				else if(!strcmp(curDnsType, "DNS Proxy"))
					dnsType = "DNSProxy";
				else //if(!strcmp(curDnsType, "Static"))
					dnsType = "Static";
			}
			if(dnsServer1 == NULL)
				strcat(dnsServer,curDnsServer);
		}
		else{
			if(dhcpMinAdr == NULL || dhcpMaxAdr == NULL || leaseTime == 0 || dnsType == NULL ){
				printf("\nInvalid parameters.\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
		}
		if(!strcmp(dnsType,"Static")){
			if(dnsServer1 == NULL){
				printf("\nInvalid parameter: DNS_Servers1\n");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			strcat(dnsServer, dnsServer1);
			if(json_object_object_get(Jobj, "DNS_Servers2")){
				strcat(dnsServer, ",");
				strcat(dnsServer, dnsServer2);
			}
		}

		//original enable dhcp, set dhcp server pool object
		if(curEnableDhcp){
			if((strcmp(dhcpMinAdr, curDhcpMinAdr)) || (strcmp(dhcpMaxAdr, curDhcpMaxAdr)) ||
				(leaseTime != curLeaseTime) || (strcmp(dnsType, curDnsType)) ||
				(dnsServer != NULL && strcmp(dnsServer, curDnsServer)) ||
				(dhcpAutoReserve != curDhcpAutoReserve)){
				setParam = true;
			}
		}
		else{//original disable dhcp, add dhcp server pool object
			setParam = true;
		}
// marked by amet to avoid always set DHCPv4 server enable
#ifndef ZYXEL_WEB_GUI_ENABLE_DHCP_SRV
		// 20170725 Max Add
		if(dhcpServerEnable == false){
			json_object_object_add(dhcpServerObj, "Enable", json_object_new_boolean(true));
			zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_SRV, &dhcpServerIid, dhcpServerObj, NULL);
			sleep(3); // make sure RDM_OID_DHCPV4_SRV already set
		}
#endif
		if(setParam){
			fflush(stdout);
			if(!strcmp(dnsType, "DNSProxy"))
				json_object_object_add(dhcpSrvObj, "X_ZYXEL_DNS_Type", json_object_new_string("DNS Proxy"));
			else if(!strcmp(dnsType, "FromISP"))
				json_object_object_add(dhcpSrvObj, "X_ZYXEL_DNS_Type", json_object_new_string("From ISP"));
			else 
				json_object_object_add(dhcpSrvObj, "X_ZYXEL_DNS_Type", json_object_new_string(dnsType));	
			json_object_object_add(dhcpSrvObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(dhcpSrvObj, "MinAddress", json_object_new_string(dhcpMinAdr));
			json_object_object_add(dhcpSrvObj, "MaxAddress", json_object_new_string(dhcpMaxAdr));
			json_object_object_add(dhcpSrvObj, "DNSServers", json_object_new_string(dnsServer));
			json_object_object_add(dhcpSrvObj, "LeaseTime", json_object_new_int(leaseTime));
			json_object_object_add(dhcpSrvObj, "X_ZYXEL_AutoReserveLanIp", json_object_new_boolean(dhcpAutoReserve));
			zcfgFeObjJsonSet(RDM_OID_DHCPV4_SRV_POOL, &dhcpSrvIid, dhcpSrvObj, NULL);
		}
	}else{
		//original enable dhcp, now disable dhcp server but not delete object
		if(curEnableDhcp){
			setParam = true;
			json_object_object_add(dhcpSrvObj, "Enable", json_object_new_boolean(false));
			zcfgFeObjJsonSet(RDM_OID_DHCPV4_SRV_POOL, &dhcpSrvIid, dhcpSrvObj, NULL);
		}
		
		//original enable dhcp relay, delete dhcp relay server
		if(curRelayDhcp){
			ret = zcfgFeObjJsonDel(RDM_OID_DHCPV4_RELAY_FWD, &dhcpRelayIid, NULL);
		}
	}
	zcfgFeJsonObjFree(dhcpSrvObj);
	zcfgFeJsonObjFree(dhcpRelayObj);
	zcfgFeJsonObjFree(dhcpServerObj);
	return ret;
}

zcfgRet_t setRadvd(struct json_object *Jobj, bool ip6PreTypeStatic, bool isRTADVApply){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *radvdObj = NULL;
	struct json_object *obj = NULL;
	objIndex_t radvdIid = {0};
	objIndex_t iid = {0};
	const char *inface = NULL;
	bool found = false;
	const char *ip6LanAddrAssign = NULL;
	const char *ip6DnsAssign = NULL;
	const char *dnsList = NULL, *dnsServer = NULL;
	
	IID_INIT(radvdIid);
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_RT_ADV_INTF_SET, &iid, &obj) == ZCFG_SUCCESS) {
		inface = json_object_get_string(json_object_object_get(obj, "Interface"));
		if(!strcmp(inface, ifacePath)){
			memcpy(&radvdIid, &iid, sizeof(objIndex_t));
			radvdObj = obj;
			found = true;
			break;
		}
		zcfgFeJsonObjFree(obj);
	}

	if(!found){//POST
		ret = zcfgFeObjJsonAdd(RDM_OID_RT_ADV_INTF_SET, &radvdIid, NULL);
		if(ret == ZCFG_SUCCESS){
			ret = zcfgFeObjJsonGet(RDM_OID_RT_ADV_INTF_SET, &radvdIid, &radvdObj);
			found = true;
		}
	}

	if(found){
		json_object_object_add(radvdObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(radvdObj, "Interface", json_object_new_string(ifacePath));
		
		ip6LanAddrAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_LanAddrAssign"));
		if(json_object_object_get(Jobj, "IPv6_LanAddrAssign")){
        if(!strcmp(ip6LanAddrAssign, "Stateless")){
			json_object_object_add(radvdObj, "AdvManagedFlag", json_object_new_boolean(false));
        }
        else{ //if(!strcmp(ip6LanAddrAssign, "Stateful")){
            json_object_object_add(radvdObj, "AdvManagedFlag", json_object_new_boolean(true));
        }
		}

		ip6DnsAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_DNSAssign"));
		if(json_object_object_get(Jobj, "IPv6_DNSAssign")){
        if(!strcmp(ip6DnsAssign, "DHCP"))//From DHCP6 server
			json_object_object_add(radvdObj, "AdvOtherConfigFlag", json_object_new_boolean(true));
        else if(!strcmp(ip6DnsAssign, "RA_DHCP")){//From DHCP6 server and RA
            json_object_object_add(radvdObj, "AdvOtherConfigFlag", json_object_new_boolean(true));
			json_object_object_add(radvdObj, "X_ZYXEL_RAandDHCP6S", json_object_new_boolean(true));
        }
        else{ //if(!strcmp(ip6DnsAssign, "RA")){
        	json_object_object_add(radvdObj, "AdvOtherConfigFlag", json_object_new_boolean(false));
        }
		}
		
        if(ip6PreTypeStatic){
			json_object_object_add(radvdObj, "ManualPrefixes", json_object_new_string(manualPrefixes));
        }
        else
            json_object_object_add(radvdObj, "ManualPrefixes", json_object_new_string(""));

 		dnsList = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSList"));
		dnsServer = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSServers"));

		json_object_object_add(radvdObj, "X_ZYXEL_DNSList", json_object_new_string(dnsList));
		json_object_object_add(radvdObj, "X_ZYXEL_DNSServers", json_object_new_string(dnsServer));
		if(json_object_object_get(Jobj, "IPv6_DNSAssign")){
		if(!strcmp(ip6DnsAssign, "RA_DHCP"))
			json_object_object_add(radvdObj, "X_ZYXEL_RAandDHCP6S", json_object_new_boolean(true));
        else
			json_object_object_add(radvdObj, "X_ZYXEL_RAandDHCP6S", json_object_new_boolean(false));
		}

		zcfgFeObjJsonSet(RDM_OID_RT_ADV_INTF_SET, &radvdIid, radvdObj, NULL);

	}
	zcfgFeJsonObjFree(radvdObj);
	return ret;
}

zcfgRet_t setIp6PreTypeStatic_Disable(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool isRTADVApply = false;
	const char *prefixWanName = NULL;
	const char *srvName = NULL;
	const char *iP6PrefixDelegateWAN = NULL;
	char selIPIntf[32] = {0};
	int i = 0;
	
	prefixWanName = json_object_get_string(json_object_object_get(Jobj, "IPv6_PrefixWan"));
	iP6PrefixDelegateWAN = json_object_get_string(json_object_object_get(curIpIfaceObj, "X_ZYXEL_IPv6PrefixDelegateWAN"));
	if(json_object_object_get(Jobj, "IPv6_PrefixWan")){	
    if(strcmp(prefixWanName, "Default")){
        for(i = 0; i < json_object_array_length(allIpIfaceObj); i++) {
			struct json_object *subIpIfaceObj = json_object_array_get_idx(allIpIfaceObj, i);
			srvName = json_object_get_string(json_object_object_get(subIpIfaceObj, "X_ZYXEL_SrvName"));
			if(!strcmp(prefixWanName, srvName)){ 
				sprintf(selIPIntf,"IP.Interface.%d",(i+1));		
				if(!strcmp(selIPIntf, iP6PrefixDelegateWAN)){
					isRTADVApply = true;
					break;
				}
        	}
		}//while
    }else{
    	if(!strcmp(prefixWanName, iP6PrefixDelegateWAN)){
			isRTADVApply = true;
    	}
    }
	}
	setRadvd(Jobj, false, isRTADVApply);
	return ret;
}

zcfgRet_t setIp6refixTypeDelegated(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *ip6Origin = NULL;
	const char *IPv6_PrefixWan = NULL;
	const char *srvName = NULL;
	const char *ip6Prefix = NULL;
	const char *ip6Id = NULL;
	const char *IPv6_IdentifierType = NULL;
	char selIPIntf[32] = {0};
	int i = 0;
	const char *ip6LanAdrAssign = NULL;
	const char *ip6DNSAssign = NULL;

	ip6Origin = json_object_get_string(json_object_object_get(Jobj, "IPv6_PrefixType"));
	json_object_object_add(curIpIfaceObj, "IPv6Enable", json_object_new_boolean(true));
	if(json_object_object_get(Jobj, "IPv6_PrefixType"))
	json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6Origin", json_object_new_string(ip6Origin));
	if(json_object_object_get(Jobj, "IPv6_PrefixType")){
	if(!strcmp(ip6Origin, "DelegateFromWan")){
		IPv6_PrefixWan = json_object_get_string(json_object_object_get(Jobj, "IPv6_PrefixWan"));
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6PrefixDelegateWAN", json_object_new_string(IPv6_PrefixWan));
	}else{
		ip6Prefix = json_object_get_string(json_object_object_get(Jobj, "IPv6_Prefix"));
		if(ip6Prefix != NULL)
			json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6Prefix", json_object_new_string(ip6Prefix));
	}
	}

	IPv6_IdentifierType = json_object_get_string(json_object_object_get(Jobj, "IPv6_IdentifierType"));
	if(json_object_object_get(Jobj, "IPv6_IdentifierType")){
	if(!strcmp(IPv6_IdentifierType, "EUI64"))
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6IdentifierType", json_object_new_string("EUI-64"));
	else{ //if(!strcmp(IPv6_IdentifierType, "Manual"))
		ip6Id = json_object_get_string(json_object_object_get(Jobj, "IPv6_Identifier"));
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6IdentifierType", json_object_new_string("Manual"));
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_IPv6Identifier", json_object_new_string(ip6Id));
	}
	}

	ip6LanAdrAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_LanAddrAssign"));
	ip6DNSAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_DNSAssign"));
	if(json_object_object_get(Jobj, "IPv6_LanAddrAssign")){
	if(!strcmp(ip6LanAdrAssign, "Stateless"))
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_AdvManagedFlag", json_object_new_int(0));
	else //if(!strcmp(ip6LanAdrAssign, "Stateful")){
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_AdvManagedFlag", json_object_new_int(1));
	}

	if(json_object_object_get(Jobj, "IPv6_DNSAssign")){
	if(!strcmp(ip6DNSAssign, "DHCP"))
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_AdvOtherConfigFlag", json_object_new_int(1));
	else if(!strcmp(ip6DNSAssign, "RA_DHCP"))
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_AdvOtherConfigFlag", json_object_new_int(1));
	else //if(!strcmp(ip6DNSAssign, "RA"))
		json_object_object_add(curIpIfaceObj, "X_ZYXEL_AdvOtherConfigFlag", json_object_new_int(0));
	}
	zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &curIpIfaceIid, curIpIfaceObj, NULL);
	return ret;
}

zcfgRet_t setIp6LinkLocalEUI64(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *v6AddrObj = NULL;
	objIndex_t v6AddrIid = {0};
	const char *IPv6_LinkLocalAddressType = NULL;
	const char *ip6Origin = NULL;
	const char *ip6LinkLocalAdr = NULL, *curIp6LinkLocalAdr = NULL;
	bool found = false;
	char tmp[5] = {0};

	IPv6_LinkLocalAddressType = json_object_get_string(json_object_object_get(Jobj, "IPv6_LinkLocalAddressType"));

	IID_INIT(v6AddrIid);
	if(IPv6_LinkLocalAddressType != NULL && !strcmp(IPv6_LinkLocalAddressType, "EUI64")){
		while(zcfgFeSubInObjJsonGetNext(RDM_OID_IP_IFACE_V6_ADDR, &curIpIfaceIid, &v6AddrIid, &v6AddrObj)== ZCFG_SUCCESS){
			ip6Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
			curIp6LinkLocalAdr = json_object_get_string(json_object_object_get(v6AddrObj, "IPAddress"));
			strncpy(tmp,curIp6LinkLocalAdr,4);
			if(!strcmp(ip6Origin, "Static") && !strcasecmp(tmp, "fe80")){
				ret = zcfgFeObjJsonDel(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, NULL);
			}
			zcfgFeJsonObjFree(v6AddrObj);
		}
	}else{
		ip6LinkLocalAdr = json_object_get_string(json_object_object_get(Jobj, "IPv6_LinkLocalAddress"));
		if(ip6LinkLocalAdr == NULL)
			return ret;
		else{
			invalidCharFilter((char *)ip6LinkLocalAdr, 26);
			if(IPv6Filter(ip6LinkLocalAdr) == false){
				printf("invalid local ipv6 address...\n");
				return false;
			}
		}
		
		while(zcfgFeSubInObjJsonGetNext(RDM_OID_IP_IFACE_V6_ADDR, &curIpIfaceIid, &v6AddrIid, &v6AddrObj)== ZCFG_SUCCESS){
			ip6Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
			curIp6LinkLocalAdr = json_object_get_string(json_object_object_get(v6AddrObj, "IPAddress"));
			strncpy(tmp,curIp6LinkLocalAdr,4);
			if(!strcmp(ip6Origin, "Static") && !strcasecmp(tmp, "fe80")){
				found = true;
				json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(true));
				json_object_object_add(v6AddrObj, "IPAddress", json_object_new_string(ip6LinkLocalAdr));
				zcfgFeObjJsonSet(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, v6AddrObj, NULL);
			}
			zcfgFeJsonObjFree(v6AddrObj);
		}

		if(!found){//Post
			IID_INIT(v6AddrIid);
			zcfgFeJsonObjFree(v6AddrObj);
			memcpy(&v6AddrIid, &curIpIfaceIid, sizeof(objIndex_t));
			v6AddrIid.level = 1;
			ret = zcfgFeObjJsonAdd(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, NULL);
			if(ret == ZCFG_SUCCESS){
				zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj);
				json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(true));
				json_object_object_add(v6AddrObj, "IPAddress", json_object_new_string(ip6LinkLocalAdr));
				zcfgFeObjJsonSet(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, v6AddrObj, NULL);
				zcfgFeJsonObjFree(v6AddrObj);
			}
		}
	}
	
	return ret;
}

zcfgRet_t setIp6PreTypeStatic_Enable(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *v6prefixObj = NULL;
	objIndex_t v6prefixIid = {0};
	const char *ip6Prefix = NULL, *ipfacev6Prefix = NULL;
	const char * v6PrefixAddress = NULL;
	bool found = false;
	const char *ip6LanAdrAssign = NULL;

	IID_INIT(v6prefixIid);
	while(zcfgFeSubInObjJsonGetNext(RDM_OID_IP_IFACE_V6_PREFIX, &curIpIfaceIid, &v6prefixIid, &v6prefixObj)== ZCFG_SUCCESS){
		ip6Prefix = json_object_get_string(json_object_object_get(v6prefixObj, "Prefix"));
		ipfacev6Prefix = json_object_get_string(json_object_object_get(curIpIfaceObj, "X_ZYXEL_IPv6Prefix"));
		if(!strcmp(ip6Prefix, ipfacev6Prefix)){
			found = true;
			break;
		}
		zcfgFeJsonObjFree(v6prefixObj);
	}

	if(!found){//Post
		IID_INIT(v6prefixIid);
		zcfgFeJsonObjFree(v6prefixObj);
		memcpy(&v6prefixIid, &curIpIfaceIid, sizeof(objIndex_t));
		ret = zcfgFeObjJsonAdd(RDM_OID_IP_IFACE_V6_PREFIX, &v6prefixIid, NULL);
		if(ret == ZCFG_SUCCESS){
			zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V6_PREFIX, &v6prefixIid, &v6prefixObj);
			found = true;
		}
	}

	if(found){
		sprintf(manualPrefixes, "IP.Interface.%hhu.IPv6Prefix.%hhu", v6prefixIid.idx[0], v6prefixIid.idx[1]);
		
		v6PrefixAddress = json_object_get_string(json_object_object_get(Jobj, "IPv6_Prefix"));
		json_object_object_add(v6prefixObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v6prefixObj, "Prefix", json_object_new_string(v6PrefixAddress));

		if(json_object_object_get(Jobj, "IPv6_LanAddrAssign")){
			ip6LanAdrAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_LanAddrAssign"));
		
			if(!strcmp(ip6LanAdrAssign, "Stateless"))
				json_object_object_add(v6prefixObj, "Autonomous", json_object_new_boolean(true));
			else //if(!strcmp(ip6LanAdrAssign, "Stateful"))
				json_object_object_add(v6prefixObj, "Autonomous", json_object_new_boolean(false));
		}

		zcfgFeObjJsonSet(RDM_OID_IP_IFACE_V6_PREFIX, &v6prefixIid, v6prefixObj, NULL);
	}

	setRadvd(Jobj, true, true);
	
	zcfgFeJsonObjFree(v6prefixObj);
	return ret;
}

zcfgRet_t setDHCPv6(struct json_object *Jobj, bool ip6PreTypeStatic){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *dhcp6SrvObj = NULL;;
	objIndex_t dhcp6sPoolIid = {0};
	const char *inface = NULL;
	bool found = false;
	const char *ip6LanAddrAssign = NULL;
	const char *ip6DnsAssign = NULL;
	const char *dnsList = NULL, *dnsServer = NULL;
	const char *minAddress = NULL, *maxAddress = NULL;
	const char *ipv6DomainName = NULL;


#ifdef DHCPV6_RELAY_SUPPORT
	int isDh6rEn = zcfgFeDal_isDhcpv6RelayEnable(ifacePath);
	if (isDh6rEn == 1) {
		printf("[%s][%d] DHCPv6 Relay enabled.\n", __FUNCTION__, __LINE__);
		return ret;
	}
#endif

	IID_INIT(dhcp6sPoolIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV6_SRV_POOL, &dhcp6sPoolIid, &dhcp6SrvObj) == ZCFG_SUCCESS) {
		inface = json_object_get_string(json_object_object_get(dhcp6SrvObj, "Interface"));
		if(!strcmp(inface, ifacePath)){
			found = true;
			break;
		}
		zcfgFeJsonObjFree(dhcp6SrvObj);
	}

	if(!found){//Post
		IID_INIT(dhcp6sPoolIid);
		ret = zcfgFeObjJsonAdd(RDM_OID_DHCPV6_SRV_POOL, &dhcp6sPoolIid, NULL);
		if(ret == ZCFG_SUCCESS){
			zcfgFeObjJsonGet(RDM_OID_DHCPV6_SRV_POOL, &dhcp6sPoolIid, &dhcp6SrvObj);
			found = true;
		}
	}

	if(found){
		json_object_object_add(dhcp6SrvObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(dhcp6SrvObj, "Interface", json_object_new_string(ifacePath));
	
		ip6LanAddrAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_LanAddrAssign"));
		if(json_object_object_get(Jobj, "IPv6_LanAddrAssign")){
		if(!strcmp(ip6LanAddrAssign, "Stateless")){
			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_MinAddress", json_object_new_string(""));
			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_MaxAddress", json_object_new_string(""));
		}
		else{ //if(!strcmp(ip6LanAddrAssign, "Stateful")){
			minAddress = json_object_get_string(json_object_object_get(Jobj, "Ipv6_MinAddress"));
			maxAddress = json_object_get_string(json_object_object_get(Jobj, "Ipv6_MaxAddress"));
			if(minAddress != NULL)
				json_object_object_add(dhcp6SrvObj, "X_ZYXEL_MinAddress", json_object_new_string(minAddress));
			if(maxAddress != NULL)
				json_object_object_add(dhcp6SrvObj, "X_ZYXEL_MaxAddress", json_object_new_string(maxAddress));
		}
		}
		
		ip6DnsAssign = json_object_get_string(json_object_object_get(Jobj, "IPv6_DNSAssign"));
		if(ip6DnsAssign != NULL && !strcmp(ip6DnsAssign, "RA_DHCP")){
			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_RAandDHCP6S", json_object_new_boolean(true));
		}else
			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_RAandDHCP6S", json_object_new_boolean(false));

		ipv6DomainName = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSSuffix"));

		if(ipv6DomainName != NULL)
			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_DNSSuffix", json_object_new_string(ipv6DomainName));
	
		dnsList = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSList"));
		dnsServer = json_object_get_string(json_object_object_get(Jobj, "Ipv6_DNSServers"));

		json_object_object_add(dhcp6SrvObj, "X_ZYXEL_DNSList", json_object_new_string(dnsList));
		json_object_object_add(dhcp6SrvObj, "X_ZYXEL_DNSServers", json_object_new_string(dnsServer));
		json_object_object_add(dhcp6SrvObj, "IANAEnable", json_object_new_boolean(true));

		if(ip6PreTypeStatic)
			json_object_object_add(dhcp6SrvObj, "IANAManualPrefixes", json_object_new_string(dnsList));
		else
			json_object_object_add(dhcp6SrvObj, "IANAManualPrefixes", json_object_new_string(""));

#ifdef MTKSOC_DHCP6S_IAPD_SUPPORT
		{
			bool iapdEnable = json_object_get_boolean(json_object_object_get(Jobj, "X_ZYXEL_IAPDEnable"));
			char *clientDuid = json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_ClientDUID"));
			char *iapdAddr = json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_IAPDIpAddress"));
			char *iapdPrefix = json_object_get_string(json_object_object_get(Jobj, "X_ZYXEL_IAPDPrefixes"));

			json_object_object_add(dhcp6SrvObj, "X_ZYXEL_IAPDEnable", json_object_new_boolean(iapdEnable));
			if (clientDuid) json_object_object_add(dhcp6SrvObj, "X_ZYXEL_ClientDUID", json_object_new_string(clientDuid));
			if (iapdAddr) json_object_object_add(dhcp6SrvObj, "X_ZYXEL_IAPDIpAddress", json_object_new_string(iapdAddr));
			if (iapdPrefix) json_object_object_add(dhcp6SrvObj, "X_ZYXEL_IAPDPrefixes", json_object_new_string(iapdPrefix));
		}
#endif // MTKSOC_DHCP6S_IAPD_SUPPORT

		zcfgFeObjJsonBlockedSet(RDM_OID_DHCPV6_SRV_POOL, &dhcp6sPoolIid, dhcp6SrvObj, NULL);
	}
	
	zcfgFeJsonObjFree(dhcp6SrvObj);
	return ret;
}

zcfgRet_t setDNSQueryScenario(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *dnsObj = NULL;
	objIndex_t dnsIid = {0};
	int dnsQueryScenario = 0;
	const char *DNSQueryScenario = NULL;
	if((DNSQueryScenario = json_object_get_string(json_object_object_get(Jobj, "DNSQueryScenario"))) == NULL)
		return ret;
	
	if(!strcmp(DNSQueryScenario, "IPv4IPv6"))
		dnsQueryScenario = 0;
	else if(!strcmp(DNSQueryScenario, "IPv6Only"))
		dnsQueryScenario = 1;
	else if(!strcmp(DNSQueryScenario, "IPv4Only"))
		dnsQueryScenario = 2;
	else if(!strcmp(DNSQueryScenario, "IPv6First"))
		dnsQueryScenario = 3;
	else //if(!strcmp(DNSQueryScenario, "IPv4First"))
		dnsQueryScenario = 4;

	IID_INIT(dnsIid);
	ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DNS, &dnsIid, &dnsObj);
	if(ret == ZCFG_SUCCESS){
		json_object_object_add(dnsObj, "X_ZYXEL_DNSQueryScenario", json_object_new_int(dnsQueryScenario));
		zcfgFeObjJsonBlockedSet(RDM_OID_DNS, &dnsIid, dnsObj, NULL);
		zcfgFeJsonObjFree(dnsObj);
	}

	return ret;
}

zcfgRet_t getLanInfo(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *bridgingObj = NULL;
	struct json_object *obj = NULL;
	objIndex_t bridgingIid = {0};
	objIndex_t iid = {0};
	const char *brName = NULL;
	const char *IfName = NULL;
	int selectIndex = 0;
	int brIndex = 0;
	int i = 0;

	IID_INIT(bridgingIid);
	selbrName = json_object_get_string(json_object_object_get(Jobj, "Name"));
	while(zcfgFeObjJsonGetNext(RDM_OID_BRIDGING_BR, &bridgingIid, &bridgingObj) == ZCFG_SUCCESS) {
		brName = json_object_get_string(json_object_object_get(bridgingObj, "X_ZYXEL_BridgeName"));
		if(!strcmp(selbrName, brName)){
			zcfgFeJsonObjFree(bridgingObj);
			break;
		}
			selectIndex++;                                           
		zcfgFeJsonObjFree(bridgingObj);
    }

	allIpIfaceObj = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &iid, &obj) == ZCFG_SUCCESS) {
		json_object_array_add(allIpIfaceObj, obj);
	}

	for(i = 0; i < json_object_array_length(allIpIfaceObj); i++) {
		struct json_object *subIpIfaceObj = json_object_array_get_idx(allIpIfaceObj, i);
		IfName = json_object_get_string(json_object_object_get(subIpIfaceObj, "X_ZYXEL_IfName"));
		if(!strncmp(IfName, "br", 2)){
			if(selectIndex == brIndex){//found
				curIpIfaceObj = subIpIfaceObj;
				curIpIfaceIid.idx[0] = (i+1);
				curIpIfaceIid.level = 1;
				sprintf(ifacePath, "IP.Interface.%hhu", curIpIfaceIid.idx[0]);
				break;
			}
			else
				brIndex++;
		}
	}
	return ret;
}

#if 0
zcfgRet_t zcfgFeDal_LanSetup_Get(struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t bridgingIid;
	struct json_object *bridgingObj = NULL;
	struct json_object *pramJobj = NULL;
	const char *Name = NULL;
	
	IID_INIT(bridgingIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_BRIDGING_BR, &bridgingIid, &bridgingObj) == ZCFG_SUCCESS) {
		pramJobj = json_object_new_object();
		Name = json_object_get_string(json_object_object_get(bridgingObj, "X_ZYXEL_BridgeName"));
		                                         
		zcfgFeJsonObjFree(bridgingObj);
    }
}
#endif

zcfgRet_t zcfgFeDal_LanSetup_Edit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool ipv6LanEnable = false;
	bool ip6PreTypeStatic = false;
	const char *IPv6_PrefixType = NULL;

	initGlobalLanObjs();
	getLanInfo(Jobj);
	json_object_object_add(curIpIfaceObj, "X_ZYXEL_SrvName", json_object_new_string(selbrName));
	ret = setLanIP(Jobj);
	if(ret != ZCFG_SUCCESS)
		return ret;
#ifdef TELIA_CUSTOMIZATION //TELIA CLI may not input DHCPType and request AUTO DHCP range after change IP address
	if (json_object_get_string(json_object_object_get(Jobj, "DHCPType")) != NULL) //if NULL, skip setDHCPState
#endif
	ret = setDHCPState(Jobj);
	if(ret != ZCFG_SUCCESS)
		return ret;

	ret = setIgmpSnooping(Jobj);
	if(ret != ZCFG_SUCCESS)
		return ret;


	if(json_object_object_get(Jobj, "IPv6_LanEnable"))
		ipv6LanEnable = json_object_get_boolean(json_object_object_get(Jobj, "IPv6_LanEnable"));
	else
		ipv6LanEnable = json_object_get_boolean(json_object_object_get(curIpIfaceObj, "IPv6Enable"));
	
	if(ipv6LanEnable){
		convertIPv6DnsList(Jobj);
		IPv6_PrefixType = json_object_get_string(json_object_object_get(Jobj, "IPv6_PrefixType"));
		if(json_object_object_get(Jobj, "IPv6_PrefixType")){
		if(!strcmp(IPv6_PrefixType, "Static"))
			ip6PreTypeStatic = true;
		}
		setmldSnooping(Jobj);
		setIp6refixTypeDelegated(Jobj);

		if(!ip6PreTypeStatic){
			setIp6PreTypeStatic_Disable(Jobj);
		}
		else{	
			setIp6PreTypeStatic_Enable(Jobj);
		}
		setIp6LinkLocalEUI64(Jobj);

		setDHCPv6(Jobj,ip6PreTypeStatic);
		setDNSQueryScenario(Jobj);
	}else{
		json_object_object_add(curIpIfaceObj, "IPv6Enable", json_object_new_boolean(false));
		zcfgFeObjJsonBlockedSet(RDM_OID_IP_IFACE, &curIpIfaceIid, curIpIfaceObj, NULL);
	}		
	freeAllLanObjs();
	return ret;
}

zcfgRet_t zcfgFeDal_LanSetup_Get(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL;
	struct json_object *ipifaceObj = NULL;
    struct json_object *ethLinkObj = NULL;
    struct json_object *bridgeBrObj = NULL;
	struct json_object *ipv4addrObj= NULL;
	struct json_object *ipv6addrObj= NULL;
	struct json_object *dhcpv4fwdObj = NULL;
	struct json_object *dhcpv4srvObj = NULL;
	struct json_object *dhcpv6srvObj = NULL;
	struct json_object *dnsObj = NULL;
	struct json_object *rtadvObj = NULL;
	struct json_object *igmpSnoopingObj = NULL;
	struct json_object *mldSnoopingObj = NULL;
	objIndex_t ipifaceIid = {0};
    objIndex_t ethLinkIid = {0};
    objIndex_t bridgeBrIid = {0};
	objIndex_t ipv4addrIid = {0};
	objIndex_t ipv6addrIid = {0};
	objIndex_t dhcpv4fwdIid = {0};
	objIndex_t dhcpv4srvIid = {0};
	objIndex_t dhcpv6srvIid = {0};
	objIndex_t dnsIid = {0};
	objIndex_t rtadvIid = {0};
	objIndex_t igmpSnoopingIid = {0};
	objIndex_t mldSnoopingIid = {0};
	bool dchprelay = false;
	bool showdetail = false;
	int leasetime = 0, min = 0, hour = 0, day = 0, DNSQueryScenario = 0;
	const char *dnstype = NULL, *dnsserver = NULL, *IPv6_IdentifierType = NULL, *IPv6_PrefixType = NULL, *v6DNSServer = NULL, *v6DNSList = NULL, *Name = NULL, *curName = NULL;
	const char *igmpSnooping = NULL, *sub_igmpSnooping = NULL, *mldSnooping = NULL, *sub_mldSnooping = NULL,*ifName = NULL;
	char currintf[32] = {0}, buff[32] = {0}, v6DNSServerTmp[256] = {0}, v6DNSListTmp[256] = {0}, brName[8]={0}, igmpType[8] = {0}, mldType[8] = {0};
	char *dns1 = NULL, *dns2 = NULL, *v6dnstype1 = NULL, *v6dnstype2 = NULL, *v6dnstype3 = NULL, *v6dnsServer1 = NULL, *v6dnsServer2 = NULL, *v6dnsServer3 = NULL, *v6DNSservertmp = NULL, *v6dnstypetmp = NULL, *ptr = NULL, *brPtr = NULL;
	const char *ip6Origin = NULL, *v6IPAddress = NULL, *LowerLayers = NULL;

	while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &ipifaceIid, &ipifaceObj) == ZCFG_SUCCESS){
		if(showdetail)
			break;
		if(!strcmp("", Jgets(ipifaceObj,"X_ZYXEL_SrvName")))
			Jadds(ipifaceObj, "X_ZYXEL_SrvName", "Default");
	
		if(!strncmp(json_object_get_string(json_object_object_get(ipifaceObj, "X_ZYXEL_IfName")), "br", 2)){
            dchprelay = false;
			paramJobj = json_object_new_object();
			curName = json_object_get_string(json_object_object_get(ipifaceObj, "X_ZYXEL_SrvName"));
			ifName = json_object_get_string(json_object_object_get(ipifaceObj, "X_ZYXEL_IfName"));
            
            //For Bridge interface, curName should be Device.Bridging.Bridge.i.X_ZYXEL_BridgeName
            LowerLayers = Jgets(ipifaceObj, "LowerLayers");
            sscanf(LowerLayers, "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
            ethLinkIid.level = 1;
            if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj) == ZCFG_SUCCESS)
            {
                LowerLayers = Jgets(ethLinkObj, "LowerLayers");
                sscanf(LowerLayers, "Bridging.Bridge.%hhu.Port.", &bridgeBrIid.idx[0]);
                bridgeBrIid.level = 1;
                zcfgFeJsonObjFree(ethLinkObj);
                if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_BRIDGING_BR, &bridgeBrIid, &bridgeBrObj) == ZCFG_SUCCESS)
                {
                    curName = Jgets(bridgeBrObj, "X_ZYXEL_BridgeName");
                    Jadds(ipifaceObj, "X_ZYXEL_SrvName", curName);
                    zcfgFeJsonObjFree(bridgeBrObj);
                    curName = Jgets(ipifaceObj, "X_ZYXEL_SrvName");//MTKSOC patches to fix the bug: curName pointer vanish after zcfgFeJsonObjFree(bridgeBrObj)		
                }
            }
			
			if(json_object_object_get(Jobj, "Name")){
				Name = json_object_get_string(json_object_object_get(Jobj, "Name"));
				if(!strcmp(Name,curName))
					showdetail = true;
			}
			json_object_object_add(paramJobj, "Name", JSON_OBJ_COPY(json_object_object_get(ipifaceObj, "X_ZYXEL_SrvName")));
			ipv4addrIid.level = 2;
			ipv4addrIid.idx[0] = ipifaceIid.idx[0];
			ipv4addrIid.idx[1] = 1;
			if(zcfgFeObjJsonGet(RDM_OID_IP_IFACE_V4_ADDR, &ipv4addrIid, &ipv4addrObj) == ZCFG_SUCCESS){
				json_object_object_add(paramJobj, "IPAddress", JSON_OBJ_COPY(json_object_object_get(ipv4addrObj, "IPAddress")));
				json_object_object_add(paramJobj, "SubnetMask", JSON_OBJ_COPY(json_object_object_get(ipv4addrObj, "SubnetMask")));

				zcfgFeJsonObjFree(ipv4addrObj);
			}
			sprintf(currintf,"IP.Interface.%u",ipifaceIid.idx[0]);	// IP.Interface.1 , IP.Interface.6 , IP.Interface.7
			IID_INIT(dhcpv4fwdIid);
			while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_RELAY_FWD, &dhcpv4fwdIid, &dhcpv4fwdObj) == ZCFG_SUCCESS){
				if(!strcmp(json_object_get_string(json_object_object_get(dhcpv4fwdObj, "Interface")),currintf)){
					json_object_object_add(paramJobj, "DHCPType", json_object_new_string("DHCPRelay"));
					json_object_object_add(paramJobj, "DHCP_RelayAddr", JSON_OBJ_COPY(json_object_object_get(dhcpv4fwdObj, "DHCPServerIPAddress")));
					json_object_object_add(paramJobj, "EnableDHCP", json_object_new_boolean(true));
					dchprelay = true;
					zcfgFeJsonObjFree(dhcpv4fwdObj);
					break;
				}
				zcfgFeJsonObjFree(dhcpv4fwdObj);
			}

			IID_INIT(dhcpv4srvIid);
			while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_SRV_POOL, &dhcpv4srvIid, &dhcpv4srvObj) == ZCFG_SUCCESS){
				if(!strcmp(json_object_get_string(json_object_object_get(dhcpv4srvObj, "Interface")),currintf) && !dchprelay){
					json_object_object_add(paramJobj, "DHCPType", json_object_new_string("DHCPServer"));
					if(json_object_get_boolean(json_object_object_get(dhcpv4srvObj, "Enable")))
						json_object_object_add(paramJobj, "EnableDHCP", json_object_new_boolean(true));
					else
						json_object_object_add(paramJobj, "EnableDHCP", json_object_new_boolean(false));

					json_object_object_add(paramJobj, "DHCP_MinAddress", JSON_OBJ_COPY(json_object_object_get(dhcpv4srvObj, "MinAddress")));
					json_object_object_add(paramJobj, "DHCP_MaxAddress", JSON_OBJ_COPY(json_object_object_get(dhcpv4srvObj, "MaxAddress")));
					json_object_object_add(paramJobj, "DHCP_AutoReserveLanIp", JSON_OBJ_COPY(json_object_object_get(dhcpv4srvObj, "X_ZYXEL_AutoReserveLanIp")));
					json_object_object_add(paramJobj, "DHCP_LeaseTime", JSON_OBJ_COPY(json_object_object_get(dhcpv4srvObj, "LeaseTime")));
// DNS servers (From ISP)  is for all ptojects  @2022/07/04 yuchih
//#ifdef TELIA_CUSTOMIZATION //For TELIA CLI show DNS servers address
					json_object_object_add(paramJobj, "DNSServers", JSON_OBJ_COPY(json_object_object_get(dhcpv4srvObj, "DNSServers")));
//#endif
					dnstype = json_object_get_string(json_object_object_get(dhcpv4srvObj, "X_ZYXEL_DNS_Type"));
					if(!strcmp(dnstype, "DNS Proxy") )
						json_object_object_add(paramJobj, "DNS_Type", json_object_new_string("DNSProxy"));
					else if(!strcmp(dnstype, "Static") || !strcmp(dnstype, "From ISP")){
						if(!strcmp(dnstype, "Static")){
							json_object_object_add(paramJobj, "DNS_Type", json_object_new_string("Static"));
							dnsserver = json_object_get_string(json_object_object_get(dhcpv4srvObj, "DNSServers"));
							strcpy(buff, dnsserver);
							dns1 = strtok_r(buff, ",", &dns2);
							json_object_object_add(paramJobj, "DNS_Servers1", json_object_new_string(dns1));
							if(dns2 != NULL){
								json_object_object_add(paramJobj, "DNS_Servers2", json_object_new_string(dns2));
							}
						}
						else if(!strcmp(dnstype, "From ISP")){
							json_object_object_add(paramJobj, "DNS_Type", json_object_new_string("FromISP"));
						}
					}
					zcfgFeJsonObjFree(dhcpv4srvObj);
					break;

				}
				zcfgFeJsonObjFree(dhcpv4srvObj);	
			}

			//IGMP snooping
			IID_INIT(igmpSnoopingIid);
			if(zcfgFeObjJsonGet(RDM_OID_ZY_IGMP, &igmpSnoopingIid, &igmpSnoopingObj) == ZCFG_SUCCESS){
				igmpSnooping = json_object_get_string(json_object_object_get(igmpSnoopingObj, "SnoopingBridgeIfName"));	
				if(igmpSnooping != NULL){
					memset(buff, 0, sizeof(buff));
					brPtr = NULL;
					strcpy(buff, igmpSnooping);
					sub_igmpSnooping = strtok_r(buff, ",", &brPtr);
					while(sub_igmpSnooping != NULL){
						if(!strncmp(ifName,sub_igmpSnooping, 3)){
							sscanf(sub_igmpSnooping, "%[^|]%s", &brName, &igmpType); 
							if(!strcmp(igmpType,"|0"))
								json_object_object_add(paramJobj, "IGMPSnoopingMode", json_object_new_string("Disabled"));
							else if(!strcmp(igmpType,"|1"))
								json_object_object_add(paramJobj, "IGMPSnoopingMode", json_object_new_string("Standard"));
							else 
								json_object_object_add(paramJobj, "IGMPSnoopingMode", json_object_new_string("Blocking"));
							break;
						}
						sub_igmpSnooping = strtok_r(brPtr, ",", &brPtr);
					}
				}
				zcfgFeJsonObjFree(igmpSnoopingObj);
			}

			json_object_object_add(paramJobj, "IPv6_LanEnable", JSON_OBJ_COPY(json_object_object_get(ipifaceObj, "IPv6Enable")));
			if(json_object_get_boolean(json_object_object_get(ipifaceObj, "IPv6Enable"))){	//IPv6
				IID_INIT(ipv6addrIid);
				ipv6addrIid.level = 2;
				ipv6addrIid.idx[0] = ipifaceIid.idx[0];
				json_object_object_add(paramJobj, "IPv6_LinkLocalAddressType", json_object_new_string("EUI64"));
				while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE_V6_ADDR, &ipv6addrIid, &ipv6addrObj) == ZCFG_SUCCESS){
					ip6Origin = json_object_get_string(json_object_object_get(ipv6addrObj, "Origin"));
					v6IPAddress = json_object_get_string(json_object_object_get(ipv6addrObj, "IPAddress"));
					if(!strcmp(ip6Origin, "Static") && !strncasecmp(v6IPAddress, "fe80", 4)){
						json_object_object_add(paramJobj, "IPv6_LinkLocalAddressType", json_object_new_string("Manual"));
						json_object_object_add(paramJobj, "IPv6_LinkLocalAddress", JSON_OBJ_COPY(json_object_object_get(ipv6addrObj, "IPAddress")));
						zcfgFeJsonObjFree(ipv6addrObj);
						break;
					}
					zcfgFeJsonObjFree(ipv6addrObj);
				}

				IPv6_IdentifierType = json_object_get_string(json_object_object_get(ipifaceObj, "X_ZYXEL_IPv6IdentifierType"));
				json_object_object_add(paramJobj, "IPv6_IdentifierType", json_object_new_string(IPv6_IdentifierType));
				if(!strcmp(IPv6_IdentifierType, "Manual"))
					json_object_object_add(paramJobj, "IPv6_Identifier", JSON_OBJ_COPY(json_object_object_get(ipifaceObj, "X_ZYXEL_IPv6Identifier")));
				IPv6_PrefixType = json_object_get_string(json_object_object_get(ipifaceObj, "X_ZYXEL_IPv6Origin"));
				json_object_object_add(paramJobj, "IPv6_PrefixType", json_object_new_string(IPv6_PrefixType));
				if(!strcmp(IPv6_PrefixType ,"Static"))
					json_object_object_add(paramJobj, "IPv6_Prefix", JSON_OBJ_COPY(json_object_object_get(ipifaceObj, "X_ZYXEL_IPv6Prefix")));
				else if(!strcmp(IPv6_PrefixType, "DelegateFromWan")){
					json_object_object_add(paramJobj, "IPv6_PrefixWan", JSON_OBJ_COPY(json_object_object_get(ipifaceObj, "X_ZYXEL_IPv6PrefixDelegateWAN")));
				}

				//MLD snooping
				IID_INIT(mldSnoopingIid);
				if(zcfgFeObjJsonGet(RDM_OID_ZY_MLD, &mldSnoopingIid, &mldSnoopingObj) == ZCFG_SUCCESS){
					mldSnooping = json_object_get_string(json_object_object_get(mldSnoopingObj, "SnoopingBridgeIfName")); 
					if(mldSnooping != NULL){
						memset(buff, 0, sizeof(buff));
						brPtr = NULL;
						strcpy(buff, mldSnooping);
						sub_mldSnooping = strtok_r(buff, ",", &brPtr);
						while(sub_mldSnooping != NULL){
							if(!strncmp(ifName,sub_mldSnooping, 3)){
								sscanf(sub_mldSnooping, "%[^|]%s", &brName, &mldType); 
								if(!strcmp(mldType,"|0"))
									json_object_object_add(paramJobj, "MLDSnoopingMode", json_object_new_string("Disabled"));
								else if(!strcmp(mldType,"|1"))
									json_object_object_add(paramJobj, "MLDSnoopingMode", json_object_new_string("Standard"));
								else 
									json_object_object_add(paramJobj, "MLDSnoopingMode", json_object_new_string("Blocking"));
								break;
							}
							sub_mldSnooping = strtok_r(brPtr, ",", &brPtr);
						}
					}
					zcfgFeJsonObjFree(mldSnoopingObj);
				}

				IID_INIT(rtadvIid);
				while(zcfgFeObjJsonGetNext(RDM_OID_RT_ADV_INTF_SET, &rtadvIid, &rtadvObj) == ZCFG_SUCCESS){
					if(!strcmp(json_object_get_string(json_object_object_get(rtadvObj, "Interface")),currintf)){
						if(json_object_get_boolean(json_object_object_get(rtadvObj, "AdvOtherConfigFlag"))){
							if(json_object_get_boolean(json_object_object_get(rtadvObj, "X_ZYXEL_RAandDHCP6S")))
								json_object_object_add(paramJobj, "IPv6_DNSAssign", json_object_new_string("RA_DHCP"));
							else
								json_object_object_add(paramJobj, "IPv6_DNSAssign", json_object_new_string("DHCP"));
						}
						else
							json_object_object_add(paramJobj, "IPv6_DNSAssign", json_object_new_string("RA"));
						zcfgFeJsonObjFree(rtadvObj);
						break;
					}
					zcfgFeJsonObjFree(rtadvObj);
				}

				IID_INIT(dhcpv6srvIid);
				while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV6_SRV_POOL, &dhcpv6srvIid, &dhcpv6srvObj) == ZCFG_SUCCESS){
					if(!strcmp(currintf, json_object_get_string(json_object_object_get(dhcpv6srvObj, "Interface")))){ 
					if(json_object_get_int(json_object_object_get(ipifaceObj, "X_ZYXEL_AdvManagedFlag")) == 1){
						json_object_object_add(paramJobj, "IPv6_LanAddrAssign", json_object_new_string("Stateful"));
						json_object_object_add(paramJobj, "Ipv6_MinAddress", JSON_OBJ_COPY(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_MinAddress")));
						json_object_object_add(paramJobj, "Ipv6_MaxAddress", JSON_OBJ_COPY(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_MaxAddress")));
						json_object_object_add(paramJobj, "Ipv6_DNSSuffix", JSON_OBJ_COPY(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_DNSSuffix")));
					}
					else
						json_object_object_add(paramJobj, "IPv6_LanAddrAssign", json_object_new_string("Stateless"));
						v6DNSServer = json_object_get_string(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_DNSServers"));
						v6DNSList = json_object_get_string(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_DNSList")); 
						
						strcpy(v6DNSServerTmp,v6DNSServer);
						strcpy(v6DNSListTmp,v6DNSList);
						v6dnstype1 = strtok_r(v6DNSListTmp, ",", &v6dnstypetmp);
						v6dnstype2 = strtok_r(NULL, ",", &v6dnstypetmp);
						v6dnstype3 = strtok_r(NULL, ",", &v6dnstypetmp);
						v6dnsServer1=v6DNSServerTmp;
						*(ptr = strchr(v6dnsServer1, ',')) = '\0';
						v6dnsServer2=ptr+1;
						*(ptr = strchr(v6dnsServer2, ',')) = '\0';
						v6dnsServer3=ptr+1;
						if(!strcmp(v6dnstype1, "None") || !strcmp(v6dnstype1, "ISP") ||!strcmp(v6dnstype1, "Proxy"))
							json_object_object_add(paramJobj, "Ipv6_DNSServer1", json_object_new_string(v6dnstype1));
						else
							json_object_object_add(paramJobj, "Ipv6_DNSServer1", json_object_new_string(v6dnsServer1));
						if(!strcmp(v6dnstype2, "None") || !strcmp(v6dnstype2, "ISP") || !strcmp(v6dnstype2, "Proxy"))
							json_object_object_add(paramJobj, "Ipv6_DNSServer2", json_object_new_string(v6dnstype2));
						else
							json_object_object_add(paramJobj, "Ipv6_DNSServer2", json_object_new_string(v6dnsServer2));
						if(!strcmp(v6dnstype3, "None") || !strcmp(v6dnstype3, "ISP") || !strcmp(v6dnstype3, "Proxy"))
							json_object_object_add(paramJobj, "Ipv6_DNSServer3", json_object_new_string(v6dnstype3));
						else
							json_object_object_add(paramJobj, "Ipv6_DNSServer3", json_object_new_string(v6dnsServer3));

#ifdef MTKSOC_DHCP6S_IAPD_SUPPORT
						{
							bool iapdEnable = false;
							char *ClientDUID = json_object_get_string(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_ClientDUID"));
							char *IAPDIpAddress = json_object_get_string(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_IAPDIpAddress"));
							char *IAPDPrefixes = json_object_get_string(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_IAPDPrefixes"));
							if (json_object_get_boolean(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_IAPDEnable"))) {
								iapdEnable = json_object_get_boolean(json_object_object_get(dhcpv6srvObj, "X_ZYXEL_IAPDEnable"));
							}

							json_object_object_add(paramJobj, "X_ZYXEL_IAPDEnable", json_object_new_boolean(iapdEnable));
							json_object_object_add(paramJobj, "X_ZYXEL_ClientDUID", ClientDUID?json_object_new_string(ClientDUID):json_object_new_string(""));
							json_object_object_add(paramJobj, "X_ZYXEL_IAPDIpAddress", IAPDIpAddress?json_object_new_string(IAPDIpAddress):json_object_new_string(""));
							json_object_object_add(paramJobj, "X_ZYXEL_IAPDPrefixes", IAPDPrefixes?json_object_new_string(IAPDPrefixes):json_object_new_string(""));
						}
#endif

						zcfgFeJsonObjFree(dhcpv6srvObj);
						break;
					}
					zcfgFeJsonObjFree(dhcpv6srvObj);
				}
				if(zcfgFeObjJsonGet(RDM_OID_DNS, &dnsIid, &dnsObj) == ZCFG_SUCCESS){
					DNSQueryScenario = json_object_get_int(json_object_object_get(dnsObj, "X_ZYXEL_DNSQueryScenario"));
					if(DNSQueryScenario == 0)
						json_object_object_add(paramJobj, "DNSQueryScenario", json_object_new_string("IPv4IPv6"));
					else if(DNSQueryScenario == 1)
						json_object_object_add(paramJobj, "DNSQueryScenario", json_object_new_string("IPv6Only"));
					else if(DNSQueryScenario == 2)
						json_object_object_add(paramJobj, "DNSQueryScenario", json_object_new_string("IPv4Only"));
					else if(DNSQueryScenario == 3)
						json_object_object_add(paramJobj, "DNSQueryScenario", json_object_new_string("IPv6First"));
					else if(DNSQueryScenario == 4)
						json_object_object_add(paramJobj, "DNSQueryScenario", json_object_new_string("IPv4First"));
					zcfgFeJsonObjFree(dnsObj);
				}
			}
			if(json_object_object_get(Jobj, "Name")){
				if(showdetail){
					json_object_object_add(paramJobj, "ShowDetail", json_object_new_boolean(true));
					json_object_array_add(Jarray, paramJobj);
				}
			}
			else
			json_object_array_add(Jarray, paramJobj);
		}
		zcfgFeJsonObjFree(ipifaceObj);
	}
	return ret;

}

void zcfgFeDalShowLanSetup(struct json_object *Jarray){
	struct json_object *obj = NULL, *obj_idx = NULL;
	int len = 0, i;
	int leasetime = 0, min = 0, hour = 0, day = 0;
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	len = json_object_array_length(Jarray);
	obj_idx = json_object_array_get_idx(Jarray, 0);
#ifdef TELIA_CUSTOMIZATION
	if(len == 1){ //Telia customization will always show detail
#else
	if(len == 1 && json_object_get_boolean(json_object_object_get(obj_idx, "ShowDetail"))){
#endif
	obj = json_object_array_get_idx(Jarray, 0);
	printf("%-15s %s \n","Group Name",json_object_get_string(json_object_object_get(obj, "Name")));
	printf("%-15s %s \n","IP Address",json_object_get_string(json_object_object_get(obj, "IPAddress")));
	printf("%-15s %s \n","Subnet Mask",json_object_get_string(json_object_object_get(obj, "SubnetMask")));

	
	if(!strcmp("DHCPRelay",json_object_get_string(json_object_object_get(obj, "DHCPType")))){
		printf("%-15s %s \n","DHCP","DHCP Relay");
		printf("%-20s %-10s\n", "DHCP Relay Address", json_object_get_string(json_object_object_get(obj, "DHCP_RelayAddr")));
	} else {
		if(json_object_get_boolean(json_object_object_get(obj, "EnableDHCP"))){
			printf("%-15s %s \n","DHCP","Enable");
			printf("%-2s %-25s %s \n", "", "Beginning IP Address",json_object_get_string(json_object_object_get(obj, "DHCP_MinAddress")));
			printf("%-2s %-25s %s \n", "", "Ending IP Address",json_object_get_string(json_object_object_get(obj, "DHCP_MaxAddress")));
			printf("%-2s %-25s %s \n", "", "AutoReserveLanIp",json_object_get_string(json_object_object_get(obj, "DHCP_AutoReserveLanIp")));
			leasetime = json_object_get_int(json_object_object_get(obj, "DHCP_LeaseTime"));
			leasetime = leasetime / 60;
			min = leasetime % 60;
			leasetime = (leasetime - (leasetime % 60)) / 60;
			hour = leasetime % 24;
			leasetime = (leasetime - (leasetime % 24)) / 24;
			day = leasetime;
			printf("%-2s %-25s %d %s %d %s %d %s \n", "", "DHCP Server Lease Time",day,"Days",hour,"Hours",min,"Minutes");
#ifdef TELIA_CUSTOMIZATION
			leasetime = json_object_get_int(json_object_object_get(obj, "DHCP_LeaseTime"));
			printf("%-2s %-25s %-13s %d \n", "", "DHCP Server Lease Time in Seconds","",leasetime);
			printf("%-2s %-15s %s \n", "", "DNS Values",json_object_get_string(json_object_object_get(obj, "DNS_Type")));
			printf("%-2s %-15s %s \n", "", "DNS Server Address ", json_object_get_string(json_object_object_get(obj, "DNSServers")));
#else
			printf("%-2s %-15s %s \n", "", "DNS Values",json_object_get_string(json_object_object_get(obj, "DNS_Type")));
#endif
		}else{
			printf("%-15s %s \n","DHCP","Disable");
		}
	}
	printf("%-15s %s \n","IPv6 Active",json_object_get_string(json_object_object_get(obj, "IPv6_LanEnable")));
	if(json_object_get_boolean(json_object_object_get(obj, "IPv6_LanEnable"))){
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_LinkLocalAddressType")),"Manual"))
			printf("%-30s %-10s %-10s\n", "Link Local Address Type", json_object_get_string(json_object_object_get(obj, "IPv6_LinkLocalAddressType")),json_object_get_string(json_object_object_get(obj, "IPv6_LinkLocalAddress")));
		else if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_LinkLocalAddressType")),"EUI64"))
			printf("%-30s %-10s\n", "Link Local Address Type", json_object_get_string(json_object_object_get(obj, "IPv6_LinkLocalAddressType")));
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_IdentifierType")),"Manual"))
				printf("%-30s %-10s %-10s\n", "LAN Global Identifier Type", json_object_get_string(json_object_object_get(obj, "IPv6_IdentifierType")),json_object_get_string(json_object_object_get(obj, "IPv6_Identifier")));
			else if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_IdentifierType")),"EUI-64"))
				printf("%-30s %-10s\n", "LAN Global Identifier Type", json_object_get_string(json_object_object_get(obj, "IPv6_IdentifierType")));
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_PrefixType")),"DelegateFromWan"))
			printf("%-30s %-30s %-10s\n", "LAN IPv6 Prefix Setup", "Delegate prefix from WAN", json_object_get_string(json_object_object_get(obj, "IPv6_PrefixWan")));
		else if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_PrefixType")),"Static"))
			printf("%-30s %-10s %-10s\n", "LAN IPv6 Prefix Setup", "Static", json_object_get_string(json_object_object_get(obj, "IPv6_Prefix")));
		printf("%-30s %-10s\n", "LAN IPv6 Address Assign Setup", json_object_get_string(json_object_object_get(obj, "IPv6_LanAddrAssign")));
		printf("%-30s %-10s\n", "LAN IPv6 DNS Assign Setup", json_object_get_string(json_object_object_get(obj, "IPv6_DNSAssign")));
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_DNSAssign")), "RA"))
			printf("%-30s %-10s\n", "DHCPv6 Active", "DHCPv6 Disable");
		else
			printf("%-30s %-10s\n", "DHCPv6 Active", "DHCPv6 Server");
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "IPv6_LanAddrAssign")),"Stateful")){
				printf("%-30s %-10s\n", "IPv6 Start Address", json_object_get_string(json_object_object_get(obj, "IPv6_MinAddress")));
				printf("%-30s %-10s\n", "IPv6 End Address", json_object_get_string(json_object_object_get(obj, "Ipv6_MaxAddress")));
				printf("%-30s %-10s\n", "IPv6 Domain Name Server", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSSuffix")));
			}
			printf("IPv6 DNS Values \n");
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer1")),"None") || !strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer1")),"ISP")){
				printf("%-2s %-25s %-12s\n", "", "IPv6 DNS Server 1", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer1")));
			} else {
				printf("%-2s %-25s %-12s %-10s\n", "", "IPv6 DNS Server 1", "User Defined", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer1")));
			}
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer2")),"None") || !strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer2")),"ISP")){
				printf("%-2s %-25s %-12s\n", "", "IPv6 DNS Server 2", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer2")));
			} else {
				printf("%-2s %-25s %-12s %-10s\n", "", "IPv6 DNS Server 2", "User Defined", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer2")));
			}
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer3")),"None") || !strcmp(json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer3")),"ISP")){
				printf("%-2s %-25s %-12s\n", "", "IPv6 DNS Server 3", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer3")));
			} else {
				printf("%-2s %-25s %-12s %-10s\n", "", "IPv6 DNS Server 3", "User Defined", json_object_get_string(json_object_object_get(obj, "Ipv6_DNSServer3")));
			}
			printf("%-30s ", "DNS Query Scenario");		
			if(!strcmp(json_object_get_string(json_object_object_get(obj, "DNSQueryScenario")), "IPv4IPv6"))
				printf("%-10s\n", "IPv4/IPv6 DNS Server");		
			else if(!strcmp(json_object_get_string(json_object_object_get(obj, "DNSQueryScenario")), "IPv6Only"))
				printf("%-10s\n", "IPv6 DNS Server Only");		
			else if(!strcmp(json_object_get_string(json_object_object_get(obj, "DNSQueryScenario")), "IPv4Only"))
				printf("%-10s\n", "IPv4 DNS Server Only");		
			else if(!strcmp(json_object_get_string(json_object_object_get(obj, "DNSQueryScenario")), "IPv6First"))
				printf("%-10s\n", "IPv6 DNS Server First");		
			else //if(!strcmp(json_object_get_string(json_object_object_get(obj, "DNSQueryScenario")), "IPv4First"))
				printf("%-10s\n", "IPv4 DNS Server First");		

		}
	}
	else{
#ifdef MTKSOC_DHCP6S_IAPD_SUPPORT
		printf("%-15s %-20s %-20s %-15s %-15s %-20s %-20s %-20s \n", "Group Name", "LAN IP Address", "IPv4 DHCP Server", "IPv6 Enable", "IAPD Enable", "Client DUID", "Client Address", "Client Prefix");
		printf("%-15s %-20s %-20s %-15s %-15s %-20s %-20s %-20s \n", "==========", "==============", "================", "===========", "===========", "===========", "==============", "=============");
		for(i=0;i<len;i++){
			int enableDHCP = 0;
			bool enableIAPD = false;
			obj = json_object_array_get_idx(Jarray, i);
			enableDHCP = json_object_get_boolean(json_object_object_get(obj, "EnableDHCP"));
			enableIAPD = json_object_get_boolean(json_object_object_get(obj, "X_ZYXEL_IAPDEnable"));
			if (enableDHCP == 1) {
				printf("%-15s %-20s %-20s %-15s %-15s %-20s %-20s %-20s \n", 
					json_object_get_string(json_object_object_get(obj, "Name")),
					json_object_get_string(json_object_object_get(obj, "IPAddress")),
					(enableDHCP==1)?"Enable":"Disable",
					json_object_get_string(json_object_object_get(obj, "IPv6_LanEnable")),

					(enableIAPD?"Enable":"Disable"),
					json_object_get_string(json_object_object_get(obj, "X_ZYXEL_ClientDUID")),
					json_object_get_string(json_object_object_get(obj, "X_ZYXEL_IAPDIpAddress")),
					json_object_get_string(json_object_object_get(obj, "X_ZYXEL_IAPDPrefixes"))
					);
			}
		}
#else
		printf("%-15s %-20s %-20s %-15s \n", "Group Name", "LAN IP Address", "IPv4 DHCP Server", "IPv6 Enable");
		for(i=0;i<len;i++){
			obj = json_object_array_get_idx(Jarray, i);
			if(json_object_get_boolean(json_object_object_get(obj, "EnableDHCP"))){
				printf("%-15s %-20s %-20s %-15s \n", 
					json_object_get_string(json_object_object_get(obj, "Name")),
					json_object_get_string(json_object_object_get(obj, "IPAddress")),
					"Enable",
					json_object_get_string(json_object_object_get(obj, "IPv6_LanEnable")));
			}else{
				printf("%-15s %-20s %-20s %-15s \n", 
					json_object_get_string(json_object_object_get(obj, "Name")),
					json_object_get_string(json_object_object_get(obj, "IPAddress")),
					"Disable",
					json_object_get_string(json_object_object_get(obj, "IPv6_LanEnable")));
			}
		}
#endif
	}
}




zcfgRet_t zcfgFeDalLanSetup(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	if(!strcmp(method, "PUT"))
		ret = zcfgFeDal_LanSetup_Edit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDal_LanSetup_Get(Jobj, Jarray, NULL); 
	else
		printf("Unknown method:%s\n", method);
	//else if(!strcmp(method, "GET"))
		//ret = zcfgFeDal_LanSetup_Get(Jobj, NULL);
	
	
	return ret;
}

