
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
#include "qmi-enums-wms.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_WMS__
#define __LIBQMI_GLIB_QMI_WMS__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWmsResetOutput:
 *
 * The #QmiMessageWmsResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsResetOutput QmiMessageWmsResetOutput;
GType qmi_message_wms_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RESET_OUTPUT (qmi_message_wms_reset_output_get_type ())

gboolean qmi_message_wms_reset_output_get_result (
    QmiMessageWmsResetOutput *self,
    GError **error);

QmiMessageWmsResetOutput *qmi_message_wms_reset_output_ref (QmiMessageWmsResetOutput *self);
void qmi_message_wms_reset_output_unref (QmiMessageWmsResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Set Event Report */


/* --- Input -- */

/**
 * QmiMessageWmsSetEventReportInput:
 *
 * The #QmiMessageWmsSetEventReportInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSetEventReportInput QmiMessageWmsSetEventReportInput;
GType qmi_message_wms_set_event_report_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SET_EVENT_REPORT_INPUT (qmi_message_wms_set_event_report_input_get_type ())

gboolean qmi_message_wms_set_event_report_input_get_new_mt_message_indicator (
    QmiMessageWmsSetEventReportInput *self,
    gboolean *new_mt_message_indicator_report,
    GError **error);

gboolean qmi_message_wms_set_event_report_input_set_new_mt_message_indicator (
    QmiMessageWmsSetEventReportInput *self,
    gboolean new_mt_message_indicator_report,
    GError **error);

QmiMessageWmsSetEventReportInput *qmi_message_wms_set_event_report_input_ref (QmiMessageWmsSetEventReportInput *self);
void qmi_message_wms_set_event_report_input_unref (QmiMessageWmsSetEventReportInput *self);
QmiMessageWmsSetEventReportInput *qmi_message_wms_set_event_report_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsSetEventReportOutput:
 *
 * The #QmiMessageWmsSetEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSetEventReportOutput QmiMessageWmsSetEventReportOutput;
GType qmi_message_wms_set_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SET_EVENT_REPORT_OUTPUT (qmi_message_wms_set_event_report_output_get_type ())

gboolean qmi_message_wms_set_event_report_output_get_result (
    QmiMessageWmsSetEventReportOutput *self,
    GError **error);

QmiMessageWmsSetEventReportOutput *qmi_message_wms_set_event_report_output_ref (QmiMessageWmsSetEventReportOutput *self);
void qmi_message_wms_set_event_report_output_unref (QmiMessageWmsSetEventReportOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication WMS Event Report */


/* --- Output -- */

/**
 * QmiIndicationWmsEventReportOutput:
 *
 * The #QmiIndicationWmsEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationWmsEventReportOutput QmiIndicationWmsEventReportOutput;
GType qmi_indication_wms_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_WMS_EVENT_REPORT_OUTPUT (qmi_indication_wms_event_report_output_get_type ())

gboolean qmi_indication_wms_event_report_output_get_sms_on_ims (
    QmiIndicationWmsEventReportOutput *self,
    gboolean *sms_on_ims,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_smsc_address (
    QmiIndicationWmsEventReportOutput *self,
    const gchar **smsc_address,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_etws_plmn_information (
    QmiIndicationWmsEventReportOutput *self,
    guint16 *etws_plmn_information_mcc,
    guint16 *etws_plmn_information_mnc,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_etws_message (
    QmiIndicationWmsEventReportOutput *self,
    QmiWmsNotificationType *etws_message_notification_type,
    GArray **etws_message_raw_data,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_message_mode (
    QmiIndicationWmsEventReportOutput *self,
    QmiWmsMessageMode *message_mode,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_transfer_route_mt_message (
    QmiIndicationWmsEventReportOutput *self,
    QmiWmsAckIndicator *transfer_route_mt_message_ack_indicator,
    guint32 *transfer_route_mt_message_transaction_id,
    QmiWmsMessageFormat *transfer_route_mt_message_format,
    GArray **transfer_route_mt_message_raw_data,
    GError **error);

gboolean qmi_indication_wms_event_report_output_get_mt_message (
    QmiIndicationWmsEventReportOutput *self,
    QmiWmsStorageType *mt_message_storage_type,
    guint32 *mt_message_memory_index,
    GError **error);

QmiIndicationWmsEventReportOutput *qmi_indication_wms_event_report_output_ref (QmiIndicationWmsEventReportOutput *self);
void qmi_indication_wms_event_report_output_unref (QmiIndicationWmsEventReportOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Raw Send */


/* --- Input -- */

/**
 * QmiMessageWmsRawSendInput:
 *
 * The #QmiMessageWmsRawSendInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawSendInput QmiMessageWmsRawSendInput;
GType qmi_message_wms_raw_send_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_SEND_INPUT (qmi_message_wms_raw_send_input_get_type ())

gboolean qmi_message_wms_raw_send_input_get_sms_on_ims (
    QmiMessageWmsRawSendInput *self,
    gboolean *sms_on_ims,
    GError **error);

gboolean qmi_message_wms_raw_send_input_set_sms_on_ims (
    QmiMessageWmsRawSendInput *self,
    gboolean sms_on_ims,
    GError **error);

gboolean qmi_message_wms_raw_send_input_get_gsm_wcdma_link_timer (
    QmiMessageWmsRawSendInput *self,
    guint8 *gsm_wcdma_link_timer,
    GError **error);

gboolean qmi_message_wms_raw_send_input_set_gsm_wcdma_link_timer (
    QmiMessageWmsRawSendInput *self,
    guint8 gsm_wcdma_link_timer,
    GError **error);

gboolean qmi_message_wms_raw_send_input_get_cdma_follow_on_dc (
    QmiMessageWmsRawSendInput *self,
    gboolean *cdma_follow_on_dc_follow,
    GError **error);

gboolean qmi_message_wms_raw_send_input_set_cdma_follow_on_dc (
    QmiMessageWmsRawSendInput *self,
    gboolean cdma_follow_on_dc_follow,
    GError **error);

gboolean qmi_message_wms_raw_send_input_get_cdma_force_on_dc (
    QmiMessageWmsRawSendInput *self,
    gboolean *cdma_force_on_dc_force,
    QmiWmsCdmaServiceOption *cdma_force_on_dc_service_option,
    GError **error);

gboolean qmi_message_wms_raw_send_input_set_cdma_force_on_dc (
    QmiMessageWmsRawSendInput *self,
    gboolean cdma_force_on_dc_force,
    QmiWmsCdmaServiceOption cdma_force_on_dc_service_option,
    GError **error);

gboolean qmi_message_wms_raw_send_input_get_raw_message_data (
    QmiMessageWmsRawSendInput *self,
    QmiWmsMessageFormat *raw_message_data_format,
    GArray **raw_message_data_raw_data,
    GError **error);

gboolean qmi_message_wms_raw_send_input_set_raw_message_data (
    QmiMessageWmsRawSendInput *self,
    QmiWmsMessageFormat raw_message_data_format,
    GArray *raw_message_data_raw_data,
    GError **error);

QmiMessageWmsRawSendInput *qmi_message_wms_raw_send_input_ref (QmiMessageWmsRawSendInput *self);
void qmi_message_wms_raw_send_input_unref (QmiMessageWmsRawSendInput *self);
QmiMessageWmsRawSendInput *qmi_message_wms_raw_send_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsRawSendOutput:
 *
 * The #QmiMessageWmsRawSendOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawSendOutput QmiMessageWmsRawSendOutput;
GType qmi_message_wms_raw_send_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_SEND_OUTPUT (qmi_message_wms_raw_send_output_get_type ())

gboolean qmi_message_wms_raw_send_output_get_result (
    QmiMessageWmsRawSendOutput *self,
    GError **error);

gboolean qmi_message_wms_raw_send_output_get_message_id (
    QmiMessageWmsRawSendOutput *self,
    guint16 *message_id,
    GError **error);

gboolean qmi_message_wms_raw_send_output_get_cdma_cause_code (
    QmiMessageWmsRawSendOutput *self,
    QmiWmsCdmaCauseCode *cdma_cause_code,
    GError **error);

gboolean qmi_message_wms_raw_send_output_get_cdma_error_class (
    QmiMessageWmsRawSendOutput *self,
    QmiWmsCdmaErrorClass *cdma_error_class,
    GError **error);

gboolean qmi_message_wms_raw_send_output_get_gsm_wcdma_cause_info (
    QmiMessageWmsRawSendOutput *self,
    QmiWmsGsmUmtsRpCause *gsm_wcdma_cause_info_rp_cause,
    QmiWmsGsmUmtsTpCause *gsm_wcdma_cause_info_tp_cause,
    GError **error);

gboolean qmi_message_wms_raw_send_output_get_message_delivery_failure_type (
    QmiMessageWmsRawSendOutput *self,
    QmiWmsMessageDeliveryFailureType *message_delivery_failure_type,
    GError **error);

QmiMessageWmsRawSendOutput *qmi_message_wms_raw_send_output_ref (QmiMessageWmsRawSendOutput *self);
void qmi_message_wms_raw_send_output_unref (QmiMessageWmsRawSendOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Raw Write */


/* --- Input -- */

/**
 * QmiMessageWmsRawWriteInput:
 *
 * The #QmiMessageWmsRawWriteInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawWriteInput QmiMessageWmsRawWriteInput;
GType qmi_message_wms_raw_write_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_WRITE_INPUT (qmi_message_wms_raw_write_input_get_type ())

gboolean qmi_message_wms_raw_write_input_get_raw_message_data (
    QmiMessageWmsRawWriteInput *self,
    QmiWmsStorageType *raw_message_data_storage_type,
    QmiWmsMessageFormat *raw_message_data_format,
    GArray **raw_message_data_raw_data,
    GError **error);

gboolean qmi_message_wms_raw_write_input_set_raw_message_data (
    QmiMessageWmsRawWriteInput *self,
    QmiWmsStorageType raw_message_data_storage_type,
    QmiWmsMessageFormat raw_message_data_format,
    GArray *raw_message_data_raw_data,
    GError **error);

QmiMessageWmsRawWriteInput *qmi_message_wms_raw_write_input_ref (QmiMessageWmsRawWriteInput *self);
void qmi_message_wms_raw_write_input_unref (QmiMessageWmsRawWriteInput *self);
QmiMessageWmsRawWriteInput *qmi_message_wms_raw_write_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsRawWriteOutput:
 *
 * The #QmiMessageWmsRawWriteOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawWriteOutput QmiMessageWmsRawWriteOutput;
GType qmi_message_wms_raw_write_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_WRITE_OUTPUT (qmi_message_wms_raw_write_output_get_type ())

gboolean qmi_message_wms_raw_write_output_get_result (
    QmiMessageWmsRawWriteOutput *self,
    GError **error);

gboolean qmi_message_wms_raw_write_output_get_memory_index (
    QmiMessageWmsRawWriteOutput *self,
    guint32 *memory_index,
    GError **error);

QmiMessageWmsRawWriteOutput *qmi_message_wms_raw_write_output_ref (QmiMessageWmsRawWriteOutput *self);
void qmi_message_wms_raw_write_output_unref (QmiMessageWmsRawWriteOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Raw Read */


/* --- Input -- */

/**
 * QmiMessageWmsRawReadInput:
 *
 * The #QmiMessageWmsRawReadInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawReadInput QmiMessageWmsRawReadInput;
GType qmi_message_wms_raw_read_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_READ_INPUT (qmi_message_wms_raw_read_input_get_type ())

gboolean qmi_message_wms_raw_read_input_get_sms_on_ims (
    QmiMessageWmsRawReadInput *self,
    gboolean *sms_on_ims,
    GError **error);

gboolean qmi_message_wms_raw_read_input_set_sms_on_ims (
    QmiMessageWmsRawReadInput *self,
    gboolean sms_on_ims,
    GError **error);

gboolean qmi_message_wms_raw_read_input_get_message_mode (
    QmiMessageWmsRawReadInput *self,
    QmiWmsMessageMode *message_mode,
    GError **error);

gboolean qmi_message_wms_raw_read_input_set_message_mode (
    QmiMessageWmsRawReadInput *self,
    QmiWmsMessageMode message_mode,
    GError **error);

gboolean qmi_message_wms_raw_read_input_get_message_memory_storage_id (
    QmiMessageWmsRawReadInput *self,
    QmiWmsStorageType *message_memory_storage_id_storage_type,
    guint32 *message_memory_storage_id_memory_index,
    GError **error);

gboolean qmi_message_wms_raw_read_input_set_message_memory_storage_id (
    QmiMessageWmsRawReadInput *self,
    QmiWmsStorageType message_memory_storage_id_storage_type,
    guint32 message_memory_storage_id_memory_index,
    GError **error);

QmiMessageWmsRawReadInput *qmi_message_wms_raw_read_input_ref (QmiMessageWmsRawReadInput *self);
void qmi_message_wms_raw_read_input_unref (QmiMessageWmsRawReadInput *self);
QmiMessageWmsRawReadInput *qmi_message_wms_raw_read_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsRawReadOutput:
 *
 * The #QmiMessageWmsRawReadOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsRawReadOutput QmiMessageWmsRawReadOutput;
GType qmi_message_wms_raw_read_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_RAW_READ_OUTPUT (qmi_message_wms_raw_read_output_get_type ())

gboolean qmi_message_wms_raw_read_output_get_result (
    QmiMessageWmsRawReadOutput *self,
    GError **error);

gboolean qmi_message_wms_raw_read_output_get_raw_message_data (
    QmiMessageWmsRawReadOutput *self,
    QmiWmsMessageTagType *raw_message_data_message_tag,
    QmiWmsMessageFormat *raw_message_data_format,
    GArray **raw_message_data_raw_data,
    GError **error);

QmiMessageWmsRawReadOutput *qmi_message_wms_raw_read_output_ref (QmiMessageWmsRawReadOutput *self);
void qmi_message_wms_raw_read_output_unref (QmiMessageWmsRawReadOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Modify Tag */


/* --- Input -- */

/**
 * QmiMessageWmsModifyTagInput:
 *
 * The #QmiMessageWmsModifyTagInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsModifyTagInput QmiMessageWmsModifyTagInput;
GType qmi_message_wms_modify_tag_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_MODIFY_TAG_INPUT (qmi_message_wms_modify_tag_input_get_type ())

gboolean qmi_message_wms_modify_tag_input_get_message_mode (
    QmiMessageWmsModifyTagInput *self,
    QmiWmsMessageMode *message_mode,
    GError **error);

gboolean qmi_message_wms_modify_tag_input_set_message_mode (
    QmiMessageWmsModifyTagInput *self,
    QmiWmsMessageMode message_mode,
    GError **error);

gboolean qmi_message_wms_modify_tag_input_get_message_tag (
    QmiMessageWmsModifyTagInput *self,
    QmiWmsStorageType *message_tag_storage_type,
    guint32 *message_tag_memory_index,
    QmiWmsMessageTagType *message_tag_message_tag,
    GError **error);

gboolean qmi_message_wms_modify_tag_input_set_message_tag (
    QmiMessageWmsModifyTagInput *self,
    QmiWmsStorageType message_tag_storage_type,
    guint32 message_tag_memory_index,
    QmiWmsMessageTagType message_tag_message_tag,
    GError **error);

QmiMessageWmsModifyTagInput *qmi_message_wms_modify_tag_input_ref (QmiMessageWmsModifyTagInput *self);
void qmi_message_wms_modify_tag_input_unref (QmiMessageWmsModifyTagInput *self);
QmiMessageWmsModifyTagInput *qmi_message_wms_modify_tag_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsModifyTagOutput:
 *
 * The #QmiMessageWmsModifyTagOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsModifyTagOutput QmiMessageWmsModifyTagOutput;
GType qmi_message_wms_modify_tag_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_MODIFY_TAG_OUTPUT (qmi_message_wms_modify_tag_output_get_type ())

gboolean qmi_message_wms_modify_tag_output_get_result (
    QmiMessageWmsModifyTagOutput *self,
    GError **error);

QmiMessageWmsModifyTagOutput *qmi_message_wms_modify_tag_output_ref (QmiMessageWmsModifyTagOutput *self);
void qmi_message_wms_modify_tag_output_unref (QmiMessageWmsModifyTagOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Delete */


/* --- Input -- */

/**
 * QmiMessageWmsDeleteInput:
 *
 * The #QmiMessageWmsDeleteInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsDeleteInput QmiMessageWmsDeleteInput;
GType qmi_message_wms_delete_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_DELETE_INPUT (qmi_message_wms_delete_input_get_type ())

gboolean qmi_message_wms_delete_input_get_message_mode (
    QmiMessageWmsDeleteInput *self,
    QmiWmsMessageMode *message_mode,
    GError **error);

gboolean qmi_message_wms_delete_input_set_message_mode (
    QmiMessageWmsDeleteInput *self,
    QmiWmsMessageMode message_mode,
    GError **error);

gboolean qmi_message_wms_delete_input_get_message_tag (
    QmiMessageWmsDeleteInput *self,
    QmiWmsMessageTagType *message_tag,
    GError **error);

gboolean qmi_message_wms_delete_input_set_message_tag (
    QmiMessageWmsDeleteInput *self,
    QmiWmsMessageTagType message_tag,
    GError **error);

gboolean qmi_message_wms_delete_input_get_memory_index (
    QmiMessageWmsDeleteInput *self,
    guint32 *memory_index,
    GError **error);

gboolean qmi_message_wms_delete_input_set_memory_index (
    QmiMessageWmsDeleteInput *self,
    guint32 memory_index,
    GError **error);

gboolean qmi_message_wms_delete_input_get_memory_storage (
    QmiMessageWmsDeleteInput *self,
    QmiWmsStorageType *memory_storage,
    GError **error);

gboolean qmi_message_wms_delete_input_set_memory_storage (
    QmiMessageWmsDeleteInput *self,
    QmiWmsStorageType memory_storage,
    GError **error);

QmiMessageWmsDeleteInput *qmi_message_wms_delete_input_ref (QmiMessageWmsDeleteInput *self);
void qmi_message_wms_delete_input_unref (QmiMessageWmsDeleteInput *self);
QmiMessageWmsDeleteInput *qmi_message_wms_delete_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsDeleteOutput:
 *
 * The #QmiMessageWmsDeleteOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsDeleteOutput QmiMessageWmsDeleteOutput;
GType qmi_message_wms_delete_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_DELETE_OUTPUT (qmi_message_wms_delete_output_get_type ())

gboolean qmi_message_wms_delete_output_get_result (
    QmiMessageWmsDeleteOutput *self,
    GError **error);

QmiMessageWmsDeleteOutput *qmi_message_wms_delete_output_ref (QmiMessageWmsDeleteOutput *self);
void qmi_message_wms_delete_output_unref (QmiMessageWmsDeleteOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Get Message Protocol */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWmsGetMessageProtocolOutput:
 *
 * The #QmiMessageWmsGetMessageProtocolOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsGetMessageProtocolOutput QmiMessageWmsGetMessageProtocolOutput;
GType qmi_message_wms_get_message_protocol_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_GET_MESSAGE_PROTOCOL_OUTPUT (qmi_message_wms_get_message_protocol_output_get_type ())

gboolean qmi_message_wms_get_message_protocol_output_get_result (
    QmiMessageWmsGetMessageProtocolOutput *self,
    GError **error);

gboolean qmi_message_wms_get_message_protocol_output_get_message_protocol (
    QmiMessageWmsGetMessageProtocolOutput *self,
    QmiWmsMessageProtocol *message_protocol,
    GError **error);

QmiMessageWmsGetMessageProtocolOutput *qmi_message_wms_get_message_protocol_output_ref (QmiMessageWmsGetMessageProtocolOutput *self);
void qmi_message_wms_get_message_protocol_output_unref (QmiMessageWmsGetMessageProtocolOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS List Messages */


/* --- Input -- */

/**
 * QmiMessageWmsListMessagesInput:
 *
 * The #QmiMessageWmsListMessagesInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsListMessagesInput QmiMessageWmsListMessagesInput;
GType qmi_message_wms_list_messages_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_LIST_MESSAGES_INPUT (qmi_message_wms_list_messages_input_get_type ())

gboolean qmi_message_wms_list_messages_input_get_message_mode (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsMessageMode *message_mode,
    GError **error);

gboolean qmi_message_wms_list_messages_input_set_message_mode (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsMessageMode message_mode,
    GError **error);

gboolean qmi_message_wms_list_messages_input_get_message_tag (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsMessageTagType *message_tag,
    GError **error);

gboolean qmi_message_wms_list_messages_input_set_message_tag (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsMessageTagType message_tag,
    GError **error);

gboolean qmi_message_wms_list_messages_input_get_storage_type (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsStorageType *storage_type,
    GError **error);

gboolean qmi_message_wms_list_messages_input_set_storage_type (
    QmiMessageWmsListMessagesInput *self,
    QmiWmsStorageType storage_type,
    GError **error);

QmiMessageWmsListMessagesInput *qmi_message_wms_list_messages_input_ref (QmiMessageWmsListMessagesInput *self);
void qmi_message_wms_list_messages_input_unref (QmiMessageWmsListMessagesInput *self);
QmiMessageWmsListMessagesInput *qmi_message_wms_list_messages_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsListMessagesOutputMessageListElement:
 * @memory_index: a #guint32.
 * @message_tag: a #QmiWmsMessageTagType.
 *
 * A QmiMessageWmsListMessagesOutputMessageListElement struct.
 */
typedef struct _QmiMessageWmsListMessagesOutputMessageListElement {
    guint32 memory_index;
    QmiWmsMessageTagType message_tag;
} QmiMessageWmsListMessagesOutputMessageListElement;

/**
 * QmiMessageWmsListMessagesOutput:
 *
 * The #QmiMessageWmsListMessagesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsListMessagesOutput QmiMessageWmsListMessagesOutput;
GType qmi_message_wms_list_messages_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_LIST_MESSAGES_OUTPUT (qmi_message_wms_list_messages_output_get_type ())

gboolean qmi_message_wms_list_messages_output_get_result (
    QmiMessageWmsListMessagesOutput *self,
    GError **error);

gboolean qmi_message_wms_list_messages_output_get_message_list (
    QmiMessageWmsListMessagesOutput *self,
    GArray **message_list,
    GError **error);

QmiMessageWmsListMessagesOutput *qmi_message_wms_list_messages_output_ref (QmiMessageWmsListMessagesOutput *self);
void qmi_message_wms_list_messages_output_unref (QmiMessageWmsListMessagesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Set Routes */


/* --- Input -- */

/**
 * QmiMessageWmsSetRoutesInputRouteListElement:
 * @message_type: a #QmiWmsMessageType.
 * @message_class: a #QmiWmsMessageClass.
 * @storage: a #QmiWmsStorageType.
 * @receipt_action: a #QmiWmsReceiptAction.
 *
 * A QmiMessageWmsSetRoutesInputRouteListElement struct.
 */
typedef struct _QmiMessageWmsSetRoutesInputRouteListElement {
    QmiWmsMessageType message_type;
    QmiWmsMessageClass message_class;
    QmiWmsStorageType storage;
    QmiWmsReceiptAction receipt_action;
} QmiMessageWmsSetRoutesInputRouteListElement;

/**
 * QmiMessageWmsSetRoutesInput:
 *
 * The #QmiMessageWmsSetRoutesInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSetRoutesInput QmiMessageWmsSetRoutesInput;
GType qmi_message_wms_set_routes_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SET_ROUTES_INPUT (qmi_message_wms_set_routes_input_get_type ())

gboolean qmi_message_wms_set_routes_input_get_transfer_status_report (
    QmiMessageWmsSetRoutesInput *self,
    QmiWmsTransferIndication *transfer_status_report,
    GError **error);

gboolean qmi_message_wms_set_routes_input_set_transfer_status_report (
    QmiMessageWmsSetRoutesInput *self,
    QmiWmsTransferIndication transfer_status_report,
    GError **error);

gboolean qmi_message_wms_set_routes_input_get_route_list (
    QmiMessageWmsSetRoutesInput *self,
    GArray **route_list,
    GError **error);

gboolean qmi_message_wms_set_routes_input_set_route_list (
    QmiMessageWmsSetRoutesInput *self,
    GArray *route_list,
    GError **error);

QmiMessageWmsSetRoutesInput *qmi_message_wms_set_routes_input_ref (QmiMessageWmsSetRoutesInput *self);
void qmi_message_wms_set_routes_input_unref (QmiMessageWmsSetRoutesInput *self);
QmiMessageWmsSetRoutesInput *qmi_message_wms_set_routes_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsSetRoutesOutput:
 *
 * The #QmiMessageWmsSetRoutesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSetRoutesOutput QmiMessageWmsSetRoutesOutput;
GType qmi_message_wms_set_routes_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SET_ROUTES_OUTPUT (qmi_message_wms_set_routes_output_get_type ())

gboolean qmi_message_wms_set_routes_output_get_result (
    QmiMessageWmsSetRoutesOutput *self,
    GError **error);

QmiMessageWmsSetRoutesOutput *qmi_message_wms_set_routes_output_ref (QmiMessageWmsSetRoutesOutput *self);
void qmi_message_wms_set_routes_output_unref (QmiMessageWmsSetRoutesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Get Routes */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageWmsGetRoutesOutputRouteListElement:
 * @message_type: a #QmiWmsMessageType.
 * @message_class: a #QmiWmsMessageClass.
 * @storage: a #QmiWmsStorageType.
 * @receipt_action: a #QmiWmsReceiptAction.
 *
 * A QmiMessageWmsGetRoutesOutputRouteListElement struct.
 */
typedef struct _QmiMessageWmsGetRoutesOutputRouteListElement {
    QmiWmsMessageType message_type;
    QmiWmsMessageClass message_class;
    QmiWmsStorageType storage;
    QmiWmsReceiptAction receipt_action;
} QmiMessageWmsGetRoutesOutputRouteListElement;

/**
 * QmiMessageWmsGetRoutesOutput:
 *
 * The #QmiMessageWmsGetRoutesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsGetRoutesOutput QmiMessageWmsGetRoutesOutput;
GType qmi_message_wms_get_routes_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_GET_ROUTES_OUTPUT (qmi_message_wms_get_routes_output_get_type ())

gboolean qmi_message_wms_get_routes_output_get_result (
    QmiMessageWmsGetRoutesOutput *self,
    GError **error);

gboolean qmi_message_wms_get_routes_output_get_route_list (
    QmiMessageWmsGetRoutesOutput *self,
    GArray **route_list,
    GError **error);

gboolean qmi_message_wms_get_routes_output_get_transfer_status_report (
    QmiMessageWmsGetRoutesOutput *self,
    QmiWmsTransferIndication *transfer_status_report,
    GError **error);

QmiMessageWmsGetRoutesOutput *qmi_message_wms_get_routes_output_ref (QmiMessageWmsGetRoutesOutput *self);
void qmi_message_wms_get_routes_output_unref (QmiMessageWmsGetRoutesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message WMS Send From Memory Storage */


/* --- Input -- */

/**
 * QmiMessageWmsSendFromMemoryStorageInput:
 *
 * The #QmiMessageWmsSendFromMemoryStorageInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSendFromMemoryStorageInput QmiMessageWmsSendFromMemoryStorageInput;
GType qmi_message_wms_send_from_memory_storage_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SEND_FROM_MEMORY_STORAGE_INPUT (qmi_message_wms_send_from_memory_storage_input_get_type ())

gboolean qmi_message_wms_send_from_memory_storage_input_get_sms_on_ims (
    QmiMessageWmsSendFromMemoryStorageInput *self,
    gboolean *sms_on_ims,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_input_set_sms_on_ims (
    QmiMessageWmsSendFromMemoryStorageInput *self,
    gboolean sms_on_ims,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_input_get_information (
    QmiMessageWmsSendFromMemoryStorageInput *self,
    QmiWmsStorageType *information_storage_type,
    guint32 *information_memory_index,
    QmiWmsMessageMode *information_message_mode,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_input_set_information (
    QmiMessageWmsSendFromMemoryStorageInput *self,
    QmiWmsStorageType information_storage_type,
    guint32 information_memory_index,
    QmiWmsMessageMode information_message_mode,
    GError **error);

QmiMessageWmsSendFromMemoryStorageInput *qmi_message_wms_send_from_memory_storage_input_ref (QmiMessageWmsSendFromMemoryStorageInput *self);
void qmi_message_wms_send_from_memory_storage_input_unref (QmiMessageWmsSendFromMemoryStorageInput *self);
QmiMessageWmsSendFromMemoryStorageInput *qmi_message_wms_send_from_memory_storage_input_new (void);

/* --- Output -- */

/**
 * QmiMessageWmsSendFromMemoryStorageOutput:
 *
 * The #QmiMessageWmsSendFromMemoryStorageOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageWmsSendFromMemoryStorageOutput QmiMessageWmsSendFromMemoryStorageOutput;
GType qmi_message_wms_send_from_memory_storage_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_WMS_SEND_FROM_MEMORY_STORAGE_OUTPUT (qmi_message_wms_send_from_memory_storage_output_get_type ())

gboolean qmi_message_wms_send_from_memory_storage_output_get_result (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_output_get_message_id (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    guint16 *message_id,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_output_get_cdma_cause_code (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    QmiWmsCdmaCauseCode *cdma_cause_code,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_output_get_cdma_error_class (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    QmiWmsCdmaErrorClass *cdma_error_class,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_output_get_gsm_wcdma_cause_info (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    QmiWmsGsmUmtsRpCause *gsm_wcdma_cause_info_rp_cause,
    QmiWmsGsmUmtsTpCause *gsm_wcdma_cause_info_tp_cause,
    GError **error);

gboolean qmi_message_wms_send_from_memory_storage_output_get_message_delivery_failure_type (
    QmiMessageWmsSendFromMemoryStorageOutput *self,
    QmiWmsMessageDeliveryFailureType *message_delivery_failure_type,
    GError **error);

QmiMessageWmsSendFromMemoryStorageOutput *qmi_message_wms_send_from_memory_storage_output_ref (QmiMessageWmsSendFromMemoryStorageOutput *self);
void qmi_message_wms_send_from_memory_storage_output_unref (QmiMessageWmsSendFromMemoryStorageOutput *self);

/*****************************************************************************/
/* Service-specific printable: WMS */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_wms_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_wms_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client WMS */

#define QMI_TYPE_CLIENT_WMS            (qmi_client_wms_get_type ())
#define QMI_CLIENT_WMS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_WMS, QmiClientWms))
#define QMI_CLIENT_WMS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_WMS, QmiClientWmsClass))
#define QMI_IS_CLIENT_WMS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_WMS))
#define QMI_IS_CLIENT_WMS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_WMS))
#define QMI_CLIENT_WMS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_WMS, QmiClientWmsClass))

typedef struct _QmiClientWms QmiClientWms;
typedef struct _QmiClientWmsClass QmiClientWmsClass;

/**
 * QmiClientWms:
 *
 * The #QmiClientWms structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientWms {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientWmsClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_wms_get_type (void);


void qmi_client_wms_reset (
    QmiClientWms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsResetOutput *qmi_client_wms_reset_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_set_event_report (
    QmiClientWms *self,
    QmiMessageWmsSetEventReportInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsSetEventReportOutput *qmi_client_wms_set_event_report_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_raw_send (
    QmiClientWms *self,
    QmiMessageWmsRawSendInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsRawSendOutput *qmi_client_wms_raw_send_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_raw_write (
    QmiClientWms *self,
    QmiMessageWmsRawWriteInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsRawWriteOutput *qmi_client_wms_raw_write_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_raw_read (
    QmiClientWms *self,
    QmiMessageWmsRawReadInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsRawReadOutput *qmi_client_wms_raw_read_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_modify_tag (
    QmiClientWms *self,
    QmiMessageWmsModifyTagInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsModifyTagOutput *qmi_client_wms_modify_tag_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_delete (
    QmiClientWms *self,
    QmiMessageWmsDeleteInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsDeleteOutput *qmi_client_wms_delete_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_get_message_protocol (
    QmiClientWms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsGetMessageProtocolOutput *qmi_client_wms_get_message_protocol_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_list_messages (
    QmiClientWms *self,
    QmiMessageWmsListMessagesInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsListMessagesOutput *qmi_client_wms_list_messages_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_set_routes (
    QmiClientWms *self,
    QmiMessageWmsSetRoutesInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsSetRoutesOutput *qmi_client_wms_set_routes_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_get_routes (
    QmiClientWms *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsGetRoutesOutput *qmi_client_wms_get_routes_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_wms_send_from_memory_storage (
    QmiClientWms *self,
    QmiMessageWmsSendFromMemoryStorageInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageWmsSendFromMemoryStorageOutput *qmi_client_wms_send_from_memory_storage_finish (
    QmiClientWms *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_WMS__ */
