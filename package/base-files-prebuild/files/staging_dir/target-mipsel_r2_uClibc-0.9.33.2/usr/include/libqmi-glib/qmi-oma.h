
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
#include "qmi-enums-oma.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_OMA__
#define __LIBQMI_GLIB_QMI_OMA__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageOmaResetOutput:
 *
 * The #QmiMessageOmaResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaResetOutput QmiMessageOmaResetOutput;
GType qmi_message_oma_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_RESET_OUTPUT (qmi_message_oma_reset_output_get_type ())

gboolean qmi_message_oma_reset_output_get_result (
    QmiMessageOmaResetOutput *self,
    GError **error);

QmiMessageOmaResetOutput *qmi_message_oma_reset_output_ref (QmiMessageOmaResetOutput *self);
void qmi_message_oma_reset_output_unref (QmiMessageOmaResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Set Event Report */


/* --- Input -- */

/**
 * QmiMessageOmaSetEventReportInput:
 *
 * The #QmiMessageOmaSetEventReportInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSetEventReportInput QmiMessageOmaSetEventReportInput;
GType qmi_message_oma_set_event_report_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SET_EVENT_REPORT_INPUT (qmi_message_oma_set_event_report_input_get_type ())

gboolean qmi_message_oma_set_event_report_input_get_session_state_reporting (
    QmiMessageOmaSetEventReportInput *self,
    gboolean *session_state_reporting,
    GError **error);

gboolean qmi_message_oma_set_event_report_input_set_session_state_reporting (
    QmiMessageOmaSetEventReportInput *self,
    gboolean session_state_reporting,
    GError **error);

gboolean qmi_message_oma_set_event_report_input_get_network_initiated_alert_reporting (
    QmiMessageOmaSetEventReportInput *self,
    gboolean *network_initiated_alert_reporting,
    GError **error);

gboolean qmi_message_oma_set_event_report_input_set_network_initiated_alert_reporting (
    QmiMessageOmaSetEventReportInput *self,
    gboolean network_initiated_alert_reporting,
    GError **error);

QmiMessageOmaSetEventReportInput *qmi_message_oma_set_event_report_input_ref (QmiMessageOmaSetEventReportInput *self);
void qmi_message_oma_set_event_report_input_unref (QmiMessageOmaSetEventReportInput *self);
QmiMessageOmaSetEventReportInput *qmi_message_oma_set_event_report_input_new (void);

/* --- Output -- */

/**
 * QmiMessageOmaSetEventReportOutput:
 *
 * The #QmiMessageOmaSetEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSetEventReportOutput QmiMessageOmaSetEventReportOutput;
GType qmi_message_oma_set_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SET_EVENT_REPORT_OUTPUT (qmi_message_oma_set_event_report_output_get_type ())

gboolean qmi_message_oma_set_event_report_output_get_result (
    QmiMessageOmaSetEventReportOutput *self,
    GError **error);

QmiMessageOmaSetEventReportOutput *qmi_message_oma_set_event_report_output_ref (QmiMessageOmaSetEventReportOutput *self);
void qmi_message_oma_set_event_report_output_unref (QmiMessageOmaSetEventReportOutput *self);

/*****************************************************************************/
/* INDICATION: Qmi Indication OMA Event Report */


/* --- Output -- */

/**
 * QmiIndicationOmaEventReportOutput:
 *
 * The #QmiIndicationOmaEventReportOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiIndicationOmaEventReportOutput QmiIndicationOmaEventReportOutput;
GType qmi_indication_oma_event_report_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_INDICATION_OMA_EVENT_REPORT_OUTPUT (qmi_indication_oma_event_report_output_get_type ())

gboolean qmi_indication_oma_event_report_output_get_session_fail_reason (
    QmiIndicationOmaEventReportOutput *self,
    QmiOmaSessionFailedReason *session_fail_reason,
    GError **error);

gboolean qmi_indication_oma_event_report_output_get_session_state (
    QmiIndicationOmaEventReportOutput *self,
    QmiOmaSessionState *session_state,
    GError **error);

gboolean qmi_indication_oma_event_report_output_get_network_initiated_alert (
    QmiIndicationOmaEventReportOutput *self,
    QmiOmaSessionType *network_initiated_alert_session_type,
    guint16 *network_initiated_alert_session_id,
    GError **error);

QmiIndicationOmaEventReportOutput *qmi_indication_oma_event_report_output_ref (QmiIndicationOmaEventReportOutput *self);
void qmi_indication_oma_event_report_output_unref (QmiIndicationOmaEventReportOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Start Session */


/* --- Input -- */

/**
 * QmiMessageOmaStartSessionInput:
 *
 * The #QmiMessageOmaStartSessionInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaStartSessionInput QmiMessageOmaStartSessionInput;
GType qmi_message_oma_start_session_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_START_SESSION_INPUT (qmi_message_oma_start_session_input_get_type ())

gboolean qmi_message_oma_start_session_input_get_session_type (
    QmiMessageOmaStartSessionInput *self,
    QmiOmaSessionType *session_type,
    GError **error);

gboolean qmi_message_oma_start_session_input_set_session_type (
    QmiMessageOmaStartSessionInput *self,
    QmiOmaSessionType session_type,
    GError **error);

QmiMessageOmaStartSessionInput *qmi_message_oma_start_session_input_ref (QmiMessageOmaStartSessionInput *self);
void qmi_message_oma_start_session_input_unref (QmiMessageOmaStartSessionInput *self);
QmiMessageOmaStartSessionInput *qmi_message_oma_start_session_input_new (void);

/* --- Output -- */

/**
 * QmiMessageOmaStartSessionOutput:
 *
 * The #QmiMessageOmaStartSessionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaStartSessionOutput QmiMessageOmaStartSessionOutput;
GType qmi_message_oma_start_session_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_START_SESSION_OUTPUT (qmi_message_oma_start_session_output_get_type ())

gboolean qmi_message_oma_start_session_output_get_result (
    QmiMessageOmaStartSessionOutput *self,
    GError **error);

QmiMessageOmaStartSessionOutput *qmi_message_oma_start_session_output_ref (QmiMessageOmaStartSessionOutput *self);
void qmi_message_oma_start_session_output_unref (QmiMessageOmaStartSessionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Cancel Session */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageOmaCancelSessionOutput:
 *
 * The #QmiMessageOmaCancelSessionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaCancelSessionOutput QmiMessageOmaCancelSessionOutput;
GType qmi_message_oma_cancel_session_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_CANCEL_SESSION_OUTPUT (qmi_message_oma_cancel_session_output_get_type ())

gboolean qmi_message_oma_cancel_session_output_get_result (
    QmiMessageOmaCancelSessionOutput *self,
    GError **error);

QmiMessageOmaCancelSessionOutput *qmi_message_oma_cancel_session_output_ref (QmiMessageOmaCancelSessionOutput *self);
void qmi_message_oma_cancel_session_output_unref (QmiMessageOmaCancelSessionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Get Session Info */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageOmaGetSessionInfoOutput:
 *
 * The #QmiMessageOmaGetSessionInfoOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaGetSessionInfoOutput QmiMessageOmaGetSessionInfoOutput;
GType qmi_message_oma_get_session_info_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_GET_SESSION_INFO_OUTPUT (qmi_message_oma_get_session_info_output_get_type ())

gboolean qmi_message_oma_get_session_info_output_get_result (
    QmiMessageOmaGetSessionInfoOutput *self,
    GError **error);

gboolean qmi_message_oma_get_session_info_output_get_session_info (
    QmiMessageOmaGetSessionInfoOutput *self,
    QmiOmaSessionState *session_info_session_state,
    QmiOmaSessionType *session_info_session_type,
    GError **error);

gboolean qmi_message_oma_get_session_info_output_get_session_failed_reason (
    QmiMessageOmaGetSessionInfoOutput *self,
    QmiOmaSessionFailedReason *session_failed_reason,
    GError **error);

gboolean qmi_message_oma_get_session_info_output_get_retry_info (
    QmiMessageOmaGetSessionInfoOutput *self,
    guint8 *retry_info_retry_count,
    guint16 *retry_info_retry_pause_timer,
    guint16 *retry_info_retry_pause_timer_remaining,
    GError **error);

gboolean qmi_message_oma_get_session_info_output_get_network_initiated_alert (
    QmiMessageOmaGetSessionInfoOutput *self,
    QmiOmaSessionType *network_initiated_alert_session_type,
    guint16 *network_initiated_alert_session_id,
    GError **error);

QmiMessageOmaGetSessionInfoOutput *qmi_message_oma_get_session_info_output_ref (QmiMessageOmaGetSessionInfoOutput *self);
void qmi_message_oma_get_session_info_output_unref (QmiMessageOmaGetSessionInfoOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Send Selection */


/* --- Input -- */

/**
 * QmiMessageOmaSendSelectionInput:
 *
 * The #QmiMessageOmaSendSelectionInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSendSelectionInput QmiMessageOmaSendSelectionInput;
GType qmi_message_oma_send_selection_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SEND_SELECTION_INPUT (qmi_message_oma_send_selection_input_get_type ())

gboolean qmi_message_oma_send_selection_input_get_network_initiated_alert_selection (
    QmiMessageOmaSendSelectionInput *self,
    gboolean *network_initiated_alert_selection_control_point_selection_accept,
    guint16 *network_initiated_alert_selection_session_id,
    GError **error);

gboolean qmi_message_oma_send_selection_input_set_network_initiated_alert_selection (
    QmiMessageOmaSendSelectionInput *self,
    gboolean network_initiated_alert_selection_control_point_selection_accept,
    guint16 network_initiated_alert_selection_session_id,
    GError **error);

QmiMessageOmaSendSelectionInput *qmi_message_oma_send_selection_input_ref (QmiMessageOmaSendSelectionInput *self);
void qmi_message_oma_send_selection_input_unref (QmiMessageOmaSendSelectionInput *self);
QmiMessageOmaSendSelectionInput *qmi_message_oma_send_selection_input_new (void);

/* --- Output -- */

/**
 * QmiMessageOmaSendSelectionOutput:
 *
 * The #QmiMessageOmaSendSelectionOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSendSelectionOutput QmiMessageOmaSendSelectionOutput;
GType qmi_message_oma_send_selection_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SEND_SELECTION_OUTPUT (qmi_message_oma_send_selection_output_get_type ())

gboolean qmi_message_oma_send_selection_output_get_result (
    QmiMessageOmaSendSelectionOutput *self,
    GError **error);

QmiMessageOmaSendSelectionOutput *qmi_message_oma_send_selection_output_ref (QmiMessageOmaSendSelectionOutput *self);
void qmi_message_oma_send_selection_output_unref (QmiMessageOmaSendSelectionOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Get Feature Setting */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageOmaGetFeatureSettingOutput:
 *
 * The #QmiMessageOmaGetFeatureSettingOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaGetFeatureSettingOutput QmiMessageOmaGetFeatureSettingOutput;
GType qmi_message_oma_get_feature_setting_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_GET_FEATURE_SETTING_OUTPUT (qmi_message_oma_get_feature_setting_output_get_type ())

gboolean qmi_message_oma_get_feature_setting_output_get_result (
    QmiMessageOmaGetFeatureSettingOutput *self,
    GError **error);

gboolean qmi_message_oma_get_feature_setting_output_get_device_provisioning_service_update_config (
    QmiMessageOmaGetFeatureSettingOutput *self,
    gboolean *device_provisioning_service_update_config,
    GError **error);

gboolean qmi_message_oma_get_feature_setting_output_get_prl_update_service_config (
    QmiMessageOmaGetFeatureSettingOutput *self,
    gboolean *prl_update_service_config,
    GError **error);

gboolean qmi_message_oma_get_feature_setting_output_get_hfa_feature_config (
    QmiMessageOmaGetFeatureSettingOutput *self,
    gboolean *hfa_feature_config,
    GError **error);

gboolean qmi_message_oma_get_feature_setting_output_get_hfa_feature_done_state (
    QmiMessageOmaGetFeatureSettingOutput *self,
    QmiOmaHfaFeatureDoneState *hfa_feature_done_state,
    GError **error);

QmiMessageOmaGetFeatureSettingOutput *qmi_message_oma_get_feature_setting_output_ref (QmiMessageOmaGetFeatureSettingOutput *self);
void qmi_message_oma_get_feature_setting_output_unref (QmiMessageOmaGetFeatureSettingOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message OMA Set Feature Setting */


/* --- Input -- */

/**
 * QmiMessageOmaSetFeatureSettingInput:
 *
 * The #QmiMessageOmaSetFeatureSettingInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSetFeatureSettingInput QmiMessageOmaSetFeatureSettingInput;
GType qmi_message_oma_set_feature_setting_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SET_FEATURE_SETTING_INPUT (qmi_message_oma_set_feature_setting_input_get_type ())

gboolean qmi_message_oma_set_feature_setting_input_get_hfa_feature_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean *hfa_feature_config,
    GError **error);

gboolean qmi_message_oma_set_feature_setting_input_set_hfa_feature_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean hfa_feature_config,
    GError **error);

gboolean qmi_message_oma_set_feature_setting_input_get_prl_update_service_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean *prl_update_service_config,
    GError **error);

gboolean qmi_message_oma_set_feature_setting_input_set_prl_update_service_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean prl_update_service_config,
    GError **error);

gboolean qmi_message_oma_set_feature_setting_input_get_device_provisioning_service_update_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean *device_provisioning_service_update_config,
    GError **error);

gboolean qmi_message_oma_set_feature_setting_input_set_device_provisioning_service_update_config (
    QmiMessageOmaSetFeatureSettingInput *self,
    gboolean device_provisioning_service_update_config,
    GError **error);

QmiMessageOmaSetFeatureSettingInput *qmi_message_oma_set_feature_setting_input_ref (QmiMessageOmaSetFeatureSettingInput *self);
void qmi_message_oma_set_feature_setting_input_unref (QmiMessageOmaSetFeatureSettingInput *self);
QmiMessageOmaSetFeatureSettingInput *qmi_message_oma_set_feature_setting_input_new (void);

/* --- Output -- */

/**
 * QmiMessageOmaSetFeatureSettingOutput:
 *
 * The #QmiMessageOmaSetFeatureSettingOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageOmaSetFeatureSettingOutput QmiMessageOmaSetFeatureSettingOutput;
GType qmi_message_oma_set_feature_setting_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_OMA_SET_FEATURE_SETTING_OUTPUT (qmi_message_oma_set_feature_setting_output_get_type ())

gboolean qmi_message_oma_set_feature_setting_output_get_result (
    QmiMessageOmaSetFeatureSettingOutput *self,
    GError **error);

QmiMessageOmaSetFeatureSettingOutput *qmi_message_oma_set_feature_setting_output_ref (QmiMessageOmaSetFeatureSettingOutput *self);
void qmi_message_oma_set_feature_setting_output_unref (QmiMessageOmaSetFeatureSettingOutput *self);

/*****************************************************************************/
/* Service-specific printable: OMA */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_oma_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_oma_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client OMA */

#define QMI_TYPE_CLIENT_OMA            (qmi_client_oma_get_type ())
#define QMI_CLIENT_OMA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_OMA, QmiClientOma))
#define QMI_CLIENT_OMA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_OMA, QmiClientOmaClass))
#define QMI_IS_CLIENT_OMA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_OMA))
#define QMI_IS_CLIENT_OMA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_OMA))
#define QMI_CLIENT_OMA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_OMA, QmiClientOmaClass))

typedef struct _QmiClientOma QmiClientOma;
typedef struct _QmiClientOmaClass QmiClientOmaClass;

/**
 * QmiClientOma:
 *
 * The #QmiClientOma structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientOma {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientOmaClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_oma_get_type (void);


void qmi_client_oma_reset (
    QmiClientOma *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaResetOutput *qmi_client_oma_reset_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_set_event_report (
    QmiClientOma *self,
    QmiMessageOmaSetEventReportInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaSetEventReportOutput *qmi_client_oma_set_event_report_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_start_session (
    QmiClientOma *self,
    QmiMessageOmaStartSessionInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaStartSessionOutput *qmi_client_oma_start_session_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_cancel_session (
    QmiClientOma *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaCancelSessionOutput *qmi_client_oma_cancel_session_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_get_session_info (
    QmiClientOma *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaGetSessionInfoOutput *qmi_client_oma_get_session_info_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_send_selection (
    QmiClientOma *self,
    QmiMessageOmaSendSelectionInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaSendSelectionOutput *qmi_client_oma_send_selection_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_get_feature_setting (
    QmiClientOma *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaGetFeatureSettingOutput *qmi_client_oma_get_feature_setting_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_oma_set_feature_setting (
    QmiClientOma *self,
    QmiMessageOmaSetFeatureSettingInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageOmaSetFeatureSettingOutput *qmi_client_oma_set_feature_setting_finish (
    QmiClientOma *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_OMA__ */
