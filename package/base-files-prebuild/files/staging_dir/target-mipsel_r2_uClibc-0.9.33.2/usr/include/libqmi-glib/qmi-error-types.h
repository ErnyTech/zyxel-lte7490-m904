
/* Generated data (by glib-mkenums) */

#ifndef __LIBQMI_GLIB_ERROR_TYPES_H__
#define __LIBQMI_GLIB_ERROR_TYPES_H__
#include "qmi-errors.h"

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "../../../src/libqmi-glib/qmi-errors.h" */
GQuark qmi_core_error_quark    (void);
GType  qmi_core_error_get_type (void) G_GNUC_CONST;
#define QMI_CORE_ERROR (qmi_core_error_quark ())
#define QMI_TYPE_CORE_ERROR (qmi_core_error_get_type ())
const gchar *qmi_core_error_get_string (QmiCoreError val);
GQuark qmi_protocol_error_quark    (void);
GType  qmi_protocol_error_get_type (void) G_GNUC_CONST;
#define QMI_PROTOCOL_ERROR (qmi_protocol_error_quark ())
#define QMI_TYPE_PROTOCOL_ERROR (qmi_protocol_error_get_type ())
const gchar *qmi_protocol_error_get_string (QmiProtocolError val);
G_END_DECLS

#endif /* __LIBQMI_GLIB_ERROR_TYPES_H__ */

/* Generated data ends here */

