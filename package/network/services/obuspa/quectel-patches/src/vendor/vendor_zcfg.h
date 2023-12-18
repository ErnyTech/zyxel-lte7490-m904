
#ifndef VENDOR_ZCFG_H
#define VENDOR_ZCFG_H

#include "common_defs.h"
#include "usp_api.h"
#include "dm_access.h"
#include "os_utils.h"
#include "vendor_operate_async.h"

#define TR181_INPUT_LENGTH 256
#define TR181_NODETYPE_LENGTH 32




int vendor_uspd_init(void);
int vendor_uspd_stop(void);
int vendor_uspd_start(void);
int cwmp_cm_get_recursive_names(int group_id, char *path, int *expiry_period);
int GetMyParams(int group_id, kv_vector_t *params);
int SetMyParams(int group_id, kv_vector_t *params, unsigned *param_types, int *failure_index);
int AddMyObject(int group_id, char *path, int *instance);
int NotifyAddMyObject(int group_id, dm_req_t *req);
int DelMyObject(int group_id, char *path);
int NotifyDelMyObject(int group_id, dm_req_t *req);

#endif
