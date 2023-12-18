#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_debug.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_rdm_oid.h"
//#include "zcfg_eid.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_msg.h"

dal_param_t CUSTOM_DHCP_param[] = {
    { "Index", dalType_int, 0, 0, NULL, NULL, dal_Edit | dal_Delete },
    { "OptId", dalType_string, 0, 0, NULL, NULL, dal_Add },
    { "SrvName", dalType_IntfGrpLanPortListNoWiFi, 0, 0, NULL, NULL, dal_Add },
    { "OptContext", dalType_string, 0, 0, NULL, NULL, dal_Add },
    { NULL, 0, 0, 0, NULL, NULL, NULL }
};

void zcfgFeDalShowCustomDHCP(struct json_object* Jarray)
{
    int i, len = 0;
    struct json_object* obj = NULL;

    if (json_object_get_type(Jarray) != json_type_array) {
        printf("wrong Jobj format!\n");
        return;
    }
    printf("%-10s %-30s %-30s %-30s \n",
        "Index", "Option ID", "Service Name", "Option Context");
    len = json_object_array_length(Jarray);
    for (i = 0; i < len; i++) {
        obj = json_object_array_get_idx(Jarray, i);
        printf("%-10s %-30s %-30s %-30s \n",
            json_object_get_string(json_object_object_get(obj, "Index")),
            json_object_get_string(json_object_object_get(obj, "OptId")),
            json_object_get_string(json_object_object_get(obj, "SrvName")),
            json_object_get_string(json_object_object_get(obj, "OptContext")));
    }
}

zcfgRet_t zcfgFeDalCustom_DHCP_Add(struct json_object* Jobj, char* replyMsg)
{
    zcfgRet_t ret = ZCFG_SUCCESS;
    struct json_object* dhcpSrvCusOptObj = NULL;
    objIndex_t dhcpSrvCusOptIid = { 0 };
    const char* IfName = NULL;
    const char* dhcpIfacePath = NULL;
    int count = 0;
    const char* optId = NULL;
    const char* optContext = NULL;
    const char* srvName = NULL;
    char ifacePath[32] = { 0 };

    ret = zcfgFeObjJsonAdd(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, NULL);
    if (ret == ZCFG_SUCCESS) {
        ret = zcfgFeObjJsonGet(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, &dhcpSrvCusOptObj);
    }

    if (ret == ZCFG_SUCCESS) {
        optId = json_object_get_string(json_object_object_get(Jobj, "OptId"));
        optContext = json_object_get_string(json_object_object_get(Jobj, "OptContext"));
        srvName = json_object_get_string(json_object_object_get(Jobj, "SrvName"));
        json_object_object_add(dhcpSrvCusOptObj, "OptId", json_object_new_string(optId));
        json_object_object_add(dhcpSrvCusOptObj, "OptContext", json_object_new_string(optContext));
        json_object_object_add(dhcpSrvCusOptObj, "SrvName", json_object_new_string(srvName));

        ret = zcfgFeObjJsonSet(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, dhcpSrvCusOptObj, NULL);
    }

    zcfgFeJsonObjFree(dhcpSrvCusOptObj);
    return ret;
}

zcfgRet_t zcfgFeDal_Custom_DHCP_Edit(struct json_object* Jobj, char* replyMsg)
{
    zcfgRet_t ret = ZCFG_SUCCESS;
    struct json_object* dhcpSrvCusOptObj = NULL;
    objIndex_t dhcpSrvCusOptIid = { 0 };
    int rowIndex = 0;
    int count = 1;
    const char* optId = NULL;
    const char* optContext = NULL;
    const char* srvName = NULL;
    bool found = false;

    rowIndex = json_object_get_int(json_object_object_get(Jobj, "Index"));

    IID_INIT(dhcpSrvCusOptIid);
    while (zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, &dhcpSrvCusOptObj) == ZCFG_SUCCESS) {
        if (count == rowIndex) {
            found = true;
            break;
        }
        count++;
        zcfgFeJsonObjFree(dhcpSrvCusOptObj);
    }
    if (found) {
        optId = json_object_get_string(json_object_object_get(Jobj, "OptId"));
        optContext = json_object_get_string(json_object_object_get(Jobj, "OptContext"));
        srvName = json_object_get_string(json_object_object_get(Jobj, "SrvName"));
        if (json_object_object_get(Jobj, "OptId") != NULL)
            json_object_object_add(dhcpSrvCusOptObj, "OptId", json_object_new_string(optId));
        if (json_object_object_get(Jobj, "OptContext") != NULL)
            json_object_object_add(dhcpSrvCusOptObj, "OptContext", json_object_new_string(optContext));
        if (json_object_object_get(Jobj, "SrvName") != NULL)
            json_object_object_add(dhcpSrvCusOptObj, "SrvName", json_object_new_string(srvName));

        ret = zcfgFeObjJsonSet(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, dhcpSrvCusOptObj, NULL);
        zcfgFeJsonObjFree(dhcpSrvCusOptObj);
    }
    return ret;
}

zcfgRet_t zcfgFeDalCustom_DHCP_Delete(struct json_object* Jobj, char* replyMsg)
{
    zcfgRet_t ret = ZCFG_SUCCESS;
    struct json_object* dhcpSrvCusOptObj = NULL;
    objIndex_t dhcpSrvCusOptIid = { 0 };
    int rowIndex = 0;
    int count = 1;
    bool found = false;

    rowIndex = json_object_get_int(json_object_object_get(Jobj, "Index"));
    IID_INIT(dhcpSrvCusOptIid);

    while (zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, &dhcpSrvCusOptObj) == ZCFG_SUCCESS) {
        if (count == rowIndex) {
            found = true;
            break;
        }
        count++;
        zcfgFeJsonObjFree(dhcpSrvCusOptObj);
    }

    if (found) {
        ret = zcfgFeObjJsonDel(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, NULL);
        zcfgFeJsonObjFree(dhcpSrvCusOptObj);
    }

    return ret;
}

// void getBrName(int iid, char *brName){
// 	struct json_object *brObj = NULL;
// 	objIndex_t brIid = {0};
// 	IID_INIT(brIid);
// 	memset(brName, 0, sizeof(brName));
// 	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_BRIDGING_BR, &brIid, &brObj) == ZCFG_SUCCESS) {
// 		if(iid == brIid.idx[0] ){
// 			strcpy(brName, json_object_get_string(json_object_object_get(brObj, "X_ZYXEL_BridgeName")));
// 		}
// 		zcfgFeJsonObjFree(brObj);
// 	}
// }

void getCustomDHCPObj(int count, int iid, struct json_object* dhcpSrvAddrObj, struct json_object* Jarray)
{
    struct json_object* paramJobj = NULL;
    char* brName[33] = { 0 };

    paramJobj = json_object_new_object();
    json_object_object_add(paramJobj, "Index", json_object_new_int(count));
    json_object_object_add(paramJobj, "OptId", JSON_OBJ_COPY(json_object_object_get(dhcpSrvAddrObj, "OptId")));
    json_object_object_add(paramJobj, "OptContext", JSON_OBJ_COPY(json_object_object_get(dhcpSrvAddrObj, "OptContext")));
    json_object_object_add(paramJobj, "SrvName", JSON_OBJ_COPY(json_object_object_get(dhcpSrvAddrObj, "SrvName")));
    json_object_array_add(Jarray, paramJobj);
    zcfgFeJsonObjFree(dhcpSrvAddrObj);
}

zcfgRet_t zcfgFeDalCustom_DHCP_Get(struct json_object* Jobj, struct json_object* Jarray, char* replyMsg)
{
    zcfgRet_t ret = ZCFG_SUCCESS;
    struct json_object* dhcpSrvCusOptObj = NULL; //Get data model parameter
    objIndex_t dhcpSrvCusOptIid = { 0 };
    int index = 0;

    IID_INIT(dhcpSrvCusOptIid);
    if (json_object_object_get(Jobj, "Index")) { //for New GUI get single obj
        index = json_object_get_int(json_object_object_get(Jobj, "Index"));
        ret = convertIndextoIid(index, &dhcpSrvCusOptIid, RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, NULL, NULL, replyMsg);
        if (ret != ZCFG_SUCCESS)
            return ret;
        if (zcfgFeObjJsonGetWithoutUpdate(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, &dhcpSrvCusOptObj) == ZCFG_SUCCESS) {
            getCustomDHCPObj(index, dhcpSrvCusOptIid.idx[0], dhcpSrvCusOptObj, Jarray);
        }
    } else {
        while (zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT, &dhcpSrvCusOptIid, &dhcpSrvCusOptObj) == ZCFG_SUCCESS) {
            index++;
            getCustomDHCPObj(index, dhcpSrvCusOptIid.idx[0], dhcpSrvCusOptObj, Jarray);
        }
    }
    return ret;
}

zcfgRet_t zcfgFeDalCustomDHCP(const char* method, struct json_object* Jobj, struct json_object* Jarray, char* replyMsg)
{
    zcfgRet_t ret = ZCFG_SUCCESS;

    if (!method || !Jobj)
        return ZCFG_INTERNAL_ERROR;

    if (!strcmp(method, "PUT")) {
        ret = zcfgFeDal_Custom_DHCP_Edit(Jobj, NULL);
    } else if (!strcmp(method, "POST")) {
        ret = zcfgFeDalCustom_DHCP_Add(Jobj, NULL);
    } else if (!strcmp(method, "DELETE")) {
        ret = zcfgFeDalCustom_DHCP_Delete(Jobj, NULL);
    } else if (!strcmp(method, "GET"))
        ret = zcfgFeDalCustom_DHCP_Get(Jobj, Jarray, NULL);
    else
        printf("Unknown method:%s\n", method);

    return ret;
}
