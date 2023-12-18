
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
#include "qmi-enums-uim.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_UIM__
#define __LIBQMI_GLIB_QMI_UIM__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message UIM Reset */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessageUimResetOutput:
 *
 * The #QmiMessageUimResetOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimResetOutput QmiMessageUimResetOutput;
GType qmi_message_uim_reset_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_RESET_OUTPUT (qmi_message_uim_reset_output_get_type ())

gboolean qmi_message_uim_reset_output_get_result (
    QmiMessageUimResetOutput *self,
    GError **error);

QmiMessageUimResetOutput *qmi_message_uim_reset_output_ref (QmiMessageUimResetOutput *self);
void qmi_message_uim_reset_output_unref (QmiMessageUimResetOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message UIM Read Transparent */


/* --- Input -- */

/**
 * QmiMessageUimReadTransparentInput:
 *
 * The #QmiMessageUimReadTransparentInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimReadTransparentInput QmiMessageUimReadTransparentInput;
GType qmi_message_uim_read_transparent_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_READ_TRANSPARENT_INPUT (qmi_message_uim_read_transparent_input_get_type ())

gboolean qmi_message_uim_read_transparent_input_get_encrypt_data (
    QmiMessageUimReadTransparentInput *self,
    gboolean *encrypt_data,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_set_encrypt_data (
    QmiMessageUimReadTransparentInput *self,
    gboolean encrypt_data,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_get_response_in_indication_token (
    QmiMessageUimReadTransparentInput *self,
    guint32 *response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_set_response_in_indication_token (
    QmiMessageUimReadTransparentInput *self,
    guint32 response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_get_read_information (
    QmiMessageUimReadTransparentInput *self,
    guint16 *read_information_offset,
    guint16 *read_information_length,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_set_read_information (
    QmiMessageUimReadTransparentInput *self,
    guint16 read_information_offset,
    guint16 read_information_length,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_get_file (
    QmiMessageUimReadTransparentInput *self,
    guint16 *file_file_id,
    GArray **file_file_path,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_set_file (
    QmiMessageUimReadTransparentInput *self,
    guint16 file_file_id,
    GArray *file_file_path,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_get_session_information (
    QmiMessageUimReadTransparentInput *self,
    QmiUimSessionType *session_information_session_type,
    const gchar **session_information_application_identifier,
    GError **error);

gboolean qmi_message_uim_read_transparent_input_set_session_information (
    QmiMessageUimReadTransparentInput *self,
    QmiUimSessionType session_information_session_type,
    const gchar *session_information_application_identifier,
    GError **error);

QmiMessageUimReadTransparentInput *qmi_message_uim_read_transparent_input_ref (QmiMessageUimReadTransparentInput *self);
void qmi_message_uim_read_transparent_input_unref (QmiMessageUimReadTransparentInput *self);
QmiMessageUimReadTransparentInput *qmi_message_uim_read_transparent_input_new (void);

/* --- Output -- */

/**
 * QmiMessageUimReadTransparentOutput:
 *
 * The #QmiMessageUimReadTransparentOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimReadTransparentOutput QmiMessageUimReadTransparentOutput;
GType qmi_message_uim_read_transparent_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_READ_TRANSPARENT_OUTPUT (qmi_message_uim_read_transparent_output_get_type ())

gboolean qmi_message_uim_read_transparent_output_get_result (
    QmiMessageUimReadTransparentOutput *self,
    GError **error);

gboolean qmi_message_uim_read_transparent_output_get_card_result (
    QmiMessageUimReadTransparentOutput *self,
    guint8 *card_result_sw1,
    guint8 *card_result_sw2,
    GError **error);

gboolean qmi_message_uim_read_transparent_output_get_read_result (
    QmiMessageUimReadTransparentOutput *self,
    GArray **read_result,
    GError **error);

gboolean qmi_message_uim_read_transparent_output_get_response_in_indication_token (
    QmiMessageUimReadTransparentOutput *self,
    guint32 *response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_read_transparent_output_get_encrypted_data (
    QmiMessageUimReadTransparentOutput *self,
    gboolean *encrypted_data,
    GError **error);

QmiMessageUimReadTransparentOutput *qmi_message_uim_read_transparent_output_ref (QmiMessageUimReadTransparentOutput *self);
void qmi_message_uim_read_transparent_output_unref (QmiMessageUimReadTransparentOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message UIM Read Record */


/* --- Input -- */

/**
 * QmiMessageUimReadRecordInput:
 *
 * The #QmiMessageUimReadRecordInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimReadRecordInput QmiMessageUimReadRecordInput;
GType qmi_message_uim_read_record_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_READ_RECORD_INPUT (qmi_message_uim_read_record_input_get_type ())

gboolean qmi_message_uim_read_record_input_get_response_in_indication_token (
    QmiMessageUimReadRecordInput *self,
    guint32 *response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_read_record_input_set_response_in_indication_token (
    QmiMessageUimReadRecordInput *self,
    guint32 response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_read_record_input_get_last_record (
    QmiMessageUimReadRecordInput *self,
    guint16 *last_record,
    GError **error);

gboolean qmi_message_uim_read_record_input_set_last_record (
    QmiMessageUimReadRecordInput *self,
    guint16 last_record,
    GError **error);

gboolean qmi_message_uim_read_record_input_get_record (
    QmiMessageUimReadRecordInput *self,
    guint16 *record_record_number,
    guint16 *record_record_length,
    GError **error);

gboolean qmi_message_uim_read_record_input_set_record (
    QmiMessageUimReadRecordInput *self,
    guint16 record_record_number,
    guint16 record_record_length,
    GError **error);

gboolean qmi_message_uim_read_record_input_get_file (
    QmiMessageUimReadRecordInput *self,
    guint16 *file_file_id,
    GArray **file_file_path,
    GError **error);

gboolean qmi_message_uim_read_record_input_set_file (
    QmiMessageUimReadRecordInput *self,
    guint16 file_file_id,
    GArray *file_file_path,
    GError **error);

gboolean qmi_message_uim_read_record_input_get_session_information (
    QmiMessageUimReadRecordInput *self,
    QmiUimSessionType *session_information_session_type,
    const gchar **session_information_application_identifier,
    GError **error);

gboolean qmi_message_uim_read_record_input_set_session_information (
    QmiMessageUimReadRecordInput *self,
    QmiUimSessionType session_information_session_type,
    const gchar *session_information_application_identifier,
    GError **error);

QmiMessageUimReadRecordInput *qmi_message_uim_read_record_input_ref (QmiMessageUimReadRecordInput *self);
void qmi_message_uim_read_record_input_unref (QmiMessageUimReadRecordInput *self);
QmiMessageUimReadRecordInput *qmi_message_uim_read_record_input_new (void);

/* --- Output -- */

/**
 * QmiMessageUimReadRecordOutput:
 *
 * The #QmiMessageUimReadRecordOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimReadRecordOutput QmiMessageUimReadRecordOutput;
GType qmi_message_uim_read_record_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_READ_RECORD_OUTPUT (qmi_message_uim_read_record_output_get_type ())

gboolean qmi_message_uim_read_record_output_get_result (
    QmiMessageUimReadRecordOutput *self,
    GError **error);

gboolean qmi_message_uim_read_record_output_get_card_result (
    QmiMessageUimReadRecordOutput *self,
    guint8 *card_result_sw1,
    guint8 *card_result_sw2,
    GError **error);

gboolean qmi_message_uim_read_record_output_get_read_result (
    QmiMessageUimReadRecordOutput *self,
    GArray **read_result,
    GError **error);

gboolean qmi_message_uim_read_record_output_get_additional_read_result (
    QmiMessageUimReadRecordOutput *self,
    GArray **additional_read_result,
    GError **error);

gboolean qmi_message_uim_read_record_output_get_response_in_indication_token (
    QmiMessageUimReadRecordOutput *self,
    guint32 *response_in_indication_token,
    GError **error);

QmiMessageUimReadRecordOutput *qmi_message_uim_read_record_output_ref (QmiMessageUimReadRecordOutput *self);
void qmi_message_uim_read_record_output_unref (QmiMessageUimReadRecordOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message UIM Get File Attributes */


/* --- Input -- */

/**
 * QmiMessageUimGetFileAttributesInput:
 *
 * The #QmiMessageUimGetFileAttributesInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimGetFileAttributesInput QmiMessageUimGetFileAttributesInput;
GType qmi_message_uim_get_file_attributes_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_GET_FILE_ATTRIBUTES_INPUT (qmi_message_uim_get_file_attributes_input_get_type ())

gboolean qmi_message_uim_get_file_attributes_input_get_response_in_indication_token (
    QmiMessageUimGetFileAttributesInput *self,
    guint32 *response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_input_set_response_in_indication_token (
    QmiMessageUimGetFileAttributesInput *self,
    guint32 response_in_indication_token,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_input_get_file (
    QmiMessageUimGetFileAttributesInput *self,
    guint16 *file_file_id,
    GArray **file_file_path,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_input_set_file (
    QmiMessageUimGetFileAttributesInput *self,
    guint16 file_file_id,
    GArray *file_file_path,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_input_get_session_information (
    QmiMessageUimGetFileAttributesInput *self,
    QmiUimSessionType *session_information_session_type,
    const gchar **session_information_application_identifier,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_input_set_session_information (
    QmiMessageUimGetFileAttributesInput *self,
    QmiUimSessionType session_information_session_type,
    const gchar *session_information_application_identifier,
    GError **error);

QmiMessageUimGetFileAttributesInput *qmi_message_uim_get_file_attributes_input_ref (QmiMessageUimGetFileAttributesInput *self);
void qmi_message_uim_get_file_attributes_input_unref (QmiMessageUimGetFileAttributesInput *self);
QmiMessageUimGetFileAttributesInput *qmi_message_uim_get_file_attributes_input_new (void);

/* --- Output -- */

/**
 * QmiMessageUimGetFileAttributesOutput:
 *
 * The #QmiMessageUimGetFileAttributesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessageUimGetFileAttributesOutput QmiMessageUimGetFileAttributesOutput;
GType qmi_message_uim_get_file_attributes_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_UIM_GET_FILE_ATTRIBUTES_OUTPUT (qmi_message_uim_get_file_attributes_output_get_type ())

gboolean qmi_message_uim_get_file_attributes_output_get_result (
    QmiMessageUimGetFileAttributesOutput *self,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_output_get_card_result (
    QmiMessageUimGetFileAttributesOutput *self,
    guint8 *card_result_sw1,
    guint8 *card_result_sw2,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_output_get_file_attributes (
    QmiMessageUimGetFileAttributesOutput *self,
    guint16 *file_attributes_file_size,
    guint16 *file_attributes_file_id,
    QmiUimFileType *file_attributes_file_type,
    guint16 *file_attributes_record_size,
    guint16 *file_attributes_record_count,
    QmiUimSecurityAttributeLogic *file_attributes_read_security_attributes_logic,
    QmiUimSecurityAttribute *file_attributes_read_security_attributes,
    QmiUimSecurityAttributeLogic *file_attributes_write_security_attributes_logic,
    QmiUimSecurityAttribute *file_attributes_write_security_attributes,
    QmiUimSecurityAttributeLogic *file_attributes_increase_security_attributes_logic,
    QmiUimSecurityAttribute *file_attributes_increase_security_attributes,
    QmiUimSecurityAttributeLogic *file_attributes_deactivate_security_attributes_logic,
    QmiUimSecurityAttribute *file_attributes_deactivate_security_attributes,
    QmiUimSecurityAttributeLogic *file_attributes_activate_security_attributes_logic,
    QmiUimSecurityAttribute *file_attributes_activate_security_attributes,
    GArray **file_attributes_raw_data,
    GError **error);

gboolean qmi_message_uim_get_file_attributes_output_get_response_in_indication_token (
    QmiMessageUimGetFileAttributesOutput *self,
    guint32 *response_in_indication_token,
    GError **error);

QmiMessageUimGetFileAttributesOutput *qmi_message_uim_get_file_attributes_output_ref (QmiMessageUimGetFileAttributesOutput *self);
void qmi_message_uim_get_file_attributes_output_unref (QmiMessageUimGetFileAttributesOutput *self);

/*****************************************************************************/
/* Service-specific printable: UIM */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_uim_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_uim_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client UIM */

#define QMI_TYPE_CLIENT_UIM            (qmi_client_uim_get_type ())
#define QMI_CLIENT_UIM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_UIM, QmiClientUim))
#define QMI_CLIENT_UIM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_UIM, QmiClientUimClass))
#define QMI_IS_CLIENT_UIM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_UIM))
#define QMI_IS_CLIENT_UIM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_UIM))
#define QMI_CLIENT_UIM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_UIM, QmiClientUimClass))

typedef struct _QmiClientUim QmiClientUim;
typedef struct _QmiClientUimClass QmiClientUimClass;

/**
 * QmiClientUim:
 *
 * The #QmiClientUim structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientUim {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientUimClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_uim_get_type (void);


void qmi_client_uim_reset (
    QmiClientUim *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageUimResetOutput *qmi_client_uim_reset_finish (
    QmiClientUim *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_uim_read_transparent (
    QmiClientUim *self,
    QmiMessageUimReadTransparentInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageUimReadTransparentOutput *qmi_client_uim_read_transparent_finish (
    QmiClientUim *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_uim_read_record (
    QmiClientUim *self,
    QmiMessageUimReadRecordInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageUimReadRecordOutput *qmi_client_uim_read_record_finish (
    QmiClientUim *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_uim_get_file_attributes (
    QmiClientUim *self,
    QmiMessageUimGetFileAttributesInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessageUimGetFileAttributesOutput *qmi_client_uim_get_file_attributes_finish (
    QmiClientUim *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_UIM__ */
