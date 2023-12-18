#include <ctype.h>
#include <json/json.h>
//#include <json/json_object.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"

dal_param_t BT_CTL_param[] = 
{
	{"name"	     ,		dalType_string,       0, 0, NULL,             NULL,                            dal_Edit},
	{"devicename",		dalType_string,       0, 0, NULL,		NULL,			0},
	{"pincode"   ,	        dalType_string,       0, 0, NULL,		NULL, 	                0},
	{"enable",		dalType_boolean,      0, 0, NULL, 	        NULL,			       dal_Edit},
	{NULL,						  0,	  0, 0, NULL, 	     NULL,					        NULL}
};


void zcfgFeDalShowBT(struct json_object *Jarray){
	int i, len = 0;
	struct json_object *obj;
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *btObj = NULL;
	struct json_object *paramJobj = NULL;
	objIndex_t btIid = {0};
	const char *name = NULL;
	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}
	printf("setting for bluetooth\n");
	printf("%-15s %-15s %-15s %-15s \n","Interface","Device Name","PinCode","Enable");
	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		name = json_object_get_string(json_object_object_get(obj, "bt_Name"));
		if(strcmp(name,"") != 0){
			printf("%-15s %-15s %-15s %-15d \n",
				json_object_get_string(json_object_object_get(obj, "bt_Name")),
				json_object_get_string(json_object_object_get(obj, "bt_DeviceName")),
				json_object_get_string(json_object_object_get(obj, "bt_PinCode")),
				json_object_get_boolean(json_object_object_get(obj, "bt_Enable")));
		}
	}

}

zcfgRet_t zcfgFeDalBTGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *bthObj = NULL;
	struct json_object *paramJobj = NULL;
	objIndex_t bthIid = {0};

	while(zcfgFeObjJsonGetNext(RDM_OID_BT_INTF, &bthIid, &bthObj) == ZCFG_SUCCESS){
		paramJobj = json_object_new_object();
		//printf("Keith:(line:%d)==== %s,bt_DeviceName:%s",__LINE__,__FUNCTION__,json_object_get_string(json_object_object_get(bthObj, "Name")));
		json_object_object_add(paramJobj, "bt_Name", JSON_OBJ_COPY(json_object_object_get(bthObj, "Name")));
		json_object_object_add(paramJobj, "bt_DeviceName", JSON_OBJ_COPY(json_object_object_get(bthObj, "DeviceName")));
		json_object_object_add(paramJobj, "bt_Enable", JSON_OBJ_COPY(json_object_object_get(bthObj, "Enable")));
		json_object_object_add(paramJobj, "bt_PinCode", JSON_OBJ_COPY(json_object_object_get(bthObj, "PinCode")));
		json_object_array_add(Jarray, paramJobj);
		zcfgFeJsonObjFree(bthObj);
	}
	return ret;
}

zcfgRet_t zcfgFeDalBTEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret= ZCFG_SUCCESS;
	struct json_object *bthObj = NULL;
	objIndex_t bthIid = {0};
	const char *targetName = NULL;
	const char *name = NULL;
	struct json_object *temp_obj =NULL;
	char *p_user_str=NULL, *p_jason_str=NULL;
	int index = 0;
	bool newEnable = false;
	bool curEnable = false;
	bool found = false;
	bool bchange = false;

	
	//printf("####name:%s enable:%s\n",json_object_get_string(json_object_object_get(Jobj, "name")),json_object_get_string(json_object_object_get(Jobj, "enable")));
	targetName = json_object_get_string(json_object_object_get(Jobj, "name"));
	sscanf(targetName, "hci%d", &index);
	//printf("Keith:(line:%d)==== %s,targetName:%s\n",__LINE__,__FUNCTION__,targetName);
	IID_INIT(bthIid);
	bthIid.level = 1;
	bthIid.idx[0] = index+1;
	
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_BT_INTF, &bthIid, &bthObj) == ZCFG_SUCCESS) {
		name = json_object_get_string(json_object_object_get(bthObj, "Name"));
		if((targetName != NULL) && !strcmp(targetName,name)){
			curEnable  =  json_object_get_boolean(json_object_object_get(bthObj, "Enable"));
			found = true;
		}
	}

	if(found) {
		if((json_object_object_get(Jobj, "enable") == NULL) ){
			strcpy(replyMsg, "Invalid parameters! Please enter --Name  <interface> --enable <1|0>");
			return ZCFG_REQUEST_REJECT;
		}else{
			//Enable/disable
			newEnable = json_object_get_int(json_object_object_get(Jobj, "enable"));
			if(newEnable != curEnable)
			{
				json_object_object_add(bthObj, "Enable", json_object_new_boolean(newEnable));
				bchange = true;	
			}

			//devicename
			temp_obj = json_object_object_get(Jobj, "devicename");
			
			if(temp_obj != NULL)
			{
				
				p_jason_str = json_object_get_string(json_object_object_get(bthObj, "DeviceName"));
				p_user_str = json_object_get_string(temp_obj);
				if(strlen(p_user_str) && p_user_str[0]!=0x20)
				{
					if(p_jason_str != NULL && strcmp(p_jason_str,p_user_str)!=0)
					{
						json_object_object_add(bthObj, "DeviceName", json_object_new_string(p_user_str));
						bchange = true;
					}
				}
				else
				{
					strcpy(replyMsg, "Invalid parameters! The devicename is empty or first character is a space!!!");
					return ZCFG_REQUEST_REJECT;
				}
			}

			//pincode
			temp_obj = json_object_object_get(Jobj, "pincode");
			if(temp_obj != NULL)
			{
				p_jason_str = json_object_get_string(json_object_object_get(bthObj, "PinCode"));
				p_user_str = json_object_get_string(temp_obj);
				if(strlen(p_user_str) && p_user_str[0]!=0x20)
				{
					if(p_jason_str != NULL && strcmp(p_jason_str,p_user_str)!=0)
					{
						json_object_object_add(bthObj, "PinCode", json_object_new_string(p_user_str));
						bchange = true;
					}
				}
				else
				{
					strcpy(replyMsg, "Invalid parameters! The pincode is empty or first character is a space!!!");
					return ZCFG_REQUEST_REJECT;
				}
			}
		
			if(bchange)
				ret = zcfgFeObjJsonSet(RDM_OID_BT_INTF, &bthIid, bthObj, NULL);
				
		}
	}
	else{
		return ZCFG_NOT_FOUND;
	}

	zcfgFeJsonObjFree(bthObj);
	return ret;
}

zcfgRet_t zcfgFeDalBT(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg) {
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(!strcmp(method, "PUT")) 
	{
		//printf("**name:%s enable:%s\n",json_object_get_string(json_object_object_get(Jobj, "name")),json_object_get_string(json_object_object_get(Jobj, "enable")));
		ret = zcfgFeDalBTEdit(Jobj, replyMsg);
	}
	else if(!strcmp(method, "GET"))
		ret = zcfgFeDalBTGet(Jobj, Jarray, replyMsg);
	else 
		printf("Unknown method:%s\n", method);
	return ret;
}
