#ifndef _ZCFG_FE_RDM_ACCESS_H
#define _ZCFG_FE_RDM_ACCESS_H
#include "zcfg_msg.h"
/*
 *  Provide the API for front end process to communicate with
 *  zcmd.
 */
zcfgRet_t zcfgFeSharedMemInit();
zcfgRet_t zcfgFeObjRead(zcfg_offset_t, objIndex_t *, uint32_t, char **);
zcfgRet_t zcfgFeObjWrite(zcfg_offset_t, objIndex_t *, char *, uint32_t, char *);
zcfgRet_t zcfgFeWholeObjReadByName(char *tr181path, uint32_t type, char **result);
zcfgRet_t zcfgFeWholeObjWriteByName(char *tr181path, char *setValue, uint32_t type, char *feedbackMsg);

zcfgRet_t zcfgFeReqSend(uint32_t, char *);

#define zcfgFeDelayApply(payload)       zcfgFeReqSend(ZCFG_MSG_DELAY_APPLY, payload)
#define zcfgFeReApply()                 zcfgFeReqSend(ZCFG_MSG_RE_APPLY, NULL)
#define zcfgFeFwUpgrade(payload)        zcfgFeReqSend(REQUEST_FIRMWARE_UPGRADE, payload)
#define zcfgFeFwUpgradeForFWUR(payload)        zcfgFeReqSend(REQUEST_FIRMWARE_UPGRADE_FWUR, payload)
#define zcfgFeFwUpgradeWrite(payload)   zcfgFeReqSend(REQUEST_FIRMWARE_WRITE_ONLY, payload)
#define zcfgFeFwUpgradeWriteFirstPart(payload)    zcfgFeReqSend(REQUEST_FIRMWARE_WRITE_ONLY_FIRST_PARTITION, payload)
#define zcfgFeFwUpgradeWriteSecondPart(payload)   zcfgFeReqSend(REQUEST_FIRMWARE_WRITE_ONLY_SECOND_PARTITION, payload)
#define zcfgFeFwUpgradeFirstPart(payload)        zcfgFeReqSend(REQUEST_FIRMWARE_UPGRADE_FIRST_PARTITION_CHK, payload)
#define zcfgFeFwUpgradeSecondPart(payload)       zcfgFeReqSend(REQUEST_FIRMWARE_UPGRADE_SECOND_PARTITION_CHK, payload)
#define zcfgFeFwUpgradeCheck(payload)	zcfgFeReqSend(REQUEST_FIRMWARE_UPGRADE_CHK, payload)
#define zcfgFeRestoreDefault(payload)   zcfgFeReqSend(REQUEST_RESTORE_DEFAULT, payload)
#define zcfgFeRestoreDeCheck(payload)   zcfgFeReqSend(REQUEST_RESTORE_DEFAULT_CHK, payload)
#define zcfgFeRomdUpgrade(payload)      zcfgFeReqSend(REQUEST_ROMD_UPGRADE, payload)
#define zcfgFeRomdSave(payload)      	zcfgFeReqSend(ZCFG_MSG_SAVE_ROMD, payload)
#define zcfgFeRomdClean(payload)      	zcfgFeReqSend(ZCFG_MSG_CLEAN_ROMD, payload)
#define zcfgFeCheckFwIdWhenUpgradingStatus(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_FWID_STATUS, payload)
#define zcfgFeCheckModelWhenUpgradingStatus(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_MODEL_STATUS, payload)
#define zcfgFeConfBackup(payload)       zcfgFeReqSend(REQUEST_CONFIG_BACKUP, payload)
#define zcfgFeAutoProvision(payload)    zcfgFeReqSend(ZCFG_MSG_AUTO_PROVISION, payload)
#define zcfgFeResetWan(payload)         zcfgFeReqSend(REQUEST_REINIT_MACLESS_PROVISIONING, payload)
#define zcfgFeReqReboot(payload)        zcfgFeReqSend(ZCFG_MSG_REQ_REBOOT, payload)
#define zcfgFeReqSyncFlash(payload)     zcfgFeReqSend(ZCFG_MSG_REQ_SYNCFLASH, payload)
#define zcfgFeCheckFwIdWhenUpgradingOff(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_FWID_OFF, payload)
#define zcfgFeCheckFwIdWhenUpgradingOn(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_FWID_ON, payload)
#define zcfgFeCheckModelWhenUpgradingOff(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_MODEL_OFF, payload)
#define zcfgFeCheckModelWhenUpgradingOn(payload)     zcfgFeReqSend(ZCFG_MSG_UPGRADING_FW_CHECK_MODEL_ON, payload)
#define zcfgFeReqTr98ParmAttrList(payload)	zcfgFeReqSend(ZCFG_MSG_REQUEST_GET_PARAM_ATTR_LIST, payload)
#define zcfgFeReqSwitchBootPartition(payload)	zcfgFeReqSend(ZCFG_MSG_REQ_SWITCH_BOOT_PARTITION, payload)
#define zcfgFeSysLogClean(payload)      zcfgFeReqSend(ZCFG_MSG_CLEAN_SYSLOG, payload)
#define zcfgFeSetParamK(payload)       zcfgFeReqSend(REQUEST_SET_PARAMETERKEY, payload)
#define zcfgFeRestorePartial(payload)   zcfgFeReqSend(REQUEST_PARTIAL_RESTORE_DEFAULT, payload)
/*TR98 support*/
zcfgRet_t zcfgFeMappingNameGet(uint32_t msgType, char *fullPathName, char *result);

zcfgRet_t zcfgFe98To181ObjMappingNameGet(const char *tr98PathName, char *tr181PathName);
zcfgRet_t zcfgFe181To98ObjMappingNameGet(const char *tr181PathName, char *tr98PathName);
#define zcfgFeAllIGDNameGet()           zcfgFeReqSend(ZCFG_MSG_GET_TR98_ALL_IGD_NAMES, NULL)

#if 1 //change  to new TR98 mapping table
#define zcfgFe98To181MappingNameGet(fullPathName, result) zcfgFe98To181ObjMappingNameGet(fullPathName, result)
#define zcfgFe181To98MappingNameGet(fullPathName, result) zcfgFe181To98ObjMappingNameGet(fullPathName, result)
#else
#define zcfgFe98To181MappingNameGet(fullPathName, result) zcfgFeMappingNameGet(ZCFG_MSG_REQUEST_TR98_MAPPING, fullPathName, result)
#define zcfgFe181To98MappingNameGet(fullPathName, result) zcfgFeMappingNameGet(ZCFG_MSG_REQUEST_TR181_MAPPING, fullPathName, result)
#endif

/*End of TR98 support*/

/*obuspa support*/
#define TR369_BOOT_INFO "/data/tr369_bootinfo.dat"
#define TR369_RESTART_INFO "/tmp/tr369_restart_info.dat"
#define tr369_strget(product_name, fp_tmp) fgets(product_name, sizeof(product_name), fp_tmp); product_name[strlen(product_name) - 1] = (product_name[strlen(product_name) - 1] == '\n') ? '\0' : product_name[strlen(product_name) - 1];
#define MAX_INFO_STR 256
//------------------------------------------------------------------------------
// Structure containing cause of last boot cycle
typedef struct
{
    char cause[MAX_INFO_STR];                     // cause of the last reboot
    char command_key[MAX_INFO_STR];               // command_key associated with the last reboot
    char last_software_version[MAX_INFO_STR];     // Software version before the current boot period
    int request_instance;            // Instance number of the request that initiated the reboot, or INVALID if reboot was not initiated by an operation
} tr369_boot_info_t;

bool tr369_get_boot_info(tr369_boot_info_t *info);
bool tr369_set_boot_info(tr369_boot_info_t info);
bool tr369_set_local_factory_reset_boot_info();
/*End of obuspa support*/

int zcfgFeObjMaxLenGet(zcfg_offset_t rdmObjId);
int zcfgFeArrayLengthGetByName(char* tr181path);

#endif
