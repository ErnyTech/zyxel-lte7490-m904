
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
#include "qmi-enums-dms.h"
#include "qmi-flags64-dms.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_DMS__
#define __LIBQMI_GLIB_QMI_DMS__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsResetOutput:
 *
 * The #QmiMessageDmsResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsResetOutput QmiMessageDmsResetOutput;
GType qmi_message_dms_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_RESET_OUTPUT (qmi_message_dms_reset_output_get_type ())

gboolean qmi_message_dms_reset_output_get_result (
    QmiMessageDmsResetOutput *self,
    GError **error);

QmiMessageDmsResetOutput *qmi_message_dms_reset_output_ref (QmiMessageDmsResetOutput *self);
void qmi_message_dms_reset_output_unref (QmiMessageDmsResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Event Report */


/* --- Input -- */

/**
 * QmiMessageDmsSetEventReportInput:
 *
 * The #QmiMessageDmsSetEventReportInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetEventReportInput QmiMessageDmsSetEventReportInput;
GType qmi_message_dms_set_event_report_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_EVENT_REPORT_INPUT (qmi_message_dms_set_event_report_input_get_type ())

gboolean qmi_message_dms_set_event_report_input_get_prl_init_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *prl_init_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_prl_init_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean prl_init_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_wireless_disable_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *wireless_disable_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_wireless_disable_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean wireless_disable_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_uim_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *uim_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_uim_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean uim_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_operating_mode_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *operating_mode_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_operating_mode_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean operating_mode_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_activation_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *activation_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_activation_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean activation_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_pin_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *pin_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_pin_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean pin_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_battery_level_report_limits (
    QmiMessageDmsSetEventReportInput *self,
    guint8 *battery_level_report_limits_lower_limit,
    guint8 *battery_level_report_limits_upper_limit,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_battery_level_report_limits (
    QmiMessageDmsSetEventReportInput *self,
    guint8 battery_level_report_limits_lower_limit,
    guint8 battery_level_report_limits_upper_limit,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_get_power_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean *power_state_reporting,
    GError **error);

gboolean qmi_message_dms_set_event_report_input_set_power_state_reporting (
    QmiMessageDmsSetEventReportInput *self,
    gboolean power_state_reporting,
    GError **error);

QmiMessageDmsSetEventReportInput *qmi_message_dms_set_event_report_input_ref (QmiMessageDmsSetEventReportInput *self);
void qmi_message_dms_set_event_report_input_unref (QmiMessageDmsSetEventReportInput *self);
QmiMessageDmsSetEventReportInput *qmi_message_dms_set_event_report_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetEventReportOutput:
 *
 * The #QmiMessageDmsSetEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetEventReportOutput QmiMessageDmsSetEventReportOutput;
GType qmi_message_dms_set_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_EVENT_REPORT_OUTPUT (qmi_message_dms_set_event_report_output_get_type ())

gboolean qmi_message_dms_set_event_report_output_get_result (
    QmiMessageDmsSetEventReportOutput *self,
    GError **error);

QmiMessageDmsSetEventReportOutput *qmi_message_dms_set_event_report_output_ref (QmiMessageDmsSetEventReportOutput *self);
void qmi_message_dms_set_event_report_output_unref (QmiMessageDmsSetEventReportOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication DMS Event Report */


/* --- Output -- */

/**
 * QmiIndicationDmsEventReportOutput:
 *
 * The #QmiIndicationDmsEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationDmsEventReportOutput QmiIndicationDmsEventReportOutput;
GType qmi_indication_dms_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_DMS_EVENT_REPORT_OUTPUT (qmi_indication_dms_event_report_output_get_type ())

gboolean qmi_indication_dms_event_report_output_get_prl_init_notification (
    QmiIndicationDmsEventReportOutput *self,
    gboolean *prl_init_notification,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_wireless_disable_state (
    QmiIndicationDmsEventReportOutput *self,
    gboolean *wireless_disable_state,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_uim_state (
    QmiIndicationDmsEventReportOutput *self,
    QmiDmsUimState *uim_state,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_operating_mode (
    QmiIndicationDmsEventReportOutput *self,
    QmiDmsOperatingMode *operating_mode,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_activation_state (
    QmiIndicationDmsEventReportOutput *self,
    QmiDmsActivationState *activation_state,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_pin2_status (
    QmiIndicationDmsEventReportOutput *self,
    QmiDmsUimPinStatus *pin2_status_current_status,
    guint8 *pin2_status_verify_retries_left,
    guint8 *pin2_status_unblock_retries_left,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_pin1_status (
    QmiIndicationDmsEventReportOutput *self,
    QmiDmsUimPinStatus *pin1_status_current_status,
    guint8 *pin1_status_verify_retries_left,
    guint8 *pin1_status_unblock_retries_left,
    GError **error);

gboolean qmi_indication_dms_event_report_output_get_power_state (
    QmiIndicationDmsEventReportOutput *self,
    guint8 *power_state_power_state_flags,
    guint8 *power_state_battery_level,
    GError **error);

QmiIndicationDmsEventReportOutput *qmi_indication_dms_event_report_output_ref (QmiIndicationDmsEventReportOutput *self);
void qmi_indication_dms_event_report_output_unref (QmiIndicationDmsEventReportOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Capabilities */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetCapabilitiesOutput:
 *
 * The #QmiMessageDmsGetCapabilitiesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetCapabilitiesOutput QmiMessageDmsGetCapabilitiesOutput;
GType qmi_message_dms_get_capabilities_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_CAPABILITIES_OUTPUT (qmi_message_dms_get_capabilities_output_get_type ())

gboolean qmi_message_dms_get_capabilities_output_get_result (
    QmiMessageDmsGetCapabilitiesOutput *self,
    GError **error);

gboolean qmi_message_dms_get_capabilities_output_get_info (
    QmiMessageDmsGetCapabilitiesOutput *self,
    guint32 *info_max_tx_channel_rate,
    guint32 *info_max_rx_channel_rate,
    QmiDmsDataServiceCapability *info_data_service_capability,
    QmiDmsSimCapability *info_sim_capability,
    GArray **info_radio_interface_list,
    GError **error);

QmiMessageDmsGetCapabilitiesOutput *qmi_message_dms_get_capabilities_output_ref (QmiMessageDmsGetCapabilitiesOutput *self);
void qmi_message_dms_get_capabilities_output_unref (QmiMessageDmsGetCapabilitiesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Manufacturer */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetManufacturerOutput:
 *
 * The #QmiMessageDmsGetManufacturerOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetManufacturerOutput QmiMessageDmsGetManufacturerOutput;
GType qmi_message_dms_get_manufacturer_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_MANUFACTURER_OUTPUT (qmi_message_dms_get_manufacturer_output_get_type ())

gboolean qmi_message_dms_get_manufacturer_output_get_result (
    QmiMessageDmsGetManufacturerOutput *self,
    GError **error);

gboolean qmi_message_dms_get_manufacturer_output_get_manufacturer (
    QmiMessageDmsGetManufacturerOutput *self,
    const gchar **manufacturer,
    GError **error);

QmiMessageDmsGetManufacturerOutput *qmi_message_dms_get_manufacturer_output_ref (QmiMessageDmsGetManufacturerOutput *self);
void qmi_message_dms_get_manufacturer_output_unref (QmiMessageDmsGetManufacturerOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Model */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetModelOutput:
 *
 * The #QmiMessageDmsGetModelOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetModelOutput QmiMessageDmsGetModelOutput;
GType qmi_message_dms_get_model_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_MODEL_OUTPUT (qmi_message_dms_get_model_output_get_type ())

gboolean qmi_message_dms_get_model_output_get_result (
    QmiMessageDmsGetModelOutput *self,
    GError **error);

gboolean qmi_message_dms_get_model_output_get_model (
    QmiMessageDmsGetModelOutput *self,
    const gchar **model,
    GError **error);

QmiMessageDmsGetModelOutput *qmi_message_dms_get_model_output_ref (QmiMessageDmsGetModelOutput *self);
void qmi_message_dms_get_model_output_unref (QmiMessageDmsGetModelOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Revision */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetRevisionOutput:
 *
 * The #QmiMessageDmsGetRevisionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetRevisionOutput QmiMessageDmsGetRevisionOutput;
GType qmi_message_dms_get_revision_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_REVISION_OUTPUT (qmi_message_dms_get_revision_output_get_type ())

gboolean qmi_message_dms_get_revision_output_get_result (
    QmiMessageDmsGetRevisionOutput *self,
    GError **error);

gboolean qmi_message_dms_get_revision_output_get_revision (
    QmiMessageDmsGetRevisionOutput *self,
    const gchar **revision,
    GError **error);

QmiMessageDmsGetRevisionOutput *qmi_message_dms_get_revision_output_ref (QmiMessageDmsGetRevisionOutput *self);
void qmi_message_dms_get_revision_output_unref (QmiMessageDmsGetRevisionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get MSISDN */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetMsisdnOutput:
 *
 * The #QmiMessageDmsGetMsisdnOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetMsisdnOutput QmiMessageDmsGetMsisdnOutput;
GType qmi_message_dms_get_msisdn_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_MSISDN_OUTPUT (qmi_message_dms_get_msisdn_output_get_type ())

gboolean qmi_message_dms_get_msisdn_output_get_result (
    QmiMessageDmsGetMsisdnOutput *self,
    GError **error);

gboolean qmi_message_dms_get_msisdn_output_get_msisdn (
    QmiMessageDmsGetMsisdnOutput *self,
    const gchar **msisdn,
    GError **error);

QmiMessageDmsGetMsisdnOutput *qmi_message_dms_get_msisdn_output_ref (QmiMessageDmsGetMsisdnOutput *self);
void qmi_message_dms_get_msisdn_output_unref (QmiMessageDmsGetMsisdnOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get IDs */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetIdsOutput:
 *
 * The #QmiMessageDmsGetIdsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetIdsOutput QmiMessageDmsGetIdsOutput;
GType qmi_message_dms_get_ids_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_IDS_OUTPUT (qmi_message_dms_get_ids_output_get_type ())

gboolean qmi_message_dms_get_ids_output_get_result (
    QmiMessageDmsGetIdsOutput *self,
    GError **error);

gboolean qmi_message_dms_get_ids_output_get_esn (
    QmiMessageDmsGetIdsOutput *self,
    const gchar **esn,
    GError **error);

gboolean qmi_message_dms_get_ids_output_get_imei (
    QmiMessageDmsGetIdsOutput *self,
    const gchar **imei,
    GError **error);

gboolean qmi_message_dms_get_ids_output_get_meid (
    QmiMessageDmsGetIdsOutput *self,
    const gchar **meid,
    GError **error);

QmiMessageDmsGetIdsOutput *qmi_message_dms_get_ids_output_ref (QmiMessageDmsGetIdsOutput *self);
void qmi_message_dms_get_ids_output_unref (QmiMessageDmsGetIdsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Power State */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetPowerStateOutput:
 *
 * The #QmiMessageDmsGetPowerStateOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetPowerStateOutput QmiMessageDmsGetPowerStateOutput;
GType qmi_message_dms_get_power_state_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_POWER_STATE_OUTPUT (qmi_message_dms_get_power_state_output_get_type ())

gboolean qmi_message_dms_get_power_state_output_get_result (
    QmiMessageDmsGetPowerStateOutput *self,
    GError **error);

gboolean qmi_message_dms_get_power_state_output_get_info (
    QmiMessageDmsGetPowerStateOutput *self,
    guint8 *info_power_state_flags,
    guint8 *info_battery_level,
    GError **error);

QmiMessageDmsGetPowerStateOutput *qmi_message_dms_get_power_state_output_ref (QmiMessageDmsGetPowerStateOutput *self);
void qmi_message_dms_get_power_state_output_unref (QmiMessageDmsGetPowerStateOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Set PIN Protection */


/* --- Input -- */

/**
 * QmiMessageDmsUimSetPinProtectionInput:
 *
 * The #QmiMessageDmsUimSetPinProtectionInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimSetPinProtectionInput QmiMessageDmsUimSetPinProtectionInput;
GType qmi_message_dms_uim_set_pin_protection_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_SET_PIN_PROTECTION_INPUT (qmi_message_dms_uim_set_pin_protection_input_get_type ())

gboolean qmi_message_dms_uim_set_pin_protection_input_get_info (
    QmiMessageDmsUimSetPinProtectionInput *self,
    QmiDmsUimPinId *info_pin_id,
    gboolean *info_protection_enabled,
    const gchar **info_pin,
    GError **error);

gboolean qmi_message_dms_uim_set_pin_protection_input_set_info (
    QmiMessageDmsUimSetPinProtectionInput *self,
    QmiDmsUimPinId info_pin_id,
    gboolean info_protection_enabled,
    const gchar *info_pin,
    GError **error);

QmiMessageDmsUimSetPinProtectionInput *qmi_message_dms_uim_set_pin_protection_input_ref (QmiMessageDmsUimSetPinProtectionInput *self);
void qmi_message_dms_uim_set_pin_protection_input_unref (QmiMessageDmsUimSetPinProtectionInput *self);
QmiMessageDmsUimSetPinProtectionInput *qmi_message_dms_uim_set_pin_protection_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimSetPinProtectionOutput:
 *
 * The #QmiMessageDmsUimSetPinProtectionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimSetPinProtectionOutput QmiMessageDmsUimSetPinProtectionOutput;
GType qmi_message_dms_uim_set_pin_protection_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_SET_PIN_PROTECTION_OUTPUT (qmi_message_dms_uim_set_pin_protection_output_get_type ())

gboolean qmi_message_dms_uim_set_pin_protection_output_get_result (
    QmiMessageDmsUimSetPinProtectionOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_set_pin_protection_output_get_pin_retries_status (
    QmiMessageDmsUimSetPinProtectionOutput *self,
    guint8 *pin_retries_status_verify_retries_left,
    guint8 *pin_retries_status_unblock_retries_left,
    GError **error);

QmiMessageDmsUimSetPinProtectionOutput *qmi_message_dms_uim_set_pin_protection_output_ref (QmiMessageDmsUimSetPinProtectionOutput *self);
void qmi_message_dms_uim_set_pin_protection_output_unref (QmiMessageDmsUimSetPinProtectionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Verify PIN */


/* --- Input -- */

/**
 * QmiMessageDmsUimVerifyPinInput:
 *
 * The #QmiMessageDmsUimVerifyPinInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimVerifyPinInput QmiMessageDmsUimVerifyPinInput;
GType qmi_message_dms_uim_verify_pin_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_VERIFY_PIN_INPUT (qmi_message_dms_uim_verify_pin_input_get_type ())

gboolean qmi_message_dms_uim_verify_pin_input_get_info (
    QmiMessageDmsUimVerifyPinInput *self,
    QmiDmsUimPinId *info_pin_id,
    const gchar **info_pin,
    GError **error);

gboolean qmi_message_dms_uim_verify_pin_input_set_info (
    QmiMessageDmsUimVerifyPinInput *self,
    QmiDmsUimPinId info_pin_id,
    const gchar *info_pin,
    GError **error);

QmiMessageDmsUimVerifyPinInput *qmi_message_dms_uim_verify_pin_input_ref (QmiMessageDmsUimVerifyPinInput *self);
void qmi_message_dms_uim_verify_pin_input_unref (QmiMessageDmsUimVerifyPinInput *self);
QmiMessageDmsUimVerifyPinInput *qmi_message_dms_uim_verify_pin_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimVerifyPinOutput:
 *
 * The #QmiMessageDmsUimVerifyPinOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimVerifyPinOutput QmiMessageDmsUimVerifyPinOutput;
GType qmi_message_dms_uim_verify_pin_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_VERIFY_PIN_OUTPUT (qmi_message_dms_uim_verify_pin_output_get_type ())

gboolean qmi_message_dms_uim_verify_pin_output_get_result (
    QmiMessageDmsUimVerifyPinOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_verify_pin_output_get_pin_retries_status (
    QmiMessageDmsUimVerifyPinOutput *self,
    guint8 *pin_retries_status_verify_retries_left,
    guint8 *pin_retries_status_unblock_retries_left,
    GError **error);

QmiMessageDmsUimVerifyPinOutput *qmi_message_dms_uim_verify_pin_output_ref (QmiMessageDmsUimVerifyPinOutput *self);
void qmi_message_dms_uim_verify_pin_output_unref (QmiMessageDmsUimVerifyPinOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Unblock PIN */


/* --- Input -- */

/**
 * QmiMessageDmsUimUnblockPinInput:
 *
 * The #QmiMessageDmsUimUnblockPinInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimUnblockPinInput QmiMessageDmsUimUnblockPinInput;
GType qmi_message_dms_uim_unblock_pin_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_UNBLOCK_PIN_INPUT (qmi_message_dms_uim_unblock_pin_input_get_type ())

gboolean qmi_message_dms_uim_unblock_pin_input_get_info (
    QmiMessageDmsUimUnblockPinInput *self,
    QmiDmsUimPinId *info_pin_id,
    const gchar **info_puk,
    const gchar **info_new_pin,
    GError **error);

gboolean qmi_message_dms_uim_unblock_pin_input_set_info (
    QmiMessageDmsUimUnblockPinInput *self,
    QmiDmsUimPinId info_pin_id,
    const gchar *info_puk,
    const gchar *info_new_pin,
    GError **error);

QmiMessageDmsUimUnblockPinInput *qmi_message_dms_uim_unblock_pin_input_ref (QmiMessageDmsUimUnblockPinInput *self);
void qmi_message_dms_uim_unblock_pin_input_unref (QmiMessageDmsUimUnblockPinInput *self);
QmiMessageDmsUimUnblockPinInput *qmi_message_dms_uim_unblock_pin_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimUnblockPinOutput:
 *
 * The #QmiMessageDmsUimUnblockPinOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimUnblockPinOutput QmiMessageDmsUimUnblockPinOutput;
GType qmi_message_dms_uim_unblock_pin_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_UNBLOCK_PIN_OUTPUT (qmi_message_dms_uim_unblock_pin_output_get_type ())

gboolean qmi_message_dms_uim_unblock_pin_output_get_result (
    QmiMessageDmsUimUnblockPinOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_unblock_pin_output_get_pin_retries_status (
    QmiMessageDmsUimUnblockPinOutput *self,
    guint8 *pin_retries_status_verify_retries_left,
    guint8 *pin_retries_status_unblock_retries_left,
    GError **error);

QmiMessageDmsUimUnblockPinOutput *qmi_message_dms_uim_unblock_pin_output_ref (QmiMessageDmsUimUnblockPinOutput *self);
void qmi_message_dms_uim_unblock_pin_output_unref (QmiMessageDmsUimUnblockPinOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Change PIN */


/* --- Input -- */

/**
 * QmiMessageDmsUimChangePinInput:
 *
 * The #QmiMessageDmsUimChangePinInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimChangePinInput QmiMessageDmsUimChangePinInput;
GType qmi_message_dms_uim_change_pin_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_CHANGE_PIN_INPUT (qmi_message_dms_uim_change_pin_input_get_type ())

gboolean qmi_message_dms_uim_change_pin_input_get_info (
    QmiMessageDmsUimChangePinInput *self,
    QmiDmsUimPinId *info_pin_id,
    const gchar **info_old_pin,
    const gchar **info_new_pin,
    GError **error);

gboolean qmi_message_dms_uim_change_pin_input_set_info (
    QmiMessageDmsUimChangePinInput *self,
    QmiDmsUimPinId info_pin_id,
    const gchar *info_old_pin,
    const gchar *info_new_pin,
    GError **error);

QmiMessageDmsUimChangePinInput *qmi_message_dms_uim_change_pin_input_ref (QmiMessageDmsUimChangePinInput *self);
void qmi_message_dms_uim_change_pin_input_unref (QmiMessageDmsUimChangePinInput *self);
QmiMessageDmsUimChangePinInput *qmi_message_dms_uim_change_pin_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimChangePinOutput:
 *
 * The #QmiMessageDmsUimChangePinOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimChangePinOutput QmiMessageDmsUimChangePinOutput;
GType qmi_message_dms_uim_change_pin_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_CHANGE_PIN_OUTPUT (qmi_message_dms_uim_change_pin_output_get_type ())

gboolean qmi_message_dms_uim_change_pin_output_get_result (
    QmiMessageDmsUimChangePinOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_change_pin_output_get_pin_retries_status (
    QmiMessageDmsUimChangePinOutput *self,
    guint8 *pin_retries_status_verify_retries_left,
    guint8 *pin_retries_status_unblock_retries_left,
    GError **error);

QmiMessageDmsUimChangePinOutput *qmi_message_dms_uim_change_pin_output_ref (QmiMessageDmsUimChangePinOutput *self);
void qmi_message_dms_uim_change_pin_output_unref (QmiMessageDmsUimChangePinOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Get PIN Status */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsUimGetPinStatusOutput:
 *
 * The #QmiMessageDmsUimGetPinStatusOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetPinStatusOutput QmiMessageDmsUimGetPinStatusOutput;
GType qmi_message_dms_uim_get_pin_status_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_PIN_STATUS_OUTPUT (qmi_message_dms_uim_get_pin_status_output_get_type ())

gboolean qmi_message_dms_uim_get_pin_status_output_get_result (
    QmiMessageDmsUimGetPinStatusOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_get_pin_status_output_get_pin2_status (
    QmiMessageDmsUimGetPinStatusOutput *self,
    QmiDmsUimPinStatus *pin2_status_current_status,
    guint8 *pin2_status_verify_retries_left,
    guint8 *pin2_status_unblock_retries_left,
    GError **error);

gboolean qmi_message_dms_uim_get_pin_status_output_get_pin1_status (
    QmiMessageDmsUimGetPinStatusOutput *self,
    QmiDmsUimPinStatus *pin1_status_current_status,
    guint8 *pin1_status_verify_retries_left,
    guint8 *pin1_status_unblock_retries_left,
    GError **error);

QmiMessageDmsUimGetPinStatusOutput *qmi_message_dms_uim_get_pin_status_output_ref (QmiMessageDmsUimGetPinStatusOutput *self);
void qmi_message_dms_uim_get_pin_status_output_unref (QmiMessageDmsUimGetPinStatusOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Hardware Revision */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetHardwareRevisionOutput:
 *
 * The #QmiMessageDmsGetHardwareRevisionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetHardwareRevisionOutput QmiMessageDmsGetHardwareRevisionOutput;
GType qmi_message_dms_get_hardware_revision_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_HARDWARE_REVISION_OUTPUT (qmi_message_dms_get_hardware_revision_output_get_type ())

gboolean qmi_message_dms_get_hardware_revision_output_get_result (
    QmiMessageDmsGetHardwareRevisionOutput *self,
    GError **error);

gboolean qmi_message_dms_get_hardware_revision_output_get_revision (
    QmiMessageDmsGetHardwareRevisionOutput *self,
    const gchar **revision,
    GError **error);

QmiMessageDmsGetHardwareRevisionOutput *qmi_message_dms_get_hardware_revision_output_ref (QmiMessageDmsGetHardwareRevisionOutput *self);
void qmi_message_dms_get_hardware_revision_output_unref (QmiMessageDmsGetHardwareRevisionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Operating Mode */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetOperatingModeOutput:
 *
 * The #QmiMessageDmsGetOperatingModeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetOperatingModeOutput QmiMessageDmsGetOperatingModeOutput;
GType qmi_message_dms_get_operating_mode_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_OPERATING_MODE_OUTPUT (qmi_message_dms_get_operating_mode_output_get_type ())

gboolean qmi_message_dms_get_operating_mode_output_get_result (
    QmiMessageDmsGetOperatingModeOutput *self,
    GError **error);

gboolean qmi_message_dms_get_operating_mode_output_get_mode (
    QmiMessageDmsGetOperatingModeOutput *self,
    QmiDmsOperatingMode *mode,
    GError **error);

gboolean qmi_message_dms_get_operating_mode_output_get_offline_reason (
    QmiMessageDmsGetOperatingModeOutput *self,
    QmiDmsOfflineReason *offline_reason,
    GError **error);

gboolean qmi_message_dms_get_operating_mode_output_get_hardware_restricted_mode (
    QmiMessageDmsGetOperatingModeOutput *self,
    gboolean *hardware_restricted_mode,
    GError **error);

QmiMessageDmsGetOperatingModeOutput *qmi_message_dms_get_operating_mode_output_ref (QmiMessageDmsGetOperatingModeOutput *self);
void qmi_message_dms_get_operating_mode_output_unref (QmiMessageDmsGetOperatingModeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Operating Mode */


/* --- Input -- */

/**
 * QmiMessageDmsSetOperatingModeInput:
 *
 * The #QmiMessageDmsSetOperatingModeInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetOperatingModeInput QmiMessageDmsSetOperatingModeInput;
GType qmi_message_dms_set_operating_mode_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_OPERATING_MODE_INPUT (qmi_message_dms_set_operating_mode_input_get_type ())

gboolean qmi_message_dms_set_operating_mode_input_get_mode (
    QmiMessageDmsSetOperatingModeInput *self,
    QmiDmsOperatingMode *mode,
    GError **error);

gboolean qmi_message_dms_set_operating_mode_input_set_mode (
    QmiMessageDmsSetOperatingModeInput *self,
    QmiDmsOperatingMode mode,
    GError **error);

QmiMessageDmsSetOperatingModeInput *qmi_message_dms_set_operating_mode_input_ref (QmiMessageDmsSetOperatingModeInput *self);
void qmi_message_dms_set_operating_mode_input_unref (QmiMessageDmsSetOperatingModeInput *self);
QmiMessageDmsSetOperatingModeInput *qmi_message_dms_set_operating_mode_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetOperatingModeOutput:
 *
 * The #QmiMessageDmsSetOperatingModeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetOperatingModeOutput QmiMessageDmsSetOperatingModeOutput;
GType qmi_message_dms_set_operating_mode_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_OPERATING_MODE_OUTPUT (qmi_message_dms_set_operating_mode_output_get_type ())

gboolean qmi_message_dms_set_operating_mode_output_get_result (
    QmiMessageDmsSetOperatingModeOutput *self,
    GError **error);

QmiMessageDmsSetOperatingModeOutput *qmi_message_dms_set_operating_mode_output_ref (QmiMessageDmsSetOperatingModeOutput *self);
void qmi_message_dms_set_operating_mode_output_unref (QmiMessageDmsSetOperatingModeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Time */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetTimeOutput:
 *
 * The #QmiMessageDmsGetTimeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetTimeOutput QmiMessageDmsGetTimeOutput;
GType qmi_message_dms_get_time_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_TIME_OUTPUT (qmi_message_dms_get_time_output_get_type ())

gboolean qmi_message_dms_get_time_output_get_result (
    QmiMessageDmsGetTimeOutput *self,
    GError **error);

gboolean qmi_message_dms_get_time_output_get_device_time (
    QmiMessageDmsGetTimeOutput *self,
    guint64 *device_time_time_count,
    QmiDmsTimeSource *device_time_time_source,
    GError **error);

gboolean qmi_message_dms_get_time_output_get_system_time (
    QmiMessageDmsGetTimeOutput *self,
    guint64 *system_time,
    GError **error);

gboolean qmi_message_dms_get_time_output_get_user_time (
    QmiMessageDmsGetTimeOutput *self,
    guint64 *user_time,
    GError **error);

QmiMessageDmsGetTimeOutput *qmi_message_dms_get_time_output_ref (QmiMessageDmsGetTimeOutput *self);
void qmi_message_dms_get_time_output_unref (QmiMessageDmsGetTimeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get PRL Version */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetPrlVersionOutput:
 *
 * The #QmiMessageDmsGetPrlVersionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetPrlVersionOutput QmiMessageDmsGetPrlVersionOutput;
GType qmi_message_dms_get_prl_version_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_PRL_VERSION_OUTPUT (qmi_message_dms_get_prl_version_output_get_type ())

gboolean qmi_message_dms_get_prl_version_output_get_result (
    QmiMessageDmsGetPrlVersionOutput *self,
    GError **error);

gboolean qmi_message_dms_get_prl_version_output_get_version (
    QmiMessageDmsGetPrlVersionOutput *self,
    guint16 *version,
    GError **error);

gboolean qmi_message_dms_get_prl_version_output_get_prl_only_preference (
    QmiMessageDmsGetPrlVersionOutput *self,
    gboolean *prl_only_preference,
    GError **error);

QmiMessageDmsGetPrlVersionOutput *qmi_message_dms_get_prl_version_output_ref (QmiMessageDmsGetPrlVersionOutput *self);
void qmi_message_dms_get_prl_version_output_unref (QmiMessageDmsGetPrlVersionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Activation State */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetActivationStateOutput:
 *
 * The #QmiMessageDmsGetActivationStateOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetActivationStateOutput QmiMessageDmsGetActivationStateOutput;
GType qmi_message_dms_get_activation_state_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_ACTIVATION_STATE_OUTPUT (qmi_message_dms_get_activation_state_output_get_type ())

gboolean qmi_message_dms_get_activation_state_output_get_result (
    QmiMessageDmsGetActivationStateOutput *self,
    GError **error);

gboolean qmi_message_dms_get_activation_state_output_get_info (
    QmiMessageDmsGetActivationStateOutput *self,
    QmiDmsActivationState *info,
    GError **error);

QmiMessageDmsGetActivationStateOutput *qmi_message_dms_get_activation_state_output_ref (QmiMessageDmsGetActivationStateOutput *self);
void qmi_message_dms_get_activation_state_output_unref (QmiMessageDmsGetActivationStateOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Activate Automatic */


/* --- Input -- */

/**
 * QmiMessageDmsActivateAutomaticInput:
 *
 * The #QmiMessageDmsActivateAutomaticInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsActivateAutomaticInput QmiMessageDmsActivateAutomaticInput;
GType qmi_message_dms_activate_automatic_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_ACTIVATE_AUTOMATIC_INPUT (qmi_message_dms_activate_automatic_input_get_type ())

gboolean qmi_message_dms_activate_automatic_input_get_activation_code (
    QmiMessageDmsActivateAutomaticInput *self,
    const gchar **activation_code,
    GError **error);

gboolean qmi_message_dms_activate_automatic_input_set_activation_code (
    QmiMessageDmsActivateAutomaticInput *self,
    const gchar *activation_code,
    GError **error);

QmiMessageDmsActivateAutomaticInput *qmi_message_dms_activate_automatic_input_ref (QmiMessageDmsActivateAutomaticInput *self);
void qmi_message_dms_activate_automatic_input_unref (QmiMessageDmsActivateAutomaticInput *self);
QmiMessageDmsActivateAutomaticInput *qmi_message_dms_activate_automatic_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsActivateAutomaticOutput:
 *
 * The #QmiMessageDmsActivateAutomaticOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsActivateAutomaticOutput QmiMessageDmsActivateAutomaticOutput;
GType qmi_message_dms_activate_automatic_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_ACTIVATE_AUTOMATIC_OUTPUT (qmi_message_dms_activate_automatic_output_get_type ())

gboolean qmi_message_dms_activate_automatic_output_get_result (
    QmiMessageDmsActivateAutomaticOutput *self,
    GError **error);

QmiMessageDmsActivateAutomaticOutput *qmi_message_dms_activate_automatic_output_ref (QmiMessageDmsActivateAutomaticOutput *self);
void qmi_message_dms_activate_automatic_output_unref (QmiMessageDmsActivateAutomaticOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Activate Manual */


/* --- Input -- */

/**
 * QmiMessageDmsActivateManualInput:
 *
 * The #QmiMessageDmsActivateManualInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsActivateManualInput QmiMessageDmsActivateManualInput;
GType qmi_message_dms_activate_manual_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_ACTIVATE_MANUAL_INPUT (qmi_message_dms_activate_manual_input_get_type ())

gboolean qmi_message_dms_activate_manual_input_get_prl (
    QmiMessageDmsActivateManualInput *self,
    guint16 *prl_prl_total_length,
    guint8 *prl_prl_segment_sequence,
    GArray **prl_prl_segment,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_set_prl (
    QmiMessageDmsActivateManualInput *self,
    guint16 prl_prl_total_length,
    guint8 prl_prl_segment_sequence,
    GArray *prl_prl_segment,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_get_mn_aaa_key (
    QmiMessageDmsActivateManualInput *self,
    const gchar **mn_aaa_key,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_set_mn_aaa_key (
    QmiMessageDmsActivateManualInput *self,
    const gchar *mn_aaa_key,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_get_mn_ha_key (
    QmiMessageDmsActivateManualInput *self,
    const gchar **mn_ha_key,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_set_mn_ha_key (
    QmiMessageDmsActivateManualInput *self,
    const gchar *mn_ha_key,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_get_info (
    QmiMessageDmsActivateManualInput *self,
    const gchar **info_service_programming_code,
    guint16 *info_system_identification_number,
    const gchar **info_mobile_directory_number,
    const gchar **info_mobile_identification_number,
    GError **error);

gboolean qmi_message_dms_activate_manual_input_set_info (
    QmiMessageDmsActivateManualInput *self,
    const gchar *info_service_programming_code,
    guint16 info_system_identification_number,
    const gchar *info_mobile_directory_number,
    const gchar *info_mobile_identification_number,
    GError **error);

QmiMessageDmsActivateManualInput *qmi_message_dms_activate_manual_input_ref (QmiMessageDmsActivateManualInput *self);
void qmi_message_dms_activate_manual_input_unref (QmiMessageDmsActivateManualInput *self);
QmiMessageDmsActivateManualInput *qmi_message_dms_activate_manual_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsActivateManualOutput:
 *
 * The #QmiMessageDmsActivateManualOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsActivateManualOutput QmiMessageDmsActivateManualOutput;
GType qmi_message_dms_activate_manual_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_ACTIVATE_MANUAL_OUTPUT (qmi_message_dms_activate_manual_output_get_type ())

gboolean qmi_message_dms_activate_manual_output_get_result (
    QmiMessageDmsActivateManualOutput *self,
    GError **error);

QmiMessageDmsActivateManualOutput *qmi_message_dms_activate_manual_output_ref (QmiMessageDmsActivateManualOutput *self);
void qmi_message_dms_activate_manual_output_unref (QmiMessageDmsActivateManualOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get User Lock State */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetUserLockStateOutput:
 *
 * The #QmiMessageDmsGetUserLockStateOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetUserLockStateOutput QmiMessageDmsGetUserLockStateOutput;
GType qmi_message_dms_get_user_lock_state_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_USER_LOCK_STATE_OUTPUT (qmi_message_dms_get_user_lock_state_output_get_type ())

gboolean qmi_message_dms_get_user_lock_state_output_get_result (
    QmiMessageDmsGetUserLockStateOutput *self,
    GError **error);

gboolean qmi_message_dms_get_user_lock_state_output_get_enabled (
    QmiMessageDmsGetUserLockStateOutput *self,
    gboolean *enabled,
    GError **error);

QmiMessageDmsGetUserLockStateOutput *qmi_message_dms_get_user_lock_state_output_ref (QmiMessageDmsGetUserLockStateOutput *self);
void qmi_message_dms_get_user_lock_state_output_unref (QmiMessageDmsGetUserLockStateOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set User Lock State */


/* --- Input -- */

/**
 * QmiMessageDmsSetUserLockStateInput:
 *
 * The #QmiMessageDmsSetUserLockStateInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetUserLockStateInput QmiMessageDmsSetUserLockStateInput;
GType qmi_message_dms_set_user_lock_state_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_USER_LOCK_STATE_INPUT (qmi_message_dms_set_user_lock_state_input_get_type ())

gboolean qmi_message_dms_set_user_lock_state_input_get_info (
    QmiMessageDmsSetUserLockStateInput *self,
    gboolean *info_enabled,
    const gchar **info_lock_code,
    GError **error);

gboolean qmi_message_dms_set_user_lock_state_input_set_info (
    QmiMessageDmsSetUserLockStateInput *self,
    gboolean info_enabled,
    const gchar *info_lock_code,
    GError **error);

QmiMessageDmsSetUserLockStateInput *qmi_message_dms_set_user_lock_state_input_ref (QmiMessageDmsSetUserLockStateInput *self);
void qmi_message_dms_set_user_lock_state_input_unref (QmiMessageDmsSetUserLockStateInput *self);
QmiMessageDmsSetUserLockStateInput *qmi_message_dms_set_user_lock_state_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetUserLockStateOutput:
 *
 * The #QmiMessageDmsSetUserLockStateOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetUserLockStateOutput QmiMessageDmsSetUserLockStateOutput;
GType qmi_message_dms_set_user_lock_state_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_USER_LOCK_STATE_OUTPUT (qmi_message_dms_set_user_lock_state_output_get_type ())

gboolean qmi_message_dms_set_user_lock_state_output_get_result (
    QmiMessageDmsSetUserLockStateOutput *self,
    GError **error);

QmiMessageDmsSetUserLockStateOutput *qmi_message_dms_set_user_lock_state_output_ref (QmiMessageDmsSetUserLockStateOutput *self);
void qmi_message_dms_set_user_lock_state_output_unref (QmiMessageDmsSetUserLockStateOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set User Lock Code */


/* --- Input -- */

/**
 * QmiMessageDmsSetUserLockCodeInput:
 *
 * The #QmiMessageDmsSetUserLockCodeInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetUserLockCodeInput QmiMessageDmsSetUserLockCodeInput;
GType qmi_message_dms_set_user_lock_code_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_USER_LOCK_CODE_INPUT (qmi_message_dms_set_user_lock_code_input_get_type ())

gboolean qmi_message_dms_set_user_lock_code_input_get_info (
    QmiMessageDmsSetUserLockCodeInput *self,
    const gchar **info_old_code,
    const gchar **info_new_code,
    GError **error);

gboolean qmi_message_dms_set_user_lock_code_input_set_info (
    QmiMessageDmsSetUserLockCodeInput *self,
    const gchar *info_old_code,
    const gchar *info_new_code,
    GError **error);

QmiMessageDmsSetUserLockCodeInput *qmi_message_dms_set_user_lock_code_input_ref (QmiMessageDmsSetUserLockCodeInput *self);
void qmi_message_dms_set_user_lock_code_input_unref (QmiMessageDmsSetUserLockCodeInput *self);
QmiMessageDmsSetUserLockCodeInput *qmi_message_dms_set_user_lock_code_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetUserLockCodeOutput:
 *
 * The #QmiMessageDmsSetUserLockCodeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetUserLockCodeOutput QmiMessageDmsSetUserLockCodeOutput;
GType qmi_message_dms_set_user_lock_code_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_USER_LOCK_CODE_OUTPUT (qmi_message_dms_set_user_lock_code_output_get_type ())

gboolean qmi_message_dms_set_user_lock_code_output_get_result (
    QmiMessageDmsSetUserLockCodeOutput *self,
    GError **error);

QmiMessageDmsSetUserLockCodeOutput *qmi_message_dms_set_user_lock_code_output_ref (QmiMessageDmsSetUserLockCodeOutput *self);
void qmi_message_dms_set_user_lock_code_output_unref (QmiMessageDmsSetUserLockCodeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Read User Data */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsReadUserDataOutput:
 *
 * The #QmiMessageDmsReadUserDataOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsReadUserDataOutput QmiMessageDmsReadUserDataOutput;
GType qmi_message_dms_read_user_data_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_READ_USER_DATA_OUTPUT (qmi_message_dms_read_user_data_output_get_type ())

gboolean qmi_message_dms_read_user_data_output_get_result (
    QmiMessageDmsReadUserDataOutput *self,
    GError **error);

gboolean qmi_message_dms_read_user_data_output_get_user_data (
    QmiMessageDmsReadUserDataOutput *self,
    GArray **user_data,
    GError **error);

QmiMessageDmsReadUserDataOutput *qmi_message_dms_read_user_data_output_ref (QmiMessageDmsReadUserDataOutput *self);
void qmi_message_dms_read_user_data_output_unref (QmiMessageDmsReadUserDataOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Write User Data */


/* --- Input -- */

/**
 * QmiMessageDmsWriteUserDataInput:
 *
 * The #QmiMessageDmsWriteUserDataInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsWriteUserDataInput QmiMessageDmsWriteUserDataInput;
GType qmi_message_dms_write_user_data_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_WRITE_USER_DATA_INPUT (qmi_message_dms_write_user_data_input_get_type ())

gboolean qmi_message_dms_write_user_data_input_get_user_data (
    QmiMessageDmsWriteUserDataInput *self,
    GArray **user_data,
    GError **error);

gboolean qmi_message_dms_write_user_data_input_set_user_data (
    QmiMessageDmsWriteUserDataInput *self,
    GArray *user_data,
    GError **error);

QmiMessageDmsWriteUserDataInput *qmi_message_dms_write_user_data_input_ref (QmiMessageDmsWriteUserDataInput *self);
void qmi_message_dms_write_user_data_input_unref (QmiMessageDmsWriteUserDataInput *self);
QmiMessageDmsWriteUserDataInput *qmi_message_dms_write_user_data_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsWriteUserDataOutput:
 *
 * The #QmiMessageDmsWriteUserDataOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsWriteUserDataOutput QmiMessageDmsWriteUserDataOutput;
GType qmi_message_dms_write_user_data_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_WRITE_USER_DATA_OUTPUT (qmi_message_dms_write_user_data_output_get_type ())

gboolean qmi_message_dms_write_user_data_output_get_result (
    QmiMessageDmsWriteUserDataOutput *self,
    GError **error);

QmiMessageDmsWriteUserDataOutput *qmi_message_dms_write_user_data_output_ref (QmiMessageDmsWriteUserDataOutput *self);
void qmi_message_dms_write_user_data_output_unref (QmiMessageDmsWriteUserDataOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Read ERI File */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsReadEriFileOutput:
 *
 * The #QmiMessageDmsReadEriFileOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsReadEriFileOutput QmiMessageDmsReadEriFileOutput;
GType qmi_message_dms_read_eri_file_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_READ_ERI_FILE_OUTPUT (qmi_message_dms_read_eri_file_output_get_type ())

gboolean qmi_message_dms_read_eri_file_output_get_result (
    QmiMessageDmsReadEriFileOutput *self,
    GError **error);

gboolean qmi_message_dms_read_eri_file_output_get_eri_file (
    QmiMessageDmsReadEriFileOutput *self,
    GArray **eri_file,
    GError **error);

QmiMessageDmsReadEriFileOutput *qmi_message_dms_read_eri_file_output_ref (QmiMessageDmsReadEriFileOutput *self);
void qmi_message_dms_read_eri_file_output_unref (QmiMessageDmsReadEriFileOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Restore Factory Defaults */


/* --- Input -- */

/**
 * QmiMessageDmsRestoreFactoryDefaultsInput:
 *
 * The #QmiMessageDmsRestoreFactoryDefaultsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsRestoreFactoryDefaultsInput QmiMessageDmsRestoreFactoryDefaultsInput;
GType qmi_message_dms_restore_factory_defaults_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_RESTORE_FACTORY_DEFAULTS_INPUT (qmi_message_dms_restore_factory_defaults_input_get_type ())

gboolean qmi_message_dms_restore_factory_defaults_input_get_service_programming_code (
    QmiMessageDmsRestoreFactoryDefaultsInput *self,
    const gchar **service_programming_code,
    GError **error);

gboolean qmi_message_dms_restore_factory_defaults_input_set_service_programming_code (
    QmiMessageDmsRestoreFactoryDefaultsInput *self,
    const gchar *service_programming_code,
    GError **error);

QmiMessageDmsRestoreFactoryDefaultsInput *qmi_message_dms_restore_factory_defaults_input_ref (QmiMessageDmsRestoreFactoryDefaultsInput *self);
void qmi_message_dms_restore_factory_defaults_input_unref (QmiMessageDmsRestoreFactoryDefaultsInput *self);
QmiMessageDmsRestoreFactoryDefaultsInput *qmi_message_dms_restore_factory_defaults_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsRestoreFactoryDefaultsOutput:
 *
 * The #QmiMessageDmsRestoreFactoryDefaultsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsRestoreFactoryDefaultsOutput QmiMessageDmsRestoreFactoryDefaultsOutput;
GType qmi_message_dms_restore_factory_defaults_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_RESTORE_FACTORY_DEFAULTS_OUTPUT (qmi_message_dms_restore_factory_defaults_output_get_type ())

gboolean qmi_message_dms_restore_factory_defaults_output_get_result (
    QmiMessageDmsRestoreFactoryDefaultsOutput *self,
    GError **error);

QmiMessageDmsRestoreFactoryDefaultsOutput *qmi_message_dms_restore_factory_defaults_output_ref (QmiMessageDmsRestoreFactoryDefaultsOutput *self);
void qmi_message_dms_restore_factory_defaults_output_unref (QmiMessageDmsRestoreFactoryDefaultsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Validate Service Programming Code */


/* --- Input -- */

/**
 * QmiMessageDmsValidateServiceProgrammingCodeInput:
 *
 * The #QmiMessageDmsValidateServiceProgrammingCodeInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsValidateServiceProgrammingCodeInput QmiMessageDmsValidateServiceProgrammingCodeInput;
GType qmi_message_dms_validate_service_programming_code_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_VALIDATE_SERVICE_PROGRAMMING_CODE_INPUT (qmi_message_dms_validate_service_programming_code_input_get_type ())

gboolean qmi_message_dms_validate_service_programming_code_input_get_service_programming_code (
    QmiMessageDmsValidateServiceProgrammingCodeInput *self,
    const gchar **service_programming_code,
    GError **error);

gboolean qmi_message_dms_validate_service_programming_code_input_set_service_programming_code (
    QmiMessageDmsValidateServiceProgrammingCodeInput *self,
    const gchar *service_programming_code,
    GError **error);

QmiMessageDmsValidateServiceProgrammingCodeInput *qmi_message_dms_validate_service_programming_code_input_ref (QmiMessageDmsValidateServiceProgrammingCodeInput *self);
void qmi_message_dms_validate_service_programming_code_input_unref (QmiMessageDmsValidateServiceProgrammingCodeInput *self);
QmiMessageDmsValidateServiceProgrammingCodeInput *qmi_message_dms_validate_service_programming_code_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsValidateServiceProgrammingCodeOutput:
 *
 * The #QmiMessageDmsValidateServiceProgrammingCodeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsValidateServiceProgrammingCodeOutput QmiMessageDmsValidateServiceProgrammingCodeOutput;
GType qmi_message_dms_validate_service_programming_code_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_VALIDATE_SERVICE_PROGRAMMING_CODE_OUTPUT (qmi_message_dms_validate_service_programming_code_output_get_type ())

gboolean qmi_message_dms_validate_service_programming_code_output_get_result (
    QmiMessageDmsValidateServiceProgrammingCodeOutput *self,
    GError **error);

QmiMessageDmsValidateServiceProgrammingCodeOutput *qmi_message_dms_validate_service_programming_code_output_ref (QmiMessageDmsValidateServiceProgrammingCodeOutput *self);
void qmi_message_dms_validate_service_programming_code_output_unref (QmiMessageDmsValidateServiceProgrammingCodeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Get ICCID */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsUimGetIccidOutput:
 *
 * The #QmiMessageDmsUimGetIccidOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetIccidOutput QmiMessageDmsUimGetIccidOutput;
GType qmi_message_dms_uim_get_iccid_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_ICCID_OUTPUT (qmi_message_dms_uim_get_iccid_output_get_type ())

gboolean qmi_message_dms_uim_get_iccid_output_get_result (
    QmiMessageDmsUimGetIccidOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_get_iccid_output_get_iccid (
    QmiMessageDmsUimGetIccidOutput *self,
    const gchar **iccid,
    GError **error);

QmiMessageDmsUimGetIccidOutput *qmi_message_dms_uim_get_iccid_output_ref (QmiMessageDmsUimGetIccidOutput *self);
void qmi_message_dms_uim_get_iccid_output_unref (QmiMessageDmsUimGetIccidOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Get CK Status */


/* --- Input -- */

/**
 * QmiMessageDmsUimGetCkStatusInput:
 *
 * The #QmiMessageDmsUimGetCkStatusInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetCkStatusInput QmiMessageDmsUimGetCkStatusInput;
GType qmi_message_dms_uim_get_ck_status_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_CK_STATUS_INPUT (qmi_message_dms_uim_get_ck_status_input_get_type ())

gboolean qmi_message_dms_uim_get_ck_status_input_get_facility (
    QmiMessageDmsUimGetCkStatusInput *self,
    QmiDmsUimFacility *facility,
    GError **error);

gboolean qmi_message_dms_uim_get_ck_status_input_set_facility (
    QmiMessageDmsUimGetCkStatusInput *self,
    QmiDmsUimFacility facility,
    GError **error);

QmiMessageDmsUimGetCkStatusInput *qmi_message_dms_uim_get_ck_status_input_ref (QmiMessageDmsUimGetCkStatusInput *self);
void qmi_message_dms_uim_get_ck_status_input_unref (QmiMessageDmsUimGetCkStatusInput *self);
QmiMessageDmsUimGetCkStatusInput *qmi_message_dms_uim_get_ck_status_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimGetCkStatusOutput:
 *
 * The #QmiMessageDmsUimGetCkStatusOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetCkStatusOutput QmiMessageDmsUimGetCkStatusOutput;
GType qmi_message_dms_uim_get_ck_status_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_CK_STATUS_OUTPUT (qmi_message_dms_uim_get_ck_status_output_get_type ())

gboolean qmi_message_dms_uim_get_ck_status_output_get_result (
    QmiMessageDmsUimGetCkStatusOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_get_ck_status_output_get_ck_status (
    QmiMessageDmsUimGetCkStatusOutput *self,
    QmiDmsUimFacilityState *ck_status_facility_state,
    guint8 *ck_status_verify_retries_left,
    guint8 *ck_status_unblock_retries_left,
    GError **error);

gboolean qmi_message_dms_uim_get_ck_status_output_get_operation_blocking_facility (
    QmiMessageDmsUimGetCkStatusOutput *self,
    gboolean *operation_blocking_facility,
    GError **error);

QmiMessageDmsUimGetCkStatusOutput *qmi_message_dms_uim_get_ck_status_output_ref (QmiMessageDmsUimGetCkStatusOutput *self);
void qmi_message_dms_uim_get_ck_status_output_unref (QmiMessageDmsUimGetCkStatusOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Set CK Protection */


/* --- Input -- */

/**
 * QmiMessageDmsUimSetCkProtectionInput:
 *
 * The #QmiMessageDmsUimSetCkProtectionInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimSetCkProtectionInput QmiMessageDmsUimSetCkProtectionInput;
GType qmi_message_dms_uim_set_ck_protection_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_SET_CK_PROTECTION_INPUT (qmi_message_dms_uim_set_ck_protection_input_get_type ())

gboolean qmi_message_dms_uim_set_ck_protection_input_get_facility (
    QmiMessageDmsUimSetCkProtectionInput *self,
    QmiDmsUimFacility *facility_facility,
    QmiDmsUimFacilityState *facility_facility_state,
    const gchar **facility_facility_depersonalization_control_key,
    GError **error);

gboolean qmi_message_dms_uim_set_ck_protection_input_set_facility (
    QmiMessageDmsUimSetCkProtectionInput *self,
    QmiDmsUimFacility facility_facility,
    QmiDmsUimFacilityState facility_facility_state,
    const gchar *facility_facility_depersonalization_control_key,
    GError **error);

QmiMessageDmsUimSetCkProtectionInput *qmi_message_dms_uim_set_ck_protection_input_ref (QmiMessageDmsUimSetCkProtectionInput *self);
void qmi_message_dms_uim_set_ck_protection_input_unref (QmiMessageDmsUimSetCkProtectionInput *self);
QmiMessageDmsUimSetCkProtectionInput *qmi_message_dms_uim_set_ck_protection_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimSetCkProtectionOutput:
 *
 * The #QmiMessageDmsUimSetCkProtectionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimSetCkProtectionOutput QmiMessageDmsUimSetCkProtectionOutput;
GType qmi_message_dms_uim_set_ck_protection_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_SET_CK_PROTECTION_OUTPUT (qmi_message_dms_uim_set_ck_protection_output_get_type ())

gboolean qmi_message_dms_uim_set_ck_protection_output_get_result (
    QmiMessageDmsUimSetCkProtectionOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_set_ck_protection_output_get_verify_retries_left (
    QmiMessageDmsUimSetCkProtectionOutput *self,
    guint8 *verify_retries_left,
    GError **error);

QmiMessageDmsUimSetCkProtectionOutput *qmi_message_dms_uim_set_ck_protection_output_ref (QmiMessageDmsUimSetCkProtectionOutput *self);
void qmi_message_dms_uim_set_ck_protection_output_unref (QmiMessageDmsUimSetCkProtectionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Unblock CK */


/* --- Input -- */

/**
 * QmiMessageDmsUimUnblockCkInput:
 *
 * The #QmiMessageDmsUimUnblockCkInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimUnblockCkInput QmiMessageDmsUimUnblockCkInput;
GType qmi_message_dms_uim_unblock_ck_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_UNBLOCK_CK_INPUT (qmi_message_dms_uim_unblock_ck_input_get_type ())

gboolean qmi_message_dms_uim_unblock_ck_input_get_facility (
    QmiMessageDmsUimUnblockCkInput *self,
    QmiDmsUimFacility *facility_facility,
    const gchar **facility_facility_control_key,
    GError **error);

gboolean qmi_message_dms_uim_unblock_ck_input_set_facility (
    QmiMessageDmsUimUnblockCkInput *self,
    QmiDmsUimFacility facility_facility,
    const gchar *facility_facility_control_key,
    GError **error);

QmiMessageDmsUimUnblockCkInput *qmi_message_dms_uim_unblock_ck_input_ref (QmiMessageDmsUimUnblockCkInput *self);
void qmi_message_dms_uim_unblock_ck_input_unref (QmiMessageDmsUimUnblockCkInput *self);
QmiMessageDmsUimUnblockCkInput *qmi_message_dms_uim_unblock_ck_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsUimUnblockCkOutput:
 *
 * The #QmiMessageDmsUimUnblockCkOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimUnblockCkOutput QmiMessageDmsUimUnblockCkOutput;
GType qmi_message_dms_uim_unblock_ck_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_UNBLOCK_CK_OUTPUT (qmi_message_dms_uim_unblock_ck_output_get_type ())

gboolean qmi_message_dms_uim_unblock_ck_output_get_result (
    QmiMessageDmsUimUnblockCkOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_unblock_ck_output_get_unblock_retries_left (
    QmiMessageDmsUimUnblockCkOutput *self,
    guint8 *unblock_retries_left,
    GError **error);

QmiMessageDmsUimUnblockCkOutput *qmi_message_dms_uim_unblock_ck_output_ref (QmiMessageDmsUimUnblockCkOutput *self);
void qmi_message_dms_uim_unblock_ck_output_unref (QmiMessageDmsUimUnblockCkOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Get IMSI */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsUimGetImsiOutput:
 *
 * The #QmiMessageDmsUimGetImsiOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetImsiOutput QmiMessageDmsUimGetImsiOutput;
GType qmi_message_dms_uim_get_imsi_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_IMSI_OUTPUT (qmi_message_dms_uim_get_imsi_output_get_type ())

gboolean qmi_message_dms_uim_get_imsi_output_get_result (
    QmiMessageDmsUimGetImsiOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_get_imsi_output_get_imsi (
    QmiMessageDmsUimGetImsiOutput *self,
    const gchar **imsi,
    GError **error);

QmiMessageDmsUimGetImsiOutput *qmi_message_dms_uim_get_imsi_output_ref (QmiMessageDmsUimGetImsiOutput *self);
void qmi_message_dms_uim_get_imsi_output_unref (QmiMessageDmsUimGetImsiOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS UIM Get State */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsUimGetStateOutput:
 *
 * The #QmiMessageDmsUimGetStateOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsUimGetStateOutput QmiMessageDmsUimGetStateOutput;
GType qmi_message_dms_uim_get_state_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_UIM_GET_STATE_OUTPUT (qmi_message_dms_uim_get_state_output_get_type ())

gboolean qmi_message_dms_uim_get_state_output_get_result (
    QmiMessageDmsUimGetStateOutput *self,
    GError **error);

gboolean qmi_message_dms_uim_get_state_output_get_state (
    QmiMessageDmsUimGetStateOutput *self,
    QmiDmsUimState *state,
    GError **error);

QmiMessageDmsUimGetStateOutput *qmi_message_dms_uim_get_state_output_ref (QmiMessageDmsUimGetStateOutput *self);
void qmi_message_dms_uim_get_state_output_unref (QmiMessageDmsUimGetStateOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Band Capabilities */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetBandCapabilitiesOutput:
 *
 * The #QmiMessageDmsGetBandCapabilitiesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetBandCapabilitiesOutput QmiMessageDmsGetBandCapabilitiesOutput;
GType qmi_message_dms_get_band_capabilities_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_BAND_CAPABILITIES_OUTPUT (qmi_message_dms_get_band_capabilities_output_get_type ())

gboolean qmi_message_dms_get_band_capabilities_output_get_result (
    QmiMessageDmsGetBandCapabilitiesOutput *self,
    GError **error);

gboolean qmi_message_dms_get_band_capabilities_output_get_band_capability (
    QmiMessageDmsGetBandCapabilitiesOutput *self,
    QmiDmsBandCapability *band_capability,
    GError **error);

gboolean qmi_message_dms_get_band_capabilities_output_get_lte_band_capability (
    QmiMessageDmsGetBandCapabilitiesOutput *self,
    QmiDmsLteBandCapability *lte_band_capability,
    GError **error);

QmiMessageDmsGetBandCapabilitiesOutput *qmi_message_dms_get_band_capabilities_output_ref (QmiMessageDmsGetBandCapabilitiesOutput *self);
void qmi_message_dms_get_band_capabilities_output_unref (QmiMessageDmsGetBandCapabilitiesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Factory SKU */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetFactorySkuOutput:
 *
 * The #QmiMessageDmsGetFactorySkuOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetFactorySkuOutput QmiMessageDmsGetFactorySkuOutput;
GType qmi_message_dms_get_factory_sku_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_FACTORY_SKU_OUTPUT (qmi_message_dms_get_factory_sku_output_get_type ())

gboolean qmi_message_dms_get_factory_sku_output_get_result (
    QmiMessageDmsGetFactorySkuOutput *self,
    GError **error);

gboolean qmi_message_dms_get_factory_sku_output_get_sku (
    QmiMessageDmsGetFactorySkuOutput *self,
    const gchar **sku,
    GError **error);

QmiMessageDmsGetFactorySkuOutput *qmi_message_dms_get_factory_sku_output_ref (QmiMessageDmsGetFactorySkuOutput *self);
void qmi_message_dms_get_factory_sku_output_unref (QmiMessageDmsGetFactorySkuOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Firmware Preference */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetFirmwarePreferenceOutputListImage:
 * @type: a #QmiDmsFirmwareImageType.
 * @unique_id: a #GArray of #guint8 elements.
 * @build_id: a string.
 *
 * A QmiMessageDmsGetFirmwarePreferenceOutputListImage struct.
 */
typedef struct _QmiMessageDmsGetFirmwarePreferenceOutputListImage {
    QmiDmsFirmwareImageType type;
    GArray * unique_id;
    gchar * build_id;
} QmiMessageDmsGetFirmwarePreferenceOutputListImage;

/**
 * QmiMessageDmsGetFirmwarePreferenceOutput:
 *
 * The #QmiMessageDmsGetFirmwarePreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetFirmwarePreferenceOutput QmiMessageDmsGetFirmwarePreferenceOutput;
GType qmi_message_dms_get_firmware_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_FIRMWARE_PREFERENCE_OUTPUT (qmi_message_dms_get_firmware_preference_output_get_type ())

gboolean qmi_message_dms_get_firmware_preference_output_get_result (
    QmiMessageDmsGetFirmwarePreferenceOutput *self,
    GError **error);

gboolean qmi_message_dms_get_firmware_preference_output_get_list (
    QmiMessageDmsGetFirmwarePreferenceOutput *self,
    GArray **list,
    GError **error);

QmiMessageDmsGetFirmwarePreferenceOutput *qmi_message_dms_get_firmware_preference_output_ref (QmiMessageDmsGetFirmwarePreferenceOutput *self);
void qmi_message_dms_get_firmware_preference_output_unref (QmiMessageDmsGetFirmwarePreferenceOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Firmware Preference */


/* --- Input -- */

/**
 * QmiMessageDmsSetFirmwarePreferenceInputListImage:
 * @type: a #QmiDmsFirmwareImageType.
 * @unique_id: a #GArray of #guint8 elements.
 * @build_id: a string.
 *
 * A QmiMessageDmsSetFirmwarePreferenceInputListImage struct.
 */
typedef struct _QmiMessageDmsSetFirmwarePreferenceInputListImage {
    QmiDmsFirmwareImageType type;
    GArray * unique_id;
    gchar * build_id;
} QmiMessageDmsSetFirmwarePreferenceInputListImage;

/**
 * QmiMessageDmsSetFirmwarePreferenceInput:
 *
 * The #QmiMessageDmsSetFirmwarePreferenceInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetFirmwarePreferenceInput QmiMessageDmsSetFirmwarePreferenceInput;
GType qmi_message_dms_set_firmware_preference_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_FIRMWARE_PREFERENCE_INPUT (qmi_message_dms_set_firmware_preference_input_get_type ())

gboolean qmi_message_dms_set_firmware_preference_input_get_modem_storage_index (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    guint8 *modem_storage_index,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_input_set_modem_storage_index (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    guint8 modem_storage_index,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_input_get_download_override (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    gboolean *download_override,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_input_set_download_override (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    gboolean download_override,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_input_get_list (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    GArray **list,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_input_set_list (
    QmiMessageDmsSetFirmwarePreferenceInput *self,
    GArray *list,
    GError **error);

QmiMessageDmsSetFirmwarePreferenceInput *qmi_message_dms_set_firmware_preference_input_ref (QmiMessageDmsSetFirmwarePreferenceInput *self);
void qmi_message_dms_set_firmware_preference_input_unref (QmiMessageDmsSetFirmwarePreferenceInput *self);
QmiMessageDmsSetFirmwarePreferenceInput *qmi_message_dms_set_firmware_preference_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetFirmwarePreferenceOutput:
 *
 * The #QmiMessageDmsSetFirmwarePreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetFirmwarePreferenceOutput QmiMessageDmsSetFirmwarePreferenceOutput;
GType qmi_message_dms_set_firmware_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_FIRMWARE_PREFERENCE_OUTPUT (qmi_message_dms_set_firmware_preference_output_get_type ())

gboolean qmi_message_dms_set_firmware_preference_output_get_result (
    QmiMessageDmsSetFirmwarePreferenceOutput *self,
    GError **error);

gboolean qmi_message_dms_set_firmware_preference_output_get_image_download_list (
    QmiMessageDmsSetFirmwarePreferenceOutput *self,
    GArray **image_download_list,
    GError **error);

QmiMessageDmsSetFirmwarePreferenceOutput *qmi_message_dms_set_firmware_preference_output_ref (QmiMessageDmsSetFirmwarePreferenceOutput *self);
void qmi_message_dms_set_firmware_preference_output_unref (QmiMessageDmsSetFirmwarePreferenceOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS List Stored Images */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsListStoredImagesOutputListImageSublistSublistElement:
 * @storage_index: a #guint8.
 * @failure_count: a #guint8.
 * @unique_id: a #GArray of #guint8 elements.
 * @build_id: a string.
 *
 * A QmiMessageDmsListStoredImagesOutputListImageSublistSublistElement struct.
 */
typedef struct _QmiMessageDmsListStoredImagesOutputListImageSublistSublistElement {
    guint8 storage_index;
    guint8 failure_count;
    GArray * unique_id;
    gchar * build_id;
} QmiMessageDmsListStoredImagesOutputListImageSublistSublistElement;

/**
 * QmiMessageDmsListStoredImagesOutputListImage:
 * @type: a #QmiDmsFirmwareImageType.
 * @maximum_images: a #guint8.
 * @index_of_running_image: a #guint8.
 * @sublist: a #GArray of #QmiMessageDmsListStoredImagesOutputListImageSublistSublistElement elements.
 *
 * A QmiMessageDmsListStoredImagesOutputListImage struct.
 */
typedef struct _QmiMessageDmsListStoredImagesOutputListImage {
    QmiDmsFirmwareImageType type;
    guint8 maximum_images;
    guint8 index_of_running_image;
    GArray * sublist;
} QmiMessageDmsListStoredImagesOutputListImage;

/**
 * QmiMessageDmsListStoredImagesOutput:
 *
 * The #QmiMessageDmsListStoredImagesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsListStoredImagesOutput QmiMessageDmsListStoredImagesOutput;
GType qmi_message_dms_list_stored_images_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_LIST_STORED_IMAGES_OUTPUT (qmi_message_dms_list_stored_images_output_get_type ())

gboolean qmi_message_dms_list_stored_images_output_get_result (
    QmiMessageDmsListStoredImagesOutput *self,
    GError **error);

gboolean qmi_message_dms_list_stored_images_output_get_list (
    QmiMessageDmsListStoredImagesOutput *self,
    GArray **list,
    GError **error);

QmiMessageDmsListStoredImagesOutput *qmi_message_dms_list_stored_images_output_ref (QmiMessageDmsListStoredImagesOutput *self);
void qmi_message_dms_list_stored_images_output_unref (QmiMessageDmsListStoredImagesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Delete Stored Image */


/* --- Input -- */

/**
 * QmiMessageDmsDeleteStoredImageInputImage:
 * @type: a #QmiDmsFirmwareImageType.
 * @unique_id: a #GArray of #guint8 elements.
 * @build_id: a string.
 *
 * A QmiMessageDmsDeleteStoredImageInputImage struct.
 */
typedef struct _QmiMessageDmsDeleteStoredImageInputImage {
    QmiDmsFirmwareImageType type;
    GArray * unique_id;
    gchar * build_id;
} QmiMessageDmsDeleteStoredImageInputImage;

/**
 * QmiMessageDmsDeleteStoredImageInput:
 *
 * The #QmiMessageDmsDeleteStoredImageInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsDeleteStoredImageInput QmiMessageDmsDeleteStoredImageInput;
GType qmi_message_dms_delete_stored_image_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_DELETE_STORED_IMAGE_INPUT (qmi_message_dms_delete_stored_image_input_get_type ())

gboolean qmi_message_dms_delete_stored_image_input_get_image (
    QmiMessageDmsDeleteStoredImageInput *self,
    QmiMessageDmsDeleteStoredImageInputImage *image,
    GError **error);

gboolean qmi_message_dms_delete_stored_image_input_set_image (
    QmiMessageDmsDeleteStoredImageInput *self,
    const QmiMessageDmsDeleteStoredImageInputImage *image,
    GError **error);

QmiMessageDmsDeleteStoredImageInput *qmi_message_dms_delete_stored_image_input_ref (QmiMessageDmsDeleteStoredImageInput *self);
void qmi_message_dms_delete_stored_image_input_unref (QmiMessageDmsDeleteStoredImageInput *self);
QmiMessageDmsDeleteStoredImageInput *qmi_message_dms_delete_stored_image_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsDeleteStoredImageOutput:
 *
 * The #QmiMessageDmsDeleteStoredImageOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsDeleteStoredImageOutput QmiMessageDmsDeleteStoredImageOutput;
GType qmi_message_dms_delete_stored_image_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_DELETE_STORED_IMAGE_OUTPUT (qmi_message_dms_delete_stored_image_output_get_type ())

gboolean qmi_message_dms_delete_stored_image_output_get_result (
    QmiMessageDmsDeleteStoredImageOutput *self,
    GError **error);

QmiMessageDmsDeleteStoredImageOutput *qmi_message_dms_delete_stored_image_output_ref (QmiMessageDmsDeleteStoredImageOutput *self);
void qmi_message_dms_delete_stored_image_output_unref (QmiMessageDmsDeleteStoredImageOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Time */


/* --- Input -- */

/**
 * QmiMessageDmsSetTimeInput:
 *
 * The #QmiMessageDmsSetTimeInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetTimeInput QmiMessageDmsSetTimeInput;
GType qmi_message_dms_set_time_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_TIME_INPUT (qmi_message_dms_set_time_input_get_type ())

gboolean qmi_message_dms_set_time_input_get_time_reference_type (
    QmiMessageDmsSetTimeInput *self,
    QmiDmsTimeReferenceType *time_reference_type,
    GError **error);

gboolean qmi_message_dms_set_time_input_set_time_reference_type (
    QmiMessageDmsSetTimeInput *self,
    QmiDmsTimeReferenceType time_reference_type,
    GError **error);

gboolean qmi_message_dms_set_time_input_get_time_value (
    QmiMessageDmsSetTimeInput *self,
    guint64 *time_value,
    GError **error);

gboolean qmi_message_dms_set_time_input_set_time_value (
    QmiMessageDmsSetTimeInput *self,
    guint64 time_value,
    GError **error);

QmiMessageDmsSetTimeInput *qmi_message_dms_set_time_input_ref (QmiMessageDmsSetTimeInput *self);
void qmi_message_dms_set_time_input_unref (QmiMessageDmsSetTimeInput *self);
QmiMessageDmsSetTimeInput *qmi_message_dms_set_time_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetTimeOutput:
 *
 * The #QmiMessageDmsSetTimeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetTimeOutput QmiMessageDmsSetTimeOutput;
GType qmi_message_dms_set_time_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_TIME_OUTPUT (qmi_message_dms_set_time_output_get_type ())

gboolean qmi_message_dms_set_time_output_get_result (
    QmiMessageDmsSetTimeOutput *self,
    GError **error);

QmiMessageDmsSetTimeOutput *qmi_message_dms_set_time_output_ref (QmiMessageDmsSetTimeOutput *self);
void qmi_message_dms_set_time_output_unref (QmiMessageDmsSetTimeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Stored Image Info */


/* --- Input -- */

/**
 * QmiMessageDmsGetStoredImageInfoInputImage:
 * @type: a #QmiDmsFirmwareImageType.
 * @unique_id: a #GArray of #guint8 elements.
 * @build_id: a string.
 *
 * A QmiMessageDmsGetStoredImageInfoInputImage struct.
 */
typedef struct _QmiMessageDmsGetStoredImageInfoInputImage {
    QmiDmsFirmwareImageType type;
    GArray * unique_id;
    gchar * build_id;
} QmiMessageDmsGetStoredImageInfoInputImage;

/**
 * QmiMessageDmsGetStoredImageInfoInput:
 *
 * The #QmiMessageDmsGetStoredImageInfoInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetStoredImageInfoInput QmiMessageDmsGetStoredImageInfoInput;
GType qmi_message_dms_get_stored_image_info_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_STORED_IMAGE_INFO_INPUT (qmi_message_dms_get_stored_image_info_input_get_type ())

gboolean qmi_message_dms_get_stored_image_info_input_get_image (
    QmiMessageDmsGetStoredImageInfoInput *self,
    QmiMessageDmsGetStoredImageInfoInputImage *image,
    GError **error);

gboolean qmi_message_dms_get_stored_image_info_input_set_image (
    QmiMessageDmsGetStoredImageInfoInput *self,
    const QmiMessageDmsGetStoredImageInfoInputImage *image,
    GError **error);

QmiMessageDmsGetStoredImageInfoInput *qmi_message_dms_get_stored_image_info_input_ref (QmiMessageDmsGetStoredImageInfoInput *self);
void qmi_message_dms_get_stored_image_info_input_unref (QmiMessageDmsGetStoredImageInfoInput *self);
QmiMessageDmsGetStoredImageInfoInput *qmi_message_dms_get_stored_image_info_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsGetStoredImageInfoOutput:
 *
 * The #QmiMessageDmsGetStoredImageInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetStoredImageInfoOutput QmiMessageDmsGetStoredImageInfoOutput;
GType qmi_message_dms_get_stored_image_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_STORED_IMAGE_INFO_OUTPUT (qmi_message_dms_get_stored_image_info_output_get_type ())

gboolean qmi_message_dms_get_stored_image_info_output_get_result (
    QmiMessageDmsGetStoredImageInfoOutput *self,
    GError **error);

gboolean qmi_message_dms_get_stored_image_info_output_get_boot_version (
    QmiMessageDmsGetStoredImageInfoOutput *self,
    guint16 *boot_version_major_version,
    guint16 *boot_version_minor_version,
    GError **error);

gboolean qmi_message_dms_get_stored_image_info_output_get_pri_version (
    QmiMessageDmsGetStoredImageInfoOutput *self,
    guint32 *pri_version_pri_version,
    const gchar **pri_version_pri_info,
    GError **error);

gboolean qmi_message_dms_get_stored_image_info_output_get_oem_lock_id (
    QmiMessageDmsGetStoredImageInfoOutput *self,
    guint32 *oem_lock_id,
    GError **error);

QmiMessageDmsGetStoredImageInfoOutput *qmi_message_dms_get_stored_image_info_output_ref (QmiMessageDmsGetStoredImageInfoOutput *self);
void qmi_message_dms_get_stored_image_info_output_unref (QmiMessageDmsGetStoredImageInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Alt Net Config */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetAltNetConfigOutput:
 *
 * The #QmiMessageDmsGetAltNetConfigOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetAltNetConfigOutput QmiMessageDmsGetAltNetConfigOutput;
GType qmi_message_dms_get_alt_net_config_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_ALT_NET_CONFIG_OUTPUT (qmi_message_dms_get_alt_net_config_output_get_type ())

gboolean qmi_message_dms_get_alt_net_config_output_get_result (
    QmiMessageDmsGetAltNetConfigOutput *self,
    GError **error);

gboolean qmi_message_dms_get_alt_net_config_output_get_config (
    QmiMessageDmsGetAltNetConfigOutput *self,
    gboolean *config,
    GError **error);

QmiMessageDmsGetAltNetConfigOutput *qmi_message_dms_get_alt_net_config_output_ref (QmiMessageDmsGetAltNetConfigOutput *self);
void qmi_message_dms_get_alt_net_config_output_unref (QmiMessageDmsGetAltNetConfigOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Alt Net Config */


/* --- Input -- */

/**
 * QmiMessageDmsSetAltNetConfigInput:
 *
 * The #QmiMessageDmsSetAltNetConfigInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetAltNetConfigInput QmiMessageDmsSetAltNetConfigInput;
GType qmi_message_dms_set_alt_net_config_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_ALT_NET_CONFIG_INPUT (qmi_message_dms_set_alt_net_config_input_get_type ())

gboolean qmi_message_dms_set_alt_net_config_input_get_config (
    QmiMessageDmsSetAltNetConfigInput *self,
    gboolean *config,
    GError **error);

gboolean qmi_message_dms_set_alt_net_config_input_set_config (
    QmiMessageDmsSetAltNetConfigInput *self,
    gboolean config,
    GError **error);

QmiMessageDmsSetAltNetConfigInput *qmi_message_dms_set_alt_net_config_input_ref (QmiMessageDmsSetAltNetConfigInput *self);
void qmi_message_dms_set_alt_net_config_input_unref (QmiMessageDmsSetAltNetConfigInput *self);
QmiMessageDmsSetAltNetConfigInput *qmi_message_dms_set_alt_net_config_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetAltNetConfigOutput:
 *
 * The #QmiMessageDmsSetAltNetConfigOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetAltNetConfigOutput QmiMessageDmsSetAltNetConfigOutput;
GType qmi_message_dms_set_alt_net_config_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_ALT_NET_CONFIG_OUTPUT (qmi_message_dms_set_alt_net_config_output_get_type ())

gboolean qmi_message_dms_set_alt_net_config_output_get_result (
    QmiMessageDmsSetAltNetConfigOutput *self,
    GError **error);

QmiMessageDmsSetAltNetConfigOutput *qmi_message_dms_set_alt_net_config_output_ref (QmiMessageDmsSetAltNetConfigOutput *self);
void qmi_message_dms_set_alt_net_config_output_unref (QmiMessageDmsSetAltNetConfigOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Get Software Version */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageDmsGetSoftwareVersionOutput:
 *
 * The #QmiMessageDmsGetSoftwareVersionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsGetSoftwareVersionOutput QmiMessageDmsGetSoftwareVersionOutput;
GType qmi_message_dms_get_software_version_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_GET_SOFTWARE_VERSION_OUTPUT (qmi_message_dms_get_software_version_output_get_type ())

gboolean qmi_message_dms_get_software_version_output_get_result (
    QmiMessageDmsGetSoftwareVersionOutput *self,
    GError **error);

gboolean qmi_message_dms_get_software_version_output_get_version (
    QmiMessageDmsGetSoftwareVersionOutput *self,
    const gchar **version,
    GError **error);

QmiMessageDmsGetSoftwareVersionOutput *qmi_message_dms_get_software_version_output_ref (QmiMessageDmsGetSoftwareVersionOutput *self);
void qmi_message_dms_get_software_version_output_unref (QmiMessageDmsGetSoftwareVersionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message DMS Set Service Programming Code */


/* --- Input -- */

/**
 * QmiMessageDmsSetServiceProgrammingCodeInput:
 *
 * The #QmiMessageDmsSetServiceProgrammingCodeInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetServiceProgrammingCodeInput QmiMessageDmsSetServiceProgrammingCodeInput;
GType qmi_message_dms_set_service_programming_code_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_SERVICE_PROGRAMMING_CODE_INPUT (qmi_message_dms_set_service_programming_code_input_get_type ())

gboolean qmi_message_dms_set_service_programming_code_input_get_new (
    QmiMessageDmsSetServiceProgrammingCodeInput *self,
    const gchar **new,
    GError **error);

gboolean qmi_message_dms_set_service_programming_code_input_set_new (
    QmiMessageDmsSetServiceProgrammingCodeInput *self,
    const gchar *new,
    GError **error);

gboolean qmi_message_dms_set_service_programming_code_input_get_current (
    QmiMessageDmsSetServiceProgrammingCodeInput *self,
    const gchar **current,
    GError **error);

gboolean qmi_message_dms_set_service_programming_code_input_set_current (
    QmiMessageDmsSetServiceProgrammingCodeInput *self,
    const gchar *current,
    GError **error);

QmiMessageDmsSetServiceProgrammingCodeInput *qmi_message_dms_set_service_programming_code_input_ref (QmiMessageDmsSetServiceProgrammingCodeInput *self);
void qmi_message_dms_set_service_programming_code_input_unref (QmiMessageDmsSetServiceProgrammingCodeInput *self);
QmiMessageDmsSetServiceProgrammingCodeInput *qmi_message_dms_set_service_programming_code_input_new (void);

/* --- Output -- */

/**
 * QmiMessageDmsSetServiceProgrammingCodeOutput:
 *
 * The #QmiMessageDmsSetServiceProgrammingCodeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageDmsSetServiceProgrammingCodeOutput QmiMessageDmsSetServiceProgrammingCodeOutput;
GType qmi_message_dms_set_service_programming_code_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_DMS_SET_SERVICE_PROGRAMMING_CODE_OUTPUT (qmi_message_dms_set_service_programming_code_output_get_type ())

gboolean qmi_message_dms_set_service_programming_code_output_get_result (
    QmiMessageDmsSetServiceProgrammingCodeOutput *self,
    GError **error);

QmiMessageDmsSetServiceProgrammingCodeOutput *qmi_message_dms_set_service_programming_code_output_ref (QmiMessageDmsSetServiceProgrammingCodeOutput *self);
void qmi_message_dms_set_service_programming_code_output_unref (QmiMessageDmsSetServiceProgrammingCodeOutput *self);

/*****************************************************************************/
/* Service-specific printable: DMS */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_dms_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_dms_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client DMS */

#define QMI_TYPE_CLIENT_DMS            (qmi_client_dms_get_type ())
#define QMI_CLIENT_DMS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_DMS, QmiClientDms))
#define QMI_CLIENT_DMS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_DMS, QmiClientDmsClass))
#define QMI_IS_CLIENT_DMS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_DMS))
#define QMI_IS_CLIENT_DMS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_DMS))
#define QMI_CLIENT_DMS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_DMS, QmiClientDmsClass))

typedef struct _QmiClientDms QmiClientDms;
typedef struct _QmiClientDmsClass QmiClientDmsClass;

/**
 * QmiClientDms:
 *
 * The #QmiClientDms structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientDms {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientDmsClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_dms_get_type (void);


void qmi_client_dms_reset (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsResetOutput *qmi_client_dms_reset_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_event_report (
    QmiClientDms *self,
    QmiMessageDmsSetEventReportInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetEventReportOutput *qmi_client_dms_set_event_report_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_capabilities (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetCapabilitiesOutput *qmi_client_dms_get_capabilities_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_manufacturer (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetManufacturerOutput *qmi_client_dms_get_manufacturer_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_model (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetModelOutput *qmi_client_dms_get_model_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_revision (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetRevisionOutput *qmi_client_dms_get_revision_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_msisdn (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetMsisdnOutput *qmi_client_dms_get_msisdn_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_ids (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetIdsOutput *qmi_client_dms_get_ids_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_power_state (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetPowerStateOutput *qmi_client_dms_get_power_state_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_set_pin_protection (
    QmiClientDms *self,
    QmiMessageDmsUimSetPinProtectionInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimSetPinProtectionOutput *qmi_client_dms_uim_set_pin_protection_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_verify_pin (
    QmiClientDms *self,
    QmiMessageDmsUimVerifyPinInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimVerifyPinOutput *qmi_client_dms_uim_verify_pin_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_unblock_pin (
    QmiClientDms *self,
    QmiMessageDmsUimUnblockPinInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimUnblockPinOutput *qmi_client_dms_uim_unblock_pin_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_change_pin (
    QmiClientDms *self,
    QmiMessageDmsUimChangePinInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimChangePinOutput *qmi_client_dms_uim_change_pin_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_get_pin_status (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimGetPinStatusOutput *qmi_client_dms_uim_get_pin_status_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_hardware_revision (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetHardwareRevisionOutput *qmi_client_dms_get_hardware_revision_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_operating_mode (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetOperatingModeOutput *qmi_client_dms_get_operating_mode_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_operating_mode (
    QmiClientDms *self,
    QmiMessageDmsSetOperatingModeInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetOperatingModeOutput *qmi_client_dms_set_operating_mode_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_time (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetTimeOutput *qmi_client_dms_get_time_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_prl_version (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetPrlVersionOutput *qmi_client_dms_get_prl_version_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_activation_state (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetActivationStateOutput *qmi_client_dms_get_activation_state_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_activate_automatic (
    QmiClientDms *self,
    QmiMessageDmsActivateAutomaticInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsActivateAutomaticOutput *qmi_client_dms_activate_automatic_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_activate_manual (
    QmiClientDms *self,
    QmiMessageDmsActivateManualInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsActivateManualOutput *qmi_client_dms_activate_manual_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_user_lock_state (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetUserLockStateOutput *qmi_client_dms_get_user_lock_state_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_user_lock_state (
    QmiClientDms *self,
    QmiMessageDmsSetUserLockStateInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetUserLockStateOutput *qmi_client_dms_set_user_lock_state_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_user_lock_code (
    QmiClientDms *self,
    QmiMessageDmsSetUserLockCodeInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetUserLockCodeOutput *qmi_client_dms_set_user_lock_code_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_read_user_data (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsReadUserDataOutput *qmi_client_dms_read_user_data_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_write_user_data (
    QmiClientDms *self,
    QmiMessageDmsWriteUserDataInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsWriteUserDataOutput *qmi_client_dms_write_user_data_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_read_eri_file (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsReadEriFileOutput *qmi_client_dms_read_eri_file_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_restore_factory_defaults (
    QmiClientDms *self,
    QmiMessageDmsRestoreFactoryDefaultsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsRestoreFactoryDefaultsOutput *qmi_client_dms_restore_factory_defaults_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_validate_service_programming_code (
    QmiClientDms *self,
    QmiMessageDmsValidateServiceProgrammingCodeInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsValidateServiceProgrammingCodeOutput *qmi_client_dms_validate_service_programming_code_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_get_iccid (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimGetIccidOutput *qmi_client_dms_uim_get_iccid_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_get_ck_status (
    QmiClientDms *self,
    QmiMessageDmsUimGetCkStatusInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimGetCkStatusOutput *qmi_client_dms_uim_get_ck_status_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_set_ck_protection (
    QmiClientDms *self,
    QmiMessageDmsUimSetCkProtectionInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimSetCkProtectionOutput *qmi_client_dms_uim_set_ck_protection_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_unblock_ck (
    QmiClientDms *self,
    QmiMessageDmsUimUnblockCkInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimUnblockCkOutput *qmi_client_dms_uim_unblock_ck_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_get_imsi (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimGetImsiOutput *qmi_client_dms_uim_get_imsi_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_uim_get_state (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsUimGetStateOutput *qmi_client_dms_uim_get_state_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_band_capabilities (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetBandCapabilitiesOutput *qmi_client_dms_get_band_capabilities_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_factory_sku (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetFactorySkuOutput *qmi_client_dms_get_factory_sku_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_firmware_preference (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetFirmwarePreferenceOutput *qmi_client_dms_get_firmware_preference_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_firmware_preference (
    QmiClientDms *self,
    QmiMessageDmsSetFirmwarePreferenceInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetFirmwarePreferenceOutput *qmi_client_dms_set_firmware_preference_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_list_stored_images (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsListStoredImagesOutput *qmi_client_dms_list_stored_images_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_delete_stored_image (
    QmiClientDms *self,
    QmiMessageDmsDeleteStoredImageInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsDeleteStoredImageOutput *qmi_client_dms_delete_stored_image_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_time (
    QmiClientDms *self,
    QmiMessageDmsSetTimeInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetTimeOutput *qmi_client_dms_set_time_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_stored_image_info (
    QmiClientDms *self,
    QmiMessageDmsGetStoredImageInfoInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetStoredImageInfoOutput *qmi_client_dms_get_stored_image_info_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_alt_net_config (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetAltNetConfigOutput *qmi_client_dms_get_alt_net_config_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_alt_net_config (
    QmiClientDms *self,
    QmiMessageDmsSetAltNetConfigInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetAltNetConfigOutput *qmi_client_dms_set_alt_net_config_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_get_software_version (
    QmiClientDms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsGetSoftwareVersionOutput *qmi_client_dms_get_software_version_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_dms_set_service_programming_code (
    QmiClientDms *self,
    QmiMessageDmsSetServiceProgrammingCodeInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageDmsSetServiceProgrammingCodeOutput *qmi_client_dms_set_service_programming_code_finish (
    QmiClientDms *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_DMS__ */
