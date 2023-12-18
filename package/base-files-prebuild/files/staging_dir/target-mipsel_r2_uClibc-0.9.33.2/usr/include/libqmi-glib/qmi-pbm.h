
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
#include "qmi-enums-pbm.h"
#include "qmi-message.h"
#include "qmi-client.h"

#ifndef __LIBQMI_GLIB_QMI_PBM__
#define __LIBQMI_GLIB_QMI_PBM__

G_BEGIN_DECLS


/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message PBM Indication Register */


/* --- Input -- */

/**
 * QmiMessagePbmIndicationRegisterInput:
 *
 * The #QmiMessagePbmIndicationRegisterInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessagePbmIndicationRegisterInput QmiMessagePbmIndicationRegisterInput;
GType qmi_message_pbm_indication_register_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_PBM_INDICATION_REGISTER_INPUT (qmi_message_pbm_indication_register_input_get_type ())

gboolean qmi_message_pbm_indication_register_input_get_event_registration_mask (
    QmiMessagePbmIndicationRegisterInput *self,
    QmiPbmEventRegistrationFlag *event_registration_mask,
    GError **error);

gboolean qmi_message_pbm_indication_register_input_set_event_registration_mask (
    QmiMessagePbmIndicationRegisterInput *self,
    QmiPbmEventRegistrationFlag event_registration_mask,
    GError **error);

QmiMessagePbmIndicationRegisterInput *qmi_message_pbm_indication_register_input_ref (QmiMessagePbmIndicationRegisterInput *self);
void qmi_message_pbm_indication_register_input_unref (QmiMessagePbmIndicationRegisterInput *self);
QmiMessagePbmIndicationRegisterInput *qmi_message_pbm_indication_register_input_new (void);

/* --- Output -- */

/**
 * QmiMessagePbmIndicationRegisterOutput:
 *
 * The #QmiMessagePbmIndicationRegisterOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessagePbmIndicationRegisterOutput QmiMessagePbmIndicationRegisterOutput;
GType qmi_message_pbm_indication_register_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_PBM_INDICATION_REGISTER_OUTPUT (qmi_message_pbm_indication_register_output_get_type ())

gboolean qmi_message_pbm_indication_register_output_get_result (
    QmiMessagePbmIndicationRegisterOutput *self,
    GError **error);

gboolean qmi_message_pbm_indication_register_output_get_event_registration_mask (
    QmiMessagePbmIndicationRegisterOutput *self,
    QmiPbmEventRegistrationFlag *event_registration_mask,
    GError **error);

QmiMessagePbmIndicationRegisterOutput *qmi_message_pbm_indication_register_output_ref (QmiMessagePbmIndicationRegisterOutput *self);
void qmi_message_pbm_indication_register_output_unref (QmiMessagePbmIndicationRegisterOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message PBM Get Capabilities */


/* --- Input -- */

/**
 * QmiMessagePbmGetCapabilitiesInput:
 *
 * The #QmiMessagePbmGetCapabilitiesInput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessagePbmGetCapabilitiesInput QmiMessagePbmGetCapabilitiesInput;
GType qmi_message_pbm_get_capabilities_input_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_PBM_GET_CAPABILITIES_INPUT (qmi_message_pbm_get_capabilities_input_get_type ())

gboolean qmi_message_pbm_get_capabilities_input_get_phonebook_information (
    QmiMessagePbmGetCapabilitiesInput *self,
    QmiPbmSessionType *phonebook_information_session_type,
    QmiPbmPhonebookType *phonebook_information_phonebook_type,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_input_set_phonebook_information (
    QmiMessagePbmGetCapabilitiesInput *self,
    QmiPbmSessionType phonebook_information_session_type,
    QmiPbmPhonebookType phonebook_information_phonebook_type,
    GError **error);

QmiMessagePbmGetCapabilitiesInput *qmi_message_pbm_get_capabilities_input_ref (QmiMessagePbmGetCapabilitiesInput *self);
void qmi_message_pbm_get_capabilities_input_unref (QmiMessagePbmGetCapabilitiesInput *self);
QmiMessagePbmGetCapabilitiesInput *qmi_message_pbm_get_capabilities_input_new (void);

/* --- Output -- */

/**
 * QmiMessagePbmGetCapabilitiesOutput:
 *
 * The #QmiMessagePbmGetCapabilitiesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessagePbmGetCapabilitiesOutput QmiMessagePbmGetCapabilitiesOutput;
GType qmi_message_pbm_get_capabilities_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_PBM_GET_CAPABILITIES_OUTPUT (qmi_message_pbm_get_capabilities_output_get_type ())

gboolean qmi_message_pbm_get_capabilities_output_get_result (
    QmiMessagePbmGetCapabilitiesOutput *self,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_capability_basic_information (
    QmiMessagePbmGetCapabilitiesOutput *self,
    QmiPbmSessionType *capability_basic_information_session_type,
    QmiPbmPhonebookType *capability_basic_information_phonebook_type,
    guint16 *capability_basic_information_used_records,
    guint16 *capability_basic_information_maximum_records,
    guint8 *capability_basic_information_maximum_number_length,
    guint8 *capability_basic_information_maximum_name_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_group_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *group_capability_maximum_groups,
    guint8 *group_capability_maximum_group_tag_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_additional_number_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *additional_number_capability_maximum_additional_numbers,
    guint8 *additional_number_capability_maximum_additional_number_length,
    guint8 *additional_number_capability_maximum_additional_number_tag_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_email_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *email_capability_maximum_emails,
    guint8 *email_capability_maximum_email_address_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_second_name_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *second_name_capability_maximum_second_name_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_hidden_records_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    gboolean *hidden_records_capability_supported,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_grouping_information_alpha_string_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *grouping_information_alpha_string_capability_maximum_records,
    guint8 *grouping_information_alpha_string_capability_used_records,
    guint8 *grouping_information_alpha_string_capability_maximum_string_length,
    GError **error);

gboolean qmi_message_pbm_get_capabilities_output_get_additional_number_alpha_string_capability (
    QmiMessagePbmGetCapabilitiesOutput *self,
    guint8 *additional_number_alpha_string_capability_maximum_records,
    guint8 *additional_number_alpha_string_capability_used_records,
    guint8 *additional_number_alpha_string_capability_maximum_string_length,
    GError **error);

QmiMessagePbmGetCapabilitiesOutput *qmi_message_pbm_get_capabilities_output_ref (QmiMessagePbmGetCapabilitiesOutput *self);
void qmi_message_pbm_get_capabilities_output_unref (QmiMessagePbmGetCapabilitiesOutput *self);

/*****************************************************************************/
/* REQUEST/RESPONSE: Qmi Message PBM Get All Capabilities */


/* --- Input -- */

/* Note: no fields in the Input container */

/* --- Output -- */

/**
 * QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElementPhonebooksElement:
 * @phonebook_type: a #QmiPbmPhonebookType.
 * @used_records: a #guint16.
 * @maximum_records: a #guint16.
 * @maximum_number_length: a #guint8.
 * @maximum_name_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElementPhonebooksElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElementPhonebooksElement {
    QmiPbmPhonebookType phonebook_type;
    guint16 used_records;
    guint16 maximum_records;
    guint8 maximum_number_length;
    guint8 maximum_name_length;
} QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElementPhonebooksElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElement:
 * @session_type: a #QmiPbmSessionType.
 * @phonebooks: a #GArray of #QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElementPhonebooksElement elements.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElement {
    QmiPbmSessionType session_type;
    GArray * phonebooks;
} QmiMessagePbmGetAllCapabilitiesOutputCapabilityBasicInformationElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputGroupCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_groups: a #guint8.
 * @maximum_group_tag_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputGroupCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputGroupCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_groups;
    guint8 maximum_group_tag_length;
} QmiMessagePbmGetAllCapabilitiesOutputGroupCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_additional_numbers: a #guint8.
 * @maximum_additional_number_length: a #guint8.
 * @maximum_additional_number_tag_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_additional_numbers;
    guint8 maximum_additional_number_length;
    guint8 maximum_additional_number_tag_length;
} QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputEmailCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_emails: a #guint8.
 * @maximum_email_address_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputEmailCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputEmailCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_emails;
    guint8 maximum_email_address_length;
} QmiMessagePbmGetAllCapabilitiesOutputEmailCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputSecondNameCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_second_name_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputSecondNameCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputSecondNameCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_second_name_length;
} QmiMessagePbmGetAllCapabilitiesOutputSecondNameCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputHiddenRecordsCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @supported: a #gboolean.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputHiddenRecordsCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputHiddenRecordsCapabilityElement {
    QmiPbmSessionType session_type;
    gboolean supported;
} QmiMessagePbmGetAllCapabilitiesOutputHiddenRecordsCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputGroupingInformationAlphaStringCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_records: a #guint8.
 * @used_records: a #guint8.
 * @maximum_string_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputGroupingInformationAlphaStringCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputGroupingInformationAlphaStringCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_records;
    guint8 used_records;
    guint8 maximum_string_length;
} QmiMessagePbmGetAllCapabilitiesOutputGroupingInformationAlphaStringCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberAlphaStringCapabilityElement:
 * @session_type: a #QmiPbmSessionType.
 * @maximum_records: a #guint8.
 * @used_records: a #guint8.
 * @maximum_string_length: a #guint8.
 *
 * A QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberAlphaStringCapabilityElement struct.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberAlphaStringCapabilityElement {
    QmiPbmSessionType session_type;
    guint8 maximum_records;
    guint8 used_records;
    guint8 maximum_string_length;
} QmiMessagePbmGetAllCapabilitiesOutputAdditionalNumberAlphaStringCapabilityElement;

/**
 * QmiMessagePbmGetAllCapabilitiesOutput:
 *
 * The #QmiMessagePbmGetAllCapabilitiesOutput structure contains private data and should only be accessed
 * using the provided API.
 */
typedef struct _QmiMessagePbmGetAllCapabilitiesOutput QmiMessagePbmGetAllCapabilitiesOutput;
GType qmi_message_pbm_get_all_capabilities_output_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_MESSAGE_PBM_GET_ALL_CAPABILITIES_OUTPUT (qmi_message_pbm_get_all_capabilities_output_get_type ())

gboolean qmi_message_pbm_get_all_capabilities_output_get_result (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_capability_basic_information (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **capability_basic_information,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_group_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **group_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_additional_number_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **additional_number_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_email_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **email_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_second_name_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **second_name_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_hidden_records_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **hidden_records_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_grouping_information_alpha_string_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **grouping_information_alpha_string_capability,
    GError **error);

gboolean qmi_message_pbm_get_all_capabilities_output_get_additional_number_alpha_string_capability (
    QmiMessagePbmGetAllCapabilitiesOutput *self,
    GArray **additional_number_alpha_string_capability,
    GError **error);

QmiMessagePbmGetAllCapabilitiesOutput *qmi_message_pbm_get_all_capabilities_output_ref (QmiMessagePbmGetAllCapabilitiesOutput *self);
void qmi_message_pbm_get_all_capabilities_output_unref (QmiMessagePbmGetAllCapabilitiesOutput *self);

/*****************************************************************************/
/* Service-specific printable: PBM */


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gchar *__qmi_message_pbm_get_printable (
    QmiMessage *self,
    const gchar *line_prefix);

#endif


#if defined (LIBQMI_GLIB_COMPILATION)

G_GNUC_INTERNAL
gboolean __qmi_message_pbm_get_version_introduced (
    QmiMessage *self,
    guint *major,
    guint *minor);

#endif


/*****************************************************************************/
/* CLIENT: QMI Client PBM */

#define QMI_TYPE_CLIENT_PBM            (qmi_client_pbm_get_type ())
#define QMI_CLIENT_PBM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QMI_TYPE_CLIENT_PBM, QmiClientPbm))
#define QMI_CLIENT_PBM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  QMI_TYPE_CLIENT_PBM, QmiClientPbmClass))
#define QMI_IS_CLIENT_PBM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QMI_TYPE_CLIENT_PBM))
#define QMI_IS_CLIENT_PBM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  QMI_TYPE_CLIENT_PBM))
#define QMI_CLIENT_PBM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  QMI_TYPE_CLIENT_PBM, QmiClientPbmClass))

typedef struct _QmiClientPbm QmiClientPbm;
typedef struct _QmiClientPbmClass QmiClientPbmClass;

/**
 * QmiClientPbm:
 *
 * The #QmiClientPbm structure contains private data and should only be accessed
 * using the provided API.
 */
struct _QmiClientPbm {
    /*< private >*/
    QmiClient parent;
    gpointer priv_unused;
};

struct _QmiClientPbmClass {
    /*< private >*/
    QmiClientClass parent;
};

GType qmi_client_pbm_get_type (void);


void qmi_client_pbm_indication_register (
    QmiClientPbm *self,
    QmiMessagePbmIndicationRegisterInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessagePbmIndicationRegisterOutput *qmi_client_pbm_indication_register_finish (
    QmiClientPbm *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_pbm_get_capabilities (
    QmiClientPbm *self,
    QmiMessagePbmGetCapabilitiesInput *input,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessagePbmGetCapabilitiesOutput *qmi_client_pbm_get_capabilities_finish (
    QmiClientPbm *self,
    GAsyncResult *res,
    GError **error);

void qmi_client_pbm_get_all_capabilities (
    QmiClientPbm *self,
    gpointer unused,
    guint timeout,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);
QmiMessagePbmGetAllCapabilitiesOutput *qmi_client_pbm_get_all_capabilities_finish (
    QmiClientPbm *self,
    GAsyncResult *res,
    GError **error);

G_END_DECLS

#endif /* __LIBQMI_GLIB_QMI_PBM__ */
