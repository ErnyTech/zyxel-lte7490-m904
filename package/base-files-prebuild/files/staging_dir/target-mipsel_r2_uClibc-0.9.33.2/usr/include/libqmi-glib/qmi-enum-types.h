
/* Generated data (by glib-mkenums) */

#ifndef __LIBQMI_GLIB_ENUM_TYPES_H__
#define __LIBQMI_GLIB_ENUM_TYPES_H__
#include "qmi-enums.h"
#include "qmi-enums-wds.h"
#include "qmi-enums-dms.h"
#include "qmi-enums-nas.h"
#include "qmi-enums-wms.h"
#include "qmi-enums-pds.h"
#include "qmi-enums-pbm.h"
#include "qmi-enums-uim.h"
#include "qmi-enums-oma.h"

#include <glib-object.h>

G_BEGIN_DECLS

/* enumerations from "../../../src/libqmi-glib/qmi-enums.h" */
GType qmi_service_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_SERVICE (qmi_service_get_type ())

/* Define type-specific symbols */
#define __QMI_SERVICE_IS_ENUM__

#if defined __QMI_SERVICE_IS_ENUM__
const gchar *qmi_service_get_string (QmiService val);
#endif

#if defined __QMI_SERVICE_IS_FLAGS__
gchar *qmi_service_build_string_from_mask (QmiService mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-wds.h" */
GType qmi_wds_ip_family_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_IP_FAMILY (qmi_wds_ip_family_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_IP_FAMILY_IS_ENUM__

#if defined __QMI_WDS_IP_FAMILY_IS_ENUM__
const gchar *qmi_wds_ip_family_get_string (QmiWdsIpFamily val);
#endif

#if defined __QMI_WDS_IP_FAMILY_IS_FLAGS__
gchar *qmi_wds_ip_family_build_string_from_mask (QmiWdsIpFamily mask);
#endif

GType qmi_wds_technology_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_TECHNOLOGY_PREFERENCE (qmi_wds_technology_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_TECHNOLOGY_PREFERENCE_IS_FLAGS__

#if defined __QMI_WDS_TECHNOLOGY_PREFERENCE_IS_ENUM__
const gchar *qmi_wds_technology_preference_get_string (QmiWdsTechnologyPreference val);
#endif

#if defined __QMI_WDS_TECHNOLOGY_PREFERENCE_IS_FLAGS__
gchar *qmi_wds_technology_preference_build_string_from_mask (QmiWdsTechnologyPreference mask);
#endif

GType qmi_wds_extended_technology_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_EXTENDED_TECHNOLOGY_PREFERENCE (qmi_wds_extended_technology_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_EXTENDED_TECHNOLOGY_PREFERENCE_IS_ENUM__

#if defined __QMI_WDS_EXTENDED_TECHNOLOGY_PREFERENCE_IS_ENUM__
const gchar *qmi_wds_extended_technology_preference_get_string (QmiWdsExtendedTechnologyPreference val);
#endif

#if defined __QMI_WDS_EXTENDED_TECHNOLOGY_PREFERENCE_IS_FLAGS__
gchar *qmi_wds_extended_technology_preference_build_string_from_mask (QmiWdsExtendedTechnologyPreference mask);
#endif

GType qmi_wds_call_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_CALL_TYPE (qmi_wds_call_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_CALL_TYPE_IS_ENUM__

#if defined __QMI_WDS_CALL_TYPE_IS_ENUM__
const gchar *qmi_wds_call_type_get_string (QmiWdsCallType val);
#endif

#if defined __QMI_WDS_CALL_TYPE_IS_FLAGS__
gchar *qmi_wds_call_type_build_string_from_mask (QmiWdsCallType mask);
#endif

GType qmi_wds_call_end_reason_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_CALL_END_REASON (qmi_wds_call_end_reason_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_CALL_END_REASON_IS_ENUM__

#if defined __QMI_WDS_CALL_END_REASON_IS_ENUM__
const gchar *qmi_wds_call_end_reason_get_string (QmiWdsCallEndReason val);
#endif

#if defined __QMI_WDS_CALL_END_REASON_IS_FLAGS__
gchar *qmi_wds_call_end_reason_build_string_from_mask (QmiWdsCallEndReason mask);
#endif

GType qmi_wds_verbose_call_end_reason_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_TYPE (qmi_wds_verbose_call_end_reason_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_type_get_string (QmiWdsVerboseCallEndReasonType val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_type_build_string_from_mask (QmiWdsVerboseCallEndReasonType mask);
#endif

GType qmi_wds_verbose_call_end_reason_mip_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_MIP (qmi_wds_verbose_call_end_reason_mip_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_MIP_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_MIP_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_mip_get_string (QmiWdsVerboseCallEndReasonMip val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_MIP_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_mip_build_string_from_mask (QmiWdsVerboseCallEndReasonMip mask);
#endif

GType qmi_wds_verbose_call_end_reason_internal_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_INTERNAL (qmi_wds_verbose_call_end_reason_internal_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_INTERNAL_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_INTERNAL_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_internal_get_string (QmiWdsVerboseCallEndReasonInternal val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_INTERNAL_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_internal_build_string_from_mask (QmiWdsVerboseCallEndReasonInternal mask);
#endif

GType qmi_wds_verbose_call_end_reason_cm_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_CM (qmi_wds_verbose_call_end_reason_cm_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_CM_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_CM_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_cm_get_string (QmiWdsVerboseCallEndReasonCm val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_CM_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_cm_build_string_from_mask (QmiWdsVerboseCallEndReasonCm mask);
#endif

GType qmi_wds_verbose_call_end_reason_3gpp_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_3GPP (qmi_wds_verbose_call_end_reason_3gpp_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_3GPP_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_3GPP_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_3gpp_get_string (QmiWdsVerboseCallEndReason3gpp val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_3GPP_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_3gpp_build_string_from_mask (QmiWdsVerboseCallEndReason3gpp mask);
#endif

GType qmi_wds_verbose_call_end_reason_ppp_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_PPP (qmi_wds_verbose_call_end_reason_ppp_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_PPP_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_PPP_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_ppp_get_string (QmiWdsVerboseCallEndReasonPpp val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_PPP_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_ppp_build_string_from_mask (QmiWdsVerboseCallEndReasonPpp mask);
#endif

GType qmi_wds_verbose_call_end_reason_ehrpd_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_EHRPD (qmi_wds_verbose_call_end_reason_ehrpd_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_EHRPD_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_EHRPD_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_ehrpd_get_string (QmiWdsVerboseCallEndReasonEhrpd val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_EHRPD_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_ehrpd_build_string_from_mask (QmiWdsVerboseCallEndReasonEhrpd mask);
#endif

GType qmi_wds_verbose_call_end_reason_ipv6_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_VERBOSE_CALL_END_REASON_IPV6 (qmi_wds_verbose_call_end_reason_ipv6_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_VERBOSE_CALL_END_REASON_IPV6_IS_ENUM__

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_IPV6_IS_ENUM__
const gchar *qmi_wds_verbose_call_end_reason_ipv6_get_string (QmiWdsVerboseCallEndReasonIpv6 val);
#endif

#if defined __QMI_WDS_VERBOSE_CALL_END_REASON_IPV6_IS_FLAGS__
gchar *qmi_wds_verbose_call_end_reason_ipv6_build_string_from_mask (QmiWdsVerboseCallEndReasonIpv6 mask);
#endif

GType qmi_wds_connection_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_CONNECTION_STATUS (qmi_wds_connection_status_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_CONNECTION_STATUS_IS_ENUM__

#if defined __QMI_WDS_CONNECTION_STATUS_IS_ENUM__
const gchar *qmi_wds_connection_status_get_string (QmiWdsConnectionStatus val);
#endif

#if defined __QMI_WDS_CONNECTION_STATUS_IS_FLAGS__
gchar *qmi_wds_connection_status_build_string_from_mask (QmiWdsConnectionStatus mask);
#endif

GType qmi_wds_data_bearer_technology_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_DATA_BEARER_TECHNOLOGY (qmi_wds_data_bearer_technology_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_DATA_BEARER_TECHNOLOGY_IS_ENUM__

#if defined __QMI_WDS_DATA_BEARER_TECHNOLOGY_IS_ENUM__
const gchar *qmi_wds_data_bearer_technology_get_string (QmiWdsDataBearerTechnology val);
#endif

#if defined __QMI_WDS_DATA_BEARER_TECHNOLOGY_IS_FLAGS__
gchar *qmi_wds_data_bearer_technology_build_string_from_mask (QmiWdsDataBearerTechnology mask);
#endif

GType qmi_wds_network_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_NETWORK_TYPE (qmi_wds_network_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_NETWORK_TYPE_IS_ENUM__

#if defined __QMI_WDS_NETWORK_TYPE_IS_ENUM__
const gchar *qmi_wds_network_type_get_string (QmiWdsNetworkType val);
#endif

#if defined __QMI_WDS_NETWORK_TYPE_IS_FLAGS__
gchar *qmi_wds_network_type_build_string_from_mask (QmiWdsNetworkType mask);
#endif

GType qmi_wds_rat_3gpp2_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_RAT_3GPP2 (qmi_wds_rat_3gpp2_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_RAT_3GPP2_IS_FLAGS__

#if defined __QMI_WDS_RAT_3GPP2_IS_ENUM__
const gchar *qmi_wds_rat_3gpp2_get_string (QmiWdsRat3gpp2 val);
#endif

#if defined __QMI_WDS_RAT_3GPP2_IS_FLAGS__
gchar *qmi_wds_rat_3gpp2_build_string_from_mask (QmiWdsRat3gpp2 mask);
#endif

GType qmi_wds_rat_3gpp_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_RAT_3GPP (qmi_wds_rat_3gpp_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_RAT_3GPP_IS_FLAGS__

#if defined __QMI_WDS_RAT_3GPP_IS_ENUM__
const gchar *qmi_wds_rat_3gpp_get_string (QmiWdsRat3gpp val);
#endif

#if defined __QMI_WDS_RAT_3GPP_IS_FLAGS__
gchar *qmi_wds_rat_3gpp_build_string_from_mask (QmiWdsRat3gpp mask);
#endif

GType qmi_wds_so_cdma1x_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_SO_CDMA1X (qmi_wds_so_cdma1x_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_SO_CDMA1X_IS_FLAGS__

#if defined __QMI_WDS_SO_CDMA1X_IS_ENUM__
const gchar *qmi_wds_so_cdma1x_get_string (QmiWdsSoCdma1x val);
#endif

#if defined __QMI_WDS_SO_CDMA1X_IS_FLAGS__
gchar *qmi_wds_so_cdma1x_build_string_from_mask (QmiWdsSoCdma1x mask);
#endif

GType qmi_wds_so_evdo_reva_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_SO_EVDO_REVA (qmi_wds_so_evdo_reva_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_SO_EVDO_REVA_IS_FLAGS__

#if defined __QMI_WDS_SO_EVDO_REVA_IS_ENUM__
const gchar *qmi_wds_so_evdo_reva_get_string (QmiWdsSoEvdoRevA val);
#endif

#if defined __QMI_WDS_SO_EVDO_REVA_IS_FLAGS__
gchar *qmi_wds_so_evdo_reva_build_string_from_mask (QmiWdsSoEvdoRevA mask);
#endif

GType qmi_wds_get_current_settings_requested_settings_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_GET_CURRENT_SETTINGS_REQUESTED_SETTINGS (qmi_wds_get_current_settings_requested_settings_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_GET_CURRENT_SETTINGS_REQUESTED_SETTINGS_IS_FLAGS__

#if defined __QMI_WDS_GET_CURRENT_SETTINGS_REQUESTED_SETTINGS_IS_ENUM__
const gchar *qmi_wds_get_current_settings_requested_settings_get_string (QmiWdsGetCurrentSettingsRequestedSettings val);
#endif

#if defined __QMI_WDS_GET_CURRENT_SETTINGS_REQUESTED_SETTINGS_IS_FLAGS__
gchar *qmi_wds_get_current_settings_requested_settings_build_string_from_mask (QmiWdsGetCurrentSettingsRequestedSettings mask);
#endif

GType qmi_wds_pdp_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_PDP_TYPE (qmi_wds_pdp_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_PDP_TYPE_IS_ENUM__

#if defined __QMI_WDS_PDP_TYPE_IS_ENUM__
const gchar *qmi_wds_pdp_type_get_string (QmiWdsPdpType val);
#endif

#if defined __QMI_WDS_PDP_TYPE_IS_FLAGS__
gchar *qmi_wds_pdp_type_build_string_from_mask (QmiWdsPdpType mask);
#endif

GType qmi_wds_traffic_class_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_TRAFFIC_CLASS (qmi_wds_traffic_class_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_TRAFFIC_CLASS_IS_ENUM__

#if defined __QMI_WDS_TRAFFIC_CLASS_IS_ENUM__
const gchar *qmi_wds_traffic_class_get_string (QmiWdsTrafficClass val);
#endif

#if defined __QMI_WDS_TRAFFIC_CLASS_IS_FLAGS__
gchar *qmi_wds_traffic_class_build_string_from_mask (QmiWdsTrafficClass mask);
#endif

GType qmi_wds_authentication_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_AUTHENTICATION (qmi_wds_authentication_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_AUTHENTICATION_IS_FLAGS__

#if defined __QMI_WDS_AUTHENTICATION_IS_ENUM__
const gchar *qmi_wds_authentication_get_string (QmiWdsAuthentication val);
#endif

#if defined __QMI_WDS_AUTHENTICATION_IS_FLAGS__
gchar *qmi_wds_authentication_build_string_from_mask (QmiWdsAuthentication mask);
#endif

GType qmi_wds_profile_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_PROFILE_TYPE (qmi_wds_profile_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_PROFILE_TYPE_IS_ENUM__

#if defined __QMI_WDS_PROFILE_TYPE_IS_ENUM__
const gchar *qmi_wds_profile_type_get_string (QmiWdsProfileType val);
#endif

#if defined __QMI_WDS_PROFILE_TYPE_IS_FLAGS__
gchar *qmi_wds_profile_type_build_string_from_mask (QmiWdsProfileType mask);
#endif

GType qmi_wds_packet_statistics_mask_flag_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_PACKET_STATISTICS_MASK_FLAG (qmi_wds_packet_statistics_mask_flag_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_PACKET_STATISTICS_MASK_FLAG_IS_FLAGS__

#if defined __QMI_WDS_PACKET_STATISTICS_MASK_FLAG_IS_ENUM__
const gchar *qmi_wds_packet_statistics_mask_flag_get_string (QmiWdsPacketStatisticsMaskFlag val);
#endif

#if defined __QMI_WDS_PACKET_STATISTICS_MASK_FLAG_IS_FLAGS__
gchar *qmi_wds_packet_statistics_mask_flag_build_string_from_mask (QmiWdsPacketStatisticsMaskFlag mask);
#endif

GType qmi_wds_ds_profile_error_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WDS_DS_PROFILE_ERROR (qmi_wds_ds_profile_error_get_type ())

/* Define type-specific symbols */
#define __QMI_WDS_DS_PROFILE_ERROR_IS_ENUM__

#if defined __QMI_WDS_DS_PROFILE_ERROR_IS_ENUM__
const gchar *qmi_wds_ds_profile_error_get_string (QmiWdsDsProfileError val);
#endif

#if defined __QMI_WDS_DS_PROFILE_ERROR_IS_FLAGS__
gchar *qmi_wds_ds_profile_error_build_string_from_mask (QmiWdsDsProfileError mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-dms.h" */
GType qmi_dms_data_service_capability_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_DATA_SERVICE_CAPABILITY (qmi_dms_data_service_capability_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_DATA_SERVICE_CAPABILITY_IS_ENUM__

#if defined __QMI_DMS_DATA_SERVICE_CAPABILITY_IS_ENUM__
const gchar *qmi_dms_data_service_capability_get_string (QmiDmsDataServiceCapability val);
#endif

#if defined __QMI_DMS_DATA_SERVICE_CAPABILITY_IS_FLAGS__
gchar *qmi_dms_data_service_capability_build_string_from_mask (QmiDmsDataServiceCapability mask);
#endif

GType qmi_dms_sim_capability_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_SIM_CAPABILITY (qmi_dms_sim_capability_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_SIM_CAPABILITY_IS_ENUM__

#if defined __QMI_DMS_SIM_CAPABILITY_IS_ENUM__
const gchar *qmi_dms_sim_capability_get_string (QmiDmsSimCapability val);
#endif

#if defined __QMI_DMS_SIM_CAPABILITY_IS_FLAGS__
gchar *qmi_dms_sim_capability_build_string_from_mask (QmiDmsSimCapability mask);
#endif

GType qmi_dms_radio_interface_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_RADIO_INTERFACE (qmi_dms_radio_interface_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_RADIO_INTERFACE_IS_ENUM__

#if defined __QMI_DMS_RADIO_INTERFACE_IS_ENUM__
const gchar *qmi_dms_radio_interface_get_string (QmiDmsRadioInterface val);
#endif

#if defined __QMI_DMS_RADIO_INTERFACE_IS_FLAGS__
gchar *qmi_dms_radio_interface_build_string_from_mask (QmiDmsRadioInterface mask);
#endif

GType qmi_dms_power_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_POWER_STATE (qmi_dms_power_state_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_POWER_STATE_IS_FLAGS__

#if defined __QMI_DMS_POWER_STATE_IS_ENUM__
const gchar *qmi_dms_power_state_get_string (QmiDmsPowerState val);
#endif

#if defined __QMI_DMS_POWER_STATE_IS_FLAGS__
gchar *qmi_dms_power_state_build_string_from_mask (QmiDmsPowerState mask);
#endif

GType qmi_dms_uim_pin_id_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_UIM_PIN_ID (qmi_dms_uim_pin_id_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_UIM_PIN_ID_IS_ENUM__

#if defined __QMI_DMS_UIM_PIN_ID_IS_ENUM__
const gchar *qmi_dms_uim_pin_id_get_string (QmiDmsUimPinId val);
#endif

#if defined __QMI_DMS_UIM_PIN_ID_IS_FLAGS__
gchar *qmi_dms_uim_pin_id_build_string_from_mask (QmiDmsUimPinId mask);
#endif

GType qmi_dms_uim_pin_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_UIM_PIN_STATUS (qmi_dms_uim_pin_status_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_UIM_PIN_STATUS_IS_ENUM__

#if defined __QMI_DMS_UIM_PIN_STATUS_IS_ENUM__
const gchar *qmi_dms_uim_pin_status_get_string (QmiDmsUimPinStatus val);
#endif

#if defined __QMI_DMS_UIM_PIN_STATUS_IS_FLAGS__
gchar *qmi_dms_uim_pin_status_build_string_from_mask (QmiDmsUimPinStatus mask);
#endif

GType qmi_dms_operating_mode_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_OPERATING_MODE (qmi_dms_operating_mode_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_OPERATING_MODE_IS_ENUM__

#if defined __QMI_DMS_OPERATING_MODE_IS_ENUM__
const gchar *qmi_dms_operating_mode_get_string (QmiDmsOperatingMode val);
#endif

#if defined __QMI_DMS_OPERATING_MODE_IS_FLAGS__
gchar *qmi_dms_operating_mode_build_string_from_mask (QmiDmsOperatingMode mask);
#endif

GType qmi_dms_offline_reason_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_OFFLINE_REASON (qmi_dms_offline_reason_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_OFFLINE_REASON_IS_FLAGS__

#if defined __QMI_DMS_OFFLINE_REASON_IS_ENUM__
const gchar *qmi_dms_offline_reason_get_string (QmiDmsOfflineReason val);
#endif

#if defined __QMI_DMS_OFFLINE_REASON_IS_FLAGS__
gchar *qmi_dms_offline_reason_build_string_from_mask (QmiDmsOfflineReason mask);
#endif

GType qmi_dms_time_source_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_TIME_SOURCE (qmi_dms_time_source_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_TIME_SOURCE_IS_ENUM__

#if defined __QMI_DMS_TIME_SOURCE_IS_ENUM__
const gchar *qmi_dms_time_source_get_string (QmiDmsTimeSource val);
#endif

#if defined __QMI_DMS_TIME_SOURCE_IS_FLAGS__
gchar *qmi_dms_time_source_build_string_from_mask (QmiDmsTimeSource mask);
#endif

GType qmi_dms_activation_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_ACTIVATION_STATE (qmi_dms_activation_state_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_ACTIVATION_STATE_IS_ENUM__

#if defined __QMI_DMS_ACTIVATION_STATE_IS_ENUM__
const gchar *qmi_dms_activation_state_get_string (QmiDmsActivationState val);
#endif

#if defined __QMI_DMS_ACTIVATION_STATE_IS_FLAGS__
gchar *qmi_dms_activation_state_build_string_from_mask (QmiDmsActivationState mask);
#endif

GType qmi_dms_uim_facility_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_UIM_FACILITY (qmi_dms_uim_facility_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_UIM_FACILITY_IS_ENUM__

#if defined __QMI_DMS_UIM_FACILITY_IS_ENUM__
const gchar *qmi_dms_uim_facility_get_string (QmiDmsUimFacility val);
#endif

#if defined __QMI_DMS_UIM_FACILITY_IS_FLAGS__
gchar *qmi_dms_uim_facility_build_string_from_mask (QmiDmsUimFacility mask);
#endif

GType qmi_dms_uim_facility_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_UIM_FACILITY_STATE (qmi_dms_uim_facility_state_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_UIM_FACILITY_STATE_IS_ENUM__

#if defined __QMI_DMS_UIM_FACILITY_STATE_IS_ENUM__
const gchar *qmi_dms_uim_facility_state_get_string (QmiDmsUimFacilityState val);
#endif

#if defined __QMI_DMS_UIM_FACILITY_STATE_IS_FLAGS__
gchar *qmi_dms_uim_facility_state_build_string_from_mask (QmiDmsUimFacilityState mask);
#endif

GType qmi_dms_uim_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_UIM_STATE (qmi_dms_uim_state_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_UIM_STATE_IS_ENUM__

#if defined __QMI_DMS_UIM_STATE_IS_ENUM__
const gchar *qmi_dms_uim_state_get_string (QmiDmsUimState val);
#endif

#if defined __QMI_DMS_UIM_STATE_IS_FLAGS__
gchar *qmi_dms_uim_state_build_string_from_mask (QmiDmsUimState mask);
#endif

GType qmi_dms_time_reference_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_TIME_REFERENCE_TYPE (qmi_dms_time_reference_type_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_TIME_REFERENCE_TYPE_IS_ENUM__

#if defined __QMI_DMS_TIME_REFERENCE_TYPE_IS_ENUM__
const gchar *qmi_dms_time_reference_type_get_string (QmiDmsTimeReferenceType val);
#endif

#if defined __QMI_DMS_TIME_REFERENCE_TYPE_IS_FLAGS__
gchar *qmi_dms_time_reference_type_build_string_from_mask (QmiDmsTimeReferenceType mask);
#endif

GType qmi_dms_firmware_image_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_DMS_FIRMWARE_IMAGE_TYPE (qmi_dms_firmware_image_type_get_type ())

/* Define type-specific symbols */
#define __QMI_DMS_FIRMWARE_IMAGE_TYPE_IS_ENUM__

#if defined __QMI_DMS_FIRMWARE_IMAGE_TYPE_IS_ENUM__
const gchar *qmi_dms_firmware_image_type_get_string (QmiDmsFirmwareImageType val);
#endif

#if defined __QMI_DMS_FIRMWARE_IMAGE_TYPE_IS_FLAGS__
gchar *qmi_dms_firmware_image_type_build_string_from_mask (QmiDmsFirmwareImageType mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-nas.h" */
GType qmi_nas_radio_interface_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_RADIO_INTERFACE (qmi_nas_radio_interface_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_RADIO_INTERFACE_IS_ENUM__

#if defined __QMI_NAS_RADIO_INTERFACE_IS_ENUM__
const gchar *qmi_nas_radio_interface_get_string (QmiNasRadioInterface val);
#endif

#if defined __QMI_NAS_RADIO_INTERFACE_IS_FLAGS__
gchar *qmi_nas_radio_interface_build_string_from_mask (QmiNasRadioInterface mask);
#endif

GType qmi_nas_active_band_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_ACTIVE_BAND (qmi_nas_active_band_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_ACTIVE_BAND_IS_ENUM__

#if defined __QMI_NAS_ACTIVE_BAND_IS_ENUM__
const gchar *qmi_nas_active_band_get_string (QmiNasActiveBand val);
#endif

#if defined __QMI_NAS_ACTIVE_BAND_IS_FLAGS__
gchar *qmi_nas_active_band_build_string_from_mask (QmiNasActiveBand mask);
#endif

GType qmi_nas_network_service_domain_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_SERVICE_DOMAIN (qmi_nas_network_service_domain_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_SERVICE_DOMAIN_IS_ENUM__

#if defined __QMI_NAS_NETWORK_SERVICE_DOMAIN_IS_ENUM__
const gchar *qmi_nas_network_service_domain_get_string (QmiNasNetworkServiceDomain val);
#endif

#if defined __QMI_NAS_NETWORK_SERVICE_DOMAIN_IS_FLAGS__
gchar *qmi_nas_network_service_domain_build_string_from_mask (QmiNasNetworkServiceDomain mask);
#endif

GType qmi_nas_evdo_sinr_level_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_EVDO_SINR_LEVEL (qmi_nas_evdo_sinr_level_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_EVDO_SINR_LEVEL_IS_ENUM__

#if defined __QMI_NAS_EVDO_SINR_LEVEL_IS_ENUM__
const gchar *qmi_nas_evdo_sinr_level_get_string (QmiNasEvdoSinrLevel val);
#endif

#if defined __QMI_NAS_EVDO_SINR_LEVEL_IS_FLAGS__
gchar *qmi_nas_evdo_sinr_level_build_string_from_mask (QmiNasEvdoSinrLevel mask);
#endif

GType qmi_nas_signal_strength_request_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_SIGNAL_STRENGTH_REQUEST (qmi_nas_signal_strength_request_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_SIGNAL_STRENGTH_REQUEST_IS_FLAGS__

#if defined __QMI_NAS_SIGNAL_STRENGTH_REQUEST_IS_ENUM__
const gchar *qmi_nas_signal_strength_request_get_string (QmiNasSignalStrengthRequest val);
#endif

#if defined __QMI_NAS_SIGNAL_STRENGTH_REQUEST_IS_FLAGS__
gchar *qmi_nas_signal_strength_request_build_string_from_mask (QmiNasSignalStrengthRequest mask);
#endif

GType qmi_nas_network_scan_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_SCAN_TYPE (qmi_nas_network_scan_type_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_SCAN_TYPE_IS_FLAGS__

#if defined __QMI_NAS_NETWORK_SCAN_TYPE_IS_ENUM__
const gchar *qmi_nas_network_scan_type_get_string (QmiNasNetworkScanType val);
#endif

#if defined __QMI_NAS_NETWORK_SCAN_TYPE_IS_FLAGS__
gchar *qmi_nas_network_scan_type_build_string_from_mask (QmiNasNetworkScanType mask);
#endif

GType qmi_nas_network_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_STATUS (qmi_nas_network_status_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_STATUS_IS_FLAGS__

#if defined __QMI_NAS_NETWORK_STATUS_IS_ENUM__
const gchar *qmi_nas_network_status_get_string (QmiNasNetworkStatus val);
#endif

#if defined __QMI_NAS_NETWORK_STATUS_IS_FLAGS__
gchar *qmi_nas_network_status_build_string_from_mask (QmiNasNetworkStatus mask);
#endif

GType qmi_nas_network_register_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_REGISTER_TYPE (qmi_nas_network_register_type_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_REGISTER_TYPE_IS_ENUM__

#if defined __QMI_NAS_NETWORK_REGISTER_TYPE_IS_ENUM__
const gchar *qmi_nas_network_register_type_get_string (QmiNasNetworkRegisterType val);
#endif

#if defined __QMI_NAS_NETWORK_REGISTER_TYPE_IS_FLAGS__
gchar *qmi_nas_network_register_type_build_string_from_mask (QmiNasNetworkRegisterType mask);
#endif

GType qmi_nas_registration_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_REGISTRATION_STATE (qmi_nas_registration_state_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_REGISTRATION_STATE_IS_ENUM__

#if defined __QMI_NAS_REGISTRATION_STATE_IS_ENUM__
const gchar *qmi_nas_registration_state_get_string (QmiNasRegistrationState val);
#endif

#if defined __QMI_NAS_REGISTRATION_STATE_IS_FLAGS__
gchar *qmi_nas_registration_state_build_string_from_mask (QmiNasRegistrationState mask);
#endif

GType qmi_nas_attach_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_ATTACH_STATE (qmi_nas_attach_state_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_ATTACH_STATE_IS_ENUM__

#if defined __QMI_NAS_ATTACH_STATE_IS_ENUM__
const gchar *qmi_nas_attach_state_get_string (QmiNasAttachState val);
#endif

#if defined __QMI_NAS_ATTACH_STATE_IS_FLAGS__
gchar *qmi_nas_attach_state_build_string_from_mask (QmiNasAttachState mask);
#endif

GType qmi_nas_network_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_TYPE (qmi_nas_network_type_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_TYPE_IS_ENUM__

#if defined __QMI_NAS_NETWORK_TYPE_IS_ENUM__
const gchar *qmi_nas_network_type_get_string (QmiNasNetworkType val);
#endif

#if defined __QMI_NAS_NETWORK_TYPE_IS_FLAGS__
gchar *qmi_nas_network_type_build_string_from_mask (QmiNasNetworkType mask);
#endif

GType qmi_nas_roaming_indicator_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_ROAMING_INDICATOR_STATUS (qmi_nas_roaming_indicator_status_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_ROAMING_INDICATOR_STATUS_IS_ENUM__

#if defined __QMI_NAS_ROAMING_INDICATOR_STATUS_IS_ENUM__
const gchar *qmi_nas_roaming_indicator_status_get_string (QmiNasRoamingIndicatorStatus val);
#endif

#if defined __QMI_NAS_ROAMING_INDICATOR_STATUS_IS_FLAGS__
gchar *qmi_nas_roaming_indicator_status_build_string_from_mask (QmiNasRoamingIndicatorStatus mask);
#endif

GType qmi_nas_data_capability_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_DATA_CAPABILITY (qmi_nas_data_capability_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_DATA_CAPABILITY_IS_ENUM__

#if defined __QMI_NAS_DATA_CAPABILITY_IS_ENUM__
const gchar *qmi_nas_data_capability_get_string (QmiNasDataCapability val);
#endif

#if defined __QMI_NAS_DATA_CAPABILITY_IS_FLAGS__
gchar *qmi_nas_data_capability_build_string_from_mask (QmiNasDataCapability mask);
#endif

GType qmi_nas_service_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_SERVICE_STATUS (qmi_nas_service_status_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_SERVICE_STATUS_IS_ENUM__

#if defined __QMI_NAS_SERVICE_STATUS_IS_ENUM__
const gchar *qmi_nas_service_status_get_string (QmiNasServiceStatus val);
#endif

#if defined __QMI_NAS_SERVICE_STATUS_IS_FLAGS__
gchar *qmi_nas_service_status_build_string_from_mask (QmiNasServiceStatus mask);
#endif

GType qmi_nas_hdr_personality_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_HDR_PERSONALITY (qmi_nas_hdr_personality_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_HDR_PERSONALITY_IS_ENUM__

#if defined __QMI_NAS_HDR_PERSONALITY_IS_ENUM__
const gchar *qmi_nas_hdr_personality_get_string (QmiNasHdrPersonality val);
#endif

#if defined __QMI_NAS_HDR_PERSONALITY_IS_FLAGS__
gchar *qmi_nas_hdr_personality_build_string_from_mask (QmiNasHdrPersonality mask);
#endif

GType qmi_nas_call_barring_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_CALL_BARRING_STATUS (qmi_nas_call_barring_status_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_CALL_BARRING_STATUS_IS_ENUM__

#if defined __QMI_NAS_CALL_BARRING_STATUS_IS_ENUM__
const gchar *qmi_nas_call_barring_status_get_string (QmiNasCallBarringStatus val);
#endif

#if defined __QMI_NAS_CALL_BARRING_STATUS_IS_FLAGS__
gchar *qmi_nas_call_barring_status_build_string_from_mask (QmiNasCallBarringStatus mask);
#endif

GType qmi_nas_network_description_display_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_DESCRIPTION_DISPLAY (qmi_nas_network_description_display_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_DESCRIPTION_DISPLAY_IS_ENUM__

#if defined __QMI_NAS_NETWORK_DESCRIPTION_DISPLAY_IS_ENUM__
const gchar *qmi_nas_network_description_display_get_string (QmiNasNetworkDescriptionDisplay val);
#endif

#if defined __QMI_NAS_NETWORK_DESCRIPTION_DISPLAY_IS_FLAGS__
gchar *qmi_nas_network_description_display_build_string_from_mask (QmiNasNetworkDescriptionDisplay mask);
#endif

GType qmi_nas_network_description_encoding_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_DESCRIPTION_ENCODING (qmi_nas_network_description_encoding_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_DESCRIPTION_ENCODING_IS_ENUM__

#if defined __QMI_NAS_NETWORK_DESCRIPTION_ENCODING_IS_ENUM__
const gchar *qmi_nas_network_description_encoding_get_string (QmiNasNetworkDescriptionEncoding val);
#endif

#if defined __QMI_NAS_NETWORK_DESCRIPTION_ENCODING_IS_FLAGS__
gchar *qmi_nas_network_description_encoding_build_string_from_mask (QmiNasNetworkDescriptionEncoding mask);
#endif

GType qmi_nas_radio_technology_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_RADIO_TECHNOLOGY_PREFERENCE (qmi_nas_radio_technology_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_RADIO_TECHNOLOGY_PREFERENCE_IS_FLAGS__

#if defined __QMI_NAS_RADIO_TECHNOLOGY_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_radio_technology_preference_get_string (QmiNasRadioTechnologyPreference val);
#endif

#if defined __QMI_NAS_RADIO_TECHNOLOGY_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_radio_technology_preference_build_string_from_mask (QmiNasRadioTechnologyPreference mask);
#endif

GType qmi_nas_preference_duration_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_PREFERENCE_DURATION (qmi_nas_preference_duration_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_PREFERENCE_DURATION_IS_ENUM__

#if defined __QMI_NAS_PREFERENCE_DURATION_IS_ENUM__
const gchar *qmi_nas_preference_duration_get_string (QmiNasPreferenceDuration val);
#endif

#if defined __QMI_NAS_PREFERENCE_DURATION_IS_FLAGS__
gchar *qmi_nas_preference_duration_build_string_from_mask (QmiNasPreferenceDuration mask);
#endif

GType qmi_nas_rat_mode_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_RAT_MODE_PREFERENCE (qmi_nas_rat_mode_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_RAT_MODE_PREFERENCE_IS_FLAGS__

#if defined __QMI_NAS_RAT_MODE_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_rat_mode_preference_get_string (QmiNasRatModePreference val);
#endif

#if defined __QMI_NAS_RAT_MODE_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_rat_mode_preference_build_string_from_mask (QmiNasRatModePreference mask);
#endif

GType qmi_nas_cdma_prl_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_CDMA_PRL_PREFERENCE (qmi_nas_cdma_prl_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_CDMA_PRL_PREFERENCE_IS_ENUM__

#if defined __QMI_NAS_CDMA_PRL_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_cdma_prl_preference_get_string (QmiNasCdmaPrlPreference val);
#endif

#if defined __QMI_NAS_CDMA_PRL_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_cdma_prl_preference_build_string_from_mask (QmiNasCdmaPrlPreference mask);
#endif

GType qmi_nas_roaming_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_ROAMING_PREFERENCE (qmi_nas_roaming_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_ROAMING_PREFERENCE_IS_ENUM__

#if defined __QMI_NAS_ROAMING_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_roaming_preference_get_string (QmiNasRoamingPreference val);
#endif

#if defined __QMI_NAS_ROAMING_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_roaming_preference_build_string_from_mask (QmiNasRoamingPreference mask);
#endif

GType qmi_nas_network_selection_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_NETWORK_SELECTION_PREFERENCE (qmi_nas_network_selection_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_NETWORK_SELECTION_PREFERENCE_IS_ENUM__

#if defined __QMI_NAS_NETWORK_SELECTION_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_network_selection_preference_get_string (QmiNasNetworkSelectionPreference val);
#endif

#if defined __QMI_NAS_NETWORK_SELECTION_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_network_selection_preference_build_string_from_mask (QmiNasNetworkSelectionPreference mask);
#endif

GType qmi_nas_change_duration_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_CHANGE_DURATION (qmi_nas_change_duration_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_CHANGE_DURATION_IS_ENUM__

#if defined __QMI_NAS_CHANGE_DURATION_IS_ENUM__
const gchar *qmi_nas_change_duration_get_string (QmiNasChangeDuration val);
#endif

#if defined __QMI_NAS_CHANGE_DURATION_IS_FLAGS__
gchar *qmi_nas_change_duration_build_string_from_mask (QmiNasChangeDuration mask);
#endif

GType qmi_nas_service_domain_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_SERVICE_DOMAIN_PREFERENCE (qmi_nas_service_domain_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_SERVICE_DOMAIN_PREFERENCE_IS_ENUM__

#if defined __QMI_NAS_SERVICE_DOMAIN_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_service_domain_preference_get_string (QmiNasServiceDomainPreference val);
#endif

#if defined __QMI_NAS_SERVICE_DOMAIN_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_service_domain_preference_build_string_from_mask (QmiNasServiceDomainPreference mask);
#endif

GType qmi_nas_gsm_wcdma_acquisition_order_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_GSM_WCDMA_ACQUISITION_ORDER_PREFERENCE (qmi_nas_gsm_wcdma_acquisition_order_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_GSM_WCDMA_ACQUISITION_ORDER_PREFERENCE_IS_ENUM__

#if defined __QMI_NAS_GSM_WCDMA_ACQUISITION_ORDER_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_gsm_wcdma_acquisition_order_preference_get_string (QmiNasGsmWcdmaAcquisitionOrderPreference val);
#endif

#if defined __QMI_NAS_GSM_WCDMA_ACQUISITION_ORDER_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_gsm_wcdma_acquisition_order_preference_build_string_from_mask (QmiNasGsmWcdmaAcquisitionOrderPreference mask);
#endif

GType qmi_nas_td_scdma_band_preference_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_TD_SCDMA_BAND_PREFERENCE (qmi_nas_td_scdma_band_preference_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_TD_SCDMA_BAND_PREFERENCE_IS_FLAGS__

#if defined __QMI_NAS_TD_SCDMA_BAND_PREFERENCE_IS_ENUM__
const gchar *qmi_nas_td_scdma_band_preference_get_string (QmiNasTdScdmaBandPreference val);
#endif

#if defined __QMI_NAS_TD_SCDMA_BAND_PREFERENCE_IS_FLAGS__
gchar *qmi_nas_td_scdma_band_preference_build_string_from_mask (QmiNasTdScdmaBandPreference mask);
#endif

GType qmi_nas_roaming_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_ROAMING_STATUS (qmi_nas_roaming_status_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_ROAMING_STATUS_IS_ENUM__

#if defined __QMI_NAS_ROAMING_STATUS_IS_ENUM__
const gchar *qmi_nas_roaming_status_get_string (QmiNasRoamingStatus val);
#endif

#if defined __QMI_NAS_ROAMING_STATUS_IS_FLAGS__
gchar *qmi_nas_roaming_status_build_string_from_mask (QmiNasRoamingStatus mask);
#endif

GType qmi_nas_hdr_protocol_revision_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_HDR_PROTOCOL_REVISION (qmi_nas_hdr_protocol_revision_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_HDR_PROTOCOL_REVISION_IS_ENUM__

#if defined __QMI_NAS_HDR_PROTOCOL_REVISION_IS_ENUM__
const gchar *qmi_nas_hdr_protocol_revision_get_string (QmiNasHdrProtocolRevision val);
#endif

#if defined __QMI_NAS_HDR_PROTOCOL_REVISION_IS_FLAGS__
gchar *qmi_nas_hdr_protocol_revision_build_string_from_mask (QmiNasHdrProtocolRevision mask);
#endif

GType qmi_nas_wcdma_hs_service_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_WCDMA_HS_SERVICE (qmi_nas_wcdma_hs_service_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_WCDMA_HS_SERVICE_IS_ENUM__

#if defined __QMI_NAS_WCDMA_HS_SERVICE_IS_ENUM__
const gchar *qmi_nas_wcdma_hs_service_get_string (QmiNasWcdmaHsService val);
#endif

#if defined __QMI_NAS_WCDMA_HS_SERVICE_IS_FLAGS__
gchar *qmi_nas_wcdma_hs_service_build_string_from_mask (QmiNasWcdmaHsService mask);
#endif

GType qmi_nas_cell_broadcast_capability_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_CELL_BROADCAST_CAPABILITY (qmi_nas_cell_broadcast_capability_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_CELL_BROADCAST_CAPABILITY_IS_ENUM__

#if defined __QMI_NAS_CELL_BROADCAST_CAPABILITY_IS_ENUM__
const gchar *qmi_nas_cell_broadcast_capability_get_string (QmiNasCellBroadcastCapability val);
#endif

#if defined __QMI_NAS_CELL_BROADCAST_CAPABILITY_IS_FLAGS__
gchar *qmi_nas_cell_broadcast_capability_build_string_from_mask (QmiNasCellBroadcastCapability mask);
#endif

GType qmi_nas_sim_reject_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_SIM_REJECT_STATE (qmi_nas_sim_reject_state_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_SIM_REJECT_STATE_IS_ENUM__

#if defined __QMI_NAS_SIM_REJECT_STATE_IS_ENUM__
const gchar *qmi_nas_sim_reject_state_get_string (QmiNasSimRejectState val);
#endif

#if defined __QMI_NAS_SIM_REJECT_STATE_IS_FLAGS__
gchar *qmi_nas_sim_reject_state_build_string_from_mask (QmiNasSimRejectState mask);
#endif

GType qmi_nas_cdma_pilot_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_CDMA_PILOT_TYPE (qmi_nas_cdma_pilot_type_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_CDMA_PILOT_TYPE_IS_ENUM__

#if defined __QMI_NAS_CDMA_PILOT_TYPE_IS_ENUM__
const gchar *qmi_nas_cdma_pilot_type_get_string (QmiNasCdmaPilotType val);
#endif

#if defined __QMI_NAS_CDMA_PILOT_TYPE_IS_FLAGS__
gchar *qmi_nas_cdma_pilot_type_build_string_from_mask (QmiNasCdmaPilotType mask);
#endif

GType qmi_nas_day_of_week_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_DAY_OF_WEEK (qmi_nas_day_of_week_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_DAY_OF_WEEK_IS_ENUM__

#if defined __QMI_NAS_DAY_OF_WEEK_IS_ENUM__
const gchar *qmi_nas_day_of_week_get_string (QmiNasDayOfWeek val);
#endif

#if defined __QMI_NAS_DAY_OF_WEEK_IS_FLAGS__
gchar *qmi_nas_day_of_week_build_string_from_mask (QmiNasDayOfWeek mask);
#endif

GType qmi_nas_daylight_savings_adjustment_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_NAS_DAYLIGHT_SAVINGS_ADJUSTMENT (qmi_nas_daylight_savings_adjustment_get_type ())

/* Define type-specific symbols */
#define __QMI_NAS_DAYLIGHT_SAVINGS_ADJUSTMENT_IS_ENUM__

#if defined __QMI_NAS_DAYLIGHT_SAVINGS_ADJUSTMENT_IS_ENUM__
const gchar *qmi_nas_daylight_savings_adjustment_get_string (QmiNasDaylightSavingsAdjustment val);
#endif

#if defined __QMI_NAS_DAYLIGHT_SAVINGS_ADJUSTMENT_IS_FLAGS__
gchar *qmi_nas_daylight_savings_adjustment_build_string_from_mask (QmiNasDaylightSavingsAdjustment mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-wms.h" */
GType qmi_wms_storage_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_STORAGE_TYPE (qmi_wms_storage_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_STORAGE_TYPE_IS_ENUM__

#if defined __QMI_WMS_STORAGE_TYPE_IS_ENUM__
const gchar *qmi_wms_storage_type_get_string (QmiWmsStorageType val);
#endif

#if defined __QMI_WMS_STORAGE_TYPE_IS_FLAGS__
gchar *qmi_wms_storage_type_build_string_from_mask (QmiWmsStorageType mask);
#endif

GType qmi_wms_ack_indicator_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_ACK_INDICATOR (qmi_wms_ack_indicator_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_ACK_INDICATOR_IS_ENUM__

#if defined __QMI_WMS_ACK_INDICATOR_IS_ENUM__
const gchar *qmi_wms_ack_indicator_get_string (QmiWmsAckIndicator val);
#endif

#if defined __QMI_WMS_ACK_INDICATOR_IS_FLAGS__
gchar *qmi_wms_ack_indicator_build_string_from_mask (QmiWmsAckIndicator mask);
#endif

GType qmi_wms_message_format_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_FORMAT (qmi_wms_message_format_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_FORMAT_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_FORMAT_IS_ENUM__
const gchar *qmi_wms_message_format_get_string (QmiWmsMessageFormat val);
#endif

#if defined __QMI_WMS_MESSAGE_FORMAT_IS_FLAGS__
gchar *qmi_wms_message_format_build_string_from_mask (QmiWmsMessageFormat mask);
#endif

GType qmi_wms_message_mode_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_MODE (qmi_wms_message_mode_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_MODE_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_MODE_IS_ENUM__
const gchar *qmi_wms_message_mode_get_string (QmiWmsMessageMode val);
#endif

#if defined __QMI_WMS_MESSAGE_MODE_IS_FLAGS__
gchar *qmi_wms_message_mode_build_string_from_mask (QmiWmsMessageMode mask);
#endif

GType qmi_wms_notification_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_NOTIFICATION_TYPE (qmi_wms_notification_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_NOTIFICATION_TYPE_IS_ENUM__

#if defined __QMI_WMS_NOTIFICATION_TYPE_IS_ENUM__
const gchar *qmi_wms_notification_type_get_string (QmiWmsNotificationType val);
#endif

#if defined __QMI_WMS_NOTIFICATION_TYPE_IS_FLAGS__
gchar *qmi_wms_notification_type_build_string_from_mask (QmiWmsNotificationType mask);
#endif

GType qmi_wms_cdma_service_option_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_CDMA_SERVICE_OPTION (qmi_wms_cdma_service_option_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_CDMA_SERVICE_OPTION_IS_ENUM__

#if defined __QMI_WMS_CDMA_SERVICE_OPTION_IS_ENUM__
const gchar *qmi_wms_cdma_service_option_get_string (QmiWmsCdmaServiceOption val);
#endif

#if defined __QMI_WMS_CDMA_SERVICE_OPTION_IS_FLAGS__
gchar *qmi_wms_cdma_service_option_build_string_from_mask (QmiWmsCdmaServiceOption mask);
#endif

GType qmi_wms_cdma_cause_code_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_CDMA_CAUSE_CODE (qmi_wms_cdma_cause_code_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_CDMA_CAUSE_CODE_IS_ENUM__

#if defined __QMI_WMS_CDMA_CAUSE_CODE_IS_ENUM__
const gchar *qmi_wms_cdma_cause_code_get_string (QmiWmsCdmaCauseCode val);
#endif

#if defined __QMI_WMS_CDMA_CAUSE_CODE_IS_FLAGS__
gchar *qmi_wms_cdma_cause_code_build_string_from_mask (QmiWmsCdmaCauseCode mask);
#endif

GType qmi_wms_cdma_error_class_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_CDMA_ERROR_CLASS (qmi_wms_cdma_error_class_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_CDMA_ERROR_CLASS_IS_ENUM__

#if defined __QMI_WMS_CDMA_ERROR_CLASS_IS_ENUM__
const gchar *qmi_wms_cdma_error_class_get_string (QmiWmsCdmaErrorClass val);
#endif

#if defined __QMI_WMS_CDMA_ERROR_CLASS_IS_FLAGS__
gchar *qmi_wms_cdma_error_class_build_string_from_mask (QmiWmsCdmaErrorClass mask);
#endif

GType qmi_wms_gsm_umts_rp_cause_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_GSM_UMTS_RP_CAUSE (qmi_wms_gsm_umts_rp_cause_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_GSM_UMTS_RP_CAUSE_IS_ENUM__

#if defined __QMI_WMS_GSM_UMTS_RP_CAUSE_IS_ENUM__
const gchar *qmi_wms_gsm_umts_rp_cause_get_string (QmiWmsGsmUmtsRpCause val);
#endif

#if defined __QMI_WMS_GSM_UMTS_RP_CAUSE_IS_FLAGS__
gchar *qmi_wms_gsm_umts_rp_cause_build_string_from_mask (QmiWmsGsmUmtsRpCause mask);
#endif

GType qmi_wms_gsm_umts_tp_cause_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_GSM_UMTS_TP_CAUSE (qmi_wms_gsm_umts_tp_cause_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_GSM_UMTS_TP_CAUSE_IS_ENUM__

#if defined __QMI_WMS_GSM_UMTS_TP_CAUSE_IS_ENUM__
const gchar *qmi_wms_gsm_umts_tp_cause_get_string (QmiWmsGsmUmtsTpCause val);
#endif

#if defined __QMI_WMS_GSM_UMTS_TP_CAUSE_IS_FLAGS__
gchar *qmi_wms_gsm_umts_tp_cause_build_string_from_mask (QmiWmsGsmUmtsTpCause mask);
#endif

GType qmi_wms_message_delivery_failure_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_DELIVERY_FAILURE_TYPE (qmi_wms_message_delivery_failure_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_DELIVERY_FAILURE_TYPE_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_DELIVERY_FAILURE_TYPE_IS_ENUM__
const gchar *qmi_wms_message_delivery_failure_type_get_string (QmiWmsMessageDeliveryFailureType val);
#endif

#if defined __QMI_WMS_MESSAGE_DELIVERY_FAILURE_TYPE_IS_FLAGS__
gchar *qmi_wms_message_delivery_failure_type_build_string_from_mask (QmiWmsMessageDeliveryFailureType mask);
#endif

GType qmi_wms_message_tag_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_TAG_TYPE (qmi_wms_message_tag_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_TAG_TYPE_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_TAG_TYPE_IS_ENUM__
const gchar *qmi_wms_message_tag_type_get_string (QmiWmsMessageTagType val);
#endif

#if defined __QMI_WMS_MESSAGE_TAG_TYPE_IS_FLAGS__
gchar *qmi_wms_message_tag_type_build_string_from_mask (QmiWmsMessageTagType mask);
#endif

GType qmi_wms_message_protocol_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_PROTOCOL (qmi_wms_message_protocol_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_PROTOCOL_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_PROTOCOL_IS_ENUM__
const gchar *qmi_wms_message_protocol_get_string (QmiWmsMessageProtocol val);
#endif

#if defined __QMI_WMS_MESSAGE_PROTOCOL_IS_FLAGS__
gchar *qmi_wms_message_protocol_build_string_from_mask (QmiWmsMessageProtocol mask);
#endif

GType qmi_wms_message_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_TYPE (qmi_wms_message_type_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_TYPE_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_TYPE_IS_ENUM__
const gchar *qmi_wms_message_type_get_string (QmiWmsMessageType val);
#endif

#if defined __QMI_WMS_MESSAGE_TYPE_IS_FLAGS__
gchar *qmi_wms_message_type_build_string_from_mask (QmiWmsMessageType mask);
#endif

GType qmi_wms_message_class_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_MESSAGE_CLASS (qmi_wms_message_class_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_MESSAGE_CLASS_IS_ENUM__

#if defined __QMI_WMS_MESSAGE_CLASS_IS_ENUM__
const gchar *qmi_wms_message_class_get_string (QmiWmsMessageClass val);
#endif

#if defined __QMI_WMS_MESSAGE_CLASS_IS_FLAGS__
gchar *qmi_wms_message_class_build_string_from_mask (QmiWmsMessageClass mask);
#endif

GType qmi_wms_receipt_action_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_RECEIPT_ACTION (qmi_wms_receipt_action_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_RECEIPT_ACTION_IS_ENUM__

#if defined __QMI_WMS_RECEIPT_ACTION_IS_ENUM__
const gchar *qmi_wms_receipt_action_get_string (QmiWmsReceiptAction val);
#endif

#if defined __QMI_WMS_RECEIPT_ACTION_IS_FLAGS__
gchar *qmi_wms_receipt_action_build_string_from_mask (QmiWmsReceiptAction mask);
#endif

GType qmi_wms_transfer_indication_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_WMS_TRANSFER_INDICATION (qmi_wms_transfer_indication_get_type ())

/* Define type-specific symbols */
#define __QMI_WMS_TRANSFER_INDICATION_IS_ENUM__

#if defined __QMI_WMS_TRANSFER_INDICATION_IS_ENUM__
const gchar *qmi_wms_transfer_indication_get_string (QmiWmsTransferIndication val);
#endif

#if defined __QMI_WMS_TRANSFER_INDICATION_IS_FLAGS__
gchar *qmi_wms_transfer_indication_build_string_from_mask (QmiWmsTransferIndication mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-pds.h" */
GType qmi_pds_operation_mode_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PDS_OPERATION_MODE (qmi_pds_operation_mode_get_type ())

/* Define type-specific symbols */
#define __QMI_PDS_OPERATION_MODE_IS_ENUM__

#if defined __QMI_PDS_OPERATION_MODE_IS_ENUM__
const gchar *qmi_pds_operation_mode_get_string (QmiPdsOperationMode val);
#endif

#if defined __QMI_PDS_OPERATION_MODE_IS_FLAGS__
gchar *qmi_pds_operation_mode_build_string_from_mask (QmiPdsOperationMode mask);
#endif

GType qmi_pds_position_session_status_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PDS_POSITION_SESSION_STATUS (qmi_pds_position_session_status_get_type ())

/* Define type-specific symbols */
#define __QMI_PDS_POSITION_SESSION_STATUS_IS_ENUM__

#if defined __QMI_PDS_POSITION_SESSION_STATUS_IS_ENUM__
const gchar *qmi_pds_position_session_status_get_string (QmiPdsPositionSessionStatus val);
#endif

#if defined __QMI_PDS_POSITION_SESSION_STATUS_IS_FLAGS__
gchar *qmi_pds_position_session_status_build_string_from_mask (QmiPdsPositionSessionStatus mask);
#endif

GType qmi_pds_data_valid_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PDS_DATA_VALID (qmi_pds_data_valid_get_type ())

/* Define type-specific symbols */
#define __QMI_PDS_DATA_VALID_IS_FLAGS__

#if defined __QMI_PDS_DATA_VALID_IS_ENUM__
const gchar *qmi_pds_data_valid_get_string (QmiPdsDataValid val);
#endif

#if defined __QMI_PDS_DATA_VALID_IS_FLAGS__
gchar *qmi_pds_data_valid_build_string_from_mask (QmiPdsDataValid mask);
#endif

GType qmi_pds_tracking_session_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PDS_TRACKING_SESSION_STATE (qmi_pds_tracking_session_state_get_type ())

/* Define type-specific symbols */
#define __QMI_PDS_TRACKING_SESSION_STATE_IS_ENUM__

#if defined __QMI_PDS_TRACKING_SESSION_STATE_IS_ENUM__
const gchar *qmi_pds_tracking_session_state_get_string (QmiPdsTrackingSessionState val);
#endif

#if defined __QMI_PDS_TRACKING_SESSION_STATE_IS_FLAGS__
gchar *qmi_pds_tracking_session_state_build_string_from_mask (QmiPdsTrackingSessionState mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-pbm.h" */
GType qmi_pbm_event_registration_flag_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PBM_EVENT_REGISTRATION_FLAG (qmi_pbm_event_registration_flag_get_type ())

/* Define type-specific symbols */
#define __QMI_PBM_EVENT_REGISTRATION_FLAG_IS_FLAGS__

#if defined __QMI_PBM_EVENT_REGISTRATION_FLAG_IS_ENUM__
const gchar *qmi_pbm_event_registration_flag_get_string (QmiPbmEventRegistrationFlag val);
#endif

#if defined __QMI_PBM_EVENT_REGISTRATION_FLAG_IS_FLAGS__
gchar *qmi_pbm_event_registration_flag_build_string_from_mask (QmiPbmEventRegistrationFlag mask);
#endif

GType qmi_pbm_phonebook_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PBM_PHONEBOOK_TYPE (qmi_pbm_phonebook_type_get_type ())

/* Define type-specific symbols */
#define __QMI_PBM_PHONEBOOK_TYPE_IS_FLAGS__

#if defined __QMI_PBM_PHONEBOOK_TYPE_IS_ENUM__
const gchar *qmi_pbm_phonebook_type_get_string (QmiPbmPhonebookType val);
#endif

#if defined __QMI_PBM_PHONEBOOK_TYPE_IS_FLAGS__
gchar *qmi_pbm_phonebook_type_build_string_from_mask (QmiPbmPhonebookType mask);
#endif

GType qmi_pbm_session_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_PBM_SESSION_TYPE (qmi_pbm_session_type_get_type ())

/* Define type-specific symbols */
#define __QMI_PBM_SESSION_TYPE_IS_ENUM__

#if defined __QMI_PBM_SESSION_TYPE_IS_ENUM__
const gchar *qmi_pbm_session_type_get_string (QmiPbmSessionType val);
#endif

#if defined __QMI_PBM_SESSION_TYPE_IS_FLAGS__
gchar *qmi_pbm_session_type_build_string_from_mask (QmiPbmSessionType mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-uim.h" */
GType qmi_uim_session_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_UIM_SESSION_TYPE (qmi_uim_session_type_get_type ())

/* Define type-specific symbols */
#define __QMI_UIM_SESSION_TYPE_IS_ENUM__

#if defined __QMI_UIM_SESSION_TYPE_IS_ENUM__
const gchar *qmi_uim_session_type_get_string (QmiUimSessionType val);
#endif

#if defined __QMI_UIM_SESSION_TYPE_IS_FLAGS__
gchar *qmi_uim_session_type_build_string_from_mask (QmiUimSessionType mask);
#endif

GType qmi_uim_file_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_UIM_FILE_TYPE (qmi_uim_file_type_get_type ())

/* Define type-specific symbols */
#define __QMI_UIM_FILE_TYPE_IS_ENUM__

#if defined __QMI_UIM_FILE_TYPE_IS_ENUM__
const gchar *qmi_uim_file_type_get_string (QmiUimFileType val);
#endif

#if defined __QMI_UIM_FILE_TYPE_IS_FLAGS__
gchar *qmi_uim_file_type_build_string_from_mask (QmiUimFileType mask);
#endif

GType qmi_uim_security_attribute_logic_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_UIM_SECURITY_ATTRIBUTE_LOGIC (qmi_uim_security_attribute_logic_get_type ())

/* Define type-specific symbols */
#define __QMI_UIM_SECURITY_ATTRIBUTE_LOGIC_IS_ENUM__

#if defined __QMI_UIM_SECURITY_ATTRIBUTE_LOGIC_IS_ENUM__
const gchar *qmi_uim_security_attribute_logic_get_string (QmiUimSecurityAttributeLogic val);
#endif

#if defined __QMI_UIM_SECURITY_ATTRIBUTE_LOGIC_IS_FLAGS__
gchar *qmi_uim_security_attribute_logic_build_string_from_mask (QmiUimSecurityAttributeLogic mask);
#endif

GType qmi_uim_security_attribute_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_UIM_SECURITY_ATTRIBUTE (qmi_uim_security_attribute_get_type ())

/* Define type-specific symbols */
#define __QMI_UIM_SECURITY_ATTRIBUTE_IS_FLAGS__

#if defined __QMI_UIM_SECURITY_ATTRIBUTE_IS_ENUM__
const gchar *qmi_uim_security_attribute_get_string (QmiUimSecurityAttribute val);
#endif

#if defined __QMI_UIM_SECURITY_ATTRIBUTE_IS_FLAGS__
gchar *qmi_uim_security_attribute_build_string_from_mask (QmiUimSecurityAttribute mask);
#endif


/* enumerations from "../../../src/libqmi-glib/qmi-enums-oma.h" */
GType qmi_oma_session_type_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_OMA_SESSION_TYPE (qmi_oma_session_type_get_type ())

/* Define type-specific symbols */
#define __QMI_OMA_SESSION_TYPE_IS_ENUM__

#if defined __QMI_OMA_SESSION_TYPE_IS_ENUM__
const gchar *qmi_oma_session_type_get_string (QmiOmaSessionType val);
#endif

#if defined __QMI_OMA_SESSION_TYPE_IS_FLAGS__
gchar *qmi_oma_session_type_build_string_from_mask (QmiOmaSessionType mask);
#endif

GType qmi_oma_session_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_OMA_SESSION_STATE (qmi_oma_session_state_get_type ())

/* Define type-specific symbols */
#define __QMI_OMA_SESSION_STATE_IS_ENUM__

#if defined __QMI_OMA_SESSION_STATE_IS_ENUM__
const gchar *qmi_oma_session_state_get_string (QmiOmaSessionState val);
#endif

#if defined __QMI_OMA_SESSION_STATE_IS_FLAGS__
gchar *qmi_oma_session_state_build_string_from_mask (QmiOmaSessionState mask);
#endif

GType qmi_oma_session_failed_reason_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_OMA_SESSION_FAILED_REASON (qmi_oma_session_failed_reason_get_type ())

/* Define type-specific symbols */
#define __QMI_OMA_SESSION_FAILED_REASON_IS_ENUM__

#if defined __QMI_OMA_SESSION_FAILED_REASON_IS_ENUM__
const gchar *qmi_oma_session_failed_reason_get_string (QmiOmaSessionFailedReason val);
#endif

#if defined __QMI_OMA_SESSION_FAILED_REASON_IS_FLAGS__
gchar *qmi_oma_session_failed_reason_build_string_from_mask (QmiOmaSessionFailedReason mask);
#endif

GType qmi_oma_hfa_feature_done_state_get_type (void) G_GNUC_CONST;
#define QMI_TYPE_OMA_HFA_FEATURE_DONE_STATE (qmi_oma_hfa_feature_done_state_get_type ())

/* Define type-specific symbols */
#define __QMI_OMA_HFA_FEATURE_DONE_STATE_IS_ENUM__

#if defined __QMI_OMA_HFA_FEATURE_DONE_STATE_IS_ENUM__
const gchar *qmi_oma_hfa_feature_done_state_get_string (QmiOmaHfaFeatureDoneState val);
#endif

#if defined __QMI_OMA_HFA_FEATURE_DONE_STATE_IS_FLAGS__
gchar *qmi_oma_hfa_feature_done_state_build_string_from_mask (QmiOmaHfaFeatureDoneState mask);
#endif

G_END_DECLS

#endif /* __LIBQMI_GLIB_ENUM_TYPES_H__ */

/* Generated data ends here */

