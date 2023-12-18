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

#ifndef _TR369_CMD_H_
#define _TR369_CMD_H_

#include <stdio.h>
#include <pthread.h>
#include "tr369socket.h"
#include "tr369typedef.h"

#define TR369CMD_PATH "/tmp/tr369_cmd"
#ifdef ZYXEL_TR369_SUPPLEMENTAL_ACS
#define TR369CMD_SUPPLEMENTAL_PATH "/tmp/tr369_supplemental_cmd"
#endif
#if defined ISKON_CUSTOMIZATION || defined TELIA_CUSTOMIZATION
#define TR369CMD_OUTPUT "/tmp/tr369cmdOutputData"
#endif
#define SIZE_IPCVAL				1024 /*ObjectValue*/

//extern uint32_t TR369_FEATURE_Flag;
#define TR369_FEATURE_UPLOAD_BY_PUT		(1 << 0) // RPC upload file by PUT method instead of POST.
#define TR369_FEATURE_CLEARTEXT_PSK		(1 << 1) // readable psk instead of empty string.

typedef enum tr369IpcCmd_s{
	eTr369IpcEnable,
	eTr369IpcDisable,
	eTr369IpcClose,
#if 0//Jessie debug_on_off
	eTr369IpcDebugOn,
	eTr369IpcDebugOff,
#endif
	eTr369IpcDisplay,
	eTr369IpcActive,
	eTr369IpcIpProtocol,
	eTr369IpcAcsUrl,
	eTr369IpcAcsUsername,
	eTr369IpcAcsPassword,
	eTr369IpcRequestUsername,
	eTr369IpcRequestPassword,
	eTr369IpcPeriodicEnable,
	eTr369IpcInformInterval,
	eTr369IpcInformTime,
	eTr369IpcActiveNotifyUpdateInterval,
	eTr369IpcApplyNow,
	eTr369IpcOutputObjNumName,
#if 1//Jessie debug_on_off
	eTr369IpcDebugOn,
	eTr369IpcDebugOff,
#endif
#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
        eTr369IpcSyncTrustCert,//Jessie SyncTrustCert()
        eTr369IpcPublish
#else
        eTr369IpcSyncTrustCert//Jessie SyncTrustCert()
#endif
}tr369IpcCmd_t;

typedef struct tr369IpcMsgData_s{
	tr369IpcCmd_t ipcCmd;
	char data[SIZE_IPCVAL+1];
}tr369IpcMsgData_t;

void tr369_cmd_task(void);

#endif /*_TR369_CMD_H_*/
