#ifndef __VENDOR_OPERATE_ASYNC__
#define __VENDOR_OPERATE_ASYNC__

#include "zyutil.h"
#include "vendor_defs.h"
#include "vendor_utils.h"
#if 1//Jessie InstallDU()
#include "dm_trans.h"
#endif
#define FIRMWARE_STORE_PATH         "/tmp/firmwareUpgrade"
#define LOCAL_CONFIG_TMP_PATH  		"/tmp/zcfg_config_tmp.json"
#define MAX_REGISTER_ELEMENTS 32
#if 1//Jessie InstallDU()
#define DU_PATH  		"/data/du"
#endif

#define DEFAULT_DATETIME_VALUE "0001-01-01T00:00:00Z"
#define DATETIME_LENGTH 32

#define MAX_ASYNC_REQUEST_NUM 256
#define REBOOT_COUNTDOWN_TIMES 10 /*Second unit*/
#define DEVICE_REQ_ROOT "Device.LocalAgent.Request"

#define MAX_BUF_LEN 32
#define DIAG_RESULT_PATH "/var/diagResult"
#define DIAG_PROCESS_PATH "/var/diagProcess"
#define PATH_PING	"/bin/ping"
#define PATH_TRACEROUTE "/usr/bin/traceroute"
#define PATH_REQUEST	 "/tmp/request"
#define DEFAULT_IPPING_NUM_REPETITION  "4"//string
#define DEFAULT_TRACE_ROUTE_NUM_TRIES "3"
#define DEFAULT_TRACE_ROUTE_MAX_HOP "30"
#define DEFAULT_TRACE_ROUTE_TIMEOUT "5000"
#define DEFAULT_TRACE_ROUTE_DATABLOCK_SIZE "38"
#define DEFAULT_DSCP "0"
#define PATH_ZHTTPPUT	 "/bin/zhttpput"
#define PATH_FTPPUT	 "/usr/bin/ftpput"
#define IPADDR_ANY "0.0.0.0"
#define DEFAULT_HTTP_PORT "80"
#define DEFAULT_FTP_PORT "21"
#ifdef ZYXEL_TR143_DIAG_ENHANCEMENT
#define PATH_HTTPDIAG "/bin/httpdiag"
#endif

//Device.IP.Diagnostics.DownloadDiagnostics() default values
#define DEFAULT_DLDIAG_DSCP									0
#define DEFAULT_DLDIAG_ETHERNET_PRIORITY					0
#define DEFAULT_DLDIAG_TIME_BASED_TEST_DURATION				0
#define DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_OFFSET	0
#define DEFAULT_DLDIAG_TIME_BASED_TEST_MEASUREMENT_INTERVAL	0
#define DEFAULT_DLDIAG_PROTOCOL_VERSION						"Any"
#define DEFAULT_DLDIAG_NUM_CONNECTIONS						1
#define DEFAULT_DLDIAG_ENABLE_PER_CONNECTION_RESULTS		false
#define DEFAULT_DLDIAG_DOWNLOAD_DIRECTORY                                      "/tmp"


typedef struct
{
	int request_instance;   // Instance number of this operation in the Device.LocalAgent.Request table
	char command[MAX_DM_PATH];
	char commandKey[MAX_DM_PATH];
	char originator[MAX_DM_PATH];
	char fullPath[MAX_DM_PATH];
	char parentPath[MAX_DM_PATH];
	char *action;
	kv_vector_t *input_args;
} oper_input_cond_t;

typedef struct
{
	char result_str[1025];
	char err_msg[256];
} oper_output_res_t;

typedef struct
{
	pthread_t threadID;
	pthread_mutex_t access_mutex;
	oper_input_cond_t *cond;
	kv_vector_t *operCompleteArgs;
	kv_vector_t *transferCompleteOuputArgs;
	int status;
	void *auxiliary;
} asyncOperReqThread_t;

typedef enum {
	reqStatusIdle = 0,
	reqStatusRequested,  	//The command has been requested but is currently not executing
	reqStatusActive,    	//The command is currently executing
	reqStatusActiveCantCanceled,
	reqStatusCanceling,		//The command has been requested to be canceled
	reqStatusCanceled,		//The command has successfully been canceled
	reqStatusSuccess,		//The command has successfully been completed its execution
	reqStatusError,			//The command has unsuccessfully completed its execution or has unsuccessfully been canceled
}asyncOperStatus_t;

asyncOperReqThread_t asyncOperReqThread[MAX_ASYNC_REQUEST_NUM];
int addTransferCompleteParam(asyncOperReqThread_t *instance);
void *AsyncOperation_Download_Thread(void *param);
void *AsyncOperation_IPPing_Thread(void *param);
void *AsyncOperation_TraceRoute_Thread(void *param);
void *AsyncOperation_DownloadDiagnostics_Thread(void *param);
void *AsyncOperation_UploadDiagnostics_Thread(void *param);
int localagentReqCancel(int reqInstance);
void *AsyncOperation_Config_Restore_Thread(void *param);
void *AsyncOperation_Config_Backup_Thread(void *param);
void *AsyncOperation_Log_Upload_Thread(void *param);
#if 1//Jessie InstallDU()
void *AsyncOperation_InstallDU_Thread(void *param);
void *AsyncOperation_DUUninstall_Thread(void *param);
void *AsyncOperation_SoftwareModules_DeploymentUnit_Update_Thread(void *param);
int DEVICE_SWMODULES_DU_Add(int *instance);
int DEVICE_SWMODULES_EU_Add();
int DEVICE_SWMODULES_EU_Del(int);
int DEVICE_SWMODULES_DU_Del(int);
#endif
#endif
