
/* GENERATED CODE... DO NOT EDIT */

/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2012 Lanedo GmbH
 */


#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "qmi-enums.h"
#include "qmi-enums-wds.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_WDS__
#define __LIBQMI_GLIB_QMI_WDS__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWdsResetOutput:
 *
 * The #QmiMessageWdsResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsResetOutput QmiMessageWdsResetOutput;
GType qmi_message_wds_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_RESET_OUTPUT (qmi_message_wds_reset_output_get_type ())

gboolean qmi_message_wds_reset_output_get_result (
    QmiMessageWdsResetOutput *self,
    GError **error);

QmiMessageWdsResetOutput *qmi_message_wds_reset_output_ref (QmiMessageWdsResetOutput *self);
void qmi_message_wds_reset_output_unref (QmiMessageWdsResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Abort */


/* --- Input -- */

/* --- Output -- */

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Start Network */


/* --- Input -- */

/**
 * QmiMessageWdsStartNetworkInput:
 *
 * The #QmiMessageWdsStartNetworkInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsStartNetworkInput QmiMessageWdsStartNetworkInput;
GType qmi_message_wds_start_network_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_START_NETWORK_INPUT (qmi_message_wds_start_network_input_get_type ())

gboolean qmi_message_wds_start_network_input_get_call_type (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsCallType *call_type,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_call_type (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsCallType call_type,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_extended_technology_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsExtendedTechnologyPreference *extended_technology_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_extended_technology_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsExtendedTechnologyPreference extended_technology_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_enable_autoconnect (
    QmiMessageWdsStartNetworkInput *self,
    gboolean *enable_autoconnect,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_enable_autoconnect (
    QmiMessageWdsStartNetworkInput *self,
    gboolean enable_autoconnect,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_profile_index_3gpp2 (
    QmiMessageWdsStartNetworkInput *self,
    guint8 *profile_index_3gpp2,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_profile_index_3gpp2 (
    QmiMessageWdsStartNetworkInput *self,
    guint8 profile_index_3gpp2,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_profile_index_3gpp (
    QmiMessageWdsStartNetworkInput *self,
    guint8 *profile_index_3gpp,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_profile_index_3gpp (
    QmiMessageWdsStartNetworkInput *self,
    guint8 profile_index_3gpp,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_technology_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsTechnologyPreference *technology_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_technology_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsTechnologyPreference technology_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_ip_family_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsIpFamily *ip_family_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_ip_family_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsIpFamily ip_family_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_password (
    QmiMessageWdsStartNetworkInput *self,
    const gchar **password,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_password (
    QmiMessageWdsStartNetworkInput *self,
    const gchar *password,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_username (
    QmiMessageWdsStartNetworkInput *self,
    const gchar **username,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_username (
    QmiMessageWdsStartNetworkInput *self,
    const gchar *username,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_authentication_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsAuthentication *authentication_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_authentication_preference (
    QmiMessageWdsStartNetworkInput *self,
    QmiWdsAuthentication authentication_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_ipv4_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 *ipv4_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_ipv4_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 ipv4_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_apn (
    QmiMessageWdsStartNetworkInput *self,
    const gchar **apn,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_apn (
    QmiMessageWdsStartNetworkInput *self,
    const gchar *apn,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_secondary_nbns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 *secondary_nbns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_secondary_nbns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 secondary_nbns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_primary_nbns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 *primary_nbns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_primary_nbns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 primary_nbns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_secondary_dns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 *secondary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_secondary_dns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 secondary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_get_primary_dns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 *primary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_start_network_input_set_primary_dns_address_preference (
    QmiMessageWdsStartNetworkInput *self,
    guint32 primary_dns_address_preference,
    GError **error);

QmiMessageWdsStartNetworkInput *qmi_message_wds_start_network_input_ref (QmiMessageWdsStartNetworkInput *self);
void qmi_message_wds_start_network_input_unref (QmiMessageWdsStartNetworkInput *self);
QmiMessageWdsStartNetworkInput *qmi_message_wds_start_network_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsStartNetworkOutput:
 *
 * The #QmiMessageWdsStartNetworkOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsStartNetworkOutput QmiMessageWdsStartNetworkOutput;
GType qmi_message_wds_start_network_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_START_NETWORK_OUTPUT (qmi_message_wds_start_network_output_get_type ())

gboolean qmi_message_wds_start_network_output_get_result (
    QmiMessageWdsStartNetworkOutput *self,
    GError **error);

gboolean qmi_message_wds_start_network_output_get_packet_data_handle (
    QmiMessageWdsStartNetworkOutput *self,
    guint32 *packet_data_handle,
    GError **error);

gboolean qmi_message_wds_start_network_output_get_call_end_reason (
    QmiMessageWdsStartNetworkOutput *self,
    QmiWdsCallEndReason *call_end_reason,
    GError **error);

gboolean qmi_message_wds_start_network_output_get_verbose_call_end_reason (
    QmiMessageWdsStartNetworkOutput *self,
    QmiWdsVerboseCallEndReasonType *verbose_call_end_reason_type,
    gint16 *verbose_call_end_reason_reason,
    GError **error);

QmiMessageWdsStartNetworkOutput *qmi_message_wds_start_network_output_ref (QmiMessageWdsStartNetworkOutput *self);
void qmi_message_wds_start_network_output_unref (QmiMessageWdsStartNetworkOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Stop Network */


/* --- Input -- */

/**
 * QmiMessageWdsStopNetworkInput:
 *
 * The #QmiMessageWdsStopNetworkInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsStopNetworkInput QmiMessageWdsStopNetworkInput;
GType qmi_message_wds_stop_network_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_STOP_NETWORK_INPUT (qmi_message_wds_stop_network_input_get_type ())

gboolean qmi_message_wds_stop_network_input_get_disable_autoconnect (
    QmiMessageWdsStopNetworkInput *self,
    gboolean *disable_autoconnect,
    GError **error);

gboolean qmi_message_wds_stop_network_input_set_disable_autoconnect (
    QmiMessageWdsStopNetworkInput *self,
    gboolean disable_autoconnect,
    GError **error);

gboolean qmi_message_wds_stop_network_input_get_packet_data_handle (
    QmiMessageWdsStopNetworkInput *self,
    guint32 *packet_data_handle,
    GError **error);

gboolean qmi_message_wds_stop_network_input_set_packet_data_handle (
    QmiMessageWdsStopNetworkInput *self,
    guint32 packet_data_handle,
    GError **error);

QmiMessageWdsStopNetworkInput *qmi_message_wds_stop_network_input_ref (QmiMessageWdsStopNetworkInput *self);
void qmi_message_wds_stop_network_input_unref (QmiMessageWdsStopNetworkInput *self);
QmiMessageWdsStopNetworkInput *qmi_message_wds_stop_network_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsStopNetworkOutput:
 *
 * The #QmiMessageWdsStopNetworkOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsStopNetworkOutput QmiMessageWdsStopNetworkOutput;
GType qmi_message_wds_stop_network_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_STOP_NETWORK_OUTPUT (qmi_message_wds_stop_network_output_get_type ())

gboolean qmi_message_wds_stop_network_output_get_result (
    QmiMessageWdsStopNetworkOutput *self,
    GError **error);

QmiMessageWdsStopNetworkOutput *qmi_message_wds_stop_network_output_ref (QmiMessageWdsStopNetworkOutput *self);
void qmi_message_wds_stop_network_output_unref (QmiMessageWdsStopNetworkOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Packet Service Status */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWdsGetPacketServiceStatusOutput:
 *
 * The #QmiMessageWdsGetPacketServiceStatusOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetPacketServiceStatusOutput QmiMessageWdsGetPacketServiceStatusOutput;
GType qmi_message_wds_get_packet_service_status_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PACKET_SERVICE_STATUS_OUTPUT (qmi_message_wds_get_packet_service_status_output_get_type ())

gboolean qmi_message_wds_get_packet_service_status_output_get_result (
    QmiMessageWdsGetPacketServiceStatusOutput *self,
    GError **error);

gboolean qmi_message_wds_get_packet_service_status_output_get_connection_status (
    QmiMessageWdsGetPacketServiceStatusOutput *self,
    QmiWdsConnectionStatus *connection_status,
    GError **error);

QmiMessageWdsGetPacketServiceStatusOutput *qmi_message_wds_get_packet_service_status_output_ref (QmiMessageWdsGetPacketServiceStatusOutput *self);
void qmi_message_wds_get_packet_service_status_output_unref (QmiMessageWdsGetPacketServiceStatusOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Packet Statistics */


/* --- Input -- */

/**
 * QmiMessageWdsGetPacketStatisticsInput:
 *
 * The #QmiMessageWdsGetPacketStatisticsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetPacketStatisticsInput QmiMessageWdsGetPacketStatisticsInput;
GType qmi_message_wds_get_packet_statistics_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PACKET_STATISTICS_INPUT (qmi_message_wds_get_packet_statistics_input_get_type ())

gboolean qmi_message_wds_get_packet_statistics_input_get_mask (
    QmiMessageWdsGetPacketStatisticsInput *self,
    QmiWdsPacketStatisticsMaskFlag *mask,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_input_set_mask (
    QmiMessageWdsGetPacketStatisticsInput *self,
    QmiWdsPacketStatisticsMaskFlag mask,
    GError **error);

QmiMessageWdsGetPacketStatisticsInput *qmi_message_wds_get_packet_statistics_input_ref (QmiMessageWdsGetPacketStatisticsInput *self);
void qmi_message_wds_get_packet_statistics_input_unref (QmiMessageWdsGetPacketStatisticsInput *self);
QmiMessageWdsGetPacketStatisticsInput *qmi_message_wds_get_packet_statistics_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsGetPacketStatisticsOutput:
 *
 * The #QmiMessageWdsGetPacketStatisticsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetPacketStatisticsOutput QmiMessageWdsGetPacketStatisticsOutput;
GType qmi_message_wds_get_packet_statistics_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PACKET_STATISTICS_OUTPUT (qmi_message_wds_get_packet_statistics_output_get_type ())

gboolean qmi_message_wds_get_packet_statistics_output_get_result (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_tx_packets_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *tx_packets_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_rx_packets_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *rx_packets_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_tx_packets_error (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *tx_packets_error,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_rx_packets_error (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *rx_packets_error,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_tx_overflows (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *tx_overflows,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_rx_overflows (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *rx_overflows,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_tx_bytes_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint64 *tx_bytes_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_rx_bytes_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint64 *rx_bytes_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_last_call_tx_bytes_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint64 *last_call_tx_bytes_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_last_call_rx_bytes_ok (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint64 *last_call_rx_bytes_ok,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_tx_packets_dropped (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *tx_packets_dropped,
    GError **error);

gboolean qmi_message_wds_get_packet_statistics_output_get_rx_packets_dropped (
    QmiMessageWdsGetPacketStatisticsOutput *self,
    guint32 *rx_packets_dropped,
    GError **error);

QmiMessageWdsGetPacketStatisticsOutput *qmi_message_wds_get_packet_statistics_output_ref (QmiMessageWdsGetPacketStatisticsOutput *self);
void qmi_message_wds_get_packet_statistics_output_unref (QmiMessageWdsGetPacketStatisticsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Profile List */


/* --- Input -- */

/**
 * QmiMessageWdsGetProfileListInput:
 *
 * The #QmiMessageWdsGetProfileListInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetProfileListInput QmiMessageWdsGetProfileListInput;
GType qmi_message_wds_get_profile_list_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PROFILE_LIST_INPUT (qmi_message_wds_get_profile_list_input_get_type ())

gboolean qmi_message_wds_get_profile_list_input_get_profile_type (
    QmiMessageWdsGetProfileListInput *self,
    QmiWdsProfileType *profile_type,
    GError **error);

gboolean qmi_message_wds_get_profile_list_input_set_profile_type (
    QmiMessageWdsGetProfileListInput *self,
    QmiWdsProfileType profile_type,
    GError **error);

QmiMessageWdsGetProfileListInput *qmi_message_wds_get_profile_list_input_ref (QmiMessageWdsGetProfileListInput *self);
void qmi_message_wds_get_profile_list_input_unref (QmiMessageWdsGetProfileListInput *self);
QmiMessageWdsGetProfileListInput *qmi_message_wds_get_profile_list_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsGetProfileListOutputProfileListProfile:
 * @profile_type: a #QmiWdsProfileType.
 * @profile_index: a #guint8.
 * @profile_name: a string.
 *
 * A QmiMessageWdsGetProfileListOutputProfileListProfile struct.
 */
typedef struct _QmiMessageWdsGetProfileListOutputProfileListProfile {
    QmiWdsProfileType profile_type;
    guint8 profile_index;
    gchar * profile_name;
} QmiMessageWdsGetProfileListOutputProfileListProfile;

/**
 * QmiMessageWdsGetProfileListOutput:
 *
 * The #QmiMessageWdsGetProfileListOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetProfileListOutput QmiMessageWdsGetProfileListOutput;
GType qmi_message_wds_get_profile_list_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PROFILE_LIST_OUTPUT (qmi_message_wds_get_profile_list_output_get_type ())

gboolean qmi_message_wds_get_profile_list_output_get_result (
    QmiMessageWdsGetProfileListOutput *self,
    GError **error);

gboolean qmi_message_wds_get_profile_list_output_get_profile_list (
    QmiMessageWdsGetProfileListOutput *self,
    GArray **profile_list,
    GError **error);

gboolean qmi_message_wds_get_profile_list_output_get_extended_error_code (
    QmiMessageWdsGetProfileListOutput *self,
    QmiWdsDsProfileError *extended_error_code,
    GError **error);

QmiMessageWdsGetProfileListOutput *qmi_message_wds_get_profile_list_output_ref (QmiMessageWdsGetProfileListOutput *self);
void qmi_message_wds_get_profile_list_output_unref (QmiMessageWdsGetProfileListOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Profile Settings */


/* --- Input -- */

/**
 * QmiMessageWdsGetProfileSettingsInput:
 *
 * The #QmiMessageWdsGetProfileSettingsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetProfileSettingsInput QmiMessageWdsGetProfileSettingsInput;
GType qmi_message_wds_get_profile_settings_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PROFILE_SETTINGS_INPUT (qmi_message_wds_get_profile_settings_input_get_type ())

gboolean qmi_message_wds_get_profile_settings_input_get_profile_id (
    QmiMessageWdsGetProfileSettingsInput *self,
    QmiWdsProfileType *profile_id_profile_type,
    guint8 *profile_id_profile_index,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_input_set_profile_id (
    QmiMessageWdsGetProfileSettingsInput *self,
    QmiWdsProfileType profile_id_profile_type,
    guint8 profile_id_profile_index,
    GError **error);

QmiMessageWdsGetProfileSettingsInput *qmi_message_wds_get_profile_settings_input_ref (QmiMessageWdsGetProfileSettingsInput *self);
void qmi_message_wds_get_profile_settings_input_unref (QmiMessageWdsGetProfileSettingsInput *self);
QmiMessageWdsGetProfileSettingsInput *qmi_message_wds_get_profile_settings_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsGetProfileSettingsOutput:
 *
 * The #QmiMessageWdsGetProfileSettingsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetProfileSettingsOutput QmiMessageWdsGetProfileSettingsOutput;
GType qmi_message_wds_get_profile_settings_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_PROFILE_SETTINGS_OUTPUT (qmi_message_wds_get_profile_settings_output_get_type ())

gboolean qmi_message_wds_get_profile_settings_output_get_result (
    QmiMessageWdsGetProfileSettingsOutput *self,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_ipv6_address_preference (
    QmiMessageWdsGetProfileSettingsOutput *self,
    GArray **ipv6_address_preference_address,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_profile_name (
    QmiMessageWdsGetProfileSettingsOutput *self,
    const gchar **profile_name,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_pdp_type (
    QmiMessageWdsGetProfileSettingsOutput *self,
    QmiWdsPdpType *pdp_type,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_apn_name (
    QmiMessageWdsGetProfileSettingsOutput *self,
    const gchar **apn_name,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_primary_ipv4_dns_address (
    QmiMessageWdsGetProfileSettingsOutput *self,
    guint32 *primary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_secondary_ipv4_dns_address (
    QmiMessageWdsGetProfileSettingsOutput *self,
    guint32 *secondary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_gprs_requested_qos (
    QmiMessageWdsGetProfileSettingsOutput *self,
    guint32 *gprs_requested_qos_precedence_class,
    guint32 *gprs_requested_qos_delay_class,
    guint32 *gprs_requested_qos_reliability_class,
    guint32 *gprs_requested_qos_peak_throughput_class,
    guint32 *gprs_requested_qos_mean_throughput_class,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_gprs_minimum_qos (
    QmiMessageWdsGetProfileSettingsOutput *self,
    guint32 *gprs_minimum_qos_precedence_class,
    guint32 *gprs_minimum_qos_delay_class,
    guint32 *gprs_minimum_qos_reliability_class,
    guint32 *gprs_minimum_qos_peak_throughput_class,
    guint32 *gprs_minimum_qos_mean_throughput_class,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_username (
    QmiMessageWdsGetProfileSettingsOutput *self,
    const gchar **username,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_password (
    QmiMessageWdsGetProfileSettingsOutput *self,
    const gchar **password,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_authentication (
    QmiMessageWdsGetProfileSettingsOutput *self,
    QmiWdsAuthentication *authentication,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_ipv4_address_preference (
    QmiMessageWdsGetProfileSettingsOutput *self,
    guint32 *ipv4_address_preference,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_pcscf_address_using_pco (
    QmiMessageWdsGetProfileSettingsOutput *self,
    gboolean *pcscf_address_using_pco,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_pcscf_address_using_dhcp (
    QmiMessageWdsGetProfileSettingsOutput *self,
    gboolean *pcscf_address_using_dhcp,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_imcn_flag (
    QmiMessageWdsGetProfileSettingsOutput *self,
    gboolean *imcn_flag,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_ipv6_primary_dns_address_preference (
    QmiMessageWdsGetProfileSettingsOutput *self,
    GArray **ipv6_primary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_ipv6_secondary_dns_address_preference (
    QmiMessageWdsGetProfileSettingsOutput *self,
    GArray **ipv6_secondary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_get_profile_settings_output_get_extended_error_code (
    QmiMessageWdsGetProfileSettingsOutput *self,
    QmiWdsDsProfileError *extended_error_code,
    GError **error);

QmiMessageWdsGetProfileSettingsOutput *qmi_message_wds_get_profile_settings_output_ref (QmiMessageWdsGetProfileSettingsOutput *self);
void qmi_message_wds_get_profile_settings_output_unref (QmiMessageWdsGetProfileSettingsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Default Settings */


/* --- Input -- */

/**
 * QmiMessageWdsGetDefaultSettingsInput:
 *
 * The #QmiMessageWdsGetDefaultSettingsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetDefaultSettingsInput QmiMessageWdsGetDefaultSettingsInput;
GType qmi_message_wds_get_default_settings_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_DEFAULT_SETTINGS_INPUT (qmi_message_wds_get_default_settings_input_get_type ())

gboolean qmi_message_wds_get_default_settings_input_get_profile_type (
    QmiMessageWdsGetDefaultSettingsInput *self,
    QmiWdsProfileType *profile_type,
    GError **error);

gboolean qmi_message_wds_get_default_settings_input_set_profile_type (
    QmiMessageWdsGetDefaultSettingsInput *self,
    QmiWdsProfileType profile_type,
    GError **error);

QmiMessageWdsGetDefaultSettingsInput *qmi_message_wds_get_default_settings_input_ref (QmiMessageWdsGetDefaultSettingsInput *self);
void qmi_message_wds_get_default_settings_input_unref (QmiMessageWdsGetDefaultSettingsInput *self);
QmiMessageWdsGetDefaultSettingsInput *qmi_message_wds_get_default_settings_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsGetDefaultSettingsOutput:
 *
 * The #QmiMessageWdsGetDefaultSettingsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetDefaultSettingsOutput QmiMessageWdsGetDefaultSettingsOutput;
GType qmi_message_wds_get_default_settings_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_DEFAULT_SETTINGS_OUTPUT (qmi_message_wds_get_default_settings_output_get_type ())

gboolean qmi_message_wds_get_default_settings_output_get_result (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_ipv6_address_preference (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    GArray **ipv6_address_preference_address,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_profile_name (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    const gchar **profile_name,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_pdp_type (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    QmiWdsPdpType *pdp_type,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_apn_name (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    const gchar **apn_name,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_primary_ipv4_dns_address (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    guint32 *primary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_secondary_ipv4_dns_address (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    guint32 *secondary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_gprs_requested_qos (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    guint32 *gprs_requested_qos_precedence_class,
    guint32 *gprs_requested_qos_delay_class,
    guint32 *gprs_requested_qos_reliability_class,
    guint32 *gprs_requested_qos_peak_throughput_class,
    guint32 *gprs_requested_qos_mean_throughput_class,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_gprs_minimum_qos (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    guint32 *gprs_minimum_qos_precedence_class,
    guint32 *gprs_minimum_qos_delay_class,
    guint32 *gprs_minimum_qos_reliability_class,
    guint32 *gprs_minimum_qos_peak_throughput_class,
    guint32 *gprs_minimum_qos_mean_throughput_class,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_username (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    const gchar **username,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_password (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    const gchar **password,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_authentication (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    QmiWdsAuthentication *authentication,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_ipv4_address_preference (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    guint32 *ipv4_address_preference,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_pcscf_address_using_pco (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    gboolean *pcscf_address_using_pco,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_pcscf_address_using_dhcp (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    gboolean *pcscf_address_using_dhcp,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_imcn_flag (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    gboolean *imcn_flag,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_ipv6_primary_dns_address_preference (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    GArray **ipv6_primary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_ipv6_secondary_dns_address_preference (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    GArray **ipv6_secondary_dns_address_preference,
    GError **error);

gboolean qmi_message_wds_get_default_settings_output_get_extended_error_code (
    QmiMessageWdsGetDefaultSettingsOutput *self,
    QmiWdsDsProfileError *extended_error_code,
    GError **error);

QmiMessageWdsGetDefaultSettingsOutput *qmi_message_wds_get_default_settings_output_ref (QmiMessageWdsGetDefaultSettingsOutput *self);
void qmi_message_wds_get_default_settings_output_unref (QmiMessageWdsGetDefaultSettingsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Current Settings */


/* --- Input -- */

/**
 * QmiMessageWdsGetCurrentSettingsInput:
 *
 * The #QmiMessageWdsGetCurrentSettingsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetCurrentSettingsInput QmiMessageWdsGetCurrentSettingsInput;
GType qmi_message_wds_get_current_settings_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_CURRENT_SETTINGS_INPUT (qmi_message_wds_get_current_settings_input_get_type ())

gboolean qmi_message_wds_get_current_settings_input_get_requested_settings (
    QmiMessageWdsGetCurrentSettingsInput *self,
    QmiWdsGetCurrentSettingsRequestedSettings *requested_settings,
    GError **error);

gboolean qmi_message_wds_get_current_settings_input_set_requested_settings (
    QmiMessageWdsGetCurrentSettingsInput *self,
    QmiWdsGetCurrentSettingsRequestedSettings requested_settings,
    GError **error);

QmiMessageWdsGetCurrentSettingsInput *qmi_message_wds_get_current_settings_input_ref (QmiMessageWdsGetCurrentSettingsInput *self);
void qmi_message_wds_get_current_settings_input_unref (QmiMessageWdsGetCurrentSettingsInput *self);
QmiMessageWdsGetCurrentSettingsInput *qmi_message_wds_get_current_settings_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsGetCurrentSettingsOutput:
 *
 * The #QmiMessageWdsGetCurrentSettingsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetCurrentSettingsOutput QmiMessageWdsGetCurrentSettingsOutput;
GType qmi_message_wds_get_current_settings_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_CURRENT_SETTINGS_OUTPUT (qmi_message_wds_get_current_settings_output_get_type ())

gboolean qmi_message_wds_get_current_settings_output_get_result (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_profile_name (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    const gchar **profile_name,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_pdp_type (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    QmiWdsPdpType *pdp_type,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_apn_name (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    const gchar **apn_name,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_primary_ipv4_dns_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *primary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_secondary_ipv4_dns_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *secondary_ipv4_dns_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_gprs_granted_qos (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *gprs_granted_qos_precedence_class,
    guint32 *gprs_granted_qos_delay_class,
    guint32 *gprs_granted_qos_reliability_class,
    guint32 *gprs_granted_qos_peak_throughput_class,
    guint32 *gprs_granted_qos_mean_throughput_class,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_username (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    const gchar **username,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_authentication (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    QmiWdsAuthentication *authentication,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv4_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *ipv4_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_profile_id (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    QmiWdsProfileType *profile_id_profile_type,
    guint8 *profile_id_profile_index,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv4_gateway_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *ipv4_gateway_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv4_gateway_subnet_mask (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *ipv4_gateway_subnet_mask,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_pcscf_address_using_pco (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint8 *pcscf_address_using_pco,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_pcscf_server_address_list (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **pcscf_server_address_list,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_pcscf_domain_name_list (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **pcscf_domain_name_list,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv6_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **ipv6_address_address,
    guint8 *ipv6_address_prefix_length,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv6_gateway_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **ipv6_gateway_address_address,
    guint8 *ipv6_gateway_address_prefix_length,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv6_primary_dns_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **ipv6_primary_dns_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ipv6_secondary_dns_address (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **ipv6_secondary_dns_address,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_mtu (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    guint32 *mtu,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_domain_name_list (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    GArray **domain_name_list,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_ip_family (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    QmiWdsIpFamily *ip_family,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_imcn_flag (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    gint8 *imcn_flag,
    GError **error);

gboolean qmi_message_wds_get_current_settings_output_get_extended_technology_preference (
    QmiMessageWdsGetCurrentSettingsOutput *self,
    QmiWdsExtendedTechnologyPreference *extended_technology_preference,
    GError **error);

QmiMessageWdsGetCurrentSettingsOutput *qmi_message_wds_get_current_settings_output_ref (QmiMessageWdsGetCurrentSettingsOutput *self);
void qmi_message_wds_get_current_settings_output_unref (QmiMessageWdsGetCurrentSettingsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Data Bearer Technology */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWdsGetDataBearerTechnologyOutput:
 *
 * The #QmiMessageWdsGetDataBearerTechnologyOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetDataBearerTechnologyOutput QmiMessageWdsGetDataBearerTechnologyOutput;
GType qmi_message_wds_get_data_bearer_technology_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_DATA_BEARER_TECHNOLOGY_OUTPUT (qmi_message_wds_get_data_bearer_technology_output_get_type ())

gboolean qmi_message_wds_get_data_bearer_technology_output_get_result (
    QmiMessageWdsGetDataBearerTechnologyOutput *self,
    GError **error);

gboolean qmi_message_wds_get_data_bearer_technology_output_get_current (
    QmiMessageWdsGetDataBearerTechnologyOutput *self,
    QmiWdsDataBearerTechnology *current,
    GError **error);

gboolean qmi_message_wds_get_data_bearer_technology_output_get_last (
    QmiMessageWdsGetDataBearerTechnologyOutput *self,
    QmiWdsDataBearerTechnology *last,
    GError **error);

QmiMessageWdsGetDataBearerTechnologyOutput *qmi_message_wds_get_data_bearer_technology_output_ref (QmiMessageWdsGetDataBearerTechnologyOutput *self);
void qmi_message_wds_get_data_bearer_technology_output_unref (QmiMessageWdsGetDataBearerTechnologyOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Get Current Data Bearer Technology */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWdsGetCurrentDataBearerTechnologyOutput:
 *
 * The #QmiMessageWdsGetCurrentDataBearerTechnologyOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsGetCurrentDataBearerTechnologyOutput QmiMessageWdsGetCurrentDataBearerTechnologyOutput;
GType qmi_message_wds_get_current_data_bearer_technology_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_GET_CURRENT_DATA_BEARER_TECHNOLOGY_OUTPUT (qmi_message_wds_get_current_data_bearer_technology_output_get_type ())

gboolean qmi_message_wds_get_current_data_bearer_technology_output_get_result (
    QmiMessageWdsGetCurrentDataBearerTechnologyOutput *self,
    GError **error);

gboolean qmi_message_wds_get_current_data_bearer_technology_output_get_last (
    QmiMessageWdsGetCurrentDataBearerTechnologyOutput *self,
    QmiWdsNetworkType *last_network_type,
    guint32 *last_rat_mask,
    guint32 *last_so_mask,
    GError **error);

gboolean qmi_message_wds_get_current_data_bearer_technology_output_get_current (
    QmiMessageWdsGetCurrentDataBearerTechnologyOutput *self,
    QmiWdsNetworkType *current_network_type,
    guint32 *current_rat_mask,
    guint32 *current_so_mask,
    GError **error);

QmiMessageWdsGetCurrentDataBearerTechnologyOutput *qmi_message_wds_get_current_data_bearer_technology_output_ref (QmiMessageWdsGetCurrentDataBearerTechnologyOutput *self);
void qmi_message_wds_get_current_data_bearer_technology_output_unref (QmiMessageWdsGetCurrentDataBearerTechnologyOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WDS Set IP Family */


/* --- Input -- */

/**
 * QmiMessageWdsSetIpFamilyInput:
 *
 * The #QmiMessageWdsSetIpFamilyInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsSetIpFamilyInput QmiMessageWdsSetIpFamilyInput;
GType qmi_message_wds_set_ip_family_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_SET_IP_FAMILY_INPUT (qmi_message_wds_set_ip_family_input_get_type ())

gboolean qmi_message_wds_set_ip_family_input_get_preference (
    QmiMessageWdsSetIpFamilyInput *self,
    QmiWdsIpFamily *preference,
    GError **error);

gboolean qmi_message_wds_set_ip_family_input_set_preference (
    QmiMessageWdsSetIpFamilyInput *self,
    QmiWdsIpFamily preference,
    GError **error);

QmiMessageWdsSetIpFamilyInput *qmi_message_wds_set_ip_family_input_ref (QmiMessageWdsSetIpFamilyInput *self);
void qmi_message_wds_set_ip_family_input_unref (QmiMessageWdsSetIpFamilyInput *self);
QmiMessageWdsSetIpFamilyInput *qmi_message_wds_set_ip_family_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWdsSetIpFamilyOutput:
 *
 * The #QmiMessageWdsSetIpFamilyOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWdsSetIpFamilyOutput QmiMessageWdsSetIpFamilyOutput;
GType qmi_message_wds_set_ip_family_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WDS_SET_IP_FAMILY_OUTPUT (qmi_message_wds_set_ip_family_output_get_type ())

gboolean qmi_message_wds_set_ip_family_output_get_result (
    QmiMessageWdsSetIpFamilyOutput *self,
    GError **error);

QmiMessageWdsSetIpFamilyOutput *qmi_message_wds_set_ip_family_output_ref (QmiMessageWdsSetIpFamilyOutput *self);
void qmi_message_wds_set_ip_family_output_unref (QmiMessageWdsSetIpFamilyOutput *self);

/*****************************************************************************/
/* Service-specific printable: WDS */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_wds_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_wds_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client WDS */

#define QMI_TYPE_CLIENT_WDS            (qmi_client_wds_get_type ())
#define QMI_CLIENT_WDS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_WDS, QmiClientWds))
#define QMI_CLIENT_WDS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_WDS, QmiClientWdsClass))
#define QMI_IS_CLIENT_WDS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_WDS))
#define QMI_IS_CLIENT_WDS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_WDS))
#define QMI_CLIENT_WDS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_WDS, QmiClientWdsClass))

typedef struct _QmiClientWds QmiClientWds;
typedef struct _QmiClientWdsClass QmiClientWdsClass;

/**
 * QmiClientWds:
 *
 * The #QmiClientWds structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientWds {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientWdsClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_wds_get_type (void);


void qmi_client_wds_reset (
    QmiClientWds *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsResetOutput *qmi_client_wds_reset_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_start_network (
    QmiClientWds *self,
    QmiMessageWdsStartNetworkInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsStartNetworkOutput *qmi_client_wds_start_network_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_stop_network (
    QmiClientWds *self,
    QmiMessageWdsStopNetworkInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsStopNetworkOutput *qmi_client_wds_stop_network_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_packet_service_status (
    QmiClientWds *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetPacketServiceStatusOutput *qmi_client_wds_get_packet_service_status_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_packet_statistics (
    QmiClientWds *self,
    QmiMessageWdsGetPacketStatisticsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetPacketStatisticsOutput *qmi_client_wds_get_packet_statistics_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_profile_list (
    QmiClientWds *self,
    QmiMessageWdsGetProfileListInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetProfileListOutput *qmi_client_wds_get_profile_list_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_profile_settings (
    QmiClientWds *self,
    QmiMessageWdsGetProfileSettingsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetProfileSettingsOutput *qmi_client_wds_get_profile_settings_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_default_settings (
    QmiClientWds *self,
    QmiMessageWdsGetDefaultSettingsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetDefaultSettingsOutput *qmi_client_wds_get_default_settings_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_current_settings (
    QmiClientWds *self,
    QmiMessageWdsGetCurrentSettingsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetCurrentSettingsOutput *qmi_client_wds_get_current_settings_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_data_bearer_technology (
    QmiClientWds *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetDataBearerTechnologyOutput *qmi_client_wds_get_data_bearer_technology_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_get_current_data_bearer_technology (
    QmiClientWds *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsGetCurrentDataBearerTechnologyOutput *qmi_client_wds_get_current_data_bearer_technology_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wds_set_ip_family (
    QmiClientWds *self,
    QmiMessageWdsSetIpFamilyInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWdsSetIpFamilyOutput *qmi_client_wds_set_ip_family_finish (
    QmiClientWds *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_WDS__ */
