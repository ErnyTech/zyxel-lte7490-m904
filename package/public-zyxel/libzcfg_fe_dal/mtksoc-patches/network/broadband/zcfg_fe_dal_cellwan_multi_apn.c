#include <ctype.h>
#include <json/json.h>
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

#define CWMP_SOAP_MSG_DEBUG (1 << 1)
#define hiddenPassword "********"

dal_param_t CELLWAN_MAPN_param[]={
{"Index", dalType_int, 0, 0, NULL},	
{"AP_Enable", dalType_boolean, 0, 0, NULL},
{"AP_ManualAPN", dalType_boolean, 0, 0, NULL},
{"AP_APN", dalType_string, 0, 0, NULL},
{"AP_Username", dalType_string, 0, 0, NULL},
{"AP_Password", dalType_string, 0, 0, NULL},
{"AP_Auth_Type", dalType_string, 0, "None/PAP/CHAP/Auto", NULL},
{"AP_PDP_Type", dalType_string, 0, "IPv4/IPv6/IPv4v6", NULL},
{"Passthru_Enable", dalType_boolean, 0, 0, NULL},
{"Passthru_Mode", dalType_string, 0, "Dynamic/Fixed", NULL},
{"Passthru_MacAddr", dalType_string, 0, 0, NULL},
{"Passthru_SubnetPrefix", dalType_int, 0, 0, NULL},
{"DHCP_LeaseTime", dalType_int,0,0,NULL},
#ifdef RILCMD_SUPPORT_DUAL_SIM_WITH_DIFFERENT_SETTINGS
{"Apply", dalType_boolean, 0, 0, NULL},
#endif
{ NULL,	0,	0,	0,	NULL}

};


int getCellWanIidByIpIfacePath(char *ipiface, objIndex_t *objIid)
{
	int ret = 0;
	objIndex_t ethLinkIid, ipIfIid, cellIid;
	struct json_object * ethLinkObj = NULL;
	struct json_object * ipIfaceObj = NULL;


	if (!ipiface) return -1;
	if (!objIid) return -2;
	//printf("[%s][%d] ipiface(%s)\n" , __FUNCTION__, __LINE__, ipiface);

	IID_INIT(ipIfIid);
	ipIfIid.level = 1;
	sscanf(ipiface, "IP.Interface.%hhu", &ipIfIid.idx[0]);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_IP_IFACE, &ipIfIid, &ipIfaceObj)) == ZCFG_SUCCESS){
		IID_INIT(ethLinkIid);
		ethLinkIid.level = 1;
		if(strstr(json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers")), "Ethernet.Link.")){
			sscanf(json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers")), "Ethernet.Link.%hhu", &ethLinkIid.idx[0]);
			//printf("[%s][%d] ethLinkIid.idx[0](%d)\n" , __FUNCTION__, __LINE__, ethLinkIid.idx[0]);
		}
		if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ETH_LINK, &ethLinkIid, &ethLinkObj)) == ZCFG_SUCCESS){
			IID_INIT(*objIid);
			IID_INIT(cellIid);
			cellIid.level = 1;
			if(strstr(json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers")), "Cellular.Interface.")){
				sscanf(json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers")), "Cellular.Interface.%hhu", &cellIid.idx[0]);
			}
			//printf("[%s][%d] cellIid.idx[0](%d)\n" , __FUNCTION__, __LINE__, cellIid.idx[0]);

			memcpy(objIid, &cellIid, sizeof(objIndex_t));

			zcfgFeJsonObjFree(ethLinkObj);
		} else {
			ret = -4;
		}
		
		zcfgFeJsonObjFree(ipIfaceObj);
	} else {
		ret = -3;
	}

	//printf("[%s][%d] objIid.idx[0](%d)\n" , __FUNCTION__, __LINE__, objIid->idx[0]);
	return ret;
}


zcfgRet_t zcfgFeDalCellWanMApnEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *accessPointObj = NULL, *intfZyIpPassthruObj = NULL;
	objIndex_t accessPointIid = {0};
	int index = -1;
	bool enableAPN = false;
	bool manualAPN = false;
	const char *apn = NULL;
	const char *username = NULL;
	const char *password = NULL;
	const char *authType = NULL;
	const char *pdpType = NULL;
	zcfg_offset_t accessPointRdmOid = RDM_OID_CELL_ACCESS_POINT;

#ifdef ZYXEL_L2TPV3
	struct json_object * l2tpv3Obj = NULL;
	objIndex_t l2tpv3Iid = {0};
	bool l2tpv3Enable = false;
	const char *l2tpv3LocalIPAddress = NULL;
	const char *l2tpv3RemoteIPAddress = NULL;
	const char *l2tpv3LocalTunnelAddress = NULL;
	const char *l2tpv3RemoteTunnelAddress = NULL;
	int l2tpv3LocalTunnelID = 0;
	int l2tpv3RemoteTunnelID = 0;
	int l2tpv3LocalSessionID = 0;
	int l2tpv3RemoteSessionID = 0;
	int l2tpv3SourcePort = 0;
	int l2tpv3DestinationPort = 0;
#endif
	objIndex_t intfZyIpPassthruIid = {0};
	bool passthruEnable = false;
	const char *passthruMode = NULL;
	const char *passthruMacAddr = NULL;
	zcfg_offset_t cellIntfZyIppassRdmOid = RDM_OID_CELL_INTF_ZY_IP_PASS_THRU;
	int modifiedAP = 0;

    int passthruSubnetPrefix = 0;

	bool passthruStaticGwEn = false;
	const char *StaticGwIPAddress = NULL;
	int DHCP_LeaseTime = 0;
	bool apply = 1;

	IID_INIT(accessPointIid);
	index = json_object_get_int(json_object_object_get(Jobj, "Index"));
	ret = convertIndextoIid(index, &accessPointIid, accessPointRdmOid, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS)
		return ret;

#ifdef RILCMD_SUPPORT_DUAL_SIM_WITH_DIFFERENT_SETTINGS
	apply = json_object_get_boolean(json_object_object_get(Jobj, "Apply"));
#endif

#ifdef ZYXEL_L2TPV3
	ret = convertIndextoIid(index, &l2tpv3Iid, RDM_OID_L2_T_PV3, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS){
		return ret;
	}
#endif
	IID_INIT(intfZyIpPassthruIid);
	ret = convertIndextoIid(index, &intfZyIpPassthruIid, cellIntfZyIppassRdmOid, NULL, NULL, replyMsg);

	enableAPN = json_object_get_boolean(json_object_object_get(Jobj, "AP_Enable"));
	manualAPN = json_object_get_boolean(json_object_object_get(Jobj, "AP_ManualAPN"));
	apn = json_object_get_string(json_object_object_get(Jobj, "AP_APN"));
	username = json_object_get_string(json_object_object_get(Jobj, "AP_Username"));
	password = json_object_get_string(json_object_object_get(Jobj, "AP_Password"));
	authType = json_object_get_string(json_object_object_get(Jobj, "AP_Auth_Type"));
	pdpType = json_object_get_string(json_object_object_get(Jobj, "AP_PDP_Type"));
	passthruEnable = json_object_get_boolean(json_object_object_get(Jobj, "Passthru_Enable"));
	passthruMode = json_object_get_string(json_object_object_get(Jobj, "Passthru_Mode"));
	passthruMacAddr = json_object_get_string(json_object_object_get(Jobj, "Passthru_MacAddr"));
    passthruSubnetPrefix = json_object_get_int(json_object_object_get(Jobj, "Passthru_SubnetPrefix"));
	passthruStaticGwEn = json_object_get_boolean(json_object_object_get(Jobj, "Passthru_StaticGWIPEnable"));
	StaticGwIPAddress = json_object_get_string(json_object_object_get(Jobj, "Passthru_StaticGWIP"));
	DHCP_LeaseTime = json_object_get_int(json_object_object_get(Jobj, "DHCP_LeaseTime"));
#ifdef ZYXEL_L2TPV3
	l2tpv3Enable = json_object_get_boolean(json_object_object_get(Jobj, "L2TPv3_Enable"));
	l2tpv3LocalIPAddress = json_object_get_string(json_object_object_get(Jobj, "L2TPv3_LocalIPAddress"));
	l2tpv3RemoteIPAddress = json_object_get_string(json_object_object_get(Jobj, "L2TPv3_RemoteIPAddress"));
	l2tpv3LocalTunnelAddress = json_object_get_string(json_object_object_get(Jobj, "L2TPv3_LocalTunnelAddress"));
	l2tpv3RemoteTunnelAddress = json_object_get_string(json_object_object_get(Jobj, "L2TPv3_RemoteTunnelAddress"));
	l2tpv3LocalTunnelID = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_LocalTunnelID"));
	l2tpv3RemoteTunnelID = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_RemoteTunnelID"));
	l2tpv3LocalSessionID = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_LocalSessionID"));
	l2tpv3RemoteSessionID = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_RemoteSessionID"));
	l2tpv3SourcePort = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_SourcePort"));
	l2tpv3DestinationPort = json_object_get_int(json_object_object_get(Jobj, "L2TPv3_DestinationPort"));
#endif

#ifdef ZYXEL_L2TPV3
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_L2_T_PV3, &l2tpv3Iid, &l2tpv3Obj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "L2TPv3_Enable"))
			json_object_object_add(l2tpv3Obj, "Enable", json_object_new_boolean(l2tpv3Enable));
		if(json_object_object_get(Jobj, "L2TPv3_LocalIPAddress"))
			json_object_object_add(l2tpv3Obj, "LocalIPAddress", json_object_new_string(l2tpv3LocalIPAddress));
		if(json_object_object_get(Jobj, "L2TPv3_RemoteIPAddress"))
			json_object_object_add(l2tpv3Obj, "RemoteIPAddress", json_object_new_string(l2tpv3RemoteIPAddress));
		if(json_object_object_get(Jobj, "L2TPv3_LocalTunnelAddress"))
			json_object_object_add(l2tpv3Obj, "LocalTunnelAddress", json_object_new_string(l2tpv3LocalTunnelAddress));
		if(json_object_object_get(Jobj, "L2TPv3_RemoteTunnelAddress"))
			json_object_object_add(l2tpv3Obj, "RemoteTunnelAddress", json_object_new_string(l2tpv3RemoteTunnelAddress));
		if(json_object_object_get(Jobj, "L2TPv3_LocalTunnelID"))
			json_object_object_add(l2tpv3Obj, "LocalTunnelID", json_object_new_int(l2tpv3LocalTunnelID));
		if(json_object_object_get(Jobj, "L2TPv3_RemoteTunnelID"))
			json_object_object_add(l2tpv3Obj, "RemoteTunnelID", json_object_new_int(l2tpv3RemoteTunnelID));
		if(json_object_object_get(Jobj, "L2TPv3_LocalSessionID"))
			json_object_object_add(l2tpv3Obj, "LocalSessionID", json_object_new_int(l2tpv3LocalSessionID));
		if(json_object_object_get(Jobj, "L2TPv3_RemoteSessionID"))
			json_object_object_add(l2tpv3Obj, "RemoteSessionID", json_object_new_int(l2tpv3RemoteSessionID));
		if(json_object_object_get(Jobj, "L2TPv3_SourcePort"))
			json_object_object_add(l2tpv3Obj, "SourcePort", json_object_new_int(l2tpv3SourcePort));
		if(json_object_object_get(Jobj, "L2TPv3_DestinationPort"))
			json_object_object_add(l2tpv3Obj, "DestinationPort", json_object_new_int(l2tpv3DestinationPort));
		zcfgFeObjJsonSet(RDM_OID_L2_T_PV3, &l2tpv3Iid, l2tpv3Obj, NULL);
		zcfgFeJsonObjFree(l2tpv3Obj);
	}
#endif

	if((ret = zcfgFeObjJsonGetWithoutUpdate(accessPointRdmOid, &accessPointIid, &accessPointObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "AP_Enable") && (enableAPN != json_object_get_boolean(json_object_object_get(accessPointObj, "Enable")))){
			json_object_object_add(accessPointObj, "Enable", json_object_new_boolean(enableAPN));
			modifiedAP = 1;
		}
		if(json_object_object_get(Jobj, "AP_ManualAPN")&& (manualAPN != json_object_get_boolean(json_object_object_get(accessPointObj, "X_ZYXEL_ManualAPN")))){
			json_object_object_add(accessPointObj, "X_ZYXEL_ManualAPN", json_object_new_boolean(manualAPN));
			modifiedAP = 1;
		}
		if(json_object_object_get(Jobj, "AP_APN")&&(strcmp(apn, json_object_get_string(json_object_object_get(accessPointObj, "APN"))))){
			json_object_object_add(accessPointObj, "APN", json_object_new_string(apn));
			modifiedAP = 1;
		}
		if(json_object_object_get(Jobj, "AP_Username")&&(strcmp(username, json_object_get_string(json_object_object_get(accessPointObj, "Username"))))){
			json_object_object_add(accessPointObj, "Username", json_object_new_string(username));
			modifiedAP = 1;
		}
#if 1
		if(json_object_object_get(Jobj, "AP_Password")&&(strcmp(password, json_object_get_string(json_object_object_get(accessPointObj, "Password"))))){
			json_object_object_add(accessPointObj, "Password", json_object_new_string(password));
			modifiedAP = 1;
		}
#else
		if ( json_object_object_get(Jobj, "AP_Password") &&
			( strcmp(password, hiddenPassword) ) &&
			( strcmp(password, json_object_get_string(json_object_object_get(accessPointObj, "Password"))) ) ) {
			json_object_object_add(accessPointObj, "Password", json_object_new_string(password));
			modifiedAP = 1;
		}
#endif
		if(json_object_object_get(Jobj, "AP_Auth_Type")&&(strcmp(authType, json_object_get_string(json_object_object_get(accessPointObj, "X_ZYXEL_AuthenticationType"))))){
			json_object_object_add(accessPointObj, "X_ZYXEL_AuthenticationType", json_object_new_string(authType));
			modifiedAP = 1;
		}
		if(json_object_object_get(Jobj, "AP_PDP_Type")&& (strcmp(pdpType, json_object_get_string(json_object_object_get(accessPointObj, "X_ZYXEL_PDP_Type"))))){
			json_object_object_add(accessPointObj, "X_ZYXEL_PDP_Type", json_object_new_string(pdpType));
			modifiedAP = 1;
		}
		if (modifiedAP == 1){
			if (apply) {
				if((ret = zcfgFeObjJsonSet(accessPointRdmOid, &accessPointIid, accessPointObj, NULL)) != ZCFG_SUCCESS){
					zcfgFeJsonObjFree(accessPointObj);
					printf("zcfgFeObjJsonSet RDM_OID_CELL_ACCESS_POINT fail.\n");
					return ret;
				}
			} else {
				if((ret = zcfgFeObjJsonSetWithoutApply(accessPointRdmOid, &accessPointIid, accessPointObj, NULL)) != ZCFG_SUCCESS){
					zcfgFeJsonObjFree(accessPointObj);
					printf("zcfgFeObjJsonSet RDM_OID_CELL_ACCESS_POINT fail.\n");
					return ret;
				}
			}
		}
	}
	
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, &intfZyIpPassthruObj)) == ZCFG_SUCCESS){
		printf("%s() get RDM_OID_CELL_INTF_ZY_IP_PASS_THRU success\n", __FUNCTION__);//Jessie
		if(json_object_object_get(Jobj, "Passthru_Enable"))
		json_object_object_add(intfZyIpPassthruObj, "Enable", json_object_new_boolean(passthruEnable));
		if(json_object_object_get(Jobj, "Passthru_Mode"))
		json_object_object_add(intfZyIpPassthruObj, "ConnectionMode", json_object_new_string(passthruMode));
		if(json_object_object_get(Jobj, "Passthru_MacAddr"))
		json_object_object_add(intfZyIpPassthruObj, "MACAddress", json_object_new_string(passthruMacAddr));
		if(json_object_object_get(Jobj, "Passthru_SubnetPrefix"))
		    json_object_object_add(intfZyIpPassthruObj, "ProxyARP_SubnetPrefix", json_object_new_int(passthruSubnetPrefix));
		if(json_object_object_get(Jobj, "Passthru_StaticGWIPEnable"))
		    json_object_object_add(intfZyIpPassthruObj, "StaticGwEn", json_object_new_boolean(passthruStaticGwEn));
		if(json_object_object_get(Jobj, "Passthru_StaticGWIP"))
		    json_object_object_add(intfZyIpPassthruObj, "StaticGwIPAddress", json_object_new_string(StaticGwIPAddress));
		//getIppassSuccess = 1;
		if(json_object_object_get(Jobj, "DHCP_LeaseTime"))
			json_object_object_add(intfZyIpPassthruObj, "DHCP_LeaseTime", json_object_new_int(DHCP_LeaseTime));
		if (apply)
			zcfgFeObjJsonSet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, intfZyIpPassthruObj, NULL);
		else
			zcfgFeObjJsonSetWithoutApply(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &intfZyIpPassthruIid, intfZyIpPassthruObj, NULL);
		zcfgFeJsonObjFree(intfZyIpPassthruObj);
	}

	zcfgFeJsonObjFree(accessPointObj);

	return ret;	
}	

zcfgRet_t zcfgFeDalCellWanMApnGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int index = 0;
	int mapn_idx = 0;
#ifdef RILCMD_SUPPORT_DUAL_SIM_WITH_DIFFERENT_SETTINGS
	int mapn_max = 8;
#else
	int mapn_max = 4;
#endif
	objIndex_t accessPointIid = {0};
	struct json_object *paramJobj = NULL, *accessPointObj = NULL;
#ifdef ZYXEL_L2TPV3
	objIndex_t l2tpv3Iid = {0};
	struct json_object *l2tpv3Obj = NULL;
#endif
	struct json_object *intfZyIpPassthruObj = NULL;
	IID_INIT(accessPointIid);
#if 1
	struct json_object *vlanGroupObj = NULL;
	objIndex_t vlanGroupIid = {0};
#endif	
#ifdef ZYXEL_L2TPV3
	l2tpv3Iid.level = 1;
	l2tpv3Iid.idx[0] = 1;
#endif
	const char *str;
	while(zcfgFeObjJsonGetNext(RDM_OID_CELL_ACCESS_POINT, &accessPointIid, &accessPointObj) == ZCFG_SUCCESS) {
		if (mapn_idx >= mapn_max) 
			break;
		else
			mapn_idx++;

		paramJobj = json_object_new_object();

		json_object_object_add(paramJobj, "AP_Enable", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Enable")));
		json_object_object_add(paramJobj, "AP_APN", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "APN")));
		json_object_object_add(paramJobj, "AP_Username", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Username")));
#if 1
		json_object_object_add(paramJobj, "AP_Password", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Password")));
#else
		str = json_object_get_string(json_object_object_get(accessPointObj, "Password"));
		if (strlen(str) > 0) {
			json_object_object_add(paramJobj, "AP_Password", json_object_new_string(hiddenPassword));
		} else {
			json_object_object_add(paramJobj, "AP_Password", json_object_new_string(""));
		}
#endif
		json_object_object_add(paramJobj, "AP_Interface", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "Interface")));
		json_object_object_add(paramJobj, "AP_ManualAPN", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_ManualAPN")));
		json_object_object_add(paramJobj, "AP_Auth_Type", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_AuthenticationType")));
		json_object_object_add(paramJobj, "AP_PDP_Type", JSON_OBJ_COPY(json_object_object_get(accessPointObj, "X_ZYXEL_PDP_Type")));
		
		if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &accessPointIid, &intfZyIpPassthruObj) == ZCFG_SUCCESS) {
			json_object_object_add(paramJobj, "Passthru_Enable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "Enable")));
			json_object_object_add(paramJobj, "Passthru_Mode", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ConnectionMode")));
			json_object_object_add(paramJobj, "Passthru_MacAddr", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "MACAddress")));
			json_object_object_add(paramJobj, "Passthru_SubnetPrefix", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "ProxyARP_SubnetPrefix")));
			json_object_object_add(paramJobj, "Passthru_StaticGWIPEnable", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwEn")));
			json_object_object_add(paramJobj, "Passthru_StaticGWIP", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "StaticGwIPAddress")));
			json_object_object_add(paramJobj, "DHCP_LeaseTime", JSON_OBJ_COPY(json_object_object_get(intfZyIpPassthruObj, "DHCP_LeaseTime")));
			zcfgFeJsonObjFree(intfZyIpPassthruObj);
		}

		int vlanID = 0;
		char *brRefKey = NULL;
		//printf("%s() accessPointIid.idx[0]=%d\n", __FUNCTION__, accessPointIid.idx[0]);
		
		IID_INIT(vlanGroupIid);
		vlanGroupIid.level = 1;
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_VLAN_GROUP, &vlanGroupIid, &vlanGroupObj) == ZCFG_SUCCESS){
			brRefKey = json_object_get_string(json_object_object_get(vlanGroupObj, "BrRefKey"));
			if(brRefKey && strlen(brRefKey)){
				objIndex_t bridgeIid = {0};
				struct json_object *bridgeJobj = NULL;
				char *bridgeName = NULL;
				bridgeIid.level = 1;
				bridgeIid.idx[0] = atoi(brRefKey);
				//printf("%s() bridgeIid.idx[0]=%d\n", __FUNCTION__, bridgeIid.idx[0]);
				if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_BRIDGING_BR, &bridgeIid, &bridgeJobj) == ZCFG_SUCCESS){
					bridgeName = json_object_get_string(json_object_object_get(bridgeJobj, "X_ZYXEL_BridgeName"));
					if(bridgeName){
						//printf("json_object_get_string(X_ZYXEL_BridgeName):%s\n", bridgeName);
						
						objIndex_t IpIfaceIid = {0};
						struct json_object *IpIfaceObj = NULL;
						char *srvName = NULL;
						char *groupWan = NULL;
						//int groupIpIdx = 0;
						objIndex_t groupIpIid = {0};
						while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE, &IpIfaceIid, &IpIfaceObj) == ZCFG_SUCCESS)
						{
							srvName = json_object_get_string(json_object_object_get(IpIfaceObj, "X_ZYXEL_SrvName"));
							if(srvName){
								//printf("json_object_get_string(X_ZYXEL_SrvName):%s\n", srvName);
								if( 0 == strcmp(bridgeName, srvName) ){
									groupWan = json_object_get_string(json_object_object_get(IpIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
									if(groupWan){
										//printf("json_object_get_string(X_ZYXEL_Group_WAN_IpIface):%s\n", groupWan);
										if(strlen(groupWan) && strstr(groupWan, "IP.Interface.")){
											getCellWanIidByIpIfacePath(groupWan, &groupIpIid);
										}
										//printf("%s() groupIpIid.idx[0]=%d\n", __FUNCTION__, groupIpIid.idx[0]);
										if(groupIpIid.idx[0] == accessPointIid.idx[0]){
											vlanID = json_object_get_int(json_object_object_get(vlanGroupObj, "VlanId"));
											//printf("%s() vlanID=%d\n", __FUNCTION__, vlanID);
											json_object_object_add(paramJobj, "AP_VlanID_Enable", JSON_OBJ_COPY(json_object_object_get(vlanGroupObj, "Enable")));
											json_object_object_add(paramJobj, "AP_VlanID", JSON_OBJ_COPY(json_object_object_get(vlanGroupObj, "VlanId")));
											json_object_object_add(paramJobj, "AP_BrRefKey", JSON_OBJ_COPY(json_object_object_get(vlanGroupObj, "BrRefKey")));
											zcfgFeJsonObjFree(IpIfaceObj);
											break;
										}
									}
								}
							}
							
							zcfgFeJsonObjFree(IpIfaceObj);
						}
					}
					zcfgFeJsonObjFree(bridgeJobj);
				}
			}
			
			zcfgFeJsonObjFree(vlanGroupObj);
		}
		

#ifdef ZYXEL_L2TPV3
		if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_L2_T_PV3, &l2tpv3Iid, &l2tpv3Obj) == ZCFG_SUCCESS) {
			json_object_object_add(paramJobj, "L2TPv3_Enable", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "Enable")));
			json_object_object_add(paramJobj, "L2TPv3_LocalIPAddress", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "LocalIPAddress")));
			json_object_object_add(paramJobj, "L2TPv3_RemoteIPAddress", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "RemoteIPAddress")));
			json_object_object_add(paramJobj, "L2TPv3_LocalTunnelAddress", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "LocalTunnelAddress")));
			json_object_object_add(paramJobj, "L2TPv3_RemoteTunnelAddress", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "RemoteTunnelAddress")));
			json_object_object_add(paramJobj, "L2TPv3_LocalTunnelID", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "LocalTunnelID")));
			json_object_object_add(paramJobj, "L2TPv3_RemoteTunnelID", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "RemoteTunnelID")));
			json_object_object_add(paramJobj, "L2TPv3_LocalSessionID", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "LocalSessionID")));
			json_object_object_add(paramJobj, "L2TPv3_RemoteSessionID", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "RemoteSessionID")));
			json_object_object_add(paramJobj, "L2TPv3_SourcePort", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "SourcePort")));
			json_object_object_add(paramJobj, "L2TPv3_DestinationPort", JSON_OBJ_COPY(json_object_object_get(l2tpv3Obj, "DestinationPort")));
			zcfgFeJsonObjFree(l2tpv3Obj);
		}
		else{
			//do nothing
		}
		l2tpv3Iid.idx[0]++;
#endif

		if(json_object_object_get(Jobj, "Index")){
			index = Jgeti(Jobj, "Index");
			if(index == accessPointIid.idx[0]){
				json_object_array_add(Jarray, JSON_OBJ_COPY(paramJobj));

				zcfgFeJsonObjFree(paramJobj);
				zcfgFeJsonObjFree(accessPointObj);
				break;
			}
		}
		else{
			json_object_array_add(Jarray, JSON_OBJ_COPY(paramJobj));
		}

		zcfgFeJsonObjFree(paramJobj);
		zcfgFeJsonObjFree(accessPointObj);
	}

	return ret;
}


typedef struct display_handler_s {
	int			width;
	int			minValue;
	char		*key;
	char		*trueValue;
	char		*falseValue;
}display_handler_t;

char *isMeetCriteria(bool criteria, char *trueValue, char *falseValue){
	return (criteria) ? trueValue : falseValue;
}

int jsonValueConvertNDisplay(json_object *obj, int width, int minValue, char *key, char *trueValue, char *falseValue)
{
	int tmpInt;
	bool tmpBool;
	char *tmpString = NULL;
	json_object *value = NULL;
	zcfgRet_t ret = ZCFG_SUCCESS;

	if((value = json_object_object_get(obj, key)) == NULL)
		return ZCFG_INTERNAL_ERROR;

	switch(json_object_get_type(value))
	{
		case json_type_boolean:
			/* printf("json_object_get_boolean(value):%d\n", json_object_get_boolean(value)); */
			tmpBool = json_object_get_boolean(value);
			printf("%*s ", width, isMeetCriteria(tmpBool, trueValue, falseValue));
			break;
		case json_type_int:
			/* printf("json_object_get_int(value):%lld\n", json_object_get_int(value)); */
			/* printf("json_object_get_string(value):%s\n", json_object_get_string(value)); */
			tmpInt = json_object_get_int(value);
			tmpString = json_object_get_string(value);
			printf("%*s ", width, isMeetCriteria(tmpInt == minValue, "N/A", tmpString));
			break;
		case json_type_string:
			/* printf("json_object_get_string(value):%s\n", json_object_get_string(value)); */
			tmpString = json_object_get_string(value);
			printf("%*s ", width, isMeetCriteria(!strcmp(tmpString, ""), "N/A", tmpString));
			break;
		default:
			ret = ZCFG_INVALID_OBJECT;
			printf("[%d] F:%s, unconverted type: %d\n", __LINE__, __func__, json_object_get_type(value));
			break;
	}

	return ret;
}

void zcfgFeDalShowCellWanMApn(struct json_object *Jarray){
	zcfgRet_t retConvert;
	json_object *Jobj = NULL;
	int idx, len = 0, handlerIdx = 0;
	display_handler_t displayHandler[] = {
		{-10,	0,	"AP_Enable",	"Enabled",	"Disabled"},
		{-10,	0,	"AP_ManualAPN",	"Manual",	"Automatic"},
		{-20,	0,	"AP_APN",	NULL,	NULL},
		{-20,	0,	"AP_Username",	NULL,	NULL},
		{-20,	0,	"AP_Password",	NULL,	NULL},
		{-15,	0,	"AP_Auth_Type",	NULL,	NULL},
		{-15,	0,	"AP_PDP_Type",	NULL,	NULL},
		{-10,   0,	"DHCP_LeaseTime", NULL, NULL},
		{0,		0,	NULL,		NULL,	NULL}
	};

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	printf("%-5s %-10s %-10s %-20s %-20s %-20s %-15s %-15s %-10s\n", "#", "Enable", "Mode", "APN", "Username", "Password", "Auth Type", "PDP Type","DHCP_LeaseTime");
	len = json_object_array_length(Jarray);
	for(idx = 0; idx < len; idx++){
		Jobj = json_object_array_get_idx(Jarray, idx);
		printf("%-5d ", idx+1);
		for(handlerIdx = 0; displayHandler[handlerIdx].key != NULL; handlerIdx++){
			retConvert = jsonValueConvertNDisplay(Jobj,
					displayHandler[handlerIdx].width,
					displayHandler[handlerIdx].minValue,
					displayHandler[handlerIdx].key,
					displayHandler[handlerIdx].trueValue,
					displayHandler[handlerIdx].falseValue);
			if(retConvert != ZCFG_SUCCESS)
				printf("[%d] F:%s K:%s, jsonValueConvertNDisplay Fail.\n", __LINE__, __func__, displayHandler[handlerIdx].key);
		}
		printf("\n");
	}
}


zcfgRet_t zcfgFeDalCellWanMApn(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanMApnEdit(Jobj, replyMsg);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanMApnGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

