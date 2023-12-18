
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
#include "qmi-enums-nas.h"
#include "qmi-flags64-nas.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_NAS__
#define __LIBQMI_GLIB_QMI_NAS__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasResetOutput:
 *
 * The #QmiMessageNasResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasResetOutput QmiMessageNasResetOutput;
GType qmi_message_nas_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_RESET_OUTPUT (qmi_message_nas_reset_output_get_type ())

gboolean qmi_message_nas_reset_output_get_result (
    QmiMessageNasResetOutput *self,
    GError **error);

QmiMessageNasResetOutput *qmi_message_nas_reset_output_ref (QmiMessageNasResetOutput *self);
void qmi_message_nas_reset_output_unref (QmiMessageNasResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Abort */


/* --- Input -- */

/* --- Output -- */

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Set Event Report */


/* --- Input -- */

/**
 * QmiMessageNasSetEventReportInput:
 *
 * The #QmiMessageNasSetEventReportInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetEventReportInput QmiMessageNasSetEventReportInput;
GType qmi_message_nas_set_event_report_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_EVENT_REPORT_INPUT (qmi_message_nas_set_event_report_input_get_type ())

gboolean qmi_message_nas_set_event_report_input_get_lte_rsrp_delta (
    QmiMessageNasSetEventReportInput *self,
    gboolean *lte_rsrp_delta_report,
    guint8 *lte_rsrp_delta_rsrp_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_lte_rsrp_delta (
    QmiMessageNasSetEventReportInput *self,
    gboolean lte_rsrp_delta_report,
    guint8 lte_rsrp_delta_rsrp_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_lte_snr_delta (
    QmiMessageNasSetEventReportInput *self,
    gboolean *lte_snr_delta_report,
    guint8 *lte_snr_delta_snr_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_lte_snr_delta (
    QmiMessageNasSetEventReportInput *self,
    gboolean lte_snr_delta_report,
    guint8 lte_snr_delta_snr_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_sinr_threshold (
    QmiMessageNasSetEventReportInput *self,
    gboolean *sinr_threshold_report,
    GArray **sinr_threshold_thresholds,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_sinr_threshold (
    QmiMessageNasSetEventReportInput *self,
    gboolean sinr_threshold_report,
    GArray *sinr_threshold_thresholds,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_ecio_threshold (
    QmiMessageNasSetEventReportInput *self,
    gboolean *ecio_threshold_report,
    GArray **ecio_threshold_thresholds,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_ecio_threshold (
    QmiMessageNasSetEventReportInput *self,
    gboolean ecio_threshold_report,
    GArray *ecio_threshold_thresholds,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_error_rate_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *error_rate_indicator,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_error_rate_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean error_rate_indicator,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_sinr_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *sinr_indicator_report,
    guint8 *sinr_indicator_sinr_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_sinr_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean sinr_indicator_report,
    guint8 sinr_indicator_sinr_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_io_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *io_indicator_report,
    guint8 *io_indicator_io_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_io_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean io_indicator_report,
    guint8 io_indicator_io_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_ecio_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *ecio_indicator_report,
    guint8 *ecio_indicator_ecio_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_ecio_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean ecio_indicator_report,
    guint8 ecio_indicator_ecio_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_rssi_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *rssi_indicator_report,
    guint8 *rssi_indicator_rssi_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_rssi_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean rssi_indicator_report,
    guint8 rssi_indicator_rssi_delta,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_registration_reject_reason (
    QmiMessageNasSetEventReportInput *self,
    gboolean *registration_reject_reason,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_registration_reject_reason (
    QmiMessageNasSetEventReportInput *self,
    gboolean registration_reject_reason,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_rf_band_information (
    QmiMessageNasSetEventReportInput *self,
    gboolean *rf_band_information,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_rf_band_information (
    QmiMessageNasSetEventReportInput *self,
    gboolean rf_band_information,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_get_signal_strength_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean *signal_strength_indicator_report,
    GArray **signal_strength_indicator_thresholds,
    GError **error);

gboolean qmi_message_nas_set_event_report_input_set_signal_strength_indicator (
    QmiMessageNasSetEventReportInput *self,
    gboolean signal_strength_indicator_report,
    GArray *signal_strength_indicator_thresholds,
    GError **error);

QmiMessageNasSetEventReportInput *qmi_message_nas_set_event_report_input_ref (QmiMessageNasSetEventReportInput *self);
void qmi_message_nas_set_event_report_input_unref (QmiMessageNasSetEventReportInput *self);
QmiMessageNasSetEventReportInput *qmi_message_nas_set_event_report_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasSetEventReportOutput:
 *
 * The #QmiMessageNasSetEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetEventReportOutput QmiMessageNasSetEventReportOutput;
GType qmi_message_nas_set_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_EVENT_REPORT_OUTPUT (qmi_message_nas_set_event_report_output_get_type ())

gboolean qmi_message_nas_set_event_report_output_get_result (
    QmiMessageNasSetEventReportOutput *self,
    GError **error);

QmiMessageNasSetEventReportOutput *qmi_message_nas_set_event_report_output_ref (QmiMessageNasSetEventReportOutput *self);
void qmi_message_nas_set_event_report_output_unref (QmiMessageNasSetEventReportOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication NAS Event Report */


/* --- Output -- */

/**
 * QmiIndicationNasEventReportOutputRfBandInformationElement:
 * @radio_interface: a #QmiNasRadioInterface.
 * @active_band_class: a #QmiNasActiveBand.
 * @active_channel: a #guint16.
 *
 * A QmiIndicationNasEventReportOutputRfBandInformationElement struct.
 */
typedef struct _QmiIndicationNasEventReportOutputRfBandInformationElement {
    QmiNasRadioInterface radio_interface;
    QmiNasActiveBand active_band_class;
    guint16 active_channel;
} QmiIndicationNasEventReportOutputRfBandInformationElement;

/**
 * QmiIndicationNasEventReportOutput:
 *
 * The #QmiIndicationNasEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationNasEventReportOutput QmiIndicationNasEventReportOutput;
GType qmi_indication_nas_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_NAS_EVENT_REPORT_OUTPUT (qmi_indication_nas_event_report_output_get_type ())

gboolean qmi_indication_nas_event_report_output_get_lte_rsrp (
    QmiIndicationNasEventReportOutput *self,
    gint16 *lte_rsrp,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_lte_snr (
    QmiIndicationNasEventReportOutput *self,
    gint16 *lte_snr,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_rsrq (
    QmiIndicationNasEventReportOutput *self,
    gint8 *rsrq_rsrq,
    QmiNasRadioInterface *rsrq_radio_interface,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_error_rate (
    QmiIndicationNasEventReportOutput *self,
    guint16 *error_rate_rate,
    QmiNasRadioInterface *error_rate_radio_interface,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_sinr (
    QmiIndicationNasEventReportOutput *self,
    QmiNasEvdoSinrLevel *sinr,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_io (
    QmiIndicationNasEventReportOutput *self,
    gint32 *io,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_ecio (
    QmiIndicationNasEventReportOutput *self,
    gint8 *ecio_ecio,
    QmiNasRadioInterface *ecio_radio_interface,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_rssi (
    QmiIndicationNasEventReportOutput *self,
    guint8 *rssi_rssi,
    QmiNasRadioInterface *rssi_radio_interface,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_registration_reject_reason (
    QmiIndicationNasEventReportOutput *self,
    QmiNasNetworkServiceDomain *registration_reject_reason_service_domain,
    guint16 *registration_reject_reason_reject_cause,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_rf_band_information (
    QmiIndicationNasEventReportOutput *self,
    GArray **rf_band_information,
    GError **error);

gboolean qmi_indication_nas_event_report_output_get_signal_strength (
    QmiIndicationNasEventReportOutput *self,
    gint8 *signal_strength_strength,
    QmiNasRadioInterface *signal_strength_radio_interface,
    GError **error);

QmiIndicationNasEventReportOutput *qmi_indication_nas_event_report_output_ref (QmiIndicationNasEventReportOutput *self);
void qmi_indication_nas_event_report_output_unref (QmiIndicationNasEventReportOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Register Indications */


/* --- Input -- */

/**
 * QmiMessageNasRegisterIndicationsInput:
 *
 * The #QmiMessageNasRegisterIndicationsInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasRegisterIndicationsInput QmiMessageNasRegisterIndicationsInput;
GType qmi_message_nas_register_indications_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_REGISTER_INDICATIONS_INPUT (qmi_message_nas_register_indications_input_get_type ())

gboolean qmi_message_nas_register_indications_input_get_rf_band_information (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *rf_band_information,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_rf_band_information (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean rf_band_information,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_embms_status (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *embms_status,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_embms_status (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean embms_status,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_current_plmn_name (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *current_plmn_name,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_current_plmn_name (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean current_plmn_name,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_managed_roaming (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *managed_roaming,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_managed_roaming (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean managed_roaming,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_hdr_session_closed (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *hdr_session_closed,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_hdr_session_closed (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean hdr_session_closed,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_hdr_new_uati_assigned (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *hdr_new_uati_assigned,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_hdr_new_uati_assigned (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean hdr_new_uati_assigned,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_error_rate (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *error_rate,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_error_rate (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean error_rate,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_signal_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *signal_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_signal_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean signal_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_system_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *system_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_system_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean system_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_network_time (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *network_time,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_network_time (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean network_time,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_subscription_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *subscription_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_subscription_info (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean subscription_info,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_dual_standby_preference (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *dual_standby_preference,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_dual_standby_preference (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean dual_standby_preference,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_serving_system_events (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *serving_system_events,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_serving_system_events (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean serving_system_events,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_ddtm_events (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *ddtm_events,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_ddtm_events (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean ddtm_events,
    GError **error);

gboolean qmi_message_nas_register_indications_input_get_system_selection_preference (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean *system_selection_preference,
    GError **error);

gboolean qmi_message_nas_register_indications_input_set_system_selection_preference (
    QmiMessageNasRegisterIndicationsInput *self,
    gboolean system_selection_preference,
    GError **error);

QmiMessageNasRegisterIndicationsInput *qmi_message_nas_register_indications_input_ref (QmiMessageNasRegisterIndicationsInput *self);
void qmi_message_nas_register_indications_input_unref (QmiMessageNasRegisterIndicationsInput *self);
QmiMessageNasRegisterIndicationsInput *qmi_message_nas_register_indications_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasRegisterIndicationsOutput:
 *
 * The #QmiMessageNasRegisterIndicationsOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasRegisterIndicationsOutput QmiMessageNasRegisterIndicationsOutput;
GType qmi_message_nas_register_indications_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_REGISTER_INDICATIONS_OUTPUT (qmi_message_nas_register_indications_output_get_type ())

gboolean qmi_message_nas_register_indications_output_get_result (
    QmiMessageNasRegisterIndicationsOutput *self,
    GError **error);

QmiMessageNasRegisterIndicationsOutput *qmi_message_nas_register_indications_output_ref (QmiMessageNasRegisterIndicationsOutput *self);
void qmi_message_nas_register_indications_output_unref (QmiMessageNasRegisterIndicationsOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Signal Strength */


/* --- Input -- */

/**
 * QmiMessageNasGetSignalStrengthInput:
 *
 * The #QmiMessageNasGetSignalStrengthInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetSignalStrengthInput QmiMessageNasGetSignalStrengthInput;
GType qmi_message_nas_get_signal_strength_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SIGNAL_STRENGTH_INPUT (qmi_message_nas_get_signal_strength_input_get_type ())

gboolean qmi_message_nas_get_signal_strength_input_get_request_mask (
    QmiMessageNasGetSignalStrengthInput *self,
    QmiNasSignalStrengthRequest *request_mask,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_input_set_request_mask (
    QmiMessageNasGetSignalStrengthInput *self,
    QmiNasSignalStrengthRequest request_mask,
    GError **error);

QmiMessageNasGetSignalStrengthInput *qmi_message_nas_get_signal_strength_input_ref (QmiMessageNasGetSignalStrengthInput *self);
void qmi_message_nas_get_signal_strength_input_unref (QmiMessageNasGetSignalStrengthInput *self);
QmiMessageNasGetSignalStrengthInput *qmi_message_nas_get_signal_strength_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasGetSignalStrengthOutputErrorRateListElement:
 * @rate: a #guint16.
 * @radio_interface: a #QmiNasRadioInterface.
 *
 * A QmiMessageNasGetSignalStrengthOutputErrorRateListElement struct.
 */
typedef struct _QmiMessageNasGetSignalStrengthOutputErrorRateListElement {
    guint16 rate;
    QmiNasRadioInterface radio_interface;
} QmiMessageNasGetSignalStrengthOutputErrorRateListElement;

/**
 * QmiMessageNasGetSignalStrengthOutputEcioListElement:
 * @ecio: a #gint8.
 * @radio_interface: a #QmiNasRadioInterface.
 *
 * A QmiMessageNasGetSignalStrengthOutputEcioListElement struct.
 */
typedef struct _QmiMessageNasGetSignalStrengthOutputEcioListElement {
    gint8 ecio;
    QmiNasRadioInterface radio_interface;
} QmiMessageNasGetSignalStrengthOutputEcioListElement;

/**
 * QmiMessageNasGetSignalStrengthOutputRssiListElement:
 * @rssi: a #guint8.
 * @radio_interface: a #QmiNasRadioInterface.
 *
 * A QmiMessageNasGetSignalStrengthOutputRssiListElement struct.
 */
typedef struct _QmiMessageNasGetSignalStrengthOutputRssiListElement {
    guint8 rssi;
    QmiNasRadioInterface radio_interface;
} QmiMessageNasGetSignalStrengthOutputRssiListElement;

/**
 * QmiMessageNasGetSignalStrengthOutputStrengthListElement:
 * @strength: a #gint8.
 * @radio_interface: a #QmiNasRadioInterface.
 *
 * A QmiMessageNasGetSignalStrengthOutputStrengthListElement struct.
 */
typedef struct _QmiMessageNasGetSignalStrengthOutputStrengthListElement {
    gint8 strength;
    QmiNasRadioInterface radio_interface;
} QmiMessageNasGetSignalStrengthOutputStrengthListElement;

/**
 * QmiMessageNasGetSignalStrengthOutput:
 *
 * The #QmiMessageNasGetSignalStrengthOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetSignalStrengthOutput QmiMessageNasGetSignalStrengthOutput;
GType qmi_message_nas_get_signal_strength_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SIGNAL_STRENGTH_OUTPUT (qmi_message_nas_get_signal_strength_output_get_type ())

gboolean qmi_message_nas_get_signal_strength_output_get_result (
    QmiMessageNasGetSignalStrengthOutput *self,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_lte_rsrp (
    QmiMessageNasGetSignalStrengthOutput *self,
    gint16 *lte_rsrp,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_lte_snr (
    QmiMessageNasGetSignalStrengthOutput *self,
    gint16 *lte_snr,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_rsrq (
    QmiMessageNasGetSignalStrengthOutput *self,
    gint8 *rsrq_rsrq,
    QmiNasRadioInterface *rsrq_radio_interface,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_error_rate_list (
    QmiMessageNasGetSignalStrengthOutput *self,
    GArray **error_rate_list,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_sinr (
    QmiMessageNasGetSignalStrengthOutput *self,
    QmiNasEvdoSinrLevel *sinr,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_io (
    QmiMessageNasGetSignalStrengthOutput *self,
    gint32 *io,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_ecio_list (
    QmiMessageNasGetSignalStrengthOutput *self,
    GArray **ecio_list,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_rssi_list (
    QmiMessageNasGetSignalStrengthOutput *self,
    GArray **rssi_list,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_strength_list (
    QmiMessageNasGetSignalStrengthOutput *self,
    GArray **strength_list,
    GError **error);

gboolean qmi_message_nas_get_signal_strength_output_get_signal_strength (
    QmiMessageNasGetSignalStrengthOutput *self,
    gint8 *signal_strength_strength,
    QmiNasRadioInterface *signal_strength_radio_interface,
    GError **error);

QmiMessageNasGetSignalStrengthOutput *qmi_message_nas_get_signal_strength_output_ref (QmiMessageNasGetSignalStrengthOutput *self);
void qmi_message_nas_get_signal_strength_output_unref (QmiMessageNasGetSignalStrengthOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Network Scan */


/* --- Input -- */

/**
 * QmiMessageNasNetworkScanInput:
 *
 * The #QmiMessageNasNetworkScanInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasNetworkScanInput QmiMessageNasNetworkScanInput;
GType qmi_message_nas_network_scan_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_NETWORK_SCAN_INPUT (qmi_message_nas_network_scan_input_get_type ())

gboolean qmi_message_nas_network_scan_input_get_network_type (
    QmiMessageNasNetworkScanInput *self,
    QmiNasNetworkScanType *network_type,
    GError **error);

gboolean qmi_message_nas_network_scan_input_set_network_type (
    QmiMessageNasNetworkScanInput *self,
    QmiNasNetworkScanType network_type,
    GError **error);

QmiMessageNasNetworkScanInput *qmi_message_nas_network_scan_input_ref (QmiMessageNasNetworkScanInput *self);
void qmi_message_nas_network_scan_input_unref (QmiMessageNasNetworkScanInput *self);
QmiMessageNasNetworkScanInput *qmi_message_nas_network_scan_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasNetworkScanOutputMncPcsDigitIncludeStatusElement:
 * @mcc: a #guint16.
 * @mnc: a #guint16.
 * @includes_pcs_digit: a #gboolean.
 *
 * A QmiMessageNasNetworkScanOutputMncPcsDigitIncludeStatusElement struct.
 */
typedef struct _QmiMessageNasNetworkScanOutputMncPcsDigitIncludeStatusElement {
    guint16 mcc;
    guint16 mnc;
    gboolean includes_pcs_digit;
} QmiMessageNasNetworkScanOutputMncPcsDigitIncludeStatusElement;

/**
 * QmiMessageNasNetworkScanOutputRadioAccessTechnologyElement:
 * @mcc: a #guint16.
 * @mnc: a #guint16.
 * @radio_interface: a #QmiNasRadioInterface.
 *
 * A QmiMessageNasNetworkScanOutputRadioAccessTechnologyElement struct.
 */
typedef struct _QmiMessageNasNetworkScanOutputRadioAccessTechnologyElement {
    guint16 mcc;
    guint16 mnc;
    QmiNasRadioInterface radio_interface;
} QmiMessageNasNetworkScanOutputRadioAccessTechnologyElement;

/**
 * QmiMessageNasNetworkScanOutputNetworkInformationElement:
 * @mcc: a #guint16.
 * @mnc: a #guint16.
 * @network_status: a #QmiNasNetworkStatus.
 * @description: a string.
 *
 * A QmiMessageNasNetworkScanOutputNetworkInformationElement struct.
 */
typedef struct _QmiMessageNasNetworkScanOutputNetworkInformationElement {
    guint16 mcc;
    guint16 mnc;
    QmiNasNetworkStatus network_status;
    gchar * description;
} QmiMessageNasNetworkScanOutputNetworkInformationElement;

/**
 * QmiMessageNasNetworkScanOutput:
 *
 * The #QmiMessageNasNetworkScanOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasNetworkScanOutput QmiMessageNasNetworkScanOutput;
GType qmi_message_nas_network_scan_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_NETWORK_SCAN_OUTPUT (qmi_message_nas_network_scan_output_get_type ())

gboolean qmi_message_nas_network_scan_output_get_result (
    QmiMessageNasNetworkScanOutput *self,
    GError **error);

gboolean qmi_message_nas_network_scan_output_get_mnc_pcs_digit_include_status (
    QmiMessageNasNetworkScanOutput *self,
    GArray **mnc_pcs_digit_include_status,
    GError **error);

gboolean qmi_message_nas_network_scan_output_get_radio_access_technology (
    QmiMessageNasNetworkScanOutput *self,
    GArray **radio_access_technology,
    GError **error);

gboolean qmi_message_nas_network_scan_output_get_network_information (
    QmiMessageNasNetworkScanOutput *self,
    GArray **network_information,
    GError **error);

QmiMessageNasNetworkScanOutput *qmi_message_nas_network_scan_output_ref (QmiMessageNasNetworkScanOutput *self);
void qmi_message_nas_network_scan_output_unref (QmiMessageNasNetworkScanOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Initiate Network Register */


/* --- Input -- */

/**
 * QmiMessageNasInitiateNetworkRegisterInput:
 *
 * The #QmiMessageNasInitiateNetworkRegisterInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasInitiateNetworkRegisterInput QmiMessageNasInitiateNetworkRegisterInput;
GType qmi_message_nas_initiate_network_register_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_INITIATE_NETWORK_REGISTER_INPUT (qmi_message_nas_initiate_network_register_input_get_type ())

gboolean qmi_message_nas_initiate_network_register_input_get_mnc_pcs_digit_include_status (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    gboolean *mnc_pcs_digit_include_status,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_set_mnc_pcs_digit_include_status (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    gboolean mnc_pcs_digit_include_status,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_get_change_duration (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    QmiNasChangeDuration *change_duration,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_set_change_duration (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    QmiNasChangeDuration change_duration,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_get_manual_registration_info_3gpp (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    guint16 *manual_registration_info_3gpp_mcc,
    guint16 *manual_registration_info_3gpp_mnc,
    QmiNasRadioInterface *manual_registration_info_3gpp_radio_interface,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_set_manual_registration_info_3gpp (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    guint16 manual_registration_info_3gpp_mcc,
    guint16 manual_registration_info_3gpp_mnc,
    QmiNasRadioInterface manual_registration_info_3gpp_radio_interface,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_get_action (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    QmiNasNetworkRegisterType *action,
    GError **error);

gboolean qmi_message_nas_initiate_network_register_input_set_action (
    QmiMessageNasInitiateNetworkRegisterInput *self,
    QmiNasNetworkRegisterType action,
    GError **error);

QmiMessageNasInitiateNetworkRegisterInput *qmi_message_nas_initiate_network_register_input_ref (QmiMessageNasInitiateNetworkRegisterInput *self);
void qmi_message_nas_initiate_network_register_input_unref (QmiMessageNasInitiateNetworkRegisterInput *self);
QmiMessageNasInitiateNetworkRegisterInput *qmi_message_nas_initiate_network_register_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasInitiateNetworkRegisterOutput:
 *
 * The #QmiMessageNasInitiateNetworkRegisterOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasInitiateNetworkRegisterOutput QmiMessageNasInitiateNetworkRegisterOutput;
GType qmi_message_nas_initiate_network_register_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_INITIATE_NETWORK_REGISTER_OUTPUT (qmi_message_nas_initiate_network_register_output_get_type ())

gboolean qmi_message_nas_initiate_network_register_output_get_result (
    QmiMessageNasInitiateNetworkRegisterOutput *self,
    GError **error);

QmiMessageNasInitiateNetworkRegisterOutput *qmi_message_nas_initiate_network_register_output_ref (QmiMessageNasInitiateNetworkRegisterOutput *self);
void qmi_message_nas_initiate_network_register_output_unref (QmiMessageNasInitiateNetworkRegisterOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Serving System */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetServingSystemOutputRoamingIndicatorListElement:
 * @radio_interface: a #QmiNasRadioInterface.
 * @roaming_indicator: a #QmiNasRoamingIndicatorStatus.
 *
 * A QmiMessageNasGetServingSystemOutputRoamingIndicatorListElement struct.
 */
typedef struct _QmiMessageNasGetServingSystemOutputRoamingIndicatorListElement {
    QmiNasRadioInterface radio_interface;
    QmiNasRoamingIndicatorStatus roaming_indicator;
} QmiMessageNasGetServingSystemOutputRoamingIndicatorListElement;

/**
 * QmiMessageNasGetServingSystemOutput:
 *
 * The #QmiMessageNasGetServingSystemOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetServingSystemOutput QmiMessageNasGetServingSystemOutput;
GType qmi_message_nas_get_serving_system_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SERVING_SYSTEM_OUTPUT (qmi_message_nas_get_serving_system_output_get_type ())

gboolean qmi_message_nas_get_serving_system_output_get_result (
    QmiMessageNasGetServingSystemOutput *self,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_mnc_pcs_digit_include_status (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *mnc_pcs_digit_include_status_mcc,
    guint16 *mnc_pcs_digit_include_status_mnc,
    gboolean *mnc_pcs_digit_include_status_includes_pcs_digit,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_umts_primary_scrambling_code (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *umts_primary_scrambling_code,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_call_barring_status (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasCallBarringStatus *call_barring_status_cs_status,
    QmiNasCallBarringStatus *call_barring_status_ps_status,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_lte_tac (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *lte_tac,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_hdr_personality (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasHdrPersonality *hdr_personality,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_cdma_system_info (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *cdma_system_info_mcc,
    guint8 *cdma_system_info_imsi_11_12,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_detailed_service_status (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasServiceStatus *detailed_service_status_status,
    QmiNasNetworkServiceDomain *detailed_service_status_capability,
    QmiNasServiceStatus *detailed_service_status_hdr_status,
    gboolean *detailed_service_status_hdr_hybrid,
    gboolean *detailed_service_status_forbidden,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_dtm_support (
    QmiMessageNasGetServingSystemOutput *self,
    gboolean *dtm_support,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_prl_indicator_3gpp2 (
    QmiMessageNasGetServingSystemOutput *self,
    gboolean *prl_indicator_3gpp2,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_concurrent_service_info_3gpp2 (
    QmiMessageNasGetServingSystemOutput *self,
    gboolean *concurrent_service_info_3gpp2,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_cid_3gpp (
    QmiMessageNasGetServingSystemOutput *self,
    guint32 *cid_3gpp,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_lac_3gpp (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *lac_3gpp,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_daylight_saving_time_adjustment_3gpp (
    QmiMessageNasGetServingSystemOutput *self,
    guint8 *daylight_saving_time_adjustment_3gpp,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_time_zone_3gpp (
    QmiMessageNasGetServingSystemOutput *self,
    gint8 *time_zone_3gpp,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_cdma_p_rev (
    QmiMessageNasGetServingSystemOutput *self,
    guint8 *cdma_p_rev,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_time_zone_3gpp2 (
    QmiMessageNasGetServingSystemOutput *self,
    guint8 *time_zone_3gpp2_leap_seconds,
    gint8 *time_zone_3gpp2_local_time_offset,
    gboolean *time_zone_3gpp2_daylight_saving_time,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_default_roaming_indicator (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasRoamingIndicatorStatus *default_roaming_indicator,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_roaming_indicator_list (
    QmiMessageNasGetServingSystemOutput *self,
    GArray **roaming_indicator_list,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_cdma_base_station_info (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *cdma_base_station_info_base_station_id,
    gint32 *cdma_base_station_info_base_station_latitude,
    gint32 *cdma_base_station_info_base_station_longitude,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_cdma_system_id (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *cdma_system_id_sid,
    guint16 *cdma_system_id_nid,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_current_plmn (
    QmiMessageNasGetServingSystemOutput *self,
    guint16 *current_plmn_mcc,
    guint16 *current_plmn_mnc,
    const gchar **current_plmn_description,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_data_service_capability (
    QmiMessageNasGetServingSystemOutput *self,
    GArray **data_service_capability,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_roaming_indicator (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasRoamingIndicatorStatus *roaming_indicator,
    GError **error);

gboolean qmi_message_nas_get_serving_system_output_get_serving_system (
    QmiMessageNasGetServingSystemOutput *self,
    QmiNasRegistrationState *serving_system_registration_state,
    QmiNasAttachState *serving_system_cs_attach_state,
    QmiNasAttachState *serving_system_ps_attach_state,
    QmiNasNetworkType *serving_system_selected_network,
    GArray **serving_system_radio_interfaces,
    GError **error);

QmiMessageNasGetServingSystemOutput *qmi_message_nas_get_serving_system_output_ref (QmiMessageNasGetServingSystemOutput *self);
void qmi_message_nas_get_serving_system_output_unref (QmiMessageNasGetServingSystemOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication NAS Serving System */


/* --- Output -- */

/**
 * QmiIndicationNasServingSystemOutputRoamingIndicatorListElement:
 * @radio_interface: a #QmiNasRadioInterface.
 * @roaming_indicator: a #QmiNasRoamingIndicatorStatus.
 *
 * A QmiIndicationNasServingSystemOutputRoamingIndicatorListElement struct.
 */
typedef struct _QmiIndicationNasServingSystemOutputRoamingIndicatorListElement {
    QmiNasRadioInterface radio_interface;
    QmiNasRoamingIndicatorStatus roaming_indicator;
} QmiIndicationNasServingSystemOutputRoamingIndicatorListElement;

/**
 * QmiIndicationNasServingSystemOutput:
 *
 * The #QmiIndicationNasServingSystemOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationNasServingSystemOutput QmiIndicationNasServingSystemOutput;
GType qmi_indication_nas_serving_system_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_NAS_SERVING_SYSTEM_OUTPUT (qmi_indication_nas_serving_system_output_get_type ())

gboolean qmi_indication_nas_serving_system_output_get_mnc_pcs_digit_include_status (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *mnc_pcs_digit_include_status_mcc,
    guint16 *mnc_pcs_digit_include_status_mnc,
    gboolean *mnc_pcs_digit_include_status_includes_pcs_digit,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_umts_primary_scrambling_code (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *umts_primary_scrambling_code,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_plmn_not_changed_indication (
    QmiIndicationNasServingSystemOutput *self,
    gboolean *plmn_not_changed_indication,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_call_barring_status (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasCallBarringStatus *call_barring_status_cs_status,
    QmiNasCallBarringStatus *call_barring_status_ps_status,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_lte_tac (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *lte_tac,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_hdr_personality (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasHdrPersonality *hdr_personality,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_cdma_system_info (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *cdma_system_info_mcc,
    guint8 *cdma_system_info_imsi_11_12,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_detailed_service_status (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasServiceStatus *detailed_service_status_status,
    QmiNasNetworkServiceDomain *detailed_service_status_capability,
    QmiNasServiceStatus *detailed_service_status_hdr_status,
    gboolean *detailed_service_status_hdr_hybrid,
    gboolean *detailed_service_status_forbidden,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_dtm_support (
    QmiIndicationNasServingSystemOutput *self,
    gboolean *dtm_support,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_prl_indicator_3gpp2 (
    QmiIndicationNasServingSystemOutput *self,
    gboolean *prl_indicator_3gpp2,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_concurrent_service_info_3gpp2 (
    QmiIndicationNasServingSystemOutput *self,
    gboolean *concurrent_service_info_3gpp2,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_cid_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    guint32 *cid_3gpp,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_lac_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *lac_3gpp,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_universal_time_and_local_time_zone_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *universal_time_and_local_time_zone_3gpp_year,
    guint8 *universal_time_and_local_time_zone_3gpp_month,
    guint8 *universal_time_and_local_time_zone_3gpp_day,
    guint8 *universal_time_and_local_time_zone_3gpp_hour,
    guint8 *universal_time_and_local_time_zone_3gpp_minute,
    guint8 *universal_time_and_local_time_zone_3gpp_second,
    guint8 *universal_time_and_local_time_zone_3gpp_time_zone,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_daylight_saving_time_adjustment_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    guint8 *daylight_saving_time_adjustment_3gpp,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_time_zone_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    gint8 *time_zone_3gpp,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_plmn_name_flag_3gpp (
    QmiIndicationNasServingSystemOutput *self,
    gboolean *plmn_name_flag_3gpp,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_cdma_p_rev (
    QmiIndicationNasServingSystemOutput *self,
    guint8 *cdma_p_rev,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_time_zone_3gpp2 (
    QmiIndicationNasServingSystemOutput *self,
    guint8 *time_zone_3gpp2_leap_seconds,
    gint8 *time_zone_3gpp2_local_time_offset,
    gboolean *time_zone_3gpp2_daylight_saving_time,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_default_roaming_indicator (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasRoamingIndicatorStatus *default_roaming_indicator,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_roaming_indicator_list (
    QmiIndicationNasServingSystemOutput *self,
    GArray **roaming_indicator_list,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_cdma_base_station_info (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *cdma_base_station_info_base_station_id,
    gint32 *cdma_base_station_info_base_station_latitude,
    gint32 *cdma_base_station_info_base_station_longitude,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_cdma_system_id (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *cdma_system_id_sid,
    guint16 *cdma_system_id_nid,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_current_plmn (
    QmiIndicationNasServingSystemOutput *self,
    guint16 *current_plmn_mcc,
    guint16 *current_plmn_mnc,
    const gchar **current_plmn_description,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_data_service_capability (
    QmiIndicationNasServingSystemOutput *self,
    GArray **data_service_capability,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_roaming_indicator (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasRoamingIndicatorStatus *roaming_indicator,
    GError **error);

gboolean qmi_indication_nas_serving_system_output_get_serving_system (
    QmiIndicationNasServingSystemOutput *self,
    QmiNasRegistrationState *serving_system_registration_state,
    QmiNasAttachState *serving_system_cs_attach_state,
    QmiNasAttachState *serving_system_ps_attach_state,
    QmiNasNetworkType *serving_system_selected_network,
    GArray **serving_system_radio_interfaces,
    GError **error);

QmiIndicationNasServingSystemOutput *qmi_indication_nas_serving_system_output_ref (QmiIndicationNasServingSystemOutput *self);
void qmi_indication_nas_serving_system_output_unref (QmiIndicationNasServingSystemOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Home Network */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetHomeNetworkOutput:
 *
 * The #QmiMessageNasGetHomeNetworkOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetHomeNetworkOutput QmiMessageNasGetHomeNetworkOutput;
GType qmi_message_nas_get_home_network_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_HOME_NETWORK_OUTPUT (qmi_message_nas_get_home_network_output_get_type ())

gboolean qmi_message_nas_get_home_network_output_get_result (
    QmiMessageNasGetHomeNetworkOutput *self,
    GError **error);

gboolean qmi_message_nas_get_home_network_output_get_home_network_3gpp2 (
    QmiMessageNasGetHomeNetworkOutput *self,
    guint16 *home_network_3gpp2_mcc,
    guint16 *home_network_3gpp2_mnc,
    QmiNasNetworkDescriptionDisplay *home_network_3gpp2_display_description,
    QmiNasNetworkDescriptionEncoding *home_network_3gpp2_description_encoding,
    const gchar **home_network_3gpp2_description,
    GError **error);

gboolean qmi_message_nas_get_home_network_output_get_home_system_id (
    QmiMessageNasGetHomeNetworkOutput *self,
    guint16 *home_system_id_sid,
    guint16 *home_system_id_nid,
    GError **error);

gboolean qmi_message_nas_get_home_network_output_get_home_network (
    QmiMessageNasGetHomeNetworkOutput *self,
    guint16 *home_network_mcc,
    guint16 *home_network_mnc,
    const gchar **home_network_description,
    GError **error);

QmiMessageNasGetHomeNetworkOutput *qmi_message_nas_get_home_network_output_ref (QmiMessageNasGetHomeNetworkOutput *self);
void qmi_message_nas_get_home_network_output_unref (QmiMessageNasGetHomeNetworkOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Set Technology Preference */


/* --- Input -- */

/**
 * QmiMessageNasSetTechnologyPreferenceInput:
 *
 * The #QmiMessageNasSetTechnologyPreferenceInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetTechnologyPreferenceInput QmiMessageNasSetTechnologyPreferenceInput;
GType qmi_message_nas_set_technology_preference_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_TECHNOLOGY_PREFERENCE_INPUT (qmi_message_nas_set_technology_preference_input_get_type ())

gboolean qmi_message_nas_set_technology_preference_input_get_current (
    QmiMessageNasSetTechnologyPreferenceInput *self,
    QmiNasRadioTechnologyPreference *current_technology_preference,
    QmiNasPreferenceDuration *current_technology_preference_duration,
    GError **error);

gboolean qmi_message_nas_set_technology_preference_input_set_current (
    QmiMessageNasSetTechnologyPreferenceInput *self,
    QmiNasRadioTechnologyPreference current_technology_preference,
    QmiNasPreferenceDuration current_technology_preference_duration,
    GError **error);

QmiMessageNasSetTechnologyPreferenceInput *qmi_message_nas_set_technology_preference_input_ref (QmiMessageNasSetTechnologyPreferenceInput *self);
void qmi_message_nas_set_technology_preference_input_unref (QmiMessageNasSetTechnologyPreferenceInput *self);
QmiMessageNasSetTechnologyPreferenceInput *qmi_message_nas_set_technology_preference_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasSetTechnologyPreferenceOutput:
 *
 * The #QmiMessageNasSetTechnologyPreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetTechnologyPreferenceOutput QmiMessageNasSetTechnologyPreferenceOutput;
GType qmi_message_nas_set_technology_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_TECHNOLOGY_PREFERENCE_OUTPUT (qmi_message_nas_set_technology_preference_output_get_type ())

gboolean qmi_message_nas_set_technology_preference_output_get_result (
    QmiMessageNasSetTechnologyPreferenceOutput *self,
    GError **error);

QmiMessageNasSetTechnologyPreferenceOutput *qmi_message_nas_set_technology_preference_output_ref (QmiMessageNasSetTechnologyPreferenceOutput *self);
void qmi_message_nas_set_technology_preference_output_unref (QmiMessageNasSetTechnologyPreferenceOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Technology Preference */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetTechnologyPreferenceOutput:
 *
 * The #QmiMessageNasGetTechnologyPreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetTechnologyPreferenceOutput QmiMessageNasGetTechnologyPreferenceOutput;
GType qmi_message_nas_get_technology_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_TECHNOLOGY_PREFERENCE_OUTPUT (qmi_message_nas_get_technology_preference_output_get_type ())

gboolean qmi_message_nas_get_technology_preference_output_get_result (
    QmiMessageNasGetTechnologyPreferenceOutput *self,
    GError **error);

gboolean qmi_message_nas_get_technology_preference_output_get_persistent (
    QmiMessageNasGetTechnologyPreferenceOutput *self,
    QmiNasRadioTechnologyPreference *persistent,
    GError **error);

gboolean qmi_message_nas_get_technology_preference_output_get_active (
    QmiMessageNasGetTechnologyPreferenceOutput *self,
    QmiNasRadioTechnologyPreference *active_technology_preference,
    QmiNasPreferenceDuration *active_technology_preference_duration,
    GError **error);

QmiMessageNasGetTechnologyPreferenceOutput *qmi_message_nas_get_technology_preference_output_ref (QmiMessageNasGetTechnologyPreferenceOutput *self);
void qmi_message_nas_get_technology_preference_output_unref (QmiMessageNasGetTechnologyPreferenceOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get RF Band Information */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetRfBandInformationOutputListElement:
 * @radio_interface: a #QmiNasRadioInterface.
 * @active_band_class: a #QmiNasActiveBand.
 * @active_channel: a #guint16.
 *
 * A QmiMessageNasGetRfBandInformationOutputListElement struct.
 */
typedef struct _QmiMessageNasGetRfBandInformationOutputListElement {
    QmiNasRadioInterface radio_interface;
    QmiNasActiveBand active_band_class;
    guint16 active_channel;
} QmiMessageNasGetRfBandInformationOutputListElement;

/**
 * QmiMessageNasGetRfBandInformationOutput:
 *
 * The #QmiMessageNasGetRfBandInformationOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetRfBandInformationOutput QmiMessageNasGetRfBandInformationOutput;
GType qmi_message_nas_get_rf_band_information_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_RF_BAND_INFORMATION_OUTPUT (qmi_message_nas_get_rf_band_information_output_get_type ())

gboolean qmi_message_nas_get_rf_band_information_output_get_result (
    QmiMessageNasGetRfBandInformationOutput *self,
    GError **error);

gboolean qmi_message_nas_get_rf_band_information_output_get_list (
    QmiMessageNasGetRfBandInformationOutput *self,
    GArray **list,
    GError **error);

QmiMessageNasGetRfBandInformationOutput *qmi_message_nas_get_rf_band_information_output_ref (QmiMessageNasGetRfBandInformationOutput *self);
void qmi_message_nas_get_rf_band_information_output_unref (QmiMessageNasGetRfBandInformationOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Set System Selection Preference */


/* --- Input -- */

/**
 * QmiMessageNasSetSystemSelectionPreferenceInput:
 *
 * The #QmiMessageNasSetSystemSelectionPreferenceInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetSystemSelectionPreferenceInput QmiMessageNasSetSystemSelectionPreferenceInput;
GType qmi_message_nas_set_system_selection_preference_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_SYSTEM_SELECTION_PREFERENCE_INPUT (qmi_message_nas_set_system_selection_preference_input_get_type ())

gboolean qmi_message_nas_set_system_selection_preference_input_get_td_scdma_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasTdScdmaBandPreference *td_scdma_band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_td_scdma_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasTdScdmaBandPreference td_scdma_band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_mnc_pds_digit_include_status (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    gboolean *mnc_pds_digit_include_status,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_mnc_pds_digit_include_status (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    gboolean mnc_pds_digit_include_status,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_gsm_wcdma_acquisition_order_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasGsmWcdmaAcquisitionOrderPreference *gsm_wcdma_acquisition_order_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_gsm_wcdma_acquisition_order_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasGsmWcdmaAcquisitionOrderPreference gsm_wcdma_acquisition_order_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_service_domain_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasServiceDomainPreference *service_domain_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_service_domain_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasServiceDomainPreference service_domain_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_change_duration (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasChangeDuration *change_duration,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_change_duration (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasChangeDuration change_duration,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_network_selection_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasNetworkSelectionPreference *network_selection_preference_mode,
    guint16 *network_selection_preference_mcc,
    guint16 *network_selection_preference_mnc,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_network_selection_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasNetworkSelectionPreference network_selection_preference_mode,
    guint16 network_selection_preference_mcc,
    guint16 network_selection_preference_mnc,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_lte_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasLteBandPreference *lte_band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_lte_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasLteBandPreference lte_band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_roaming_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasRoamingPreference *roaming_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_roaming_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasRoamingPreference roaming_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_cdma_prl_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasCdmaPrlPreference *cdma_prl_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_cdma_prl_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasCdmaPrlPreference cdma_prl_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasBandPreference *band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_band_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasBandPreference band_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_mode_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasRatModePreference *mode_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_mode_preference (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    QmiNasRatModePreference mode_preference,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_get_emergency_mode (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    gboolean *emergency_mode,
    GError **error);

gboolean qmi_message_nas_set_system_selection_preference_input_set_emergency_mode (
    QmiMessageNasSetSystemSelectionPreferenceInput *self,
    gboolean emergency_mode,
    GError **error);

QmiMessageNasSetSystemSelectionPreferenceInput *qmi_message_nas_set_system_selection_preference_input_ref (QmiMessageNasSetSystemSelectionPreferenceInput *self);
void qmi_message_nas_set_system_selection_preference_input_unref (QmiMessageNasSetSystemSelectionPreferenceInput *self);
QmiMessageNasSetSystemSelectionPreferenceInput *qmi_message_nas_set_system_selection_preference_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasSetSystemSelectionPreferenceOutput:
 *
 * The #QmiMessageNasSetSystemSelectionPreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasSetSystemSelectionPreferenceOutput QmiMessageNasSetSystemSelectionPreferenceOutput;
GType qmi_message_nas_set_system_selection_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_SET_SYSTEM_SELECTION_PREFERENCE_OUTPUT (qmi_message_nas_set_system_selection_preference_output_get_type ())

gboolean qmi_message_nas_set_system_selection_preference_output_get_result (
    QmiMessageNasSetSystemSelectionPreferenceOutput *self,
    GError **error);

QmiMessageNasSetSystemSelectionPreferenceOutput *qmi_message_nas_set_system_selection_preference_output_ref (QmiMessageNasSetSystemSelectionPreferenceOutput *self);
void qmi_message_nas_set_system_selection_preference_output_unref (QmiMessageNasSetSystemSelectionPreferenceOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get System Selection Preference */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetSystemSelectionPreferenceOutput:
 *
 * The #QmiMessageNasGetSystemSelectionPreferenceOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetSystemSelectionPreferenceOutput QmiMessageNasGetSystemSelectionPreferenceOutput;
GType qmi_message_nas_get_system_selection_preference_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SYSTEM_SELECTION_PREFERENCE_OUTPUT (qmi_message_nas_get_system_selection_preference_output_get_type ())

gboolean qmi_message_nas_get_system_selection_preference_output_get_result (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_manual_network_selection (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    guint16 *manual_network_selection_mcc,
    guint16 *manual_network_selection_mnc,
    gboolean *manual_network_selection_includes_pcs_digit,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_td_scdma_band_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasTdScdmaBandPreference *td_scdma_band_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_gsm_wcdma_acquisition_order_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasGsmWcdmaAcquisitionOrderPreference *gsm_wcdma_acquisition_order_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_service_domain_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasServiceDomainPreference *service_domain_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_network_selection_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasNetworkSelectionPreference *network_selection_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_lte_band_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasLteBandPreference *lte_band_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_roaming_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasRoamingPreference *roaming_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_cdma_prl_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasCdmaPrlPreference *cdma_prl_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_band_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasBandPreference *band_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_mode_preference (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    QmiNasRatModePreference *mode_preference,
    GError **error);

gboolean qmi_message_nas_get_system_selection_preference_output_get_emergency_mode (
    QmiMessageNasGetSystemSelectionPreferenceOutput *self,
    gboolean *emergency_mode,
    GError **error);

QmiMessageNasGetSystemSelectionPreferenceOutput *qmi_message_nas_get_system_selection_preference_output_ref (QmiMessageNasGetSystemSelectionPreferenceOutput *self);
void qmi_message_nas_get_system_selection_preference_output_unref (QmiMessageNasGetSystemSelectionPreferenceOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication NAS Network Time */


/* --- Output -- */

/**
 * QmiIndicationNasNetworkTimeOutput:
 *
 * The #QmiIndicationNasNetworkTimeOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationNasNetworkTimeOutput QmiIndicationNasNetworkTimeOutput;
GType qmi_indication_nas_network_time_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_NAS_NETWORK_TIME_OUTPUT (qmi_indication_nas_network_time_output_get_type ())

gboolean qmi_indication_nas_network_time_output_get_radio_interface (
    QmiIndicationNasNetworkTimeOutput *self,
    QmiNasRadioInterface *radio_interface,
    GError **error);

gboolean qmi_indication_nas_network_time_output_get_daylight_savings_adjustment (
    QmiIndicationNasNetworkTimeOutput *self,
    QmiNasDaylightSavingsAdjustment *daylight_savings_adjustment,
    GError **error);

gboolean qmi_indication_nas_network_time_output_get_timezone_offset (
    QmiIndicationNasNetworkTimeOutput *self,
    gint8 *timezone_offset,
    GError **error);

gboolean qmi_indication_nas_network_time_output_get_universal_time (
    QmiIndicationNasNetworkTimeOutput *self,
    guint16 *universal_time_year,
    guint8 *universal_time_month,
    guint8 *universal_time_day,
    guint8 *universal_time_hour,
    guint8 *universal_time_minute,
    guint8 *universal_time_second,
    QmiNasDayOfWeek *universal_time_day_of_week,
    GError **error);

QmiIndicationNasNetworkTimeOutput *qmi_indication_nas_network_time_output_ref (QmiIndicationNasNetworkTimeOutput *self);
void qmi_indication_nas_network_time_output_unref (QmiIndicationNasNetworkTimeOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get System Info */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetSystemInfoOutput:
 *
 * The #QmiMessageNasGetSystemInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetSystemInfoOutput QmiMessageNasGetSystemInfoOutput;
GType qmi_message_nas_get_system_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SYSTEM_INFO_OUTPUT (qmi_message_nas_get_system_info_output_get_type ())

gboolean qmi_message_nas_get_system_info_output_get_result (
    QmiMessageNasGetSystemInfoOutput *self,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_sim_reject_info (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasSimRejectState *sim_reject_info,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_lte_embms_coverage_info_support (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *lte_embms_coverage_info_support,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_td_scdma_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *td_scdma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *td_scdma_system_info_domain,
    gboolean *td_scdma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *td_scdma_system_info_service_capability,
    gboolean *td_scdma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *td_scdma_system_info_roaming_status,
    gboolean *td_scdma_system_info_forbidden_valid,
    gboolean *td_scdma_system_info_forbidden,
    gboolean *td_scdma_system_info_lac_valid,
    guint16 *td_scdma_system_info_lac,
    gboolean *td_scdma_system_info_cid_valid,
    guint32 *td_scdma_system_info_cid,
    gboolean *td_scdma_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *td_scdma_system_info_registration_reject_domain,
    guint8 *td_scdma_system_info_registration_reject_cause,
    gboolean *td_scdma_system_info_network_id_valid,
    const gchar **td_scdma_system_info_mcc,
    const gchar **td_scdma_system_info_mnc,
    gboolean *td_scdma_system_info_hs_call_status_valid,
    QmiNasWcdmaHsService *td_scdma_system_info_hs_call_status,
    gboolean *td_scdma_system_info_hs_service_valid,
    QmiNasWcdmaHsService *td_scdma_system_info_hs_service,
    gboolean *td_scdma_system_info_cell_parameter_id_valid,
    guint16 *td_scdma_system_info_cell_parameter_id,
    gboolean *td_scdma_system_info_cell_broadcast_support_valid,
    QmiNasCellBroadcastCapability *td_scdma_system_info_cell_broadcast_support,
    gboolean *td_scdma_system_info_cs_call_barring_status_valid,
    QmiNasCallBarringStatus *td_scdma_system_info_cs_call_barring_status,
    gboolean *td_scdma_system_info_ps_call_barring_status_valid,
    QmiNasCallBarringStatus *td_scdma_system_info_ps_call_barring_status,
    gboolean *td_scdma_system_info_cipher_domain_valid,
    QmiNasNetworkServiceDomain *td_scdma_system_info_cipher_domain,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_td_scdma_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *td_scdma_service_status_service_status,
    QmiNasServiceStatus *td_scdma_service_status_true_service_status,
    gboolean *td_scdma_service_status_preferred_data_path,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_wcdma_cipher_domain (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasNetworkServiceDomain *wcdma_cipher_domain,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_gsm_cipher_domain (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasNetworkServiceDomain *gsm_cipher_domain,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_lte_voice_support (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *lte_voice_support,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_wcdma_call_barring_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasCallBarringStatus *wcdma_call_barring_status_cs_status,
    QmiNasCallBarringStatus *wcdma_call_barring_status_ps_status,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_gsm_call_barring_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasCallBarringStatus *gsm_call_barring_status_cs_status,
    QmiNasCallBarringStatus *gsm_call_barring_status_ps_status,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_additional_lte_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    guint16 *additional_lte_system_info_geo_system_index,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_additional_wcdma_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    guint16 *additional_wcdma_system_info_geo_system_index,
    QmiNasCellBroadcastCapability *additional_wcdma_system_info_cell_broadcast_support,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_additional_gsm_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    guint16 *additional_gsm_system_info_geo_system_index,
    QmiNasCellBroadcastCapability *additional_gsm_system_info_cell_broadcast_support,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_additional_hdr_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    guint16 *additional_hdr_system_info_geo_system_index,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_additional_cdma_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    guint16 *additional_cdma_system_info_geo_system_index,
    guint16 *additional_cdma_system_info_registration_period,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_lte_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *lte_system_info_domain_valid,
    QmiNasNetworkServiceDomain *lte_system_info_domain,
    gboolean *lte_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *lte_system_info_service_capability,
    gboolean *lte_system_info_roaming_status_valid,
    QmiNasRoamingStatus *lte_system_info_roaming_status,
    gboolean *lte_system_info_forbidden_valid,
    gboolean *lte_system_info_forbidden,
    gboolean *lte_system_info_lac_valid,
    guint16 *lte_system_info_lac,
    gboolean *lte_system_info_cid_valid,
    guint32 *lte_system_info_cid,
    gboolean *lte_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *lte_system_info_registration_reject_domain,
    guint8 *lte_system_info_registration_reject_cause,
    gboolean *lte_system_info_network_id_valid,
    const gchar **lte_system_info_mcc,
    const gchar **lte_system_info_mnc,
    gboolean *lte_system_info_tac_valid,
    guint16 *lte_system_info_tac,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_wcdma_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *wcdma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_domain,
    gboolean *wcdma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_service_capability,
    gboolean *wcdma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *wcdma_system_info_roaming_status,
    gboolean *wcdma_system_info_forbidden_valid,
    gboolean *wcdma_system_info_forbidden,
    gboolean *wcdma_system_info_lac_valid,
    guint16 *wcdma_system_info_lac,
    gboolean *wcdma_system_info_cid_valid,
    guint32 *wcdma_system_info_cid,
    gboolean *wcdma_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_registration_reject_domain,
    guint8 *wcdma_system_info_registration_reject_cause,
    gboolean *wcdma_system_info_network_id_valid,
    const gchar **wcdma_system_info_mcc,
    const gchar **wcdma_system_info_mnc,
    gboolean *wcdma_system_info_hs_call_status_valid,
    QmiNasWcdmaHsService *wcdma_system_info_hs_call_status,
    gboolean *wcdma_system_info_hs_service_valid,
    QmiNasWcdmaHsService *wcdma_system_info_hs_service,
    gboolean *wcdma_system_info_primary_scrambling_code_valid,
    guint16 *wcdma_system_info_primary_scrambling_code,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_gsm_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *gsm_system_info_domain_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_domain,
    gboolean *gsm_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_service_capability,
    gboolean *gsm_system_info_roaming_status_valid,
    QmiNasRoamingStatus *gsm_system_info_roaming_status,
    gboolean *gsm_system_info_forbidden_valid,
    gboolean *gsm_system_info_forbidden,
    gboolean *gsm_system_info_lac_valid,
    guint16 *gsm_system_info_lac,
    gboolean *gsm_system_info_cid_valid,
    guint32 *gsm_system_info_cid,
    gboolean *gsm_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_registration_reject_domain,
    guint8 *gsm_system_info_registration_reject_cause,
    gboolean *gsm_system_info_network_id_valid,
    const gchar **gsm_system_info_mcc,
    const gchar **gsm_system_info_mnc,
    gboolean *gsm_system_info_egprs_support_valid,
    gboolean *gsm_system_info_egprs_support,
    gboolean *gsm_system_info_dtm_support_valid,
    gboolean *gsm_system_info_dtm_support,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_hdr_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *hdr_system_info_domain_valid,
    QmiNasNetworkServiceDomain *hdr_system_info_domain,
    gboolean *hdr_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *hdr_system_info_service_capability,
    gboolean *hdr_system_info_roaming_status_valid,
    QmiNasRoamingStatus *hdr_system_info_roaming_status,
    gboolean *hdr_system_info_forbidden_valid,
    gboolean *hdr_system_info_forbidden,
    gboolean *hdr_system_info_prl_match_valid,
    gboolean *hdr_system_info_prl_match,
    gboolean *hdr_system_info_personality_valid,
    QmiNasHdrPersonality *hdr_system_info_personality,
    gboolean *hdr_system_info_protocol_revision_valid,
    QmiNasHdrProtocolRevision *hdr_system_info_protocol_revision,
    gboolean *hdr_system_info_is_856_system_id_valid,
    const gchar **hdr_system_info_is_856_system_id,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_cdma_system_info (
    QmiMessageNasGetSystemInfoOutput *self,
    gboolean *cdma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *cdma_system_info_domain,
    gboolean *cdma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *cdma_system_info_service_capability,
    gboolean *cdma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *cdma_system_info_roaming_status,
    gboolean *cdma_system_info_forbidden_valid,
    gboolean *cdma_system_info_forbidden,
    gboolean *cdma_system_info_prl_match_valid,
    gboolean *cdma_system_info_prl_match,
    gboolean *cdma_system_info_p_rev_valid,
    guint8 *cdma_system_info_p_rev,
    gboolean *cdma_system_info_base_station_p_rev_valid,
    guint8 *cdma_system_info_base_station_p_rev,
    gboolean *cdma_system_info_concurrent_service_support_valid,
    gboolean *cdma_system_info_concurrent_service_support,
    gboolean *cdma_system_info_cdma_system_id_valid,
    guint16 *cdma_system_info_sid,
    guint16 *cdma_system_info_nid,
    gboolean *cdma_system_info_base_station_info_valid,
    guint16 *cdma_system_info_base_station_id,
    gint32 *cdma_system_info_base_station_latitude,
    gint32 *cdma_system_info_base_station_longitude,
    gboolean *cdma_system_info_packet_zone_valid,
    guint16 *cdma_system_info_packet_zone,
    gboolean *cdma_system_info_network_id_valid,
    const gchar **cdma_system_info_mcc,
    const gchar **cdma_system_info_mnc,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_lte_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *lte_service_status_service_status,
    QmiNasServiceStatus *lte_service_status_true_service_status,
    gboolean *lte_service_status_preferred_data_path,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_wcdma_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *wcdma_service_status_service_status,
    QmiNasServiceStatus *wcdma_service_status_true_service_status,
    gboolean *wcdma_service_status_preferred_data_path,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_gsm_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *gsm_service_status_service_status,
    QmiNasServiceStatus *gsm_service_status_true_service_status,
    gboolean *gsm_service_status_preferred_data_path,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_hdr_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *hdr_service_status_service_status,
    gboolean *hdr_service_status_preferred_data_path,
    GError **error);

gboolean qmi_message_nas_get_system_info_output_get_cdma_service_status (
    QmiMessageNasGetSystemInfoOutput *self,
    QmiNasServiceStatus *cdma_service_status_service_status,
    gboolean *cdma_service_status_preferred_data_path,
    GError **error);

QmiMessageNasGetSystemInfoOutput *qmi_message_nas_get_system_info_output_ref (QmiMessageNasGetSystemInfoOutput *self);
void qmi_message_nas_get_system_info_output_unref (QmiMessageNasGetSystemInfoOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication NAS System Info */


/* --- Output -- */

/**
 * QmiIndicationNasSystemInfoOutput:
 *
 * The #QmiIndicationNasSystemInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationNasSystemInfoOutput QmiIndicationNasSystemInfoOutput;
GType qmi_indication_nas_system_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_NAS_SYSTEM_INFO_OUTPUT (qmi_indication_nas_system_info_output_get_type ())

gboolean qmi_indication_nas_system_info_output_get_sim_reject_info (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasSimRejectState *sim_reject_info,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_lte_embms_coverage_info_support (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *lte_embms_coverage_info_support,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_td_scma_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *td_scma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *td_scma_system_info_domain,
    gboolean *td_scma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *td_scma_system_info_service_capability,
    gboolean *td_scma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *td_scma_system_info_roaming_status,
    gboolean *td_scma_system_info_forbidden_valid,
    gboolean *td_scma_system_info_forbidden,
    gboolean *td_scma_system_info_lac_valid,
    guint16 *td_scma_system_info_lac,
    gboolean *td_scma_system_info_cid_valid,
    guint32 *td_scma_system_info_cid,
    gboolean *td_scma_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *td_scma_system_info_registration_reject_domain,
    guint8 *td_scma_system_info_registration_reject_cause,
    gboolean *td_scma_system_info_network_id_valid,
    const gchar **td_scma_system_info_mcc,
    const gchar **td_scma_system_info_mnc,
    gboolean *td_scma_system_info_hs_call_status_valid,
    QmiNasWcdmaHsService *td_scma_system_info_hs_call_status,
    gboolean *td_scma_system_info_hs_service_valid,
    QmiNasWcdmaHsService *td_scma_system_info_hs_service,
    gboolean *td_scma_system_info_cell_parameter_id_valid,
    guint16 *td_scma_system_info_cell_parameter_id,
    gboolean *td_scma_system_info_cell_broadcast_support_valid,
    QmiNasCellBroadcastCapability *td_scma_system_info_cell_broadcast_support,
    gboolean *td_scma_system_info_cs_call_barring_status_valid,
    QmiNasCallBarringStatus *td_scma_system_info_cs_call_barring_status,
    gboolean *td_scma_system_info_ps_call_barring_status_valid,
    QmiNasCallBarringStatus *td_scma_system_info_ps_call_barring_status,
    gboolean *td_scma_system_info_cipher_domain_valid,
    QmiNasNetworkServiceDomain *td_scma_system_info_cipher_domain,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_td_scdma_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *td_scdma_service_status_service_status,
    QmiNasServiceStatus *td_scdma_service_status_true_service_status,
    gboolean *td_scdma_service_status_preferred_data_path,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_plmn_not_changed_indication (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *plmn_not_changed_indication,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_wcdma_cipher_domain (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasNetworkServiceDomain *wcdma_cipher_domain,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_gsm_cipher_domain (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasNetworkServiceDomain *gsm_cipher_domain,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_lte_voice_support (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *lte_voice_support,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_wcdma_call_barring_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasCallBarringStatus *wcdma_call_barring_status_cs_status,
    QmiNasCallBarringStatus *wcdma_call_barring_status_ps_status,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_gsm_call_barring_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasCallBarringStatus *gsm_call_barring_status_cs_status,
    QmiNasCallBarringStatus *gsm_call_barring_status_ps_status,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_additional_lte_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    guint16 *additional_lte_system_info_geo_system_index,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_additional_wcdma_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    guint16 *additional_wcdma_system_info_geo_system_index,
    QmiNasCellBroadcastCapability *additional_wcdma_system_info_cell_broadcast_support,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_additional_gsm_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    guint16 *additional_gsm_system_info_geo_system_index,
    QmiNasCellBroadcastCapability *additional_gsm_system_info_cell_broadcast_support,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_additional_hdr_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    guint16 *additional_hdr_system_info_geo_system_index,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_additional_cdma_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    guint16 *additional_cdma_system_info_geo_system_index,
    guint16 *additional_cdma_system_info_registration_period,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_lte_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *lte_system_info_domain_valid,
    QmiNasNetworkServiceDomain *lte_system_info_domain,
    gboolean *lte_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *lte_system_info_service_capability,
    gboolean *lte_system_info_roaming_status_valid,
    QmiNasRoamingStatus *lte_system_info_roaming_status,
    gboolean *lte_system_info_forbidden_valid,
    gboolean *lte_system_info_forbidden,
    gboolean *lte_system_info_lac_valid,
    guint16 *lte_system_info_lac,
    gboolean *lte_system_info_cid_valid,
    guint32 *lte_system_info_cid,
    gboolean *lte_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *lte_system_info_registration_reject_domain,
    guint8 *lte_system_info_registration_reject_cause,
    gboolean *lte_system_info_network_id_valid,
    const gchar **lte_system_info_mcc,
    const gchar **lte_system_info_mnc,
    gboolean *lte_system_info_tac_valid,
    guint16 *lte_system_info_tac,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_wcdma_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *wcdma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_domain,
    gboolean *wcdma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_service_capability,
    gboolean *wcdma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *wcdma_system_info_roaming_status,
    gboolean *wcdma_system_info_forbidden_valid,
    gboolean *wcdma_system_info_forbidden,
    gboolean *wcdma_system_info_lac_valid,
    guint16 *wcdma_system_info_lac,
    gboolean *wcdma_system_info_cid_valid,
    guint32 *wcdma_system_info_cid,
    gboolean *wcdma_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *wcdma_system_info_registration_reject_domain,
    guint8 *wcdma_system_info_registration_reject_cause,
    gboolean *wcdma_system_info_network_id_valid,
    const gchar **wcdma_system_info_mcc,
    const gchar **wcdma_system_info_mnc,
    gboolean *wcdma_system_info_hs_call_status_valid,
    QmiNasWcdmaHsService *wcdma_system_info_hs_call_status,
    gboolean *wcdma_system_info_hs_service_valid,
    QmiNasWcdmaHsService *wcdma_system_info_hs_service,
    gboolean *wcdma_system_info_primary_scrambling_code_valid,
    guint16 *wcdma_system_info_primary_scrambling_code,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_gsm_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *gsm_system_info_domain_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_domain,
    gboolean *gsm_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_service_capability,
    gboolean *gsm_system_info_roaming_status_valid,
    QmiNasRoamingStatus *gsm_system_info_roaming_status,
    gboolean *gsm_system_info_forbidden_valid,
    gboolean *gsm_system_info_forbidden,
    gboolean *gsm_system_info_lac_valid,
    guint16 *gsm_system_info_lac,
    gboolean *gsm_system_info_cid_valid,
    guint32 *gsm_system_info_cid,
    gboolean *gsm_system_info_registration_reject_info_valid,
    QmiNasNetworkServiceDomain *gsm_system_info_registration_reject_domain,
    guint8 *gsm_system_info_registration_reject_cause,
    gboolean *gsm_system_info_network_id_valid,
    const gchar **gsm_system_info_mcc,
    const gchar **gsm_system_info_mnc,
    gboolean *gsm_system_info_egprs_support_valid,
    gboolean *gsm_system_info_egprs_support,
    gboolean *gsm_system_info_dtm_support_valid,
    gboolean *gsm_system_info_dtm_support,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_hdr_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *hdr_system_info_domain_valid,
    QmiNasNetworkServiceDomain *hdr_system_info_domain,
    gboolean *hdr_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *hdr_system_info_service_capability,
    gboolean *hdr_system_info_roaming_status_valid,
    QmiNasRoamingStatus *hdr_system_info_roaming_status,
    gboolean *hdr_system_info_forbidden_valid,
    gboolean *hdr_system_info_forbidden,
    gboolean *hdr_system_info_prl_match_valid,
    gboolean *hdr_system_info_prl_match,
    gboolean *hdr_system_info_personality_valid,
    QmiNasHdrPersonality *hdr_system_info_personality,
    gboolean *hdr_system_info_protocol_revision_valid,
    QmiNasHdrProtocolRevision *hdr_system_info_protocol_revision,
    gboolean *hdr_system_info_is_856_system_id_valid,
    const gchar **hdr_system_info_is_856_system_id,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_cdma_system_info (
    QmiIndicationNasSystemInfoOutput *self,
    gboolean *cdma_system_info_domain_valid,
    QmiNasNetworkServiceDomain *cdma_system_info_domain,
    gboolean *cdma_system_info_service_capability_valid,
    QmiNasNetworkServiceDomain *cdma_system_info_service_capability,
    gboolean *cdma_system_info_roaming_status_valid,
    QmiNasRoamingStatus *cdma_system_info_roaming_status,
    gboolean *cdma_system_info_forbidden_valid,
    gboolean *cdma_system_info_forbidden,
    gboolean *cdma_system_info_prl_match_valid,
    gboolean *cdma_system_info_prl_match,
    gboolean *cdma_system_info_p_rev_valid,
    guint8 *cdma_system_info_p_rev,
    gboolean *cdma_system_info_base_station_p_rev_valid,
    guint8 *cdma_system_info_base_station_p_rev,
    gboolean *cdma_system_info_concurrent_service_support_valid,
    gboolean *cdma_system_info_concurrent_service_support,
    gboolean *cdma_system_info_cdma_system_id_valid,
    guint16 *cdma_system_info_sid,
    guint16 *cdma_system_info_nid,
    gboolean *cdma_system_info_base_station_info_valid,
    guint16 *cdma_system_info_base_station_id,
    gint32 *cdma_system_info_base_station_latitude,
    gint32 *cdma_system_info_base_station_longitude,
    gboolean *cdma_system_info_packet_zone_valid,
    guint16 *cdma_system_info_packet_zone,
    gboolean *cdma_system_info_network_id_valid,
    const gchar **cdma_system_info_mcc,
    const gchar **cdma_system_info_mnc,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_lte_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *lte_service_status_service_status,
    QmiNasServiceStatus *lte_service_status_true_service_status,
    gboolean *lte_service_status_preferred_data_path,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_wcdma_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *wcdma_service_status_service_status,
    QmiNasServiceStatus *wcdma_service_status_true_service_status,
    gboolean *wcdma_service_status_preferred_data_path,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_gsm_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *gsm_service_status_service_status,
    QmiNasServiceStatus *gsm_service_status_true_service_status,
    gboolean *gsm_service_status_preferred_data_path,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_hdr_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *hdr_service_status_service_status,
    gboolean *hdr_service_status_preferred_data_path,
    GError **error);

gboolean qmi_indication_nas_system_info_output_get_cdma_service_status (
    QmiIndicationNasSystemInfoOutput *self,
    QmiNasServiceStatus *cdma_service_status_service_status,
    gboolean *cdma_service_status_preferred_data_path,
    GError **error);

QmiIndicationNasSystemInfoOutput *qmi_indication_nas_system_info_output_ref (QmiIndicationNasSystemInfoOutput *self);
void qmi_indication_nas_system_info_output_unref (QmiIndicationNasSystemInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Signal Info */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetSignalInfoOutput:
 *
 * The #QmiMessageNasGetSignalInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetSignalInfoOutput QmiMessageNasGetSignalInfoOutput;
GType qmi_message_nas_get_signal_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_SIGNAL_INFO_OUTPUT (qmi_message_nas_get_signal_info_output_get_type ())

gboolean qmi_message_nas_get_signal_info_output_get_result (
    QmiMessageNasGetSignalInfoOutput *self,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_tdma_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *tdma_signal_strength,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_lte_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *lte_signal_strength_rssi,
    gint8 *lte_signal_strength_rsrq,
    gint16 *lte_signal_strength_rsrp,
    gint16 *lte_signal_strength_snr,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_wcdma_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *wcdma_signal_strength_rssi,
    gint16 *wcdma_signal_strength_ecio,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_gsm_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *gsm_signal_strength,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_hdr_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *hdr_signal_strength_rssi,
    gint16 *hdr_signal_strength_ecio,
    QmiNasEvdoSinrLevel *hdr_signal_strength_sinr,
    gint32 *hdr_signal_strength_io,
    GError **error);

gboolean qmi_message_nas_get_signal_info_output_get_cdma_signal_strength (
    QmiMessageNasGetSignalInfoOutput *self,
    gint8 *cdma_signal_strength_rssi,
    gint16 *cdma_signal_strength_ecio,
    GError **error);

QmiMessageNasGetSignalInfoOutput *qmi_message_nas_get_signal_info_output_ref (QmiMessageNasGetSignalInfoOutput *self);
void qmi_message_nas_get_signal_info_output_unref (QmiMessageNasGetSignalInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Config Signal Info */


/* --- Input -- */

/**
 * QmiMessageNasConfigSignalInfoInput:
 *
 * The #QmiMessageNasConfigSignalInfoInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasConfigSignalInfoInput QmiMessageNasConfigSignalInfoInput;
GType qmi_message_nas_config_signal_info_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_CONFIG_SIGNAL_INFO_INPUT (qmi_message_nas_config_signal_info_input_get_type ())

gboolean qmi_message_nas_config_signal_info_input_get_rscp_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **rscp_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_rscp_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *rscp_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_lte_report (
    QmiMessageNasConfigSignalInfoInput *self,
    guint8 *lte_report_rate,
    guint8 *lte_report_average_period,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_lte_report (
    QmiMessageNasConfigSignalInfoInput *self,
    guint8 lte_report_rate,
    guint8 lte_report_average_period,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_rsrp_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **rsrp_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_rsrp_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *rsrp_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_rsrq_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **rsrq_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_rsrq_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *rsrq_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_io_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **io_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_io_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *io_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_lte_snr_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **lte_snr_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_lte_snr_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *lte_snr_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_sinr_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **sinr_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_sinr_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *sinr_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_ecio_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **ecio_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_ecio_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *ecio_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_get_rssi_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray **rssi_threshold,
    GError **error);

gboolean qmi_message_nas_config_signal_info_input_set_rssi_threshold (
    QmiMessageNasConfigSignalInfoInput *self,
    GArray *rssi_threshold,
    GError **error);

QmiMessageNasConfigSignalInfoInput *qmi_message_nas_config_signal_info_input_ref (QmiMessageNasConfigSignalInfoInput *self);
void qmi_message_nas_config_signal_info_input_unref (QmiMessageNasConfigSignalInfoInput *self);
QmiMessageNasConfigSignalInfoInput *qmi_message_nas_config_signal_info_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasConfigSignalInfoOutput:
 *
 * The #QmiMessageNasConfigSignalInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasConfigSignalInfoOutput QmiMessageNasConfigSignalInfoOutput;
GType qmi_message_nas_config_signal_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_CONFIG_SIGNAL_INFO_OUTPUT (qmi_message_nas_config_signal_info_output_get_type ())

gboolean qmi_message_nas_config_signal_info_output_get_result (
    QmiMessageNasConfigSignalInfoOutput *self,
    GError **error);

QmiMessageNasConfigSignalInfoOutput *qmi_message_nas_config_signal_info_output_ref (QmiMessageNasConfigSignalInfoOutput *self);
void qmi_message_nas_config_signal_info_output_unref (QmiMessageNasConfigSignalInfoOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication NAS Signal Info */


/* --- Output -- */

/**
 * QmiIndicationNasSignalInfoOutput:
 *
 * The #QmiIndicationNasSignalInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationNasSignalInfoOutput QmiIndicationNasSignalInfoOutput;
GType qmi_indication_nas_signal_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_NAS_SIGNAL_INFO_OUTPUT (qmi_indication_nas_signal_info_output_get_type ())

gboolean qmi_indication_nas_signal_info_output_get_tdma_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *tdma_signal_strength,
    GError **error);

gboolean qmi_indication_nas_signal_info_output_get_lte_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *lte_signal_strength_rssi,
    gint8 *lte_signal_strength_rsrq,
    gint16 *lte_signal_strength_rsrp,
    gint16 *lte_signal_strength_snr,
    GError **error);

gboolean qmi_indication_nas_signal_info_output_get_wcdma_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *wcdma_signal_strength_rssi,
    gint16 *wcdma_signal_strength_ecio,
    GError **error);

gboolean qmi_indication_nas_signal_info_output_get_gsm_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *gsm_signal_strength,
    GError **error);

gboolean qmi_indication_nas_signal_info_output_get_hdr_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *hdr_signal_strength_rssi,
    gint16 *hdr_signal_strength_ecio,
    QmiNasEvdoSinrLevel *hdr_signal_strength_sinr,
    gint32 *hdr_signal_strength_io,
    GError **error);

gboolean qmi_indication_nas_signal_info_output_get_cdma_signal_strength (
    QmiIndicationNasSignalInfoOutput *self,
    gint8 *cdma_signal_strength_rssi,
    gint16 *cdma_signal_strength_ecio,
    GError **error);

QmiIndicationNasSignalInfoOutput *qmi_indication_nas_signal_info_output_ref (QmiIndicationNasSignalInfoOutput *self);
void qmi_indication_nas_signal_info_output_unref (QmiIndicationNasSignalInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get Tx Rx Info */


/* --- Input -- */

/**
 * QmiMessageNasGetTxRxInfoInput:
 *
 * The #QmiMessageNasGetTxRxInfoInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetTxRxInfoInput QmiMessageNasGetTxRxInfoInput;
GType qmi_message_nas_get_tx_rx_info_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_TX_RX_INFO_INPUT (qmi_message_nas_get_tx_rx_info_input_get_type ())

gboolean qmi_message_nas_get_tx_rx_info_input_get_radio_interface (
    QmiMessageNasGetTxRxInfoInput *self,
    QmiNasRadioInterface *radio_interface,
    GError **error);

gboolean qmi_message_nas_get_tx_rx_info_input_set_radio_interface (
    QmiMessageNasGetTxRxInfoInput *self,
    QmiNasRadioInterface radio_interface,
    GError **error);

QmiMessageNasGetTxRxInfoInput *qmi_message_nas_get_tx_rx_info_input_ref (QmiMessageNasGetTxRxInfoInput *self);
void qmi_message_nas_get_tx_rx_info_input_unref (QmiMessageNasGetTxRxInfoInput *self);
QmiMessageNasGetTxRxInfoInput *qmi_message_nas_get_tx_rx_info_input_new (void);

/* --- Output -- */

/**
 * QmiMessageNasGetTxRxInfoOutput:
 *
 * The #QmiMessageNasGetTxRxInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetTxRxInfoOutput QmiMessageNasGetTxRxInfoOutput;
GType qmi_message_nas_get_tx_rx_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_TX_RX_INFO_OUTPUT (qmi_message_nas_get_tx_rx_info_output_get_type ())

gboolean qmi_message_nas_get_tx_rx_info_output_get_result (
    QmiMessageNasGetTxRxInfoOutput *self,
    GError **error);

gboolean qmi_message_nas_get_tx_rx_info_output_get_rx_chain_0_info (
    QmiMessageNasGetTxRxInfoOutput *self,
    gboolean *rx_chain_0_info_is_radio_tuned,
    gint32 *rx_chain_0_info_rx_power,
    gint32 *rx_chain_0_info_ecio,
    gint32 *rx_chain_0_info_rscp,
    gint32 *rx_chain_0_info_rsrp,
    guint32 *rx_chain_0_info_phase,
    GError **error);

gboolean qmi_message_nas_get_tx_rx_info_output_get_rx_chain_1_info (
    QmiMessageNasGetTxRxInfoOutput *self,
    gboolean *rx_chain_1_info_is_radio_tuned,
    gint32 *rx_chain_1_info_rx_power,
    gint32 *rx_chain_1_info_ecio,
    gint32 *rx_chain_1_info_rscp,
    gint32 *rx_chain_1_info_rsrp,
    guint32 *rx_chain_1_info_phase,
    GError **error);

gboolean qmi_message_nas_get_tx_rx_info_output_get_tx_info (
    QmiMessageNasGetTxRxInfoOutput *self,
    gboolean *tx_info_is_in_traffic,
    gint32 *tx_info_tx_power,
    GError **error);

QmiMessageNasGetTxRxInfoOutput *qmi_message_nas_get_tx_rx_info_output_ref (QmiMessageNasGetTxRxInfoOutput *self);
void qmi_message_nas_get_tx_rx_info_output_unref (QmiMessageNasGetTxRxInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message NAS Get CDMA Position Info */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageNasGetCdmaPositionInfoOutputCdmaPositionInfoBasestationsBasestation:
 * @pilot_type: a #QmiNasCdmaPilotType.
 * @system_id: a #guint16.
 * @network_id: a #guint16.
 * @base_station_id: a #guint16.
 * @pilot_pn: a #guint16.
 * @pilot_strength: a #guint16.
 * @latitude: a #gint32.
 * @longitude: a #gint32.
 * @gps_time_in_milliseconds: a #guint64.
 *
 * A QmiMessageNasGetCdmaPositionInfoOutputCdmaPositionInfoBasestationsBasestation struct.
 */
typedef struct _QmiMessageNasGetCdmaPositionInfoOutputCdmaPositionInfoBasestationsBasestation {
    QmiNasCdmaPilotType pilot_type;
    guint16 system_id;
    guint16 network_id;
    guint16 base_station_id;
    guint16 pilot_pn;
    guint16 pilot_strength;
    gint32 latitude;
    gint32 longitude;
    guint64 gps_time_in_milliseconds;
} QmiMessageNasGetCdmaPositionInfoOutputCdmaPositionInfoBasestationsBasestation;

/**
 * QmiMessageNasGetCdmaPositionInfoOutput:
 *
 * The #QmiMessageNasGetCdmaPositionInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageNasGetCdmaPositionInfoOutput QmiMessageNasGetCdmaPositionInfoOutput;
GType qmi_message_nas_get_cdma_position_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_NAS_GET_CDMA_POSITION_INFO_OUTPUT (qmi_message_nas_get_cdma_position_info_output_get_type ())

gboolean qmi_message_nas_get_cdma_position_info_output_get_result (
    QmiMessageNasGetCdmaPositionInfoOutput *self,
    GError **error);

gboolean qmi_message_nas_get_cdma_position_info_output_get_cdma_position_info (
    QmiMessageNasGetCdmaPositionInfoOutput *self,
    gint8 *cdma_position_info_ui_in_idle_mode,
    GArray **cdma_position_info_basestations,
    GError **error);

QmiMessageNasGetCdmaPositionInfoOutput *qmi_message_nas_get_cdma_position_info_output_ref (QmiMessageNasGetCdmaPositionInfoOutput *self);
void qmi_message_nas_get_cdma_position_info_output_unref (QmiMessageNasGetCdmaPositionInfoOutput *self);

/*****************************************************************************/
/* Service-specific printable: NAS */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_nas_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_nas_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client NAS */

#define QMI_TYPE_CLIENT_NAS            (qmi_client_nas_get_type ())
#define QMI_CLIENT_NAS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_NAS, QmiClientNas))
#define QMI_CLIENT_NAS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_NAS, QmiClientNasClass))
#define QMI_IS_CLIENT_NAS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_NAS))
#define QMI_IS_CLIENT_NAS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_NAS))
#define QMI_CLIENT_NAS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_NAS, QmiClientNasClass))

typedef struct _QmiClientNas QmiClientNas;
typedef struct _QmiClientNasClass QmiClientNasClass;

/**
 * QmiClientNas:
 *
 * The #QmiClientNas structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientNas {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientNasClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_nas_get_type (void);


void qmi_client_nas_reset (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasResetOutput *qmi_client_nas_reset_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_set_event_report (
    QmiClientNas *self,
    QmiMessageNasSetEventReportInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasSetEventReportOutput *qmi_client_nas_set_event_report_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_register_indications (
    QmiClientNas *self,
    QmiMessageNasRegisterIndicationsInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasRegisterIndicationsOutput *qmi_client_nas_register_indications_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_signal_strength (
    QmiClientNas *self,
    QmiMessageNasGetSignalStrengthInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetSignalStrengthOutput *qmi_client_nas_get_signal_strength_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_network_scan (
    QmiClientNas *self,
    QmiMessageNasNetworkScanInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasNetworkScanOutput *qmi_client_nas_network_scan_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_initiate_network_register (
    QmiClientNas *self,
    QmiMessageNasInitiateNetworkRegisterInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasInitiateNetworkRegisterOutput *qmi_client_nas_initiate_network_register_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_serving_system (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetServingSystemOutput *qmi_client_nas_get_serving_system_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_home_network (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetHomeNetworkOutput *qmi_client_nas_get_home_network_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_set_technology_preference (
    QmiClientNas *self,
    QmiMessageNasSetTechnologyPreferenceInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasSetTechnologyPreferenceOutput *qmi_client_nas_set_technology_preference_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_technology_preference (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetTechnologyPreferenceOutput *qmi_client_nas_get_technology_preference_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_rf_band_information (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetRfBandInformationOutput *qmi_client_nas_get_rf_band_information_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_set_system_selection_preference (
    QmiClientNas *self,
    QmiMessageNasSetSystemSelectionPreferenceInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasSetSystemSelectionPreferenceOutput *qmi_client_nas_set_system_selection_preference_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_system_selection_preference (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetSystemSelectionPreferenceOutput *qmi_client_nas_get_system_selection_preference_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_system_info (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetSystemInfoOutput *qmi_client_nas_get_system_info_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_signal_info (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetSignalInfoOutput *qmi_client_nas_get_signal_info_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_config_signal_info (
    QmiClientNas *self,
    QmiMessageNasConfigSignalInfoInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasConfigSignalInfoOutput *qmi_client_nas_config_signal_info_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_tx_rx_info (
    QmiClientNas *self,
    QmiMessageNasGetTxRxInfoInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetTxRxInfoOutput *qmi_client_nas_get_tx_rx_info_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_nas_get_cdma_position_info (
    QmiClientNas *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageNasGetCdmaPositionInfoOutput *qmi_client_nas_get_cdma_position_info_finish (
    QmiClientNas *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_NAS__ */
