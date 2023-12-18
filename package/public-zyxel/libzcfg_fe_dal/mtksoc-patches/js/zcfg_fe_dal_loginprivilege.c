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

#define DAL_LOGIN_USERNAME "DAL_LOGIN_USERNAME"

dal_param_t LOGIN_PRIVILEGE_param[]={		
	{"Enabled", 	 dalType_boolean,	0,	0,	NULL},
	{"Username", 	 dalType_string,	0,	0,	NULL},
	{"Password", 	 dalType_string,	0,	0,	NULL},
	{"EnableQuickStart", 	 dalType_boolean,	0,	0,	NULL},
	{"Page", 	 dalType_string,	0,	0,	NULL},
	{NULL,					0,	0,	0,	NULL},

};

zcfgRet_t zcfgFeDal_LoginPrivilege_Edit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *loginPrivilegeObj = NULL;
	struct json_object *loginPrivilegeAccountObj = NULL;
	struct json_object *reqObjectindex = NULL;
	struct json_object *reqObject = NULL;
	objIndex_t loginPrivilegeIid = {0};
	bool enable = false;
	const char *username = NULL;
	const char *password = NULL;
	const char *oldPassword = NULL;
	bool enablequickstart = false;
	const char *page = NULL;
	int index0 = 0;
	int index1 = 0;
	
#if 1  // login_privilege edit is available for supervisor account if at GUI dal handler called
	const char* loginAccount = json_object_get_string(json_object_object_get(Jobj, DAL_LOGIN_USERNAME));
	if (strcmp(loginAccount, "supervisor")) {
		printf("(%s:%d) GUI dal handler (login_privilege) EDIT method is for supervisor only\n", __func__, __LINE__);
		return ZCFG_REQUEST_REJECT;
	}
#endif

	IID_INIT(loginPrivilegeIid);
	reqObjectindex = json_object_array_get_idx(Jobj,0);
	index0 = json_object_get_int(json_object_object_get(reqObjectindex, "Index0"));
	index1 = json_object_get_int(json_object_object_get(reqObjectindex, "Index1"));
	
	if(index0 == 1 ){
		loginPrivilegeIid.idx[0] = index0;
		loginPrivilegeIid.idx[1] = index1;
		loginPrivilegeIid.level = 2;
		ret = zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, &loginPrivilegeObj);
		if(ret == ZCFG_SUCCESS){
			reqObject = json_object_array_get_idx(Jobj,1);
			enable = json_object_get_boolean(json_object_object_get(reqObject, "Enabled"));
			username = json_object_get_string(json_object_object_get(reqObject, "Username"));
			password = json_object_get_string(json_object_object_get(reqObject, "Password"));
#ifndef HAWAIIAN_TELCOM
			oldPassword = json_object_get_string(json_object_object_get(reqObject, "oldPassword"));
#endif
			if((json_object_object_get(reqObject, "EnableQuickStart"))){
				enablequickstart = json_object_get_boolean(json_object_object_get(reqObject, "EnableQuickStart"));
			}
			if(username && password && strlen(username) && strlen(password))
			{
#ifndef HAWAIIAN_TELCOM
				int authResult = -1;

				if(oldPassword == NULL || strlen(oldPassword)==0) {
					if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
					ret =  ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
				} else {
#ifdef SHA512_PASSWD
					authResult = zyUtilAuthUserAccount(username, oldPassword, password);
#else
					authResult = zyUtilAuthUserAccount(username, oldPassword);
#endif
					if (authResult == 1) {
						if(strcmp(Jgets(loginPrivilegeObj, "Password"), oldPassword)){
							if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
							ret =  ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
						}
					}
					else if (authResult < 0) {
						if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
						ret = ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
					}
				}
#ifdef PACKAGE_WEB_GUI_VUE
				if(ret != ZCFG_SUCCESS){
					json_object_object_add(reqObject, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.oldpassword_err2"));
					json_object_array_add(Jobj, reqObject);
					return ret;
				}
#endif
				if(ret == ZCFG_SUCCESS) {
					json_object_object_add(loginPrivilegeObj, "Enabled", json_object_new_boolean(enable));
					json_object_object_add(loginPrivilegeObj, "Username", json_object_new_string(username));
					json_object_object_add(loginPrivilegeObj, "Password", json_object_new_string(password));
					if((json_object_object_get(reqObject, "EnableQuickStart"))){
						json_object_object_add(loginPrivilegeObj, "EnableQuickStart", json_object_new_boolean(enablequickstart));
					}
					zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, loginPrivilegeObj, NULL);
				}
#else
				json_object_object_add(loginPrivilegeObj, "Enabled", json_object_new_boolean(enable));
				json_object_object_add(loginPrivilegeObj, "Username", json_object_new_string(username));
				json_object_object_add(loginPrivilegeObj, "Password", json_object_new_string(password));
				json_object_object_add(loginPrivilegeObj, "EnableQuickStart", json_object_new_boolean(enablequickstart));
				zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, loginPrivilegeObj, NULL);
#endif
			}
		}
	}
	else if(index0 == 2 || index0 == 3){
		loginPrivilegeIid.idx[0] = index0;
		loginPrivilegeIid.level = 1;
		reqObject = json_object_array_get_idx(Jobj,1);
		page = json_object_get_string(json_object_object_get(reqObject, "Page"));
		enablequickstart = json_object_get_boolean(json_object_object_get(reqObject, "EnableQuickStart"));
		ret = zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP, &loginPrivilegeIid, &loginPrivilegeObj);
		if(ret == ZCFG_SUCCESS){

			json_object_object_add(loginPrivilegeObj, "Page", json_object_new_string(page));
			json_object_object_add(loginPrivilegeObj, "HiddenPage", json_object_new_string(""));
			zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP, &loginPrivilegeIid, loginPrivilegeObj, NULL);
		}

		if(json_object_object_get(reqObject, "EnableQuickStart")){
			IID_INIT(loginPrivilegeIid);
			while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, &loginPrivilegeAccountObj)== ZCFG_SUCCESS){
				if(loginPrivilegeIid.idx[0] == index0){
					json_object_object_add(loginPrivilegeAccountObj, "EnableQuickStart", json_object_new_boolean(enablequickstart));
					zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, loginPrivilegeAccountObj, NULL);
				}
				zcfgFeJsonObjFree(loginPrivilegeAccountObj);
			}
		}
#ifdef HAWAIIAN_TELCOM
		//__ZyXEL__, JackYu, modify account also, for HT requires enabling account password modification at login previlige page
		loginPrivilegeIid.idx[0] = index0;
		loginPrivilegeIid.idx[1] = index1;
		loginPrivilegeIid.level = 2;
		ret = zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, &loginPrivilegeObj);
		if(ret == ZCFG_SUCCESS){
			reqObject = json_object_array_get_idx(Jobj,2);
			enable = json_object_get_boolean(json_object_object_get(reqObject, "Enabled"));
			username = json_object_get_string(json_object_object_get(reqObject, "Username"));
			password = json_object_get_string(json_object_object_get(reqObject, "Password"));
			enablequickstart = json_object_get_boolean(json_object_object_get(reqObject, "EnableQuickStart"));

			//validate
			if(username && password && strlen(username) && strlen(password))
			{
				json_object_object_add(loginPrivilegeObj, "Enabled", json_object_new_boolean(enable));
				json_object_object_add(loginPrivilegeObj, "Username", json_object_new_string(username));
				json_object_object_add(loginPrivilegeObj, "Password", json_object_new_string(password));
				json_object_object_add(loginPrivilegeObj, "EnableQuickStart", json_object_new_boolean(enablequickstart));
				zcfgFeObjJsonSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, loginPrivilegeObj, NULL);
			}
		}
#endif
	}
	
	zcfgFeJsonObjFree(reqObjectindex);
	zcfgFeJsonObjFree(reqObject);
	zcfgFeJsonObjFree(loginPrivilegeObj);
	return ret;

}

zcfgRet_t zcfgFeDal_LoginPrivilege_Get(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL;
	struct json_object *loginPrivilegeObj = NULL;
	objIndex_t loginPrivilegeIid = {0};
	IID_INIT(loginPrivilegeIid);

	// supervisor can get all privilege, other account just get its own data
	const char* loginAccount = json_object_get_string(json_object_object_get(Jobj, DAL_LOGIN_USERNAME));

	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &loginPrivilegeIid, &loginPrivilegeObj) == ZCFG_SUCCESS){

		if ( strcmp(loginAccount, "supervisor") && strcmp( loginAccount, json_object_get_string( json_object_object_get(loginPrivilegeObj, "Username") ) ) ) {
			continue;
		}

		paramJobj = json_object_new_object();
		json_object_object_add(paramJobj, "Index0", json_object_new_int(loginPrivilegeIid.idx[0]));
		json_object_object_add(paramJobj, "Index1", json_object_new_int(loginPrivilegeIid.idx[1]));
		json_object_object_add(paramJobj, "Enabled", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "Enabled")));
		json_object_object_add(paramJobj, "Username", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "Username")));
		/* json_object_object_add(paramJobj, "Password", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "Password"))); */
		json_object_object_add(paramJobj, "EnableQuickStart", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "EnableQuickStart")));
		json_object_array_add(Jarray, paramJobj);
		zcfgFeJsonObjFree(loginPrivilegeObj);
	}


	if (strcmp(loginAccount, "supervisor")) {
		printf("(%s:%d) GUI dal handler (login_privilege) page information is for supervisor only. Other account could not get page detail information.\n", __func__, __LINE__);
	}
	else {  // only supervisor can get page/HiddenPage information
		IID_INIT(loginPrivilegeIid);
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP, &loginPrivilegeIid, &loginPrivilegeObj) == ZCFG_SUCCESS){
			paramJobj = json_object_new_object();
			json_object_object_add(paramJobj, "Index0", json_object_new_int(loginPrivilegeIid.idx[0]));
			json_object_object_add(paramJobj, "Index1", json_object_new_int(loginPrivilegeIid.idx[1]));
			json_object_object_add(paramJobj, "Page", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "Page")));
			json_object_object_add(paramJobj, "HiddenPage", JSON_OBJ_COPY(json_object_object_get(loginPrivilegeObj, "HiddenPage")));
			json_object_array_add(Jarray, paramJobj);
			zcfgFeJsonObjFree(loginPrivilegeObj);
		}
	}
	return ret;
}

zcfgRet_t zcfgFeDalLoginPrivilege(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	if(!strcmp(method, "PUT")){
		ret = zcfgFeDal_LoginPrivilege_Edit(Jobj, NULL);
	}
	else if(!strcmp(method, "GET")){
		ret = zcfgFeDal_LoginPrivilege_Get(Jobj, Jarray, NULL);
	}
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

