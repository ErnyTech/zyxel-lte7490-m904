/******************************************************************************/
/*
*  Copyright (C) 2012 ZyXEL Communications, Corp.
*  All Rights Reserved.
*
* ZyXEL Confidential; Need to Know only.
* Protected as an unpublished work.
*
* The computer program listings, specifications and documentation
* herein are the property of ZyXEL Communications, Corp. and
* shall not be reproduced, copied, disclosed, or used in whole or
* in part for any reason without the prior express written permission of
* ZyXEL Communications, Corp.
*/
/******************************************************************************/

#ifndef _ZTR69_COMMON_H_
#define _ZTR69_COMMON_H_

#include "stdint.h"

#define XSD_STRING "xsd:string"
#define XSD_INT "xsd:int"
#define XSD_UINT "xsd:unsignedInt"
#define XSD_BOOLEAN "xsd:boolean"
#define XSD_DATETIME "xsd:dateTime"
#define XSD_BASE64 "xsd:base64"
#define XSD_DOUBLE "xsd:double"
#define XSD_OBJ "xsd:object"
#define XSD_ARRAY "xsd:array"
#define XSD_HEX "xsd:hexBinary"
#define XSD_LONG "xsd:long"
#define XSD_ULONG "xsd:unsignedLong"

#define SIZE_NAME			256
#define	STRSIZE(size)	(size+1)	/* include "\0" */
typedef enum
{
	 ZTR69_COMMON_OK = 0,
	 ZTR69_COMMON_REQUEST_DENIED = 9001,
	 ZTR69_COMMON_INTERNAL_ERROR = 9002,
	 ZTR69_COMMON_INVALID_ARGUMENTS = 9003,
	 ZTR69_COMMON_RESOURCES_EXCEEDED = 9004,
	 ZTR69_COMMON_INVALID_PARAMETER_NAME = 9005,
	 ZTR69_COMMON_INVALID_PARAMETER_TYPE = 9006,
	 ZTR69_COMMON_INVALID_PARAMETER_VALUE = 9007,
	 ZTR69_COMMON_ATTEMPT_TO_SET_NONWRITABLE_PARAMETER = 9008
}ZTR69_COMMON_ERR_e;

#define ZTR69_COMMON_LOG_LEVEL_MUST 0
#define ZTR69_COMMON_LOG_LEVEL_OUTPUT 1
#define ZTR69_COMMON_LOG_LEVEL_INFO 3
#define ZTR69_COMMON_LOG_LEVEL_DEBUG 9

typedef struct aParameterValueStruct_s {
	char		name[STRSIZE(SIZE_NAME)];
	char       	*xsiType;
	char		*value;
} aParameterValueStruct_t;

typedef struct aParameterList_s {
	int			arrayNum;
	aParameterValueStruct_t *parameterValueStruct;

	void	*rpcHdlDesc;    // rpc handle routine descript
} aParameterList_t;

/*Fault*/
typedef struct aSetParameterValuesFault_s{
	char		parameterName[STRSIZE(SIZE_NAME)];
	uint16_t	faultCode;
	char		*faultString;
}aSetParameterValuesFault_t;


typedef struct aFault_s {
	uint16_t 		faultCode;
	char		*faultString;
	int			arrayNum;
	aSetParameterValuesFault_t *setParameterValuesFault;
} aFault_t;

typedef struct ztr69ParamterList_s
{   
    aParameterList_t List;
    int maxNumOfListBuf;
    aFault_t faultBuf;
    int maxNumOfListfaultBuf;
}ztr69ParamterList_t;
ZTR69_COMMON_ERR_e ztr69_common_init(int log_level);
ZTR69_COMMON_ERR_e ztr69_common_para_clear(ztr69ParamterList_t *pCwmpPList);
ZTR69_COMMON_ERR_e ztr69_common_para_alloc(ztr69ParamterList_t *pCwmpPList);
ZTR69_COMMON_ERR_e ztr69_common_para_free(ztr69ParamterList_t *pCwmpPList);
ZTR69_COMMON_ERR_e ztr69_common_para_add(ztr69ParamterList_t *pCwmpPList, char * objname, char * value);
ZTR69_COMMON_ERR_e ztr69_common_get(char *objname, ztr69ParamterList_t *pCwmpPList);
ZTR69_COMMON_ERR_e ztr69_common_set(ztr69ParamterList_t *pCwmpPList);
ZTR69_COMMON_ERR_e ztr69_common_add(char* objname,uint16_t* instanceNum);
ZTR69_COMMON_ERR_e ztr69_common_delete(char* objname);

#endif /*_CWMP_PUBLIC_H_*/
