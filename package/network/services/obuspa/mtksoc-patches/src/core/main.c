/*
 *
 * Copyright (C) 2019-2021, Broadband Forum
 * Copyright (C) 2016-2021  CommScope, Inc
 * Copyright (C) 2020, BT PLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file main.c
 *
 * Main function for USP Agent
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <protobuf-c/protobuf-c.h>
#include <curl/curl.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include "usp-msg.pb-c.h"
#include "common_defs.h"
#include "mtp_exec.h"
#include "dm_exec.h"
#include "bdc_exec.h"
#include "data_model.h"
#include "dm_access.h"
#include "device.h"
#include "database.h"
#include "sync_timer.h"
#include "cli.h"
#include "os_utils.h"
#include "text_utils.h"
#include "usp_coap.h"
#include "stomp.h"
#include "retry_wait.h"
#include "nu_macaddr.h"

#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_dal.h"
#include <sys/stat.h>

#include "tr369cmd.h"
#include "tr369socket.h"
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
#include "mqtt.h"
//20 is not used by USP
#define	MSG_PUBLISH 20
/* src/protobuf-c/usp-msg.pb-c.h
typedef enum _Usp__Header__MsgType {
  USP__HEADER__MSG_TYPE__ERROR = 0,
  USP__HEADER__MSG_TYPE__GET = 1,
  USP__HEADER__MSG_TYPE__GET_RESP = 2,
  USP__HEADER__MSG_TYPE__NOTIFY = 3,
  USP__HEADER__MSG_TYPE__SET = 4,
  USP__HEADER__MSG_TYPE__SET_RESP = 5,
  USP__HEADER__MSG_TYPE__OPERATE = 6,
  USP__HEADER__MSG_TYPE__OPERATE_RESP = 7,
  USP__HEADER__MSG_TYPE__ADD = 8,
  USP__HEADER__MSG_TYPE__ADD_RESP = 9,
  USP__HEADER__MSG_TYPE__DELETE = 10,
  USP__HEADER__MSG_TYPE__DELETE_RESP = 11,
  USP__HEADER__MSG_TYPE__GET_SUPPORTED_DM = 12,
  USP__HEADER__MSG_TYPE__GET_SUPPORTED_DM_RESP = 13,
  USP__HEADER__MSG_TYPE__GET_INSTANCES = 14,
  USP__HEADER__MSG_TYPE__GET_INSTANCES_RESP = 15,
  USP__HEADER__MSG_TYPE__NOTIFY_RESP = 16,
  USP__HEADER__MSG_TYPE__GET_SUPPORTED_PROTO = 17,
  USP__HEADER__MSG_TYPE__GET_SUPPORTED_PROTO_RESP = 18
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(USP__HEADER__MSG_TYPE)
} Usp__Header__MsgType;
*/
#endif
#ifndef OVERRIDE_MAIN
//--------------------------------------------------------------------------------------
// Array used by the getopt_long() function to parse a command line
// See http://www.gnu.org/s/hello/manual/libc/Getopt-Long-Options.html
// NOTE: When altering this array, make sure that you also alter the short options array as well
static struct option long_options[] =
{
//  long option,   option+argument?,  flag, short option
    {"help",       no_argument,       NULL, 'h'},    // Prints help for command line options
    {"log",        required_argument, NULL, 'l'},    // Sets the destination for the log file (either syslog, stdout or a filename)
    {"dbfile",     required_argument, NULL, 'f'},    // Sets the name of the path to use for the database file
    {"verbose",    required_argument, NULL, 'v'},    // Verbosity level for debug logging
    {"meminfo",    no_argument,       NULL, 'm'},    // Collects and prints information useful to debugging memory leaks
    {"error",      no_argument,       NULL, 'e'},    // Prints the callstack whenever an error is detected
    {"prototrace" ,no_argument,       NULL, 'p'},    // Enables logging of the protocol trace
    {"command",    no_argument,       NULL, 'c'},    // The rest of the command line is a command to invoke on the active USP Agent.
                                                     // Using this option turns this executable into just a CLI for the active USP Agent.
    {"authcert",   no_argument,       NULL, 'a'},    // Specifies the location of a file containing the client certificate to use authenticating this device
    {"truststore", required_argument, NULL, 't'},    // Specifies the location of a file containing the trust store certificates to use
    {"resetfile",  required_argument, NULL, 'r'},    // Specifies the location of a text file containing factory reset parameters
    {"interface",  required_argument, NULL, 'i'},    // Specifies the networking interface to use for communications

    {0, 0, 0, 0}
};

// In the string argument, the colons (after the option) mean that those options require arguments
static char short_options[] = "hl:f:v:a:t:r:i:mepc";
#endif

//--------------------------------------------------------------------------------------
// Variables set by command line arguments
bool enable_callstack_debug = false;    // Enables printing of the callstack when an error occurs


//--------------------------------------------------------------------------------------
// Forward declarations. Note these are not static, because we need them in the symbol table for USP_LOG_Callstack() to show them
void PrintUsage(char *prog_name);
int MAIN_Start(char *db_file, bool enable_mem_info);
void MAIN_Stop(void);

//----------Port zr69cmd applynow start
static int tr369_cmd_apply( tr369IpcMsgData_t *msgData );

typedef struct tr369IpcCmdHandler_s{
	tr369IpcCmd_t ipcCmd;
	int (*cmdFunc)(tr369IpcMsgData_t *msgData);
}tr369IpcCmdHandler_t;

#if 1//Jessie debug_on_off
/*extern */tr069ERR_e usp_set_srv(tr369IpcMsgData_t * msgData);
#define usp_cmd_set_mgmtserver usp_set_srv

int usp_set_srv( tr369IpcMsgData_t *msgData ){
        struct json_object *mgmtData;
		bool needFrontEndProcessing = false;
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
		int usp_msg_type = MSG_PUBLISH;//Jessie hard code test
		int instance = 2;//Jessie hard code test
		char *topic = NULL;
		char *pbuf = NULL;
		int pbuf_len = 0;
		char path[128];
		mqtt_conn_params_t *mp;
#endif
        if(!msgData)
                return USP_ERR_INTERNAL_ERROR;

        mgmtData = json_tokener_parse((const char *)msgData->data);
		#if 0
        if(is_error(mgmtData))
                return USP_ERR_INTERNAL_ERROR;
		#endif
        switch(msgData->ipcCmd) {
        case eTr369IpcDebugOn:
                {
                        enable_protocol_trace = true;
						//needFrontEndProcessing = true;
                }
                break;
        case eTr369IpcDebugOff:
                {
                        enable_protocol_trace = false;
						//needFrontEndProcessing = true;
                }
                break;
#if 1//Jessie SyncTrustCert()
        case eTr369IpcSyncTrustCert:
                {
						USP_LOG_Debug("%s() case eTr369IpcSyncTrustCert\n", __FUNCTION__);
						SyncTrustCert(false);
                }
                break;
#endif
//#if 1//Jessie [#220700403] CLI command to publish to a MQTT broker
#ifdef ZYXEL_CLI_PUBLISH_TO_MQTT_BROKER
        case eTr369IpcPublish:
                {

						for (int ii = 1; ii <= MAX_AGENT_MTPS; ii++){
							snprintf(path, sizeof(path), "Device.LocalAgent.MTP.%d.MQTT.Reference", ii);
							if (DEVICE_MTP_GetMqttReference(path, &instance) != USP_ERR_OK)
							{
								USP_LOG_Error("%s:%d Get path %s error).\n", __func__, __LINE__, path);
								continue;
							} else {
								mp = NULL;
								mp = FindMqttParamsByInstance(instance);
								if(mp != NULL && mp->enable){
									break;//find the first enabled MTP Protocol Reference
								}
							}
						}


						if(mgmtData){
							char *topicStr = NULL;
							char *msgStr = NULL;
							topicStr = json_object_get_string(json_object_object_get(mgmtData, "Topic"));
							msgStr = json_object_get_string(json_object_object_get(mgmtData, "Message"));
							if(topicStr){
								topic = USP_MALLOC(strlen(topicStr)+1);
								if(topic){
									memset(topic, 0, strlen(topicStr)+1);
									snprintf(topic, strlen(topicStr)+1, "%s", topicStr);
								}
							}
							if(msgStr){
								pbuf = USP_MALLOC(strlen(msgStr)+1);
								if(pbuf){
									memset(pbuf, 0, strlen(msgStr)+1);
									snprintf(pbuf, strlen(msgStr)+1, "%s", msgStr);
								}
								pbuf_len = strlen(pbuf);
							}
							USP_LOG_Info("%s: instance %d", __FUNCTION__, instance);
							if (MQTT_QueueBinaryMessage(usp_msg_type, instance, topic, pbuf, pbuf_len) != USP_ERR_OK)
							{
								USP_LOG_Error("%s: No internal MQTT Queue Binary message for topic %s", __FUNCTION__, topic);
								return USP_ERR_INTERNAL_ERROR;
							}
							if(topic){
								USP_SAFE_FREE(topic);
							}

						}
                }
                break;
#endif
        default: break;
        }

		#if 1
		if( needFrontEndProcessing ){
			USP_LOG_Debug("%s() zcfgFeDalHandler tr369_local_agent PUT\n", __FUNCTION__);
			zcfgFeDalHandler((const char *)"tr369_local_agent", (const char *)"PUT", mgmtData, NULL, NULL);
		}
		#endif
        json_object_put(mgmtData);
		return USP_ERR_OK;
}
#endif
const tr369IpcCmdHandler_t tr369HandleCmd[] = {
#if 0
	{eTr069IpcEnable, 	usp_cmd_main},
	{eTr069IpcDisable, 	usp_cmd_main},
	{eTr069IpcClose,	usp_cmd_main},
	{eTr069IpcDebugOn,	usp_cmd_set_mgmtserver},
	{eTr069IpcDebugOff,	usp_cmd_set_mgmtserver},
	{eTr069IpcDisplay,	usp_cmd_display},
	{eTr069IpcActive,	usp_cmd_set_mgmtserver},
	{eTr069IpcIpProtocol,	usp_cmd_set_mgmtserver},
	{eTr069IpcAcsUrl,	usp_cmd_set_mgmtserver},
	{eTr069IpcAcsUsername,	usp_cmd_set_mgmtserver},
	{eTr069IpcAcsPassword,	usp_cmd_set_mgmtserver},
	{eTr069IpcRequestUsername,	usp_cmd_set_mgmtserver},
	{eTr069IpcRequestPassword,	usp_cmd_set_mgmtserver},
	{eTr069IpcPeriodicEnable,	usp_cmd_set_mgmtserver},
	{eTr069IpcInformInterval,	usp_cmd_set_mgmtserver},
	{eTr069IpcInformTime,	usp_cmd_set_mgmtserver},
	{eTr069IpcActiveNotifyUpdateInterval, usp_cmd_set_mgmtserver},
	{eTr069IpcApplyNow,	usp_cmd_apply},
	{eTr069IpcOutputObjNumName, usp_cmd_output_objnumname},
#endif
#if 1//Jessie debug_on_off
	{eTr369IpcDebugOn,	usp_cmd_set_mgmtserver},
	{eTr369IpcDebugOff,	usp_cmd_set_mgmtserver},
#endif
#if 1//Jessie SyncTrustCert()
	{eTr369IpcSyncTrustCert,	usp_cmd_set_mgmtserver},
#endif
#if 1//Jessie SyncTrustCert()
	{eTr369IpcPublish,	usp_cmd_set_mgmtserver},
#endif
	{eTr369IpcApplyNow,	tr369_cmd_apply},
	{ -1, NULL}
};
pthread_mutex_t tr369MutexLock;
int gTr369CmdSockFd = -1;
/*************************
* Function 	: tr369_cmd_proc()
* Description	: tr369 cmd process
* Output 	: 
*************************/
int tr369_cmd_proc(
	void  *recvBuf
){
	tr369IpcMsgData_t *msgData = (tr369IpcMsgData_t *)recvBuf;
	tr369IpcCmdHandler_t *cmdhandlPtr = NULL;
	int ret = 0;
	
	//printf( "%s:%d ipcCmd=%d, data=%s", __func__, __LINE__, msgData->ipcCmd, msgData->data);

	cmdhandlPtr = (tr369IpcCmdHandler_t*)tr369HandleCmd;
	while( cmdhandlPtr->ipcCmd != -1) {
		if ( msgData->ipcCmd == cmdhandlPtr->ipcCmd ) {
			pthread_mutex_lock(&tr369MutexLock);
			if( cmdhandlPtr->cmdFunc(msgData) == -1/*TR069_SOCK_CLOSE*/ ){
				ret = -1;/*TR069_SOCK_CLOSE*/
			}
			pthread_mutex_unlock(&tr369MutexLock);
			break;
		}
		cmdhandlPtr++;
	}

	return ret;
}
/*************************
* Function 	: tr369_cmd_task()
* Description	: tr369 cmd task
* Output 	: 
*************************/
void tr369_cmd_task(void){
	OS_UTILS_SetZtr369CmdThread();
#ifdef ZYXEL_TR369_SUPPLEMENTAL_ACS
	if(SUPACS){
		if (0 == filesock_open(TR369CMD_SUPPLEMENTAL_PATH, &gTr369CmdSockFd)) {
			filesock_listen(TR369CMD_SUPPLEMENTAL_PATH, gTr369CmdSockFd, 1, 0, sizeof(tr369IpcMsgData_t), tr369_cmd_proc);
		}
	} else 
#endif
	if (0 == filesock_open(TR369CMD_PATH, &gTr369CmdSockFd)) {
		filesock_listen(TR369CMD_PATH, gTr369CmdSockFd, 1, 0, sizeof(tr369IpcMsgData_t), tr369_cmd_proc);
	}
	filesock_close( &gTr369CmdSockFd );
	gTr369CmdSockFd = -1;
} 
/*************************
* Function 	: tr369_cmd_apply()
* Description	: apply tr369 configure
* Output 	: 
*************************/
static int tr369_cmd_apply( tr369IpcMsgData_t *msgData ){
	//printf("%s\n", __func__);
#if 1
	mqtt_conn_params_t *mp;
	int i = 0, valueChangeHandle[MAX_MQTT_CLIENTS + 1] = {0};
	printf("[Marcus debug] %s:%d Now start to check mqtt instance number.\n", __func__, __LINE__);
    for (i = 1; i<=MAX_MQTT_CLIENTS; i++)
    {
		valueChangeHandle[i] = VALUE_CHANGED_HANDLE_DONOTHING;
	}

	device_Update_LocalAgent_Parameters(valueChangeHandle);

    for (i = 1; i <= MAX_CONTROLLERS; i++){
		printf("[Marcus debug] %s:%d Checking Controller with instance %d.\n", __func__, __LINE__, i);
		device_Update_LocalAgent_Controller_Parameters(i, valueChangeHandle);
	}
    for (i = 1; i <= MAX_AGENT_MTPS; i++){
		printf("[Marcus debug] %s:%d Checking MTP with instance %d.\n", __func__, __LINE__, i);
		device_Update_LocalAgent_MTP_MQTT_Parameters(i, valueChangeHandle);
	}

    for (i = 1; i<=MAX_MQTT_CLIENTS; i++)
    {
		printf("[Marcus debug] %s:%d Checking MQTT with instance %d.\n", __func__, __LINE__, i);
		mp = FindMqttParamsByInstance(i);
		if(mp != NULL){
#if 0
			printf("Enable:%s\n", mp->enable?"True":"False");
			printf("host %s, bind %s, port %d.\n", mp->host, mp->bind_address, mp->port);
			printf("Topic %s\n", mp->topic);
			printf("================\n");
#endif
			device_Update_MQTT_ClientConnection_Parameters(i, valueChangeHandle);
			device_MQTTOperation(i, valueChangeHandle);
		}
	}
	
#endif
#if 0//Jessie SyncTrustCert()
    SyncTrustCert(false);
#endif
	//cwmp_monitor_cwmp();
	return USP_ERR_OK;
}
//----------Port zr69cmd applynow end

#ifndef OVERRIDE_MAIN
/*********************************************************************//**
**
** main
**
** Main function
**
** \param   argc - Number of command line arguments
** \param   argv - Array of pointers to command line argument strings
**
** \return  -1 (if this function ever returns, then it will be because of an error)
**
**************************************************************************/
int main(int argc, char *argv[])
{
    int err;
    int c;
    int option_index = 0;
    char *db_file = DEFAULT_DATABASE_FILE;
    bool enable_mem_info = false;
	int retzcfg = ZCFG_SUCCESS;
	objIndex_t localagentIid;
	objIndex_t      ipIfaceIid;
	rdm_LocalAgent_t *localagentObj = NULL;
	rdm_IpIface_t   *ipIfaceObj = NULL;
	int boundInterNum = 1, ret = ZCFG_SUCCESS;
	char boundInterName[32] = {0};
	#if 1//Remove Device.LocalAgent.Enable
	objIndex_t      agentMtpIid;
	rdm_DevLocalAgentMTP_t *agentMtpObj = NULL;
	int enableLocalAgent;
	#endif

    // Determine a handle for the data model thread (this thread)
    OS_UTILS_SetDataModelThread();

    // Exit if unable to initialise basic subsystems
    USP_LOG_Init();
    USP_ERR_Init();
    err = USP_MEM_Init();
    if (err != USP_ERR_OK)
    {
        return -1;
    }

    // Iterate over all command line options
    while (FOREVER)
    {
        // Parse the next command line option
        c = getopt_long_only(argc, argv, short_options, long_options, &option_index);

        // Exit this loop, if no more options
        if (c == -1)
        {
            break;
        }

        // Determine which option was read this time
        switch (c)
        {
            case 'h':
                PrintUsage(argv[0]);
                exit(0);
                break;

            case 'l':
                // Exit if an error occurred whilst trying to open the log file
                err = USP_LOG_SetFile(optarg);
                if (err != USP_ERR_OK)
                {
                    goto exit;
                }
                break;

            case 'f':
                // Set the location of the database
                db_file = optarg;
                break;

            case 'm':
                // Enable memory info collection
                enable_mem_info = true;
                break;

            case 'e':
                // Enable callstack printing when an error occurs
                enable_callstack_debug = true;
                break;

            case 'p':
                // Enable logging of protocol trace
                enable_protocol_trace = true;
                break;

            case 'a':
                // Set the location of the client certificate file to use
                auth_cert_file = optarg;
                break;

            case 't':
                // Set the location of the file containing trust store certificates
                usp_trust_store_file = optarg;
                break;

            case 'r':
                // Set the location of the text file containing the factory reset parameters
                factory_reset_text_file = optarg;
                break;

            case 'i':
                // Set the networking interface to use for USP communication
                if (nu_ipaddr_is_valid_interface(optarg) != true)
                {
                    usp_log_level = kLogLevel_Error;
                    USP_LOG_Error("ERROR: Network interface '%s' does not exist or has no IP address", optarg);
                    goto exit;
                }
                usp_interface = optarg;
                break;

            case 'v':
                // Verbosity level
                err = TEXT_UTILS_StringToUnsigned(optarg, &usp_log_level);
                if ((err != USP_ERR_OK) || (usp_log_level >= kLogLevel_Max))
                {
                    usp_log_level = kLogLevel_Error;
                    USP_LOG_Error("ERROR: Verbosity level (%s) is invalid or out of range", optarg);
                    goto exit;
                }
                break;

            case 'c':
                // Rest of command line contains a command to send to the active USP Agent
                err = CLI_CLIENT_ExecCommand(argc-optind, &argv[optind], db_file);
                return err;
                break;


            default:
                USP_LOG_Error("ERROR: USP Agent was invoked with the '-%c' option but the code was not compiled in.", c);
                goto exit;
                break;

            case '?':
                usp_log_level = kLogLevel_Error;
                USP_LOG_Error("ERROR: Missing option value");
                goto exit;
                break;
        }
    }

	/*init message server*/
	zcfgEidInit(ZCFG_EID_OBUSPA, "OBUSPA");
	ret = zcfgMsgServerInit();
	if(ret != ZCFG_SUCCESS) {
		/* zcfgLog(ZCFG_LOG_ERR, "obuspa message server init fail\n"); */
		printf("obuspa message server init fail\n");
		goto exit;
	}
	
	
	//Remove arguments
	//read the configuration from data model
	#if 1//Remove Device.LocalAgent.Enable
	enableLocalAgent = 0;
	IID_INIT(agentMtpIid);
	while(zcfgFeObjStructGetNextWithoutUpdate(RDM_OID_DEV_LOCAL_AGENT_M_T_P, &agentMtpIid, (void **)&agentMtpObj) == ZCFG_SUCCESS){
		if(1 == agentMtpObj->Enable){
			enableLocalAgent = 1;
		}
		zcfgFeObjStructFree(agentMtpObj);
	}

	if(0 == enableLocalAgent){
		printf(" obuspa goto exit;");
		goto exit;
	}
	#endif

	IID_INIT(localagentIid);
	if(zcfgFeObjStructGet(RDM_OID_LOCAL_AGENT, &localagentIid, (void **)&localagentObj) == ZCFG_SUCCESS){
		enable_protocol_trace = localagentObj->X_ZYXEL_Prototrace;
		if(localagentObj->X_ZYXEL_Prototrace_level >= 0 && localagentObj->X_ZYXEL_Prototrace_level < kLogLevel_Max){
			usp_log_level = localagentObj->X_ZYXEL_Prototrace_level;
		}

		sscanf(localagentObj->X_ZYXEL_ActiveBoundInterface, "IP.Interface.%d", &boundInterNum);
		//convert the IP.Interface.%d to interface name wwan%d
		IID_INIT(ipIfaceIid);
		while (zcfgFeObjStructGetNext(RDM_OID_IP_IFACE, &ipIfaceIid, (void **)&ipIfaceObj) == ZCFG_SUCCESS)
		{
			if(boundInterNum == ipIfaceIid.idx[0]){
				snprintf(boundInterName, sizeof(boundInterName), "%s", ipIfaceObj->X_ZYXEL_IfName);
				zcfgFeObjStructFree(ipIfaceObj);
				break;
			}
			zcfgFeObjStructFree(ipIfaceObj);
		}
		zcfgFeObjStructFree(localagentObj);
	}

	usp_interface = boundInterName;

    // Print a warning for any remaining command line arguments
    if (optind < argc)
    {
        USP_LOG_Error("WARNING: unknown command line arguments:-");
        while (optind < argc)
        {
            USP_LOG_Error("   %s", argv[optind++]);
        }
    }

    // Following debug is only logged when running as a daemon (not when running as CLI client).
    syslog(LOG_INFO, "USP Agent starting...");

    // Sleep until other services which USP Agent uses (eg DNS) are running
    // (ideally USP Agent should be started when the services are running, rather than sleeping here. But sometimes, there is no easy way to ensure this).
    if (DAEMON_START_DELAY_MS > 0)
    {
        usleep(DAEMON_START_DELAY_MS*1000);
    }


    // Exit if unable to start USP Agent
    err = MAIN_Start(db_file, enable_mem_info);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // Exit if unable to spawn off a thread to service the MTPs
#ifndef DISABLE_STOMP
    err = OS_UTILS_CreateThread(MTP_EXEC_StompMain, NULL);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }
#endif

#ifdef ENABLE_COAP
    err = OS_UTILS_CreateThread(MTP_EXEC_CoapMain, NULL);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }
#endif

#ifdef ENABLE_MQTT
    err = OS_UTILS_CreateThread(MTP_EXEC_MqttMain, NULL);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }
#endif

    // Exit if unable to spawn off a thread to perform bulk data collection posts
    err = OS_UTILS_CreateThread(BDC_EXEC_Main, NULL);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

	//start to listen ztr369cmd
    err = OS_UTILS_CreateThread(tr369_cmd_task, NULL);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }
	//tr369_cmd_task();

    // Run the data model main loop of USP Agent (this function does not return)
    DM_EXEC_Main(NULL);

exit:
    // If the code gets here, an error occurred
    USP_LOG_Error("USP Agent aborted unexpectedly");
    return -1;
}
#endif

/*********************************************************************//**
**
** MAIN_Start
**
** Initializes and starts USP Agent
**
** \param   db_file - pointer to name of USP Agent's database file to open
** \param   enable_mem_info - Set to true if memory debugging info should be collected
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int MAIN_Start(char *db_file, bool enable_mem_info)
{
    CURLcode curl_err;
    int err;

    // Exit if unable to initialise libraries which need to be initialised when running single threaded
    curl_err = curl_global_init(CURL_GLOBAL_ALL);
    if (curl_err != 0)
    {
        USP_LOG_Error("%s: curl_global_init() failed (curl_err=%d)", __FUNCTION__, curl_err);
        return USP_ERR_INTERNAL_ERROR;
    }

    SYNC_TIMER_Init();

    // Turn off SIGPIPE, since we use non-blocking connections and would prefer to get the EPIPE error
    // NOTE: If running USP Agent in GDB: GDB ignores this code and will still generate SIGPIPE
    signal(SIGPIPE, SIG_IGN);


    // Exit if an error occurred when initialising the database
    err = DATABASE_Init(db_file);
    if (err != USP_ERR_OK)
    {
        return err;
    }

    // Exit if an error occurred when initialising any of the the message queues used by the threads
    err = DM_EXEC_Init();
    err |= MTP_EXEC_Init();
    err |= BDC_EXEC_Init();
    if (err != USP_ERR_OK)
    {
        return err;
    }

    // Initialise the random number generator seeds
    RETRY_WAIT_Init();

    // Exit if unable to add all schema paths to the data model
    err = DATA_MODEL_Init();
    if (err != USP_ERR_OK)
    {
        return err;
    }

    // Start logging memory usage from now on (since the static data model schema allocations have completed)
    if (enable_mem_info)
    {
        USP_MEM_StartCollection();
    }

    // Exit if unable to start the datamodel objects
    err = DATA_MODEL_Start();
    if (err != USP_ERR_OK)
    {
        return err;
    }

#if 1//Jessie syncTrustCert()
	SyncTrustCert(true);//to avoid disconnect/connect, so sync trust controller cerfiticates before MQTT client starts
#endif

#ifndef DISABLE_STOMP
    // Start the STOMP connections. This must be done here, before other parts of the data model that require stomp connections
    // to queue messages (eg object creation/deletion notifications)
    err = DEVICE_STOMP_StartAllConnections();
    if (err != USP_ERR_OK)
    {
        return err;
    }
#endif

#ifdef ENABLE_MQTT
    // Start the MQTT connections. This must be done here, before other parts of the data model that require MQTT clients
    // to queue messages (eg object creation/deletion notifications)
    err = DEVICE_MQTT_StartAllClients();
    if (err != USP_ERR_OK)
    {

        return err;
    }
#endif

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** MAIN_Stop
**
** Frees all memory and closes all sockets and file handles
** Called from the MTP thread
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
void MAIN_Stop(void)
{
    // Free all memory used by USP Agent
    DM_EXEC_Destroy();
    curl_global_cleanup();
}

/*********************************************************************//**
**
** PrintUsage
**
** Prints the command line options for this program
**
** \param   prog_name - name of this executable from command line
**
** \return  None
**
**************************************************************************/
void PrintUsage(char *prog_name)
{
    char *p;
    char *name;

    // Strip off any leading directories from the executable path
    p = strrchr(prog_name, '/');
    name = (p == NULL) ? prog_name : &p[1];

    printf("USAGE: %s options\n", name);
    printf("--help (-h)       Displays this help\n");
    printf("--log (-l)        Sets the destination for debug logging. Default is 'stdout'. Can also use 'syslog' or a filename\n");
    printf("--dbfile (-f)     Sets the path of the file to store the database in (default=%s)\n", DEFAULT_DATABASE_FILE);
    printf("--verbose (-v)    Sets the debug verbosity log level: 0=Off, 1=Error(default), 2=Warning, 3=Info\n");
    printf("--prototrace (-p) Enables trace logging of the USP protocol messages\n");
    printf("--authcert (-a)   Sets the path of the PEM formatted file containing a client certificate and private key to authenticate this device with\n");
    printf("--truststore (-t) Sets the path of the PEM formatted file containing trust store certificates\n");
    printf("--resetfile (-r)  Sets the path of the text file containing factory reset parameters\n");
    printf("--interface (-i)  Sets the name of the networking interface to use for USP communication\n");
    printf("--meminfo (-m)    Collects and prints information useful to debugging memory leaks\n");
    printf("--error (-e)      Enables printing of the callstack whenever an error is detected\n");
    printf("--command (-c)    Sends a CLI command to the running USP Agent and prints the response\n");
    printf("                  To get a list of all CLI commands use '-c help'\n");
    printf("\n");
}

