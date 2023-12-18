/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/
/*
 * lwm2mclient.h
 *
 *  General functions of lwm2m test client.
 *
 *  Created on: 22.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2015 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */

#ifndef LWM2MCLIENT_H_
#define LWM2MCLIENT_H_

#include "liblwm2m.h"

extern int g_reboot;


typedef struct _change_value_list_
{
    struct _change_value_list_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    lwm2m_uri_t uri;
} change_value_list_t;

change_value_list_t * changeValueList;

typedef struct resource_ins_list_s
{   // linked list:
    struct resource_ins_list_s* next;  // matches lwm2m_list_t::next
    uint16_t resInstId; // matches lwm2m_list_t::id
    // resource data:
    char value[257];
} resource_ins_list_t;



/*
 * object_device.c
 */
typedef struct _device_instance_
{
    struct _device_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char manufacturer[257];
    char model_number[257];
    char serial_number[257];
    char firmware_version[257];
    // char avl_power_sources[257];
    // char power_source_voltage[257];
    // char power_source_current[257];
    // char battery_level[257];
    char memory_free[257];
    resource_ins_list_t *error_code;
    char current_time[257];
    char utc_offset[257];
    char timezone[257];
    char binding_modes[257];
    // char device_type[257];
    char hardware_version[257];
    char software_version[257];
    // char battery_status[257];
    char memory_total[257];
    // char extdevinfo[257];
} device_instance_t;

lwm2m_object_t * get_object_device(void);
void free_object_device(lwm2m_object_t * objectP);
uint8_t device_change(lwm2m_data_t * dataArray, lwm2m_object_t * objectP);
void display_device_object(lwm2m_object_t * objectP);
/*
 * object_firmware.c
 */
lwm2m_object_t * get_object_firmware(void);
void free_object_firmware(lwm2m_object_t * objectP);
void display_firmware_object(lwm2m_object_t * objectP);
/*
 * object_location.c
 */
typedef struct _location_instance_
{
    struct _location_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char latitude[257];
    char longitude[257];
    char altitude[257];
    char radius[257];
    char velocity[257];
    char timestamp[257];
    char speed[257];
} location_instance_t;

lwm2m_object_t * get_object_location(void);
void free_object_location(lwm2m_object_t * object);
void display_location_object(lwm2m_object_t * objectP);
/*
 * object_test.c
 */
#define TEST_OBJECT_ID 31024
lwm2m_object_t * get_test_object(void);
void free_test_object(lwm2m_object_t * object);
void display_test_object(lwm2m_object_t * objectP);
/*
 * object_server.c
 */
typedef struct _server_instance_
{
    struct _server_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    uint16_t    shortServerId;
    uint32_t    lifetime;
    uint32_t    defaultMinPeriod;
    uint32_t    defaultMaxPeriod;
    uint32_t    disableTimeout;
    bool        storing;
    char        binding[4];
#ifndef LWM2M_VERSION_1_0
    int         registrationPriorityOrder; // <0 when it doesn't exist
    int         initialRegistrationDelayTimer; // <0 when it doesn't exist
    int8_t      registrationFailureBlock; // <0 when it doesn't exist, 0 for false, > 0 for true
    int8_t      bootstrapOnRegistrationFailure; // <0 when it doesn't exist, 0 for false, > 0 for true
    int         communicationRetryCount; // <0 when it doesn't exist
    int         communicationRetryTimer; // <0 when it doesn't exist
    int         communicationSequenceDelayTimer; // <0 when it doesn't exist
    int         communicationSequenceRetryCount; // <0 when it doesn't exist
#endif

    //observe save data
    char observe_data_shortServerId[257];
    char observe_data_lifetime[257];
    char observe_data_defaultMinPeriod[257];
    char observe_data_defaultMaxPeriod[257];
    char observe_data_disableTimeout[257];
    char observe_data_storing[257];
    char observe_data_binding[257];
#ifndef LWM2M_VERSION_1_0
    char observe_data_registrationPriorityOrder[257];
    char observe_data_initialRegistrationDelayTimer[257];
    char observe_data_registrationFailureBlock[257];
    char observe_data_bootstrapOnRegistrationFailure[257];
    char observe_data_communicationRetryCount[257];
    char observe_data_communicationRetryTimer[257];
    char observe_data_communicationSequenceDelayTimer[257];
    char observe_data_communicationSequenceRetryCount[257];
#endif
} server_instance_t;

lwm2m_object_t * get_server_object(int serverId, const char* binding, int lifetime, bool storing);
void clean_server_object(lwm2m_object_t * object);
void display_server_object(lwm2m_object_t * objectP);
void copy_server_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc);

/*
 * object_connectivity_moni.c
 */
typedef struct _conn_moni_instance_
{
    struct _conn_moni_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char network_bearer[257];
    resource_ins_list_t *avl_network_bearer;
    char radio_signal_strength[257];
    char link_quality[257];
    resource_ins_list_t *ip_addresses;
    resource_ins_list_t *router_ip_address;
    char link_utilization[257];
    resource_ins_list_t *apn;
    char cell_id[257];
    char smnc[257];
    char smcc[257];
} conn_moni_instance_t;

lwm2m_object_t * get_object_conn_m(void);
void free_object_conn_m(lwm2m_object_t * objectP);
uint8_t connectivity_moni_change(lwm2m_data_t * dataArray, lwm2m_object_t * objectP);

/*
 * object_connectivity_stat.c
 */
typedef struct _conn_stat_instance_
{
    struct _conn_stat_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
	char sms_tx_counter[257];
	char sms_rx_counter[257];
	char tx_data[257];
	char rx_data[257];
	char max_message_size[257];
	char average_message_size[257];
	char collection_period[257];
} conn_stat_instance_t;

extern lwm2m_object_t * get_object_conn_s(void);
void free_object_conn_s(lwm2m_object_t * objectP);
extern void conn_s_updateTxStatistic(lwm2m_object_t * objectP, uint16_t txDataByte, bool smsBased);
extern void conn_s_updateRxStatistic(lwm2m_object_t * objectP, uint16_t rxDataByte, bool smsBased);

/*
 * object_access_control.c
 */
lwm2m_object_t* acc_ctrl_create_object(void);
void acl_ctrl_free_object(lwm2m_object_t * objectP);
bool  acc_ctrl_obj_add_inst (lwm2m_object_t* accCtrlObjP, uint16_t instId,
                 uint16_t acObjectId, uint16_t acObjInstId, uint16_t acOwner);
bool  acc_ctrl_oi_add_ac_val(lwm2m_object_t* accCtrlObjP, uint16_t instId,
                 uint16_t aclResId, uint16_t acValue);
/*
 * lwm2mclient.c
 */
void handle_value_changed(lwm2m_context_t* lwm2mH, lwm2m_uri_t* uri, const char * value, size_t valueLength);
/*
 * system_api.c
 */
void init_value_change(lwm2m_context_t * lwm2m);
void system_reboot(void);

/*
 * object_security.c
 */
lwm2m_object_t * get_security_object(int serverId, const char* serverUri, char * bsPskId, char * psk, uint16_t pskLen, bool isBootstrap);
void clean_security_object(lwm2m_object_t * objectP);
char * get_server_uri(lwm2m_object_t * objectP, uint16_t secObjInstID);
void display_security_object(lwm2m_object_t * objectP);
void copy_security_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc);

/*
 * object_cell_connectivity.c
 */
typedef struct _cell_conn_instance_
{
    struct _cell_conn_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char smsc_addr[257];
    char disable_radio_period[257];
    char module_activation_code[257];
    char vendor_specific_extensions[257];
    char psm_timer[257];
    char active_timer[257];
    // char edrx_param_4_iu_mode[257];
    // char edrx_param_4_wbs1_mode[257];
    // char edrx_param_4_nbs1_mode[257];
    // char edrx_param_4_agb_mode[257];
    resource_ins_list_t * activated_profile_names;
    char sup_power_saving_modes[257];
    char active_power_saving_modes[257];
    char release_assistance_indication_usage[257];
} cell_conn_instance_t;

lwm2m_object_t * get_object_cell_conn(void);
void free_object_cell_conn(lwm2m_object_t * objectP);
void display_cell_conn_object(lwm2m_object_t *object);

/*
 * object_apn_conn_profile.c
 */
typedef struct _apn_conn_profile_instance_
{
    struct _apn_conn_profile_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char profile_name[257];
    char apn[257];
    char auto_select_apn[257];
    char enable_status[257];
    char authen_type[257];
    char user_name[257];
    char secret[257];
    char reconnect_schedule[257];
    resource_ins_list_t *validity_mcc_mnc;
    resource_ins_list_t *connection_establish_time;
    resource_ins_list_t *connection_establish_result;
    resource_ins_list_t *connection_establish_reject_cause;
    resource_ins_list_t *connection_end_time;
    char total_bytes_sent[257];
    char total_bytes_received[257];
    resource_ins_list_t *ip_addr;
    resource_ins_list_t *prefix_len;
    resource_ins_list_t *subnet_mask;
    resource_ins_list_t *gateway;
    resource_ins_list_t *pri_dns_addr;
    resource_ins_list_t *sec_dns_addr;
    char qci[257];
    char vendor_specific_extensions[257];
    char total_packets_sent[257];
    char pdn_type[257];
    char apn_rate_ctl[257];
    char serving_plmn_rate_ctl[257];
    char uplink_time_unit[257];
    char apn_rate_ctl_4_exception_data[257];
    char apn_exception_data_uplink_time_unit[257];
    resource_ins_list_t *sup_rat_types;
    resource_ins_list_t *rds_app_id;
    resource_ins_list_t *rds_destination_port;
    resource_ins_list_t *rds_src_port;
} apn_conn_profile_instance_t;

lwm2m_object_t * get_object_apn_conn(void);
void free_object_apn_conn(lwm2m_object_t * objectP);
void display_apn_conn_object(lwm2m_object_t *object);

/*
 * object_poweruplog.c
 */
typedef struct _pwr_up_log_instance_
{
    struct _pwr_up_log_instance_ * next;   // matches lwm2m_list_t::next
    uint16_t    instanceId;            // matches lwm2m_list_t::id
    char devicename[257];
    char tool_version[257];
    char imei[257];
    char imsi[257];
    char msisdn[257];
} pwr_up_log_instance_t;

lwm2m_object_t * get_object_poweruplog(void);
void free_object_poweruplog(lwm2m_object_t * objectP);
void display_poweruplog_object(lwm2m_object_t *object);

#endif /* LWM2MCLIENT_H_ */
