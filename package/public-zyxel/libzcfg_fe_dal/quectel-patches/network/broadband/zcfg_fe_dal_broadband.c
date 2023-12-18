#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_net.h"
#if defined(SAME_FW_FOR_TWO_BOARD)
#include "zyutil_cofw.h"
#endif

//#define JSON_OBJ_COPY(json_object) json_tokener_parse(json_object_to_json_string(json_object))

dal_param_t WAN_param[]={
	{"Index",				dalType_int,	0,	0,	NULL, NULL, dal_Add_ignore, dal_Edit_ignore},
	{"ipIfacePath",			dalType_string,	1,	32,	NULL, NULL, dalcmd_Forbid}, //read only
	{"Name",				dalType_string,	1,	32,	NULL, NULL, dal_Add|dal_Edit|dal_Delete},
	{"Enable",				dalType_boolean,	0,	0,	NULL},
	{"Type",				dalType_string,	3,	4,	NULL, "ATM|PTM|ETH|GPON|CELL", dal_Add | dal_Edit_ignore},
	{"Mode",				dalType_string,	9,	10,	NULL, "IP_Routed|IP_Bridged", dal_Add},
	{"Encapsulation",		dalType_string,	4,	5,	NULL, "IPoE|IPoA|PPPoE|PPPoA"},
	{"ipMode",				dalType_string,	4,	9,	NULL, "IPv4|DualStack|IPv6"},
	//atmLink
	{"vpivci",				dalType_vpivci,	1,	256,	NULL}, // VPI/VCI
	{"AtmEncapsulation",	dalType_string,	1,	6,	NULL},
	{"QoSClass",			dalType_string,	3,	8,	NULL, "UBR|CBR|VBR-nrt|VBR-rt"},
	{"atmPeakCellRate",		dalType_int,	0,	255000,	NULL},
	{"atmSustainedCellRate",dalType_int,	0,	255000,	NULL},
	{"atmMaxBurstSize",		dalType_int,	0,	1000000,	NULL},
	//GPON
	//{"slidValue",			dalType_int,	0,	0,	NULL},
	//VLAN term
	{"VLANEnable",			dalType_boolean,	0,	0,	NULL},
	{"VLANID",				dalType_int,		-1,	4094,	NULL},
	{"VLANPriority",		dalType_int,		-1,	7,	NULL},
	//ppp Iface
	{"pppUsername",			dalType_string,	1,	64,	NULL},
	{"pppPassword",			dalType_string,	1,	64,	NULL},
	{"ConnectionTrigger",	dalType_string,	0,	0,	NULL, "AlwaysOn|OnDemand"},
	{"IdleDisconnectTime",	dalType_int,	0,	0,	NULL},
	{"pppoePassThrough",	dalType_boolean,	0,	0,	NULL},
	//Nat
	{"NatEnable",			dalType_boolean,	0,	0,	NULL},
	{"FullConeEnabled",		dalType_boolean,	0,	0,	NULL},
	//IGMP/MLD
	{"IGMPEnable",			dalType_boolean,	0,	0,	NULL},
	{"MLDEnable",			dalType_boolean,	0,	0,	NULL},
	//DNS
	{"ipDnsStatic",			dalType_boolean,	0,	0,	NULL},
	{"DNSServer",			dalType_string,	0,	0,	NULL},
	{"ip6DnsStatic",		dalType_boolean,	0,	0,	NULL},
	{"DNS6Server",			dalType_v6Addr,	0,	0,	NULL},
	//router
	{"sysGwEnable",			dalType_boolean,	0,	0,	NULL},
	{"sysGw6Enable",		dalType_boolean,	0,	0,	NULL},
	//6RD
	{"Enable_6RD",			dalType_boolean,	0,	0,	NULL},
	{"v6RD_Type",			dalType_string,	0,	0,	NULL, "dhcp|static"},
	{"SPIPv6Prefix",		dalType_string,	0,	0,	NULL},
	{"IPv4MaskLength",		dalType_int,	0,	32,	NULL},
	{"BorderRelayIPv4Addresses",dalType_v4Addr,	0,	0,	NULL},
	//DSLite
	{"DSLiteEnable",		dalType_boolean,	0,	0,	NULL},
	{"DSLiteType",			dalType_string,	0,	0,	NULL, "static"},
	{"DSLiteRelayIPv6Addresses",	dalType_v6Addr,	0,	0,	NULL},
	//ipIface or pppIface
	{"MaxMTUSize",			dalType_int,	0,	0,	NULL},
	//Address, gateway, DHCP
	{"ipStatic",			dalType_boolean,	0,	0,	NULL},
	{"IPAddress",			dalType_v4Addr,	0,	0,	NULL},
	{"SubnetMask",			dalType_v4Mask,	0,	0,	NULL},
	{"GatewayIPAddress",	dalType_v4Addr,	0,	0,	NULL},
	{"ip6Static",			dalType_boolean,	0,	0,	NULL},
	{"IP6Address",			dalType_v6AddrPrefix,	0,	0,	NULL},
	{"NextHop",				dalType_v6Addr,	0,	0,	NULL},
     /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
	{"IPv6Manual_IANA",		dalType_boolean,	0,	0,	NULL},
	{"IPv6Manual_IAPD",		dalType_boolean,	0,	0,	NULL},	
	//DHCP
	{"option42Enable",		dalType_boolean,	0,	0,	NULL},
	{"option43Enable",		dalType_boolean,	0,	0,	NULL},
	{"option120Enable",		dalType_boolean,	0,	0,	NULL},
	{"option121Enable",		dalType_boolean,	0,	0,	NULL},
	{"option60Enable",		dalType_boolean,	0,	0,	NULL},
	{"option60Value",		dalType_string,	0,	255,	NULL},
	{"option61Enable",		dalType_boolean,	0,	0,	NULL},
	{"option61Value",		dalType_string,	0,	255,	NULL, NULL, dalcmd_Forbid},
	{"option61Iaid",		dalType_string, 0,	255,	NULL},
	{"option61Duid",		dalType_string, 0,	255,	NULL},
	{"option125Enable",		dalType_boolean,	0,	0,	NULL},
	{"option125Value",		dalType_string,	0,	255,	NULL},
	//ConcurrentWan
	{"ConcurrentWan",		dalType_int,	0,	15,	NULL},
	{"ManufacturerOUI",     dalType_string,	0,	0,	NULL, NULL, dalcmd_Forbid},
	{"SerialNumber",		dalType_string,	0,	0,	NULL, NULL, dalcmd_Forbid},
	{"ProductClass",		dalType_string,	0,	0,	NULL, NULL, dalcmd_Forbid},
#ifdef ZYXEL_WAN_MAC
	{"WanMacType",		    dalType_string,	0,	0,	NULL, "Default|ClientIP|UserDefine"},
	{"WanMacIP",		    dalType_v4Addr,	0,	0,	NULL},
	{"WanMacAddr",		    dalType_string,	0,	0,	NULL},
#endif
	{"LantoWan",			dalType_boolean, 0,  0,  NULL},
	{NULL,					0,	0,	0,	NULL},
};


char currLowerLayers[128] = {0};
char ipIfacePath[32] = {0};

bool isAdd = false;
bool isDelete = false;

const char *Name, *Type, *Mode, *Encapsulation, *ipMode, *v6RD_Type, *deleteipIfacePath;
#ifdef ZYXEL_WAN_MAC
const char *WanMacType, *WanMacAddr, *WanMacIP;
#endif
bool ipStatic, ip6Static, Enable_6RD, ip6DnsStatic, IPv6Manual_IANA, IPv6Manual_IAPD;

char CurrType[16] = {0};
char CurrMode[16] = {0};
char CurrEncapsulation[16] = {0};
char CurrIpMode[16] = {0};
char CurrV6RD_Type[16] = {0};
bool CurrEnable = false, CurrIpStatic = false, CurrIp6Static = false, CurrPppStatic = false, CurrEnable_6RD = false;


struct json_object *atmLinkObj = NULL;
struct json_object *atmLinkQosObj = NULL;
struct json_object *ptmLinkObj = NULL;
struct json_object *ethIfaceObj = NULL;
struct json_object *optIfaceObj = NULL;
struct json_object *cellIfaceObj = NULL;
struct json_object *ethLinkObj = NULL;
struct json_object *vlanTermObj = NULL;
struct json_object *pppIfaceObj = NULL;
struct json_object *ipIfaceObj = NULL;
struct json_object *v4ClientObj = NULL;
struct json_object *opt42Obj = NULL;
struct json_object *opt43Obj = NULL;
struct json_object *opt120Obj = NULL;
struct json_object *opt121Obj = NULL;
struct json_object *opt212Obj = NULL;
struct json_object *opt60Obj = NULL;
struct json_object *opt61Obj = NULL;
struct json_object *opt125Obj = NULL;
struct json_object *igmpObj = NULL;
struct json_object *mldObj = NULL;
struct json_object *natIntfObj = NULL;
struct json_object *dnsClientSrv4Obj = NULL;
struct json_object *dnsClientSrv6Obj = NULL;
struct json_object *dynamicDnsClientSrv4Obj = NULL;
struct json_object *dynamicDnsClientSrv6Obj = NULL;
struct json_object *routerObj = NULL;
struct json_object *v4FwdObj = NULL;
struct json_object *v6FwdObj = NULL;
//struct json_object *dynamicV4FwdObj = NULL;
//struct json_object *dynamicV6FwdObj = NULL;
struct json_object *v4AddrObj = NULL;
struct json_object *v6AddrObj = NULL;
struct json_object *dnsObj = NULL;

objIndex_t atmLinkIid = {0};
objIndex_t atmLinkQosIid = {0};
objIndex_t ptmLinkIid = {0};
objIndex_t ethIfaceIid = {0};
objIndex_t optIfaceIid = {0};
objIndex_t cellIfaceIid = {0};
objIndex_t ethLinkIid = {0};
objIndex_t vlanTermIid = {0};
objIndex_t pppIfaceIid = {0};
objIndex_t ipIfaceIid = {0};
objIndex_t v4ClientIid = {0};
objIndex_t opt42Iid = {0};
objIndex_t opt43Iid = {0};
objIndex_t opt120Iid = {0};
objIndex_t opt121Iid = {0};
objIndex_t opt212Iid = {0};
objIndex_t opt60Iid = {0};
objIndex_t opt61Iid = {0};
objIndex_t opt125Iid = {0};
objIndex_t igmpIid = {0};
objIndex_t mldIid = {0};
objIndex_t natIntfIid = {0};
objIndex_t ripIid = {0};
objIndex_t dnsClientSrv4Iid = {0};
objIndex_t dnsClientSrv6Iid = {0};
objIndex_t dynamicDnsClientSrv4Iid = {0};
objIndex_t dynamicDnsClientSrv6Iid = {0};
objIndex_t routerIid = {0};
objIndex_t v4FwdIid = {0};
objIndex_t v6FwdIid = {0};
objIndex_t v4AddrIid = {0};
objIndex_t v6AddrIid = {0};
objIndex_t dnsIid = {0};

time_t t1, t2;

void zcfgFeDalShowWan(struct json_object *Jarray){
	int i, len = 0;
	struct json_object *obj, *obj_idx = NULL;
	const char *mode = NULL;
	const char *encapsulation = NULL;
	const char *vlanpriority = NULL;
	const char *vlanID = NULL;
	const char *igmpEnable = NULL;
	const char *nat = NULL;
	const char *defaultgateway = NULL;
	const char *dnsServer = NULL, *dns6Server =NULL;
	char *primaryDns = NULL, *secondaryDns = NULL, *primary6Dns = NULL, *secondary6Dns = NULL;
	

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	len = json_object_array_length(Jarray);
	obj_idx = json_object_array_get_idx(Jarray, 0);
	if(len == 1 && json_object_get_boolean(json_object_object_get(obj_idx, "ShowDetail"))){
		if(json_object_get_boolean(json_object_object_get(obj_idx, "ipDnsStatic"))){
			dnsServer = json_object_get_string(json_object_object_get(obj_idx, "DNSServer"));
			primaryDns = strtok_r(dnsServer, ",", &secondaryDns);
		}
		if(json_object_get_boolean(json_object_object_get(obj_idx, "ip6DnsStatic"))){
			dns6Server = json_object_get_string(json_object_object_get(obj_idx, "DNS6Server"));
			primary6Dns = strtok_r(dns6Server, ",", &secondary6Dns);
		}
		printf("General: \n");
		printf("%-2s %-20s %-10s \n", "", "Active", json_object_get_string(json_object_object_get(obj_idx, "Enable")));
		printf("%-2s %-20s %-10s \n", "", "Name", json_object_get_string(json_object_object_get(obj_idx, "Name")));
		if(!strcmp("ATM",json_object_get_string(json_object_object_get(obj_idx, "Type"))))
			printf("%-2s %-20s %-10s \n", "", "Type", "ADSL over ATM.");
		else if(!strcmp("PTM",json_object_get_string(json_object_object_get(obj_idx, "Type"))))
			printf("%-2s %-20s %-10s \n", "", "Type", "ADSL/VDSL over PTM.");
		else if(!strcmp("ETH",json_object_get_string(json_object_object_get(obj_idx, "Type"))))
			printf("%-2s %-20s %-10s \n", "", "Type", "Ethnet.");
		if(!strcmp("IP_Routed",json_object_get_string(json_object_object_get(obj_idx, "Mode"))))
			printf("%-2s %-20s %-10s \n", "", "Mode", "Routing");
		else if(!strcmp("IP_Bridged",json_object_get_string(json_object_object_get(obj_idx, "Mode"))))
			printf("%-2s %-20s %-10s \n", "", "Mode", "Bridge");
		if(!strcmp("IP_Routed",json_object_get_string(json_object_object_get(obj_idx, "Mode")))){
			printf("%-2s %-20s %-10s \n", "", "Mode", json_object_get_string(json_object_object_get(obj_idx, "Encapsulation")));
			printf("%-2s %-20s %-10s \n", "", "IPv4/IPv6 Mode", json_object_get_string(json_object_object_get(obj_idx, "ipMode")));
			printf("%-2s %-20s %-10s \n", "", "Static IP", json_object_get_string(json_object_object_get(obj_idx, "ipStatic")));
			if(json_object_get_boolean(json_object_object_get(obj_idx, "ipStatic"))){
				printf("%-2s %-20s %-10s \n", "", "IP Address", json_object_get_string(json_object_object_get(obj_idx, "IPAddress")));
				printf("%-2s %-20s %-10s \n", "", "Subnet Mask", json_object_get_string(json_object_object_get(obj_idx, "SubnetMask")));
				printf("%-2s %-20s %-10s \n", "", "Gateway IP Address", json_object_get_string(json_object_object_get(obj_idx, "GatewayIPAddress")));
				printf("%-2s %-20s %-10s \n", "", "DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ipDnsStatic")));
				printf("%-2s %-25s %-10s \n", "", "Primary DNS Server", primaryDns);
				printf("%-2s %-25s %-10s \n", "", "Secondary DNS Server", secondaryDns);
			}
			else if(json_object_get_boolean(json_object_object_get(obj_idx, "ipDnsStatic"))){
				printf("%-2s %-20s %-10s \n", "", "DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ipDnsStatic")));
				printf("%-2s %-25s %-10s \n", "", "Primary DNS Server", primaryDns);
				printf("%-2s %-25s %-10s \n", "", "Secondary DNS Server", secondaryDns);
			}
			else
				printf("%-2s %-20s %-10s \n", "", "DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ipDnsStatic")));
		}
		if(!strcmp("ATM",json_object_get_string(json_object_object_get(obj_idx, "Type")))){
			printf("%-2s %-20s %-10s \n", "", "VPI/VCI", json_object_get_string(json_object_object_get(obj_idx, "vpivci")));
			if(!strcmp("LLC",json_object_get_string(json_object_object_get(obj_idx, "AtmEncapsulation"))))
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "LLC/SNAP-BRIDGING");
			else if(!strcmp("VCMUX",json_object_get_string(json_object_object_get(obj_idx, "AtmEncapsulation"))))
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "VC/MUX");
			if(!strcmp("UBR",json_object_get_string(json_object_object_get(obj_idx, "QoSClass"))))
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "UBR without PCR");
			else if(!strcmp("CBR",json_object_get_string(json_object_object_get(obj_idx, "QoSClass")))){
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "CBR");
				printf("%-4s %-20s %-10s \n", "", "Peak Cell Rate [cells/s]", json_object_get_string(json_object_object_get(obj_idx, "atmPeakCellRate")));
			}
			else if(!strcmp("VBR-nrt",json_object_get_string(json_object_object_get(obj_idx, "QoSClass")))){
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "Non Realtime VBR");
				printf("%-4s %-20s %-10s \n", "", "Peak Cell Rate [cells/s]", json_object_get_string(json_object_object_get(obj_idx, "atmPeakCellRate")));
				printf("%-4s %-20s %-10s \n", "", "Sustainable Cell Rate", json_object_get_string(json_object_object_get(obj_idx, "atmSustainedCellRate")));
				printf("%-4s %-20s %-10s \n", "", "Maximum Burst Size [cells]", json_object_get_string(json_object_object_get(obj_idx, "atmMaxBurstSize")));
			}
			else if(!strcmp("VBR-rt",json_object_get_string(json_object_object_get(obj_idx, "QoSClass")))){
				printf("%-2s %-20s %-10s \n", "", "Encapsulation", "Realtime VBR");
				printf("%-4s %-20s %-10s \n", "", "Peak Cell Rate [cells/s]", json_object_get_string(json_object_object_get(obj_idx, "atmPeakCellRate")));
				printf("%-4s %-20s %-10s \n", "", "Sustainable Cell Rate", json_object_get_string(json_object_object_get(obj_idx, "atmSustainedCellRate")));
				printf("%-4s %-20s %-10s \n", "", "Maximum Burst Size [cells]", json_object_get_string(json_object_object_get(obj_idx, "atmMaxBurstSize")));
			}
		}
		if(!strncmp(json_object_get_string(json_object_object_get(obj_idx, "Encapsulation")),"PPP",3)){
			printf("PPP Information \n");
			printf("%-2s %-25s %-10s \n", "", "PPP User Name", json_object_get_string(json_object_object_get(obj_idx, "pppUsername")));
			printf("%-2s %-25s %-10s \n", "", "PPP Password", json_object_get_string(json_object_object_get(obj_idx, "pppPassword")));
			if(!strcmp("AlwaysOn",json_object_get_string(json_object_object_get(obj_idx, "ConnectionTrigger"))))
				printf("%-2s %-25s %-10s \n", "", "PPP Connection Trigger", "Auto Connect");
			else if(!strcmp("OnDemand",json_object_get_string(json_object_object_get(obj_idx, "ConnectionTrigger")))){
				printf("%-2s %-25s %-10s \n", "", "PPP Connection Trigger", "On Demand");
				printf("%-2s %-25s %-10s \n", "", "Idle Timeout(sec.)", json_object_get_string(json_object_object_get(obj_idx, "IdleDisconnectTime")));
			}
			printf("%-2s %-25s %-10s \n", "", "PPPoE Passthrough", json_object_get_string(json_object_object_get(obj_idx, "pppoePassThrough")));
		}
		printf("VLAN: \n");
		printf("%-2s %-20s %-10s \n", "", "Active :", json_object_get_string(json_object_object_get(obj_idx, "VLANEnable")));
		if(!strcmp("-1",json_object_get_string(json_object_object_get(obj_idx, "VLANPriority"))))
			printf("%-2s %-20s \n", "", "802.1p :");
		else
			printf("%-2s %-20s %-10s \n", "", "802.1p :", json_object_get_string(json_object_object_get(obj_idx, "VLANPriority")));
		if(!strcmp("-1",json_object_get_string(json_object_object_get(obj_idx, "VLANID"))))
			printf("%-2s %-20s \n", "", "802.1q :");
		else 
			printf("%-2s %-20s %-10s \n", "", "802.1q :", json_object_get_string(json_object_object_get(obj_idx, "VLANID")));
		printf("%-20s %-10s \n", "MTU :", json_object_get_string(json_object_object_get(obj_idx, "MaxMTUSize")));
		printf("Routing Feature: \n");
		printf("%-2s %-30s %-10s \n", "", "NAT Enable :", json_object_get_string(json_object_object_get(obj_idx, "NatEnable")));
		if(json_object_get_boolean(json_object_object_get(obj_idx, "NatEnable")))
			printf("%-2s %-30s %-10s \n", "", "Fullcone NAT Enable :", json_object_get_string(json_object_object_get(obj_idx, "FullConeEnabled")));
		printf("%-2s %-30s %-10s \n", "", "IGMP Proxy Enable :", json_object_get_string(json_object_object_get(obj_idx, "IGMPEnable")));
		printf("%-2s %-30s %-10s \n", "", "Apply as Default Gateway :", json_object_get_string(json_object_object_get(obj_idx, "sysGwEnable")));
		if(!json_object_get_boolean(json_object_object_get(obj_idx, "ipStatic"))){
			printf("DHCPC Options: \n");
			printf("%-2s %-20s %-10s \n", "", "options43", json_object_get_string(json_object_object_get(obj_idx, "option43Enable")));
			printf("%-2s %-20s %-10s \n", "", "options60", json_object_get_string(json_object_object_get(obj_idx, "option60Enable")));
			printf("%-2s %-20s %-10s \n", "", "options61", json_object_get_string(json_object_object_get(obj_idx, "option61Enable")));
			printf("%-2s %-20s %-10s \n", "", "options120", json_object_get_string(json_object_object_get(obj_idx, "option120Enable")));
			printf("%-2s %-20s %-10s \n", "", "options121", json_object_get_string(json_object_object_get(obj_idx, "option121Enable")));
			printf("%-2s %-20s %-10s \n", "", "options125", json_object_get_string(json_object_object_get(obj_idx, "option125Enable")));
		}
		printf("IPv6 Address: \n");
		printf("%-2s %-20s %-10s \n", "", "IPv6 Address", json_object_get_string(json_object_object_get(obj_idx, "ip6Static")));
		if(json_object_get_boolean(json_object_object_get(obj_idx, "ip6Static"))){
			printf("%-2s %-20s %-10s \n", "", "Static IPv6 Address", json_object_get_string(json_object_object_get(obj_idx, "IP6Address")));
			printf("%-2s %-20s %-10s \n", "", "IPv6 Default Gateway", json_object_get_string(json_object_object_get(obj_idx, "NextHop")));
			printf("%-2s %-20s %-10s \n", "", "IPv6 DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ip6DnsStatic")));
			printf("%-2s %-25s %-10s \n", "", "Primary DNS Server", primary6Dns);
			printf("%-2s %-25s %-10s \n", "", "Secondary DNS Server", secondary6Dns);
		}
		else if(json_object_get_boolean(json_object_object_get(obj_idx, "ipDnsStatic"))){
			printf("%-2s %-20s %-10s \n", "", "IPv6 DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ip6DnsStatic")));
			printf("%-2s %-25s %-10s \n", "", "Primary DNS Server", primary6Dns);
			printf("%-2s %-25s %-10s \n", "", "Secondary DNS Server", secondary6Dns);
		}
		else
			printf("%-2s %-20s %-10s \n", "", "IPv6 DNS Server", json_object_get_string(json_object_object_get(obj_idx, "ip6DnsStatic")));
		printf("IPv6 Routing Feature: \n");
		printf("%-2s %-30s %-10s \n", "", "MLD Proxy Enable", json_object_get_string(json_object_object_get(obj_idx, "MLDEnable")));
		printf("%-2s %-30s %-10s \n", "", "Apply as Default Gateway", json_object_get_string(json_object_object_get(obj_idx, "sysGw6Enable")));
		
		

	}
	else{
		printf("%-10s %-30s %-10s %-12s %-15s %-8s %-8s %-12s %-5s %-17s\n",
			    "Index", "Name", "Type", "Mode", "Encapsulation", "802.1p", "802.1q", "IGMP Proxy", "NAT", "Default Gateway");
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "Mode")),"IP_Routed")){
			mode = "Routing";
			encapsulation = json_object_get_string(json_object_object_get(obj, "Encapsulation"));
		}else{
			mode = "Bridge";
			encapsulation = "Bridge";
		}
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "VLANPriority")),"-1"))
			vlanpriority = "N/A";
		else
			vlanpriority = json_object_get_string(json_object_object_get(obj, "VLANPriority"));
		if(!strcmp(json_object_get_string(json_object_object_get(obj, "VLANID")),"-1"))
			vlanID = "N/A";
		else
			vlanID = json_object_get_string(json_object_object_get(obj, "VLANID"));
		if(json_object_get_boolean(json_object_object_get(obj, "IGMPEnable")))
			igmpEnable = "Y";
		else
			igmpEnable = "N";
		if(json_object_get_boolean(json_object_object_get(obj, "NatEnable")))
			nat = "Y";
		else
			nat = "N";
		if(json_object_get_boolean(json_object_object_get(obj, "sysGwEnable")))
			defaultgateway = "Y";
		else
			defaultgateway = "N";

			printf("%-10s %-30s %-10s %-12s %-15s %-8s %-8s %-12s %-5s %-17s\n",
				json_object_get_string(json_object_object_get(obj, "index")),
		   json_object_get_string(json_object_object_get(obj, "Name")),
		   json_object_get_string(json_object_object_get(obj, "Type")),
			  	mode, encapsulation, vlanpriority, vlanID, igmpEnable, nat, defaultgateway);
		}
	}
	
}

int charStrToHexStr(char *str, char *hex){
	int i;
	for(i=0;i<strlen(str);i++){
		char a = str[i];
		int b = (int)a;
		char c[2]={0};
		sprintf(c,"%x%x",(b/16),(b%16));
		strcat(hex,c);
	}
	return 1;
}
int intToHexStr(int num, char *hex){
	if(num>=(16*16)||num<0)
		return 0;
	sprintf(hex,"%x%x",(num/16),(num%16));
	return 1;
}
void opt125ValGet(char *val){
	struct json_object *devInfoObj = NULL;
	objIndex_t objIid;
	char tmpHexStr[256] = {0};
	char ouiHexStr[256] = {0};
	char snHexStr[256] = {0};
	char productHexStr[256] = {0};
	int len = 0, dataLen = 0, lenOui = 0, lenSn = 0, lenProduct = 0;

	IID_INIT(objIid);
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_INFO, &objIid, &devInfoObj) == ZCFG_SUCCESS){
		lenOui = strlen(json_object_get_string(json_object_object_get(devInfoObj, "ManufacturerOUI")));
		lenSn = strlen(json_object_get_string(json_object_object_get(devInfoObj, "SerialNumber")));
		lenProduct = strlen(json_object_get_string(json_object_object_get(devInfoObj, "ProductClass")));
		charStrToHexStr(json_object_get_string(json_object_object_get(devInfoObj, "ManufacturerOUI")), ouiHexStr);
		charStrToHexStr(json_object_get_string(json_object_object_get(devInfoObj, "SerialNumber")), snHexStr);
		charStrToHexStr(json_object_get_string(json_object_object_get(devInfoObj, "ProductClass")), productHexStr);

		/* entprise(4 bytes) + dataLen(1 byte) + code1(1 byte) + len1(1 byte) + oui(string) + code2 + len2 + sn(string) + code3 + len3 + product(string) */
		len = 4 + 1 + 1 + 1 + lenOui + 1 + 1 + lenSn + 1 + 1 + lenProduct;
		/* -entiprise(4 bytes) - dataLen(1 byte)*/
		dataLen = len - 4 - 1; 

		/* Follow GUI: option125Value = 125 + len + entNum + dataLen + codeOui + lenOui + oui + codeSn + lenSn + sn + codeProduct + lenProduct + product */
		sprintf(tmpHexStr,"%02x%02x%08x%02x%02x%02x%s%02x%02x%s%02x%02x%s",125,len,3561,dataLen,1,lenOui,ouiHexStr,2,lenSn,snHexStr,3,lenProduct,productHexStr);
		strcat(val,tmpHexStr);
		
	}
	if(devInfoObj){
        zcfgFeJsonObjFree(devInfoObj);
    }
}
int opt61ValEncode(char *iaid, char *duid, char *encVal, int tag){
	if(encVal)
		strcpy(encVal,"");

	//*(iaid+4)='\0';
	
	int strLen = 1+4+2+(strlen(duid)/2);
	char tagStr[2]={0};
	char hexLen[2]={0};

	intToHexStr(tag,tagStr);
	strcat(encVal,tagStr);

	intToHexStr(strLen,hexLen);
	strcat(encVal,hexLen);

	strcat(encVal,"ff");
	strcat(encVal,iaid);
	strcat(encVal,"0002");
	strcat(encVal,duid);

	return 1;
}

int opt60ValEncode(char *val, char *encVal, int tag){

	if(encVal)
		strcpy(encVal,"");

	int strLen = strlen(val);
	char tagStr[2]={0};
	char hexLen[2]={0};
	char charStr[256]={0};
	char hexStr[256]={0};

	intToHexStr(tag,tagStr);
	strcat(encVal,tagStr);

	intToHexStr(strLen,hexLen);
	strcat(encVal,hexLen);

	strncpy(charStr,val,strLen);
	*(charStr+strLen)='\0';
	charStrToHexStr(charStr,hexStr);
	strcat(encVal,hexStr);

	return 1;
}

int hexStrToCharStr(char *hex, char* str){
	char a[2]={0};
	int i=0;
	int hexTokLen = 2;
	while(1){
		if(i>=strlen(hex)){
			break;
		}
		strncpy(a,hex+i,hexTokLen);
		*(a+hexTokLen)='\0';
		int hh = (int)strtol(a,NULL,16);
		char t[5]={0};
		sprintf(t,"%c",(char)hh);
		strcat(str,t);
		i=i+hexTokLen;
	}
	return 1;
}

int opt60ValDecode(char *val, char *decVal){
	char hexStr[256]={0};
	char charStr[256]={0};
	if(strlen(val)<=4){
		strcpy(decVal,"");
		return 1;
	}
	if(decVal)
		strcpy(decVal,"");
	strncpy(hexStr,val+4,(strlen(val)-4));
	hexStrToCharStr(hexStr,charStr);
	strcpy(decVal,charStr);
	return 1;
}

int opt61ValDecode(char *val, char *iaid, char *duid){
	int duidLen = strlen(val)-18;
	strncpy(iaid,val+6,8);
	strncpy(duid,val+18,duidLen);
	return 1;
}

//the length of delim must be 1 or function may not work well
bool genNewInterface(char *delim, char *newInterface, const char *oldInterface, const char *targetIntf, bool add){
	bool changed = false, firstEnter = true, findTarget = false;
	char buff[512] = {0}, *intf = NULL, *tmp_ptr = NULL;

	strcpy(buff, oldInterface);
	intf = strtok_r(buff, delim, &tmp_ptr);
	while(intf != NULL){
		if(!strcmp(intf, targetIntf))
			findTarget = true;
		intf = strtok_r(NULL, delim, &tmp_ptr);
	}
	
	if(add && !findTarget){
		strcpy(newInterface, oldInterface);
		if(newInterface[0] != '\0' && newInterface[strlen(newInterface)-1] != *delim)
			strcat(newInterface, delim);
		strcat(newInterface, targetIntf);
		changed = true;
	}
	else if(!add && findTarget){
		tmp_ptr = NULL;
		intf = NULL;
		strcpy(buff, oldInterface);
			
		intf = strtok_r(buff, delim, &tmp_ptr);
		while(intf != NULL){
			if(*intf != '\0' && !strstr(intf, targetIntf)){
				if(!firstEnter && intf != NULL){
					strcat(newInterface, delim);
				}
				strcat(newInterface, intf);
				firstEnter = false;
			}
			intf = strtok_r(NULL, delim, &tmp_ptr);
		}
		changed = true;
	}

	return changed;
}

static zcfgRet_t addDefaultParam(struct json_object *Jobj, char *replyMsg){

	if(Type == NULL){
		json_object_object_add(Jobj, "Type", json_object_new_string("PTM"));
		Type = json_object_get_string(json_object_object_get(Jobj, "Type"));
	}
	else if(strcmp(Type, "ATM") && 
			strcmp(Type, "PTM") && 
			strcmp(Type, "ETH") && 
			strcmp(Type, "GPON") &&
			strcmp(Type, "CELL")){
		return ZCFG_INVALID_PARAM_VALUE;
	}

	if(!strcmp(Type, "ATM")){
		if(json_object_object_get(Jobj, "vpivci") == NULL){
			if(replyMsg)
				strcpy(replyMsg, "vpivci is Mandatory for ATM WAN");
			return ZCFG_INVALID_ARGUMENTS;
		}
		
		if(json_object_object_get(Jobj, "QoSClass") == NULL)
			json_object_object_add(Jobj, "QoSClass", json_object_new_string("UBR"));
	}

	if(Mode == NULL){
		json_object_object_add(Jobj, "Mode", json_object_new_string("IP_Routed"));
		Mode = json_object_get_string(json_object_object_get(Jobj, "Mode"));
	}
	else if(strcmp(Mode, "IP_Routed") && strcmp(Mode, "IP_Bridged")){
		return ZCFG_INVALID_PARAM_VALUE;
	}

	if(json_object_get_boolean(json_object_object_get(Jobj, "VLANEnable")) == false){
		json_object_object_add(Jobj, "VLANID", json_object_new_int(-1));
		json_object_object_add(Jobj, "VLANPriority", json_object_new_int(-1));
	}
	
	if(!strcmp(Mode, "IP_Bridged")){
		return ZCFG_SUCCESS;
	}

	if(Encapsulation == NULL || *Encapsulation == '\0'){
		json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
		Encapsulation = json_object_get_string(json_object_object_get(Jobj, "Encapsulation"));
	}
	else if(strcmp(Encapsulation, "IPoE") &&
			strcmp(Encapsulation, "IPoA") &&
			strcmp(Encapsulation, "PPPoE") &&
			strcmp(Encapsulation, "PPPoA")){
		return ZCFG_INVALID_PARAM_VALUE;
	}

	if(!strcmp(Encapsulation, "PPPoE") || !strcmp(Encapsulation, "PPPoA")){
		if(json_object_object_get(Jobj, "ConnectionTrigger") == NULL)
			json_object_object_add(Jobj, "ConnectionTrigger", json_object_new_string("AlwaysOn"));
		if(json_object_object_get(Jobj, "MaxMTUSize") == NULL)
			json_object_object_add(Jobj, "MaxMTUSize", json_object_new_int(1492));
	}
	else{ //IPoE/IPoA
		if(json_object_object_get(Jobj, "MaxMTUSize") == NULL)
			json_object_object_add(Jobj, "MaxMTUSize", json_object_new_int(1500));
	}

	if(ipMode == NULL){//IPv4/DualStack/IPv6
		json_object_object_add(Jobj, "ipMode", json_object_new_string("IPv4"));
		ipMode = json_object_get_string(json_object_object_get(Jobj, "ipMode"));
	}
	else if(strcmp(ipMode, "IPv4") &&
			strcmp(ipMode, "IPv6") &&
			strcmp(ipMode, "DualStack")){
		return ZCFG_INVALID_PARAM_VALUE;
	}
	if(json_object_object_get(Jobj, "NatEnable") == NULL)
		json_object_object_add(Jobj, "NatEnable", json_object_new_boolean(true));
	
	if(!strcmp(ipMode, "IPv4")){
		if(Enable_6RD){
			if(v6RD_Type == NULL){
				json_object_object_add(Jobj, "v6RD_Type", json_object_new_string("dhcp"));
				v6RD_Type = json_object_get_string(json_object_object_get(Jobj, "v6RD_Type"));
			}
			else if(strcmp(v6RD_Type, "dhcp") && strcmp(v6RD_Type, "static")){
				return ZCFG_INVALID_PARAM_VALUE;
			}

			if(!strcmp(v6RD_Type, "dhcp"))
				json_object_object_add(Jobj, "option212Enable", json_object_new_boolean(true));
		}
	}
	
	if(!strcmp(ipMode, "IPv6")){//dslite
		if(json_object_object_get(Jobj, "DSLiteType") == NULL)
			json_object_object_add(Jobj, "DSLiteType", json_object_new_string("static"));
			
		if(json_object_object_get(Jobj, "DSLiteRelayIPv6Addresses") == NULL)
			json_object_object_add(Jobj, "DSLiteRelayIPv6Addresses", json_object_new_string(""));
	}

	return ZCFG_SUCCESS;
}

static bool isXTMLinkHasReference(struct json_object *Jobj){
	char atmLinkPath[16] = {0};
	const char *LowerLayer = NULL;
	zcfg_offset_t oid;
	objIndex_t iid;
	struct json_object *Obj;
	zcfg_offset_t currRefOid; 
	objIndex_t currRefIid;

	dbg_printf("isXTMLinkHasReference\n");

	if(!strcmp(CurrType, "ATM") && (atmLinkObj != NULL))
		sprintf(atmLinkPath, "ATM.Link.%u", atmLinkIid.idx[0]);
	else if(!strcmp(CurrType, "PTM") && (ptmLinkObj != NULL))
		sprintf(atmLinkPath, "PTM.Link.%u", ptmLinkIid.idx[0]);
	else
		return true;

	dbg_printf("atmLinkPath=%s\n", atmLinkPath);
	
	IID_INIT(currRefIid);
	if(!strcmp(CurrEncapsulation, "PPPoA")){
		currRefOid = RDM_OID_PPP_IFACE; 
		memcpy(&currRefIid, &pppIfaceIid, sizeof(objIndex_t));
	}
	else if(!strcmp(CurrEncapsulation, "IPoA")){
		currRefOid = RDM_OID_IP_IFACE; 
		memcpy(&currRefIid, &ipIfaceIid, sizeof(objIndex_t));
	}
	else{//IPoE or PPPoE
		currRefOid = RDM_OID_ETH_LINK; 
		memcpy(&currRefIid, &ethLinkIid, sizeof(objIndex_t));
	}

	dbg_printf("currRefOid=%d\n", currRefOid);

	oid = RDM_OID_ETH_LINK;
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &Obj) == ZCFG_SUCCESS) {
		LowerLayer = json_object_get_string(json_object_object_get(Obj, "LowerLayers"));
		if((LowerLayer != NULL) && !strcmp(LowerLayer, atmLinkPath)){
			if((oid != currRefOid) || memcmp(&iid, &currRefIid, sizeof(objIndex_t))){
				json_object_put(Obj);
				dbg_printf("line=%d\n", __LINE__);
				return true;
			}
		}
		json_object_put(Obj);
	}

	if(!strcmp(CurrType, "PTM")){
		printf("isXTMLinkHasReference false\n");
		return false;
	}

	oid = RDM_OID_PPP_IFACE;
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &Obj) == ZCFG_SUCCESS) {
		LowerLayer = json_object_get_string(json_object_object_get(Obj, "LowerLayers"));
		if((LowerLayer != NULL) && !strcmp(LowerLayer, atmLinkPath)){
			if((oid != currRefOid) || memcmp(&iid, &currRefIid, sizeof(objIndex_t))){
				json_object_put(Obj);
				dbg_printf("line=%d\n", __LINE__);
				return true;
			}
		}
		json_object_put(Obj);
	}

	oid = RDM_OID_IP_IFACE;
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &Obj) == ZCFG_SUCCESS) {
		LowerLayer = json_object_get_string(json_object_object_get(Obj, "LowerLayers"));
		if((LowerLayer != NULL) && !strcmp(LowerLayer, atmLinkPath)){
			if((oid != currRefOid) || memcmp(&iid, &currRefIid, sizeof(objIndex_t))){
				json_object_put(Obj);
				dbg_printf("line=%d\n", __LINE__);
				return true;
			}
		}
		json_object_put(Obj);
	}

	dbg_printf("isXTMLinkHasReference false\n");
	return false;
	
}

zcfgRet_t editAtmObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool isNewAtmObj = false;
	const char *vpivci = json_object_get_string(json_object_object_get(Jobj, "vpivci"));
	const char *currVpivci;

	if(isAdd && vpivci == NULL){ //add case
		printf("need vpivci\n");
		return ZCFG_INVALID_PARAM_VALUE;
	}
	
	if(!isAdd){ //edit case
		currVpivci = json_object_get_string(json_object_object_get(atmLinkObj, "DestinationAddress"));
		if(vpivci != NULL && strcmp(currVpivci, vpivci)){
			if(!isXTMLinkHasReference(Jobj)){
				//ensure the qos restart after edit vpivci when linktype is EOA
				if(ethLinkObj){
					json_object_object_add(ethLinkObj, "Enable", json_object_new_boolean(false));
					zcfgFeObjJsonSetWithoutApply(RDM_OID_ETH_LINK, &ethLinkIid, ethLinkObj, NULL);
					json_object_object_add(ethLinkObj, "Enable", json_object_new_boolean(true));
				} else {
					printf("RDM_OID_ETH_LINK Object is not exist!\n");
				}
				zcfgFeObjJsonDel(RDM_OID_ATM_LINK, &atmLinkIid, NULL);
			}
			zcfgFeJsonObjFree(atmLinkObj);
		}
		else{
			//do nothing. If vpivci == NULL or vpivci == currVpivci, it means vci/vpi are not changed
		}
	}
	
	if(!isAdd && atmLinkObj != NULL){
		//edit case and vci/vpi are not changed, no need to re-get atm object.
	}
	else if(!getSpecificObj(RDM_OID_ATM_LINK, "DestinationAddress", json_type_string, vpivci, &atmLinkIid, &atmLinkObj)){
		//edit or add case, need new atmLink obj.
		IID_INIT(atmLinkIid);
		zcfgFeObjJsonAdd(RDM_OID_ATM_LINK, &atmLinkIid, NULL);
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
		isNewAtmObj = true;
		replaceParam(atmLinkObj, "DestinationAddress", Jobj, "vpivci");
	}
	sprintf(currLowerLayers, "ATM.Link.%u", atmLinkIid.idx[0]);

	if(atmLinkQosObj == NULL)
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkIid, &atmLinkQosObj);
	
	if(isNewAtmObj){ //set defalult values
		json_object_object_add(atmLinkObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(atmLinkObj, "Encapsulation", json_object_new_string("LLC"));
#ifdef SUPPORT_DSL_BONDING
		json_object_object_add(atmLinkObj, "LowerLayers", json_object_new_string("DSL.Channel.1,DSL.Channel.3,DSL.BondingGroup.1"));
#else
		json_object_object_add(atmLinkObj, "LowerLayers", json_object_new_string("DSL.Channel.1"));
#endif

		dbg_printf("%s: SetIpIfaceStackModified  \n", __FUNCTION__);
		if(ipIfaceObj)
			json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));
	}

	/* set atmQos object first */
	replaceParam(atmLinkQosObj, "QoSClass", Jobj, "QoSClass");
	replaceParam(atmLinkQosObj, "MaximumBurstSize", Jobj, "atmMaxBurstSize");
	replaceParam(atmLinkQosObj, "SustainableCellRate", Jobj, "atmSustainedCellRate");
	replaceParam(atmLinkQosObj, "PeakCellRate", Jobj, "atmPeakCellRate");
	memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));

	if(!isAdd && Mode == NULL){
		Mode = CurrMode;
	}
	if(!strcmp(Mode, "IP_Bridged")){
		json_object_object_add(atmLinkObj, "LinkType", json_object_new_string("EoA"));
	}
	else{ //IP_Routed
		if(!strcmp(Encapsulation, "PPPoE") || !strcmp(Encapsulation, "IPoE")){
			json_object_object_add(atmLinkObj, "LinkType", json_object_new_string("EoA"));
		}
		else{
			json_object_object_add(atmLinkObj, "LinkType", json_object_new_string(Encapsulation));
		}
	}
	replaceParam(atmLinkObj, "Encapsulation", Jobj, "AtmEncapsulation");

	return ret;
}

zcfgRet_t editPtmObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!isAdd){ //do nothing for edit case
		sprintf(currLowerLayers, "PTM.Link.%u", ptmLinkIid.idx[0]);
		if(ptmLinkObj) {zcfgFeJsonObjFree(ptmLinkObj);}
		return ret;
	}
		
	IID_INIT(ptmLinkIid);
	if(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_PTM_LINK, &ptmLinkIid, &ptmLinkObj) != ZCFG_SUCCESS){
		IID_INIT(ptmLinkIid);
		zcfgFeObjJsonAdd(RDM_OID_PTM_LINK, &ptmLinkIid, NULL);
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PTM_LINK, &ptmLinkIid, &ptmLinkObj);

		json_object_object_add(ptmLinkObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(ptmLinkObj, "X_ZYXEL_QueueShapingBurstSize", json_object_new_int(3000));
		json_object_object_add(ptmLinkObj, "X_ZYXEL_ConnectionMode", json_object_new_string("VlanMuxMode"));
		json_object_object_add(ptmLinkObj, "X_ZYXEL_SchedulerAlgorithm", json_object_new_string("WRR"));
		json_object_object_add(ptmLinkObj, "X_ZYXEL_EnableADSLPtm", json_object_new_boolean(true));
#ifdef SUPPORT_DSL_BONDING
		json_object_object_add(ptmLinkObj, "LowerLayers", json_object_new_string("DSL.Channel.2,DSL.Channel.4,DSL.BondingGroup.2"));
#else
		json_object_object_add(ptmLinkObj, "LowerLayers", json_object_new_string("DSL.Channel.2"));
#endif
	}
	sprintf(currLowerLayers, "PTM.Link.%u", ptmLinkIid.idx[0]);

	return ret;
}

#if 0
zcfgRet_t editGponObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	struct json_object *opticalIntfObj = NULL;
	int Enable = 1;

	if(getSpecificObj(RDM_OID_ONU, "Upstream", json_type_boolean, &Enable, &iid, &opticalIntfObj))
		sprintf(currLowerLayers, "Optical.Interface.%u", iid.idx[0]);
	else
		return ZCFG_INTERNAL_ERROR;

	
	json_object_put(opticalIntfObj);
	return ret;
}
#endif

zcfgRet_t editPhyLayerObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	int value;
	
	if(!strcmp(Type, "ATM")){
		ret = editAtmObjects(Jobj);
	}
	else if(!strcmp(Type, "PTM")){
		ret = editPtmObjects(Jobj);
	}
	else if(!strcmp(Type, "ETH")){
		value = 1;
		if(getSpecificObj(RDM_OID_ETH_IFACE, "Upstream", json_type_boolean, &value, &iid, NULL))
			sprintf(currLowerLayers, "Ethernet.Interface.%u", iid.idx[0]);
		else
			currLowerLayers[0] = '\0';
	}
	else if(!strcmp(Type, "GPON")){
		value = 1;
		if(getSpecificObj(RDM_OID_OPT_INTF, "Upstream", json_type_boolean, &value, &iid, NULL))
			sprintf(currLowerLayers, "Optical.Interface.%u", iid.idx[0]);
		else{
			printf("There is no GPON interface\n");
			ret = ZCFG_REQUEST_REJECT;
		}
	}
	else if(!strcmp(Type, "CELL")){
		sprintf(currLowerLayers, "Cellular.Interface.%u", cellIfaceIid.idx[0]);
	}
	//else if(!strcmp(Type, "GPON")){
	//	ret = editGponObjects(Jobj);
	//}

	return ret;
}

zcfgRet_t editEthLinkObject(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(Mode, "IP_Routed") && (!strcmp(Encapsulation, "IPoA") || !strcmp(Encapsulation, "PPPoA")))
		return ret;

	if(isAdd || ethLinkObj == NULL){
		IID_INIT(ethLinkIid);
		if((ret = zcfgFeObjJsonAdd(RDM_OID_ETH_LINK, &ethLinkIid, NULL)) != ZCFG_SUCCESS)
			return ret;
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj)) != ZCFG_SUCCESS)
			return ret;
	}

	json_object_object_add(ethLinkObj, "Enable", json_object_new_boolean(true));
	json_object_object_add(ethLinkObj, "LowerLayers", json_object_new_string(currLowerLayers));
	if(Type) {
		if(!strcmp(Type, "ETH")){
			json_object_object_add(ethLinkObj, "X_ZYXEL_OrigEthWAN", json_object_new_boolean(true));
		}
#ifdef ZYXEL_WAN_MAC
		if(!strcmp(Type, "ETH") || !strcmp(Type, "ATM") || !strcmp(Type, "PTM")){
			if(!strcmp(Mode, "IP_Routed"))
			{
				json_object_object_add(ethLinkObj, "X_ZYXEL_WanMacType", json_object_new_string(WanMacType));
				if (!strcmp(WanMacType, WAN_MAC_CLIENTIP)) {
					dbg_printf("%s, set WanMacIP\n", __FUNCTION__);
					if (WanMacIP != NULL)
						json_object_object_add(ethLinkObj, "X_ZYXEL_WanMacClientIP", json_object_new_string(WanMacIP));
				} else if (!strcmp(WanMacType, WAN_MAC_USER)) {
					dbg_printf("%s, set WanMacAddr\n", __FUNCTION__);
					if (WanMacAddr != NULL)
						json_object_object_add(ethLinkObj, "X_ZYXEL_WanMacAddr", json_object_new_string(WanMacAddr));
				}
			}
		}
#endif
	}
	
	sprintf(currLowerLayers, "Ethernet.Link.%u", ethLinkIid.idx[0]);
	
	return ret;
}

zcfgRet_t editVlanTermObject(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(Mode, "IP_Routed") && (!strcmp(Encapsulation, "IPoA") || !strcmp(Encapsulation, "PPPoA")))
		return ret;

#ifdef ZYXEL_WEB_GUI_SHOW_CELLWAN
	printf("[%d] %s Type: %s, CurrType: %s\n", __LINE__, __func__, Type, CurrType);
	if(strcmp(Type, "CELL") == 0)
		return ret;
#endif

	if(isAdd || vlanTermObj == NULL){
		IID_INIT(vlanTermIid);
		if((ret = zcfgFeObjJsonAdd(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, NULL)) != ZCFG_SUCCESS)
			return ret;
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, &vlanTermObj)) != ZCFG_SUCCESS)
			return ret;
	}
	
	json_object_object_add(vlanTermObj, "Enable", json_object_new_boolean(true));
	json_object_object_add(vlanTermObj, "LowerLayers", json_object_new_string(currLowerLayers));

#ifdef ZCFG_VMG8825_B50B_WIND	
	if ((json_object_get_boolean(json_object_object_get(Jobj, "VLANEnable")) == true) && !strcmp(Encapsulation, "PPPoE")){
		if ((strstr(json_object_get_string(json_object_object_get(Jobj, "pppUsername")), "megaoffice.it") != NULL) ||
			(strstr(json_object_get_string(json_object_object_get(Jobj, "pppUsername")), "megaoffice-ws.it") != NULL)){
			json_object_object_add(Jobj, "VLANPriority", json_object_new_int(1));
		}
	}
#endif //ZCFG_VMG8825_B50B_WIND
	
	replaceParam(vlanTermObj, "X_ZYXEL_VLANEnable", Jobj, "VLANEnable");
	replaceParam(vlanTermObj, "VLANID", Jobj, "VLANID");
	replaceParam(vlanTermObj, "X_ZYXEL_VLANPriority", Jobj, "VLANPriority");
	sprintf(currLowerLayers, "Ethernet.VLANTermination.%u", vlanTermIid.idx[0]);
	
	return ret;
}

zcfgRet_t editNatSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//objIndex_t iid;
	zcfg_offset_t oid;
	//struct json_object *obj = NULL;
	bool CurrNatEnabled=false, CurrFullConeEnabled=false;
	bool NatEnable=false, FullConeEnabled=false;
	struct json_object *pramJobj = NULL;
	if(natIntfObj != NULL){
		CurrNatEnabled = json_object_get_boolean(json_object_object_get(natIntfObj, "Enable"));
		CurrFullConeEnabled = json_object_get_boolean(json_object_object_get(natIntfObj, "X_ZYXEL_FullConeEnabled"));
	}

	pramJobj = json_object_object_get(Jobj, "NatEnable");
	if(pramJobj)
		NatEnable = json_object_get_boolean(pramJobj);
	else
		NatEnable = CurrNatEnabled;

	if(NatEnable){
		pramJobj = json_object_object_get(Jobj, "FullConeEnabled");
		if(pramJobj)
			FullConeEnabled = json_object_get_boolean(pramJobj);
		else
			FullConeEnabled = CurrFullConeEnabled;
	}

	dbg_printf("CurrNatEnabled=%d NatEnable=%d\n", CurrNatEnabled, NatEnable);
	dbg_printf("CurrFullConeEnabled=%d FullConeEnabled=%d\n", CurrFullConeEnabled, FullConeEnabled);

	if(CurrNatEnabled && NatEnable){ //enable to enable
		if(FullConeEnabled != CurrFullConeEnabled){
			json_object_object_add(natIntfObj, "X_ZYXEL_FullConeEnabled", json_object_new_boolean(FullConeEnabled));
		}else{
			if(natIntfObj) {zcfgFeJsonObjFree(natIntfObj);}
		}
	}
	else if(CurrNatEnabled && !NatEnable){ ////enable to disable
		json_object_object_add(natIntfObj, "Enable", json_object_new_boolean(false));
		json_object_object_add(natIntfObj, "FullConeEnabled", json_object_new_boolean(false));

	}
	else if(!CurrNatEnabled && NatEnable){ //disable to enable or add WAN case
		oid = RDM_OID_NAT_INTF_SETTING;
		if(natIntfObj == NULL && !getSpecificObj(oid, "Interface", json_type_string, "", &natIntfIid, &natIntfObj)){
			IID_INIT(natIntfIid);
			zcfgFeObjJsonAdd(oid, &natIntfIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &natIntfIid, &natIntfObj);
		}
		json_object_object_add(natIntfObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(natIntfObj, "Interface", json_object_new_string(ipIfacePath));
		replaceParam(natIntfObj, "X_ZYXEL_FullConeEnabled", Jobj, "FullConeEnabled");

	}
	else{//disable to disable (or add WAN case without enable NAT, so need to add rip obj)
		if(natIntfObj){zcfgFeJsonObjFree(natIntfObj);}

	}

	return ret;
}

zcfgRet_t editDnsSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	zcfg_offset_t oid;
	struct json_object *pramJobj = NULL;
	bool isIp4DnsObjAdd = false;
	bool ipDnsStatic=false, ip6DnsStatic=false;
	bool CurrIpDnsStatic=false, CurrIp6DnsStatic=false;
	const char *DNSServer;
	const char *CurrDNSServer;

	if(dnsClientSrv4Obj != NULL) CurrIpDnsStatic = true;
	if(dnsClientSrv6Obj != NULL) CurrIp6DnsStatic = true;	

	pramJobj = json_object_object_get(Jobj, "ipDnsStatic");
	if(!strcmp(ipMode, "IPv6"))
		ipDnsStatic = false;
	else if(pramJobj)
		ipDnsStatic = json_object_get_boolean(pramJobj);
	else
		ipDnsStatic = CurrIpDnsStatic;

	pramJobj = json_object_object_get(Jobj, "ip6DnsStatic");
	if(!strcmp(ipMode, "IPv4"))
		ip6DnsStatic = false;
	else if(pramJobj)
		ip6DnsStatic = json_object_get_boolean(pramJobj);
	else
		ip6DnsStatic = CurrIp6DnsStatic;
	
	dbg_printf("CurrIpDnsStatic=%d ipDnsStatic=%d\n", CurrIpDnsStatic, ipDnsStatic);
	dbg_printf("CurrIp6DnsStatic=%d ip6DnsStatic=%d\n", CurrIp6DnsStatic, ip6DnsStatic);

	if(CurrIpDnsStatic && ipDnsStatic){ //enable to enable
		DNSServer = json_object_get_string(json_object_object_get(Jobj, "DNSServer"));
		CurrDNSServer = json_object_get_string(json_object_object_get(dnsClientSrv4Obj, "DNSServer"));
		if(json_object_object_get(Jobj, "DNSServer")){
			if(strcmp(DNSServer, CurrDNSServer))
				json_object_object_add(dnsClientSrv4Obj, "DNSServer", json_object_new_string(DNSServer));
		}
		else{
			if(dnsClientSrv4Obj) {zcfgFeJsonObjFree(dnsClientSrv4Obj);}
		}
	}
	else if(CurrIpDnsStatic && !ipDnsStatic){ ////enable to disable
		//json_object_object_add(dnsClientSrv4Obj, "Enable", json_object_new_boolean(false));
		json_object_object_add(dnsClientSrv4Obj, "Interface", json_object_new_string(""));
	}
	else if(!CurrIpDnsStatic && ipDnsStatic){ //disable to enable
		if(dnsClientSrv4Obj) {zcfgFeJsonObjFree(dnsClientSrv4Obj);} //should not happened, just to make sure no memory leak.

		isIp4DnsObjAdd = true;
		oid = RDM_OID_DNS_CLIENT_SRV;
		if(!getSpecificObj(oid, "Interface", json_type_string, "", &dnsClientSrv4Iid, &dnsClientSrv4Obj)){
			IID_INIT(dnsClientSrv4Iid);
			zcfgFeObjJsonAdd(oid, &dnsClientSrv4Iid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &dnsClientSrv4Iid, &dnsClientSrv4Obj);
		}
		json_object_object_add(dnsClientSrv4Obj, "Enable", json_object_new_boolean(true));
		json_object_object_add(dnsClientSrv4Obj, "Interface", json_object_new_string(ipIfacePath));
		json_object_object_add(dnsClientSrv4Obj, "X_ZYXEL_Type", json_object_new_string("Static"));
		replaceParam(dnsClientSrv4Obj, "DNSServer", Jobj, "DNSServer");
	}
	else{//do nothing for disable to disable
		if(dnsClientSrv4Obj) {zcfgFeJsonObjFree(dnsClientSrv4Obj);}
	}

	if(CurrIp6DnsStatic && ip6DnsStatic){ //enable to enable
		DNSServer = json_object_get_string(json_object_object_get(Jobj, "DNS6Server"));
		CurrDNSServer = json_object_get_string(json_object_object_get(dnsClientSrv6Obj, "DNSServer"));
		if(strcmp(DNSServer, CurrDNSServer)){
			json_object_object_add(dnsClientSrv6Obj, "DNSServer", json_object_new_string(DNSServer));
		}else{
			if(dnsClientSrv6Obj) {zcfgFeJsonObjFree(dnsClientSrv6Obj);}
		}
	}
	else if(CurrIp6DnsStatic && !ip6DnsStatic){ ////enable to disable
		//json_object_object_add(dnsClientSrv6Obj, "Enable", json_object_new_boolean(false));
		json_object_object_add(dnsClientSrv6Obj, "Interface", json_object_new_string(""));
		
	}
	else if(!CurrIp6DnsStatic && ip6DnsStatic){ //disable to enable
		if(dnsClientSrv6Obj) {zcfgFeJsonObjFree(dnsClientSrv6Obj);} //should not happened, just to make sure no memory leak.
		oid = RDM_OID_DNS_CLIENT_SRV;
		if(isIp4DnsObjAdd){ // if v4dns obj was added before, do not try to get empty obj!
			IID_INIT(dnsClientSrv6Iid);
			zcfgFeObjJsonAdd(oid, &dnsClientSrv6Iid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &dnsClientSrv6Iid, &dnsClientSrv6Obj);
		}
		else if(!getSpecificObj(oid, "Interface", json_type_string, "", &dnsClientSrv6Iid, &dnsClientSrv6Obj)){
			IID_INIT(dnsClientSrv6Iid);
			zcfgFeObjJsonAdd(oid, &dnsClientSrv6Iid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &dnsClientSrv6Iid, &dnsClientSrv6Obj);
		}
		json_object_object_add(dnsClientSrv6Obj, "Enable", json_object_new_boolean(true));
		json_object_object_add(dnsClientSrv6Obj, "Interface", json_object_new_string(ipIfacePath));
		json_object_object_add(dnsClientSrv6Obj, "X_ZYXEL_Type", json_object_new_string("Static"));
		replaceParam(dnsClientSrv6Obj, "DNSServer", Jobj, "DNS6Server");
	}
	else{//do nothing for disable to disable
		if(dnsClientSrv6Obj) {zcfgFeJsonObjFree(dnsClientSrv6Obj);}
	}

	return ret;
}

zcfgRet_t addStaticAddrSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//objIndex_t iid;
	zcfg_offset_t oid;
	//struct json_object *obj = NULL;
	struct json_object *pramJobj = NULL;
	//const char *Encapsulation, *ipMode;
	bool ipStatic = false, ip6Static = false;

	/* static ipv4*/
	pramJobj=json_object_object_get(Jobj, "ipStatic");
	if(pramJobj)
		ipStatic = json_object_get_boolean(pramJobj);
	if(strcmp(ipMode, "IPv6") && (!strcmp(Encapsulation, "IPoA")|| ipStatic)){
		oid = RDM_OID_ROUTING_ROUTER_V4_FWD;
		if(!getSpecificObj(oid, "Interface", json_type_string, "", &v4FwdIid, &v4FwdObj)){
			IID_INIT(v4FwdIid);
			v4FwdIid.level = 1;
			v4FwdIid.idx[0] = 1;
			zcfgFeObjJsonAdd(oid, &v4FwdIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &v4FwdIid, &v4FwdObj);
		}
		json_object_object_add(v4FwdObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v4FwdObj, "Interface", json_object_new_string(ipIfacePath));
		replaceParam(v4FwdObj, "GatewayIPAddress", Jobj, "GatewayIPAddress");
		//if((ret = zcfgFeObjJsonSetWithoutApply(oid, &v4FwdIid, obj, NULL)) != ZCFG_SUCCESS)
		//	printf("set oid:%d fail, line=%d, ret=%d\n", oid, __LINE__, ret);
		//json_object_put(obj);

		oid = RDM_OID_IP_IFACE_V4_ADDR;
		IID_INIT(v4AddrIid);
		v4AddrIid.level = 1;
		sscanf(ipIfacePath, "IP.Interface.%hhu", &v4AddrIid.idx[0]);
		zcfgFeObjJsonAdd(oid, &v4AddrIid, NULL);
		zcfgFeObjJsonGetWithoutUpdate(oid, &v4AddrIid, &v4AddrObj);
		json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(true));
		replaceParam(v4AddrObj, "IPAddress", Jobj, "IPAddress");
		replaceParam(v4AddrObj, "SubnetMask", Jobj, "SubnetMask");
		//if((ret = zcfgFeObjJsonSetWithoutApply(oid, &v4AddrIid, obj, NULL)) != ZCFG_SUCCESS)
		//	printf("set oid:%d fail, line=%d, ret=%d\n", oid, __LINE__, ret);
		//json_object_put(obj);
	}

	/* static ipv6*/
	pramJobj=json_object_object_get(Jobj, "ip6Static");
	if(pramJobj)
		ip6Static = json_object_get_boolean(pramJobj);
	if(strcmp(ipMode, "IPv4") && ip6Static){
		oid = RDM_OID_ROUTING_ROUTER_V6_FWD;
		if(!getSpecificObj(oid, "Interface", json_type_string, "", &v6FwdIid, &v6FwdObj)){
			IID_INIT(v6FwdIid);
			v6FwdIid.level = 1;
			v6FwdIid.idx[0] = 1;
			zcfgFeObjJsonAdd(oid, &v6FwdIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &v6FwdIid, &v6FwdObj);
		}
		json_object_object_add(v6FwdObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v6FwdObj, "Interface", json_object_new_string(ipIfacePath));
		replaceParam(v6FwdObj, "NextHop", Jobj, "NextHop");
		//if((ret = zcfgFeObjJsonSetWithoutApply(oid, &v6FwdIid, v6FwdObj, NULL)) != ZCFG_SUCCESS)
		//	printf("set oid:%d fail, line=%d, ret=%d\n", oid, __LINE__, ret);
		//json_object_put(v6FwdObj);

		oid = RDM_OID_IP_IFACE_V6_ADDR;
		IID_INIT(v6AddrIid);
		v6AddrIid.level = 1;
		sscanf(ipIfacePath, "IP.Interface.%hhu", &v6AddrIid.idx[0]);
		zcfgFeObjJsonAdd(oid, &v6AddrIid, NULL);
		zcfgFeObjJsonGetWithoutUpdate(oid, &v6AddrIid, &v6AddrObj);
		json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(true));
		replaceParam(v6AddrObj, "IPAddress", Jobj, "IP6Address");
		//if((ret = zcfgFeObjJsonSetWithoutApply(oid, &v6AddrIid, v6AddrObj, NULL)) != ZCFG_SUCCESS)
		//	printf("set oid:%d fail, line=%d, ret=%d\n", oid, __LINE__, ret);
		//json_object_put(v6AddrObj);
	}

	return ret;
}


//This function is only for IP mode, not deal with static PPP address
zcfgRet_t editStaticAddrSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	zcfg_offset_t oid;
	struct json_object *pramJobj = NULL;
	bool ipStatic = false, ip6Static = false;
	const char *IPAddress, *SubnetMask, *GatewayIPAddress;
	const char *CurrIPAddress, *CurrSubnetMask, *CurrGatewayIPAddress;
	const char *IP6Address, *NextHop;
	const char *CurrIP6Address, *CurrNextHop;
	bool ipdnsmodify = false;

	pramJobj = json_object_object_get(Jobj, "ipStatic");
	if(!strcmp(ipMode, "IPv6"))
		ipStatic = false;
	else if(pramJobj)
		ipStatic = json_object_get_boolean(pramJobj);
	else
		ipStatic = CurrIpStatic;

	pramJobj = json_object_object_get(Jobj, "ip6Static");
	if(!strcmp(ipMode, "IPv4"))
		ip6Static = false;
	else if(pramJobj)
		ip6Static = json_object_get_boolean(pramJobj);
	else
		ip6Static = CurrIp6Static;

	if(strstr(Encapsulation, "PPP"))
		ipStatic = false;

	dbg_printf("CurrIpStatic=%d ipStatic=%d\n", CurrIpStatic, ipStatic);
	dbg_printf("CurrIp6Static=%d ip6Static=%d\n", CurrIp6Static, ip6Static);

	if(CurrIpStatic){
		if(v4AddrObj == NULL){
			IID_INIT(v4AddrIid);
			v4AddrIid.level = 1;
			sscanf(ipIfacePath, "IP.Interface.%hhu", &v4AddrIid.idx[0]);
			zcfgFeObjJsonAdd(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj);
		}
		json_object_object_add(v4AddrObj, "AddressingType", json_object_new_string("Static"));
	}

	//static v4ip
	if(CurrIpStatic && ipStatic){ //enable to enable
		//v4 addr
		IPAddress = json_object_get_string(json_object_object_get(Jobj, "IPAddress"));
		SubnetMask = json_object_get_string(json_object_object_get(Jobj, "SubnetMask"));
		GatewayIPAddress = json_object_get_string(json_object_object_get(Jobj, "GatewayIPAddress"));
		if(GatewayIPAddress == NULL){GatewayIPAddress = "";}
		
		CurrIPAddress = json_object_get_string(json_object_object_get(v4AddrObj, "IPAddress"));
		CurrSubnetMask = json_object_get_string(json_object_object_get(v4AddrObj, "SubnetMask"));
		CurrGatewayIPAddress = json_object_get_string(json_object_object_get(v4FwdObj, "GatewayIPAddress"));
			
		if(json_object_object_get(Jobj, "IPAddress")){
			if(strcmp(IPAddress, CurrIPAddress)){
				json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(true));
				json_object_object_add(v4AddrObj, "IPAddress", json_object_new_string(IPAddress));
				ipdnsmodify = true;
			}
		}
		if(json_object_object_get(Jobj, "SubnetMask")){
			if(strcmp(SubnetMask, CurrSubnetMask)){
				json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(true));
				json_object_object_add(v4AddrObj, "SubnetMask", json_object_new_string(SubnetMask));
				ipdnsmodify = true;
			}
		}
		if(!ipdnsmodify){
			if(v4AddrObj) {zcfgFeJsonObjFree(v4AddrObj);}
		}

		//v4 fwd
		if(v4FwdObj){
            //json_object_object_add(v4FwdObj, "StaticRoute", json_object_new_boolean(true)); //readonly parameter, cant set from front end.
			if(strcmp(GatewayIPAddress, CurrGatewayIPAddress)){
				//json_object_object_add(v4FwdObj, "Interface", json_object_new_string(ipIfacePath));
				json_object_object_add(v4FwdObj, "GatewayIPAddress", json_object_new_string(GatewayIPAddress));
			}
			else{
				if(v4FwdObj) {zcfgFeJsonObjFree(v4FwdObj);}
			}
		}
	}
	else if(CurrIpStatic && !ipStatic){ ////enable to disable
		//v4 addr, disable or delete or do nothing?
		if(v4AddrObj)
		{	json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(false));	}
		//v4 fwd
		if(v4FwdObj)
		{
			json_object_object_add(v4FwdObj, "Interface", json_object_new_string(""));
			//json_object_object_add(v4FwdObj, "StaticRoute", json_object_new_boolean(false));
		}
	}
	else if(!CurrIpStatic && ipStatic){ //disable to enable
		if(v4AddrObj == NULL){
			oid = RDM_OID_IP_IFACE_V4_ADDR;
			IID_INIT(v4AddrIid);
			v4AddrIid.level = 1;
			sscanf(ipIfacePath, "IP.Interface.%hhu", &v4AddrIid.idx[0]);
			zcfgFeObjJsonAdd(oid, &v4AddrIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &v4AddrIid, &v4AddrObj);
		}
		json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v4AddrObj, "AddressingType", json_object_new_string("Static"));
		replaceParam(v4AddrObj, "IPAddress", Jobj, "IPAddress");
		replaceParam(v4AddrObj, "SubnetMask", Jobj, "SubnetMask");


		if(v4FwdObj == NULL){
			oid = RDM_OID_ROUTING_ROUTER_V4_FWD;
			if(!getSpecificObj(oid, "Interface", json_type_string, "", &v4FwdIid, &v4FwdObj)){
				IID_INIT(v4FwdIid);
				v4FwdIid.level = 1;
				v4FwdIid.idx[0] = 1;
				zcfgFeObjJsonAdd(oid, &v4FwdIid, NULL);
				zcfgFeObjJsonGetWithoutUpdate(oid, &v4FwdIid, &v4FwdObj);
			}
		}
		json_object_object_add(v4FwdObj, "Enable", json_object_new_boolean(true));
		//json_object_object_add(v4FwdObj, "StaticRoute", json_object_new_boolean(true));
		json_object_object_add(v4FwdObj, "Origin", json_object_new_string("Static"));
		json_object_object_add(v4FwdObj, "Interface", json_object_new_string(ipIfacePath));
		json_object_object_add(v4FwdObj, "DestIPAddress", json_object_new_string(""));
		json_object_object_add(v4FwdObj, "DestSubnetMask", json_object_new_string(""));
		replaceParam(v4FwdObj, "GatewayIPAddress", Jobj, "GatewayIPAddress");
	}
	else{//do nothing for disable to disable
		if(v4AddrObj) {zcfgFeJsonObjFree(v4AddrObj);}
		if(v4FwdObj) {zcfgFeJsonObjFree(v4FwdObj);}
	}

	//static v6ip
	if(CurrIp6Static && ip6Static){ //enable to enable
		//v6 addr
		IP6Address = json_object_get_string(json_object_object_get(Jobj, "IP6Address"));
		NextHop = json_object_get_string(json_object_object_get(Jobj, "NextHop"));
		CurrIP6Address = json_object_get_string(json_object_object_get(v6AddrObj, "IPAddress"));
		CurrNextHop = json_object_get_string(json_object_object_get(v6FwdObj, "NextHop"));
		
		if(strcmp(IP6Address, CurrIP6Address)){
			json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(v6AddrObj, "IPAddress", json_object_new_string(IP6Address));
		}else{
			if(v6AddrObj) {zcfgFeJsonObjFree(v6AddrObj);}
		}

		//v6 fwd
		if(v6FwdObj == NULL){
			oid = RDM_OID_ROUTING_ROUTER_V6_FWD;
			if(!getSpecificObj(oid, "Interface", json_type_string, "", &v6FwdIid, &v6FwdObj)){
				IID_INIT(v6FwdIid);
				v6FwdIid.level = 1;
				v6FwdIid.idx[0] = 1;
				zcfgFeObjJsonAdd(oid, &v6FwdIid, NULL);
				zcfgFeObjJsonGetWithoutUpdate(oid, &v6FwdIid, &v6FwdObj);
			}
		}
		json_object_object_add(v6FwdObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v6FwdObj, "Origin", json_object_new_string("Static"));
		json_object_object_add(v6FwdObj, "Interface", json_object_new_string(ipIfacePath));
		json_object_object_add(v6FwdObj, "DestIPPrefix", json_object_new_string("::/0"));
		replaceParam(v6FwdObj, "NextHop", Jobj, "NextHop");
	}
	else if(CurrIp6Static && !ip6Static){ ////enable to disable
		//v6 addr, disable or delete or do nothing?
		if(v6AddrObj)
		{	json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(false));	}
		//v6 fwd
		if(v6FwdObj)
		{   json_object_object_add(v6FwdObj, "Interface", json_object_new_string(""));	}
	}
	else if(!CurrIp6Static && ip6Static){ //disable to enable
		if(v6AddrObj == NULL){
			oid = RDM_OID_IP_IFACE_V6_ADDR;
			IID_INIT(v6AddrIid);
			v6AddrIid.level = 1;
			sscanf(ipIfacePath, "IP.Interface.%hhu", &v6AddrIid.idx[0]);
			zcfgFeObjJsonAdd(oid, &v6AddrIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &v6AddrIid, &v6AddrObj);
		}
		json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v6AddrObj, "Origin", json_object_new_string("Static"));
		replaceParam(v6AddrObj, "IPAddress", Jobj, "IP6Address");


		if(v6FwdObj == NULL){
			oid = RDM_OID_ROUTING_ROUTER_V6_FWD;
			if(!getSpecificObj(oid, "Interface", json_type_string, "", &v6FwdIid, &v6FwdObj)){
				IID_INIT(v6FwdIid);
				v6FwdIid.level = 1;
				v6FwdIid.idx[0] = 1;
				zcfgFeObjJsonAdd(oid, &v6FwdIid, NULL);
				zcfgFeObjJsonGetWithoutUpdate(oid, &v6FwdIid, &v6FwdObj);
			}
		}
		json_object_object_add(v6FwdObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v6FwdObj, "Origin", json_object_new_string("Static"));
		json_object_object_add(v6FwdObj, "Interface", json_object_new_string(ipIfacePath));
		json_object_object_add(v6FwdObj, "DestIPPrefix", json_object_new_string("::/0"));
		replaceParam(v6FwdObj, "NextHop", Jobj, "NextHop");
	}
	else{//do nothing for disable to disable
		if(v6AddrObj) {zcfgFeJsonObjFree(v6AddrObj);}
		if(v6FwdObj) {zcfgFeJsonObjFree(v6FwdObj);}
	}

	return ret;
}

zcfgRet_t getDhcpv4ClientOpt(objIndex_t *dhcpv4ClinetIid){
	zcfg_offset_t oid;
	objIndex_t iid;
	struct json_object *obj = NULL;
	int tag;
	
	IID_INIT(iid);
	oid = RDM_OID_DHCPV4_REQ_OPT;
	while(zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, dhcpv4ClinetIid, &iid, &obj)== ZCFG_SUCCESS) {
		tag = json_object_get_int(json_object_object_get(obj, "Tag"));
		if(tag == 42){
			memcpy(&opt42Iid, &iid, sizeof(objIndex_t));
			opt42Obj = obj;
		}
		else if(tag == 43){
			memcpy(&opt43Iid, &iid, sizeof(objIndex_t));
			opt43Obj = obj;
		}
		else if(tag == 120){
			memcpy(&opt120Iid, &iid, sizeof(objIndex_t));
			opt120Obj = obj;
		}
		else if(tag == 121){
			memcpy(&opt121Iid, &iid, sizeof(objIndex_t));
			opt121Obj = obj;
		}
		else if(tag == 212){
			memcpy(&opt212Iid, &iid, sizeof(objIndex_t));
			opt212Obj = obj;
		}
		else
			json_object_put(obj);
	}

	IID_INIT(iid);
	oid = RDM_OID_DHCPV4_SENT_OPT;
	while(zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, dhcpv4ClinetIid, &iid, &obj)== ZCFG_SUCCESS) {
		tag = json_object_get_int(json_object_object_get(obj, "Tag"));
		if(tag == 60){
			memcpy(&opt60Iid, &iid, sizeof(objIndex_t));
			opt60Obj = obj;
		}
		else if(tag == 61){
			memcpy(&opt61Iid, &iid, sizeof(objIndex_t));
			opt61Obj = obj;
		}
		else if(tag == 125){
			memcpy(&opt125Iid, &iid, sizeof(objIndex_t));
			opt125Obj = obj;
		}
		else
			json_object_put(obj);
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t setDhcpReqOption(struct json_object *Jobj, struct json_object **optObj, objIndex_t *optIid, objIndex_t *dhcpClientIid, int Tag, char *Alias){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//char optEnableName[32] = {0};
	zcfg_offset_t oid;
	//const char *ipMode, *v6RD_Type;
	//bool Enable_6RD, Enable = false;

	oid = RDM_OID_DHCPV4_REQ_OPT;
	
#if 0
	sprintf(optEnableName, "option%dEnable", Tag); //ex. option43Enable
	ipMode = json_object_get_string(json_object_object_get(Jobj, "ipMode"));
	Enable_6RD = json_object_get_boolean(json_object_object_get(Jobj, "Enable_6RD"));
	v6RD_Type = json_object_get_string(json_object_object_get(Jobj, "v6RD_Type"));

	if(Tag == 212){ //6RD
		if(!strcmp(ipMode, "IPv4") && Enable_6RD && !strcmp(v6RD_Type, "dhcp"))
			Enable = true;
	}else{
		Enable = json_object_get_boolean(json_object_object_get(Jobj, optEnableName));
	}
#endif

	if(*optObj){
		//if(Enable)
		json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
		json_object_object_add(*optObj, "Alias", json_object_new_string(Alias));
		//else
		//json_object_object_add(*optObj, "Enable", json_object_new_boolean(false));
		
		//zcfgFeObjJsonSetWithoutApply(oid, optIid, *optObj, NULL);
	}
	else{
		//if(Enable){
			memcpy(optIid, dhcpClientIid, sizeof(objIndex_t));
			zcfgFeObjJsonAdd(oid, optIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, optIid, optObj);
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
			json_object_object_add(*optObj, "Alias", json_object_new_string(Alias));
			//zcfgFeObjJsonSetWithoutApply(oid, optIid, *optObj, NULL);
		//}
	}

	return ret;
}

zcfgRet_t setDhcpSentOption(struct json_object *Jobj, struct json_object **optObj, objIndex_t *optIid, objIndex_t *dhcpClientIid, int Tag){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//char optEnableName[32] = {0};
	char optValueName[32] = {0};
	zcfg_offset_t oid;
	//bool Enable = false;
	char tmp[256]={0};
	char Value[256]={0};
	char *opt60Val = NULL;
	
	oid = RDM_OID_DHCPV4_SENT_OPT;
	//sprintf(optEnableName, "option%dEnable", Tag); //ex. option60Enable
	sprintf(optValueName, "option%dValue", Tag); //ex. option60Value
	//Enable = json_object_get_boolean(json_object_object_get(Jobj, optEnableName));

	if(Tag==60){
		opt60Val = json_object_get_string(json_object_object_get(Jobj, "option60Value"));
		strcpy(tmp,opt60Val);
		opt60ValEncode(tmp,Value,Tag);
		json_object_object_add(Jobj,"option60Value",json_object_new_string(Value));
	}
	else if(Tag==61){
		char *tmpIaid = NULL;
		char *tmpDuid = NULL;
		if(json_object_object_get(Jobj, "option61Iaid"))
			tmpIaid = json_object_get_string(json_object_object_get(Jobj, "option61Iaid"));
		if(json_object_object_get(Jobj, "option61Duid"))
			tmpDuid = json_object_get_string(json_object_object_get(Jobj, "option61Duid"));
		if(json_object_object_get(Jobj, "option61Iaid")&&json_object_object_get(Jobj, "option61Duid")){
			opt61ValEncode(tmpIaid,tmpDuid,Value,Tag);
			json_object_object_add(Jobj,"option61Value",json_object_new_string(Value));
		}
	}
    else if(Tag == 125){
        opt125ValGet(Value);
        json_object_object_add(Jobj,"option125Value",json_object_new_string(Value));
    }

	if(*optObj){
		json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
		replaceParam(*optObj, "Value", Jobj, optValueName);
		//zcfgFeObjJsonSetWithoutApply(oid, optIid, *optObj, NULL);
	}
	else{
		//if(Enable){
			memcpy(optIid, dhcpClientIid, sizeof(objIndex_t));
			zcfgFeObjJsonAdd(oid, optIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, optIid, optObj);
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
			replaceParam(*optObj, "Value", Jobj, optValueName);
			//zcfgFeObjJsonSetWithoutApply(oid, optIid, *optObj, NULL);
		//}
	}

	return ret;
}

zcfgRet_t addDhcpSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	zcfg_offset_t oid;
	objIndex_t iid;
	//objIndex_t v4ClientIid;
	//objIndex_t opt43Iid = {0}, opt120Iid = {0}, opt121Iid = {0}, opt212Iid = {0};
	//objIndex_t opt60Iid = {0}, opt61Iid = {0}, opt125Iid = {0};
	struct json_object *obj = NULL;
	//struct json_object *v4ClientObj = NULL;
	//struct json_object *opt43obj = NULL, *opt120obj = NULL, *opt121obj = NULL, *opt212obj = NULL;
	//struct json_object *opt60obj = NULL, *opt61obj = NULL, *opt125obj = NULL;
	//const char *Encapsulation, *ipMode;
	//bool ipStatic;
	bool option42Enable=false,option43Enable=false, option120Enable=false, option121Enable=false, option212Enable=false;
	bool option60Enable=false, option61Enable=false, option125Enable=false;
	int tag;

	if(!strcmp(Encapsulation, "PPPoA") || !strcmp(Encapsulation, "PPPoE"))
		return ret;

	option42Enable = json_object_get_boolean(json_object_object_get(Jobj, "option42Enable"));
	option43Enable = json_object_get_boolean(json_object_object_get(Jobj, "option43Enable"));
	option120Enable = json_object_get_boolean(json_object_object_get(Jobj, "option120Enable"));
	option121Enable = json_object_get_boolean(json_object_object_get(Jobj, "option121Enable"));
	if(!strcmp(ipMode, "IPv4") && Enable_6RD && !strcmp(v6RD_Type, "dhcp"))
		option212Enable = json_object_get_boolean(json_object_object_get(Jobj, "option212Enable"));
	
	option60Enable = json_object_get_boolean(json_object_object_get(Jobj, "option60Enable"));
	option61Enable = json_object_get_boolean(json_object_object_get(Jobj, "option61Enable"));
	option125Enable = json_object_get_boolean(json_object_object_get(Jobj, "option125Enable"));
	
	if(strcmp(ipMode, "IPv6") && strcmp(Encapsulation, "IPoA") && !ipStatic){
		if(!getSpecificObj(RDM_OID_DHCPV4_CLIENT, "Interface", json_type_string, "", &v4ClientIid, &v4ClientObj)){
			IID_INIT(v4ClientIid);
			zcfgFeObjJsonAdd(RDM_OID_DHCPV4_CLIENT, &v4ClientIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DHCPV4_CLIENT, &v4ClientIid, &v4ClientObj);
		}

		//option 42, 43, 120, 121, 212
		IID_INIT(iid);
		oid = RDM_OID_DHCPV4_REQ_OPT;
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, &v4ClientIid, &iid, &obj)== ZCFG_SUCCESS) {
			tag = json_object_get_int(json_object_object_get(obj, "Tag"));
			if(tag == 42 && option42Enable){
				memcpy(&opt42Iid, &iid, sizeof(objIndex_t));
				opt42Obj = obj;
			}
			else if(tag == 43 && option43Enable){
				memcpy(&opt43Iid, &iid, sizeof(objIndex_t));
				opt43Obj = obj;
			}
			else if(tag == 120 && option120Enable){
				memcpy(&opt120Iid, &iid, sizeof(objIndex_t));
				opt120Obj = obj;
			}
			else if(tag == 121 && option121Enable){
				memcpy(&opt121Iid, &iid, sizeof(objIndex_t));
				opt121Obj = obj;
			}
			else if(tag == 212 && option212Enable){
				memcpy(&opt212Iid, &iid, sizeof(objIndex_t));
				opt212Obj = obj;
			}
			else
				json_object_put(obj);
		}

		if(option42Enable) setDhcpReqOption(Jobj, &opt42Obj, &opt42Iid, &v4ClientIid, 42, "ntpsrv");
		if(option43Enable) setDhcpReqOption(Jobj, &opt43Obj, &opt43Iid, &v4ClientIid, 43, "acsinfo");
		if(option120Enable) setDhcpReqOption(Jobj, &opt120Obj, &opt120Iid, &v4ClientIid, 120, "sipsrv");
		if(option121Enable) setDhcpReqOption(Jobj, &opt121Obj, &opt121Iid, &v4ClientIid, 121, "sroute");
		if(option212Enable) setDhcpReqOption(Jobj, &opt212Obj, &opt212Iid, &v4ClientIid, 212, "");
		
		//if(opt43Obj) json_object_put(opt43Obj);
		//if(opt120Obj) json_object_put(opt120Obj);
		//if(opt121Obj) json_object_put(opt121Obj);
		//if(opt212Obj) json_object_put(opt212Obj);


		//option 60, 61, 125
		IID_INIT(iid);
		oid = RDM_OID_DHCPV4_SENT_OPT;
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, &v4ClientIid, &iid, &obj)== ZCFG_SUCCESS) {
			tag = json_object_get_int(json_object_object_get(obj, "Tag"));
			if(tag == 60 && option60Enable){
				memcpy(&opt60Iid, &iid, sizeof(objIndex_t));
				opt60Obj = obj;
			}
			else if(tag == 61 && option61Enable){
				memcpy(&opt61Iid, &iid, sizeof(objIndex_t));
				opt61Obj = obj;
			}
			else if(tag == 125 && option125Enable){
				memcpy(&opt125Iid, &iid, sizeof(objIndex_t));
				opt125Obj = obj;
			}
			else
				json_object_put(obj);
		}

		if(option60Enable) setDhcpSentOption(Jobj, &opt60Obj, &opt60Iid, &v4ClientIid, 60);
		if(option61Enable) setDhcpSentOption(Jobj, &opt61Obj, &opt61Iid, &v4ClientIid, 61);
		if(option125Enable) setDhcpSentOption(Jobj, &opt125Obj, &opt125Iid, &v4ClientIid, 125);
		
		//if(opt60Obj) json_object_put(opt60Obj);
		//if(opt61Obj) json_object_put(opt61Obj);
		//if(opt125Obj) json_object_put(opt125Obj);

		//set DHCPv4 Client
		json_object_object_add(v4ClientObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v4ClientObj, "Interface", json_object_new_string(ipIfacePath));
		//ret = zcfgFeObjJsonSet(RDM_OID_DHCPV4_CLIENT, &v4ClientIid, v4ClientObj, NULL);
		//json_object_put(v4ClientObj);
	}

	return ret;
}


zcfgRet_t editDhcpReqOption(struct json_object *Jobj, struct json_object **optObj, objIndex_t *optIid, objIndex_t *dhcpClientIid, int Tag, char *Alias){
	zcfgRet_t ret = ZCFG_SUCCESS;
	char optEnableName[32] = {0};
	zcfg_offset_t oid;
	//const char *ipMode, *v6RD_Type;
	bool optEnable = false, CurrOptEnable = false;
	struct json_object *pramJobj = NULL;
	bool needSet = false;

	oid = RDM_OID_DHCPV4_REQ_OPT;

	if(*optObj){
		if(json_object_get_boolean(json_object_object_get(*optObj, "Enable")))
			CurrOptEnable = true;
	}
	
	sprintf(optEnableName, "option%dEnable", Tag); //ex. option43Enable
	pramJobj = json_object_object_get(Jobj, optEnableName);
	if(pramJobj)
		optEnable = json_object_get_boolean(pramJobj);
	else
		optEnable = CurrOptEnable;

	if(CurrOptEnable && !optEnable){ //enable to disable
		if(*optObj){
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(false));
			json_object_object_add(*optObj, "Value", json_object_new_string(""));
			needSet = true;
		}
	}
	else if(!CurrOptEnable && optEnable){ //disable to enable
		if(*optObj){
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Alias", json_object_new_string(Alias));
		}
		else{
			memcpy(optIid, dhcpClientIid, sizeof(objIndex_t));
			zcfgFeObjJsonAdd(oid, optIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, optIid, optObj);
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
			json_object_object_add(*optObj, "Alias", json_object_new_string(Alias));
		}
		needSet = true;
	}

	if(!needSet){
		if(*optObj) {zcfgFeJsonObjFree(*optObj);}
	}

	return ret;
}

zcfgRet_t editDhcpSentOption(struct json_object *Jobj, struct json_object **optObj, objIndex_t *optIid, objIndex_t *dhcpClientIid, int Tag){
	zcfgRet_t ret = ZCFG_SUCCESS;
	char optEnableName[32] = {0};
	char optValueName[32] = {0};
	zcfg_offset_t oid;
	//const char *ipMode, *v6RD_Type;
	bool optEnable = false, CurrOptEnable = false;
	const char *CurrValue = "";
	struct json_object *pramJobj = NULL;
	bool needSet = false;
	char tmp[256]={0}, Value[256]={0};
	char *newValue = NULL;
	
	oid = RDM_OID_DHCPV4_SENT_OPT;
	
	if(*optObj){
		CurrOptEnable = json_object_get_boolean(json_object_object_get(*optObj, "Enable"));
		CurrValue = json_object_get_string(json_object_object_get(*optObj, "Value"));
		if(CurrValue == NULL)
			CurrValue = "";
	}
	
	sprintf(optEnableName, "option%dEnable", Tag); //ex. option60Enable
	pramJobj = json_object_object_get(Jobj, optEnableName);
	if(pramJobj)
		optEnable = json_object_get_boolean(pramJobj);
	else
		optEnable = CurrOptEnable;

	sprintf(optValueName, "option%dValue", Tag); //ex. option60Value
	pramJobj = json_object_object_get(Jobj, optValueName);
	if(pramJobj)
		newValue = json_object_get_string(pramJobj);
	else
		newValue = CurrValue;

	if(Tag==60 && optEnable){
		strcpy(tmp,newValue);
		opt60ValEncode(tmp,Value,Tag);
	}
	else if(Tag==61 && optEnable){
		char *tmpIaid = NULL;
		char *tmpDuid = NULL;
		if(json_object_object_get(Jobj, "option61Iaid"))
			tmpIaid = json_object_get_string(json_object_object_get(Jobj, "option61Iaid"));
		if(json_object_object_get(Jobj, "option61Duid"))
			tmpDuid = json_object_get_string(json_object_object_get(Jobj, "option61Duid"));
		if(json_object_object_get(Jobj, "option61Iaid")&&json_object_object_get(Jobj, "option61Duid")){
			strcpy(tmp,"");
			opt61ValEncode(tmpIaid,tmpDuid,tmp,Tag);
			strcpy(Value, tmp);
		}
	}
	else if(Tag==125 && optEnable){
		strcpy(Value, newValue);
	}
    else if(Tag == 125 && optEnable){
        if(strlen(newValue) == 0){
            opt125ValGet(Value);
        }
        else{
            snprintf(Value, sizeof(Value), newValue);
        }
    }

	if(CurrOptEnable && optEnable){//enable to enable
		if(*optObj && strcmp(CurrValue, Value)){
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Value", json_object_new_string(Value));
			needSet = true;
		}
	}
	else if(CurrOptEnable && !optEnable){ //enable to disable
		if(*optObj){
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(false));
			json_object_object_add(*optObj, "Value", json_object_new_string(""));
			needSet = true;
		}
	}
	else if(!CurrOptEnable && optEnable){ //disable to enable
		if(*optObj){
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Value", json_object_new_string(Value));
		}
		else{
			memcpy(optIid, dhcpClientIid, sizeof(objIndex_t));
			zcfgFeObjJsonAdd(oid, optIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, optIid, optObj);
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
			json_object_object_add(*optObj, "Value", json_object_new_string(Value));

			memcpy(optIid, dhcpClientIid, sizeof(objIndex_t));
			zcfgFeObjJsonAdd(oid, optIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, optIid, optObj);
			json_object_object_add(*optObj, "Enable", json_object_new_boolean(true));
			json_object_object_add(*optObj, "Tag", json_object_new_int(Tag));
			replaceParam(*optObj, "Value", Jobj, optValueName);
		}
		needSet = true;
	}

	if(!needSet)
		if(*optObj) {zcfgFeJsonObjFree(*optObj);}

	printf("fe_dal_broadband.c:%d::%s() toString(*optObj)=%s\n",__LINE__,__FUNCTION__,json_object_to_json_string(*optObj));
	return ret;
}

void editDhcpOption(struct json_object *Jobj, int action){

	if(action == dal_Delete){
		json_object_object_add(Jobj, "option42Enable", json_object_new_boolean(false));
        json_object_object_add(Jobj, "option43Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option120Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option121Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option212Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option60Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option61Enable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "option125Enable", json_object_new_boolean(false));
	}

	editDhcpReqOption(Jobj, &opt42Obj, &opt42Iid, &v4ClientIid, 42, "ntpsrv");
	editDhcpReqOption(Jobj, &opt43Obj, &opt43Iid, &v4ClientIid, 43, "acsinfo");
	editDhcpReqOption(Jobj, &opt120Obj, &opt120Iid, &v4ClientIid, 120, "sipsrv");
	editDhcpReqOption(Jobj, &opt121Obj, &opt121Iid, &v4ClientIid, 121, "sroute");
	editDhcpReqOption(Jobj, &opt212Obj, &opt212Iid, &v4ClientIid, 212, "");
	editDhcpSentOption(Jobj, &opt60Obj, &opt60Iid, &v4ClientIid, 60);
	editDhcpSentOption(Jobj, &opt61Obj, &opt61Iid, &v4ClientIid, 61);
	editDhcpSentOption(Jobj, &opt125Obj, &opt125Iid, &v4ClientIid, 125);
	
}

zcfgRet_t editDhcpSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	zcfg_offset_t oid;
	struct json_object *pramJobj = NULL;
	bool dhcpEnable = false;
	bool CrrrDhcpEnable = false;
	bool setObj = false;

	if(v4ClientObj != NULL) CrrrDhcpEnable = true;

	pramJobj = json_object_object_get(Jobj, "ipStatic");
	if(pramJobj)
		dhcpEnable = !json_object_get_boolean(pramJobj);
	else if(strstr(CurrEncapsulation, "PPP") != NULL && !strcmp(Encapsulation, "IPoE")) //PPPoX to IPoE and not set static IP
		dhcpEnable = true;
	else
		dhcpEnable = CrrrDhcpEnable;

	if(!strcmp(Encapsulation, "PPPoE") || !strcmp(Encapsulation, "PPPoA"))
		dhcpEnable = false;

	dbg_printf("CrrrDhcpEnable=%d dhcpEnable=%d\n", CrrrDhcpEnable, dhcpEnable);

	if(CrrrDhcpEnable && dhcpEnable){
		json_object_object_add(v4ClientObj, "Enable", json_object_new_boolean(true));
		editDhcpOption(Jobj, dal_Edit);
	}
	else if(CrrrDhcpEnable && !dhcpEnable){
		editDhcpOption(Jobj, dal_Delete);
		json_object_object_add(v4ClientObj, "Interface", json_object_new_string(""));
		setObj = true;
	}
	else if(!CrrrDhcpEnable && dhcpEnable){
		oid = RDM_OID_DHCPV4_CLIENT;
		if(!getSpecificObj(oid, "Interface", json_type_string, "", &v4ClientIid, &v4ClientObj)){
			IID_INIT(v4ClientIid);
			zcfgFeObjJsonAdd(oid, &v4ClientIid, NULL);
			zcfgFeObjJsonGetWithoutUpdate(oid, &v4ClientIid, &v4ClientObj);
		}
		json_object_object_add(v4ClientObj, "Enable", json_object_new_boolean(true));
		json_object_object_add(v4ClientObj, "Interface", json_object_new_string(ipIfacePath));
		setObj = true;
		
		getDhcpv4ClientOpt(&v4ClientIid);
		editDhcpOption(Jobj, dal_Add);
	}

	if(!setObj){
		if(v4ClientObj) {zcfgFeJsonObjFree(v4ClientObj);}
	}

	return ret;
}

zcfgRet_t editIgmpMldSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//objIndex_t iid;
	//struct json_object *obj = NULL;
	const char *Interface = NULL;
	char newInterface[512] = {0};
	struct json_object *pramJobj = NULL;
	bool igmpEnable = false, mldEnable = false;
	bool CurrIgmpEnable = false, CurrMldEnable = false;
	
	if(igmpObj == NULL){ //add case
		IID_INIT(igmpIid);
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_IGMP, &igmpIid, &igmpObj);
	}
	if(mldObj == NULL){ //add case
		IID_INIT(mldIid);
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_MLD, &mldIid, &mldObj);
	}
		
	Interface = json_object_get_string(json_object_object_get(igmpObj, "Interface"));
	if(strstr(Interface, ipIfacePath))
		CurrIgmpEnable = true;

	Interface = json_object_get_string(json_object_object_get(mldObj, "Interface"));
	if(strstr(Interface, ipIfacePath))
		CurrMldEnable = true;

	pramJobj = json_object_object_get(Jobj, "IGMPEnable");
	if(pramJobj)
		igmpEnable = json_object_get_boolean(pramJobj);
	else
		igmpEnable = CurrIgmpEnable;

	pramJobj = json_object_object_get(Jobj, "MLDEnable");
	if(pramJobj)
		mldEnable = json_object_get_boolean(pramJobj);
	else
		mldEnable = CurrMldEnable;
	

	/* IGMP */
	Interface = json_object_get_string(json_object_object_get(igmpObj, "Interface"));
	if(CurrIgmpEnable && igmpEnable){ //enable to enable
		if(igmpObj) {zcfgFeJsonObjFree(igmpObj);}
	}
	else if(CurrIgmpEnable && !igmpEnable){ ////enable to disable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, false)){
			json_object_object_add(igmpObj, "Interface", json_object_new_string(newInterface));
		}
		else{
			if(igmpObj) {zcfgFeJsonObjFree(igmpObj);}
		}
	}
	else if(!CurrIgmpEnable && igmpEnable){ //disable to enable
#ifndef ZyXEL_WEB_GUI_SINGLE_IGMP_MLD
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, true)){
			json_object_object_add(igmpObj, "Interface", json_object_new_string(newInterface));
		}
#else
		char IgmpIntf[512] = {0};
		DisableSameWanTypeIgmpMld(IgmpIntf, Interface, ipIfacePath, 0);
		if(genNewInterface(",", newInterface, IgmpIntf, ipIfacePath, true)){
			json_object_object_add(igmpObj, "Interface", json_object_new_string(newInterface));
		}
#endif	
		else{
			if(igmpObj) {zcfgFeJsonObjFree(igmpObj);}
		}
	}
	else{//do nothing for disable to disable
		if(igmpObj) {zcfgFeJsonObjFree(igmpObj);}
	}

	/* MLD */
	Interface = json_object_get_string(json_object_object_get(mldObj, "Interface"));
	if(CurrMldEnable && mldEnable){ //enable to enable
		if(mldObj) {zcfgFeJsonObjFree(mldObj);}
	}
	else if(CurrMldEnable && !mldEnable){ ////enable to disable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, false)){
			json_object_object_add(mldObj, "Interface", json_object_new_string(newInterface));
		}
		else{
			if(mldObj) {zcfgFeJsonObjFree(mldObj);}
		}
	}
	else if(!CurrMldEnable && mldEnable){ //disable to enable
#ifndef ZyXEL_WEB_GUI_SINGLE_IGMP_MLD
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, true)){
			json_object_object_add(mldObj, "Interface", json_object_new_string(newInterface));
		}
#else
		char MldIntf[512] = {0};
		DisableSameWanTypeIgmpMld(MldIntf, Interface, ipIfacePath, 0);	
		if(genNewInterface(",", newInterface, MldIntf, ipIfacePath, true)){
			json_object_object_add(mldObj, "Interface", json_object_new_string(newInterface));
		}
#endif
		else{
			if(mldObj) {zcfgFeJsonObjFree(mldObj);}
		}
	}
	else{//do nothing for disable to disable
		if(mldObj) {zcfgFeJsonObjFree(mldObj);}
	}

	return ret;
}

zcfgRet_t DisableSameWanTypeIgmpMld(char *newInterface, const char *oldInterface, char *Ifname, int IgmpMld)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *pramJobj = NULL;
	char buff[512] = {0}, *intf = NULL, *tmp_ptr = NULL;
	char ipWanType[16] = {0}, otherWanType[16] = {0};	

	memset(ipWanType, 0, sizeof(ipWanType));

	strncpy(ipWanType, Type, sizeof(ipWanType)-1);	
	
	//disable other same wan type igmp and mld proxy
	strcpy(buff, oldInterface);
	intf = strtok_r(buff, ",", &tmp_ptr);
	while(intf != NULL)
	{
		memset(otherWanType, 0, sizeof(otherWanType));
		IpIfaceWanTypeGet(intf, otherWanType);
			
		if(strcmp(ipWanType,otherWanType) == 0)
		{			
				genNewInterface(",", newInterface, oldInterface, intf, false);			
	
			return ret;
		}
		
		intf = strtok_r(NULL, ",", &tmp_ptr);
	}

	strcpy(newInterface, oldInterface);
	return ret;
}

zcfgRet_t IpIfaceWanTypeGet(char *Ifname, char *WanType){
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *HigherLayer;
	const char *LowerLayer;
	int count = 0;	
	
	memset(WanType, 0, sizeof(WanType));
	
	struct json_object *atmObj = NULL;
	struct json_object *ptmObj = NULL;
	struct json_object *ethObj = NULL;
	struct json_object *optObj = NULL;
	struct json_object *cellObj = NULL;
	struct json_object *ethlink = NULL;
	struct json_object *vlanterm = NULL;
	struct json_object *pppObj = NULL;
	struct json_object *ipObj = NULL;

	objIndex_t atmIid = {0};
	objIndex_t ptmIid = {0};
	objIndex_t ethIid = {0};
	objIndex_t optIid = {0};
	objIndex_t cellIid = {0};
	objIndex_t ethlinkIid = {0};
	objIndex_t vlantermIid = {0};
	objIndex_t pppIid = {0};	
	objIndex_t ipIid = {0};	
	
	IID_INIT(ipIid);
	ipIid.level = 1;
	sscanf(Ifname, "IP.Interface.%hhu", &ipIid.idx[0]);
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE, &ipIid, &ipObj) != ZCFG_SUCCESS)
		return ZCFG_REQUEST_REJECT;		

	LowerLayer = json_object_get_string(json_object_object_get(ipObj, "LowerLayers"));		
	HigherLayer = "IP.Interface.";
	strcpy(CurrMode, json_object_get_string(json_object_object_get(ipObj, "X_ZYXEL_ConnectionType")));
	
	while(count < 4)
	{
		count++;
		if(!strncmp(HigherLayer, "IP.Interface.", 13))
		{
			if(!strncmp(LowerLayer, "PPP.Interface.", 14))
			{
				IID_INIT(pppIid);
				pppIid.level = 1;
				sscanf(LowerLayer, "PPP.Interface.%hhu", &pppIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PPP_IFACE, &pppIid, &pppObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(pppObj, "LowerLayers"));
			}
			else if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25))
			{
				IID_INIT(vlantermIid);
				vlantermIid.level = 1;
				sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlantermIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlantermIid, &vlanterm);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(vlanterm, "LowerLayers"));
			}
			else if(!strncmp("Ethernet.Link.", LowerLayer, 14))
			{
				IID_INIT(ethlinkIid);
				ethlinkIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethlinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethlinkIid, &ethlink);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(ethlink, "LowerLayers"));
			}
			else if(!strncmp("ATM.Link.", LowerLayer, 9) && !strcmp("IP_Routed", CurrMode))
			{
				IID_INIT(atmIid);
				atmIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmIid, &atmObj);		
				strcpy(WanType, "ATM");
				break;
			}
			else
			{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp(HigherLayer, "PPP.Interface.", 14))
		{
			if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25))
			{
				IID_INIT(vlantermIid);
				vlantermIid.level = 1;
				sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlantermIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlantermIid, &vlanterm);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(vlanterm, "LowerLayers"));		
			}
			else if(!strncmp("ATM.Link.", LowerLayer, 9))
			{
				IID_INIT(atmIid);
				atmIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmIid, &atmObj);
				strcpy(WanType, "ATM");		
				break;
			}
			else
			{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25))
		{
			if(!strncmp("Ethernet.Link.", LowerLayer, 14))
			{
				IID_INIT(ethlinkIid);
				ethlinkIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethlinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethlinkIid, &ethlink);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(ethlink, "LowerLayers"));
			}
			else
			{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp("Ethernet.Link.", HigherLayer, 14))
		{
			if(!strncmp("ATM.Link.", LowerLayer, 9))
			{
				IID_INIT(atmIid);
				atmIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmIid, &atmObj);
				strcpy(WanType, "ATM");		
				break;
			}
			else if(!strncmp("PTM.Link.", LowerLayer, 9))
			{
				IID_INIT(ptmIid);
				ptmIid.level = 1;
				sscanf(LowerLayer, "PTM.Link.%hhu", &ptmIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PTM_LINK, &ptmIid, &ptmObj);
				strcpy(WanType, "PTM");
				break;
			}
			else if(!strncmp("Ethernet.Interface.", LowerLayer, 19))
			{
				IID_INIT(ethIid);
				ethIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Interface.%hhu", &ethIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_IFACE, &ethIid, &ethObj);
				strcpy(WanType, "ETH");
				break;
			}
			else if(!strncmp("Optical.Interface.", LowerLayer, 18))
			{
				IID_INIT(optIid);
				optIid.level = 1;
				sscanf(LowerLayer, "Optical.Interface.%hhu", &optIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_OPT_INTF, &optIid, &optObj);				
				strcpy(WanType, "GPON");
				break;
			}
			else if(!strncmp("Cellular.Interface.", LowerLayer, 19))
			{
				IID_INIT(cellIid);
				cellIid.level = 1;
				sscanf(LowerLayer, "Cellular.Interface.%hhu", &cellIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &cellIid, &cellObj);				
				strcpy(WanType, "CELL");
				break;
			}
			else if(!strcmp("", LowerLayer)){
				if(json_object_get_boolean(json_object_object_get(ethlink, "X_ZYXEL_OrigEthWAN")) == false)
				{
					return ZCFG_REQUEST_REJECT;
				}
			}
			else{
				return ZCFG_REQUEST_REJECT;
			}
		}
	}

	//release	
	if(atmObj) json_object_put(atmObj);
	if(ptmObj) json_object_put(ptmObj);
	if(ethObj) json_object_put(ethObj);
	if(optObj) json_object_put(optObj);
	if(cellObj) json_object_put(cellObj);
	if(ethlink) json_object_put(ethlink);
	if(vlanterm) json_object_put(vlanterm);
	if(pppObj) json_object_put(pppObj);
	if(ipObj) json_object_put(ipObj);
	
	return ret;
}


zcfgRet_t editDefaultGwSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//objIndex_t iid;
	//struct json_object *obj = NULL;
	const char *Interface = NULL;
	char newInterface[512] = {0};
	struct json_object *pramJobj = NULL;
	bool v4GwEnable = false, v6GwEnable = false;
	bool CurrV4GwEnable = false, CurrV6GwEnable = false;
	bool needSet = false;
	char *temp1 = NULL, *temp2 = NULL;
	
	if(routerObj == NULL){ //add case
		IID_INIT(routerIid);
		routerIid.level = 1;
		routerIid.idx[0] = 1;
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ROUTING_ROUTER, &routerIid, &routerObj);
	}
		
	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveDefaultGateway"));
	if(strstr(Interface, ipIfacePath))
		CurrV4GwEnable = true;

	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveV6DefaultGateway"));
	if(strstr(Interface, ipIfacePath))
		CurrV6GwEnable = true;

	pramJobj = json_object_object_get(Jobj, "sysGwEnable");
	if(pramJobj)
		v4GwEnable = json_object_get_boolean(pramJobj);
	else
		v4GwEnable = CurrV4GwEnable;

	pramJobj = json_object_object_get(Jobj, "sysGw6Enable");
	if(pramJobj)
		v6GwEnable = json_object_get_boolean(pramJobj);
	else
		v6GwEnable = CurrV6GwEnable;

#ifdef ZYXEL_SYNC_CCWAN_TO_ALL_DEFGW
	if(v4GwEnable)
		json_object_object_add(ipIfaceObj, "X_ZYXEL_DefaultGatewayIface", json_object_new_boolean(true));
	else
		json_object_object_add(ipIfaceObj, "X_ZYXEL_DefaultGatewayIface", json_object_new_boolean(false));
#endif

	dbg_printf("CurrV4GwEnable=%d v4GwEnable=%d\n", CurrV4GwEnable, v4GwEnable);
	dbg_printf("CurrV6GwEnable=%d v6GwEnable=%d\n", CurrV6GwEnable, v6GwEnable);

	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveDefaultGateway"));
	if(CurrV4GwEnable && !v4GwEnable){ ////enable to disable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, false)){
			json_object_object_add(routerObj, "X_ZYXEL_ActiveDefaultGateway", json_object_new_string(newInterface));
			needSet = true;
		}
	}
	else if(!CurrV4GwEnable && v4GwEnable){ //disable to enable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, true)){
			json_object_object_add(routerObj, "X_ZYXEL_ActiveDefaultGateway", json_object_new_string(newInterface));
			needSet = true;
		}
	}
	//Just get the last default gateway
	if(strlen(newInterface) > 0){
		temp2 = newInterface;

		while((temp1 = strchr(temp2, ',')) != NULL){
			//find the last ',' character
			temp2 = temp1 + 1;
		}
		json_object_object_add(routerObj, "X_ZYXEL_ActiveDefaultGateway", json_object_new_string(temp2));
	}

	memset(newInterface, 0, sizeof(newInterface));

	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveV6DefaultGateway"));
	if(CurrV6GwEnable && !v6GwEnable){ ////enable to disable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, false)){
			json_object_object_add(routerObj, "X_ZYXEL_ActiveV6DefaultGateway", json_object_new_string(newInterface));
			needSet = true;
		}
	}
	else if(!CurrV6GwEnable && v6GwEnable){ //disable to enable
		if(genNewInterface(",", newInterface, Interface, ipIfacePath, true)){
			json_object_object_add(routerObj, "X_ZYXEL_ActiveV6DefaultGateway", json_object_new_string(newInterface));
			needSet = true;
		}
	}
	//Just get the last default gateway
	if(strlen(newInterface) > 0){
		temp2 = newInterface;
		while((temp1 = strchr(temp2, ',')) != NULL){
			//find the last ',' character
			temp2 = temp1 + 1;
		}
		json_object_object_add(routerObj, "X_ZYXEL_ActiveV6DefaultGateway", json_object_new_string(temp2));
	}

	if(!needSet){
		if(routerObj) {zcfgFeJsonObjFree(routerObj);}
	}

	return ret;
}


zcfgRet_t editPppIfaceObject(struct json_object *Jobj, struct json_object *ipIfaceObj, objIndex_t *ipIfaceIid){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *pramJobj = NULL;
	bool pppStatic = false;
	int idleDisconnectTime = NULL;
#ifdef ISKON_CUSTOMIZATION //Adam
	int mark;
#endif
	pramJobj = json_object_object_get(Jobj, "ipStatic");
	if(pramJobj)
		pppStatic = json_object_get_boolean(pramJobj);
	else
		pppStatic = CurrPppStatic;

	if(isAdd || pppIfaceObj == NULL){ //add case
		IID_INIT(pppIfaceIid);
		if((ret = zcfgFeObjJsonAdd(RDM_OID_PPP_IFACE, &pppIfaceIid, NULL)) != ZCFG_SUCCESS)
			return ret;
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PPP_IFACE, &pppIfaceIid, &pppIfaceObj)) != ZCFG_SUCCESS)
			return ret;
	}
	
	//set ppp obj defalult values
	//json_object_object_add(pppIfaceObj, "Enable", json_object_new_boolean(true));
#ifdef ISKON_CUSTOMIZATION //Adam
	mark = json_object_get_int(json_object_object_get(ipIfaceObj, "X_ZYXEL_IPR2_MARKING"));
	//printf("%s(%d)  mark value %d\n",__FUNCTION__, __LINE__, mark);
#endif
	json_object_object_add(pppIfaceObj, "X_ZYXEL_ConnectionId", json_object_new_int(ipIfaceIid->idx[0] - 1));
#ifdef ISKON_CUSTOMIZATION //Adam
	json_object_object_add(pppIfaceObj, "X_ZYXEL_IPR2_MARKING", json_object_new_int(mark));
#else
	json_object_object_add(pppIfaceObj, "X_ZYXEL_IPR2_MARKING", json_object_new_int(ipIfaceIid->idx[0] - 1));
#endif
	json_object_object_add(pppIfaceObj, "MaxMRUSize", json_object_new_int(1492));
	json_object_object_add(pppIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));

	// need to set ipIface object first
	sprintf(currLowerLayers, "PPP.Interface.%u", pppIfaceIid.idx[0]);
	const char *lowerLayers = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
	if(lowerLayers && strcmp(lowerLayers, currLowerLayers))
		json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));

	json_object_object_add(ipIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));
	
	sprintf(currLowerLayers, "IP.Interface.%u", ipIfaceIid->idx[0]);
	
	//set ppp parameter
	if(!replaceParam(pppIfaceObj, "Enable", Jobj, "Enable"))
		json_object_object_add(pppIfaceObj, "Enable", json_object_new_boolean(CurrEnable));
		
	replaceParam(pppIfaceObj, "Username", Jobj, "pppUsername");
	replaceParam(pppIfaceObj, "Password", Jobj, "pppPassword");
	replaceParam(pppIfaceObj, "MaxMRUSize", Jobj, "MaxMTUSize");
	replaceParam(pppIfaceObj, "ConnectionTrigger", Jobj, "ConnectionTrigger");
	idleDisconnectTime = json_object_get_int(json_object_object_get(Jobj, "IdleDisconnectTime"));
	if(json_object_object_get(Jobj, "IdleDisconnectTime")){
		json_object_object_add(pppIfaceObj, "IdleDisconnectTime", json_object_new_int(60*idleDisconnectTime));
	}
	if(json_object_get_boolean(json_object_object_get(Jobj, "pppoePassThrough")) == true)
		json_object_object_add(pppIfaceObj, "X_ZYXEL_ConnectionType", json_object_new_string("PPPoE_Bridged"));
	else
		json_object_object_add(pppIfaceObj, "X_ZYXEL_ConnectionType", json_object_new_string("IP_Routed"));
	
	if(pppStatic)
		replaceParam(pppIfaceObj, "X_ZYXEL_LocalIPAddress", Jobj, "IPAddress");
	else
		json_object_object_add(pppIfaceObj, "X_ZYXEL_LocalIPAddress", json_object_new_string("0.0.0.0"));

	if(!strcmp(ipMode, "IPv4")){
		json_object_object_add(pppIfaceObj, "IPv6CPEnable", json_object_new_boolean(false));
		json_object_object_add(pppIfaceObj, "IPCPEnable", json_object_new_boolean(true));
	}
	else if(!strcmp(ipMode, "IPv6")){
		json_object_object_add(pppIfaceObj, "IPv6CPEnable", json_object_new_boolean(true));
		json_object_object_add(pppIfaceObj, "IPCPEnable", json_object_new_boolean(false));
	}else{//Dual stack
		json_object_object_add(pppIfaceObj, "IPv6CPEnable", json_object_new_boolean(true));
		json_object_object_add(pppIfaceObj, "IPCPEnable", json_object_new_boolean(true));
	}

	return ret;
}

zcfgRet_t addLayer3Objects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	//objIndex_t ipIfaceIid;
	//struct json_object *ipIfaceObj = NULL;
	//const char *Name, *Mode, *Encapsulation, *ipMode;
	//objIndex_t dnsIid;
	//struct json_object *dnsObj = NULL;

	dbg_printf("line:%d\n", __LINE__);

	IID_INIT(ipIfaceIid);
	if((ret = zcfgFeObjJsonAdd(RDM_OID_IP_IFACE, &ipIfaceIid, NULL)) != ZCFG_SUCCESS)
		return ret;
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE, &ipIfaceIid, &ipIfaceObj)) != ZCFG_SUCCESS)
		return ret;

	dbg_printf("line:%d\n", __LINE__);

	//Name = json_object_get_string(json_object_object_get(Jobj, "Name"));
	//Mode = json_object_get_string(json_object_object_get(Jobj, "Mode"));

	//set defalult values
	//json_object_object_add(ipIfaceObj, "Enable", json_object_new_boolean(true));//set to false here, trigger to true after WAN add comlete
	replaceParam(ipIfaceObj, "Enable", Jobj, "Enable");
	json_object_object_add(ipIfaceObj, "X_ZYXEL_ConnectionId", json_object_new_int(ipIfaceIid.idx[0] - 1));
	json_object_object_add(ipIfaceObj, "X_ZYXEL_IPR2_MARKING", json_object_new_int(ipIfaceIid.idx[0] - 1));

	//set Name and Mode
	json_object_object_add(ipIfaceObj, "X_ZYXEL_SrvName", json_object_new_string(Name));
	json_object_object_add(ipIfaceObj, "X_ZYXEL_ConnectionType", json_object_new_string(Mode));

	dbg_printf("line:%d\n", __LINE__);

	if(!strcmp(Mode, "IP_Bridged")){
		json_object_object_add(ipIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));
		//ret = zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &ipIfaceIid, ipIfaceObj, NULL);
		//json_object_put(ipIfaceObj);
		return ret;
	}

	replaceParam(ipIfaceObj, "X_ZYXEL_ConcurrentWan", Jobj, "ConcurrentWan");
		
	dbg_printf("line:%d\n", __LINE__);
	sprintf(ipIfacePath, "IP.Interface.%u", ipIfaceIid.idx[0]);

	//routting mode, need to set Nat and Static DNS before set IP and PPP.
	editNatSetting(Jobj);
	editDnsSetting(Jobj);
	
	dbg_printf("line:%d\n", __LINE__);
	if(!strcmp(ipMode, "IPv4")){
		json_object_object_add(ipIfaceObj, "IPv4Enable", json_object_new_boolean(true));
		replaceParam(ipIfaceObj, "X_ZYXEL_Enable_6RD", Jobj, "Enable_6RD");
		replaceParam(ipIfaceObj, "X_ZYXEL_6RD_Type", Jobj, "v6RD_Type");
		replaceParam(ipIfaceObj, "X_ZYXEL_SPIPv6Prefix", Jobj, "SPIPv6Prefix");
		replaceParam(ipIfaceObj, "X_ZYXEL_IPv4MaskLength", Jobj, "IPv4MaskLength");
		replaceParam(ipIfaceObj, "X_ZYXEL_BorderRelayIPv4Addresses", Jobj, "BorderRelayIPv4Addresses");
	}
	if(!strcmp(ipMode, "IPv6")){
		json_object_object_add(ipIfaceObj, "IPv6Enable", json_object_new_boolean(true));
		replaceParam(ipIfaceObj, "X_ZYXEL_Enable_DSLite", Jobj, "DSLiteEnable");
		replaceParam(ipIfaceObj, "X_ZYXEL_DSLite_Type", Jobj, "DSLiteType");
		replaceParam(ipIfaceObj, "X_ZYXEL_DSLiteRelayIPv6Addresses", Jobj, "DSLiteRelayIPv6Addresses");
         /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvManagedFlag_Manual", Jobj, "IPv6Manual_IANA");
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvOtherConfigFlag_Manual", Jobj, "IPv6Manual_IAPD");       
	}
	else if(!strcmp(ipMode, "DualStack")){
		json_object_object_add(ipIfaceObj, "IPv4Enable", json_object_new_boolean(true));
		json_object_object_add(ipIfaceObj, "IPv6Enable", json_object_new_boolean(true));
        /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvManagedFlag_Manual", Jobj, "IPv6Manual_IANA");
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvOtherConfigFlag_Manual", Jobj, "IPv6Manual_IAPD");
	}

	json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("AddSetIpIface"));

	if(!strcmp(Encapsulation, "PPPoE") || !strcmp(Encapsulation, "PPPoA")){
		ret = editPppIfaceObject(Jobj, ipIfaceObj, &ipIfaceIid);
	}
	else{
		replaceParam(ipIfaceObj, "MaxMTUSize", Jobj, "MaxMTUSize");
		json_object_object_add(ipIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));
		//ret = zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &ipIfaceIid, ipIfaceObj, NULL);
		dbg_printf("line:%d\n", __LINE__);
		sprintf(currLowerLayers, "IP.Interface.%u", ipIfaceIid.idx[0]);
		addStaticAddrSetting(Jobj);
		addDhcpSetting(Jobj);
    	dbg_printf("line=%d\n", __LINE__);
	}
	//json_object_put(ipIfaceObj);

	editIgmpMldSetting(Jobj);
	editDefaultGwSetting(Jobj);
	
	IID_INIT(dnsIid);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DNS, &dnsIid, &dnsObj);
	//ret = zcfgFeObjJsonSet(RDM_OID_DNS, &dnsIid, dnsObj, NULL);
	//json_object_put(dnsObj);

	dbg_printf("line=%d\n", __LINE__);
	
	return ret;
}

zcfgRet_t editLayer3Objects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	dbg_printf("line:%d\n", __LINE__);
#ifdef ISKON_CUSTOMIZATION //Adam
	int mark;
#endif
	if(isAdd){
		IID_INIT(ipIfaceIid);
		if((ret = zcfgFeObjJsonAdd(RDM_OID_IP_IFACE, &ipIfaceIid, NULL)) != ZCFG_SUCCESS)
			return ret;
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE, &ipIfaceIid, &ipIfaceObj)) != ZCFG_SUCCESS)
			return ret;

		sprintf(ipIfacePath, "IP.Interface.%u", ipIfaceIid.idx[0]);

		IID_INIT(dnsIid);
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DNS, &dnsIid, &dnsObj);
	}

	dbg_printf("line:%d\n", __LINE__);
#ifdef ISKON_CUSTOMIZATION //Adam
	mark = json_object_get_int(json_object_object_get(ipIfaceObj, "X_ZYXEL_IPR2_MARKING"));
	//printf("%s(%d)  mark value %d\n",__FUNCTION__, __LINE__, mark);
#endif
	//json_object_object_add(ipIfaceObj, "Enable", json_object_new_boolean(true));
	if(!replaceParam(ipIfaceObj, "Enable", Jobj, "Enable"))
		json_object_object_add(ipIfaceObj, "Enable", json_object_new_boolean(CurrEnable));
	json_object_object_add(ipIfaceObj, "X_ZYXEL_ConnectionId", json_object_new_int(ipIfaceIid.idx[0] - 1));
#ifdef ISKON_CUSTOMIZATION //Adam
	json_object_object_add(ipIfaceObj, "X_ZYXEL_IPR2_MARKING", json_object_new_int(mark));
#else
	json_object_object_add(ipIfaceObj, "X_ZYXEL_IPR2_MARKING", json_object_new_int(ipIfaceIid.idx[0] - 1));
#endif
	
	json_object_object_add(ipIfaceObj, "X_ZYXEL_SrvName", json_object_new_string(Name));
	json_object_object_add(ipIfaceObj, "X_ZYXEL_ConnectionType", json_object_new_string(Mode));

	dbg_printf("line:%d\n", __LINE__);

	if(!strcmp(Mode, "IP_Bridged")){
		json_object_object_add(ipIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));
		return ret;
	}

	replaceParam(ipIfaceObj, "X_ZYXEL_ConcurrentWan", Jobj, "ConcurrentWan");
	
	dbg_printf("line:%d\n", __LINE__);
	if(!strcmp(ipMode, "IPv4")){
		json_object_object_add(ipIfaceObj, "IPv4Enable", json_object_new_boolean(true));
		json_object_object_add(ipIfaceObj, "IPv6Enable", json_object_new_boolean(false));
		replaceParam(ipIfaceObj, "X_ZYXEL_Enable_6RD", Jobj, "Enable_6RD");
		replaceParam(ipIfaceObj, "X_ZYXEL_6RD_Type", Jobj, "v6RD_Type");
		replaceParam(ipIfaceObj, "X_ZYXEL_SPIPv6Prefix", Jobj, "SPIPv6Prefix");
		replaceParam(ipIfaceObj, "X_ZYXEL_IPv4MaskLength", Jobj, "IPv4MaskLength");
		replaceParam(ipIfaceObj, "X_ZYXEL_BorderRelayIPv4Addresses", Jobj, "BorderRelayIPv4Addresses");
		json_object_object_add(ipIfaceObj, "X_ZYXEL_Enable_DSLite", json_object_new_boolean(false));
	}
	if(!strcmp(ipMode, "IPv6")){
		json_object_object_add(ipIfaceObj, "IPv4Enable", json_object_new_boolean(false));
		json_object_object_add(ipIfaceObj, "IPv6Enable", json_object_new_boolean(true));
		replaceParam(ipIfaceObj, "X_ZYXEL_Enable_DSLite", Jobj, "DSLiteEnable");
		replaceParam(ipIfaceObj, "X_ZYXEL_DSLite_Type", Jobj, "DSLiteType");
		replaceParam(ipIfaceObj, "X_ZYXEL_DSLiteRelayIPv6Addresses", Jobj, "DSLiteRelayIPv6Addresses");
		json_object_object_add(ipIfaceObj, "X_ZYXEL_Enable_6RD", json_object_new_boolean(false));
        /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvManagedFlag_Manual", Jobj, "IPv6Manual_IANA");
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvOtherConfigFlag_Manual", Jobj, "IPv6Manual_IAPD");

	}
	else if(!strcmp(ipMode, "DualStack")){
		json_object_object_add(ipIfaceObj, "IPv4Enable", json_object_new_boolean(true));
		json_object_object_add(ipIfaceObj, "IPv6Enable", json_object_new_boolean(true));
		json_object_object_add(ipIfaceObj, "X_ZYXEL_Enable_6RD", json_object_new_boolean(false));
		json_object_object_add(ipIfaceObj, "X_ZYXEL_Enable_DSLite", json_object_new_boolean(false));
        /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvManagedFlag_Manual", Jobj, "IPv6Manual_IANA");
        replaceParam(ipIfaceObj, "X_ZYXEL_AdvOtherConfigFlag_Manual", Jobj, "IPv6Manual_IAPD");      
	}

	if(!strcmp(Encapsulation, "PPPoE") || !strcmp(Encapsulation, "PPPoA")){
		ret = editPppIfaceObject(Jobj, ipIfaceObj, &ipIfaceIid);
	}
	else{
		json_object_object_add(ipIfaceObj, "MaxMTUSize", json_object_new_int(1500));
		replaceParam(ipIfaceObj, "MaxMTUSize", Jobj, "MaxMTUSize");
		const char *lowerLayers = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
		if(lowerLayers && strcmp(lowerLayers, currLowerLayers))
			json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));

		json_object_object_add(ipIfaceObj, "LowerLayers", json_object_new_string(currLowerLayers));
		sprintf(currLowerLayers, "IP.Interface.%u", ipIfaceIid.idx[0]);
	}

   	dbg_printf("line=%d\n", __LINE__);

	editNatSetting(Jobj);
	editDnsSetting(Jobj);
	editStaticAddrSetting(Jobj);
	editDhcpSetting(Jobj);
	editIgmpMldSetting(Jobj);
	editDefaultGwSetting(Jobj);

	dbg_printf("line=%d\n", __LINE__);
	
	return ret;
}

zcfgRet_t setAllObjects(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	
	if(atmLinkQosObj) zcfgFeObjJsonSet(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, atmLinkQosObj, NULL);
	if(atmLinkObj)zcfgFeObjJsonSet(RDM_OID_ATM_LINK, &atmLinkIid, atmLinkObj, NULL);
	if(ptmLinkObj)zcfgFeObjJsonSet(RDM_OID_PTM_LINK, &ptmLinkIid, ptmLinkObj, NULL);
	if(ethLinkObj)zcfgFeObjJsonSet(RDM_OID_ETH_LINK, &ethLinkIid, ethLinkObj, NULL);
	
	if(vlanTermObj)
	{  
        //When editing PPPoE or PPPoA wan, do not set RDM_OID_ETH_VLAN_TERM to avoid to trigger pppoe daemon twice
		//Trigger pppoe daemon only in setting RDM_OID_PPP_IFACE
		if(!isAdd && (Encapsulation != NULL))
		{
	        if((!strcmp(Encapsulation, "PPPoE"))){
				if(!strcmp(CurrEncapsulation, "PPPoA") || !strcmp(CurrEncapsulation, "IPoA")){
	       			zcfgFeObjJsonSet(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, vlanTermObj, NULL);
				}
				else{
	        		zcfgFeObjJsonSetWithoutApply(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, vlanTermObj, NULL);
				}
	        }
		    else{
				zcfgFeObjJsonSet(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, vlanTermObj, NULL);
    		}
    	}
		else 
	    {
	        zcfgFeObjJsonSet(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, vlanTermObj, NULL);
	    }
	}
	if(!strcmp(Mode, "IP_Bridged")){
		dbg_printf("IP_Bridged\n");
#if 0
		if(!isAdd && !strcmp(CurrMode, "IP_Routed")){ //route to bridge case , need to restart or reload service?
			if(igmpObj)zcfgFeObjJsonSet(RDM_OID_ZY_IGMP, &igmpIid, igmpObj, NULL);
			if(mldObj)zcfgFeObjJsonSet(RDM_OID_ZY_MLD, &mldIid, mldObj, NULL);
			if(dnsObj)zcfgFeObjJsonSet(RDM_OID_DNS, &dnsIid, dnsObj, NULL);
		}
#endif
		if(v4AddrObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, v4AddrObj, NULL);
        if(ipIfaceObj)zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &ipIfaceIid, ipIfaceObj, NULL);
		return ret;
	}
	
	if(natIntfObj)zcfgFeObjJsonSet(RDM_OID_NAT_INTF_SETTING, &natIntfIid, natIntfObj, NULL);
	if(dnsClientSrv4Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DNS_CLIENT_SRV, &dnsClientSrv4Iid, dnsClientSrv4Obj, NULL);
	if(dnsClientSrv6Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DNS_CLIENT_SRV, &dnsClientSrv6Iid, dnsClientSrv6Obj, NULL);
	if(v4FwdObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_ROUTING_ROUTER_V4_FWD, &v4FwdIid, v4FwdObj, NULL);
	if(v4AddrObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, v4AddrObj, NULL);
	if(v6FwdObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_ROUTING_ROUTER_V6_FWD, &v6FwdIid, v6FwdObj, NULL);
	if(v6AddrObj)zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, v6AddrObj, NULL);
#if 0//https://its.mitrastar.com/issues/20760
	if(ipIfaceObj)zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &ipIfaceIid, ipIfaceObj, NULL);
#endif
	if(pppIfaceObj)zcfgFeObjJsonSet(RDM_OID_PPP_IFACE, &pppIfaceIid, pppIfaceObj, NULL);
	if(opt42Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_REQ_OPT, &opt42Iid, opt42Obj, NULL);
	if(opt43Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_REQ_OPT, &opt43Iid, opt43Obj, NULL);
	if(opt120Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_REQ_OPT, &opt120Iid, opt120Obj, NULL);
	if(opt121Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_REQ_OPT, &opt121Iid, opt121Obj, NULL);
	if(opt212Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_REQ_OPT, &opt212Iid, opt212Obj, NULL);
	if(opt60Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_SENT_OPT, &opt60Iid, opt60Obj, NULL);
	if(opt61Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_SENT_OPT, &opt61Iid, opt61Obj, NULL);
	if(opt125Obj)zcfgFeObjJsonSetWithoutApply(RDM_OID_DHCPV4_SENT_OPT, &opt125Iid, opt125Obj, NULL);
	if(v4ClientObj)zcfgFeObjJsonSet(RDM_OID_DHCPV4_CLIENT, &v4ClientIid, v4ClientObj, NULL);
	if(igmpObj)zcfgFeObjJsonSet(RDM_OID_ZY_IGMP, &igmpIid, igmpObj, NULL);
	if(mldObj)zcfgFeObjJsonSet(RDM_OID_ZY_MLD, &mldIid, mldObj, NULL);
	if(routerObj)zcfgFeObjJsonBlockedSet(RDM_OID_ROUTING_ROUTER, &routerIid, routerObj, NULL);
	if(dnsObj)zcfgFeObjJsonSet(RDM_OID_DNS, &dnsIid, dnsObj, NULL);
	
	return ret;
}

zcfgRet_t freeAllObjects(){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(atmLinkQosObj) zcfgFeJsonObjFree(atmLinkQosObj);
	if(atmLinkObj) zcfgFeJsonObjFree(atmLinkObj);
	if(ptmLinkObj) zcfgFeJsonObjFree(ptmLinkObj);
	if(ethLinkObj) zcfgFeJsonObjFree(ethLinkObj);
	if(vlanTermObj) zcfgFeJsonObjFree(vlanTermObj);
	if(pppIfaceObj) zcfgFeJsonObjFree(pppIfaceObj);
	if(ipIfaceObj) zcfgFeJsonObjFree(ipIfaceObj);
	if(natIntfObj) zcfgFeJsonObjFree(natIntfObj);
	if(dnsClientSrv4Obj) zcfgFeJsonObjFree(dnsClientSrv4Obj);
	if(dnsClientSrv6Obj) zcfgFeJsonObjFree(dnsClientSrv6Obj);
	if(v4FwdObj) zcfgFeJsonObjFree(v4FwdObj);
	if(v4AddrObj) zcfgFeJsonObjFree(v4AddrObj);
	if(v6FwdObj) zcfgFeJsonObjFree(v6FwdObj);
	if(v6AddrObj) zcfgFeJsonObjFree(v6AddrObj);
	if(opt42Obj) zcfgFeJsonObjFree(opt42Obj);
	if(opt43Obj) zcfgFeJsonObjFree(opt43Obj);
	if(opt120Obj) zcfgFeJsonObjFree(opt120Obj);
	if(opt121Obj) zcfgFeJsonObjFree(opt121Obj);
	if(opt212Obj) zcfgFeJsonObjFree(opt212Obj);
	if(opt60Obj) zcfgFeJsonObjFree(opt60Obj);
	if(opt61Obj) zcfgFeJsonObjFree(opt61Obj);
	if(opt125Obj) zcfgFeJsonObjFree(opt125Obj);
	if(v4ClientObj) zcfgFeJsonObjFree(v4ClientObj);
	if(igmpObj) zcfgFeJsonObjFree(igmpObj);
	if(mldObj) zcfgFeJsonObjFree(mldObj);
	if(routerObj) zcfgFeJsonObjFree(routerObj);
	if(dnsObj) zcfgFeJsonObjFree(dnsObj);
	if(ethIfaceObj) zcfgFeJsonObjFree(ethIfaceObj);
	if(optIfaceObj) zcfgFeJsonObjFree(optIfaceObj);
	if(cellIfaceObj) zcfgFeJsonObjFree(cellIfaceObj);
	if(dynamicDnsClientSrv4Obj) zcfgFeJsonObjFree(dynamicDnsClientSrv4Obj);
	if(dynamicDnsClientSrv6Obj) zcfgFeJsonObjFree(dynamicDnsClientSrv6Obj);
	
	return ret;
}


void getBasicInfo(struct json_object *Jobj){
	Name = json_object_get_string(json_object_object_get(Jobj, "Name"));
	Type = json_object_get_string(json_object_object_get(Jobj, "Type"));
	Mode = json_object_get_string(json_object_object_get(Jobj, "Mode"));
	Encapsulation = json_object_get_string(json_object_object_get(Jobj, "Encapsulation"));
	ipMode = json_object_get_string(json_object_object_get(Jobj, "ipMode"));
	ipStatic = json_object_get_boolean(json_object_object_get(Jobj, "ipStatic"));
	ip6Static = json_object_get_boolean(json_object_object_get(Jobj, "ip6Static"));
	ip6DnsStatic = json_object_get_boolean(json_object_object_get(Jobj, "ip6DnsStatic"));
	Enable_6RD = json_object_get_boolean(json_object_object_get(Jobj, "Enable_6RD"));
	v6RD_Type = json_object_get_string(json_object_object_get(Jobj, "v6RD_Type"));
#ifdef ZYXEL_WAN_MAC
	WanMacType = json_object_get_string(json_object_object_get(Jobj, "WanMacType"));
	WanMacAddr = json_object_get_string(json_object_object_get(Jobj, "WanMacAddr"));
	WanMacIP = json_object_get_string(json_object_object_get(Jobj, "WanMacIP"));
#endif

	if(isDelete)
	{	deleteipIfacePath = json_object_get_string(json_object_object_get(Jobj, "Del_ipIfacePath"));	}
}

void initGlobalObjects(){
	
	atmLinkObj = NULL;
	atmLinkQosObj = NULL;
	ptmLinkObj = NULL;
	ethIfaceObj = NULL;
	optIfaceObj = NULL;
	cellIfaceObj = NULL;
	ethLinkObj = NULL;
	vlanTermObj = NULL;
	pppIfaceObj = NULL;
	ipIfaceObj = NULL;
	v4ClientObj = NULL;
	opt42Obj = NULL;
	opt43Obj = NULL;
	opt120Obj = NULL;
	opt121Obj = NULL;
	opt212Obj = NULL;
	opt60Obj = NULL;
	opt61Obj = NULL;
	opt125Obj = NULL;
	igmpObj = NULL;
	mldObj = NULL;
	natIntfObj = NULL;
	dnsClientSrv4Obj = NULL;
	dnsClientSrv6Obj = NULL;
	dynamicDnsClientSrv4Obj = NULL;
	dynamicDnsClientSrv6Obj = NULL;
	routerObj = NULL;
	v4FwdObj = NULL;
	v6FwdObj = NULL;
	//dynamicV4FwdObj = NULL;
	//dynamicV6FwdObj = NULL;
	v4AddrObj = NULL;
	v6AddrObj = NULL;
	dnsObj = NULL;

	IID_INIT(atmLinkIid);
	IID_INIT(atmLinkQosIid);
	IID_INIT(ptmLinkIid);
	IID_INIT(ethIfaceIid);
	IID_INIT(optIfaceIid);
	IID_INIT(cellIfaceIid);
	IID_INIT(ethLinkIid);
	IID_INIT(vlanTermIid);
	IID_INIT(pppIfaceIid);
	IID_INIT(ipIfaceIid);
	IID_INIT(v4ClientIid);
	IID_INIT(opt42Iid);
	IID_INIT(opt43Iid);
	IID_INIT(opt120Iid);
	IID_INIT(opt121Iid);
	IID_INIT(opt212Iid);
	IID_INIT(opt60Iid);
	IID_INIT(opt61Iid);
	IID_INIT(opt125Iid);
	IID_INIT(igmpIid);
	IID_INIT(mldIid);
	IID_INIT(natIntfIid);
	IID_INIT(ripIid);
	IID_INIT(dnsClientSrv4Iid);
	IID_INIT(dnsClientSrv6Iid);
	IID_INIT(dynamicDnsClientSrv4Iid);
	IID_INIT(dynamicDnsClientSrv6Iid);
	IID_INIT(routerIid);
	IID_INIT(v4FwdIid);
	IID_INIT(v6FwdIid);
	IID_INIT(v4AddrIid);
	IID_INIT(v6AddrIid);
	IID_INIT(dnsIid);

	memset(CurrType, 0, sizeof(CurrType));
	memset(CurrMode, 0, sizeof(CurrMode));
	memset(CurrEncapsulation, 0, sizeof(CurrEncapsulation));
	memset(CurrIpMode, 0, sizeof(CurrIpMode));
	memset(CurrV6RD_Type, 0, sizeof(CurrV6RD_Type));
	
	CurrIpStatic = false;
	CurrIp6Static = false;
	CurrPppStatic = false;
	CurrEnable_6RD = false;
	
}

zcfgRet_t getCurrentConfig(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *HigherLayer;
	const char *LowerLayer;
	struct json_object *obj = NULL;
	struct json_object *pramJobj = NULL;
	objIndex_t iid;
	int count = 0;
	bool v4enable, v6enable, Enable;
	const char *Interface;
	const char *AddressingType;
	const char *DestIPAddress;
	const char *DestSubnetMask;
	const char *Origin;
	bool staticRoute = false;
	const char *DestIPPrefix;
	const char *DNSServer;
	const char *X_ZYXEL_Type;
	const char *X_ZYXEL_LocalIPAddress;
	bool found = false;

	//check if target WAN exist
	if(isDelete == false)
	{
		if(!getSpecificObj(RDM_OID_IP_IFACE, "X_ZYXEL_SrvName", json_type_string, Name, &ipIfaceIid, &ipIfaceObj))
			return ZCFG_REQUEST_REJECT;
	}
	else
	{
		if(deleteipIfacePath != NULL)
		{
			if(strstr(deleteipIfacePath, "IP.Interface.") != NULL)
			{
				ipIfaceIid.level = 1;
				sscanf(deleteipIfacePath, "IP.Interface.%hhu", &ipIfaceIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE, &ipIfaceIid, &ipIfaceObj);
				if(ipIfaceObj)
				{	found = true;	}
			}
		}
		else{
			if(!getSpecificObj(RDM_OID_IP_IFACE, "X_ZYXEL_SrvName", json_type_string, Name, &ipIfaceIid, &ipIfaceObj))
				return ZCFG_REQUEST_REJECT;
			found = true;
		}
		
		if(!found)
		{	return ZCFG_REQUEST_REJECT;   }
	}
	
	sprintf(ipIfacePath, "IP.Interface.%u", ipIfaceIid.idx[0]);
	LowerLayer = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
	strcpy(CurrMode, json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConnectionType")));

	HigherLayer = ipIfacePath;
	while(count < 4){
		dbg_printf("count=%d\n", count);
		dbg_printf("HigherLayer=%s\n", HigherLayer);
		dbg_printf("LowerLayer=%s\n", LowerLayer);
		
		count++;
		if(!strncmp(HigherLayer, "IP.Interface.", 13)){
			if(!strncmp(LowerLayer, "PPP.Interface.", 14)){
				pppIfaceIid.level = 1;
				sscanf(LowerLayer, "PPP.Interface.%hhu", &pppIfaceIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PPP_IFACE, &pppIfaceIid, &pppIfaceObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(pppIfaceObj, "LowerLayers"));
			}
			else if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25)){
				vlanTermIid.level = 1;
				sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlanTermIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, &vlanTermObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
				strcpy(CurrEncapsulation, "IPoE");
			}
			else if(!strncmp("Ethernet.Link.", LowerLayer, 14)){
				ethLinkIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
				strcpy(CurrEncapsulation, "IPoE");
			}
			else if(!strncmp("ATM.Link.", LowerLayer, 9) && !strcmp("IP_Routed", CurrMode)){
				atmLinkIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
				strcpy(CurrEncapsulation, "IPoA");
				strcpy(CurrType, "ATM");
				memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
				break;
			}
			else{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp(HigherLayer, "PPP.Interface.", 14)){
			if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25)){
				vlanTermIid.level = 1;
				sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlanTermIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, &vlanTermObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
				strcpy(CurrEncapsulation, "PPPoE");
			}
			else if(!strncmp("ATM.Link.", LowerLayer, 9)){
				atmLinkIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
				strcpy(CurrEncapsulation, "PPPoA");
				strcpy(CurrType, "ATM");
				memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
				break;
			}
			else{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25)){
			if(!strncmp("Ethernet.Link.", LowerLayer, 14)){
				ethLinkIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj);
				HigherLayer = LowerLayer;
				LowerLayer = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
			}
			else{
				return ZCFG_REQUEST_REJECT;
			}
		}
		else if(!strncmp("Ethernet.Link.", HigherLayer, 14)){
			if(!strncmp("ATM.Link.", LowerLayer, 9)){
				atmLinkIid.level = 1;
				sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
				strcpy(CurrType, "ATM");
				memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
				break;
			}
			else if(!strncmp("PTM.Link.", LowerLayer, 9)){
				ptmLinkIid.level = 1;
				sscanf(LowerLayer, "PTM.Link.%hhu", &ptmLinkIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PTM_LINK, &ptmLinkIid, &ptmLinkObj);
				strcpy(CurrType, "PTM");
				break;
			}
			else if(!strncmp("Ethernet.Interface.", LowerLayer, 19)){
				ethIfaceIid.level = 1;
				sscanf(LowerLayer, "Ethernet.Interface.%hhu", &ethIfaceIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_IFACE, &ethIfaceIid, &ethIfaceObj);
				strcpy(CurrType, "ETH");
				if(json_object_get_boolean(json_object_object_get(ethIfaceObj, "Upstream")) == false){
					if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
						return ZCFG_REQUEST_REJECT;
					}
				}
				break;
			}
			else if(!strncmp("Optical.Interface.", LowerLayer, 18)){
				optIfaceIid.level = 1;
				sscanf(LowerLayer, "Optical.Interface.%hhu", &optIfaceIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_OPT_INTF, &optIfaceIid, &optIfaceObj);
				if(json_object_get_boolean(json_object_object_get(optIfaceObj, "Upstream")) == false){
					printf("The LowerLayer Optical Interface is a LAN interface.\n");
					return ZCFG_REQUEST_REJECT;
				}
				strcpy(CurrType, "GPON");
				break;
			}
			else if(!strncmp("Cellular.Interface.", LowerLayer, 19)){
				cellIfaceIid.level = 1;
				sscanf(LowerLayer, "Cellular.Interface.%hhu", &cellIfaceIid.idx[0]);
				zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &cellIfaceIid, &cellIfaceObj);
				if(json_object_get_boolean(json_object_object_get(cellIfaceObj, "Upstream")) == false){
					printf("The LowerLayer Cellular Interface is a LAN interface.\n");
					return ZCFG_REQUEST_REJECT;
				}
				strcpy(CurrType, "CELL");
				break;
			}
			else if(!strcmp("", LowerLayer)){
				if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
					return ZCFG_REQUEST_REJECT;
				}
				strcpy(CurrType, "ETH");
				break;
			}
			else{
				return ZCFG_REQUEST_REJECT;
			}
		}
	}

	if(isDelete)
		return ZCFG_SUCCESS;

	if(Type == NULL)
		Type = CurrType;
	if(Mode == NULL)
		Mode = CurrMode;

	//dont allow to change Type
	if(strcmp(CurrType, Type))
		return ZCFG_REQUEST_REJECT;

	if(!strcmp(Mode, "IP_Bridged")){
		json_object_object_add(Jobj, "IGMPEnable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "MLDEnable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "sysGwEnable", json_object_new_boolean(false));
		json_object_object_add(Jobj, "sysGw6Enable", json_object_new_boolean(false));

        /*Consider static ip/routing mode to bridge mode*/
		v4AddrIid.level = 2;
		v4AddrIid.idx[0] = ipIfaceIid.idx[0];
		v4AddrIid.idx[1] = 1;
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj);
		if(v4AddrObj){
			AddressingType = json_object_get_string(json_object_object_get(v4AddrObj, "AddressingType"));
			Enable = json_object_get_boolean(json_object_object_get(v4AddrObj, "Enable"));
			if(Enable == true && !strcmp(AddressingType, "Static") && strstr(CurrEncapsulation, "IP")){
				json_object_object_add(v4AddrObj, "Enable", json_object_new_boolean(false));
			}
		}

		v6AddrIid.level = 2;
		v6AddrIid.idx[0] = ipIfaceIid.idx[0];
		v6AddrIid.idx[1] = 1;
		zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj);
		if(v6AddrObj){
			Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
			Enable = json_object_get_boolean(json_object_object_get(v6AddrObj, "Enable"));
			if(Enable == true && !strcmp(Origin, "Static")){
				json_object_object_add(v6AddrObj, "Enable", json_object_new_boolean(false));
			}
		}

		return ret;
	}

	if(!strcmp(CurrEncapsulation, "IPoE") || !strcmp(CurrEncapsulation, "IPoA")){
		v4enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv4Enable"));
		v6enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv6Enable"));
	}
	else{ //PPPoE or PPPoA
		v4enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPCPEnable"));
		v6enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv6CPEnable"));
	}

    /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
    IPv6Manual_IANA = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv6Manual_IANA"));
    IPv6Manual_IAPD = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv6Manual_IAPD"));

	if(v4enable && v6enable)
		strcpy(CurrIpMode, "DualStack");
	else if(v4enable)
		strcpy(CurrIpMode, "IPv4");
	else if(v6enable)
		strcpy(CurrIpMode, "IPv6");

	if(Encapsulation == NULL)
		Encapsulation = CurrEncapsulation;
	if(ipMode == NULL)
		ipMode = CurrIpMode;

	getSpecificObj(RDM_OID_DHCPV4_CLIENT, "Interface", json_type_string, ipIfacePath, &v4ClientIid, &v4ClientObj);
	if(v4ClientObj){
		getDhcpv4ClientOpt(&v4ClientIid);
		if(!strcmp(CurrIpMode, "IPv4"))
			CurrEnable_6RD = json_object_get_boolean(json_object_object_get(ipIfaceObj, "X_ZYXEL_Enable_6RD"));
		
		if(CurrEnable_6RD)
			strcpy(CurrV6RD_Type, json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_6RD_Type")));
	}

	Enable_6RD = false;
	if(!strcmp(ipMode, "IPv4")){
		pramJobj = json_object_object_get(Jobj, "Enable_6RD");
		if(pramJobj)
			Enable_6RD = json_object_get_boolean(pramJobj);
		else
			Enable_6RD = CurrEnable_6RD;

		if(Enable_6RD){
			pramJobj = json_object_object_get(Jobj, "v6RD_Type");
			if(pramJobj){
				v6RD_Type = json_object_get_string(json_object_object_get(Jobj, "v6RD_Type"));
			}
			else{
				if(strcmp(CurrV6RD_Type, ""))
					v6RD_Type = CurrV6RD_Type;
				else{
					strcpy(CurrV6RD_Type, "dhcp");
					v6RD_Type = CurrV6RD_Type;
				}
			}

			if(!strcmp(v6RD_Type, "dhcp"))
				json_object_object_add(Jobj, "option212Enable", json_object_new_boolean(true));
		}
	}

	//NAT
	getSpecificObj(RDM_OID_NAT_INTF_SETTING, "Interface", json_type_string, ipIfacePath, &natIntfIid, &natIntfObj);
	//Default Gateway
	IID_INIT(routerIid);
	routerIid.level = 1;
	routerIid.idx[0] = 1;
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ROUTING_ROUTER, &routerIid, &routerObj);
	
	//static ipv4 Addr
	IID_INIT(v4AddrIid);
#if 1
	v4AddrIid.level = 2;
	v4AddrIid.idx[0] = ipIfaceIid.idx[0];
	v4AddrIid.idx[1] = 1;
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj);
	if(v4AddrObj){
		AddressingType = json_object_get_string(json_object_object_get(v4AddrObj, "AddressingType"));
		Enable = json_object_get_boolean(json_object_object_get(v4AddrObj, "Enable"));
		if(Enable == true && !strcmp(AddressingType, "Static") && strstr(CurrEncapsulation, "IP")){
			CurrIpStatic = true;
		}
	}
#else
	while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE_V4_ADDR, &ipIfaceIid, &v4AddrIid, &v4AddrObj) == ZCFG_SUCCESS){
		AddressingType = json_object_get_string(json_object_object_get(v4AddrObj, "AddressingType"));
		if(!strcmp(AddressingType, "Static")){
			break;
		}
		zcfgFeJsonObjFree(v4AddrObj);
	}
#endif
	if(CurrIpStatic){
		IID_INIT(v4FwdIid);
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_ROUTING_ROUTER_V4_FWD, &routerIid, &v4FwdIid, &v4FwdObj) == ZCFG_SUCCESS){
			Interface = json_object_get_string(json_object_object_get(v4FwdObj, "Interface"));
			//Origin = json_object_get_string(json_object_object_get(v4FwdObj, "Origin"));
			staticRoute =  json_object_get_boolean(json_object_object_get(v4FwdObj, "StaticRoute"));
			//if(!strcmp(Interface, ipIfacePath) && !strcmp(Origin, "Static")){
			if(!strcmp(Interface, ipIfacePath) && staticRoute == true){
				DestSubnetMask = json_object_get_string(json_object_object_get(v4FwdObj, "DestSubnetMask"));
				DestIPAddress = json_object_get_string(json_object_object_get(v4FwdObj, "DestIPAddress"));
				if(!strcmp(DestIPAddress, "") && !strcmp(DestSubnetMask, "")){
					break;
				}
			}
			zcfgFeJsonObjFree(v4FwdObj);
		}
	}

	//static ppp
	if(strstr(CurrEncapsulation, "PPP") && pppIfaceObj != NULL){
		X_ZYXEL_LocalIPAddress = json_object_get_string(json_object_object_get(pppIfaceObj, "X_ZYXEL_LocalIPAddress"));
		if(strcmp(X_ZYXEL_LocalIPAddress, "0.0.0.0") && strcmp(X_ZYXEL_LocalIPAddress, "")){
			CurrPppStatic = true;
		}
	}

	//static ipv6 Addr
	IID_INIT(v6AddrIid);
#if 1
	v6AddrIid.level = 2;
	v6AddrIid.idx[0] = ipIfaceIid.idx[0];
	v6AddrIid.idx[1] = 1;
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj);
	if(v6AddrObj){
		Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
		Enable = json_object_get_boolean(json_object_object_get(v6AddrObj, "Enable"));
		if(Enable == true && !strcmp(Origin, "Static")){
			CurrIp6Static = true;
		}
	}
#else
	while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE_V6_ADDR, &ipIfaceIid, &v6AddrIid, &v6AddrObj) == ZCFG_SUCCESS){
		Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
		if(!strcmp(Origin, "Static")){
			CurrIp6Static = true;
			break;
		}
		zcfgFeJsonObjFree(v6AddrObj);
	}
#endif
	//if(CurrIp6Static){
		IID_INIT(v6FwdIid);
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_ROUTING_ROUTER_V6_FWD, &routerIid, &v6FwdIid, &v6FwdObj) == ZCFG_SUCCESS){
			Interface = json_object_get_string(json_object_object_get(v6FwdObj, "Interface"));
			Origin = json_object_get_string(json_object_object_get(v6FwdObj, "Origin"));
			//if(!strcmp(Interface, ipIfacePath) && !strcmp(Origin, "Static")){
			if(!strcmp(Interface, ipIfacePath)){
				DestIPPrefix = json_object_get_string(json_object_object_get(v6FwdObj, "DestIPPrefix"));
				if(!strcmp(DestIPPrefix, "::/0") || !strcmp(DestIPPrefix, "")){
					break;
				}
			}
			zcfgFeJsonObjFree(v6FwdObj);
		}
	//}

	//static DNS
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DNS_CLIENT_SRV, &iid, &obj) == ZCFG_SUCCESS){
		Interface = json_object_get_string(json_object_object_get(obj, "Interface"));
		X_ZYXEL_Type = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_Type"));
		if(!strcmp(Interface, ipIfacePath) && !strcmp(X_ZYXEL_Type, "Static")){
			DNSServer = json_object_get_string(json_object_object_get(obj, "DNSServer"));
			if((strchr(DNSServer, ':') == NULL) && (dnsClientSrv4Obj == NULL)){
				dnsClientSrv4Obj = obj;
				memcpy(&dnsClientSrv4Iid, &iid, sizeof(objIndex_t));
			}
			else if(dnsClientSrv6Obj == NULL){
				dnsClientSrv6Obj = obj;
				memcpy(&dnsClientSrv6Iid, &iid, sizeof(objIndex_t));
			}
			else{
				zcfgFeJsonObjFree(obj);
			}
		}
		else if(!strcmp(Interface, ipIfacePath) && strcmp(X_ZYXEL_Type, "Static")){
			DNSServer = json_object_get_string(json_object_object_get(obj, "DNSServer"));
			if((strchr(DNSServer, ':') == NULL) && (dynamicDnsClientSrv4Obj == NULL)){
				dynamicDnsClientSrv4Obj = obj;
				memcpy(&dynamicDnsClientSrv4Iid, &iid, sizeof(objIndex_t));
			}
			else if(dynamicDnsClientSrv6Obj == NULL){
				dynamicDnsClientSrv6Obj = obj;
				memcpy(&dynamicDnsClientSrv6Iid, &iid, sizeof(objIndex_t));
			}
			else{
				zcfgFeJsonObjFree(obj);
			}
		}
		else{
			zcfgFeJsonObjFree(obj);
		}
	}
	
	//IGMP, MLD
	IID_INIT(igmpIid);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_IGMP, &igmpIid, &igmpObj);
	IID_INIT(mldIid);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_MLD, &mldIid, &mldObj);

	IID_INIT(dnsIid);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DNS, &dnsIid, &dnsObj);

	return ret;
}

void deleteUnnecessaryObj(){

	bool delPpp = false, delVlanEthLink = false;

	dbg_printf("CurrMode=%s, Mode=%s\n", CurrMode, Mode);

	if(!strcmp(CurrMode, "IP_Routed") && !strcmp(Mode, "IP_Bridged")){
		if(strstr(CurrEncapsulation, "PPP"))
			delPpp = true;
	}
	else if(!strcmp(CurrMode, "IP_Routed") && !strcmp(Mode, "IP_Routed")){
		if((!strcmp(CurrEncapsulation, "PPPoA") || !strcmp(CurrEncapsulation, "PPPoE")) &&
		   (!strcmp(Encapsulation, "IPoA") || !strcmp(Encapsulation, "IPoE"))){
		   delPpp = true;
		}
		if((!strcmp(CurrEncapsulation, "PPPoE") || !strcmp(CurrEncapsulation, "IPoE")) &&
		   (!strcmp(Encapsulation, "PPPoA") || !strcmp(Encapsulation, "IPoA"))){
		   delVlanEthLink = true;
		}
	}
	else if(!strcmp(CurrMode, "IP_Bridged") && !strcmp(Mode, "IP_Routed")){
		if(!strcmp(Encapsulation, "PPPoA") || !strcmp(Encapsulation, "IPoA"))
			delVlanEthLink = true;
	}

	if(delPpp){
		dbg_printf("delPpp\n");
		json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));
		zcfgFeObjJsonDel(RDM_OID_PPP_IFACE, &pppIfaceIid, NULL);
		zcfgFeJsonObjFree(pppIfaceObj);
	}

	if(delVlanEthLink){
		dbg_printf("delVlanEthLink\n");
		json_object_object_add(ipIfaceObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));
		zcfgFeObjJsonDel(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, NULL);
		zcfgFeObjJsonDel(RDM_OID_ETH_LINK, &ethLinkIid, NULL);
		zcfgFeJsonObjFree(vlanTermObj);
		zcfgFeJsonObjFree(ethLinkObj);
	}
	
}


/*
The same type of WAN interface can be added up to 8.
* Call getWanLanList return Jarray:
*    Jarray.i.Type : LAN|WAN
*    Jarray.i.LinkType = ATM|PTM|ETH|PON|USB (For WAN only)
* Inuput Parameter:
*    currObj.Type : ATM|PTM|ETH (WAN type be checked)
*    isAdd : 0 for edit WAN, 1 for add new WAN
*/
bool checkWanInterfaceLimit(struct json_object * currObj, struct json_object *wanLanList, int isAdd)
{
    struct json_object *listObj = NULL;
    int i, wanIntfCount = 0, len = 0;
    int MaxSameWANInterface = 8;

    len = json_object_array_length(wanLanList);
	if(json_object_object_get(currObj, "Type")){
	    for(i=0; i<len; i++){
	        listObj = json_object_array_get_idx(wanLanList, i);
	        if(!strcmp("WAN",json_object_get_string(json_object_object_get(listObj, "Type"))))
	        {
	            if(!strcmp(json_object_get_string(json_object_object_get(currObj, "Type")),json_object_get_string(json_object_object_get(listObj, "LinkType"))))
	            {
	                wanIntfCount ++;
	            }
	        }
	    }
	}
    if(isAdd)
        wanIntfCount++;
    
    if(MaxSameWANInterface < wanIntfCount)
    {
        dbg_printf("Type %s WAN interface count would be %d, over threshold %d\n",json_object_get_string(json_object_object_get(currObj, "Type")), wanIntfCount,MaxSameWANInterface);
        return false;
    }
    return true;
}

bool isVlanRepeat(char *type, struct json_object *Jobj, struct json_object *wanLanList, int isAdd)
{
    struct json_object *wanLanObj = NULL;
	struct json_object *curWanObj = NULL;
    int i, len = 0;
	const char *LinkType, *VpiVci, *Name;
	int VLANID;

	Name = Jgets(Jobj, "Name");
	if(Name == NULL)
		return false;
	
	if(isAdd){
		if((LinkType = Jgets(Jobj, "Type")) == NULL)
			LinkType = "";
		if((VpiVci = Jgets(Jobj, "vpivci")) == NULL)
			VpiVci = "";
		if(Jget(Jobj, "VLANID"))
			VLANID = Jgeti(Jobj, "VLANID");
		else
			VLANID = -1;
	}
	else{
		len = json_object_array_length(wanLanList);
		for(i=0; i<len; i++){
			wanLanObj = json_object_array_get_idx(wanLanList, i);
			if(!strcmp(Name, Jgets(wanLanObj, "Name")) && !strcmp("WAN", Jgets(wanLanObj, "Type"))){
				curWanObj = wanLanObj;
				break;
			}
		}
		if(curWanObj == NULL)
			return false;

		if((LinkType = Jgets(Jobj, "Type")) == NULL)
			LinkType = Jgets(curWanObj, "LinkType");
		if((VpiVci = Jgets(Jobj, "vpivci")) == NULL)
			VpiVci = Jgets(curWanObj, "VpiVci");
		if(Jget(Jobj, "VLANID"))
			VLANID = Jgeti(Jobj, "VLANID");
		else
			VLANID = Jgeti(curWanObj, "VLANID");
	}

	if(LinkType != NULL)
		strcpy(type, LinkType);

	len = json_object_array_length(wanLanList);
	for(i=0; i<len; i++){
		wanLanObj = json_object_array_get_idx(wanLanList, i);
		if(strcmp("WAN", Jgets(wanLanObj, "Type")) || !strcmp(Name, Jgets(wanLanObj, "Name"))){
			continue;
		}
		
		if(!strcmp(LinkType, Jgets(wanLanObj, "LinkType")) && (VLANID != -1) && (VLANID == Jgeti(wanLanObj, "VLANID"))){
			if(strcmp(LinkType, "ATM"))
				return true;
			else if(!strcmp(VpiVci, Jgets(wanLanObj, "VpiVci")))
				return true;
		}
	}

	return false;
}

zcfgRet_t zcfgFeDalWanAdd(struct json_object *Jobj, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *wanLanList = NULL, *wanLanObj = NULL;
	int i, len;
	char type[16] = {0};

	isAdd = true;
	isDelete = false;
	initGlobalObjects();
	getBasicInfo(Jobj);

#if defined(SAME_FW_FOR_TWO_BOARD)
    if (strcmp(Type, "ETH") == 0)
    {
        /* Not support ETHWAN */
        if (zyutil_cofw_modelNameGet() == MODEL_VMG1312_T20A)
        {
            return ZCFG_REQUEST_REJECT;
        }
    }
#endif

	getWanLanList(&wanLanList);

	//check if duplicate Name exist
	len = json_object_array_length(wanLanList);
	for(i=0; i<len; i++){
		wanLanObj = json_object_array_get_idx(wanLanList, i);
		if(!strcmp(Name, Jgets(wanLanObj, "Name")) && !strcmp("WAN", Jgets(wanLanObj, "Type"))){
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.duplicate_name"));
			if(replyMsg)
				strcpy(replyMsg,"Name is duplicate.");

			json_object_put(wanLanList);
			return ZCFG_REQUEST_REJECT;
		}
	}

	if(checkWanInterfaceLimit(Jobj, wanLanList, 1) == false){
		json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.exceed_wan_limit"));
		if(replyMsg)
			strcpy(replyMsg,"The number of WAN interfaces exceeds the limit.");

		json_object_put(wanLanList);
		return ZCFG_REQUEST_REJECT;
	}

	if(isVlanRepeat(type, Jobj, wanLanList, 1)){
		if(!strcmp(type, "ATM"))
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.vlan_pvc_repeat"));
		else
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.vlan_repeat"));
		if(replyMsg)
			strcpy(replyMsg,"The Vlan is repeat.");

		json_object_put(wanLanList);
		return ZCFG_REQUEST_REJECT;
	}
	json_object_put(wanLanList);
	
	ret = zcfgFeCheckIPDNS(Jobj, replyMsg);
	if((ret = addDefaultParam(Jobj, replyMsg)) != ZCFG_SUCCESS)//for CLI
		return ret;

	if((ret = editPhyLayerObjects(Jobj)) != ZCFG_SUCCESS)
		return ret;

	if((ret = editEthLinkObject(Jobj)) != ZCFG_SUCCESS)
		return ret;

	if((ret = editVlanTermObject(Jobj)) != ZCFG_SUCCESS)
		return ret;

	if((ret = addLayer3Objects(Jobj)) != ZCFG_SUCCESS)
		return ret;
	setAllObjects(Jobj);
#ifdef ZYXEL_SYNC_CCWAN_TO_ALL_DEFGW
	syncCCWanSetting(Jobj);
#endif
	freeAllObjects();
	
	return ret;
}

zcfgRet_t objectStepStackLowerLayer(const char *startLayer, const char *destLowLayer, char *higherLayer, char *lowerLayer)
{
	objIndex_t iid;
	zcfg_offset_t oid;
	const char *ptr = NULL;
	struct json_object *tmpJobj = NULL;
	char tr181Name[64] = {0};
	char buf[64];
	char *bufptr;

	zcfgLog(ZCFG_LOG_INFO, "%s\n", __FUNCTION__);

	if(!startLayer || !destLowLayer || !higherLayer)
		return ZCFG_INTERNAL_ERROR;

	higherLayer[0] = '\0';

	strncpy(tr181Name, startLayer, sizeof(tr181Name)-1);
	IID_INIT(iid);
	oid = zcfgFeObjNameToObjId(tr181Name, &iid);
	if((int)oid < 0)
		return ZCFG_NO_SUCH_OBJECT;

	while(zcfgFeObjJsonGetWithoutUpdate(oid, &iid, &tmpJobj) == ZCFG_SUCCESS){
		ptr = json_object_get_string(json_object_object_get(tmpJobj, "LowerLayers"));

		if (ptr == NULL || !strcmp(ptr, "")){
			json_object_put(tmpJobj);
			return ZCFG_NO_SUCH_OBJECT;
		}

		//prevent multi lowerlayers case
		strncpy(buf, ptr, sizeof(buf)-1);
		bufptr = strchr(buf, ',');
		if(bufptr)
			*bufptr = '\0';
		strcpy(lowerLayer, buf);

		json_object_put(tmpJobj);

		if(!strcmp(lowerLayer, destLowLayer)){
			strcpy(higherLayer, tr181Name);
			return ZCFG_SUCCESS;
		}
		else{
			strcpy(tr181Name, lowerLayer);
			IID_INIT(iid);
			oid = zcfgFeObjNameToObjId(tr181Name, &iid);
			if((int)oid < 0)
				return ZCFG_NO_SUCH_OBJECT;
		}
	}

	return ZCFG_INTERNAL_ERROR;
}

zcfgRet_t configEthIfaceWan(struct json_object *Jobj)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	char higherLayers[64] = {0}, lowerLayers[64] = {0};
	bool enable = json_object_get_boolean(json_object_object_get(Jobj, (const char *)"EnableEthWAN"));
	const char *ethIfaceName = json_object_get_string(json_object_object_get(Jobj, (const char *)"EthIfaceName"));
	struct json_object *ipIntObj = NULL, *ethIntfObj = NULL;
	objIndex_t ipIntIid, ethIntfIid;

	if(!ethIfaceName || !strstr(ethIfaceName, "Ethernet")){
		return ZCFG_INVALID_OBJECT;
	}

	dbg_printf("%s: enable/disable %d EthWAN %s\n", __FUNCTION__, (int)enable, ethIfaceName);

	IID_INIT(ethIntfIid);
	ethIntfIid.level = 1;
	sscanf(ethIfaceName, "Ethernet.Interface.%hhu", &ethIntfIid.idx[0]);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_IFACE, &ethIntfIid, &ethIntfObj)) != ZCFG_SUCCESS)
		return ret;

	json_object_object_add(ethIntfObj, "X_ZYXEL_Upstream", json_object_new_boolean(enable));
	if((ret = zcfgFeObjJsonBlockedSet(RDM_OID_ETH_IFACE, &ethIntfIid, ethIntfObj, NULL)) != ZCFG_SUCCESS){
		json_object_put(ethIntfObj);
		return ret;
	}
	json_object_put(ethIntfObj);

	if(enable != true)
		return ZCFG_SUCCESS;

	IID_INIT(ipIntIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE, &ipIntIid, &ipIntObj) == ZCFG_SUCCESS) {
		const char *ipIfaceLowerLayers = json_object_get_string(json_object_object_get(ipIntObj, "LowerLayers"));
		if(ipIfaceLowerLayers && strlen(ipIfaceLowerLayers)){
			if(objectStepStackLowerLayer(ipIfaceLowerLayers, ethIfaceName, higherLayers, lowerLayers) == ZCFG_SUCCESS){
				break;
			}
			json_object_put(ipIntObj);
			ipIntObj = NULL;
		}
	}

	if(ipIntObj){
		json_object_object_add(ipIntObj, "X_ZYXEL_AccessDescription", json_object_new_string("SetIpIfaceStackModified"));
		if((ret = zcfgFeObjJsonBlockedSet(RDM_OID_IP_IFACE, &ipIntIid, ipIntObj, NULL)) != ZCFG_SUCCESS){
			json_object_put(ipIntObj);
			return ret;
		}
		json_object_put(ipIntObj);
	}

	return ZCFG_SUCCESS;
}


zcfgRet_t zcfgFeDalWanEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *wanLanList = NULL;
	char type[16] = {0};
	
	isAdd = false;
	isDelete = false;
	initGlobalObjects();
	getBasicInfo(Jobj);

	/*
	if(json_object_object_get(Jobj, "EnableEthWAN")){
		// it's EthWAN enable/disable
		if(configEthIfaceWan(Jobj) == ZCFG_SUCCESS)
			zcfgFeReqReboot(NULL);

		goto dalwan_edit_fail;
	}
	else
	*/
		ret = zcfgFeCheckIPDNS(Jobj, replyMsg);

	
	getWanLanList(&wanLanList);
	if(checkWanInterfaceLimit(Jobj, wanLanList, 0) == false){
		json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.exceed_wan_limit"));
		if(replyMsg)
			strcpy(replyMsg,"The number of WAN interfaces exceeds the limit.");
		json_object_put(wanLanList);
		ret = ZCFG_REQUEST_REJECT;
		goto dalwan_edit_fail;
	}
	if(isVlanRepeat(type, Jobj, wanLanList, 0)){
		if(!strcmp(type, "ATM"))
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.vlan_pvc_repeat"));
		else
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.Broadband.warning.vlan_repeat"));
		if(replyMsg)
			strcpy(replyMsg,"The Vlan is repeat.");
		json_object_put(wanLanList);
		ret = ZCFG_REQUEST_REJECT;
		goto dalwan_edit_fail;
	}
	json_object_put(wanLanList);

	if ((ret = getCurrentConfig(Jobj)) != ZCFG_SUCCESS)
		goto dalwan_edit_fail;
	//disable ipIface pppIface first
	CurrEnable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "Enable"));
#if 0//https://its.mitrastar.com/issues/20760
	json_object_object_add(ipIfaceObj, "Enable", json_object_new_boolean(false));
	zcfgFeObjJsonBlockedSet(RDM_OID_IP_IFACE, &ipIfaceIid, ipIfaceObj, NULL);
#endif
	if(pppIfaceObj){
		json_object_object_add(pppIfaceObj, "Enable", json_object_new_boolean(false));
		zcfgFeObjJsonSet(RDM_OID_PPP_IFACE, &pppIfaceIid, pppIfaceObj, NULL);
	}

#if 0 // don't need clean dns,zcfg_be will handle everything.
	//Always clean dynamic dns
	if(dynamicDnsClientSrv4Obj){
		json_object_object_add(dynamicDnsClientSrv4Obj, "Interface", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv4Obj, "X_ZYXEL_Type", json_object_new_string("Static"));
		json_object_object_add(dynamicDnsClientSrv4Obj, "DNSServer", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv4Obj, "X_ZYXEL_Ifname", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv4Obj, "X_ZYXEL_GwAddr", json_object_new_string(""));
		zcfgFeObjJsonSet(RDM_OID_DNS_CLIENT_SRV, &dynamicDnsClientSrv4Iid, dynamicDnsClientSrv4Obj, NULL);
		zcfgFeJsonObjFree(dynamicDnsClientSrv4Obj);
	}
	if(dynamicDnsClientSrv6Obj){
		json_object_object_add(dynamicDnsClientSrv6Obj, "Interface", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv6Obj, "X_ZYXEL_Type", json_object_new_string("Static"));
		json_object_object_add(dynamicDnsClientSrv6Obj, "DNSServer", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv6Obj, "X_ZYXEL_Ifname", json_object_new_string(""));
		json_object_object_add(dynamicDnsClientSrv6Obj, "X_ZYXEL_GwAddr", json_object_new_string(""));
		zcfgFeObjJsonSet(RDM_OID_DNS_CLIENT_SRV, &dynamicDnsClientSrv6Iid, dynamicDnsClientSrv6Obj, NULL);
		zcfgFeJsonObjFree(dynamicDnsClientSrv6Obj);
	}
#endif

/* __ZYXEL__, Chi-Hsiang When WAN IPv6 address mode change from dynamic ip to static ip,
	may encounter some data model error that BE has deleted RDM_OID_ROUTING_ROUTER_V6_FWD
	and RDM_OID_IP_IFACE_V6_ADDR, which results in setAllObjects() couldn't find curObj to set.
	Sympton: atfer set IPv6 address from dynamic ip to static and apply, web GUI still shows
	dynamic ip.
*/
	if(ip6Static){
		zcfgFeObjJsonDel(RDM_OID_ROUTING_ROUTER_V6_FWD, &v6FwdIid, NULL);
		zcfgFeJsonObjFree(v6FwdObj);
		v6FwdObj = NULL;
		
		if(!CurrIp6Static){
			zcfgFeObjJsonDel(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, NULL);
			zcfgFeJsonObjFree(v6AddrObj);
			v6AddrObj = NULL;
		}
	}

	if(!ip6Static){
		if(CurrIp6Static){
			zcfgFeObjJsonDel(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, NULL);
		}
	}
	
	if((ip6DnsStatic)){
		zcfgFeObjJsonDel(RDM_OID_DNS_CLIENT_SRV, &dynamicDnsClientSrv6Iid, NULL);
		zcfgFeJsonObjFree(dynamicDnsClientSrv6Obj);
		dynamicDnsClientSrv6Obj = NULL;
	}
	
	deleteUnnecessaryObj();
	if((ret = editPhyLayerObjects(Jobj)) != ZCFG_SUCCESS)
		goto dalwan_edit_fail;
	if((ret = editEthLinkObject(Jobj)) != ZCFG_SUCCESS)
		goto dalwan_edit_fail;

	if((ret = editVlanTermObject(Jobj)) != ZCFG_SUCCESS)
		goto dalwan_edit_fail;
	
	if((ret = editLayer3Objects(Jobj)) != ZCFG_SUCCESS)
		goto dalwan_edit_fail;

	setAllObjects(Jobj);
#ifdef ZYXEL_SYNC_CCWAN_TO_ALL_DEFGW
	syncCCWanSetting(Jobj);
#endif

dalwan_edit_fail:
	freeAllObjects();
	
	return ret;
}

zcfgRet_t zcfgFeDalWanDelete(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	isDelete = true;
	initGlobalObjects();
	getBasicInfo(Jobj);
	getCurrentConfig(Jobj);

	if(ipIfaceObj)
		zcfgFeObjJsonDel(RDM_OID_IP_IFACE, &ipIfaceIid, NULL);

	if(pppIfaceObj)
		zcfgFeObjJsonDel(RDM_OID_PPP_IFACE, &pppIfaceIid, NULL);

	if(vlanTermObj)
		zcfgFeObjJsonDel(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, NULL);

	if(ethLinkObj)
		zcfgFeObjJsonDel(RDM_OID_ETH_LINK, &ethLinkIid, NULL);

	/*if(ptmLinkObj){
		if(!isXTMLinkHasReference(Jobj))
			zcfgFeObjJsonDel(RDM_OID_PTM_LINK, &ptmLinkIid, NULL);
	}*/

	if(atmLinkObj){
		if(!isXTMLinkHasReference(Jobj))
			zcfgFeObjJsonDel(RDM_OID_ATM_LINK, &atmLinkIid, NULL);
	}

	freeAllObjects();
	return ret;
}

zcfgRet_t parseIpIfaceObj(struct json_object *pramJobj,objIndex_t ipIfaceIid, struct json_object *ipIfaceObj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	const char *HigherLayer;
	const char *LowerLayer;
	int count = 0;

	sprintf(ipIfacePath, "IP.Interface.%u", ipIfaceIid.idx[0]);
	dbg_printf("%s: enter , ipIfacePath = %s\n",__FUNCTION__,ipIfacePath);

	LowerLayer = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
	strcpy(CurrMode, json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConnectionType")));
	
		HigherLayer = ipIfacePath;
		while(count < 4){
			dbg_printf("count=%d\n", count);
			dbg_printf("HigherLayer=%s\n", HigherLayer);
			dbg_printf("LowerLayer=%s\n", LowerLayer);
			
			count++;
			if(!strncmp(HigherLayer, "IP.Interface.", 13)){
				if(!strncmp(LowerLayer, "PPP.Interface.", 14)){
					pppIfaceIid.level = 1;
					sscanf(LowerLayer, "PPP.Interface.%hhu", &pppIfaceIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PPP_IFACE, &pppIfaceIid, &pppIfaceObj);
					HigherLayer = LowerLayer;
					LowerLayer = json_object_get_string(json_object_object_get(pppIfaceObj, "LowerLayers"));
				}
				else if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25)){
					vlanTermIid.level = 1;
					sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlanTermIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, &vlanTermObj);
					HigherLayer = LowerLayer;
					LowerLayer = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					strcpy(CurrEncapsulation, "IPoE");
				}
				else if(!strncmp("Ethernet.Link.", LowerLayer, 14)){
					ethLinkIid.level = 1;
					sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj);
					HigherLayer = LowerLayer;
					LowerLayer = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
					strcpy(CurrEncapsulation, "IPoE");
				}
				else if(!strncmp("ATM.Link.", LowerLayer, 9) && !strcmp("IP_Routed", CurrMode)){
					atmLinkIid.level = 1;
					sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
					strcpy(CurrEncapsulation, "IPoA");
					strcpy(CurrType, "ATM");
					memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
					break;
				}
				else{
					return ZCFG_REQUEST_REJECT;
				}
			}
			else if(!strncmp(HigherLayer, "PPP.Interface.", 14)){
				if(!strncmp("Ethernet.VLANTermination.", LowerLayer, 25)){
					vlanTermIid.level = 1;
					sscanf(LowerLayer, "Ethernet.VLANTermination.%hhu", &vlanTermIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_VLAN_TERM, &vlanTermIid, &vlanTermObj);
					HigherLayer = LowerLayer;
					LowerLayer = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					strcpy(CurrEncapsulation, "PPPoE");
				}
				else if(!strncmp("ATM.Link.", LowerLayer, 9)){
					atmLinkIid.level = 1;
					sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
					strcpy(CurrEncapsulation, "PPPoA");
					strcpy(CurrType, "ATM");
					memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
					break;
				}
				else{
					return ZCFG_REQUEST_REJECT;
				}
			}
			else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25)){
				if(!strncmp("Ethernet.Link.", LowerLayer, 14)){
					ethLinkIid.level = 1;
					sscanf(LowerLayer, "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj);
					HigherLayer = LowerLayer;
					LowerLayer = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
				}
				else{
					return ZCFG_REQUEST_REJECT;
				}
			}
			else if(!strncmp("Ethernet.Link.", HigherLayer, 14)){
				if(!strncmp("ATM.Link.", LowerLayer, 9)){
					atmLinkIid.level = 1;
					sscanf(LowerLayer, "ATM.Link.%hhu", &atmLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK, &atmLinkIid, &atmLinkObj);
					strcpy(CurrType, "ATM");
					memcpy(&atmLinkQosIid, &atmLinkIid, sizeof(objIndex_t));
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ATM_LINK_QOS, &atmLinkQosIid, &atmLinkQosObj);
					break;
				}
				else if(!strncmp("PTM.Link.", LowerLayer, 9)){
					ptmLinkIid.level = 1;
					sscanf(LowerLayer, "PTM.Link.%hhu", &ptmLinkIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_PTM_LINK, &ptmLinkIid, &ptmLinkObj);
					strcpy(CurrType, "PTM");
					break;
				}
				else if(!strncmp("Ethernet.Interface.", LowerLayer, 19)){
					ethIfaceIid.level = 1;
					sscanf(LowerLayer, "Ethernet.Interface.%hhu", &ethIfaceIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_IFACE, &ethIfaceIid, &ethIfaceObj);
					strcpy(CurrType, "ETH");
					if(json_object_get_boolean(json_object_object_get(ethIfaceObj, "Upstream")) == false){
						if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
							return ZCFG_REQUEST_REJECT;
						}
					}
					break;
				}
				else if(!strncmp("Optical.Interface.", LowerLayer, 18)){
					optIfaceIid.level = 1;
					sscanf(LowerLayer, "Optical.Interface.%hhu", &optIfaceIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_OPT_INTF, &optIfaceIid, &optIfaceObj);
					if(json_object_get_boolean(json_object_object_get(optIfaceObj, "Upstream")) == false){
						printf("The LowerLayer Optical Interface is a LAN interface.\n");
						return ZCFG_REQUEST_REJECT;
					}
					strcpy(CurrType, "GPON");
					break;
				}
				else if(!strncmp("Cellular.Interface.", LowerLayer, 19)){
					cellIfaceIid.level = 1;
					sscanf(LowerLayer, "Cellular.Interface.%hhu", &cellIfaceIid.idx[0]);
					zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF, &cellIfaceIid, &cellIfaceObj);
					if(json_object_get_boolean(json_object_object_get(cellIfaceObj, "Upstream")) == false){
						printf("The LowerLayer Cellular Interface is a LAN interface.\n");
						return ZCFG_REQUEST_REJECT;
					}
					strcpy(CurrType, "CELL");
					break;
				}
				else if(!strcmp("", LowerLayer)){
					if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
						return ZCFG_REQUEST_REJECT;
					}
					strcpy(CurrType, "ETH");
					break;
				}
				else{
					return ZCFG_REQUEST_REJECT;
				}
			}
		}

	//Basic param
	json_object_object_add(pramJobj, "Name", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_SrvName")));
	json_object_object_add(pramJobj, "Enable", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "Enable")));
	json_object_object_add(pramJobj, "Type", json_object_new_string(CurrType));
	json_object_object_add(pramJobj, "Mode", json_object_new_string(CurrMode));
	json_object_object_add(pramJobj, "VLANID", JSON_OBJ_COPY(json_object_object_get(vlanTermObj, "VLANID")));
	json_object_object_add(pramJobj, "VLANEnable", JSON_OBJ_COPY(json_object_object_get(vlanTermObj, "X_ZYXEL_VLANEnable")));
	json_object_object_add(pramJobj, "VLANPriority", JSON_OBJ_COPY(json_object_object_get(vlanTermObj, "X_ZYXEL_VLANPriority")));
	json_object_object_add(pramJobj, "Iid", json_object_new_int(ipIfaceIid.idx[0]));
	if(vlanTermObj == NULL){
		json_object_object_add(pramJobj, "VLANID", json_object_new_int(-1));
		json_object_object_add(pramJobj, "VLANPriority", json_object_new_int(-1));
	}

    /* __ZyXEL__, Albert, 20180209,Support IPv6 option IA_NA and IA_PD  */
	json_object_object_add(pramJobj, "IPv6Manual_IANA", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_AdvManagedFlag_Manual")));
	json_object_object_add(pramJobj, "IPv6Manual_IAPD", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_AdvOtherConfigFlag_Manual")));

	if(!strcmp(CurrType, "ATM")){
		json_object_object_add(pramJobj, "vpivci", JSON_OBJ_COPY(json_object_object_get(atmLinkObj, "DestinationAddress")));
		json_object_object_add(pramJobj, "AtmEncapsulation", JSON_OBJ_COPY(json_object_object_get(atmLinkObj, "Encapsulation")));
		json_object_object_add(pramJobj, "QoSClass", JSON_OBJ_COPY(json_object_object_get(atmLinkQosObj, "QoSClass")));
		json_object_object_add(pramJobj, "atmMaxBurstSize", JSON_OBJ_COPY(json_object_object_get(atmLinkQosObj, "MaximumBurstSize")));
		json_object_object_add(pramJobj, "atmSustainedCellRate",JSON_OBJ_COPY(json_object_object_get(atmLinkQosObj, "SustainableCellRate")));
		json_object_object_add(pramJobj, "atmPeakCellRate", JSON_OBJ_COPY(json_object_object_get(atmLinkQosObj, "PeakCellRate")));
	}

#ifdef ZYXEL_WAN_MAC
	if( strcmp(CurrEncapsulation, "PPPoA") && strcmp(CurrEncapsulation, "IPoA")){   //Not PPPoA and not IPoA
		if(!strcmp(CurrType,"ETH") || !strcmp(CurrType,"ATM") || !strcmp(CurrType,"PTM") ){
			json_object_object_add(pramJobj, "WanMacType", JSON_OBJ_COPY(json_object_object_get(ethLinkObj, "X_ZYXEL_WanMacType")));
			json_object_object_add(pramJobj, "WanMacAddr", JSON_OBJ_COPY(json_object_object_get(ethLinkObj, "X_ZYXEL_WanMacAddr")));
			json_object_object_add(pramJobj, "WanMacIP", JSON_OBJ_COPY(json_object_object_get(ethLinkObj, "X_ZYXEL_WanMacClientIP")));
		}
	}
#endif

	if(vlanTermObj != NULL)
		zcfgFeJsonObjFree(vlanTermObj);
	return ret;
}

static zcfgRet_t getWanInfo(struct json_object *pramJobj,objIndex_t ipIfaceIid, struct json_object *ipIfaceObj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool v4enable, v6enable;
	const char *Interface;
	const char *pppPassTh;
	bool enable_6RD = false;
	char v6RD_Type[16] = {0};
	const char *lanport = NULL;
	if(!strcmp(CurrEncapsulation, "IPoE") || !strcmp(CurrEncapsulation, "IPoA")){
		v4enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv4Enable"));
		v6enable = json_object_get_boolean(json_object_object_get(ipIfaceObj, "IPv6Enable"));
		json_object_object_add(pramJobj, "MaxMTUSize",json_object_object_get(ipIfaceObj, "MaxMTUSize"));
	}
	else{ //PPPoE or PPPoA
		v4enable = json_object_get_boolean(json_object_object_get(pppIfaceObj, "IPCPEnable"));
		v6enable = json_object_get_boolean(json_object_object_get(pppIfaceObj, "IPv6CPEnable"));
		pppPassTh = json_object_get_string(json_object_object_get(pppIfaceObj, "X_ZYXEL_ConnectionType"));

		if(!strcmp(pppPassTh, "PPPoE_Bridged"))	
			json_object_object_add(pramJobj, "pppoePassThrough",json_object_new_boolean(true));
		else
			json_object_object_add(pramJobj, "pppoePassThrough",json_object_new_boolean(false));
		
		json_object_object_add(pramJobj, "MaxMTUSize", JSON_OBJ_COPY(json_object_object_get(pppIfaceObj, "MaxMRUSize")));
		json_object_object_add(pramJobj, "IdleDisconnectTime",JSON_OBJ_COPY(json_object_object_get(pppIfaceObj, "IdleDisconnectTime")));
		json_object_object_add(pramJobj, "pppUsername", JSON_OBJ_COPY(json_object_object_get(pppIfaceObj, "Username")));
		json_object_object_add(pramJobj, "pppPassword", JSON_OBJ_COPY(json_object_object_get(pppIfaceObj, "Password")));
		json_object_object_add(pramJobj, "ConnectionTrigger", JSON_OBJ_COPY(json_object_object_get(pppIfaceObj, "ConnectionTrigger")));
	}

	if(v4enable && v6enable)
		strcpy(CurrIpMode, "DualStack");
	else if(v4enable)
		strcpy(CurrIpMode, "IPv4");
	else if(v6enable)
		strcpy(CurrIpMode, "IPv6");

	json_object_object_add(pramJobj, "Encapsulation", json_object_new_string(CurrEncapsulation));
	json_object_object_add(pramJobj, "ipMode", json_object_new_string(CurrIpMode));
	json_object_object_add(pramJobj, "ConcurrentWan", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConcurrentWan")));
	json_object_object_add(pramJobj, "Enable_6RD",json_object_new_boolean(false));
	
#if 0
	if(!strcmp(CurrEncapsulation, "PPPoE") || !strcmp(CurrEncapsulation, "PPPoA")){
		if(!strcmp(CurrIpMode, "IPv4")){
			enable_6RD = json_object_get_boolean(json_object_object_get(ipIfaceObj, "X_ZYXEL_Enable_6RD"));
			strcpy(v6RD_Type, json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_6RD_Type")));
		}
		if(enable_6RD && !strcmp(v6RD_Type, "static")){
			json_object_object_add(pramJobj, "Enable_6RD",json_object_new_boolean(enable_6RD));
			json_object_object_add(pramJobj, "v6RD_Type", json_object_new_string(v6RD_Type));
			json_object_object_add(pramJobj, "SPIPv6Prefix", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_SPIPv6Prefix")));
			json_object_object_add(pramJobj, "IPv4MaskLength", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_IPv4MaskLength")));
			json_object_object_add(pramJobj, "BorderRelayIPv4Addresses", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_BorderRelayIPv4Addresses")));
		}
	}
#endif

	if(v4ClientObj) zcfgFeJsonObjFree(v4ClientObj);
	getSpecificObj(RDM_OID_DHCPV4_CLIENT, "Interface", json_type_string, ipIfacePath, &v4ClientIid, &v4ClientObj);
	if(v4ClientObj){
		getDhcpv4ClientOpt(&v4ClientIid);
	}
	
	if(!strcmp(CurrIpMode, "IPv4")){
		enable_6RD = json_object_get_boolean(json_object_object_get(ipIfaceObj, "X_ZYXEL_Enable_6RD"));
		json_object_object_add(pramJobj, "Enable_6RD",json_object_new_boolean(enable_6RD));
	}
	if(enable_6RD){
		strcpy(v6RD_Type, json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_6RD_Type")));
		json_object_object_add(pramJobj, "v6RD_Type", json_object_new_string(v6RD_Type));
		if(!strcmp(v6RD_Type, "static")){
			json_object_object_add(pramJobj, "SPIPv6Prefix", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_SPIPv6Prefix")));
			json_object_object_add(pramJobj, "IPv4MaskLength", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_IPv4MaskLength")));
			json_object_object_add(pramJobj, "BorderRelayIPv4Addresses", JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_BorderRelayIPv4Addresses")));
		}
	}
	
	
	if(!strcmp(CurrIpMode, "IPv6")){
		json_object_object_add(pramJobj, "DSLiteEnable",JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_Enable_DSLite")));
		json_object_object_add(pramJobj, "DSLiteType",JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_DSLite_Type")));
		json_object_object_add(pramJobj, "DSLiteRelayIPv6Addresses",JSON_OBJ_COPY(json_object_object_get(ipIfaceObj, "X_ZYXEL_DSLiteRelayIPv6Addresses")));
	}

	//NAT
	if(natIntfObj) zcfgFeJsonObjFree(natIntfObj);
	if (getSpecificObj(RDM_OID_NAT_INTF_SETTING, "Interface", json_type_string, ipIfacePath, &natIntfIid, &natIntfObj)) {
		json_object_object_add(pramJobj, "NatEnable",JSON_OBJ_COPY(json_object_object_get(natIntfObj, "Enable")));
		json_object_object_add(pramJobj, "FullConeEnabled",JSON_OBJ_COPY(json_object_object_get(natIntfObj, "X_ZYXEL_FullConeEnabled")));
	}
	else{
		Jaddb(pramJobj, "NatEnable", false);
		Jaddb(pramJobj, "FullConeEnabled", false);
	}

	//Default Gateway
	IID_INIT(routerIid);
	routerIid.level = 1;
	routerIid.idx[0] = 1;
	if(routerObj) zcfgFeJsonObjFree(routerObj);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ROUTING_ROUTER, &routerIid, &routerObj);
	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveDefaultGateway"));
	if(strstr(Interface, ipIfacePath))
		json_object_object_add(pramJobj, "sysGwEnable", json_object_new_boolean(true));
	else	
		json_object_object_add(pramJobj, "sysGwEnable", json_object_new_boolean(false));

	
	Interface = json_object_get_string(json_object_object_get(routerObj, "X_ZYXEL_ActiveV6DefaultGateway"));
	if(strstr(Interface, ipIfacePath))
		json_object_object_add(pramJobj, "sysGw6Enable", json_object_new_boolean(true));
	else
		json_object_object_add(pramJobj, "sysGw6Enable", json_object_new_boolean(false));

	//IGMP
	IID_INIT(igmpIid);
	if(igmpObj) zcfgFeJsonObjFree(igmpObj);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_IGMP, &igmpIid, &igmpObj);
	Interface = json_object_get_string(json_object_object_get(igmpObj, "Interface"));
	if(strstr(Interface, ipIfacePath))
		json_object_object_add(pramJobj, "IGMPEnable", json_object_new_boolean(true));
	else
		json_object_object_add(pramJobj, "IGMPEnable", json_object_new_boolean(false));

	//MLD
	IID_INIT(mldIid);
	if(mldObj) zcfgFeJsonObjFree(mldObj);
	zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_MLD, &mldIid, &mldObj);
	Interface = json_object_get_string(json_object_object_get(mldObj, "Interface"));
	if(strstr(Interface, ipIfacePath))
		json_object_object_add(pramJobj, "MLDEnable", json_object_new_boolean(true));
	else
		json_object_object_add(pramJobj, "MLDEnable", json_object_new_boolean(false));

	//Wan Port Check	
	IID_INIT(ethIfaceIid);
	ethIfaceIid.level = 1;
	ethIfaceIid.idx[0] = 4;
	if(ethIfaceObj) zcfgFeJsonObjFree(ethIfaceObj);
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_IFACE, &ethIfaceIid, &ethIfaceObj)== ZCFG_SUCCESS){
		lanport = json_object_get_string(json_object_object_get(ethIfaceObj, "X_ZYXEL_LanPort"));
		if(!strcmp(lanport,"ETHWAN")){
			json_object_object_add(pramJobj, "LantoWan", json_object_new_boolean(true));
		}
		else{
			json_object_object_add(pramJobj, "LantoWan", json_object_new_boolean(false));
		}
	}
	
	dbg_printf("====testH %s: exit\n",__FUNCTION__);
	return ret;
}

zcfgRet_t getStaticInfo(struct json_object *pramJobj,objIndex_t ipIfaceIid, struct json_object *ipIfaceObj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *obj = NULL;
	objIndex_t iid;
	bool Enable;
	const char *Interface;
	const char *AddressingType;
	const char *Origin;
	bool staticRoute = false;
	const char *IP6Address;
	const char *DNSServer;
	const char *X_ZYXEL_Type;
	const char *X_ZYXEL_LocalIPAddress;
	const char *GatewayIPAddress;
	const char *IPAddress;
	const char *SubnetMask;
	const char *WanMacType;
	const char *WanMacAddr;
	const char *WanMacIP;
	const char *value = NULL;
	const char *DevOUI;
	const char *DevSN;
	const char *DevPClass;
	bool ipStatic = false;
	bool ip6Static = false;
	bool ipDnsStatic = false;
	bool ip6DnsStatic = false;
	int tag = 0;
	bool opt = false;
	
	//Default
	json_object_object_add(pramJobj, "ipStatic", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "ip6Static", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option42Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option43Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option120Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option121Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option60Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option61Enable", json_object_new_boolean(false));
	json_object_object_add(pramJobj, "option125Enable", json_object_new_boolean(false));

	//static ipv4 Addr
	IID_INIT(v4AddrIid);
	v4AddrIid.level = 2;
	v4AddrIid.idx[0] = ipIfaceIid.idx[0];
	v4AddrIid.idx[1] = 1;
	if(v4AddrObj) zcfgFeJsonObjFree(v4AddrObj);
	ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V4_ADDR, &v4AddrIid, &v4AddrObj);
	if(ret == ZCFG_SUCCESS && v4AddrObj){
		AddressingType = json_object_get_string(json_object_object_get(v4AddrObj, "AddressingType"));
		Enable = json_object_get_boolean(json_object_object_get(v4AddrObj, "Enable"));
		if(Enable == true && !strcmp(AddressingType, "Static") && strstr(CurrEncapsulation, "IP")){
			ipStatic = true;
			json_object_object_add(pramJobj, "ipStatic", json_object_new_boolean(true));
			json_object_object_add(pramJobj, "IPAddress", JSON_OBJ_COPY(json_object_object_get(v4AddrObj, "IPAddress")));
			json_object_object_add(pramJobj, "SubnetMask", JSON_OBJ_COPY(json_object_object_get(v4AddrObj, "SubnetMask")));
		}
	}

	if(ipStatic){
		//GatewayIPAddress
		IID_INIT(v4FwdIid);
		if(v4FwdObj) zcfgFeJsonObjFree(v4FwdObj);
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_ROUTING_ROUTER_V4_FWD, &routerIid, &v4FwdIid, &v4FwdObj) == ZCFG_SUCCESS){
			Interface = json_object_get_string(json_object_object_get(v4FwdObj, "Interface"));
			staticRoute =  json_object_get_boolean(json_object_object_get(v4FwdObj, "StaticRoute"));
			if(!strcmp(Interface, ipIfacePath) && staticRoute == true){
				GatewayIPAddress= json_object_get_string(json_object_object_get(v4FwdObj, "GatewayIPAddress"));
				json_object_object_add(pramJobj, "GatewayIPAddress", json_object_new_string(GatewayIPAddress));
			}
			zcfgFeJsonObjFree(v4FwdObj);
		}
	}else{
		//DHCP REQ OPT
		IID_INIT(iid);
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_DHCPV4_REQ_OPT, &v4ClientIid, &iid, &obj) == ZCFG_SUCCESS){
			tag = json_object_get_int(json_object_object_get(obj, "Tag"));
			opt = json_object_get_boolean(json_object_object_get(obj, "Enable"));
			if(tag == 42 )
				json_object_object_add(pramJobj, "option42Enable", json_object_new_boolean(opt));
			else if(tag == 43)
				json_object_object_add(pramJobj, "option43Enable", json_object_new_boolean(opt));
			else if(tag == 120 )
				json_object_object_add(pramJobj, "option120Enable", json_object_new_boolean(opt));
			else if(tag == 121 )
				json_object_object_add(pramJobj, "option121Enable", json_object_new_boolean(opt));
				zcfgFeJsonObjFree(obj);                                                                                           
   		}
		//DHCP SENT OPT
		IID_INIT(iid);
		while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_DHCPV4_SENT_OPT, &v4ClientIid, &iid, &obj) == ZCFG_SUCCESS){
			tag = json_object_get_int(json_object_object_get(obj, "Tag"));
			opt = json_object_get_boolean(json_object_object_get(obj, "Enable"));
			value = json_object_get_string(json_object_object_get(obj, "Value"));
			if(tag == 60 ){
				json_object_object_add(pramJobj, "option60Enable", json_object_new_boolean(opt));
				if(strlen(value)>=4){
					char decVal[256]={0};
					opt60ValDecode(value,decVal);
					json_object_object_add(pramJobj, "option60Value", json_object_new_string(decVal));
				}
				else{
					json_object_object_add(pramJobj, "option60Value", json_object_new_string(""));
				}
			}else if(tag == 61 ){
				json_object_object_add(pramJobj, "option61Enable", json_object_new_boolean(opt));
				char iaidVal[256]={0};
				char duidVal[256]={0};
				if(opt){
					opt61ValDecode(value,iaidVal,duidVal);
				}
				json_object_object_add(pramJobj, "option61Iaid", json_object_new_string(iaidVal));
				json_object_object_add(pramJobj, "option61Duid", json_object_new_string(duidVal));
			}else if(tag == 125 ){
				json_object_object_add(pramJobj, "option125Enable", json_object_new_boolean(opt));
				json_object_object_add(pramJobj, "option125Value", json_object_new_string(value));
			}
			zcfgFeJsonObjFree(obj);
		}                                                                                                
	}

	//static ppp
	if(strstr(CurrEncapsulation, "PPP") && pppIfaceObj != NULL){
		X_ZYXEL_LocalIPAddress = json_object_get_string(json_object_object_get(pppIfaceObj, "X_ZYXEL_LocalIPAddress"));
		if(strcmp(X_ZYXEL_LocalIPAddress, "0.0.0.0") && strcmp(X_ZYXEL_LocalIPAddress, "")){
			CurrPppStatic = true;
			json_object_object_add(pramJobj, "IPAddress", json_object_new_string(X_ZYXEL_LocalIPAddress));
			json_object_object_add(pramJobj, "ipStatic", json_object_new_boolean(true));
		}
	}
	//static ipv6 Addr
	IID_INIT(v6AddrIid);
	v6AddrIid.level = 2;
	v6AddrIid.idx[0] = ipIfaceIid.idx[0];
	v6AddrIid.idx[1] = 1;
	if(v6AddrObj) zcfgFeJsonObjFree(v6AddrObj);
	ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE_V6_ADDR, &v6AddrIid, &v6AddrObj);
	if(ret == ZCFG_SUCCESS && v6AddrObj){
		Origin = json_object_get_string(json_object_object_get(v6AddrObj, "Origin"));
		Enable = json_object_get_boolean(json_object_object_get(v6AddrObj, "Enable"));
		if(Enable == true && !strcmp(Origin, "Static")){
			ip6Static = true;
			json_object_object_add(pramJobj, "ip6Static", json_object_new_boolean(true));
			json_object_object_add(pramJobj, "IP6Address", JSON_OBJ_COPY(json_object_object_get(v6AddrObj, "IPAddress")));
		}
	}

	IID_INIT(v6FwdIid);
	if(v6FwdObj) zcfgFeJsonObjFree(v6FwdObj);
	while(zcfgFeSubInObjJsonGetNextWithoutUpdate(RDM_OID_ROUTING_ROUTER_V6_FWD, &routerIid, &v6FwdIid, &v6FwdObj) == ZCFG_SUCCESS){
		Interface = json_object_get_string(json_object_object_get(v6FwdObj, "Interface"));
		Origin = json_object_get_string(json_object_object_get(v6FwdObj, "Origin"));
		if(strstr(CurrEncapsulation, "IP") && ip6Static){
			if(!strcmp(Interface, ipIfacePath)){
				json_object_object_add(pramJobj, "NextHop", JSON_OBJ_COPY(json_object_object_get(v6FwdObj, "NextHop")));
				break;
			}
		}
		zcfgFeJsonObjFree(v6FwdObj);
	}


	//static DNS
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DNS_CLIENT_SRV, &iid, &obj) == ZCFG_SUCCESS){
		Interface = json_object_get_string(json_object_object_get(obj, "Interface"));
		X_ZYXEL_Type = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_Type"));
		if(!strcmp(Interface, ipIfacePath) && !strcmp(X_ZYXEL_Type, "Static")){
			DNSServer = json_object_get_string(json_object_object_get(obj, "DNSServer"));
			if((strchr(DNSServer, ':') == NULL)) { //&& (dnsClientSrv4Obj == NULL)){
				json_object_object_add(pramJobj, "DNSServer", json_object_new_string(DNSServer));
				json_object_object_add(pramJobj, "ipDnsStatic", json_object_new_boolean(true));
				ipDnsStatic = true;
			}
			else {//if(dnsClientSrv6Obj == NULL){
				json_object_object_add(pramJobj, "DNS6Server", json_object_new_string(DNSServer));
				json_object_object_add(pramJobj, "ip6DnsStatic", json_object_new_boolean(true));
				ip6DnsStatic = true;
			}
		}
		/*else{
			json_object_object_add(pramJobj, "ipDnsStatic", json_object_new_boolean(false));
			json_object_object_add(pramJobj, "ip6DnsStatic", json_object_new_boolean(false));
			zcfgFeJsonObjFree(obj);
		}*/
		zcfgFeJsonObjFree(obj);
	}

	if(ipDnsStatic == false)
	{	json_object_object_add(pramJobj, "ipDnsStatic", json_object_new_boolean(false));   }

	if(ip6DnsStatic == false)
	{	json_object_object_add(pramJobj, "ip6DnsStatic", json_object_new_boolean(false));   }

    //Dev info
	IID_INIT(iid);
	iid.level = 0;
	iid.idx[0] = 0;
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DEV_INFO, &iid, &obj) == ZCFG_SUCCESS){

		DevOUI = json_object_get_string(json_object_object_get(obj, "ManufacturerOUI"));
		DevSN = json_object_get_string(json_object_object_get(obj, "SerialNumber"));
		DevPClass = json_object_get_string(json_object_object_get(obj, "ProductClass"));

		json_object_object_add(pramJobj, "ManufacturerOUI", json_object_new_string(DevOUI));
		json_object_object_add(pramJobj, "SerialNumber", json_object_new_string(DevSN));
		json_object_object_add(pramJobj, "ProductClass", json_object_new_string(DevPClass));

		zcfgFeJsonObjFree(obj);
	}

	return ret;
}

#ifdef ZYXEL_SYNC_CCWAN_TO_ALL_DEFGW
zcfgRet_t restartRunningWan(objIndex_t ccWanIid, struct json_object *ccWanObj){
	zcfgRet_t ret = ZCFG_SUCCESS;

	printf("%s: Restart running wan!\n",__FUNCTION__);
	json_object_object_add(ccWanObj, "Enable", json_object_new_boolean(false));
	zcfgFeObjJsonBlockedSet(RDM_OID_IP_IFACE, &ccWanIid, ccWanObj, NULL);
	json_object_object_add(ccWanObj, "Enable", json_object_new_boolean(true));
	zcfgFeObjJsonSet(RDM_OID_IP_IFACE, &ccWanIid, ccWanObj, NULL);

	return ret;
}
zcfgRet_t syncCCWanSetting(struct json_object *Jobj){
	zcfgRet_t ret = ZCFG_SUCCESS;
	bool defaultGatewayIface = false;
	int newCCWan = 0, CurCCWan = 0;
	char *status = NULL;
	struct json_object *subIpIfaceObj =   NULL;
	struct json_object *ccWanObj = NULL;
	objIndex_t ccWanIid = {0};
	bool wanEnable = false, reRunningWan = false;
	bool sysGwEnable = false;

	newCCWan = json_object_get_int(json_object_object_get(Jobj, "ConcurrentWan"));
	sysGwEnable = json_object_get_int(json_object_object_get(Jobj, "sysGwEnable"));

	if(!sysGwEnable)
		return ret;

	/*Sync other default WAN*/
	IID_INIT(ccWanIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &ccWanIid, &ccWanObj) == ZCFG_SUCCESS){
		status = json_object_get_string(json_object_object_get(ccWanObj, "Status"));
		defaultGatewayIface = json_object_get_boolean(json_object_object_get(ccWanObj, "X_ZYXEL_DefaultGatewayIface"));
		wanEnable = json_object_get_boolean(json_object_object_get(ccWanObj, "Enable"));
		CurCCWan = json_object_get_int(json_object_object_get(ccWanObj, "X_ZYXEL_ConcurrentWan"));

		if(ccWanIid.idx[0] == ipIfaceIid.idx[0] || ccWanIid.idx[0] <=2){
			zcfgFeJsonObjFree(ccWanObj);
			continue;
		}

		replaceParam(ccWanObj, "X_ZYXEL_ConcurrentWan", Jobj, "ConcurrentWan");
		if(defaultGatewayIface && !strcmp("Up", status)){
			if(wanEnable) {//restart running wan!
				restartRunningWan(ccWanIid, ccWanObj);
			}else{
				zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE, &ccWanIid, ccWanObj, NULL);
			}
		}else if (defaultGatewayIface && strcmp("Up", status)){
			zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE, &ccWanIid, ccWanObj, NULL);
		}else{
			if(!defaultGatewayIface && CurCCWan !=0){
				json_object_object_add(ccWanObj, "X_ZYXEL_ConcurrentWan", json_object_new_int(0));
				zcfgFeObjJsonSetWithoutApply(RDM_OID_IP_IFACE, &ccWanIid, ccWanObj, NULL);
			}
		}
		zcfgFeJsonObjFree(ccWanObj);
	}

	return ret;
}
#endif
zcfgRet_t zcfgFeDalWanGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int count = 0, index = 0;
	
	struct json_object *pramJobj = NULL;
	struct json_object *ipIfObj = NULL;
	initGlobalObjects();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE, &ipIfaceIid, &ipIfObj) == ZCFG_SUCCESS) {
		
		pramJobj = json_object_new_object();
		ret = parseIpIfaceObj(pramJobj, ipIfaceIid, ipIfObj);
		
		if( ret == ZCFG_SUCCESS )
		{
			count++;
			json_object_object_add(pramJobj, "index", json_object_new_int(count));
			Jadds(pramJobj, "ipIfacePath", ipIfacePath);

#if defined(SAME_FW_FOR_TWO_BOARD)
            if (strcmp(CurrType, "ETH") == 0)
            {
                /* Not support ETHWAN */
                if (zyutil_cofw_modelNameGet() == MODEL_VMG1312_T20A)
                {
                    zcfgFeJsonObjFree(pramJobj);
                    zcfgFeJsonObjFree(ipIfObj);
					freeAllObjects();
                    continue;
                }
            }
#endif
			if(strcmp(CurrMode, "IP_Bridged")){
				getWanInfo(pramJobj, ipIfaceIid, ipIfObj);
				getStaticInfo(pramJobj, ipIfaceIid, ipIfObj);
			}
			if(json_object_object_get(Jobj, "Index")){
				index = json_object_get_int(json_object_object_get(Jobj, "Index"));
				if(index == count){
					json_object_object_add(pramJobj, "ShowDetail", json_object_new_boolean(true));
					json_object_array_add(Jarray, JSON_OBJ_COPY(pramJobj));

					zcfgFeJsonObjFree(pramJobj);
					zcfgFeJsonObjFree(ipIfObj);
					freeAllObjects();
					break;
				}
			}
            else
            {
			    json_object_array_add(Jarray, JSON_OBJ_COPY(pramJobj));
            }
		}
		zcfgFeJsonObjFree(pramJobj);
		zcfgFeJsonObjFree(ipIfObj);
		freeAllObjects();
	}

	return ZCFG_SUCCESS;
}

zcfgRet_t zcfgFeCheckIPDNS(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *Jarray = NULL, *obj = NULL;
	const char *curname = NULL;
	int len = 0, i;
	bool editmode = false, ipv4static = false, dnsv4static = false, ipv6static = false, dnsv6static = false;
	bool oriipv4static = false, oridnsv4static = false, oriipv6static = false, oridnsv6static = false;

	Jarray = json_object_new_array();
	curname = json_object_get_string(json_object_object_get(Jobj, "Name"));
	ipv4static = json_object_get_boolean(json_object_object_get(Jobj, "ipStatic"));
	dnsv4static = json_object_get_boolean(json_object_object_get(Jobj, "ipDnsStatic"));
	ipv6static = json_object_get_boolean(json_object_object_get(Jobj, "ip6Static"));
	dnsv6static = json_object_get_boolean(json_object_object_get(Jobj, "ip6DnsStatic"));
	zcfgFeDalWanGet(Jobj, Jarray, NULL);
	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		if(!strcmp(curname, json_object_get_string(json_object_object_get(obj, "Name")))){
			oriipv4static = json_object_get_boolean(json_object_object_get(obj, "ipStatic"));
			oridnsv4static = json_object_get_boolean(json_object_object_get(obj, "ipDnsStatic"));
			oriipv6static = json_object_get_boolean(json_object_object_get(obj, "ip6Static"));
			oridnsv6static = json_object_get_boolean(json_object_object_get(obj, "ip6DnsStatic"));
			editmode = true;
			break;
		}
	}
	if(editmode){	//edit case
		if(json_object_object_get(Jobj, "ipStatic") && json_object_object_get(Jobj, "ipDnsStatic")){
			if(ipv4static && !dnsv4static){
				if(replyMsg)
					strcpy(replyMsg, "--ipDnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!json_object_object_get(Jobj, "ipStatic") && json_object_object_get(Jobj, "ipDnsStatic")){
			if(oriipv4static && !dnsv4static){
				if(replyMsg)
					strcpy(replyMsg, "--ipDnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}

		if(json_object_object_get(Jobj, "ip6Static") && json_object_object_get(Jobj, "ip6DnsStatic")){
			if(ipv6static && !dnsv6static){
				if(replyMsg)
					strcpy(replyMsg, "--ip6DnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
		else if(!json_object_object_get(Jobj, "ip6Static") && json_object_object_get(Jobj, "ip6DnsStatic")){
			if(oriipv6static && !dnsv6static){
				if(replyMsg)
					strcpy(replyMsg, "--ip6DnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
	}
	else{	//add case
		if(ipv4static){	//for ipv4 case
			if(!json_object_object_get(Jobj, "ipDnsStatic")){
				if(replyMsg)
					strcpy(replyMsg, "Please set --ipDnsStatic parameter.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!dnsv4static){
				if(replyMsg)
					strcpy(replyMsg, "--ipDnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
		if(ipv6static){	//for ipv6 case
			if(!json_object_object_get(Jobj, "ip6DnsStatic")){
				if(replyMsg)
					strcpy(replyMsg, "Please set --i6pDnsStatic parameter.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
			else if(!dnsv6static){
				if(replyMsg)
					strcpy(replyMsg, "--ip6DnsStatic should be set as 1.");
				ret = ZCFG_INVALID_PARAM_VALUE;
			}
		}
	}

	zcfgFeJsonObjFree(Jarray);
	return ret;
}

zcfgRet_t zcfgFeDalWan(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "POST"))
		ret = zcfgFeDalWanAdd(Jobj, replyMsg);
	else if(!strcmp(method, "PUT"))
		ret = zcfgFeDalWanEdit(Jobj, replyMsg);
	else if(!strcmp(method, "DELETE"))
		ret = zcfgFeDalWanDelete(Jobj, NULL);
	else if(!strcmp(method, "GET")){
		ret = zcfgFeDalWanGet(Jobj, Jarray, NULL);
	}
	else
		printf("Unknown method:%s\n", method);

	return ret;
}



