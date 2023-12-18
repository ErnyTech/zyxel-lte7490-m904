#include <ctype.h>
#include <json/json.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_dal.h"

int highGpIdx = 1;
int mediumGpIdx = 2;
int lowGpIdx = 3;

dal_param_t USER_ACCOUNT_param[] =  //RDM_OID_ZY_LOG_CFG_GP_ACCOUNT
{
	{"Username",			dalType_string,  1, 	16,		NULL,	NULL,	dal_Add|dal_Edit|dal_Delete},
    {"FirstIndex",			dalType_int,	 1, 	3,		NULL,	NULL,	dalcmd_Forbid}, // group index 
    {"SecondIndex", 		dalType_int,	 1, 	4,		NULL,	NULL,	dalcmd_Forbid}, // account index
	{"Enabled", 			dalType_boolean, 0, 	0,		NULL,	NULL,	0}, //default true
	{"EnableQuickStart",	dalType_boolean, 0, 	0,		NULL,	NULL,	dalcmd_Forbid}, //default value based on schema.json
	{"oldPw",				dalType_string,  0, 	0,		NULL,	NULL,	dal_Add_ignore},
	{"Password",			dalType_string,  0, 	0,		NULL,	NULL,	dal_Add},
	{"verifyNewpassword",	dalType_string,  0, 	0,		NULL,	NULL,	dal_Add},
	{"AccountRetryTime",	dalType_int,	 0, 	5,		NULL,	NULL,	0},
	{"AccountIdleTime", 	dalType_int,	 60, 	3600,	NULL,	NULL,	0},
#ifdef WIND_ITALY_CUSTOMIZATION
	{"AccountLockTime", 	dalType_int,	 180,	5400,	NULL,	NULL,	0},
#else
	{"AccountLockTime", 	dalType_int,	 300, 	5400,	NULL,	NULL,	0},
#endif
	{"group",				dalType_string,  0, 	0,		NULL,	"Administrator|User", dal_Add|dal_Edit_ignore}, //depend on login acocunt, see paramPrevilegeChange(), defatul is "User"
	{"Index",				dalType_string,  0, 	0,		NULL,	NULL,	dal_Add_ignore|dal_Edit_ignore},
#ifdef ZYXEL_REMOTE_ACCESS_PRIVILEGE
	{"RemoteAccessPrivilege",dalType_string, 0, 	0,		NULL,	NULL,	dalcmd_Forbid},
#endif
	{NULL,					0,				 0, 	0,		NULL,	NULL,	0}
};

void zcfgFeDalShowUserAccount(struct json_object *Jarray){
	int i, len = 0;
	struct json_object *obj;
	int AccountRetryTime, AccountIdleTime, AccountLockTime;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	//printf("%-10s %-10s %-15s %-15s %-15s %-15s %-15s\n", "Index", "Active", "User Name", "Retry Times", "Idle Timeout", "Lock Period", "Group");
	printf("%-15s %-10s %-15s %-22s %-22s %-15s\n", "User Name", "Active", "Retry Times", "Idle Timeout(second)", "Lock Period(second)", "Group");
	len = json_object_array_length(Jarray);
	for(i=0;i<len;i++){
		obj = json_object_array_get_idx(Jarray, i);
		//printf("%-10s ",json_object_get_string(json_object_object_get(obj, "Index")));
		printf("%-15s ",Jgets(obj, "Username"));
		if(Jgetb(obj, "Enabled"))
			printf("%-10s ","Enable");
		else
			printf("%-10s ","Disable");

		AccountRetryTime= Jgeti(obj, "AccountRetryTime");
		AccountIdleTime= Jgeti(obj, "AccountIdleTime");
		AccountLockTime= Jgeti(obj, "AccountLockTime");
		printf("%-15d ", AccountRetryTime);
		printf("%-22d ", AccountIdleTime);
		printf("%-22d ", AccountLockTime);
		printf("%-15s ",json_object_get_string(json_object_object_get(obj, "group")));
#ifdef ZYXEL_REMOTE_ACCESS_PRIVILEGE
#ifdef TELIA_CUSTOMIZATION
		;
#else
		printf("%-15s ",Jgets(obj, "RemoteAccessPrivilege"));
#endif
#endif		
		printf("\n");
	}
	
}

#if 0 //use username as key, not use index
static zcfgRet_t index_to_iid(int index, objIndex_t &iid){
	struct json_object *obj = NULL;
	int idx = 0;

	IID_INIT(*iid);
	iid->level=2;
	if(!strcmp(loginLevel, "high") || !strcmp(loginLevel, "medium"))
		iid->idx[0]=mediumGpIdx;
	else
		iid->idx[0]=lowGpIdx;

	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, iid, &obj) == ZCFG_SUCCESS) {
		idx++;
		zcfgFeJsonObjFree(obj);
		if(idx == index){
			return ZCFG_SUCCESS;
		}
	}
	return ZCFG_NOT_FOUND;
}
#endif

#ifdef HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
void removeSpaces(char *str)
{
	char *i = str;
	char *j = str;
	
	while(*j != 0){
		*i = *j++;
		if(*i != ' ')
		i++;
	}
	*i = 0;
}
#endif
/*
 * For current design, high level account data not showen on GUI
 * high and medium level user get medium and low level account data
 * low level user get only low level account data
 *
 * Note: the index is not the same for different level login user
 * Index to iid maping mechanism need be the same for zcfgFeDal_User_Account_Get() and index_to_iid()
*/
zcfgRet_t zcfgFeDal_User_Account_Get(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	struct json_object *obj = NULL;
	struct json_object *pramJobj = NULL;
	char loginLevel[16] = {0};
	#ifdef HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
	char loginUserName[16] = {0};
	char userNameBuffer[16] = {0};
	char HiddenAccount[1024] = {0};
	#endif
	int index = 0;
	
	
#ifndef HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
	if(getLoginUserInfo(Jobj, NULL, loginLevel) != ZCFG_SUCCESS){
#else
	if(getLoginUserInfo(Jobj, loginUserName, loginLevel) != ZCFG_SUCCESS){
#endif
		if(replyMsg) strcpy(replyMsg, "Internal Error, can't get account information");
		dbg_printf("getLoginUserInfo Fail");
		return ZCFG_INTERNAL_ERROR;
	}
	
	if (json_object_object_get(Jobj, "loginLevel") != NULL){
		IID_INIT(iid);
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP, &iid, &obj) == ZCFG_SUCCESS) {
			if(!strcmp( json_object_get_string(json_object_object_get(obj, "Level")),loginLevel) ){
				pramJobj = json_object_new_object();
				replaceParam(pramJobj, "GP_Privilege", obj, "GP_Privilege");
				json_object_array_add(Jarray, pramJobj);
				zcfgFeJsonObjFree(obj);
				break;
			}
			zcfgFeJsonObjFree(obj);
		}
		
	}else{

		IID_INIT(iid);
		
		#ifdef HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &iid, &obj) == ZCFG_SUCCESS) {
			if(!strcmp(loginLevel, "high") || !strcmp(loginLevel, "medium")){
				if( highGpIdx == iid.idx[0]){
					zcfgFeJsonObjFree(obj);
					index++;
					continue;
				}
			}else{
				if( (highGpIdx == iid.idx[0]) || (mediumGpIdx == iid.idx[0])){
					zcfgFeJsonObjFree(obj);
					index++;
					continue;
				}
			}
			
			if( strcmp(loginUserName, (char *)json_object_get_string(json_object_object_get(obj, "Username"))) == 0 ){
				//printf("%s() %d: (json_object_get_string(json_object_object_get(obj, HiddenAccount))=%s\n", __FUNCTION__,__LINE__,(char *)json_object_get_string(json_object_object_get(obj, "HiddenAccount")));
				strcpy(HiddenAccount, (char *)json_object_get_string(json_object_object_get(obj, "HiddenAccount")));
				break;
			}
			
			zcfgFeJsonObjFree(obj);
			index++;
		}
		
		removeSpaces(HiddenAccount);
		//printf("%s() %d: HiddenAccount=%s\n", __FUNCTION__,__LINE__,HiddenAccount);
		//necessary reset value
		index = 0;
		IID_INIT(iid);
		#endif //HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
		while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &iid, &obj) == ZCFG_SUCCESS) {
			if(!strcmp(loginLevel, "high") || !strcmp(loginLevel, "medium")){
				if( highGpIdx == iid.idx[0]){
					zcfgFeJsonObjFree(obj);
					index++;
					continue;
				}
			}else{
				if( (highGpIdx == iid.idx[0]) || (mediumGpIdx == iid.idx[0])){
					zcfgFeJsonObjFree(obj);
					index++;
					continue;
				}
			}

			#ifdef HIDE_OTHER_ACCOUNT_INFO_FOR_INDIVIDUAL_USER_ACCOUNT
			strcpy(userNameBuffer, (char *)json_object_get_string(json_object_object_get(obj, "Username")));
			if( findInList(HiddenAccount, userNameBuffer, ",") ){
				//printf("%s() %d: in HiddenAccount list, ignore this object", __FUNCTION__,__LINE__);
				continue;
			}
			#endif
			
			pramJobj = json_object_new_object();
			json_object_object_add(pramJobj, "Index", json_object_new_int(index));
			replaceParam(pramJobj, "Enabled", obj, "Enabled");
			replaceParam(pramJobj, "Username", obj, "Username");
			replaceParam(pramJobj, "AccountRetryTime", obj, "AccountRetryTime");
			replaceParam(pramJobj, "AccountIdleTime", obj, "AccountIdleTime");
			replaceParam(pramJobj, "AccountLockTime", obj, "AccountLockTime");
	#ifdef COULD_CHANGE_ITS_OWN_PASSWORD
			replaceParam(pramJobj, "CouldChangeItsOwnPassword", obj, "CouldChangeItsOwnPassword");
	#endif			
	#ifdef ZYXEL_REMOTE_ACCESS_PRIVILEGE
			#ifdef TELIA_CUSTOMIZATION
			;
			#else
			replaceParam(pramJobj, "RemoteAccessPrivilege", obj, "RemoteAccessPrivilege");
			#endif
	#endif
			if(iid.idx[0]==highGpIdx){//should not happen for current design
				json_object_object_add(pramJobj, "group",json_object_new_string("Root"));
			}
			else if(iid.idx[0]==mediumGpIdx){
				json_object_object_add(pramJobj, "group",json_object_new_string("Administrator"));
			}
			else if(iid.idx[0]==lowGpIdx){
				json_object_object_add(pramJobj, "group",json_object_new_string("User"));
			}
			else{
				dbg_printf("%s : Error\n",__FUNCTION__);
			}
			
			if(json_object_object_get(Jobj, "Index") != NULL) {
				if(index == json_object_get_int(json_object_object_get(Jobj, "Index"))) {
					json_object_array_add(Jarray, pramJobj);
					zcfgFeJsonObjFree(obj);
					break;
				}
			}else {
				json_object_array_add(Jarray, JSON_OBJ_COPY(pramJobj));
			}
			zcfgFeJsonObjFree(obj);
			index++;
		}
	}
	return ret;
}

//POST function
zcfgRet_t zcfgFeDal_User_Account_Add(struct json_object *Jobj, char *replyMsg){ 
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *accountObj = NULL;
	objIndex_t accountIid ={0};
	const char *username = NULL;
#if	0
	const char *password = NULL;
	const char *oldPw = NULL;
	const char *verifyNewpassword = NULL;
	char *accountName = NULL;
	int accountRetryTime = 0;
	int accountIdleTime = 0;
	int accountLockTime = 0;
	bool enable = false;
	bool enableQuickStart = false;
	int groupnumber = 0;
#endif
	int FirstIndex = 0;
	char loginLevel[16] = {0};

	if(getLoginUserInfo(Jobj, NULL, loginLevel) != ZCFG_SUCCESS){
		if(replyMsg) strcpy(replyMsg, "Internal Error, can't get account information");
		dbg_printf("getLoginUserInfo Fail");
		return ZCFG_INTERNAL_ERROR;
	}

	if(strcmp(Jgets(Jobj, "Password"), Jgets(Jobj, "verifyNewpassword"))){
		if(replyMsg) strcpy(replyMsg, "Password and verifyNewpassword don't match");
		json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.notMatch"));
		return ZCFG_REQUEST_REJECT;
	}

	username = Jgets(Jobj, "Username");
	if(getSpecificObj(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, "Username", json_type_string, username, NULL, NULL)){
		if(replyMsg) strcpy(replyMsg, "Username is duplicated");
		json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.account_repeat"));
		return ZCFG_REQUEST_REJECT;
	}

	//add default value
	if(!ISSET("group")) Jadds(Jobj, "group", "User");
	if(!ISSET("Enabled")) Jaddb(Jobj, "Enabled", true);
	if(!ISSET("AccountRetryTime")) Jaddi(Jobj, "AccountRetryTime", 3);
	if(!ISSET("AccountIdleTime")) Jaddi(Jobj, "AccountIdleTime", 5*60);
	if(!ISSET("AccountLockTime")) Jaddi(Jobj, "AccountLockTime", 5*60);

	if(!strcmp("Administrator", Jgets(Jobj, "group"))){
		if(!strcmp(loginLevel, "low")){
			if(replyMsg) strcpy(replyMsg, "Request Deny, can't add Administrator level account");
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.addAdminErr"));
			return ZCFG_REQUEST_REJECT;
		}
		FirstIndex = 2;
#ifdef WIND_ITALY_CUSTOMIZATION
		if(!ISSET("EnableQuickStart")) Jaddb(Jobj, "EnableQuickStart", false);
#else
		if(!ISSET("EnableQuickStart")) Jaddb(Jobj, "EnableQuickStart", true);
#endif
	}
	else if(!strcmp("User",Jgets(Jobj, "group"))){
		FirstIndex = 3;
		if(!ISSET("EnableQuickStart")) Jaddb(Jobj, "EnableQuickStart", false);
	}
	else
		dbg_printf("ERROR!\n");

	IID_INIT(accountIid);
	accountIid.level = 1;
	accountIid.idx[0] = FirstIndex;
	ret = zcfgFeObjJsonAdd(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, NULL);
	
	if( ZCFG_SUCCESS == ret ){
		ret = zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, &accountObj);
	}

	if( ZCFG_SUCCESS == ret){
		replaceParam(accountObj, "Enabled", Jobj, "Enabled");
		replaceParam(accountObj, "EnableQuickStart", Jobj, "EnableQuickStart");
		replaceParam(accountObj, "Username", Jobj, "Username");
		replaceParam(accountObj, "Password", Jobj, "Password");
		replaceParam(accountObj, "AccountRetryTime", Jobj, "AccountRetryTime");
		replaceParam(accountObj, "AccountIdleTime", Jobj, "AccountIdleTime");
		replaceParam(accountObj, "AccountLockTime", Jobj, "AccountLockTime");
		//replaceParam(accountObj, "verifyNewpassword", Jobj, "verifyNewpassword");
		//replaceParam(accountObj, "groupnumber", Jobj, "groupnumber");
#ifdef ZYXEL_REMOTE_ACCESS_PRIVILEGE
#ifdef TELIA_CUSTOMIZATION
		;
#else
		replaceParam(accountObj, "RemoteAccessPrivilege", Jobj, "RemoteAccessPrivilege");
#endif
#endif
		
		ret = zcfgFeObjJsonBlockedSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, accountObj, NULL);
	}
	
	if( ZCFG_EXCEED_MAX_INS_NUM == ret){
		if(replyMsg) strcpy(replyMsg, "Error: Each group can not exceed 4 user accounts!");
		json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.exceed_account_max_err"));
	}
	
	zcfgFeJsonObjFree(accountObj);
	return ret;
}

//PUT function
zcfgRet_t zcfgFeDal_User_Account_Edit(struct json_object *Jobj, char *replyMsg){ 
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *accountObj = NULL;
	struct json_object *reqObj = NULL;
	int arrayIdx = 0;
	objIndex_t accountIid = {0};
	const char *username = NULL;
	const char *curPassword = NULL;
	bool enable = false;
	enum json_type jsonType;
	char loginLevel[16] = {0};
//SHA512_PASSWD
	int authResult = -1;
//SHA512_PASSWD
	if(getLoginUserInfo(Jobj, NULL, loginLevel) != ZCFG_SUCCESS){
		if(replyMsg) strcpy(replyMsg, "Internal Error, can't get account information");
		dbg_printf("getLoginUserInfo Fail");
		return ZCFG_INTERNAL_ERROR;
	}

	jsonType = json_object_get_type(Jobj);
	
	if(jsonType == json_type_object){
		
		username = Jgets(Jobj, "Username");
		if(!getSpecificObj(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, "Username", json_type_string, username, &accountIid, &accountObj)){
			if(replyMsg) strcpy(replyMsg, "Request reject");
			return ZCFG_REQUEST_REJECT;
		}
#ifdef ZYXEL_TELIA_FINLAND_TELIAROOT_PASSWORD //let CLI "dalcmd user_account" change teliaroot account password
		if( ((accountIid.idx[0] < mediumGpIdx) && strcmp(username, "teliaroot")) || (!strcmp(loginLevel, "low") && accountIid.idx[0] < lowGpIdx)){
#else
		if(accountIid.idx[0] < mediumGpIdx || (!strcmp(loginLevel, "low") && accountIid.idx[0] < lowGpIdx)){
#endif
			if(replyMsg) strcpy(replyMsg, "Request reject");
			zcfgFeJsonObjFree(accountObj);
			return ZCFG_REQUEST_REJECT;
		}

		if(ISSET("Password")){
			if(!strcmp("", Jgets(Jobj ,"Password"))){
				curPassword = Jgets(accountObj, "Password");
				if(strcmp(curPassword, Jgets(Jobj ,"oldPw"))!= 0){
					if(replyMsg) strcpy(replyMsg, "Wrong --oldPw value");
					json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.oldpassword_err2"));
						zcfgFeJsonObjFree(accountObj);
					return ZCFG_NOT_FOUND; //GUI use ZCFG_NOT_FOUND to detect oldPw error.
				}
				replaceParam(accountObj, "Password", Jobj, "oldPw");
			}
			else{
				if(!ISSET("oldPw") || !ISSET("verifyNewpassword")){
					if(replyMsg) strcpy(replyMsg, "Need --oldPw and --verifyNewpassword to modify Password");
					zcfgFeJsonObjFree(accountObj);
					return ZCFG_DELETE_REJECT;
				}

				if(strcmp(Jgets(Jobj, "Password"), Jgets(Jobj, "verifyNewpassword"))){
					if(replyMsg) strcpy(replyMsg, "--Password and --verifyNewpassword don't match");
					json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Common.notMatch"));
					zcfgFeJsonObjFree(accountObj);
					return ZCFG_REQUEST_REJECT;
				}
				
				authResult = zyUtilAuthUserAccount(username, Jgets(Jobj ,"oldPw"));
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
				replaceParam(accountObj, "Password", Jobj, "Password");
			}
		}
		
		replaceParam(accountObj, "Enabled", Jobj, "Enabled");
		replaceParam(accountObj, "AccountRetryTime", Jobj, "AccountRetryTime");
		replaceParam(accountObj, "AccountIdleTime", Jobj, "AccountIdleTime");
		replaceParam(accountObj, "AccountLockTime", Jobj, "AccountLockTime");
#ifdef ZYXEL_REMOTE_ACCESS_PRIVILEGE
#ifdef TELIA_CUSTOMIZATION
		;
#else
		replaceParam(accountObj, "RemoteAccessPrivilege", Jobj, "RemoteAccessPrivilege");
#endif
#endif
			
        ret = zcfgFeObjJsonBlockedSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, accountObj, NULL);
		zcfgFeJsonObjFree(accountObj);
	}

	//GUI only, enable/dissable mutil accounts
	if(jsonType == json_type_array){
		while((reqObj = json_object_array_get_idx(Jobj, arrayIdx)) !=NULL){
			arrayIdx++;
			username = Jgets(reqObj, "Username");
			if((Jget(reqObj, "Enabled")!=NULL) && getSpecificObj(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, "Username", json_type_string, username, &accountIid, &accountObj)){
				if(accountIid.idx[0] < mediumGpIdx || (!strcmp(loginLevel, "low") && accountIid.idx[0] < lowGpIdx)){
					zcfgFeJsonObjFree(accountObj);
					continue;
				}
				
				enable = Jgetb(accountObj, "Enabled");
				if(enable != Jgetb(reqObj, "Enabled")){
					replaceParam(accountObj, "Enabled", reqObj, "Enabled");
                    zcfgFeObjJsonBlockedSet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, accountObj, NULL);
				}
				zcfgFeJsonObjFree(accountObj);
			}
		}
	}
	
	return ret;
}

//DELETE function
zcfgRet_t zcfgFeDal_User_Account_Delete(struct json_object *Jobj, char *replyMsg){ 
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *accountObj = NULL;
	objIndex_t accountIid = {0};
	const char *username;
	char loginLevel[16] = {0};
	char loginUserName[32] = {0};
	int objIndex;

	if(getLoginUserInfo(Jobj, loginUserName, loginLevel) != ZCFG_SUCCESS){
		dbg_printf("getLoginUserInfo Fail");
		if(replyMsg) strcpy(replyMsg, "Internal Error, can't get login user information");
		return ZCFG_INTERNAL_ERROR;
	}

    if(Jget(Jobj, "Username") != NULL){ // from CLI
    	username = Jgets(Jobj, "Username");
    	if(!getSpecificObj(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, "Username", json_type_string, username, &accountIid, &accountObj)){
    		if(replyMsg) strcpy(replyMsg, "Request reject");
    		return ZCFG_REQUEST_REJECT;
    	}
    }
    else{ // from GUI
		objIndex = json_object_get_int(json_object_object_get(Jobj, "Index"));
		if(0 !=objIndex  ){
			ret = convertIndextoIid(objIndex, &accountIid, RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, NULL, NULL, replyMsg);
			if(ret != ZCFG_SUCCESS){
				dbg_printf("Wrong index..\n");
				return ret;
			}
			
		}else{
			IID_INIT(accountIid);
			accountIid.level = 2;
			accountIid.idx[0] = Jgeti(Jobj, "FirstIndex");
			accountIid.idx[1] = Jgeti(Jobj, "SecondIndex");
		}
        if(zcfgFeObjJsonGet(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, &accountObj) != ZCFG_SUCCESS){
            dbg_printf("get target user info fail\n");
            if(replyMsg) strcpy(replyMsg, "Internal Error, can't get target user information");
            return ZCFG_INTERNAL_ERROR;
        }
        username = Jgets(accountObj, "Username");
    }

    if(!strcmp(username, loginUserName)){
        if(replyMsg) strcpy(replyMsg, "Can't delete yourself");
        return ZCFG_REQUEST_REJECT;
    }

    if(accountIid.idx[0] < mediumGpIdx || (!strcmp(loginLevel, "low") && accountIid.idx[0] < lowGpIdx)){
        if(replyMsg) strcpy(replyMsg, "Request reject");
        zcfgFeJsonObjFree(accountObj);
        return ZCFG_REQUEST_REJECT;
    }
	
	ret = zcfgFeObjJsonDel(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, NULL);
	zcfgFeJsonObjFree(accountObj);
	return ret;
}

zcfgRet_t zcfgFeDalUserAccount(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;	
	char loginLevel[16] = {0};
	char loginUserName[32] = {0};

	if(!method || !Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(getLoginUserInfo(Jobj, loginUserName, loginLevel) != ZCFG_SUCCESS){
		printf("getLoginUserInfo Fail");
		return ZCFG_INTERNAL_ERROR;
	}

	if(!strcmp(method, "GET")){
		ret = zcfgFeDal_User_Account_Get(Jobj, Jarray, replyMsg);
	}else if(!strcmp(method, "PUT")) {
		ret = zcfgFeDal_User_Account_Edit(Jobj, replyMsg);
	}else if(!strcmp(method, "POST")) {
		ret = zcfgFeDal_User_Account_Add(Jobj, replyMsg);
	}else if(!strcmp(method, "DELETE")) {
		ret = zcfgFeDal_User_Account_Delete(Jobj, replyMsg);
	}else
		printf("Unknown method:%s\n", method);

	return ret;
}

