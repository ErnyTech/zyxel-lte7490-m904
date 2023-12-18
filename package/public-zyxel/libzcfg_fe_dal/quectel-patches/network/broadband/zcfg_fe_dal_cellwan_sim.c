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

dal_param_t CELLWAN_SIM_param[]={
{"USIM_PIN_Protection", dalType_boolean, 0, 0, NULL},
/* {"USIM_Auto_Unlock", dalType_boolean, 0, 0, NULL}, */
{"USIM_PIN", dalType_string, 0, 0, NULL},
{"USIM_PUK", dalType_string, 0, 0, NULL},
{"USIM_New_PIN", dalType_string, 0, 0, NULL},
{"USIM_PIN_STATE", dalType_string, 0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}

};

#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
dal_param_t CELLWAN_ESIM_param[]={
{"LpaEnable", dalType_boolean, 0, 0, NULL},
{"ActivationCode", dalType_string, 0, 0, NULL},
{ NULL, 0,      0,      0,      NULL}

};

dal_param_t CELLWAN_ESIM_PROFILE_param[]={
{"Index", dalType_int, 1, 8,  NULL,   NULL, dal_Edit|dal_Delete},
{"Profile_Enable", dalType_boolean, 0, 0, NULL},
{ NULL, 0,      0,      0,      NULL}

};
#endif

zcfgRet_t zcfgFeDalCellWanSimEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *intfUsimObj = NULL;
	objIndex_t intfUsimIid = {0};
	bool pinProtection = false;
	/* bool autoUnlockPin = false; */
	const char *pin = NULL;
	const char *puk = NULL;
	const char *new_pin = NULL;
	const char *pin_state = NULL;

	IID_INIT(intfUsimIid);
	intfUsimIid.level=1;
	intfUsimIid.idx[0]=1; 

	pinProtection = json_object_get_boolean(json_object_object_get(Jobj, "USIM_PIN_Protection"));
	/* autoUnlockPin = json_object_get_boolean(json_object_object_get(Jobj, "USIM_Auto_Unlock")); */
	pin = json_object_get_string(json_object_object_get(Jobj, "USIM_PIN"));
	puk = json_object_get_string(json_object_object_get(Jobj, "USIM_PUK"));
	new_pin = json_object_get_string(json_object_object_get(Jobj, "USIM_New_PIN"));
	pin_state = json_object_get_string(json_object_object_get(Jobj, "USIM_PIN_STATE"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_USIM, &intfUsimIid, &intfUsimObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "USIM_PIN_Protection"))
			json_object_object_add(intfUsimObj, "X_ZYXEL_PIN_Protection", json_object_new_boolean(pinProtection));
		/* if(json_object_object_get(Jobj, "USIM_Auto_Unlock")) */
			/* json_object_object_add(intfUsimObj, "X_ZYXEL_AutoUnlockPIN", json_object_new_boolean(autoUnlockPin)); */
		if(json_object_object_get(Jobj, "USIM_PIN"))
			json_object_object_add(intfUsimObj, "PIN", json_object_new_string(pin));
		if(json_object_object_get(Jobj, "USIM_PUK"))
			json_object_object_add(intfUsimObj, "X_ZYXEL_PUK", json_object_new_string(puk));
		if(json_object_object_get(Jobj, "USIM_New_PIN"))
			json_object_object_add(intfUsimObj, "X_ZYXEL_NewPIN", json_object_new_string(new_pin));
		if(json_object_object_get(Jobj, "USIM_PIN_STATE"))
			json_object_object_add(intfUsimObj, "X_ZYXEL_PIN_STATE", json_object_new_string(pin_state));
		zcfgFeObjJsonSet(RDM_OID_CELL_INTF_USIM, &intfUsimIid, intfUsimObj, NULL);
	}

	zcfgFeJsonObjFree(intfUsimObj);

	return ret;
}	

zcfgRet_t zcfgFeDalCellWanSimGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *intfUsimObj = NULL;
	objIndex_t intfUsimIid = {0};
	paramJobj = json_object_new_object();

	IID_INIT(intfUsimIid);
	intfUsimIid.level=1;
	intfUsimIid.idx[0]=1;
	if((ret =  zcfgFeObjJsonGet(RDM_OID_CELL_INTF_USIM, &intfUsimIid, &intfUsimObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "USIM_Status", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "Status")));
		json_object_object_add(paramJobj, "USIM_IMSI", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "IMSI")));
		json_object_object_add(paramJobj, "USIM_ICCID", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "ICCID")));
		/* json_object_object_add(paramJobj, "USIM_PIN", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "PIN"))); */
		/* json_object_object_add(paramJobj, "USIM_New_PIN", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_NewPIN"))); */
		/* json_object_object_add(paramJobj, "USIM_Auto_Unlock", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_AutoUnlockPIN"))); */
		json_object_object_add(paramJobj, "USIM_PIN_Protection", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_PIN_Protection")));
		json_object_object_add(paramJobj, "USIM_PIN_STATE", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_PIN_STATE")));
		json_object_object_add(paramJobj, "USIM_PIN_RemainingAttempts", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_PIN_RemainingAttempts")));
		json_object_object_add(paramJobj, "USIM_PUK_RemainingAttempts", JSON_OBJ_COPY(json_object_object_get(intfUsimObj, "X_ZYXEL_PUK_RemainingAttempts")));

		zcfgFeJsonObjFree(intfUsimObj);
	}

	json_object_array_add(Jarray, paramJobj);
	return ret;
}

void zcfgFeDalShowCellWanSim(struct json_object *Jarray){
	struct json_object *obj = NULL;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	obj = json_object_array_get_idx(Jarray, 0);
	
	printf("\nCellular SIM Settings:\n");
	printf("%-30s %-10s \n","SIM Card Status:", json_object_get_string(json_object_object_get(obj, "USIM_Status")));
	printf("%-30s %-10s \n","IMSI:", json_object_get_string(json_object_object_get(obj, "USIM_IMSI")));
	printf("%-30s %-10s \n","ICCID:", json_object_get_string(json_object_object_get(obj, "USIM_ICCID")));
	if(json_object_get_boolean(json_object_object_get(obj, "USIM_PIN_Protection")))
		printf("%-30s %-10s\n","PIN Protection:", "Enable");
	else
		printf("%-30s %-10s\n","PIN Protection:", "Disable");
	printf("%-30s %-10s \n","Attempts remaining:", json_object_get_string(json_object_object_get(obj, "USIM_PIN_RemainingAttempts")));
	printf("%-30s %-10s \n","PIN STATE:", json_object_get_string(json_object_object_get(obj, "USIM_PIN_STATE")));
	
}


zcfgRet_t zcfgFeDalCellWanSim(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanSimEdit(Jobj, NULL);
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanSimGet(Jobj, Jarray, NULL);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
zcfgRet_t zcfgFeDalCellWanESimEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *ZyEsimObj = NULL;
	objIndex_t ZyEsimIid = {0};
	zcfg_offset_t ZyEsimRdmOid = RDM_OID_CELL_ZY_ESIM;
	const char *activationCode = NULL;
	bool lpaEnable = false;

	lpaEnable = Jgetb(Jobj, "LpaEnable");

	IID_INIT(ZyEsimIid);

	activationCode = json_object_get_string(json_object_object_get(Jobj, "ActivationCode"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyEsimRdmOid, &ZyEsimIid, &ZyEsimObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "LpaEnable")){
			json_object_object_add(ZyEsimObj, "LpaEnable", json_object_new_boolean(lpaEnable));
		}
		if(json_object_object_get(Jobj, "ActivationCode")){
			json_object_object_add(ZyEsimObj, "ActivationCode", json_object_new_string(activationCode));
		}
	} else {
		printf("get object fail\n");
	}

	if((ret = zcfgFeObjJsonSet(ZyEsimRdmOid, &ZyEsimIid, ZyEsimObj, NULL)) != ZCFG_SUCCESS){
		zcfgFeJsonObjFree(ZyEsimObj);
		printf("zcfgFeObjJsonSet RDM_OID_CELL_ZY_ESIM fail.\n");
		return ret;
	}

	if (ZyEsimObj) zcfgFeJsonObjFree(ZyEsimObj);

	return ret;
}

zcfgRet_t zcfgFeDalCellWanESim(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
        zcfgRet_t ret = ZCFG_SUCCESS;

        if(!strcmp(method, "PUT"))
                ret = zcfgFeDalCellWanESimEdit(Jobj, replyMsg);
	else if (!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanESimGet(Jobj, Jarray, replyMsg);
        else
                printf("Unknown method:%s\n", method);

        return ret;
}

zcfgRet_t zcfgFeDalCellWanESimProfileEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *ZyEsimProfileObj = NULL;
	objIndex_t ZyEsimProfileIid = {0};
	zcfg_offset_t ZyEsimProfileRdmOid = RDM_OID_CELL_ZY_ESIM_PROFILE;
	int index = -1;
	bool profileEnable = false;

	IID_INIT(ZyEsimProfileIid);

	index = json_object_get_int(json_object_object_get(Jobj, "Index"));

	ret = convertIndextoIid(index, &ZyEsimProfileIid, ZyEsimProfileRdmOid, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS) {
		printf("convertIndextoIid fail\n");
		return ret;
	}

	ZyEsimProfileIid.idx[0] = index;

	profileEnable = json_object_get_boolean(json_object_object_get(Jobj, "Profile_Enable"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyEsimProfileRdmOid, &ZyEsimProfileIid, &ZyEsimProfileObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "Profile_Enable")){
			json_object_object_add(ZyEsimProfileObj, "Enable", json_object_new_boolean(profileEnable));
		}
		if((ret = zcfgFeObjJsonSet(ZyEsimProfileRdmOid, &ZyEsimProfileIid, ZyEsimProfileObj, NULL)) != ZCFG_SUCCESS){
			zcfgFeJsonObjFree(ZyEsimProfileObj);
			printf("zcfgFeObjJsonSet RDM_OID_CELL_ZY_ESIM_PROFILE fail.\n");
			return ret;
		}
	} else {
		printf("get object fail\n");
	}

	if (ZyEsimProfileObj) zcfgFeJsonObjFree(ZyEsimProfileObj);

	return ret;
}

zcfgRet_t zcfgFeDalCellWanESimProfileDelete(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *ZyEsimProfileObj = NULL;
	objIndex_t ZyEsimProfileIid = {0};
	zcfg_offset_t ZyEsimProfileRdmOid = RDM_OID_CELL_ZY_ESIM_PROFILE;
	int index = 0;

	IID_INIT(ZyEsimProfileIid);

	index = Jgeti(Jobj, "Index");

	ret = convertIndextoIid(index, &ZyEsimProfileIid, ZyEsimProfileRdmOid, NULL, NULL, replyMsg);
	if(ret != ZCFG_SUCCESS) {
		printf("convertIndextoIid fail\n");
		return  ret;
	}

	ZyEsimProfileIid.idx[0] = index;

        if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyEsimProfileRdmOid, &ZyEsimProfileIid, &ZyEsimProfileObj)) == ZCFG_SUCCESS){

		json_object_object_add(ZyEsimProfileObj, "Nickname", json_object_new_string("delete"));

	        if((ret = zcfgFeObjJsonSet(ZyEsimProfileRdmOid, &ZyEsimProfileIid, ZyEsimProfileObj, NULL)) != ZCFG_SUCCESS){
        	        zcfgFeJsonObjFree(ZyEsimProfileObj);
                        printf("zcfgFeObjJsonSet RDM_OID_CELL_ZY_ESIM_PROFILE fail.\n");
                        return ret;
	        }
        } else {
                printf("get object fail\n");
        }

        if (ZyEsimProfileObj) zcfgFeJsonObjFree(ZyEsimProfileObj);

        return ret;

}

void zcfgFeDalShowCellWanESim(struct json_object *Jarray){
	objIndex_t iid = {0};
	struct json_object *Jobj = NULL;
	struct json_object *profileListObj = NULL;
	struct json_object *profileListArray = NULL;
	int i, numbers = 0, len = 0, index = 0;
	
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	Jobj = json_object_array_get_idx(Jarray, 0);
	printf("%-30s %s\n","Enable:", Jgets(Jobj, "LpaEnable"));
	printf("%-30s %s\n","EID:", Jgets(Jobj, "Eid"));
	numbers = Jgeti(Jobj, "NumberOfEsimProfileList");
	printf("%-30s %d\n","Number Of Esim Profile List:", numbers);

	if (numbers) {
		profileListArray = json_object_object_get(Jobj, "profileList");
		len = json_object_array_length(profileListArray);
		printf("%-10s %-10s %-25s %-25s %-25s %-25s %-10s\n","Index", "Enable","ICCID", "Nickname", "SPN","Name","Class");

		for(i = 0; i < len; i++) {
			IID_INIT(iid);
			profileListObj = json_object_array_get_idx(profileListArray, i);
			index = i+1;
			printf("%-10d %-10s %-25s %-25s %-25s %-25s %-10s\n", index, Jgets(profileListObj,"Enable"), Jgets(profileListObj,"ICCID"), Jgets(profileListObj,"Nickname"), Jgets(profileListObj,"SPN"), Jgets(profileListObj,"Name"), Jgets(profileListObj,"Class"));
		}
		zcfgFeJsonObjFree(profileListArray);
	}
}

zcfgRet_t zcfgFeDalCellWanESimProfile(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
        zcfgRet_t ret = ZCFG_SUCCESS;

        if(!strcmp(method, "PUT"))
                ret = zcfgFeDalCellWanESimProfileEdit(Jobj, replyMsg);
        else if (!strcmp(method, "GET"))
                ret = zcfgFeDalCellWanESimGet(Jobj, Jarray, replyMsg);
	else if (!strcmp(method, "DELETE"))
		ret = zcfgFeDalCellWanESimProfileDelete(Jobj, replyMsg);
        else
                printf("Unknown method:%s\n", method);

        return ret;
}

zcfgRet_t zcfgFeDalCellWanESimGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *ZyEsimObj = NULL;
	struct json_object *profileListArray = NULL, *profileListObj = NULL;
	zcfg_offset_t ZyEsimRdmOid = RDM_OID_CELL_ZY_ESIM;
	zcfg_offset_t ZyEsimProfileRdmOid = RDM_OID_CELL_ZY_ESIM_PROFILE;
	objIndex_t ZyEsimIid = {0};
	bool ZyEsimEnable = false;

	paramJobj = json_object_new_object();

	IID_INIT(ZyEsimIid);

	if((ret = zcfgFeObjJsonGetWithoutUpdate(ZyEsimRdmOid, &ZyEsimIid, &ZyEsimObj)) == ZCFG_SUCCESS){
		ZyEsimEnable = Jgetb(ZyEsimObj, "LpaEnable");
		json_object_object_add(paramJobj, "LpaEnable", json_object_new_boolean(ZyEsimEnable));
		json_object_object_add(paramJobj, "Eid", JSON_OBJ_COPY(json_object_object_get(ZyEsimObj, "Eid")));
		json_object_object_add(paramJobj, "NumberOfEsimProfileList", JSON_OBJ_COPY(json_object_object_get(ZyEsimObj, "NumberOfEsimProfileList")));
		zcfgFeJsonObjFree(ZyEsimObj);
	}

	IID_INIT(ZyEsimIid);
	profileListArray = json_object_new_array();
	while(zcfgFeObjJsonGetNext(ZyEsimProfileRdmOid, &ZyEsimIid, &profileListObj) == ZCFG_SUCCESS) {
		json_object_array_add(profileListArray, profileListObj);
	}
	json_object_object_add(paramJobj, "profileList", profileListArray);
	json_object_array_add(Jarray, paramJobj);

	return ret;
}
#endif
