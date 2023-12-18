#include <ctype.h>
#include <json/json.h>
#include <time.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"

dal_param_t BACKUP_RESTORE_param[]={
{"FactoryResetUnpurgedList", dalType_string, 0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}

};

zcfgRet_t zcfgFeDalBackupRestoreEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t intfIid = {0};
	struct json_object *intfObj = NULL;
	const char *unpurgedListStr = NULL;
	uint32_t uFactoryResetUnpurged;

	IID_INIT(intfIid);
	unpurgedListStr = json_object_get_string(json_object_object_get(Jobj, "FactoryResetUnpurgedList"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_FEATURE_FLAG, &intfIid, &intfObj)) == ZCFG_SUCCESS)
	{
		char *token;
		const char sepChar[2] = ",";
		int keepItemID;
		
		uFactoryResetUnpurged = 0;
		token = strtok(unpurgedListStr, sepChar);
		
		while(token != NULL)
		{
			sscanf(token, "ResetKeep_%d", &keepItemID);
			uFactoryResetUnpurged = uFactoryResetUnpurged | 1 << (keepItemID - 1);
			token = strtok(NULL, sepChar);
		}

		json_object_object_add(intfObj, "FactoryResetUnpurgedList", json_object_new_int(uFactoryResetUnpurged));
		zcfgFeObjJsonSet(RDM_OID_FEATURE_FLAG, &intfIid, intfObj, NULL);
	}

	zcfgFeJsonObjFree(intfObj);

	return ret;
}	

zcfgRet_t zcfgFeDalBackupRestoreGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *paramJobj = NULL, *intfObj = NULL;
	objIndex_t intfIid = {0};
	uint32_t supportedUnpurgedList = 0;	//default value
	char supportedUnpurgedListStr[256];
	uint32_t unpurgedList = 0;	//default value
	char unpurgedListStr[256];
	int i;
	char szBuffer[16];
	bool bPreviousOne = false;
	
	paramJobj = json_object_new_object();
	IID_INIT(intfIid);
	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_FEATURE_FLAG, &intfIid, &intfObj)) == ZCFG_SUCCESS)
	{
		supportedUnpurgedList = json_object_get_int(json_object_object_get(intfObj, "SupportedFactoryResetUnpurgedList"));
		bPreviousOne = false;
		for (i = 0; i<32; ++i)
		{
			if (supportedUnpurgedList & (0x1 << i))
			{
				if (bPreviousOne == true)
				{
					strcat(supportedUnpurgedListStr, ",");
				}
				snprintf(szBuffer, sizeof(szBuffer), "ResetKeep_%d", i+1);
				strcat(supportedUnpurgedListStr, szBuffer);
				bPreviousOne = true;
			}
		}
		json_object_object_add(paramJobj, "SupportedFactoryResetUnpurgedList", json_object_new_string(supportedUnpurgedListStr));

		unpurgedList = json_object_get_int(json_object_object_get(intfObj, "FactoryResetUnpurgedList"));
		bPreviousOne = false;
		for (i = 0; i<32; ++i)
		{
			if (unpurgedList & (0x1 << i))
			{
				if (bPreviousOne == true)
				{
					strcat(unpurgedListStr, ",");
				}
				snprintf(szBuffer, sizeof(szBuffer), "ResetKeep_%d", i+1);
				strcat(unpurgedListStr, szBuffer);
				bPreviousOne = true;
			}
		}
		
		json_object_object_add(paramJobj, "FactoryResetUnpurgedList", json_object_new_string(unpurgedListStr));
		zcfgFeJsonObjFree(intfObj);
	}

	json_object_array_add(Jarray, paramJobj);
	return ret;
}

void zcfgFeDalShowBackupRestore(struct json_object *Jarray)
{
	struct json_object *obj = NULL;

	if(json_object_get_type(Jarray) != json_type_array){
		printf("wrong Jobj format!\n");
		return;
	}

	obj = json_object_array_get_idx(Jarray, 0);
}

zcfgRet_t zcfgFeDalBackupRestore(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg)
{
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "PUT"))
	{
		ret = zcfgFeDalBackupRestoreEdit(Jobj, NULL);
	}
	else if(!strcmp(method, "GET"))
	{
		ret = zcfgFeDalBackupRestoreGet(Jobj, Jarray, NULL);
	}
	else
	{
		printf("Unknown method:%s\n", method);
	}
	return ret;
}
