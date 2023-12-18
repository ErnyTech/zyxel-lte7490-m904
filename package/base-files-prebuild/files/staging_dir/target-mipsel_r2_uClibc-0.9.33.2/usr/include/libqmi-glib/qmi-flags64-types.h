
/* Generated data (by glib-mkenums) */

#ifndef __LIBQMI_GLIB_FLAGS64_TYPES_H__
#define __LIBQMI_GLIB_FLAGS64_TYPES_H__
#include "qmi-flags64-dms.h"
#include "qmi-flags64-nas.h"

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "../../../src/libqmi-glib/qmi-flags64-dms.h" */

#define __QMI_DMS_BAND_CAPABILITY_IS_FLAGS__

#if defined __QMI_DMS_BAND_CAPABILITY_IS_ENUM__
#error Only flags expected, QmiDmsBandCapability is an enumeration
#endif

gchar *qmi_dms_band_capability_build_string_from_mask (QmiDmsBandCapability mask);


#define __QMI_DMS_LTE_BAND_CAPABILITY_IS_FLAGS__

#if defined __QMI_DMS_LTE_BAND_CAPABILITY_IS_ENUM__
#error Only flags expected, QmiDmsLteBandCapability is an enumeration
#endif

gchar *qmi_dms_lte_band_capability_build_string_from_mask (QmiDmsLteBandCapability mask);


/* enumerations from "../../../src/libqmi-glib/qmi-flags64-nas.h" */

#define __QMI_NAS_BAND_PREFERENCE_IS_FLAGS__

#if defined __QMI_NAS_BAND_PREFERENCE_IS_ENUM__
#error Only flags expected, QmiNasBandPreference is an enumeration
#endif

gchar *qmi_nas_band_preference_build_string_from_mask (QmiNasBandPreference mask);


#define __QMI_NAS_LTE_BAND_PREFERENCE_IS_FLAGS__

#if defined __QMI_NAS_LTE_BAND_PREFERENCE_IS_ENUM__
#error Only flags expected, QmiNasLteBandPreference is an enumeration
#endif

gchar *qmi_nas_lte_band_preference_build_string_from_mask (QmiNasLteBandPreference mask);

G_END_DECLS

#endif /* __LIBQMI_GLIB_FLAGS64_TYPES_H__ */

/* Generated data ends here */

