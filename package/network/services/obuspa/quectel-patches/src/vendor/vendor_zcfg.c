#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "usp_err_codes.h"
#include "vendor_defs.h"
#include "vendor_api.h"
#include "vendor_zcfg.h"
#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"

#include "json-c/json_object.h"
#include "zcfg_fe_schema.h"
#include "zcmd_schema.h"
#include "zcfg_msg.h"
#include "zcmd_tool.h"
#include "zlog_api.h"
#include "data_model.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_tr98.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_dal.h"


#define MAX_GROUP_SEP 2
#define MAX_ENTRIES_LIST 32
#define NUMENTRY "NumberOfEntries"
#define ROOT_PATH "Device."
#define NUM_ELEM(x) (sizeof((x)) / sizeof((x)[0]))
#define CM_ROOTPATH_TR181 "Device"
int vendor_uspd_init(void);
int vendor_uspd_stop(void);
int vendor_uspd_start(void);
int factory_reset(void);
int reboot(void);
static str_vector_t g_group_vec;
#define MY_GROUP 3
#define TR369FILE "/etc/zcfg_tr369_file"
int vendor_parse_tr181_and_register(void);
extern int CountPathSeparator(char *path);
extern asyncOperReqThread_t asyncOperReqThread[MAX_ASYNC_REQUEST_NUM];
// #define TR369_DEBUG 1
#define ATTR_READ        (1 << 1)
#define ATTR_WRITE       (1 << 2)
#define ATTR_UNIQUEKEY   (1 << 3)
uint32_t TR369_FEATURE_Flag = 1;
int checkCertificate = 0;
int checkCertificateCN = 0;

/*************************
* Function 	: cwmp_cm_parse_fullpath()
* Description	: parse the fullpath to object and parameter
* Output 	: 
* NOTE		: size of fullpath, objpath and name  is SIZE_NAME+1
*************************/
int cwmp_cm_parse_fullpath( char *fullpath, char *objpath, char *name, int *isPartial){
	char *ptr;
	int fullpathLen = 0;
	#ifdef TR369_DEBUG
	printf("%s() Enter fullpath=%s\n", __FUNCTION__, fullpath);
	#endif
	memset(objpath, 0, MAX_DM_PATH);
	memset(name, 0, MAX_DM_PATH);
	*isPartial = 0;
	fullpathLen = strlen(fullpath);
	
	/*BBF TR-069 An empty string indicates the top of the name hierarchy */
	if( fullpathLen == 0 ){
		snprintf(objpath, MAX_DM_PATH, "%s", CM_ROOTPATH_TR181);
		snprintf( fullpath, MAX_DM_PATH, "%s.", CM_ROOTPATH_TR181);

		*isPartial = 1;	
		return 0;
	}
	
	if(fullpathLen > MAX_DM_PATH){ /*shold not be happened*/	
		printf( "fullpath=%s length >%d\n", fullpath, MAX_DM_PATH);
		return -2;
	}
	/* Check parameter path is a partial path or not */	
	if ((fullpath[fullpathLen-1] == '.')){
		*isPartial = 1;
	}
	/* path & name*/
	if( *isPartial ){
		strncpy( objpath, fullpath, fullpathLen-1 );
	}
	else{
		ptr = fullpath + fullpathLen-1;
		
		while( *ptr != '\0' ){
			if(*ptr == '.'){
				strncpy( objpath, fullpath, ptr-fullpath );
				strncpy( name, ptr+1, MAX_DM_PATH-1  );
				break;
			}
			ptr--;
		}
		
	}
	#ifdef TR369_DEBUG
	printf("%s() objpath=%s,name=%s,*isPartial=%d\n", __FUNCTION__, objpath, name, *isPartial);
	#endif
	if( strlen(objpath)==0 ){
		return -1;
	}	
	
	return 0;
}

/*************************
* Function 	: cwmp_cm_is_indexobj()
* Description	: check the input fullpath is a index object or not
* Output 	: 
*************************/
int cwmp_cm_is_indexobj( char *fullpath ){
	/*EX: A.B.i */
	if( fullpath[strlen(fullpath)-1] == 'i' &&
		fullpath[strlen(fullpath)-2] == '.'){
		return 1;
	}
	return 0;
}

int GetMyParams(int group_id, kv_vector_t *params)
{
	int inst = 0;
	int oid = -1, oldOid = -1; // oid initial value cannot 0, 0 is effective value, oid 0 is RDM_OID_DEVICE.
	objIndex_t objIid, oldObjIid;
	struct json_object *value = NULL;
	char name[MAX_DM_PATH]={0};
	char buf[MAX_DM_PATH]={0};
	char objpath[MAX_DM_PATH] = {0};
	int len = 0;
	int ret = 0;
	char *paramStrVal;
	int isPartial = 0;
	#ifdef TR369_DEBUG
	printf("%s() Enter\n", __FUNCTION__);
	printf("%s() group_id=%d,params->vector[0].key=%s\n", __FUNCTION__, group_id, params->vector[0].key);
	#endif
	

	IID_INIT(oldObjIid);
	while( inst < params->num_entries ){
		if(cwmp_cm_parse_fullpath( params->vector[inst].key, objpath, name, &isPartial) != TR069_OK){
			printf("%s() cwmp_cm_parse_fullpath fail\n", __FUNCTION__);
			return -1;
		}
		#ifdef TR369_DEBUG
		printf( "%s() inst=%d\n", __FUNCTION__, inst);
		printf( "%s() objpath=%s,name=%s,isPartial=%d\n", __FUNCTION__, objpath, name, isPartial);				
		#endif
					
		IID_INIT(objIid);
		oid = zcfgFeObjNameToObjId( objpath, &objIid);

		if( (oid >= 0) && ( (oldOid != oid) || memcmp(&oldObjIid, &objIid, sizeof(objIndex_t)) ) )  {
			ret = zcfgFeObjJsonGet(oid, &objIid, &value);
			#ifdef TR369_DEBUG
			printf("%s() ret=%d\n", __FUNCTION__, ret);
			if(value)
				printf("%s() value=%s\n", __FUNCTION__, json_object_to_json_string(value));
			#endif
		}
		
		/*Get parameter value and type from json object*/
		if(value){
			paramStrVal = zcfgFeParamValGet(value, name);
			if(paramStrVal){
				snprintf(buf,sizeof(buf), "%s", paramStrVal);
				len = strlen(buf);
				#ifdef TR369_DEBUG
				printf("%s() buf=%s,len=%d\n", __FUNCTION__, buf, len);
				#endif
				USP_ARG_Replace(params, params->vector[inst].key, buf);//kv_vector_t *kvv, char *key, char *value
			}
		}	

		memcpy(&oldObjIid, &objIid, sizeof(objIndex_t));
		oldOid = oid;

		inst++;
	}
	
    // params->vector[].key contains parameters to get
    // Obtain the value of these parameters from the other executable then use USP_ARG_Replace()
    // or USP_ARG_ReplaceWithHint() to copy the obtained value back into params->vector[]
    // If some parameters could not be obtained, then just do not call USP_ARG_Replace()
    // Only return an error if none of the parameters could be obtained (Example: RPC call failure)
	return 0;
}

int SetMyParams(int group_id, kv_vector_t *params, unsigned *param_types, int *failure_index)
{
    // params->vector[].key contains parameters to set
    // params->vector[].value contains the associated values to set
    // param_types[] contains the associated type of each parameter (Example: DM_BOOL)
    // return an error if any of the parameters could not be set.
    // *failure_index may be used to return the index of the first parameter to fail (index in the params->vector[] and param_types[] arrays).
    // If this is not known, you should return an index of the value INVALID.
	int ret = USP_ERR_OK;
	#ifdef TR369_DEBUG
    printf("[%s]Enter\n", __FUNCTION__);
	#endif
	for(int i=0; i<params->num_entries; i++)
	{
		#ifdef TR369_DEBUG
		printf("[%s]%s = %s(%d)\n", __FUNCTION__, params->vector[i].key, params->vector[i].value, param_types[i]);
		#endif
		if((ret = zcfg_set_value(params->vector[i].key, params->vector[i].value)) != USP_ERR_OK)
		{
			printf("[%s]zcfg_set_value fail, %s!!!\n", __FUNCTION__, USP_ERR_UspErrToString(ret));
			*failure_index = i;
			break;
		}
	}
	return ret;
}

int AddMyObject(int group_id, char *path, int *instance)
{
    // return the instance number of the object created by the other executable in *instance
    // return USP_ERR_CREATION_FAILURE if the object could not be created

	int ret = USP_ERR_OK;
	#ifdef TR369_DEBUG
	printf("[%s]Enter\n", __FUNCTION__);
	#endif

	if((ret = zcfg_add_object(path, instance)) !=USP_ERR_OK)
	{
		printf("[%s]zcfg_add_object fail, %s!!!\n", __FUNCTION__, USP_ERR_UspErrToString(ret));
	}

	return ret;
}

int NotifyAddMyObject(int group_id, dm_req_t *req)
{

	int ret = USP_ERR_OK;

	#ifdef TR369_DEBUG
    printf("[%s]Enter\n", __FUNCTION__);
	#endif

	return ret;
}

int DelMyObject(int group_id, char *path)
{
    // return USP_ERR_OBJECT_NOT_DELETABLE if the object could not be deleted
	int ret = USP_ERR_OK;

	#ifdef TR369_DEBUG
	printf("[%s]Enter\n", __FUNCTION__);
    printf("[%s]req->pat = %s\n", __FUNCTION__, path);
	#endif

	if((ret = zcfg_del_object(path)) != USP_ERR_OK)
	{
		printf("[%s]zcfg_del_object fail, %s!!!\n", __FUNCTION__, USP_ERR_UspErrToString(ret));
	}

	return ret;
}

int NotifyDelMyObject(int group_id, dm_req_t *req)
{
	int ret = USP_ERR_OK;

	#ifdef TR369_DEBUG
    printf("[%s]Enter\n", __FUNCTION__);
	#endif

	return ret;
}

/*************************
* Function 	: cwmp_cm_get_names()
* Description	: get parameter names of one object
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
/*
	EX: objname = A.B.
		parameterlist = 	A.B.p1
						A.B.p2
						A.B.p3
	EX: objname = A.B.p2
		parameterlist = 	A.B.p2
*/
int cwmp_cm_get_names( char	*objname/*, aParameterInfoList_t *pInfoList*/){
	objIndex_t objIid;
	int oid = 0;
	struct json_object *value = NULL;
	struct json_object *nameList = NULL;
	int attrVal = 0;
	char objnamebuf[MAX_DM_PATH] = {0};
	char objpath[MAX_DM_PATH] = {0};
	char name[MAX_DM_PATH] = {0};
	int isPartial = 0;
	//aParameterInfoStruct_t	*pInfoStruct = NULL;
	int retzcfg = 0;
	int ret = 0;
	#ifdef TR369_DEBUG
	printf( "%s() Enter objname=%s\n", __FUNCTION__, objname);
	#endif
	if( cwmp_cm_parse_fullpath( objname, objpath, name, &isPartial) != TR069_OK){
		return -1;
	}
	//pInfoStruct = pInfoList->parameterInfoStruct;
	#ifdef TR369_DEBUG
	printf( "%s() objpath=%s,name=%s,isPartial=%d\n", __FUNCTION__, objpath, name, isPartial);
	#endif
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	if(oid < 0 ) {
		#ifdef TR369_DEBUG
		printf( "%s() oid < 0\n", __FUNCTION__);
		#endif
		/* check if index node, ignor json-obj A.B of A.B.i don't have oid */
		memset(objnamebuf, 0, sizeof(objnamebuf));
		snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objpath );
		#ifdef TR369_DEBUG
		printf( "%s() objnamebuf=%s\n", __FUNCTION__, objnamebuf);
		#endif
		oid = zcfgFeObjNameToObjId( objnamebuf, &objIid ); 
		if( oid >= 0){
			#ifdef TR369_DEBUG
			printf( "%s() oid >= 0, return OK\n", __FUNCTION__);
			#endif
			return 0;
		}
		printf( "zcfgFeObjNameToObjId %s fail\n", objpath);					
		return -3;
	}
	
	retzcfg = zcfgFeObjJsonGetWithoutUpdate(oid, &objIid, &value);
	#ifdef TR369_DEBUG
	printf( "%s() oid=%d,retzcfg=%d,ZCFG_SUCCESS=%d,ZCFG_EMPTY_OBJECT=%d\n", __FUNCTION__, oid, retzcfg, ZCFG_SUCCESS,ZCFG_EMPTY_OBJECT);
	#endif
	if (retzcfg == ZCFG_SUCCESS || retzcfg == ZCFG_EMPTY_OBJECT) {
		/* parameter attr */
		if( isPartial ){
			#ifdef TR369_DEBUG
			printf( "%s() isPartial==true\n", __FUNCTION__);
			#endif
			if( zcfgFeAllParamNameGet(oid, &nameList) != ZCFG_SUCCESS){
				printf( "%s() zcfgFeAllParamNameGet fail return -2\n", __FUNCTION__);
				ret = -2;
			}
			zcfgFeParamNameForEach(nameList, pName){	
				attrVal = zcfgFeParamAttrGetByName(oid, pName);

				memset( objnamebuf, 0, sizeof(objnamebuf));
				snprintf( objnamebuf, sizeof(objnamebuf), "%s.%s", objpath, pName);
				#ifdef TR369_DEBUG
				printf( "%s() 222objnamebuf=%s\n", __FUNCTION__, objnamebuf);
				#endif
				#if 0
				strncpy( pInfoStruct[pInfoList->arrayNum].name, objnamebuf, WRITESIZE(sizeof(pInfoStruct[pInfoList->arrayNum].name)));
				pInfoStruct[pInfoList->arrayNum].writable = (attrVal & PARAMETER_ATTR_WRITE ? 1:0);
				pInfoList->arrayNum++;
				#endif
			}
			#ifdef TR369_DEBUG
			printf( "<-zcfgFeParamNameForEach\n");
			#endif
		}
		else{
			#ifdef TR369_DEBUG
			printf( "%s() isPartial==false\n", __FUNCTION__);
			#endif
			if( zcfgFeAllParamNameGet(oid, &nameList) != ZCFG_SUCCESS){
				ret = -4;
			}
			attrVal =  zcfgFeParamAttrGetByName(oid, name);			

			if( attrVal < 0 /*ZCFG_NO_SUCH_PARAMETER*/) {
				printf( "%s not exist at object %s\n", name, objpath);
				ret = -3;
			}
			
			else{
				#if 0
				strncpy( pInfoStruct[pInfoList->arrayNum].name, objname, WRITESIZE(sizeof(pInfoStruct[pInfoList->arrayNum].name)));
				pInfoStruct[pInfoList->arrayNum].writable= (attrVal & PARAMETER_ATTR_WRITE ? 1:0);
				pInfoList->arrayNum++;
				#endif
			}
		}
	}
	else{
		/*EX: instance not exist*/
		printf( "get objpath=%s fail, retzcfg=%d\n", objpath, retzcfg);
		ret = -3;
	}

	/*Free the json object*/
	zcfgFeJsonObjFree(nameList);
	zcfgFeJsonObjFree(value);	
	return ret;
}

/*************************
* Function 	: cwmp_cm_get_names_obj()
* Description	: get parameter object names
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
/*
	EX: objname = A.B.
		parameterlist = 	A.B.
	EX: objname = A.B.3.
		parameterlist = 	A.B.3.
*/
int cwmp_cm_get_names_obj( char *objname/*, aParameterInfoList_t *pInfoList*/ ){
	objIndex_t objIid;
	int oid = 0;
	struct json_object *value = NULL;
	char objnamebuf[MAX_DM_PATH] = {0};
	char objpath[MAX_DM_PATH] = {0};
	char name[MAX_DM_PATH] = {0};
	int isPartial = 0;
	//aParameterInfoStruct_t	*pInfoStruct = NULL;
	int retzcfg = 0;
	int ret = 0;
	#ifdef TR369_DEBUG
	printf( "%s() Enter objname %s\n", __FUNCTION__, objname);
	#endif
	if( cwmp_cm_parse_fullpath( objname, objpath, name, &isPartial)!= 0){
		return -1;
	}
	//pInfoStruct = pInfoList->parameterInfoStruct;

	if( !isPartial ){
		return ret;
	}
	
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	#ifdef TR369_DEBUG
	printf( "%s() objpath %s,name %s\n", __FUNCTION__, objpath, name);
	#endif
	if(oid < 0 ) {
		#ifdef TR369_DEBUG
		printf( "%s() oid < 0\n", __FUNCTION__);
		#endif
		/* check if index node, ignor json-obj A.B of A.B.i don't have oid */
		memset(objnamebuf, 0, sizeof(objnamebuf));
		snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objpath );
		#ifdef TR369_DEBUG
		printf( "%s() objnamebuf %s\n", __FUNCTION__, objnamebuf);
		#endif
		oid = zcfgFeObjNameToObjId( objnamebuf, &objIid ); 
		if( oid >= 0){
			#if 0			
			printf( "%s() oid >= 0\n", __FUNCTION__);
			
			strncpy( pInfoStruct[pInfoList->arrayNum].name, objname, WRITESIZE(sizeof(pInfoStruct[pInfoList->arrayNum].name)));
			pInfoStruct[pInfoList->arrayNum].writable = 0;
			pInfoList->arrayNum++;
			#endif
			return 0;
		}
		printf( "%s zcfgFeObjNameToObjId %s fail\n", __FUNCTION__, objpath);					
		return -2;
	}


	/*Get parameter value and type from json object*/
	retzcfg = zcfgFeObjJsonGetWithoutUpdate(oid, &objIid, &value);
	#ifdef TR369_DEBUG
	printf( "%s() retzcfg %d\n", __FUNCTION__, retzcfg);
	#endif
	if (retzcfg == ZCFG_SUCCESS || retzcfg == ZCFG_EMPTY_OBJECT) {
		/* parameter attr */
		#if 0
		strncpy( pInfoStruct[pInfoList->arrayNum].name, objname, WRITESIZE(sizeof(pInfoStruct[pInfoList->arrayNum].name)));
		pInfoStruct[pInfoList->arrayNum].writable = (zcfgFeObjAttrGet(oid)&OBJECT_ATTR_CREATE ? 1:0);
		pInfoList->arrayNum++;
		#endif
	}
	else{
		/*EX: instance not exist*/
		printf( "%s get objpath=%s fail, retzcfg=%d\n", __FUNCTION__, objpath, retzcfg);
		ret = -2;
	}

	/*Free the json object*/
	zcfgFeJsonObjFree(value);
	return ret;
}
#define CM_ROOTPATH_TR181 "Device"
/*************************
* Function 	: cwmp_cm_get_recursive_names()
* Description	: recursive get paramter names list
* Output 	: 
* NOTE		: size of objname  is SIZE_NAME+1
*************************/
/*
	EX: objname = A.B.
	EX: objname = A.B.p2
	EX: objname = A.B.C.             (i)
	EX: objname = A.B.C.3.		
*/
int cwmp_cm_get_recursive_names(int group_id, char *path, int *expiry_period){
//( char *objname, int nextlevel/*, aParameterInfoList_t *pInfoList, int *maxNumOfBuf*/ ){
	objIndex_t objIid;
	objIndex_t parentObjIid;
	int oid = 0;
	struct json_object *value = NULL;
	zcfgSubObjNameList_t *head = NULL, *node = NULL;
	char objnamebuf[MAX_DM_PATH] = {0};
	char preObjpath[MAX_DM_PATH] = {0};
	char objpath[MAX_DM_PATH] = {0};
	char name[MAX_DM_PATH] = {0};
	int isPartial = 0, isIndex = 0;
	int retzcfg = 0;
	int ret = 0;
	int nextlevel = 0;//hard code test
	//aParameterInfoStruct_t *tmpInfoStruct = NULL;
	#ifdef TR369_DEBUG
	printf("%s() Enter path%s\n", __FUNCTION__, path);
	#endif
	
	#if 0
	/* pre- increase the buffer if lease than MAX_NUM_PAR */
	if( *maxNumOfBuf-pInfoList->arrayNum < MAX_NUM_PAR ){
		tmpInfoStruct = pInfoList->parameterInfoStruct;
		pInfoList->parameterInfoStruct = (aParameterInfoStruct_t*)cwmp_rpc_buf_increase( (char*)tmpInfoStruct, sizeof(aParameterInfoStruct_t), maxNumOfBuf);
		tmpInfoStruct = NULL;
		/*Resourec Exceeded*/
		if(pInfoList->parameterInfoStruct == NULL){
			TR069LOG( TR069_DEBUG_ERROR, "Resourec Exceeded!!!");
			return TR069_RESOURCES_EXCEEDED;
		}
	}
	#endif

	#if 0
	/*BBF TR-069: if ParameterPath were empty, with NextLevel equal true, 
	the response would list only "InternetGatewayDevice." (if the CPE is an Internet Gateway Device).
	*/
	if( strlen(path)==0 ){
		snprintf( path,SIZE_NAME+1, "%s.", CM_ROOTPATH_TR181);
		cwmp_cm_get_names_obj( path, pInfoList);
		return 0;
	}
	#endif
	if( cwmp_cm_parse_fullpath( path, objpath, name, &isPartial)!= 0){
		return -2;
	}
	#ifdef TR369_DEBUG
	printf("%s() path=%s,objpath=%s,name=%s,isPartial=%d\n", __FUNCTION__, path, objpath, name, isPartial);
	printf("%s() nextlevel=%d\n", __FUNCTION__, nextlevel);
	#endif
	/*BBF TR-069: If NextLevel is true and ParameterPath is a Parameter name rather than a Partial Path Name,
	the CPE MUST return a fault response with the Invalid Arguments fault code (9003).*/
	if( !isPartial && nextlevel )
		return -3;
	
	/* a parameter */
	if( !isPartial ){
		#ifdef TR369_DEBUG
		printf("%s() a parameter call cwmp_cm_get_names path=%s\n", __FUNCTION__, path);
		#endif
		ret = cwmp_cm_get_names( path/*, pInfoList*/ );
		if( ret != 0 ){
			printf( "%s cwmp_cm_get_names %s fail\n", __FUNCTION__, path);
		}		
		return ret;
	}
	
	/* the object */
	if( !nextlevel ){
		#ifdef TR369_DEBUG
		printf("%s() the object call cwmp_cm_get_names_obj path=%s\n", __FUNCTION__, path);
		#endif
		ret = cwmp_cm_get_names_obj( path/*, pInfoList*/);
		if( ret != 0 ){
			printf( "cwmp_cm_get_names_obj %s fail\n", path);
			return ret;
		}	
	}
	#ifdef TR369_DEBUG
	printf("%s() 222call cwmp_cm_get_names path=%s\n", __FUNCTION__, path);
	#endif
	ret = cwmp_cm_get_names( path/*, pInfoList*/ );
	if( ret != 0 ){
		printf( "cwmp_cm_get_names %s fail\n", path);
		return ret;
	}
	
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	#ifdef TR369_DEBUG
	printf("%s() after zcfgFeObjNameToObjId objpath=%s,oid=%d\n", __FUNCTION__, objpath, oid);
	#endif
	if(oid < 0 ) {
		#ifdef TR369_DEBUG
		printf("%s() oid < 0\n", __FUNCTION__);
		#endif
		/* check if index node, ignor json-obj A.B of A.B.i don't have oid */
		memset(objnamebuf, 0, sizeof(objnamebuf));
		snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objpath );
		#ifdef TR369_DEBUG
		printf("%s() 222 oid < 0,objnamebuf=%s\n", __FUNCTION__, objnamebuf);
		#endif
		oid = zcfgFeObjNameToObjId( objnamebuf, &objIid );
		#ifdef TR369_DEBUG
		printf("%s() after zcfgFeObjNameToObjId again \n", __FUNCTION__);
		#endif
		if( oid < 0){			
			printf("%s() oid < 0 again return TR069_INVALID_PARAMETER_NAME\n", __FUNCTION__);
			printf( "zcfgFeObjNameToObjId %s fail\n", objnamebuf);
			return -4;
		}
		isIndex = 1;
	}
	#ifdef TR369_DEBUG
	printf("%s() isIndex=%d\n", __FUNCTION__, isIndex);
	#endif
	memcpy(&parentObjIid, &objIid, sizeof(parentObjIid));
	/* sub objects - for index node */
	if( isIndex == 1 ){
		#ifdef TR369_DEBUG
		printf("%s() enter isIndex == TRUE oid=%d\n", __FUNCTION__, oid);
		#endif
		/* search each exist index */
		IID_INIT(objIid);
		while((retzcfg = zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, &parentObjIid, &objIid, &value)) == ZCFG_SUCCESS) {
			/*not an index node*/
			if( objIid.level == 0){
				zcfgFeJsonObjFree(value);
				printf("%s() objIid.level == 0 break\n", __FUNCTION__);
				break;
			}
			memset( objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.%d.", objpath, objIid.idx[objIid.level-1]);
			#ifdef TR369_DEBUG
			printf("%s() 333 objnamebuf=%s,nextlevel=%d\n", __FUNCTION__, objnamebuf, nextlevel);
			#endif
			USP_DM_RefreshInstance(objnamebuf);
			#ifdef TR369_DEBUG
			printf("%s() 333 after USP_DM_RefreshInstance\n", __FUNCTION__);
			#endif
			zcfgFeJsonObjFree(value);
			if( nextlevel ){
				#ifdef TR369_DEBUG
				printf("%s() nextlevel is true, call cwmp_cm_get_names_obj \n", __FUNCTION__);
				#endif
				cwmp_cm_get_names_obj( objnamebuf/*, pInfoList*/);
			}
			else{
				#ifdef TR369_DEBUG
				printf("%s() nextlevel is false, call cwmp_cm_get_recursive_names objnamebuf=%s\n", __FUNCTION__, objnamebuf);
				#endif
				if( 0 != cwmp_cm_get_recursive_names( 0, objnamebuf, expiry_period/*int group_id, char *path, int *expiry_period*/)){
					printf( "cwmp_cm_get_recursive_names %s index fail\n", objnamebuf);
					/*Resourec Exceeded*/
					#if 0
					if(pInfoList->parameterInfoStruct == NULL){
						ret = -5;
					}
					#endif
					break;
				}	
			}
		}
		return ret;
	}


	
	/* sub objects */
	head = zcfgFeSubObjNameListGet(oid);
	#ifdef TR369_DEBUG
	printf("%s() after zcfgFeSubObjNameListGet oid=%d\n", __FUNCTION__, oid);
	#endif
	node = head;
	while(node != NULL) {
		/* the sub objects */
		if( cwmp_cm_is_indexobj(node->objName) ){
			#ifdef TR369_DEBUG
			printf("%s() cwmp_cm_is_indexobj node->objName=%s\n", __FUNCTION__, node->objName);
			#endif
			memset(objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.%s", objpath, node->objName);
			if( cwmp_cm_parse_fullpath( objnamebuf, preObjpath, name, &isPartial)!= 0){
				zcfgFeSubObjListFree(head);
				return -4;
			}
			memset(objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.", preObjpath);
			ret = cwmp_cm_get_names_obj( objnamebuf/*, pInfoList*/ );
			if( ret != 0 ){ /*should not be happened*/			
				ret = 0;
				node = node->next;
				continue;
			}	
			if( !nextlevel ){
				/* search each exist index */
				memset(objnamebuf, 0, sizeof(objnamebuf));
				snprintf( objnamebuf, sizeof(objnamebuf), "%s.%s.", objpath, node->objName);
				oid = zcfgFeObjNameToObjId( objnamebuf, &objIid);
				if(oid < 0 ) {
					printf( "zcfgFeObjNameToObjId %s fail\n", objnamebuf);					
					node = node->next;
					continue;
				}

				IID_INIT(objIid);
				while((retzcfg = zcfgFeSubInObjJsonGetNextWithoutUpdate(oid, &parentObjIid, &objIid, &value)) == ZCFG_SUCCESS) {
					if( objIid.level == 0){
						zcfgFeJsonObjFree(value);
						break;
					}
					memset( objnamebuf, 0, sizeof(objnamebuf));
					snprintf( objnamebuf, sizeof(objnamebuf), "%s.%d.", preObjpath, objIid.idx[objIid.level-1]);
					USP_DM_RefreshInstance(objnamebuf);
					#ifdef TR369_DEBUG
					printf("%s() 555 after USP_DM_RefreshInstance\n", __FUNCTION__);
					#endif
					zcfgFeJsonObjFree(value);
					if( 0 != cwmp_cm_get_recursive_names( 0, objnamebuf, expiry_period/*, pInfoList, maxNumOfBuf*/)){
						printf( "cwmp_cm_get_recursive_names %s index fail\n", objnamebuf);
						/*Resourec Exceeded*/
						#if 0
						if(pInfoList->parameterInfoStruct == NULL){
							ret = -5;
						}
						#endif
						break;
					}
				}
			}
		}
		else{
			memset(objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.%s.", objpath, node->objName);
			#ifdef TR369_DEBUG
			printf("%s() 555 objnamebuf=%s call zcfgFeObjNameToObjId\n", __FUNCTION__, objnamebuf);
			#endif
			oid = zcfgFeObjNameToObjId( objnamebuf, &objIid);
			#ifdef TR369_DEBUG
			printf("%s() 555 oid=%d\n", __FUNCTION__, oid);
			#endif
			if(oid < 0 ) {
				printf("%s() 555 oid<0 node = node->next; continue\n", __FUNCTION__);
				printf(  "zcfgFeObjNameToObjId %s fail\n", objnamebuf);					
				node = node->next;
				continue;
			}

			if( !nextlevel ){
				#ifdef TR369_DEBUG
				printf("%s() 555 nextlevel==false,  call cwmp_cm_get_recursive_names\n", __FUNCTION__);
				#endif
				if( TR069_OK != cwmp_cm_get_recursive_names( 0, objnamebuf, expiry_period/*, pInfoList, maxNumOfBuf*/)){
					#ifdef TR369_DEBUG
					printf( "cwmp_cm_get_recursive_names %s fail\n", objnamebuf);
					#endif
					/*Resourec Exceeded*/
					#if 0
					if(pInfoList->parameterInfoStruct == NULL){
						ret = -5;
						break;
					}
					#endif
					node = node->next;
					continue;
				}			
				
			}
			else{
				cwmp_cm_get_names_obj( objnamebuf/*, pInfoList*/ );
			}
		}

		node = node->next;
	}
	
	/* free the sub-object name list*/
	zcfgFeSubObjListFree(head);

	return ret;
}


/*********************************************************************//**
**
** uspd_get_value
**
** Gets the value of req->path
** First lookup into the local USPD Database
** else call usp_get method for the requested path
**
** \param   req - pointer to structure identifying the path
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int uspd_get_value(dm_req_t *req, char *buf, int len)
{
	//printf("[%s:%d] Enter req->path=%s\n",__func__, __LINE__, req->path);
	#if 1
	int ret = 0;
	char *paramStrVal;
	objIndex_t objIid;
	objIndex_t parentObjIid;
	int oid = 0;
	struct json_object *value = NULL;
	char objnamebuf[MAX_DM_PATH] = {0};
	char objpath[MAX_DM_PATH]={0};
	char name[MAX_DM_PATH]={0};
	int  isPartial = 0;
	bool isIndex = 0;
	bool isIndexExit = 0;
	char objPathName[MAX_DM_PATH] = {0};
	
	snprintf(objPathName, sizeof(objPathName), "%s",  req->path);

	#ifdef TR369_DEBUG
	printf("[%s:%d] Enter req->path=%s\n",__func__, __LINE__, req->path);		
	#endif
	if( cwmp_cm_parse_fullpath( objPathName, objpath, name, &isPartial) != 0){
		printf( "invalid fullname :%s", objPathName );
		//continue;
	}
	#ifdef TR369_DEBUG	
	printf( "objpath :%s, name: %s,isPartial: %d\n", objpath, name, isPartial );
	#endif
	IID_INIT(objIid);
	oid = zcfgFeObjNameToObjId( objpath, &objIid);
	#ifdef TR369_DEBUG
	printf( "oid=%d\n", oid );
	#endif
	if(oid < 0 ) {
		if(isPartial){
			/* check if index node, ignor json-obj A.B of A.B.i don't have oid */
			memset(objnamebuf, 0, sizeof(objnamebuf));
			snprintf( objnamebuf, sizeof(objnamebuf), "%s.i", objpath );
			oid = zcfgFeObjNameToObjId( objnamebuf, &objIid ); 
			if( oid >= 0){
				isIndex = 1;
			}
			else{
				return -1;
			}
		}/*isPartial*/
		else{
			printf("%s() Jessi oid < 0 but is not partial\n", __FUNCTION__);
			//return -2;
		}
	}
	#ifdef TR369_DEBUG
	printf( "222oid=%d,isIndex=%d\n", oid, isIndex );
	#endif
	
	#endif//if 1
	
#if 1//test index object, mark this temporarily	
	if(oid >= 0 ) {
		/*Get parameter value and type from json object*/
		ret = zcfgFeObjJsonGet(oid, &objIid, &value);
		#ifdef TR369_DEBUG
		printf("%s() Jessi ret=%d\n", __FUNCTION__, ret);
		#endif
		paramStrVal = zcfgFeParamValGet(value, name);
		if(paramStrVal){
			sprintf(buf,"%s", paramStrVal);
			len = strlen(buf);
		}
		else {
			return -1;
		}
	}
#endif
#if 0
	memcpy(&parentObjIid, &objIid, sizeof(parentObjIid));
	/* search each exist index */
	if(isIndex){
		IID_INIT(objIid);
		isIndexExit = 0;
		if(oid != RDM_OID_IFACE_STACK){
			while((retzcfg =zcfgFeSubInObjJsonGetNext(oid, &parentObjIid, &objIid, &value)) == ZCFG_SUCCESS) {
				/*not an index node*/
				if( objIid.level == 0){
					zcfgFeJsonObjFree(value);
					break;
				}
				isIndexExit = 1;
				memset( objnamebuf, 0, sizeof(objnamebuf));
				snprintf( objnamebuf, sizeof(objnamebuf), "%s.%d.", objpath, objIid.idx[objIid.level-1]);
				printf("%s() Jessi isIndex==1,objnamebuf=%s\n", __FUNCTION__, objnamebuf);
				/*Free the json object*/
				zcfgFeJsonObjFree(value);
				if(cwmp_cm_get_values( objnamebuf, pList, maxNumOfBuf )!= TR069_OK){
					break;
				}
			}
		}
		else{
			while((retzcfg =zcfgFeObjJsonGetNext(oid, &objIid, &value)) == ZCFG_SUCCESS) {
				/*not an index node*/
				if( objIid.level == 0){
					zcfgFeJsonObjFree(value);
					break;
				}
				isIndexExit = 1;
				memset( objnamebuf, 0, sizeof(objnamebuf));
				snprintf( objnamebuf, sizeof(objnamebuf), "%s.%d.", objpath, objIid.idx[objIid.level-1]);
				/*Free the json object*/
				zcfgFeJsonObjFree(value);
				if(cwmp_cm_get_values( objnamebuf, pList, maxNumOfBuf )!= TR069_OK){
					break;
				}
			}
		}
		if(isIndexExit) return 0;
		else return -2; /*no index object exist*/
	}
#endif
#if 0
	char json_buff[MAX_DM_VALUE_LEN] = {'\0'};

	if(buf==NULL) {
		USP_LOG_Error("[%s:%d] value buffer is null",__func__, __LINE__);
		return USP_ERR_INTERNAL_ERROR;
	}

	/* First lookup into local uspd_database */
	if (false == json_get_param_value(req->path, buf)) {
		USP_LOG_Debug("Not found in local database:|%s|", req->path);
		if (USP_ERR_OK == uspd_get(req->path, json_buff))
			json_get_value_index(json_buff, NULL, buf, 0);
	}

	len = strlen(buf);
#endif	
	return USP_ERR_OK;
}

/*********************************************************************//**
**
** uspd_set_value
**
** Gets the value of req->path
**
** \param   req - pointer to structure identifying the path
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int uspd_set_value(dm_req_t *req, char *buf)
{
	//uspd_set(req->path, buf);
	return USP_ERR_OK;
}

int uspd_add(dm_req_t *req)
{
#if 0
	uint32_t id;
	struct ubus_context *ctx = ubus_connect(NULL);
	struct blob_buf b = { };

	if (!ctx) {
		USP_LOG_Error("[%s:%d] ubus_connect failed",__func__, __LINE__);
		return USP_ERR_INTERNAL_ERROR;
	}

	if (!req) {
		USP_LOG_Error("[%s:%d] req is null",__func__, __LINE__);
		ubus_free(ctx);
		return USP_ERR_INTERNAL_ERROR;
	}

	if (ubus_lookup_id(ctx, USP_UBUS, &id)) {
		USP_LOG_Error("[%s:%d] %s not present",__func__, __LINE__, USP_UBUS);
		ubus_free(ctx);
		return USP_ERR_INTERNAL_ERROR;
	}

	memset(&b, 0, sizeof(struct blob_buf));
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "path", req->path);
	if (ubus_invoke(ctx, id, "add_object", b.head, receive_data_print, NULL, USPD_TIMEOUT)) {
		USP_LOG_Error("[%s:%d] ubus call failed for |%s|",__func__, __LINE__, req->path);
		blob_buf_free(&b);
		ubus_free(ctx);
		return USP_ERR_INTERNAL_ERROR;
	}
	blob_buf_free(&b);
	ubus_free(ctx);
#endif	
	return USP_ERR_OK;
}


int uspd_add_notify(dm_req_t *req)
{
	int err = USP_ERR_OK;
#if 0
	char path[MAX_DM_PATH];

	USP_SNPRINTF(path, sizeof(path), "%s.Alias", req->path);
	err = add_object_aliase(path);
#endif
	return err;
}

int uspd_del(dm_req_t *req)
{
#if 0
	uint32_t id;
	struct ubus_context *ctx = ubus_connect(NULL);
	struct blob_buf b = { };

	if (!ctx) {
		USP_LOG_Error("[%s:%d] ubus_connect failed",__func__, __LINE__);
		return USP_ERR_INTERNAL_ERROR;
	}

	if (!req) {
		USP_LOG_Error("[%s:%d] req is null",__func__, __LINE__);
		ubus_free(ctx);
		return USP_ERR_INTERNAL_ERROR;
	}

	if (ubus_lookup_id(ctx, USP_UBUS, &id)) {
		USP_LOG_Error("[%s:%d] %s not present",__func__, __LINE__, USP_UBUS);
		ubus_free(ctx);
		return USP_ERR_INTERNAL_ERROR;
	}

	memset(&b, 0, sizeof(struct blob_buf));
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "path", req->path);
	if (ubus_invoke(ctx, id, "del_object", b.head, receive_data_print, NULL, USPD_TIMEOUT)) {
		USP_LOG_Error("[%s:%d] ubus call failed for |%s|",__func__, __LINE__, req->path);
		ubus_free(ctx);
		blob_buf_free(&b);
		return USP_ERR_INTERNAL_ERROR;
	}
	blob_buf_free(&b);
	ubus_free(ctx);
#endif	
	return USP_ERR_OK;
}

static void *cwmp_cm_msg_proc(void *arg)
{
	printf("%s() Enter\n", __FUNCTION__);
        return NULL;
}

int factory_reset(void)
{
	zcfgRet_t retzcfg = 0;
	printf("%s() Enter\n", __FUNCTION__);
	retzcfg = zcfgFeRestoreDefault( NULL );
	if( retzcfg != ZCFG_SUCCESS ){
		return USP_ERR_COMMAND_FAILURE;
	}
	return 0;
}

int reboot(void)
{
	zcfgRet_t retzcfg = 0;
	printf("%s() Enter\n", __FUNCTION__);
	zcfgFeReqReboot(NULL);
	return 0;
}

int vendor_register_cellular(void)
{
	zcfgRet_t retzcfg = 0;
	printf("%s() Enter\n", __FUNCTION__);
	
	return 0;
}

int vendor_get_active_software_version(char *buf, int len)
{
	zcfgRet_t retzcfg = 0;
	printf("%s() Enter\n", __FUNCTION__);
	retzcfg = zcfg_get_value("Device.DeviceInfo.SoftwareVersion", buf, &len);
	if( retzcfg != 0 ){
		return USP_ERR_COMMAND_FAILURE;
	}
	return 0;
}

static int vendor_hook_init(void)
{
	vendor_hook_cb_t callbacks;
	printf("%s() Enter\n", __FUNCTION__);
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.reboot_cb = reboot;
	callbacks.factory_reset_cb = factory_reset;
	//callbacks.register_controller_trust_cb = vendor_register_roles;
	callbacks.cellular_cb = vendor_register_cellular;
#ifndef REMOVE_DEVICE_INFO
	callbacks.get_active_software_version_cb = vendor_get_active_software_version;
#endif
	return USP_REGISTER_CoreVendorHooks(&callbacks);
}

int vendor_uspd_init(void)
{
	zcfgRet_t ret;
	int err = USP_ERR_OK;
	rdm_LocalAgent_t *localAgentObj = NULL;
	objIndex_t objIid;
	
	printf("init_zcfg\n");
	
	/*init message server*/
	zcfgEidInit(ZCFG_EID_OBUSPA, "OBUSPA");
	ret = zcfgMsgServerInit();
	
	STR_VECTOR_Init(&g_group_vec);
	
	IID_INIT(objIid);
	if((zcfgFeObjStructGet(RDM_OID_LOCAL_AGENT, &objIid, (void **)&localAgentObj)) != ZCFG_SUCCESS) {
		USP_LOG_Error("Device.LocalAgent. empty!");
	} else {
		TR369_FEATURE_Flag = localAgentObj->X_ZYXEL_Feature;// set feature flag
#if 0
		checkCertificate = localAgentObj->X_ZYXEL_CheckCertificate;
		checkCertificateCN = localAgentObj->X_ZYXEL_CheckCertificateCN;
#endif
		zcfgFeObjStructFree(localAgentObj);
	}
	
#if 1
	vendor_parse_tr181_and_register();
#endif
	
	if(ret != ZCFG_SUCCESS) {
		/* zcfgLog(ZCFG_LOG_ERR, "obuspa message server init fail\n"); */
		printf("obuspa message server init fail\n");
	} else {
		ret = vendor_hook_init();
		if(ret != ZCFG_SUCCESS) {
			vendor_uspd_stop();
		}
	}
	

	//return err;
	
	return ret;
}

int vendor_uspd_stop(void)
{
	return USP_ERR_OK;
}

int vendor_uspd_start(void)
{
	int fault = USP_ERR_OK;
	int err = 0;
	printf("%s() Enter\n", __FUNCTION__);
	
	dm_vendor_cellular_cb_t load_cellular_cb;
	
	load_cellular_cb = vendor_hook_callbacks.cellular_cb;
    if (load_cellular_cb == NULL)
    {
        //load_agent_cert_cb = LoadClientCert;  // Fallback to a function which calls the get_agent_cert vendor hook
		printf("%s() load_cellular_cb is NULL\n", __FUNCTION__);
    }
    else
    {
        printf("%s: load_cellular_cb vendor hook", __FUNCTION__);
    }
	err = load_cellular_cb();
    if (err != 0)
    {
        printf("%s: load_cellular_cb() failed", __FUNCTION__);
        return -1;
    }
	
	fault = OS_UTILS_CreateThread(cwmp_cm_msg_proc, NULL);

	return fault;
}


//TEMP for test
void devicetest(void* param){
	int *i = (int*)param;
	printf("[Marcus debug] %s(%d) instance is %d In\n", __func__, __LINE__, *i);
	printf("[Marcus debug] %s(%d) tid %lu\n", __func__, __LINE__, (unsigned long)(pthread_self()));
	asyncOperReqThread[*i].threadID = pthread_self();
	printf("[Marcus debug] %s(%d) tid %lu\n", __func__, __LINE__, (unsigned long)(asyncOperReqThread[*i].threadID));
	sleep(300);
	printf("[Marcus debug] %s(%d) Out\n", __func__, __LINE__);
	USP_FREE(i);
}
int async_operate_test_handler(dm_req_t *req, kv_vector_t *input_args, int instance){
	printf("[Marcus debug] %s(%d) instance is %d In\n", __func__, __LINE__, instance);
	int *inst = USP_MALLOC(sizeof(int));
	
	*inst = instance;
	OS_UTILS_CreateThread(devicetest, inst);


	return 0;
}
//TEMP for test
int sync_operate_handler(dm_req_t *req, char *command_key, kv_vector_t *input_args, kv_vector_t *output_args){
	int err = USP_ERR_OK, i;
	oper_input_cond_t *cond;
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	cond = USP_MALLOC(sizeof(oper_input_cond_t));
	memset(cond, 0, sizeof(oper_input_cond_t));
	strcpy(cond->fullPath, req->path);

	cond->input_args = (kv_vector_t *) USP_MALLOC(sizeof(kv_vector_t));
	KV_VECTOR_Init(cond->input_args);

	if(input_args->num_entries) {
		for(i=0; i<input_args->num_entries; ++i) {
			kv_pair_t *kv = &input_args->vector[i];
			//USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			printf("[%s:%d] INPUT key:%s value:%s\n",__func__, __LINE__, kv->key, kv->value);
			KV_VECTOR_Add(cond->input_args, kv->key, kv->value);
		}
	}

	// Log the input conditions for the operation
	USP_LOG_Info("=== Conditions ===");
	USP_LOG_Info("req path: %s\n", cond->fullPath);
	cond->action = TEXT_UTILS_SplitPath(cond->fullPath, cond->parentPath, MAX_DM_PATH);
	USP_LOG_Info("path: %s\n", cond->parentPath);
	USP_LOG_Info("action: %s\n", cond->action);

	if(strcmp("Cancel()", cond->action) == 0){
		//cancel progress
		int reqInstance;
		sscanf(cond->parentPath, "Device.LocalAgent.Request.%d.", &reqInstance);
		if(localagentReqCancel(reqInstance) != 0){
			printf("%s(%d) Error with localagentReqCancel\n", __func__, __LINE__);
			err = USP_ERR_COMMAND_FAILURE;
			goto release_and_exit;
		}
	}
	else if(strcmp("AddCertificate()", cond->action) == 0) {//Device.LocalAgent.AddCertificate()
		if (localAgentAddCertificate(cond) != USP_ERR_OK) {
			printf("%s(%d) Error with localAgentAddCertificate\n", __func__, __LINE__);
			err = USP_ERR_COMMAND_FAILURE;
			goto release_and_exit;
		}
	}
	else if(strcmp("Delete()", cond->action) == 0) {//Device.LocalAgent.Certificate.{i}.Delete()
		if (localAgentCertificateDelete(cond) != USP_ERR_OK) {
			printf("%s(%d) Error with localAgentCertificateDelete\n", __func__, __LINE__);
			err = USP_ERR_COMMAND_FAILURE;
			goto release_and_exit;
		}
	}
	else if(strcmp("AddMyCertificate()", cond->action) == 0) {//Device.LocalAgent.Controller.{i}.AddMyCertificate()
		if (localAgentAddMyCertificate(cond) != USP_ERR_OK) {
			printf("%s(%d) Error with localAgentAddMyCertificate\n", __func__, __LINE__);
			err = USP_ERR_COMMAND_FAILURE;
			goto release_and_exit;
		}
	}
	else if(strcmp("SendOnBoardRequest()", cond->action) == 0) {//Device.LocalAgent.Controller.{i}.SendOnBoardRequest()
		if (SendOnBoardRequest(req, command_key, input_args, output_args) != USP_ERR_OK) {
			printf("%s(%d) Error with localAgentControllerSendOnBoardRequest\n", __func__, __LINE__);
			err = USP_ERR_COMMAND_FAILURE;
			goto release_and_exit;
		}
	}
	
release_and_exit:
	if(cond != NULL){
		USP_FREE(cond);
		cond = NULL;
	}

	return err;
}

int async_operate_handler(dm_req_t *req, kv_vector_t *input_args, int instance){

	int err = USP_ERR_OK, i;
	char value[256] = {0};
	char path[256] = {0};

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	asyncOperReqThread[instance].cond = USP_MALLOC(sizeof(oper_input_cond_t));
	memset(asyncOperReqThread[instance].cond, 0, sizeof(oper_input_cond_t));
	asyncOperReqThread[instance].cond->request_instance = instance;
	strcpy((asyncOperReqThread[instance].cond)->fullPath, req->path);


	(asyncOperReqThread[instance].cond)->input_args = (kv_vector_t *) USP_MALLOC(sizeof(kv_vector_t));
	KV_VECTOR_Init((asyncOperReqThread[instance].cond)->input_args);


	if(input_args->num_entries) {
		for(i=0; i<input_args->num_entries; ++i) {
			kv_pair_t *kv = &input_args->vector[i];
			//USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			printf("[%s:%d] INPUT key:%s value:%s\n",__func__, __LINE__, kv->key, kv->value);
			KV_VECTOR_Add((asyncOperReqThread[instance].cond)->input_args, kv->key, kv->value);
		}
	}

	// Log the input conditions for the operation
	USP_LOG_Info("=== Conditions ===");
	USP_LOG_Info("instance_number: %d", (asyncOperReqThread[instance].cond)->request_instance);
	USP_LOG_Info("req path: %s\n", (asyncOperReqThread[instance].cond)->fullPath);
	(asyncOperReqThread[instance].cond)->action = TEXT_UTILS_SplitPath((asyncOperReqThread[instance].cond)->fullPath, (asyncOperReqThread[instance].cond)->parentPath, MAX_DM_PATH);
	USP_LOG_Info("path: %s\n", (asyncOperReqThread[instance].cond)->parentPath);
	USP_LOG_Info("action: %s\n", (asyncOperReqThread[instance].cond)->action);

	//Set request info.
	snprintf(path, sizeof(path), "Device.LocalAgent.Request.%d.Command", instance);
	memset(value, 0, sizeof(value));
	DATA_MODEL_GetParameterValue(path, value, sizeof(value), 0);
	strcpy((asyncOperReqThread[instance].cond)->command, value);
	USP_LOG_Info("command: %s\n", (asyncOperReqThread[instance].cond)->command);

	snprintf(path, sizeof(path), "Device.LocalAgent.Request.%d.CommandKey", instance);
	memset(value, 0, sizeof(value));
	DATA_MODEL_GetParameterValue(path, value, sizeof(value), 0);
	strcpy((asyncOperReqThread[instance].cond)->commandKey, value);
	USP_LOG_Info("commandKey: %s\n", (asyncOperReqThread[instance].cond)->commandKey);

	snprintf(path, sizeof(path), "Device.LocalAgent.Request.%d.Originator", instance);
	memset(value, 0, sizeof(value));
	DATA_MODEL_GetParameterValue(path, value, sizeof(value), 0);
	strcpy((asyncOperReqThread[instance].cond)->originator, value);
	USP_LOG_Info("Originator: %s\n", (asyncOperReqThread[instance].cond)->originator);



	if(strcmp("Download()", (asyncOperReqThread[instance].cond)->action) == 0){
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_Download_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	} else 	if(strcmp("IPPing()", (asyncOperReqThread[instance].cond)->action) == 0){
		((asyncOperReqThread[instance]).auxiliary) = malloc(MAX_DM_PATH);
		if((asyncOperReqThread[instance]).auxiliary){
			memset((asyncOperReqThread[instance]).auxiliary, 0, MAX_DM_PATH);
			char symLink[MAX_DM_PATH];
			snprintf(symLink, sizeof(symLink), "%s%d/ping", PATH_REQUEST, (asyncOperReqThread[instance].cond)->request_instance);
			USP_LOG_Info("%s() symLink=%s\n", __FUNCTION__, symLink);
			memcpy((asyncOperReqThread[instance]).auxiliary, symLink, MAX_DM_PATH);
		}
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_IPPing_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	} else 	if(strcmp("TraceRoute()", (asyncOperReqThread[instance].cond)->action) == 0){
		((asyncOperReqThread[instance]).auxiliary) = malloc(MAX_DM_PATH);
		if((asyncOperReqThread[instance]).auxiliary){
			memset((asyncOperReqThread[instance]).auxiliary, 0, MAX_DM_PATH);
			char symLink[MAX_DM_PATH];
			snprintf(symLink, sizeof(symLink), "%s%d/traceroute", PATH_REQUEST, (asyncOperReqThread[instance].cond)->request_instance);
			USP_LOG_Info("%s() symLink=%s\n", __FUNCTION__, symLink);
			memcpy((asyncOperReqThread[instance]).auxiliary, symLink, MAX_DM_PATH);
		}
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_TraceRoute_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	} else 	if(strcmp("DownloadDiagnostics()", (asyncOperReqThread[instance].cond)->action) == 0){
		((asyncOperReqThread[instance]).auxiliary) = malloc(MAX_DM_PATH);
		if((asyncOperReqThread[instance]).auxiliary){
			memset((asyncOperReqThread[instance]).auxiliary, 0, MAX_DM_PATH);
			char symLink[MAX_DM_PATH];
			snprintf(symLink, sizeof(symLink), "%s%d/wget", PATH_REQUEST, (asyncOperReqThread[instance].cond)->request_instance);
			USP_LOG_Info("%s() symLink=%s\n", __FUNCTION__, symLink);
			memcpy((asyncOperReqThread[instance]).auxiliary, symLink, MAX_DM_PATH);
		}
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_DownloadDiagnostics_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	} else 	if(strcmp("Restore()", (asyncOperReqThread[instance].cond)->action) == 0){
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_Config_Restore_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	} else 	if(strcmp("Backup()", (asyncOperReqThread[instance].cond)->action) == 0){
		// Exit if unable to start a thread to perform this operation
		// NOTE: ownership of input conditions passes to the thread
		err = OS_UTILS_CreateThread(AsyncOperation_Config_Backup_Thread, &asyncOperReqThread[instance]);
		if (err != USP_ERR_OK)
		{
			USP_LOG_Info("%s(%d) Error return with command %s.\n", __func__, __LINE__, (asyncOperReqThread[instance].cond)->action);
			err = USP_ERR_COMMAND_FAILURE;
			goto exit;
		}
	}

exit:
	if (err != USP_ERR_OK)
	{
		if((asyncOperReqThread[instance].cond) != NULL){
			USP_FREE(asyncOperReqThread[instance].cond);
			asyncOperReqThread[instance].cond = NULL;
		}
		return USP_ERR_COMMAND_FAILURE;
	}

	return USP_ERR_OK;
}

//handle NumberOfEntries begin
static char num_entries_list[MAX_ENTRIES_LIST][MAX_DM_PATH] = {0};
/**
 * @brief Set NumberOfEntries param to candidate list.
 * 
 * @param path path is paramNumberOfEntries (e.g. Device.Cellular.InterfaceNumberOfEntries)
 * @return true Successful, false Fail.
 */
bool handle_num_entries_insert_list(char *path)
{
	char *p;
	int len;
	int empty_list_num;

	// check if the dm entry is a NumberOfEntries parameter
	len = strlen(path) - strlen(NUMENTRY);
	if (len < 0)
	{
		return false;
	}	

	p = path + len;
	if (strcmp(p, NUMENTRY) == 0) {
		for(empty_list_num = 0; empty_list_num < MAX_ENTRIES_LIST; empty_list_num++)
		{
			if(!strcmp(num_entries_list[empty_list_num], ""))
				break;
		}

		if(empty_list_num >=MAX_ENTRIES_LIST)
		{
			printf("%s() num_entries_list is overflow!!!\n", __FUNCTION__);
			return false;
		}
		strcpy(num_entries_list[empty_list_num], path);
		#ifdef TR369_DEBUG
		printf("num_entries_list[%d] = %s\n", empty_list_num, num_entries_list[empty_list_num]);
		#endif

		return true;
	}
	return false;
}

/**
 * @brief Delete num entries candidate list
 * 
 * @return true Successful, false Fail.
 */
bool handle_num_entries_reset_list(void)
{
	int empty_list_num;

	#ifdef TR369_DEBUG
	printf("%s() Enter\n", __FUNCTION__);
	#endif

	for(empty_list_num = 0; empty_list_num < MAX_ENTRIES_LIST; empty_list_num++)
	{
		strcpy(num_entries_list[empty_list_num], "");
	}
	return true;
}

/**
 * @brief Check param is in candidate list & register it.
 * 
 * @param path path is param.{i}. (e.g. Device.Cellular.Interface.{i}.)
 * @return true Successful, false Fail.
 */
bool handle_num_entries_reg(char *path)
{
	int len;
	int empty_list_num;
	char param[MAX_DM_PATH];
	int fault = USP_ERR_OK;

	#ifdef TR369_DEBUG
	printf("%s() Enter\n", __FUNCTION__);
	#endif

	// param string change from param.{i}. to paramNumberOfEntries
	strcpy(param, path);
	len = strlen(param) - strlen(".{i}.");
	if (len < 0)
		return false;

	param[len] = '\0';
	strcat(param, NUMENTRY);

	// check if the list has param & register it
	for(empty_list_num = 0; empty_list_num < MAX_ENTRIES_LIST; empty_list_num++)
	{
		if(!strcmp(num_entries_list[empty_list_num], param))
		{
			#ifdef TR369_DEBUG
			printf("call USP_REGISTER_Param_NumEntries num_entries_list[%d]=%s, path=%s\n", empty_list_num, num_entries_list[empty_list_num], path);
			#endif
			fault = USP_REGISTER_Param_NumEntries(num_entries_list[empty_list_num], path);
			if (fault == USP_ERR_OK)
			{
				strcpy(num_entries_list[empty_list_num], "");
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Get candidate list index param
 * 
 * @param list_num candidate list index, index < MAX_ENTRIES_LIST
 * @return char* candidate list index param, if Fail return NULL pointer.
 */
char *handle_num_entries_get(int list_num)
{	
	#ifdef TR369_DEBUG
	printf("%s() Enter\n", __FUNCTION__);
	#endif
	
	if(0 <= list_num && list_num < MAX_ENTRIES_LIST)
	{
		return num_entries_list[list_num];
	}
	else
	{
		return NULL;
	}
}

/**
 * @brief Print num entries candidate list
 * 
 */
void handle_num_entries_print(void)
{
	int empty_list_num;

	#ifdef TR369_DEBUG
	printf("%s() Enter\n", __FUNCTION__);
	#endif

	printf("%s() Print num_entries_list:\n", __FUNCTION__);
	for(empty_list_num = 0; empty_list_num < MAX_ENTRIES_LIST; empty_list_num++)
	{
		printf("num_entries_list[%d] = %s\n", empty_list_num, num_entries_list[empty_list_num]);
	}
}
//handle NumberOfEntries end

int asyncOperThreadMutexInit(void){
	int i = 0, err = USP_ERR_OK;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	for(i = 0; i < MAX_ASYNC_REQUEST_NUM; i++){
		err = OS_UTILS_InitMutex(&(asyncOperReqThread[i].access_mutex));
		if(err != USP_ERR_OK){
			USP_LOG_Info("%s(%d): Error(= %d) with OS_UTILS_InitMutex(index = %d).\n", __func__, __LINE__, err, i);
			return err;
		}
		asyncOperReqThread[i].status = reqStatusIdle;

	}
	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return err;
}
int vendor_parse_tr181_and_register(void){
	int fault = USP_ERR_OK;
	char tr181Input[TR181_INPUT_LENGTH] = {0};
	const char delimChr[2] = ":";
	char *token = NULL;
	FILE *tr369Fp = NULL;
	char *path = NULL, tr181NodeType[TR181_NODETYPE_LENGTH] = {0};
	int tokenParseIndex = 0;
	int stillHaveToParse = 0;
	str_vector_t ref_param_vec;
	int group, i;
	int ret = 0;
	int group_id = 0, index, addGroup = 0;
	char group_name[MAX_DM_PATH], nodeName[MAX_DM_PATH], regNodeName[MAX_DM_PATH], pathName[MAX_DM_PATH];
	char buf[MAX_DM_PATH];
	int attr = 0;
	ulong type = 0;
	
	int err = USP_ERR_OK;
	int regInputArgNum = 0, regOutputArgNum = 0;
	char **regInputArg = NULL, **regOutputArg = NULL;
	int pathAdded = 0;//handle NumberOfEntries
	
	#if 1//JessieUniq
	char name[MAX_DM_PATH]={0};
	int isPartial = 0;
	char objpath[MAX_DM_PATH] = {0};
	char **keys = NULL;
	#endif

	STR_VECTOR_Init(&ref_param_vec);
	group = INVALID;

	regInputArg = (char**) USP_MALLOC(MAX_REGISTER_ELEMENTS * sizeof(char *));
	for(i = 0; i < MAX_REGISTER_ELEMENTS; i++){
		regInputArg[i] = (char*)USP_MALLOC((MAX_PARAMETER_NAME_LEN + 1)* sizeof(char));
	}

	regOutputArg = (char**) USP_MALLOC(MAX_REGISTER_ELEMENTS * sizeof(char *));
	for(i = 0; i < MAX_REGISTER_ELEMENTS; i++){
		regOutputArg[i] = (char*)USP_MALLOC((MAX_PARAMETER_NAME_LEN + 1)* sizeof(char));
	}

	//TEMP for test
	USP_REGISTER_AsyncOperation("Device.test()", async_operate_test_handler, NULL);
	//TEMP for test
	if((tr369Fp = fopen(TR369FILE, "rw")) == NULL){
		printf("%s(%d):Error on opening file %s.\n", __func__, __LINE__, TR369FILE);
		fault = USP_ERR_INTERNAL_ERROR;
		goto Release_and_exit;
	}

	//stillHaveToParse = 0;
	if(fgets(tr181Input, TR181_INPUT_LENGTH, tr369Fp) == NULL){
		printf("%s(%d) fgets() Error.\n", __func__, __LINE__);
		fault = USP_ERR_INTERNAL_ERROR;
		goto Release_and_exit;
	}
	handle_num_entries_reset_list();
	stillHaveToParse = 1;
	while(1){
		if(!stillHaveToParse) break;
		stillHaveToParse = 0;
		if(strstr(tr181Input, "object") != NULL || strstr(tr181Input, "index_object") != NULL 
				|| strstr(tr181Input, "TR369Operate") != NULL
				|| strstr(tr181Input, "TR369AsyncOperate") != NULL
				|| strstr(tr181Input, "TR369Event") != NULL ){
			regInputArgNum = 0;
			regOutputArgNum = 0;
			tokenParseIndex = 0;
			//first token
			token = strtok(tr181Input, delimChr);
			while(token != NULL){
				switch(tokenParseIndex){
					case 0:
						#ifdef TR369_DEBUG
						printf("Node is %s\n", token);
						#endif
						snprintf(nodeName, sizeof(nodeName), "%s.", token);//Jessie add .
						snprintf(regNodeName, sizeof(regNodeName), "%s", token);
						
						index = STR_VECTOR_Find(&g_group_vec, nodeName);
						#ifdef TR369_DEBUG
						printf("index is %d\n", index);
						#endif
						if (index == INVALID) {
							group_id = g_group_vec.num_entries;
						}
						
						break;
					case 1:
						snprintf(tr181NodeType, TR181_NODETYPE_LENGTH, "%s", token);
						#ifdef TR369_DEBUG
						printf("Nodetype is %s, nodeName is %s,group_id=%d\n", tr181NodeType, nodeName, group_id);
						#endif
						/*
						if (CountPathSeparator(nodeName) < MAX_GROUP_SEP) {
							printf("path(%s) count(%d) is less\n", nodeName, MAX_GROUP_SEP);
							addGroup = 0;
						} else {
							addGroup = 1;
						}
						*/
						addGroup = 1;
						
						if(addGroup){
							if(!strcmp(tr181NodeType, "object")){
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_GroupedObject\n");
								#endif
								USP_REGISTER_GroupedObject(group_id, nodeName, true);
							} else if(!strcmp(tr181NodeType, "index_object")){
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_GroupedObject and USP_REGISTER_Object_RefreshInstances\n");
								#endif
								USP_REGISTER_GroupedObject(group_id, nodeName, true);
								USP_REGISTER_Object_RefreshInstances(nodeName, cwmp_cm_get_recursive_names);
								handle_num_entries_reg(nodeName);
							}
							else if(!strcmp(tr181NodeType, "TR369Operate")){
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_GroupedObject\n");
								#endif
								USP_REGISTER_GroupedObject(group_id, nodeName, true);
							}
							else if(!strcmp(tr181NodeType, "TR369AsyncOperate")){
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_GroupedObject\n");
								#endif
								USP_REGISTER_GroupedObject(group_id, nodeName, true);
							}
							else if(!strcmp(tr181NodeType, "TR369Event")){
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_GroupedObject\n");
								#endif
								USP_REGISTER_GroupedObject(group_id, nodeName, true);
							}
						}
						break;
				}
				if(addGroup){
					#ifdef TR369_DEBUG
					printf("nodeName=%s,call USP_REGISTER_GroupVendorHooks and STR_VECTOR_Add\n", nodeName);
					#endif
					USP_REGISTER_GroupVendorHooks(group_id, GetMyParams, SetMyParams, AddMyObject, NotifyAddMyObject, DelMyObject, NotifyDelMyObject);
					STR_VECTOR_Add(&g_group_vec, nodeName);
					addGroup = 0;
				}
				
				tokenParseIndex++;
				token = strtok(NULL, delimChr);
			}
		}

		//parse parameters part
		while(fgets(tr181Input, TR181_INPUT_LENGTH, tr369Fp) != NULL){
			attr = 0;//reset attr
			
			//get the object/index_object/operate/event means it is all the parameters of the last object
			if(strstr(tr181Input, "object") != NULL || strstr(tr181Input, "index_object") != NULL 
					|| strstr(tr181Input, "TR369Operate") != NULL
					|| strstr(tr181Input, "TR369AsyncOperate") != NULL
					|| strstr(tr181Input, "TR369Event") != NULL
					){
				//register the last objec/index_object/operate/event
				if(!strcmp(tr181NodeType, "TR369AsyncOperate") 
						|| strstr(nodeName, "IPPing")
						|| !strcmp(nodeName, "Device.IP.Diagnostics.TraceRoute.")
						|| !strcmp(nodeName, "Device.IP.Diagnostics.DownloadDiagnostics.")
						|| strstr(nodeName, "Restore")
						|| strstr(nodeName, "Backup")
						){
					strcat(regNodeName, "()");
//#ifdef TR369_DEBUG
					printf("%s(%d), %s: Input\n", __func__, __LINE__, regNodeName);
					for(i = 0; i< regInputArgNum; i++){
						printf("%p %s\n", regInputArg[i], regInputArg[i]);
					}
					printf("%s(%d) %s: Output\n", __func__, __LINE__, regNodeName);
					for(i = 0; i< regOutputArgNum; i++){
						printf("%p %s\n", regOutputArg[i], regOutputArg[i]);
					}
//#endif
					err = USP_ERR_OK;
					err |= USP_REGISTER_AsyncOperation(regNodeName, async_operate_handler, NULL);
					if (err == USP_ERR_OK)
					{
						if(regInputArgNum != 0 || regOutputArgNum != 0){
							err |= USP_REGISTER_OperationArguments(regNodeName, regInputArg, regInputArgNum, regOutputArg, regOutputArgNum);

							if (err != USP_ERR_OK)
							{
								printf("%s(%d): Error on USP_REGISTER_OperationArguments() :%s\n", __func__, __LINE__, regNodeName);
								fault = USP_ERR_INTERNAL_ERROR;
								goto Release_and_exit;
							}
						}
					}
					else{
						printf("%s(%d): Error on USP_REGISTER_AsyncOperation(err = %d) :%s\n", __func__, __LINE__, err, regNodeName);
					}

				}
				else if(!strcmp(tr181NodeType, "TR369Operate")){
					strcat(regNodeName, "()");
//#ifdef TR369_DEBUG
					printf("%s(%d) %s: Input\n", __func__, __LINE__, regNodeName);
					for(i = 0; i< regInputArgNum; i++){
						printf("%p %s\n", regInputArg[i], regInputArg[i]);
					}
					printf("%s(%d) %s: Output\n", __func__, __LINE__, regNodeName);
					for(i = 0; i< regOutputArgNum; i++){
						printf("%p %s\n", regOutputArg[i], regOutputArg[i]);
					}
//#endif
					err = USP_ERR_OK;
					err |= USP_REGISTER_SyncOperation(regNodeName, sync_operate_handler);
					if (err == USP_ERR_OK)
					{
						if(regInputArgNum != 0 || regOutputArgNum != 0){
							err |= USP_REGISTER_OperationArguments(regNodeName, regInputArg, regInputArgNum, regOutputArg, regOutputArgNum);

							if (err != USP_ERR_OK)
							{
								printf("%s(%d): Error on USP_REGISTER_OperationArguments() :%s\n", __func__, __LINE__, regNodeName);
								fault = USP_ERR_INTERNAL_ERROR;
								goto Release_and_exit;
							}
						}
					}
					else{
						printf("%s(%d): Error on USP_REGISTER_SyncOperation(err = %d) :%s\n", __func__, __LINE__, err, regNodeName);
					}

				}
				else if(!strcmp(tr181NodeType, "TR369Event")){
					strcat(regNodeName, "!");
//#ifdef TR369_DEBUG
					printf("%s(%d), %s: Output\n", __func__, __LINE__, regNodeName);
					for(i = 0; i< regOutputArgNum; i++){
						printf("%p %s\n", regOutputArg[i], regOutputArg[i]);
					}
//#endif
					err = USP_ERR_OK;
					err |=  USP_REGISTER_Event(regNodeName);
					if (err == USP_ERR_OK)
					{
						if(regOutputArgNum != 0){
							err |= USP_REGISTER_EventArguments(regNodeName, regOutputArg, regOutputArgNum);

							if (err != USP_ERR_OK)
							{
								printf("%s(%d): Error on USP_REGISTER_EventArguments() :%s\n", __func__, __LINE__, regNodeName);
								fault = USP_ERR_INTERNAL_ERROR;
								goto Release_and_exit;
							}
						}
					}
					else{
						printf("%s(%d): Error on USP_REGISTER_Event(err = %d) :%s\n", __func__, __LINE__, err, regNodeName);
					}
				}
				
				#ifdef TR369_DEBUG
				printf("==============\n");
				#endif
				stillHaveToParse = 1;
				break;
			}

			//first token
			tokenParseIndex = 0;
			token = strtok(tr181Input, delimChr);
			while(token != NULL){
				switch(tokenParseIndex){
					case 0:
						snprintf(pathName, sizeof(pathName), "%s", token);
						//handle NumberOfEntries begin
						#ifdef TR369_DEBUG
						printf("call handle_num_entries_insert_list, pathName=%s\n", pathName);
						#endif
						if (handle_num_entries_insert_list(pathName)){
							pathAdded = 1;
						} else pathAdded = 0;
						//handle NumberOfEntries end
						break;
					case 1:
						if(strstr(token,"read")){

							if(!strcmp(tr181NodeType, "TR369Operate") || 
									!strcmp(tr181NodeType, "TR369AsyncOperate") ||
									!strcmp(tr181NodeType, "TR369Event") || 
									strstr(nodeName, "IPPing") || 
									!strcmp(nodeName, "Device.IP.Diagnostics.TraceRoute.") || 
									!strcmp(nodeName, "Device.IP.Diagnostics.DownloadDiagnostics.") ||
									strstr(nodeName, "Restore") ||
									strstr(nodeName, "Backup")
									){
								i = 0;
								//only need the last parameter name/ the nodeName should contain the last dot
								while(nodeName[i] == pathName[i]){
									i++;
								}
								snprintf(regOutputArg[regOutputArgNum++], MAX_PARAMETER_NAME_LEN, "%s", pathName+i);
							}

							attr |= ATTR_READ;
						} else if(strstr(token,"write")){

							if(!strcmp(tr181NodeType, "TR369Operate") || 
									!strcmp(tr181NodeType, "TR369AsyncOperate") ||
									!strcmp(tr181NodeType, "TR369Event") || 
									strstr(nodeName, "IPPing") ||
									!strcmp(nodeName, "Device.IP.Diagnostics.TraceRoute.") ||
									!strcmp(nodeName, "Device.IP.Diagnostics.DownloadDiagnostics.") ||
									strstr(nodeName, "Restore") ||
									strstr(nodeName, "Backup")
									){
								i = 0;
								//only need the last parameter name/ the nodeName should contain the last dot
								while(nodeName[i] == pathName[i]){
									i++;
								}
								snprintf(regInputArg[regInputArgNum++], MAX_PARAMETER_NAME_LEN, "%s", pathName+i);
							}

							attr |= ATTR_WRITE;
						}

						if(strstr(token,"uniquekey")){
							attr |= ATTR_UNIQUEKEY;
						}
						
						break;
					case 2:
						if(!strncmp(token, "string", strlen("string"))){
							type = DM_STRING;
						} else if(!strncmp(token, "boolean", strlen("boolean"))){
							type = DM_BOOL;
						} else if(!strncmp(token, "int32", strlen("int32"))){
							type = DM_INT;
						} else if(!strncmp(token, "uint32", strlen("uint32"))){
							type = DM_UINT;
						} else if(!strncmp(token, "ulong", strlen("ulong"))){
							type = DM_ULONG;
						} else if(!strncmp(token, "dateTime", strlen("dateTime"))){
							type = DM_DATETIME;
						}
						
						if(attr & ATTR_WRITE){
							#ifdef TR369_DEBUG
							printf("call USP_REGISTER_GroupedVendorParam_ReadWrite group_id=%d,pathName=%s,type=%lu\n", group_id, pathName, type);
							#endif
							USP_REGISTER_GroupedVendorParam_ReadWrite(group_id, pathName, type);
						} else if(attr & ATTR_READ){
							#ifdef TR369_DEBUG
							printf("call USP_REGISTER_GroupedVendorParam_ReadOnly group_id=%d,pathName=%s,type=%lu\n", group_id, pathName, type);
							#endif
							USP_REGISTER_GroupedVendorParam_ReadOnly(group_id, pathName, type);
						}

						if(attr & ATTR_UNIQUEKEY){
							if(cwmp_cm_parse_fullpath( pathName, objpath, name, &isPartial) != TR069_OK){
								printf("%s() cwmp_cm_parse_fullpath fail\n", __FUNCTION__);
							} else {
								keys = (char **) USP_MALLOC(1 * sizeof(char *));
								if (keys == NULL)
									break;

								keys[0] = name;
								#ifdef TR369_DEBUG
								printf("call USP_REGISTER_Object_UniqueKey nodeName=%s,pathName=%s\n", nodeName, name);
								#endif
								USP_REGISTER_Object_UniqueKey(nodeName, keys, 1);
								USP_FREE(keys);
							}
						}

						break;
				}
				//handle NumberOfEntries begin
				if(pathAdded){
					token = NULL;
					break;
				}
				//handle NumberOfEntries end
				tokenParseIndex++;
				token = strtok(NULL, delimChr);
			}
			
		}//end of while of fgets() to get all possible parameters

	}// end of while(1)

	// call SP_REGISTER_VendorParam_ReadOnly for cleaning candidate list.
	#ifdef TR369_DEBUG
	handle_num_entries_print();
	#endif
	for(int i = 0; i < MAX_ENTRIES_LIST; i++)
	{
		path = handle_num_entries_get(i);
		if(strcmp(path, ""))
			err |= USP_REGISTER_VendorParam_ReadOnly(path, uspd_get_value, DM_UINT);
	}
	if (err != USP_ERR_OK)
	{
		printf("%s(%d): Error on USP_REGISTER_VendorParam_ReadOnly():%s\n", __func__, __LINE__, path);
		fault = USP_ERR_INTERNAL_ERROR;
	}

Release_and_exit:

	if(tr369Fp != NULL){
		fclose(tr369Fp);
	}

	//free allocated memory
	if(regInputArg != NULL){
		for(i = 0; i < MAX_REGISTER_ELEMENTS; i++){
			if(regInputArg[i] != NULL){
				USP_FREE(regInputArg[i]);
			}
		}
		USP_FREE(regInputArg);
	}

	if(regOutputArg != NULL){
		for(i = 0; i < MAX_REGISTER_ELEMENTS; i++){
			if(regOutputArg[i] != NULL){
				USP_FREE(regOutputArg[i]);
			}
		}
		USP_FREE(regOutputArg);
	}

	return fault;
}
