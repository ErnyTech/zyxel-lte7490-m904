#include "zcfg_common.h"
#include <json/json.h>
#include <netinet/in.h>
#include <ctype.h>
#include <regex.h>
#include "zcfg_rdm_oid.h"
#include "zcfg_fe_dal_common.h"
#include "zcfg_fe_dal.h"
#include <sys/types.h>
#include <pwd.h>

#define isdigit(c) ((c >= 0x30) && (c < 0x3A))
#define isEmptyIns(obj) json_object_get_boolean(json_object_object_get(obj, "emptyIns"))

#define DAL_LOGIN_LEVEL "DAL_LOGIN_LEVEL"
#define DAL_LOGIN_USERNAME "DAL_LOGIN_USERNAME"
#define DAL_LOGIN_TYPE "cmdtype"

bool printDebugMsg = false;

extern dal_param_t WAN_param[];
extern dal_param_t MGMT_param[];
extern dal_param_t ETHWANLAN_param[];
extern dal_param_t ADVANCE_param[];
extern dal_param_t WWANBACKUP_param[];
extern dal_param_t CELLWAN_param[];
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
extern dal_param_t CELLWAN_MAPN_param[];
#endif
extern dal_param_t CELLWAN_SIM_param[];
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
extern dal_param_t CELLWAN_ESIM_param[];
extern dal_param_t CELLWAN_ESIM_PROFILE_param[];
#endif
extern dal_param_t CELLWAN_BAND_param[];
extern dal_param_t CELLWAN_PLMN_param[];
extern dal_param_t CELLWAN_PSRU_param[];
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SMS
extern dal_param_t CELLWAN_SMS_param[];
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK
extern dal_param_t CELLWAN_LOCK_param[];
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK_NR
extern dal_param_t CELLWAN_LOCK_NR_param[];
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_QSCAN
extern dal_param_t CELLWAN_QSCAN_param[];
#endif
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_GNSS_LOCATION
extern dal_param_t CELLWAN_GNSS_param[];
#endif
extern dal_param_t CELLWAN_MGMT_param[];
extern dal_param_t CELLWAN_STATUS_param[];
extern dal_param_t CELLWAN_WAIT_STATE_param[];
extern dal_param_t ETH_CTL_param[];
extern dal_param_t DNS_ENTRY_param[];
extern dal_param_t D_DNS_param[];
extern dal_param_t STATICROUTE_param[];
extern dal_param_t DNSROUTE_param[];
extern dal_param_t POLICYROUTE_param[];
extern dal_param_t RIP_param[];
extern dal_param_t NatPortFwd_param[];
extern dal_param_t NAT_PORT_TRIGGERING_param[];
extern dal_param_t NAT_ADDR_MAPPING_param[];
extern dal_param_t NAT_CONF_param[];
#if defined(TELIA_CUSTOMIZATION) || defined(DNA_CUSTOMIZATION)
extern dal_param_t NAT_ENABLE_CONF_param[];
#endif
extern dal_param_t NAT_Application_param[];
extern dal_param_t IntfGrp_param[];
extern dal_param_t TunnelSet_param[];
extern dal_param_t LANADV_param[];

extern dal_param_t LANSETUP_param[];

#ifdef DHCPV6_RELAY_SUPPORT
extern dal_param_t DHCPv6Relay_param[];
#endif
#ifdef ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY
extern dal_param_t CheckPwdAgain_param[];
#endif
extern dal_param_t SUBNET_param[];
extern dal_param_t STATIC_DHCP_param[];
extern dal_param_t VLANGRP_param[];
extern dal_param_t IGMPMLD_param[];
extern dal_param_t FIREWALL_param[];
extern dal_param_t FIREWALL_PROTOCOL_param[];
extern dal_param_t FIREWALL_ACL_param[];
extern dal_param_t WIFI_GENERAL_param[];
extern dal_param_t MAC_Filter_param[];
extern dal_param_t QosGeneral_param[];
extern dal_param_t QosQueue_param[];
extern dal_param_t QosClass_param[];
extern dal_param_t QosShaper_param[];
extern dal_param_t QosPolicer_param[];
extern dal_param_t WIFI_MACFILTER_param[];
extern dal_param_t WIFI_WPS_param[];
extern dal_param_t WIFI_WMM_param[];
#ifdef BUILD_SONIQ
extern dal_param_t WIFI_SONIQ_param[];
#endif
extern dal_param_t WIFI_OTHERS_param[];
extern dal_param_t WLAN_SCHEDULER_param[];
extern dal_param_t WLAN_SCH_ACCESS_param[];
extern dal_param_t WLAN_SITE_SURVEY_param[];
extern dal_param_t SNMP_param[];
extern dal_param_t TIME_param[];
extern dal_param_t TRUST_DOMAIN_param[];
extern dal_param_t TRUST_DOMAIN_PASSTHRU_param[];
extern dal_param_t SP_TRUST_DOMAIN_param[];
#ifdef CONFIG_PPTP_SUPPORT
extern dal_param_t PPTP_param[];
#endif
#if BUILD_PACKAGE_ZyIMS
extern dal_param_t VOIP_LINE_param[];
extern dal_param_t VOIP_PROFILE_param[];
extern dal_param_t VOIP_REGION_param[];
extern dal_param_t VOIP_PHONEDEVICE_param[];
extern dal_param_t VOIP_PHONEDECT_param[];
extern dal_param_t VOIP_CALLRULE_param[];
extern dal_param_t VOIP_CALLBLOCK_param[];
extern dal_param_t VOIP_CALLHISTORY_param[];
extern dal_param_t VOIP_CALLSUMMARY_param[];
#endif
extern dal_param_t MAIL_NOTIFICATION_param[];
extern dal_param_t LOG_SETTING_param[];
extern dal_param_t PING_TEST_param[];
extern dal_param_t IEEE_8021AG_param[];
extern dal_param_t IEEE_8023AH_param[];
extern dal_param_t OAMPING_param[];
extern dal_param_t USB_FILESHARING_param[];
extern dal_param_t USB_SAMBA_param[];
extern dal_param_t USB_MEDIASERVER_param[];
extern dal_param_t USB_PRINTSERVER_param[];
extern dal_param_t PAREN_CTL_param[];
extern dal_param_t SCHEDULE_param[];
extern dal_param_t DEV_SEC_CERT_param[];
#ifdef ZYXEL_MQTT_PUBLISHER
extern dal_param_t DEV_MQTT_CLIENT_param[];
extern dal_param_t DEV_MQTT_CLIENT_PUBLISH_param[];
extern dal_param_t DEV_MQTT_NODE_param[];
#endif
#ifdef ZYXEL_L2TPV3
extern dal_param_t L2TPv3_param[];
#endif
#ifdef ZYXEL_NUTTCP
extern dal_param_t NUTTCP_param[];
#endif
extern dal_param_t HOME_CONNECTIVYITY_param[];
extern dal_param_t OUI_param[];
extern dal_param_t MGMT_SRV_param[];
extern dal_param_t MGMT_SRV_PASSTHRU_param[];
extern dal_param_t SP_MGMT_SRV_param[];
extern dal_param_t LOGIN_PRIVILEGE_param[];
extern dal_param_t QUICK_START_param[];
extern dal_param_t USER_ACCOUNT_param[];
extern dal_param_t NETWORK_MAP_param[];
extern dal_param_t SYS_LOG_param[];
extern dal_param_t Traffic_Status_param[];
extern dal_param_t VOIP_Status_param[];
//#ifdef CBT_CUSTOMIZATION // 20170711 Max Add
#ifdef ZYXEL_WEB_WIRELESS_NEW_VERSION_ONE
extern dal_param_t WIFI_CBT_param[];
extern dal_param_t WIFI_ADV_CBT_param[];
#endif
extern dal_param_t PORT_MIRROR_param[];
extern dal_param_t SENSOR_param[];
//#Support Online Firmware Upgrade for MTKSOC
#ifdef ZyXEL_Online_Firmware_Upgrade
extern dal_param_t ONLINE_FW_param[];
#endif
//#Support Online Module Upgrade for MTKSOC
#ifdef ZyXEL_Online_Module_Upgrade
extern dal_param_t ONLINE_Module_param[];
#endif
extern dal_param_t DEFAULT_SSID_param[];
extern dal_param_t DEFAULT_PSK_param[];
extern dal_param_t WIFI_CLIENT_LIST_param[];
#ifdef  CONFIG_MTKSOC_SCHEDULE_REBOOT_SUPPORT
extern dal_param_t SCH_REBOOT_param[];
#endif
#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
extern dal_param_t CELLWAN_MODULE_TEMP_param[];
#endif

//Bluetooth
#if defined(BLUETOOTH_SUPPORT)
extern dal_param_t BT_CTL_param[];
extern zcfgRet_t zcfgFeDalBT(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern void zcfgFeDalShowBT(struct json_object *Jarray);
#endif


extern zcfgRet_t zcfgFeDalWan(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMgmt(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalDnsEntry(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalDDns(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalEthWanLan(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWanAdvance(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWwanBackup(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWan(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
extern zcfgRet_t zcfgFeDalCellWanMApn(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalCellWanSim(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
extern zcfgRet_t zcfgFeDalCellWanESim(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWanESimProfile(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalCellWanBand(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWanPlmn(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWanPsru(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SMS
extern zcfgRet_t zcfgFeDalCellWanSms(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK
extern zcfgRet_t zcfgFeDalCellWanLock(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK_NR
extern zcfgRet_t zcfgFeDalCellWanNrLock(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_QSCAN
extern zcfgRet_t zcfgFeDalCellWanQscan(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_GNSS_LOCATION
extern zcfgRet_t zcfgFeDalCellWanGnss(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalCellWanMgmt(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWanStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCellWanWaitState(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalEthCtl(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalStaticRoute(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalDnsRoute(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalPolicyRoute(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalRip(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalNatPortMapping(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalNatPortTriggering(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalNatConf(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#if defined(TELIA_CUSTOMIZATION) || defined(DNA_CUSTOMIZATION)
extern zcfgRet_t zcfgFeDalNatEnableConf(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalNatAddrMapping(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalNatApplication(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalIntrGrp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalTunnelSetting(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalLanAdvance(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalLanSetup(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef DHCPV6_RELAY_SUPPORT
extern zcfgRet_t zcfgFeDalDhcpv6Relay(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#ifdef ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY
extern zcfgRet_t zcfgFeDalCheckPwdAgain(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalAddnlSubnet(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalStaticDHCP(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVlanGroup(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalIGMPMLD(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalFirewall(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalFirewallProtocol(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalFirewallACL(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiGeneral(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMACFilter(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQosGeneral(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQosQueue(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQosClass(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQosShaper(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQosPolicer(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiMACFilter(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiWps(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiWmm(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef BUILD_SONIQ
extern zcfgRet_t zcfgFeDalWifiSoniq(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalWlanScheduler(const char *method, struct json_object *Jobj, char *replyMsg);
extern zcfgRet_t zcfgFeDalWlanSchAccess(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWlanSiteSurvey(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiOthers(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalSNMP(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalTime(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalTrustDomain(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalTrustDomainPassthru(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalSpTrustDomain(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef CONFIG_PPTP_SUPPORT
extern zcfgRet_t zcfgFeDalPptp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#if BUILD_PACKAGE_ZyIMS
extern zcfgRet_t zcfgFeDalVoipLine(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipProfile(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipPhoneRegion(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipPhoneDevice(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipPhoneDECT(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipCallRule(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipCallBlock(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipCallHistory(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoipCallSummary(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalMailNotification(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalLogSetting(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalSysLog(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalPingTest(const char *method, struct json_object *Jobj,struct json_object *Jarray,  char *replyMsg);
extern zcfgRet_t zcfgFe8021ag(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFe8023ah(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeOamPing(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalUsbFilesharing(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalUsbSamba(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalUsbMediaserver(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalUsbPrintserver(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalParentCtl(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalSchedule(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalDevSecCert(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef ZYXEL_MQTT_PUBLISHER
extern zcfgRet_t zcfgFeDalMQTT(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMQTTNode(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMQTTClientPublish(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#ifdef ZYXEL_L2TPV3
extern zcfgRet_t zcfgFeDalL2TPv3(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalHomeConnectivity(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDaloui(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMgmtSrv(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalMgmtSrvPassthru(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef TELIA_CUSTOMIZATION
extern zcfgRet_t zcfgFeDalMgmtSrvTelnetOnly(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalSPMgmtSrv(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalLoginPrivilege(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalQuickStart(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalUserAccount(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalNetworkMAP(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalCardPageStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalLanPortInfo(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
//#ifdef CBT_CUSTOMIZATION // 20170711 Max Add
#ifdef ZYXEL_WEB_WIRELESS_NEW_VERSION_ONE
extern zcfgRet_t zcfgFeDalCbtWifi(const char *method, struct json_object *Jobj, char *replyMsg);
extern zcfgRet_t zcfgFeDalCbtWifiAdv(const char *method, struct json_object *Jobj, char *replyMsg);
#endif
extern zcfgRet_t zcfgFeDalTrafficStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalVoIPStatus(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalPortMirror(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalSensor(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);


//#Support Online Firmware Upgrade for MTKSOC
#ifdef ZyXEL_Online_Firmware_Upgrade
extern zcfgRet_t zcfgFeDalOnlineFWInfo(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg);
#endif
//#Support Online Module Upgrade for MTKSOC
#ifdef ZyXEL_Online_Module_Upgrade
extern zcfgRet_t zcfgFeDalOnlineModuleInfo(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg);
#endif

extern zcfgRet_t zcfgFeDalDefaultSsid(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalDefaultPsk(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
extern zcfgRet_t zcfgFeDalWifiClientList(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#ifdef CONFIG_MTKSOC_SCHEDULE_REBOOT_SUPPORT
extern zcfgRet_t zcfgFeDalSchReboot(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
extern zcfgRet_t zcfgFeDalCellWanModuleTemp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif

extern void zcfgFeDalShowStaticRoute(struct json_object *Jarray);
extern void zcfgFeDalShowRip(struct json_object *Jarray);
extern void zcfgFeDalShowMgmtSrv(struct json_object *Jarray);
#ifdef TELIA_CUSTOMIZATION
extern void zcfgFeDalShowMgmtSrvTelnetOnly(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowMgmtSrvPassthru(struct json_object *Jarray);
extern void zcfgFeDalShowSPMgmtSrv(struct json_object *Jarray);
extern void zcfgFeDalShowPortFwd(struct json_object *Jarray);
extern void zcfgFeDalShowPortTrigger(struct json_object *Jarray);
extern void zcfgFeDalShowAddrMapping(struct json_object *Jarray);
extern void zcfgFeDalShowNatConf(struct json_object *Jarray);
#if defined(TELIA_CUSTOMIZATION) || defined(DNA_CUSTOMIZATION)
extern void zcfgFeDalShowNatEnableConf(struct json_object *Jarray);
#endif
#ifdef ZYXEL_MQTT_PUBLISHER
extern void zcfgFeDalShowMQTT(struct json_object *Jarray);
extern void zcfgFeDalShowMQTTNode(struct json_object *Jarray);
extern void zcfgFeDalShowMQTTClientPublish(struct json_object *Jarray);
#endif
#ifdef ZYXEL_L2TPV3
extern void zcfgFeDalShowL2TPv3(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowNetworkMAP(struct json_object *Jarray);
extern void zcfgFeDalShowEthCtl(struct json_object *Jarray);
extern void zcfgFeDalShowFirewallACL(struct json_object *Jarray);
extern void zcfgFeDalShowWifiGeneral(struct json_object *Jarray);
extern void zcfgFeDalShowWan(struct json_object *Jarray);
extern void zcfgFeDalShowDDns(struct json_object *Jarray);
extern void zcfgFeDalShowIntrGrp(struct json_object *Jarray);
extern void zcfgFeDalShowTunnelSetting(struct json_object *Jarray);
extern void zcfgFeDalShowStaticDHCP(struct json_object *Jarray);
extern void zcfgFeDalShowDnsEntry(struct json_object *Jarray);
extern void zcfgFeDalShowIGMPMLD(struct json_object *Jarray);
extern void zcfgFeDalShowLanAdv(struct json_object *Jarray);
extern void zcfgFeDalShowAddnl(struct json_object *Jarray);
extern void zcfgFeDalShowUsbFilesharing(struct json_object *Jarray);
extern void zcfgFeDalShowUsbSamba(struct json_object *Jarray);
extern void zcfgFeDalShowVlanGroup(struct json_object *Jarray);
extern void zcfgFeDalShowMACFilter(struct json_object *Jarray);
extern void zcfgFeDalShowFirewall(struct json_object *Jarray);
extern void zcfgFeDalShowHomeConnect(struct json_object *Jarray);
extern void zcfgFeDalShowoui(struct json_object *Jarray);
extern void zcfgFeDalShowWanAdvance(struct json_object *Jarray);
extern void zcfgFeDalShowSNMP(struct json_object *Jarray);
extern void zcfgFeDalShowPolicyRoute(struct json_object *Jarray);
extern void zcfgFeDalShowFirewallDos(struct json_object *Jarray);
extern void zcfgFeDalShowDnsRoute(struct json_object *Jarray);
extern void zcfgFeDalShowVoipCallRule(struct json_object *Jarray);
extern void zcfgFeDalShowVoipRegion(struct json_object *Jarray);
extern void zcfgFeDalShowVoipPhoneDevice(struct json_object *Jarray);
extern void zcfgFeDalShowVoipCallHistory(struct json_object *Jarray);
extern void zcfgFeDalShowVoipCallSummary(struct json_object *Jarray);
extern void zcfgFeDalShowFirewallProtocol(struct json_object *Jarray);
extern void zcfgFeDalShowVoipLine(struct json_object *Jarray);
extern void zcfgFeDalShowVoipProfile(struct json_object *Jarray);
extern void zcfgFeDalShowSchedule(struct json_object *Jarray);
extern void zcfgFeDalShowMgmt(struct json_object *Jarray);
extern void zcfgFeDalShowUsbMediaserver(struct json_object *Jarray);
extern void zcfgFeDalShowWifiWps(struct json_object *Jarray);
extern void zcfgFeDalShowUserAccount(struct json_object *Jarray);
extern void zcfgFeDalShowTrustDomain(struct json_object *Jarray);
extern void zcfgFeDalShowTrustDomainPassthru(struct json_object *Jarray);
extern void zcfgFeDalShowSpTrustDomain(struct json_object *Jarray);
extern void zcfgFeDalShowQosGeneral(struct json_object *Jarray);
extern void zcfgFeDalShowQosShaper(struct json_object *Jarray);
extern void zcfgFeDalShowQosQueue(struct json_object *Jarray);
extern void zcfgFeDalShowQosPolicer(struct json_object *Jarray);
extern void zcfgFeDalShowQosClass(struct json_object *Jarray);
extern void zcfgFeDalShowWwanBackup(struct json_object *Jarray);
extern void zcfgFeDalShowCellWan(struct json_object *Jarray);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
extern void zcfgFeDalShowCellWanMApn(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowCellWanSim(struct json_object *Jarray);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
extern void zcfgFeDalShowCellWanESim(struct json_object *Jarray);
extern void zcfgFeDalShowCellWanESimProfile(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowCellWanBand(struct json_object *Jarray);
extern void zcfgFeDalShowCellWanPlmn(struct json_object *Jarray);
extern void zcfgFeDalShowCellWanPsru(struct json_object *Jarray);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK
extern void zcfgFeDalShowCellWanLock(struct json_object *Jarray);
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK_NR
extern void zcfgFeDalShowCellWanNrLock(struct json_object *Jarray);
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_QSCAN
extern void zcfgFeDalShowCellWanQscan(struct json_object *Jarray);
#endif
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_GNSS_LOCATION
extern void zcfgFeDalShowCellWanGnss(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowCellWanMgmt(struct json_object *Jarray);
extern void zcfgFeDalShowCellWanStatus(struct json_object *Jarray);
extern void zcfgFeDalShowCellWanWaitState(struct json_object *Jarray);
extern void zcfgFeDalShowMailNotification(struct json_object *Jarray);
extern void zcfgFeDalShowLogSetting(struct json_object *Jarray);
extern void zcfgFeDalShowWifiWmm(struct json_object *Jarray);
extern void zcfgFeDalShowLanSetup(struct json_object *Jarray);

#ifdef DHCPV6_RELAY_SUPPORT
extern void zcfgFeDalShowDhcpv6Relay(struct json_object *Jarray);
#endif
#ifdef ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY
extern void zcfgFeDalShowCheckPwdAgain(struct json_object *Jarray);
#endif
extern void zcfgFeDalShowParentCtl(struct json_object *Jarray);
extern void zcfgFeDalShowTime(struct json_object *Jarray);
extern void zcfgFeDalShowSysLog(struct json_object *Jarray);
extern void zcfgFeDalShowWifiMACFilter(struct json_object *Jarray);
extern void zcfgFeDalShowWifiOthers(struct json_object *Jarray);
extern void zcfgFeDalShowEthWanLan(struct json_object *Jarray);
extern void zcfgFeDalShowWlanScheduler(struct json_object *Jarray);
extern void zcfgFeDalShowWlanSchAccess(struct json_object *Jarray);
extern void zcfgFeDalShowWlanSiteSurvey(struct json_object *Jarray);
extern void zcfgFeDalShowPORT_MIRROR(struct json_object *Jarray);
extern void zcfgFeDalShowDefaultSsid(struct json_object *Jarray);
extern void zcfgFeDalShowDefaultPsk(struct json_object *Jarray);
extern void zcfgFeDalShowWifiClientList(struct json_object *Jarray);
#ifdef CONFIG_MTKSOC_SCHEDULE_REBOOT_SUPPORT
extern void zcfgFeDalShowSchReboot(struct json_object *Jarray);
#endif
#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
extern void zcfgFeDalShowCellWanModuleTemp(struct json_object *Jarray);
#endif
#ifdef ZYXEL_NUTTCP
extern void zcfgFeDalNutTcpShow(struct json_object *Jarray);
extern zcfgRet_t zcfgFeDalNutTcp(const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
#endif
dal_handler_t dalHandler[] = {
{"wan",				WAN_param,			zcfgFeDalWan,			zcfgFeDalShowWan,			"get|add|edit|delete",	"Broadband"},
{"tr69",			MGMT_param,			zcfgFeDalMgmt,			zcfgFeDalShowMgmt,			"get|edit",			"TR-069_Client"},
{"ethwanlan", 		ETHWANLAN_param, 	zcfgFeDalEthWanLan,		zcfgFeDalShowEthWanLan,		"get|edit",				"Broadband"},
{"dns",				DNS_ENTRY_param,	zcfgFeDalDnsEntry,		zcfgFeDalShowDnsEntry,		"get|add|edit|delete",	"DNS"}, //DNS
{"ddns",			D_DNS_param,		zcfgFeDalDDns,			zcfgFeDalShowDDns,			"get|edit",			"DNS"},
{"wan_adv",			ADVANCE_param,		zcfgFeDalWanAdvance,	zcfgFeDalShowWanAdvance,	"get|edit",				"Broadband"}, //Broadband
{"cellular", 		WWANBACKUP_param,	zcfgFeDalWwanBackup,	zcfgFeDalShowWwanBackup,	"get|edit",				"Broadband"}, //Broadband
#if defined(BLUETOOTH_SUPPORT)
{"bluetooth",                      BT_CTL_param,          zcfgFeDalBT,                zcfgFeDalShowBT,            "get|edit",                             "Bluetooth"}, //bluetooth command
#endif
{"ethctl",			ETH_CTL_param,		zcfgFeDalEthCtl,		zcfgFeDalShowEthCtl,		"get|edit",				"ethctl"}, //ETHCTL command
{"static_route", 	STATICROUTE_param,	zcfgFeDalStaticRoute,	zcfgFeDalShowStaticRoute,	"get|add|edit|delete",	"Routing"},
{"dns_route", 		DNSROUTE_param,		zcfgFeDalDnsRoute,		zcfgFeDalShowDnsRoute,		"get|add|edit|delete",	"Routing"}, //Routing
{"policy_route", 	POLICYROUTE_param,	zcfgFeDalPolicyRoute,	zcfgFeDalShowPolicyRoute,	"get|add|edit|delete",	"Routing"}, //Routing
{"rip", 			RIP_param,			zcfgFeDalRip,			zcfgFeDalShowRip,			"get|edit",				"Routing"}, //Routing
{"nat",				NatPortFwd_param,	zcfgFeDalNatPortMapping,zcfgFeDalShowPortFwd,		"get|add|edit|delete", "NAT"},
{"nat_trigger",NAT_PORT_TRIGGERING_param,zcfgFeDalNatPortTriggering,zcfgFeDalShowPortTrigger,			"get|add|edit|delete", 	"NAT"}, //NAT
{"nat_addr_map",NAT_ADDR_MAPPING_param,	zcfgFeDalNatAddrMapping,zcfgFeDalShowAddrMapping,	"get|add|edit|delete",	 "NAT"}, //NAT
{"NAT_APPLICATION",NAT_Application_param,	zcfgFeDalNatApplication,NULL,					"add|delete",		""}, //NAT
{"nat_conf",		NAT_CONF_param,		zcfgFeDalNatConf, 		zcfgFeDalShowNatConf,		"edit|get",				"NAT"}, //NAT
#if defined(TELIA_CUSTOMIZATION) || defined(DNA_CUSTOMIZATION)
{"nat_enable_conf",		NAT_ENABLE_CONF_param,		zcfgFeDalNatEnableConf, 		zcfgFeDalShowNatEnableConf,		"edit|get",				"NAT"}, //NAT
#endif
{"intf_group",		IntfGrp_param,		zcfgFeDalIntrGrp,		zcfgFeDalShowIntrGrp,		"add|edit|delete|get",	"Interface_Grouping"},//
{"tunnel_setting",		TunnelSet_param,		zcfgFeDalTunnelSetting,		zcfgFeDalShowTunnelSetting,		"add|edit|delete|get",	"Tunnel_Setting"},//Tunnel_Setting
{"lanadv",			LANADV_param, 		zcfgFeDalLanAdvance,	zcfgFeDalShowLanAdv,		"edit|get",			"Home_Networking"},
{"lan",				LANSETUP_param, 	zcfgFeDalLanSetup,		zcfgFeDalShowLanSetup,		"get|edit", 			"Home_Networking"},

#ifdef DHCPV6_RELAY_SUPPORT
{"dh6r",				DHCPv6Relay_param, 	zcfgFeDalDhcpv6Relay,		zcfgFeDalShowDhcpv6Relay,		"add|get|delete|edit", 			"Home_Networking"},
#endif
#ifdef ZYXEL_RETYPE_LOGIN_PASSWORD_BEFORE_APPLY
{"pwd_check_again",				CheckPwdAgain_param, 	zcfgFeDalCheckPwdAgain,		zcfgFeDalShowCheckPwdAgain,		"add|get|delete|edit", 			"Home_Networking"},
#endif
{"ipalias",			SUBNET_param,		zcfgFeDalAddnlSubnet,	zcfgFeDalShowAddnl,			"get|edit" ,			"Home_Networking"},//Home_Networking
{"static_dhcp",		STATIC_DHCP_param,	zcfgFeDalStaticDHCP,	zcfgFeDalShowStaticDHCP,	"get|add|edit|delete",	"Home_Networking"},//Home_Networking
{"vlan_group", 		VLANGRP_param,		zcfgFeDalVlanGroup,		zcfgFeDalShowVlanGroup,		"get|add|edit|delete",	"Vlan Group"},//Vlan_Group
{"igmp_mld", 		IGMPMLD_param,		zcfgFeDalIGMPMLD,		zcfgFeDalShowIGMPMLD,		"get|edit",			"IGMP_MLD"},//IGMP_MLD
{"firewall",		FIREWALL_param, 	zcfgFeDalFirewall,		zcfgFeDalShowFirewall,		"get|edit",			"Firewall"}, //Firewall
{"firewall_proto",	FIREWALL_PROTOCOL_param, zcfgFeDalFirewallProtocol,zcfgFeDalShowFirewallProtocol,	"get|add|edit|delete", 	"Firewall"},//Firewall
{"firewall_acl", 	FIREWALL_ACL_param, zcfgFeDalFirewallACL,	zcfgFeDalShowFirewallACL,	"add|edit|delete|get",	"Firewall"},
{"wlan",			WIFI_GENERAL_param,	zcfgFeDalWifiGeneral,   zcfgFeDalShowWifiGeneral,	"edit|get",			"Wireless"},
{"macfilter",		MAC_Filter_param,	zcfgFeDalMACFilter,		zcfgFeDalShowMACFilter,		"get|add|edit|delete",	"MAC_Filter"},//MAC_Filter
{"qos",				QosGeneral_param,	zcfgFeDalQosGeneral,	zcfgFeDalShowQosGeneral,	"get|edit",	"QoS"},//QoS
{"qos_queue",		QosQueue_param,		zcfgFeDalQosQueue,		zcfgFeDalShowQosQueue,		"get|add|edit|delete",	"QoS"},//QoS
{"qos_class",		QosClass_param,		zcfgFeDalQosClass,		zcfgFeDalShowQosClass,	    "get|add|edit|delete",	"QoS"},//QoS
{"qos_shaper",		QosShaper_param,	zcfgFeDalQosShaper,		zcfgFeDalShowQosShaper,		"get|add|edit|delete",	"QoS"},//QoS
{"qos_policer",		QosPolicer_param,	zcfgFeDalQosPolicer,	zcfgFeDalShowQosPolicer,	"get|add|edit|delete",	"QoS"},//QoS
{"wifi_macfilter",	WIFI_MACFILTER_param,	zcfgFeDalWifiMACFilter,zcfgFeDalShowWifiMACFilter,"get|add|edit|delete",	"Wireless"},//
{"wps",				WIFI_WPS_param,		zcfgFeDalWifiWps,		zcfgFeDalShowWifiWps,		"edit|get",			"Wireless"},
{"wmm",				WIFI_WMM_param,		zcfgFeDalWifiWmm,		zcfgFeDalShowWifiWmm,		"edit|get",			"Wireless"},//
#ifdef BUILD_SONIQ
{"WIFI_SONIQ",		WIFI_SONIQ_param, 	zcfgFeDalWifiSoniq,		NULL,						"edit|get", 			"Wireless"},//Wireless
#endif
{"wifi_others",		WIFI_OTHERS_param,	zcfgFeDalWifiOthers,	zcfgFeDalShowWifiOthers,	"edit|get",				"Wireless"},//
{"wlan_scheduler",	WLAN_SCHEDULER_param, zcfgFeDalWlanScheduler,	zcfgFeDalShowWlanScheduler,	"add|edit|delete|get",	"Wireless"},
{"wlan_sch_access",	WLAN_SCH_ACCESS_param, zcfgFeDalWlanSchAccess,	zcfgFeDalShowWlanSchAccess, "edit|get",	"Wireless"},
{"wlan_site_survey",	WLAN_SITE_SURVEY_param, zcfgFeDalWlanSiteSurvey,	zcfgFeDalShowWlanSiteSurvey, "get",	"Wireless"},
{"snmp",			SNMP_param, 		zcfgFeDalSNMP,			zcfgFeDalShowSNMP,			"edit|get",				"SNMP"},
{"time",			TIME_param, 		zcfgFeDalTime,			zcfgFeDalShowTime,			"edit|get",				"Time"},//Time
{"trust_domain",	TRUST_DOMAIN_param, zcfgFeDalTrustDomain,	zcfgFeDalShowTrustDomain,	"get|add|delete",		"Remote_MGMT"},//Remote_MGMT
{"trust_domain_passthru",	TRUST_DOMAIN_PASSTHRU_param, zcfgFeDalTrustDomainPassthru,	zcfgFeDalShowTrustDomainPassthru,	"get|add|delete",	"Remote_MGMT"},//Remote_MGMT
#ifdef CONFIG_PPTP_SUPPORT
{"pptp",			PPTP_param,			zcfgFeDalPptp,			NULL,						"get|edit",				"PPTP"},
#endif
{"sp_trust_domain",	SP_TRUST_DOMAIN_param, zcfgFeDalSpTrustDomain, zcfgFeDalShowSpTrustDomain,	"get|add|delete",		"SP_Domain"},//SP_Domain
#if BUILD_PACKAGE_ZyIMS
{"sip_account",		VOIP_LINE_param,	zcfgFeDalVoipLine,		zcfgFeDalShowVoipLine,		"get|add|edit|delete", "SIP"},//SIP
{"sip_sp",			VOIP_PROFILE_param,	zcfgFeDalVoipProfile,	zcfgFeDalShowVoipProfile,	"get|add|edit|delete", "SIP"},//SIP
{"region",			VOIP_REGION_param,	zcfgFeDalVoipPhoneRegion,zcfgFeDalShowVoipRegion,	"get|edit",				"Phone"},//Phone
{"phone",		VOIP_PHONEDEVICE_param,	zcfgFeDalVoipPhoneDevice,zcfgFeDalShowVoipPhoneDevice,					"get|edit",			"Phone"},//Phone
{"PHONEDECT",		VOIP_PHONEDECT_param,zcfgFeDalVoipPhoneDECT,NULL,						"edit",				""},//Phone
{"speeddial",		VOIP_CALLRULE_param,zcfgFeDalVoipCallRule,	zcfgFeDalShowVoipCallRule,	"get|edit|delete",	"Call_Rule"},//Call_Rule
{"CALLBLOCK",		VOIP_CALLBLOCK_param,zcfgFeDalVoipCallBlock,NULL,						"edit",				""},//Call_Rule
{"call_history",	VOIP_CALLHISTORY_param,zcfgFeDalVoipCallHistory,	zcfgFeDalShowVoipCallHistory,	"get|delete",	"Call_Rule"},
{"call_summary",	VOIP_CALLSUMMARY_param,zcfgFeDalVoipCallSummary,	zcfgFeDalShowVoipCallSummary,	"get|delete",	"Call_Rule"},
#endif
{"email_ntfy",		MAIL_NOTIFICATION_param,zcfgFeDalMailNotification,zcfgFeDalShowMailNotification,	"add|delete|get", 		"Email_Notify"},//Email_Notify
{"logset",			LOG_SETTING_param,	zcfgFeDalLogSetting,	zcfgFeDalShowLogSetting,	"get|edit", 		"Log"}, //Log
{"sys_log", 		SYS_LOG_param, 		zcfgFeDalSysLog,		zcfgFeDalShowSysLog,		"get|delete", 		"Log"},
#ifdef ZYXEL_NUTTCP
{"nutcp",	NUTTCP_param,	zcfgFeDalNutTcp,	zcfgFeDalNutTcpShow,	"get|edit",	"NUTTCP"},
#endif
{"PINGTEST",		PING_TEST_param,	zcfgFeDalPingTest,		NULL,						"edit",				""},//
{"8021AG",			IEEE_8021AG_param,	zcfgFe8021ag,			NULL,						"edit",				""},//Diagnostic_Result
{"8023AH",			IEEE_8023AH_param,	zcfgFe8023ah,			NULL,						"edit",				""},//Diagnostic_Result
{"OAMPING", 		OAMPING_param,		zcfgFeOamPing,			NULL,						"edit",				""},//Diagnostic_Result
{"usb_info", 		USB_FILESHARING_param,	zcfgFeDalUsbFilesharing,zcfgFeDalShowUsbFilesharing,	"get|edit", 	"USB_Service"},//USB_Service
{"usb_filesharing",	USB_SAMBA_param,	zcfgFeDalUsbSamba,		zcfgFeDalShowUsbSamba,		"get|add|edit|delete",	"USB_Service"},//USB_Service
{"media_server", 	USB_MEDIASERVER_param,	zcfgFeDalUsbMediaserver,	zcfgFeDalShowUsbMediaserver,"edit|get", 	"USB_Service"},
{"print_server",	USB_PRINTSERVER_param,	zcfgFeDalUsbPrintserver,	NULL,				"edit|get", 			"USB_Service"},
{"paren_ctl", 		PAREN_CTL_param,	zcfgFeDalParentCtl,		zcfgFeDalShowParentCtl,						"get|add|edit|delete",	"Parental_Control"},//Parental_Control
{"scheduler",		SCHEDULE_param, 	zcfgFeDalSchedule,		zcfgFeDalShowSchedule,		"get|add|edit|delete",	"Scheduler_Rule"},
{"dev_sec_cert", 	DEV_SEC_CERT_param,	zcfgFeDalDevSecCert, 	NULL,						"get|add|edit|delete", 	""},//Certificates
#ifdef ZYXEL_MQTT_PUBLISHER
{"mqtt",	DEV_MQTT_NODE_param,	zcfgFeDalMQTTNode,	zcfgFeDalShowMQTTNode,	"get|edit","MQTT_Node"},
{"mqtt_client", 	DEV_MQTT_CLIENT_param,	zcfgFeDalMQTT, 	zcfgFeDalShowMQTT,						"get|add|edit|delete", 	"MQTT_Client"},//MQTT Client
{"mqtt_client_publish", 	DEV_MQTT_CLIENT_PUBLISH_param,	zcfgFeDalMQTTClientPublish, 	zcfgFeDalShowMQTTClientPublish,						"get|add|edit|delete", 	"MQTT_Client_Publish"},//MQTT_Client_Publish
#endif
#ifdef ZYXEL_L2TPV3
{"L2TPv3", 	L2TPv3_param,	zcfgFeDalL2TPv3, 	zcfgFeDalShowL2TPv3,						"get|edit", 	"L2TPv3"},
#endif
{"one_connect", 	HOME_CONNECTIVYITY_param, zcfgFeDalHomeConnectivity,zcfgFeDalShowHomeConnect, 	"get|edit", 		"Home_Connectivity"},//Home_Connectivity
{"oui",				OUI_param,		zcfgFeDaloui,		zcfgFeDalShowoui,		"get|edit",		"Home_Connectivity"},//Set one connect oui enable/disable, not for GUI
{"mgmt_srv", 		MGMT_SRV_param,	zcfgFeDalMgmtSrv,			zcfgFeDalShowMgmtSrv,		"edit|get",			"Remote_MGMT"},
{"mgmt_srv_passthru", 		MGMT_SRV_PASSTHRU_param,	zcfgFeDalMgmtSrvPassthru,			zcfgFeDalShowMgmtSrvPassthru,		"edit|get",			"Remote_MGMT"},
#ifdef TELIA_CUSTOMIZATION
{"mgmt_srv_telnetonly", 		MGMT_SRV_param,	zcfgFeDalMgmtSrvTelnetOnly,			zcfgFeDalShowMgmtSrvTelnetOnly,		"edit|get",			"Remote_MGMT"},
#endif
{"sp_mgmt_srv", 	SP_MGMT_SRV_param,	zcfgFeDalSPMgmtSrv,		zcfgFeDalShowSPMgmtSrv,		"edit|get",			"SP_Domain"},
{"cellwan", 		CELLWAN_param,		zcfgFeDalCellWan,		zcfgFeDalShowCellWan,		"get|edit",				"Broadband"},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_MULTI_APN
{"cellwan_mapn", 	CELLWAN_MAPN_param,	zcfgFeDalCellWanMApn,	zcfgFeDalShowCellWanMApn,	"get|edit",				"Broadband"},
#endif
{"cellwan_sim", 	CELLWAN_SIM_param,	zcfgFeDalCellWanSim,	zcfgFeDalShowCellWanSim,	"get|edit",				"Broadband"},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_ESIM
{"cellwan_esim",   		CELLWAN_ESIM_param,    		  zcfgFeDalCellWanESim,   		zcfgFeDalShowCellWanESim,    "get|edit",                         "Broadband"},
{"cellwan_esim_profile",        CELLWAN_ESIM_PROFILE_param,   	  zcfgFeDalCellWanESimProfile, 		zcfgFeDalShowCellWanESim,    "get|edit|delete",                  "Broadband"},
#endif
{"cellwan_band", 	CELLWAN_BAND_param,	zcfgFeDalCellWanBand,	zcfgFeDalShowCellWanBand,	"get|edit",				"Broadband"},
{"cellwan_plmn", 	CELLWAN_PLMN_param,	zcfgFeDalCellWanPlmn,	zcfgFeDalShowCellWanPlmn,	"get|edit",				"Broadband"},
{"cellwan_psru", 	CELLWAN_PSRU_param,	zcfgFeDalCellWanPsru,	zcfgFeDalShowCellWanPsru,	"get|edit",				"Broadband"},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SMS
{"cellwan_sms", 	CELLWAN_SMS_param,	zcfgFeDalCellWanSms,	NULL,						"",					""}, //temporarily GUI only
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK
{"cellwan_lock", 	CELLWAN_LOCK_param,	zcfgFeDalCellWanLock,	zcfgFeDalShowCellWanLock,	"get|add|edit|delete",			"Broadband"},
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_PCI_LOCK_NR
{"cellwan_nr_lock",        CELLWAN_LOCK_NR_param,     zcfgFeDalCellWanNrLock,   zcfgFeDalShowCellWanNrLock,       "get|edit",                  "Broadband"},
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_QSCAN
{"cellwan_qscan",        CELLWAN_QSCAN_param,     zcfgFeDalCellWanQscan,   zcfgFeDalShowCellWanQscan,       "get|edit",                  "Broadband"},
#endif
#endif
#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_GNSS_LOCATION
{"cellwan_gnss",        CELLWAN_GNSS_param,     zcfgFeDalCellWanGnss,   zcfgFeDalShowCellWanGnss,       "get|edit",				"Broadband"},
#endif
{"cellwan_mgmt",	CELLWAN_MGMT_param,	zcfgFeDalCellWanMgmt,	zcfgFeDalShowCellWanMgmt,	"get|edit",				"Remote_MGMT"},
{"cellwan_status", 	CELLWAN_STATUS_param,	zcfgFeDalCellWanStatus,	zcfgFeDalShowCellWanStatus,	"get",				"Status"},
{"cellwan_wait_state", 	CELLWAN_WAIT_STATE_param,	zcfgFeDalCellWanWaitState,	zcfgFeDalShowCellWanWaitState,	"get|edit",		"Status"},
{"login_privilege", LOGIN_PRIVILEGE_param,	zcfgFeDalLoginPrivilege,NULL, 					"edit|get",				""},//root_only
{"QUICKSTART", 		QUICK_START_param,	zcfgFeDalQuickStart,	NULL,						"edit",				""},//root_only
{"user_account",	USER_ACCOUNT_param, zcfgFeDalUserAccount,	zcfgFeDalShowUserAccount,	"get|add|edit|delete",	"User_Account"}, //temporarily root_only //User_Account
{"lanhosts",		NETWORK_MAP_param, zcfgFeDalNetworkMAP,	zcfgFeDalShowNetworkMAP,		"get",				"Status"},
{"status",			NETWORK_MAP_param, zcfgFeDalStatus, 		NULL,						"",					""}, //temporarily GUI only
{"cardpage_status", NETWORK_MAP_param, zcfgFeDalCardPageStatus, NULL,						"", 				""}, //temporarily cardpage only
{"LanPortInfo", 	NETWORK_MAP_param, zcfgFeDalLanPortInfo, 	NULL,						"", 				""},
//Monitor
{"Traffic_Status", Traffic_Status_param, zcfgFeDalTrafficStatus, 	NULL,						"get", 				""},
{"VOIP_Status", VOIP_Status_param, zcfgFeDalVoIPStatus, 	NULL,						"get", 				""},
{"port_mirror", PORT_MIRROR_param, zcfgFeDalPortMirror, 	zcfgFeDalShowPORT_MIRROR,						"edit|get",				"Status"},
{"sensor", 		SENSOR_param, zcfgFeDalSensor, 				NULL,						"edit", 			""},

//#ifdef CBT_CUSTOMIZATION // 20170711 Max Add
#ifdef ZYXEL_WEB_WIRELESS_NEW_VERSION_ONE
{"WIFI_CBT",		WIFI_CBT_param,		zcfgFeDalCbtWifi,		NULL,		"edit",		""},
{"WIFI_ADVANCED_CBT",		WIFI_ADV_CBT_param,		zcfgFeDalCbtWifiAdv,		NULL,		"edit",		""},
#endif
//#Support Online Firmware Upgrade for MTKSOC
#ifdef ZyXEL_Online_Firmware_Upgrade
{"online_fw_info",		ONLINE_FW_param,		zcfgFeDalOnlineFWInfo,		NULL,		"get",		""},
#endif
//#Support Online Module Upgrade for MTKSOC
#ifdef ZyXEL_Online_Module_Upgrade
{"online_module_info",		ONLINE_Module_param,		zcfgFeDalOnlineModuleInfo,		NULL,		"get",		""},
#endif
{"wlan_def_ssid", 	DEFAULT_SSID_param,	zcfgFeDalDefaultSsid,	zcfgFeDalShowDefaultSsid,	"get",				"Wireless"},
{"wlan_def_psk", 	DEFAULT_PSK_param,	zcfgFeDalDefaultPsk,	zcfgFeDalShowDefaultPsk,	"get",				"Wireless"},
{"wifi_client_list",	WIFI_CLIENT_LIST_param, zcfgFeDalWifiClientList,	zcfgFeDalShowWifiClientList, "get",	"Wireless"},
#ifdef CONFIG_MTKSOC_SCHEDULE_REBOOT_SUPPORT
{"schedule_reboot",    SCH_REBOOT_param, zcfgFeDalSchReboot,    zcfgFeDalShowSchReboot, "get|edit", "Remote_MGMT"},
#endif
#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
{"module_temperature",    CELLWAN_MODULE_TEMP_param, zcfgFeDalCellWanModuleTemp,    zcfgFeDalShowCellWanModuleTemp, "get", "Broadband"},
#endif
{NULL,				NULL,				NULL, 					NULL,						NULL,				 NULL}
};

dal_voip_region_t dalVoipRegion[] = {
{"AE",	"ARE-United_Arab_Emirates"},
{"AU",	"AUS-Austrailia"},
{"BE",	"BEL-Beligium"},
{"BR",	"BRA-Brazil"},
{"CH",	"CHE-Switzerland"},
{"CL",	"CHL-Chile"},
{"CN",	"CHN-China"},
{"CO",	"COL-Colombia"},
{"CZ",	"CZE-Czech"},
{"DE",	"DEU-Germany"},
{"DK",	"DNK-Denmark"},
{"ES",	"ESP-Spain"},
{"FI",	"FIN-Finland"},
{"FR",	"FRA-France"},
{"GB",	"GBR-UK"},
{"HR",	"HRV-Croatia"},
{"HU",	"HUN-Hungary"},
{"IN",	"IND-India"},
{"IT",	"ITA-Italy"},
{"JP",	"JPN-Japan"},
{"NL",	"NLD-Netherlands"},
{"NO",	"NOR-Norway"},
{"NZ",	"NZL-New_Zealand"},
{"PL",	"POL-Poland"},
{"SK",	"SVK-Slovakia"},
{"SI",	"SVN-Slovenia"},
{"SE",	"SWE-Sweden"},
{"TR",	"TUR-Turkey"},
{"TW",	"TWN-Taiwan"},
{"US",	"USA-United_States_of_America"},
{NULL,	NULL}
};

dal_firewall_protocol_t dalFirewallICMP6[] = {
{"1",		"0",		"1/No-Rute"},
{"1", 		"1",		"1/Communication-Prohibited"},
{"1", 		"3",		"1/Address-Unreachable"},
{"1", 		"4",		"1/Port-Unreachable"},
{"2", 		"0",		"2/Packet_too_Big"},
{"3", 		"0",		"3/Time_Exceeded"},
{"3", 		"1",		"3/Time_Exceeded"},
{"4", 		"0",		"4/Parameter_Problem"},
{"4", 		"1",		"4/Parameter_Problem"},
{"4", 		"2",		"4/Parameter_Problem"},
{"128", 	"-1",		"128/Echo_Request"},
{"129", 	"-1",		"129/Echo_Response"},
{"130", 	"-1",		"130/Listener_Query"},
{"131", 	"-1",		"131/Listener_Report"},
{"132", 	"-1",		"132/Listener_Done"},
{"143", 	"-1",		"143/Listener_Report_v2"},
{"133", 	"-1",		"133/Router_Solicitation"},
{"134", 	"-1",		"134/Router_Advertisement"},
{"135", 	"-1",		"135/Neighbor_Solicitation"},
{"136", 	"-1",		"136/Neighbor_Advertisement"},
{"137", 	"-1",		"137/Redirect"},
{NULL,		NULL,		NULL}
};

dal_time_zone_t dalTimeZone[] = {
{"(GMT-12:00) International Date Line West",				"International_Date_Line_West[GMT-12:00]",		"IDLW+12"},
{"(GMT-11:00) Midway Island, Samoa",						"Midway_Island,Samoa[GMT-11:00]",	"SST+11"},
{"(GMT-10:00) Hawaii",										"Hawaii[GMT-10:00]",	"HST+10"},
{"(GMT-09:00) Alaska",										"Alaska[GMT-09:00]",	"AKST+9AKDT"},
{"(GMT-08:00) Pacific Time, Tijuana",						"Pacific_Time,Tijuana[GMT-08:00]",	"PST+8PDT"},
{"(GMT-07:00) Arizona, Mazatlan",							"Arizona,Mazatlan[GMT-07:00]",	"MST+7"},
{"(GMT-07:00) Chihuahua",									"Chihuahua[GMT-07:00]",	"MST+7MDT"},
{"(GMT-07:00) Mountain Time",								"Mountain_Time[GMT-07:00]",	"MST+7MDT2"},
{"(GMT-06:00) Central America",								"Central_America[GMT-06:00]",	"CST+6CDT"},
{"(GMT-06:00) Central Time",								"Central_Time[GMT-06:00]",	"CST+6CDT2"},
{"(GMT-06:00) Guadalajara, Mexico City, Monterrey",			"Guadalajara,Mexico_City,Monterrey[GMT-06:00]",	"CST+6CDT3"},
{"(GMT-06:00) Saskatchewan",								"Saskatchewan[GMT-06:00]",	"CST+6"},
{"(GMT-05:00) Bogota, Lima, Quito",							"Bogota,Lima,Quito[GMT-05:00]",	"COT+5"},
{"(GMT-05:00) Eastern Time",								"Eastern_Time[GMT-05:00]",	"EST+5EDT"},
{"(GMT-05:00) Indiana",										"Indiana[GMT-05:00]",	"EST+5EDT2"},
{"(GMT-04:00) Atlantic Time",								"Atlantic_Time[GMT-04:00]",	"AST+4ADT"},
{"(GMT-04:00) Caracas, La Paz",								"Caracas,La_Paz[GMT-04:00]",	"BOT+4"},
{"(GMT-04:00) Santiago",									"Santiago[GMT-04:00]",	"CLT+4CLST"},
{"(GMT-04:00) Georgetown",									"Georgetown[GMT-04:00]",	"ART+4"},
{"(GMT-03:30) Newfoundland",								"Newfoundland[GMT-03:30]",	"NST+3:30NDT"},
{"(GMT-03:00) Brasilia",									"Brasilia[GMT-03:00]",	"BRT+3BRST"},
{"(GMT-03:00) Buenos Aires",								"Buenos_Aires[GMT-03:00]",	"ART+3"},
{"(GMT-03:00) Greenland",									"Greenland[GMT-03:00]",	"CGT+3"},
{"(GMT-02:00) Mid-Atlantic",								"Mid-Atlantic[GMT-02:00]",	"MAT+2"},
{"(GMT-01:00) Azores",										"Azores[GMT-01:00]",	"AZOT+1AZOST"},
{"(GMT-01:00) Cape Verde Is.",								"Cape_Verde_Is[GMT-01:00]",	"CVT+1"},
{"(GMT-00:00) Casablanca",									"Casablanca[GMT-00:00]",	"WET-0WEST"},
{"(GMT-00:00) Monrovia",									"Monrovia[GMT-00:00]",	"WET-0"},
{"(GMT-00:00) Greenwich Mean Time: Edinburgh, London",		"Greenwich_Mean_Time_Edinburgh,London[GMT-00:00]",	"GMT-0BST"},
{"(GMT-00:00) Greenwich Mean Time: Dublin",					"Greenwich_Mean_Time_Dublin[GMT-00:00]",	"GMT-0IST"},
{"(GMT-00:00) Lisbon",										"Lisbon[GMT-00:00]",	"WET-0WEST2"},
{"(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna",	"Amsterdam,Berlin,Bern,Rome,Stockholm,Vienna[GMT+01:00]",	"CET-1CEST"},
{"(GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague",	"Belgrade,Bratislava,Budapest,Ljubljana,Prague[GMT+01:00]",	"CET-1CEST2"},
{"(GMT+01:00) Brussels, Copenhagen, Madrid, Paris",			"Brussels,Copenhagen,Madrid,Paris[GMT+01:00]",	"CET-1CEST3"},
{"(GMT+01:00) Sarajevo, Skopje, Warsaw, Zagreb",			"Sarajevo,Skopje,Warsaw,Zagreb[GMT+01:00]",	"CET-1CEST4"},
{"(GMT+01:00) West Central Africa",							"West_Central_Africa[GMT+01:00]",	"WAT-1"},
{"(GMT+02:00) Athens, Istanbul, Minsk",						"Athens,Istanbul,Minsk[GMT+02:00]",	"EET-2EEST"},
{"(GMT+02:00) Bucharest",									"Bucharest[GMT+02:00]",	"EET-2EEST2"},
{"(GMT+02:00) Cairo",										"Cairo[GMT+02:00]",	"EET-2EEST3"},
{"(GMT+02:00) Harare, Pretoria",							"Harare,Pretoria[GMT+02:00]",	"CAT-2"},
{"(GMT+02:00) Pretoria",									"Pretoria[GMT+02:00]",	"SAST-2"},
{"(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius",		"Helsinki,Kyiv,Riga,Sofia,Tallinn,Vilnius[GMT+02:00]",	"EET-2EEST4"},
{"(GMT+02:00) Jerusalem",									"Jerusalem[GMT+02:00]",	"IST-2IDT"},
{"(GMT+03:00) Baghdad",										"Baghdad[GMT+03:00]",	"AST-3"},
{"(GMT+03:00) Kuwait, Riyadh",								"Kuwait,Riyadh[GMT+03:00]",	"AST-3-2"},
{"(GMT+03:00) Moscow, St. Petersburg, Volgograd",			"Moscow,St.Petersburg,Volgograd[GMT+03:00]",	"MSK-3MSD"},
{"(GMT+03:00) Nairobi",										"Nairobi[GMT+03:00]",	"EAT-3"},
{"(GMT+03:30) Tehran",										"Tehran[GMT+03:30]",	"IRST-3IRDT"},
{"(GMT+04:00) Abu Dhabi, Muscat",							"Abu_Dhabi,Muscat[GMT+04:00]",	"GST-4"},
{"(GMT+04:00) Baku",										"Baku[GMT+04:00]",	"AZT-4AZST"},
{"(GMT+04:00) Yerevan",										"Yerevan[GMT+04:00]",	"AMT-4AMST"},
{"(GMT+04:00) Tbilisi",										"Tbilisi[GMT+04:00]",	"GET-4"},
{"(GMT+04:30) Kabul",										"Kabul[GMT+04:30]",	"AFT-4:30"},
{"(GMT+05:00) Yekaterinburg",								"Yekaterinburg[GMT+05:00]",	"YEKT-5YEKST"},
{"(GMT+05:00) Islamabad, Karachi",							"Islamabad,Karachi[GMT+05:00]",	"PKT-5"},
{"(GMT+05:00) Tashkent",									"Tashkent[GMT+05:00]",	"UZT-5"},
{"(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi",			"Chennai,Kolkata,Mumbai,New_Delhi[GMT+05:30]",	"IST-5:30"},
{"(GMT+05:45) Kathmandu",									"Kathmandu[GMT+05:45]",	"NPT-5:45"},
{"(GMT+06:00) Almaty, Astana",								"Almaty,Astana[GMT+06:00]",	"ALMT-6"},
{"(GMT+06:00) Novosibirsk",									"Novosibirsk[GMT+06:00]",	"NOVT-6NOVST"},
{"(GMT+06:00) Dhaka",										"Dhaka[GMT+06:00]",	"BST-6"},
{"(GMT+06:00) Sri Jayawardenapura",							"Sri_Jayawardenapura[GMT+06:00]",	"LKT-6"},
{"(GMT+06:30) Yangoon",										"Yangoon[GMT+06:30]",	"MMT-6:30"},
{"(GMT+07:00) Bangkok, Hanoi",								"Bangkok,Hanoi[GMT+07:00]",	"ICT-7"},
{"(GMT+07:00) Jakarta",										"Jakarta[GMT+07:00]",	"WIB-7"},
{"(GMT+07:00) Krasnoyarsk",									"Krasnoyarsk[GMT+07:00]",	"KRAT-7KRAST"},
{"(GMT+08:00) Hong Kong",									"Hong_Kong[GMT+08:00]",	"HKT-8"},
{"(GMT+08:00) Beijing, Chongquing, Urumqi",					"Beijing,Chongquing,Urumqi[GMT+08:00]",	"CST-8"},
{"(GMT+08:00) Irkutsk",										"Irkutsk[GMT+08:00]",	"IRKT-8IRST"},
{"(GMT+08:00) Ulaan Bataar",								"Ulaan_Bataar[GMT+08:00]",	"LUAT-8"},
{"(GMT+08:00) Kuala Lumpur",								"Kuala_Lumpur[GMT+08:00]",	"MYT-8"},
{"(GMT+08:00) Singapore",									"Singapore[GMT+08:00]",	"SGT-8"},
{"(GMT+08:00) Perth",										"Perth[GMT+08:00]",	"WST-8"},
{"(GMT+08:00) Taipei",										"Taipei[GMT+08:00]",	"CST-8-2"},
{"(GMT+09:00) Osaka, Sapporo, Tokyo",						"Osaka,Sapporo,Tokyo[GMT+09:00]",	"JST-9"},
{"(GMT+09:00) Seoul",										"Seoul[GMT+09:00]",	"KST-9"},
{"(GMT+09:00) Yakutsk",										"Yakutsk[GMT+09:00]",	"YAKT-9YAKST"},
{"(GMT+09:30) Adelaide",									"Adelaide[GMT+09:30]",	"CST-9:30CDT"},
{"(GMT+09:30) Darwin",										"Darwin[GMT+09:30]",	"CST-9:30"},
{"(GMT+10:00) Brisbane",									"Brisbane[GMT+10:00]",	"EST-10"},
{"(GMT+10:00) Canberra, Melbourne, Sydney",					"Canberra,Melbourne,Sydney[GMT+10:00]",	"EST-10EDT"},
{"(GMT+10:00) Guam",										"Guam[GMT+10:00]",	"ChST-10"},
{"(GMT+10:00) Port Moresby",								"Port_Moresby[GMT+10:00]",	"PGT-10"},
{"(GMT+10:00) Hobart",										"Hobart[GMT+10:00]",	"EST-10EDT2"},
{"(GMT+10:00) Vladivostok",									"Vladivostok[GMT+10:00]",	"VLAT-10VLAST"},
{"(GMT+11:00) Magadan",										"Magadan[GMT+11:00]",	"MAGT-11MAGST-11"},
{"(GMT+11:00) Solomon Is.",									"SolomonIs.[GMT+11:00]",	"SBT-11"},
{"(GMT+11:00) New Caledonia",								"New_Caledonia[GMT+11:00]",	"NCT-11"},
{"(GMT+12:00) Auckland, Wellington",						"Auckland,Wellington[GMT+12:00]",	"NZST-12NZDT"},
{"(GMT+12:00) Kamchatka",									"Kamchatka[GMT+12:00]",	"PETT-12PETST"},
{"(GMT+12:00) Marshall Is.",								"Marshall_Is.[GMT+12:00]",	"MHT-12"},
{"(GMT+12:00) Fiji",										"Fiji[GMT+12:00]",	"FJT-12FJST"},
{NULL,	NULL,	NULL}
};

bool findInList(char *list, char *target, char *separator){
	char *buf, *ptr, *tmp_ptr = NULL;

	if(list == NULL || target == NULL || separator == NULL)
		return false;

	buf = strdup(list);
	ptr = strtok_r(buf, separator, &tmp_ptr);
	while(ptr != NULL){
		if(!strcmp(ptr, target)){
			free(buf);
			return true;
		}
		ptr = strtok_r(NULL, separator, &tmp_ptr);
	}
	free(buf);
	return false;
}

bool getSpecificObj(zcfg_offset_t oid, char *key, json_type type, void *value, objIndex_t *outIid, struct json_object **outJobj)
{
	objIndex_t iid;
	struct json_object *Obj = NULL;
	struct json_object *pramJobj;
	bool found = false;

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &Obj) == ZCFG_SUCCESS) {
		pramJobj = json_object_object_get(Obj, key);
		if(pramJobj != NULL){
			if(type == json_type_string){
				if(!strcmp((const char*)value, json_object_get_string(pramJobj))){
					found = true;
				}
			}
			else{
				if(*(int*)value == json_object_get_int(pramJobj))
					found = true;
			}

			if(found){
				if(outIid) memcpy(outIid, &iid, sizeof(objIndex_t));
				if(outJobj) *outJobj = Obj;
				else json_object_put(Obj);
				break;
			}
		}
		json_object_put(Obj);
	}

	return found;
}

/*
 * parse time range format: "hh:mm-hh:mm"
*/
bool parseTimeRange(const char *timeRange, int *startHour, int *startMin, int *stopHour, int *stopMin){
	int count = 0;

	count = sscanf(timeRange, "%d:%d-%d:%d", startHour, startMin, stopHour, stopMin);
	if(count!=4){
		return false;
	}

	if(*startHour<0 || *startHour>23 || *stopHour<0 || *stopHour>23 || *startMin<0 || *startMin>59 || *stopMin<0 || *stopMin>59){
		return false;
	}

	if((*startHour*60+*startMin)>=(*stopHour*60+*stopMin)){
		return false;
	}

	return true;
}

/*
 * Format: "serviceName@Protocol:startPort:endPort,startPort:endPort,..."
 * e.g. http@TCP:80
 *       xboxlive@UDP:88
 *       vnc@TCP:5500,5800,5900:5910
 *       userdifine1@UDP1510,1511,1550:1580
 *       userdifine2@BOTH3330,3355,3340:3356
*/
bool parseParenService(const char *service, char *parenCtlService){
	int count = 0;
	int startPort, endPort;
	char *buf, *ptr, *tmp, *serviceName, *protocol, *portList;
	char partRange[16];

	buf = strdup(service);

	if((ptr = strchr(buf, '@'))==NULL)
		goto ParenServiceError;
	*ptr = '\0';
	serviceName = buf;
	protocol = ptr+1;

	if((ptr = strchr(protocol, ':'))==NULL)
		goto ParenServiceError;
	*ptr = '\0';
	portList = ptr+1;

	if(strlen(serviceName)>17)
		goto ParenServiceError;

	if(strcmp(protocol, "TCP") && strcmp(protocol, "UDP") && strcmp(protocol, "BOTH"))
		goto ParenServiceError;

	if(parenCtlService){
		if(strncmp(serviceName, "1[", 2))
			sprintf(parenCtlService, "1[%s", serviceName);
		else
			strcat(parenCtlService, serviceName);
	}

	count = 0;
	ptr = strtok_r(portList, ",", &tmp);
	while(ptr != NULL){
		count++;
		if(count>6)
			goto ParenServiceError;

		if(sscanf(ptr, "%d:%d", &startPort, &endPort)==2){
			if(startPort < 1 || startPort > 65535 || endPort < 1 || endPort > 65535)
				goto ParenServiceError;
			else if(startPort >= endPort)
				goto ParenServiceError;

			if(parenCtlService){
				sprintf(partRange, "@%s#%d:%d", protocol, startPort, endPort);
				strcat(parenCtlService, partRange);
			}

		}
		else if(sscanf(ptr, "%d", &startPort)==1){
			if(startPort < 1 || startPort > 65535 )
				goto ParenServiceError;
			if(parenCtlService){
				sprintf(partRange, "@%s#%d:-1", protocol, startPort);
				strcat(parenCtlService, partRange);
			}
		}
		else{
			goto ParenServiceError;
		}
		ptr = strtok_r(NULL, ",", &tmp);
	}

	free(buf);
	return true;

ParenServiceError:
	free(buf);
	return false;
}

zcfgRet_t getWifiSsidList(struct json_object **Jarray){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	struct json_object *ssidObj = NULL, *obj = NULL;
	char *ssid = NULL;
	*Jarray = json_object_new_array();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_WIFI_SSID, &iid, &ssidObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		ssid = json_object_get_string(json_object_object_get(ssidObj,"SSID"));
		json_object_object_add(obj, "SSID", json_object_new_string(ssid));
		json_object_array_add(*Jarray, obj);
		json_object_put(ssidObj);
	}

	return ret;
}

zcfgRet_t getCertList(struct json_object **Jarray){
	struct json_object *obj = NULL;
	objIndex_t iid;
	char path[64] = {0};

	*Jarray = json_object_new_array();

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_DEV_SEC_CERT, &iid, &obj) == ZCFG_SUCCESS){
		sprintf(path, "Security.Certificate.%u", iid.idx[0]);
		json_object_object_add(obj, "path", json_object_new_string(path));
		json_object_array_add(*Jarray, obj);
	}

	//printf("*Jarray=%s\n", json_object_to_json_string(*Jarray));
	return ZCFG_SUCCESS;
}

zcfgRet_t getEmailList(struct json_object **Jarray){
	struct json_object *obj = NULL;
	objIndex_t iid;
	*Jarray = json_object_new_array();

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_MAIL_SERVICE, &iid, &obj) == ZCFG_SUCCESS){
		json_object_object_add(obj, "emailAddress", JSON_OBJ_COPY(json_object_object_get(obj, "EmailAddress")));
		json_object_array_add(*Jarray, obj);
	}
	return ZCFG_SUCCESS;
}


//Suppose parameter need to check is string.
zcfgRet_t dalcmdParamCheck(objIndex_t *Iid, const char *param, zcfg_offset_t oid, char *key, char *key1, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	struct json_object *obj = NULL;
	objIndex_t iid = {0};
	int len = 0, i, count = 1;
	bool duplicate = false;
	if(Iid == NULL){		//	for add function use
		while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &obj) == ZCFG_SUCCESS){
			if(!strcmp(param, json_object_get_string(json_object_object_get(obj, key)))){
				json_object_put(obj);
				obj = NULL;
				strcat(replyMsg, key1);
				strcat(replyMsg, " is duplicate.");
				return ZCFG_INVALID_PARAM_VALUE;
			}
			json_object_put(obj);
			obj = NULL;
		}
	}
	else{					//	for edit function use, need iid
		while(zcfgFeObjJsonGetNextWithoutUpdate(oid, &iid, &obj) == ZCFG_SUCCESS){
		if(!strcmp(param, json_object_get_string(json_object_object_get(obj, key)))){
				for(i=0;i<6;i++){
					if(iid.idx[i] != Iid->idx[i]){
						duplicate = true;
						break;
					}
				}
				if(duplicate){
			json_object_put(obj);
			obj = NULL;
					strcat(replyMsg, key1);
			strcat(replyMsg, " is duplicate.");
			return ZCFG_INVALID_PARAM_VALUE;
		}
			}
		json_object_put(obj);
		obj = NULL;
	}
	}
	return ret;
}

zcfgRet_t convertIndextoIid(int idx, objIndex_t *Iid, zcfg_offset_t oid, char *key, void *ptr, char *replyMsg)
{
	struct json_object *obj = NULL;
	struct json_object *paramObj = NULL;
	enum json_type type = json_type_null;
	int count = 1;
	bool keyMatch = false;
	bool found = false;

	if(idx <= 0) {
		strcat(replyMsg, " Index error.");
		return ZCFG_NO_SUCH_OBJECT;
	}
	while(zcfgFeObjJsonGetNextWithoutUpdate(oid, Iid, &obj) == ZCFG_SUCCESS) {
		if(key != NULL) {
			/*
			 * Conditional converting.
			 * i.g.
			 *   If idx = 5, key = "Enable", *ptr = true
			 *   Iid should be the fifth enabled object of (oid).
			 */
			keyMatch = false;
			paramObj = json_object_object_get(obj, key);
			type = json_object_get_type(paramObj);
			switch(type) {
				case json_type_boolean:
					if(json_object_get_boolean(paramObj) == (bool *)ptr)
						keyMatch = true;
					break;

				case json_type_int:
				case json_type_uint8:
				case json_type_uint16:
				case json_type_uint32:
				case json_type_ulong:
					if(json_object_get_int(paramObj) == (int *)ptr)
						keyMatch = true;
					break;

				case json_type_string:
				case json_type_time:
				case json_type_base64:
				case json_type_hex:
					if(!strcmp((char *)ptr, json_object_get_string(paramObj)))
						keyMatch = true;
					break;

				default:
					break;
			}

			if(keyMatch) {
				if(idx == count) {
					json_object_put(obj);
					obj = NULL;
					found = true;
					break;
				}
				count++;
			}
		}
		else {
			/* Without conditional check, only convert idx to corresponding iid. */
			if(idx == count) {
				json_object_put(obj);
				obj = NULL;
				found = true;
				break;
			}
			count++;
		}
		json_object_put(obj);
		obj = NULL;
	}
	if(!found){
		if(replyMsg)
			strcat(replyMsg, " Index error.");

		return ZCFG_NO_SUCH_OBJECT;
	}

	return ZCFG_SUCCESS;
}

/*
 * return WAN and LAN list in Jason Array.
 * Format:Jarray.i.Name = Name
 *           Jarray.i.IpIfacePath = IP.Interface.i
 *           Jarray.i.PppIfacePath = PPP.Interface.i  (For PPPoE, PPPoA only)
 *           Jarray.i.Type : LAN|WAN
 *           Jarray.i.LinkType = ATM|PTM|ETH|PON|USB (For WAN only)
 *           Jarray.i.VpiVci = vpi/vci (For ATM only)
 *           Jarray.i.VLANPriority (For WAN only)
 *           Jarray.i.VLANID (For WAN only)
 *           Jarray.i.ConnectionType = IP_Routed|IP_Bridged (For WAN only)
 *           Jarray.i.Encapsulation = IPoE|IPoA|PPPoE|PPPoA (For WAN only, NUL if LinkType is USB))
 *           Jarray.i.BindToIntfGrp = true|false (For WAN only)
 *           Jarray.i.LowerLayerUp = true|false (For  IPoE, PPPoE only, other WAN LowerLayerUp value may not correct)
 *           Jarray.i.WANConnectionReady = true|false (For  WAN only)
 *           Jarray.i.DefaultGateway = true|false (For  WAN only)
 *           Jarray.i.IPv6DefaultGateway = true|false (For  WAN only)
 *           Jarray.i.Group_WAN_IpIface = IP.Interface.i,IP.Interface.i,IP.Interface.i,... (For LAN only)
 *           Jarray.i.BridgingBrPath = Bridging.Bridge.i (For LAN only)


 *           Jarray.i.Enable = true|false (For WAN only)
 *           Jarray.i.IPv4Enable = true|false (For WAN only)
 *           Jarray.i.IPv6Enable = true|false (For WAN only)
 *	       Jarray.i.NatEnable = true|false (For WAN only)
 *           Jarray.i.IGMPEnable = true|false (For WAN only)
 *           Jarray.i.MLDEnable = true|false (For WAN only)

 */
zcfgRet_t getWanLanList(struct json_object **Jarray){
	struct json_object *ipIfaceJarray, *pppIfaceJarray, *vlanTermJarray, *ethLinkJarray, *bridgeBrJarray;
	struct json_object *ipIfaceObj, *pppIfaceObj, *vlanTermObj, *ethLinkObj, *bridgeBrObj, *routerObj;
	struct json_object *v4AddrJarray, *v6AddrJarray, *v4AddrObj, *v6AddrObj;
	struct json_object *natIntfJarray, *natIntfObj, *igmpObj, *mldObj;
	struct json_object *atmLinkJarray, *atmLinkObj;
	struct json_object *Jobj = NULL;
	char ifacePath[32] = {0}, BridgingBrPath[32] = {0};
	int len, len2, i, j;
	unsigned char idx;
	const char *X_ZYXEL_BridgeName, *X_ZYXEL_SrvName, *X_ZYXEL_ConnectionType, *X_ZYXEL_IfName, *X_ZYXEL_Group_WAN_IpIface, *LowerLayers;
	const char *Status, *ipIfaceStatus, *IPAddress, *X_ZYXEL_ActiveDefaultGateway, *X_ZYXEL_ActiveV6DefaultGateway;
	const char *Username, Password;
	char intfGrpWANList[512] = {0};
	char *intf = NULL, *tmp_ptr = NULL;
	const char *IpIfacePath = NULL, *natInterface = NULL, *igmpInterface = NULL, *mldInterface = NULL;

	zcfgFeWholeObjJsonGet("Device.IP.Interface.", &ipIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.PPP.Interface.", &pppIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.Ethernet.VLANTermination.", &vlanTermJarray);
	zcfgFeWholeObjJsonGet("Device.Ethernet.Link.", &ethLinkJarray);
	zcfgFeWholeObjJsonGet("Device.ATM.Link.", &atmLinkJarray);
	zcfgFeWholeObjJsonGet("Device.Bridging.Bridge.", &bridgeBrJarray);
	zcfgFeWholeObjJsonGet("Device.Routing.Router.1.", &routerObj);

	zcfgFeWholeObjJsonGet("Device.NAT.InterfaceSetting.", &natIntfJarray);
	zcfgFeWholeObjJsonGet("Device.X_ZYXEL_IGMP.", &igmpObj);
	zcfgFeWholeObjJsonGet("Device.X_ZYXEL_MLD.", &mldObj);

	if(ipIfaceJarray==NULL||pppIfaceJarray==NULL||vlanTermJarray==NULL||
		ethLinkJarray==NULL||atmLinkJarray==NULL||bridgeBrJarray==NULL||
		routerObj==NULL||natIntfJarray==NULL||igmpObj==NULL||mldObj==NULL){

		json_object_put(ipIfaceJarray);
		json_object_put(pppIfaceJarray);
		json_object_put(vlanTermJarray);
		json_object_put(ethLinkJarray);
		json_object_put(atmLinkJarray);
		json_object_put(bridgeBrJarray);
		json_object_put(routerObj);
		json_object_put(natIntfJarray);
		json_object_put(igmpObj);
		json_object_put(mldObj);

		return ZCFG_INTERNAL_ERROR;
	}

	X_ZYXEL_ActiveDefaultGateway = Jgets(routerObj, "X_ZYXEL_ActiveDefaultGateway");
	X_ZYXEL_ActiveV6DefaultGateway = Jgets(routerObj, "X_ZYXEL_ActiveV6DefaultGateway");
	igmpInterface = Jgets(igmpObj, "Interface");
	mldInterface = Jgets(mldObj, "Interface");
	if(X_ZYXEL_ActiveDefaultGateway == NULL) X_ZYXEL_ActiveDefaultGateway = "";
	if(X_ZYXEL_ActiveV6DefaultGateway == NULL) X_ZYXEL_ActiveV6DefaultGateway = "";
	if(igmpInterface == NULL) igmpInterface = "";
	if(mldInterface == NULL) mldInterface = "";

	*Jarray = json_object_new_array();

	len = json_object_array_length(ipIfaceJarray);
	for(i=0;i<len;i++){
		ipIfaceObj = json_object_array_get_idx(ipIfaceJarray, i);
		X_ZYXEL_SrvName = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_SrvName"));
		X_ZYXEL_ConnectionType = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConnectionType"));
		LowerLayers = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
		X_ZYXEL_Group_WAN_IpIface = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
		sprintf(ifacePath, "IP.Interface.%u", i+1);
		if(X_ZYXEL_SrvName==NULL || X_ZYXEL_ConnectionType==NULL || LowerLayers==NULL){
			continue;
		}

		Jobj = json_object_new_object();
		json_object_object_add(Jobj, "IpIfacePath", json_object_new_string(ifacePath));
		json_object_object_add(Jobj, "Name", json_object_new_string(X_ZYXEL_SrvName));

		json_object_object_add(Jobj, "Type", json_object_new_string("WAN"));
		json_object_object_add(Jobj, "VpiVci", json_object_new_string(""));
		Jaddi(Jobj, "VLANPriority", -1);
		Jaddi(Jobj, "VLANID", -1);
		json_object_object_add(Jobj, "ConnectionType", json_object_new_string(X_ZYXEL_ConnectionType));
		json_object_object_add(Jobj, "BindToIntfGrp", json_object_new_boolean(false));
		json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(false));
		json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(false));
		Jadds(Jobj, "PppIfacePath", "");
		//json_object_object_add(Jobj, "pppUsername", json_object_new_string(""));
		//json_object_object_add(Jobj, "pppPassword", json_object_new_string(""));

		if(Jgetb(ipIfaceObj, "Enable"))
			Jaddb(Jobj, "Enable", true);
		else
			Jaddb(Jobj, "Enable", false);
		if(Jgetb(ipIfaceObj, "IPv4Enable"))
			Jaddb(Jobj, "IPv4Enable", true);
		else
			Jaddb(Jobj, "IPv4Enable", false);
		if(Jgetb(ipIfaceObj, "IPv6Enable"))
			Jaddb(Jobj, "IPv6Enable", true);
		else
			Jaddb(Jobj, "IPv6Enable", false);

		//Get DefaultGateway, IPv6DefaultGateway
		if(findInList(X_ZYXEL_ActiveDefaultGateway, ifacePath, ","))
			json_object_object_add(Jobj, "DefaultGateway", json_object_new_boolean(true));
		else
			json_object_object_add(Jobj, "DefaultGateway", json_object_new_boolean(false));

		if(findInList(X_ZYXEL_ActiveV6DefaultGateway, ifacePath, ","))
			json_object_object_add(Jobj, "IPv6DefaultGateway", json_object_new_boolean(true));
		else
			json_object_object_add(Jobj, "IPv6DefaultGateway", json_object_new_boolean(false));

		if(findInList(igmpInterface, ifacePath, ","))
			json_object_object_add(Jobj, "IGMPEnable", json_object_new_boolean(true));
		else
			json_object_object_add(Jobj, "IGMPEnable", json_object_new_boolean(false));

		if(findInList(mldInterface, ifacePath, ","))
			json_object_object_add(Jobj, "MLDEnable", json_object_new_boolean(true));
		else
			json_object_object_add(Jobj, "MLDEnable", json_object_new_boolean(false));

		//Get NatEnable
		Jaddb(Jobj, "NatEnable", false);
		len2 = json_object_array_length(natIntfJarray);
		for(j=0;j<len2;j++){
			natIntfObj = json_object_array_get_idx(natIntfJarray, j);
			if(isEmptyIns(natIntfObj))
				continue;
			natInterface = Jgets(natIntfObj, "Interface");
			if(natInterface==NULL) continue;
			if(!strcmp(natInterface, ifacePath)){
				if(Jgetb(natIntfObj, "Enable"))
					Jaddb(Jobj, "NatEnable", true);
				break;
			}
		}

		ipIfaceStatus = json_object_get_string(json_object_object_get(ipIfaceObj, "Status"));
		if(ipIfaceStatus != NULL && !strcmp(ipIfaceStatus, "Up")){  //|| !strcmp(ipIfaceStatus, "LowerLayerDown")
			v4AddrJarray = Jget(ipIfaceObj, "IPv4Address");
			if(v4AddrJarray != NULL){
				len2 = json_object_array_length(v4AddrJarray);
				for(j=0;j<len2;j++){
					v4AddrObj = json_object_array_get_idx(v4AddrJarray, j);
					if(isEmptyIns(v4AddrObj))
						continue;
					IPAddress = json_object_get_string(json_object_object_get(v4AddrObj,"IPAddress"));
					Status = json_object_get_string(json_object_object_get(v4AddrObj, "Status"));
					if(IPAddress != NULL && Status != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
						json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(true));
						break;
					}
				}
			}
			if(!Jgetb(Jobj, "WANConnectionReady")){
			v6AddrJarray = Jget(ipIfaceObj, "IPv6Address");
			if(v6AddrJarray != NULL){
				len2 = json_object_array_length(v6AddrJarray);
				for(j=0;j<len2;j++){
					v6AddrObj = json_object_array_get_idx(v6AddrJarray, j);
					if(isEmptyIns(v6AddrObj))
						continue;
					IPAddress = json_object_get_string(json_object_object_get(v6AddrObj,"IPAddress"));
					Status = json_object_get_string(json_object_object_get(v6AddrObj, "Status"));
					if(IPAddress != NULL && Status != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
						json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(true));
						break;
					}
				}
			}
			}
		}


		int count = 0;
		const char *HigherLayer = "IP.Interface.";
		while(count < 4){
			count++;
			if(LowerLayers == NULL){
				json_object_put(Jobj);
				Jobj = NULL;
				break;
			}
			if(!strncmp(HigherLayer, "IP.Interface.", 13)){
				if(!strncmp(LowerLayers, "PPP.Interface.", 14)){
					sscanf(LowerLayers, "PPP.Interface.%hhu", &idx);
					pppIfaceObj = json_object_array_get_idx(pppIfaceJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(pppIfaceObj, "LowerLayers"));
				}
				else if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));

					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
					replaceParam(Jobj, "VLANPriority", vlanTermObj, "X_ZYXEL_VLANPriority");
					replaceParam(Jobj, "VLANID", vlanTermObj, "VLANID");
				}
				else if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &idx);
					ethLinkObj = json_object_array_get_idx(ethLinkJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9) && !strcmp("IP_Routed", X_ZYXEL_ConnectionType)){
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));

					sscanf(LowerLayers, "ATM.Link.%hhu", &idx);
					atmLinkObj = json_object_array_get_idx(atmLinkJarray, idx-1);
					replaceParam(Jobj, "VpiVci", atmLinkObj, "DestinationAddress");
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp(HigherLayer, "PPP.Interface.", 14)){
				Jadds(Jobj, "PppIfacePath", HigherLayer);
				//replaceParam(Jobj, "pppUsername", pppIfaceObj, "Username");
				//replaceParam(Jobj, "pppPassword", pppIfaceObj, "Password");

				if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoE"));
					replaceParam(Jobj, "VLANPriority", vlanTermObj, "X_ZYXEL_VLANPriority");
					replaceParam(Jobj, "VLANID", vlanTermObj, "VLANID");
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					Status = json_object_get_string(json_object_object_get(pppIfaceObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));

					sscanf(LowerLayers, "ATM.Link.%hhu", &idx);
					atmLinkObj = json_object_array_get_idx(atmLinkJarray, idx-1);
					replaceParam(Jobj, "VpiVci", atmLinkObj, "DestinationAddress");
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					Status = json_object_get_string(json_object_object_get(pppIfaceObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25)){
				if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &idx);
					ethLinkObj = json_object_array_get_idx(ethLinkJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
					replaceParam(Jobj, "VLANPriority", vlanTermObj, "X_ZYXEL_VLANPriority");
					replaceParam(Jobj, "VLANID", vlanTermObj, "VLANID");
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.Link.", HigherLayer, 14)){
				if(!strncmp("ATM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));

					sscanf(LowerLayers, "ATM.Link.%hhu", &idx);
					atmLinkObj = json_object_array_get_idx(atmLinkJarray, idx-1);
					replaceParam(Jobj, "VpiVci", atmLinkObj, "DestinationAddress");
					break;
				}
				else if(!strncmp("PTM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PTM"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Ethernet.Interface.", LowerLayers, 19)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Optical.Interface.", LowerLayers, 18)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PON"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Cellular.Interface.", LowerLayers, 19)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("CELL"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Bridging.Bridge.", LowerLayers, 16)){
					sscanf(LowerLayers, "Bridging.Bridge.%hhu.Port.1", &idx);
					bridgeBrObj = json_object_array_get_idx(bridgeBrJarray, idx-1);
					X_ZYXEL_BridgeName = json_object_get_string(json_object_object_get(bridgeBrObj, "X_ZYXEL_BridgeName"));
					if(X_ZYXEL_BridgeName == NULL){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					if(X_ZYXEL_Group_WAN_IpIface == NULL)
						X_ZYXEL_Group_WAN_IpIface = "";

					sprintf(BridgingBrPath, "Bridging.Bridge.%u", idx);
					json_object_object_add(Jobj, "Name", json_object_new_string(X_ZYXEL_BridgeName));
					json_object_object_add(Jobj, "Type", json_object_new_string("LAN"));
					json_object_object_add(Jobj, "BridgingBrPath", json_object_new_string(BridgingBrPath));
					json_object_object_add(Jobj, "Group_WAN_IpIface", json_object_new_string(X_ZYXEL_Group_WAN_IpIface));
					strcat(intfGrpWANList, X_ZYXEL_Group_WAN_IpIface);
					strcat(intfGrpWANList, ",");
					break;
				}
				else if(!strcmp("", LowerLayers)){
					if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
		}

		if(Jobj != NULL)
			json_object_array_add(*Jarray, Jobj);
	}

	if(strcmp(intfGrpWANList, "")){
		intf = strtok_r(intfGrpWANList, ",", &tmp_ptr);
		while(intf != NULL){
			if(*intf != '\0'){
				len = json_object_array_length(*Jarray);
				for(i=0;i<len;i++){
					Jobj = json_object_array_get_idx(*Jarray, i);
					IpIfacePath = json_object_get_string(json_object_object_get(Jobj, "IpIfacePath"));
					if(!strcmp(IpIfacePath, intf)){
						json_object_object_add(Jobj, "BindToIntfGrp", json_object_new_boolean(true));
					}
				}
			}
			intf = strtok_r(NULL, ",", &tmp_ptr);
		}
	}
	//printf("*Jarray=%s\n", json_object_to_json_string(*Jarray));

	json_object_put(ipIfaceJarray);
	json_object_put(pppIfaceJarray);
	json_object_put(vlanTermJarray);
	json_object_put(ethLinkJarray);
	json_object_put(atmLinkJarray);
	json_object_put(bridgeBrJarray);
	json_object_put(routerObj);
	json_object_put(natIntfJarray);
	json_object_put(igmpObj);
	json_object_put(mldObj);

	return ZCFG_SUCCESS;
}

void invalidCharFilter(char *input, int maxlen){
	int i = 0;
	int length = strlen(input);

	if(NULL != input){
		for( i = 0; i < length; i++){
			if(input[i] == ';' || input[i] == '&' || input[i] == '|')
				input[i] = '\0';
		}
		if(length > maxlen){
			input[maxlen-1] = '\0';
		}
	}
}

bool HostNameFilter(const char *input){
	bool result = false;

	regex_t preg;
	regmatch_t matchptr[1];
	const size_t nmatch = 1;
	const char* pattern = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$";
	int status;

	regcomp(&preg, pattern, REG_EXTENDED|REG_ICASE);
	status = regexec(&preg, input, nmatch, matchptr, 0);
	if (status == REG_NOMATCH){
	    printf("invalid Hostname\n");
	    result = false;
	}
	else if (status == 0){
	    printf("valid Hostname\n");
	    result = true;
	}
	regfree(&preg);
	return result;
}

bool DomainFilter(const char *input){
	bool result = false;

	regex_t preg;
	regmatch_t matchptr[1];
	const size_t nmatch = 1;
	const char* pattern = "^([^-][A-Za-z0-9-]{1,63}\\.)+([A-Za-z0-9-]{1,63}\\.)*+[A-Za-z]{2,6}+([.]?)$";
	int status, maxlen = 254;

	if(strlen(input) > maxlen)
		return result;

	regcomp(&preg, pattern, REG_EXTENDED|REG_ICASE);
	status = regexec(&preg, input, nmatch, matchptr, 0);
	if (status == REG_NOMATCH){
	    printf("invalid Domain\n");
	    result = false;
	}
	else if (status == 0){
	    printf("valid Domain\n");
	    result = true;
	}
	regfree(&preg);
	return result;
}

bool IPFilter(const char *input){
	bool result = false;

	regex_t preg;
	regmatch_t matchptr[1];
	const size_t nmatch = 1;
	const char* pattern = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
	int status;

	regcomp(&preg, pattern, REG_EXTENDED|REG_ICASE);
	status = regexec(&preg, input, nmatch, matchptr, 0);
	if (status == REG_NOMATCH){
	    printf("invalid IP\n");
	    result = false;
	}
	else if (status == 0){
	    printf("valid IP\n");
	    result = true;
	}
	regfree(&preg);
	return result;
}

bool IPv6Filter(const char *input){
	bool result = false;

	regex_t preg;
	regmatch_t matchptr[1];
	const size_t nmatch = 1;
	const char* pattern = "^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|(fe80:(:[0-9a-fA-F]{0,4}){0,6}(((25[0-5]|(2[0-4][0-9]|1{0,1}[0-9]|[0-9]?[0-9]))\\.){3,3}(25[0-5]|(2[0-4][0-9]|1[0-9][0-9]|[0-9]?[0-9])))?(/((1[0-2][0-8]|1{0,1}[0-9])))?)|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))$";
	int status;

	regcomp(&preg, pattern, REG_EXTENDED|REG_ICASE);

	status = regexec(&preg, input, nmatch, matchptr, 0);
	if (status == REG_NOMATCH){
	    printf("invalid IPv6 address\n");
	    result = false;
	}
	else if (status == 0){
	    printf("valid IPv6 address\n");
	    result = true;
	}
	regfree(&preg);
	return result;
}

#if 0
/*
 * return WAN and LAN list in Jason Array.
 * Format:Jarray.i.NAME = Name
 *           Jarray.i.IpIfacePath = IP.Interface.i
 *           Jarray.i.PppIfacePath = PPP.Interface.i  (For PPPoE, PPPoA only)
 *           Jarray.i.Type : LAN|WAN
 *           Jarray.i.LinkType = ATM|PTM|ETH|PON|USB (For WAN only)
 *           Jarray.i.ConnectionType = IP_Routed|IP_Bridged (For WAN only)
 *           Jarray.i.Encapsulation = IPoE|IPoA|PPPoE|PPPoA (For WAN only, NUL if LinkType is USB))
 *           Jarray.i.BindToIntfGrp = true|false (For WAN only)
 *           Jarray.i.LowerLayerUp = true|false (For  IPoE, PPPoE only, other WAN LowerLayerUp value may not correct)
 *           Jarray.i.WANConnectionReady = true|false (For  WAN only)
 *           Jarray.i.DefaultGateway = true|false (For  WAN only)
 *           Jarray.i.Group_WAN_IpIface = IP.Interface.i,IP.Interface.i,IP.Interface.i,... (For LAN only)
 *           Jarray.i.BridgingBrPath = Bridging.Bridge.i (For LAN only)
 */
zcfgRet_t getWanLanList(struct json_object **Jarray){
	struct json_object *ipIfaceJarray, *pppIfaceJarray, *vlanTermJarray, *ethLinkJarray, *bridgeBrJarray;
	struct json_object *ipIfaceObj, *pppIfaceObj, *vlanTermObj, *ethLinkObj, *bridgeBrObj, *routerObj;
	struct json_object *v4AddrJarray, *v6AddrJarray, *v4AddrObj, *v6AddrObj;
	struct json_object *Jobj = NULL;
	char ifacePath[32] = {0}, BridgingBrPath[32] = {0};
	int len, len2, i, j;
	unsigned char idx;
	const char *X_ZYXEL_BridgeName, *X_ZYXEL_SrvName, *X_ZYXEL_ConnectionType, *X_ZYXEL_IfName, *X_ZYXEL_Group_WAN_IpIface, *LowerLayers;
	const char *Status, *ipIfaceStatus, *IPAddress, *X_ZYXEL_ActiveDefaultGateway;
	const char *Username, Password;
	char intfGrpWANList[256] = {0};
	char *intf = NULL, *tmp_ptr = NULL;
	const char *IpIfacePath = NULL;

	zcfgFeWholeObjJsonGet("Device.IP.Interface.", &ipIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.PPP.Interface.", &pppIfaceJarray);
	zcfgFeWholeObjJsonGet("Device.Ethernet.VLANTermination.", &vlanTermJarray);
	zcfgFeWholeObjJsonGet("Device.Ethernet.Link.", &ethLinkJarray);
	zcfgFeWholeObjJsonGet("Device.Bridging.Bridge.", &bridgeBrJarray);
	zcfgFeWholeObjJsonGet("Device.Routing.Router.1.", &routerObj);

	X_ZYXEL_ActiveDefaultGateway = Jgets(routerObj, "X_ZYXEL_ActiveDefaultGateway");

	if(ipIfaceJarray==NULL||pppIfaceJarray==NULL||vlanTermJarray==NULL||ethLinkJarray==NULL||bridgeBrJarray==NULL)
		return ZCFG_INTERNAL_ERROR;

	*Jarray = json_object_new_array();

	len = json_object_array_length(ipIfaceJarray);
	for(i=0;i<len;i++){
		ipIfaceObj = json_object_array_get_idx(ipIfaceJarray, i);
		X_ZYXEL_SrvName = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_SrvName"));
		X_ZYXEL_ConnectionType = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_ConnectionType"));
		LowerLayers = json_object_get_string(json_object_object_get(ipIfaceObj, "LowerLayers"));
		X_ZYXEL_Group_WAN_IpIface = json_object_get_string(json_object_object_get(ipIfaceObj, "X_ZYXEL_Group_WAN_IpIface"));
		sprintf(ifacePath, "IP.Interface.%u", i+1);
		if(X_ZYXEL_SrvName==NULL || X_ZYXEL_ConnectionType==NULL || LowerLayers==NULL){
			continue;
		}

		Jobj = json_object_new_object();
		json_object_object_add(Jobj, "IpIfacePath", json_object_new_string(ifacePath));
		json_object_object_add(Jobj, "Name", json_object_new_string(X_ZYXEL_SrvName));

		json_object_object_add(Jobj, "Type", json_object_new_string("WAN"));
		json_object_object_add(Jobj, "ConnectionType", json_object_new_string(X_ZYXEL_ConnectionType));
		json_object_object_add(Jobj, "BindToIntfGrp", json_object_new_boolean(false));
		json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(false));
		json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(false));
		Jadds(Jobj, "PppIfacePath", "");
		//json_object_object_add(Jobj, "pppUsername", json_object_new_string(""));
		//json_object_object_add(Jobj, "pppPassword", json_object_new_string(""));

		if(findInList(X_ZYXEL_ActiveDefaultGateway, ifacePath, ","))
			json_object_object_add(Jobj, "DefaultGateway", json_object_new_boolean(true));
		else
			json_object_object_add(Jobj, "DefaultGateway", json_object_new_boolean(false));

		ipIfaceStatus = json_object_get_string(json_object_object_get(ipIfaceObj, "Status"));
		if(ipIfaceStatus != NULL && !strcmp(ipIfaceStatus, "Up")){  //|| !strcmp(ipIfaceStatus, "LowerLayerDown")
			v4AddrJarray = Jget(ipIfaceObj, "IPv4Address");
			if(v4AddrJarray != NULL){
				len2 = json_object_array_length(v4AddrJarray);
				for(j=0;j<len2;j++){
					v4AddrObj = json_object_array_get_idx(v4AddrJarray, j);
					if(isEmptyIns(v4AddrObj))
						continue;
					IPAddress = json_object_get_string(json_object_object_get(v4AddrObj,"IPAddress"));
					Status = json_object_get_string(json_object_object_get(v4AddrObj, "Status"));
					if(IPAddress != NULL && Status != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
						json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(true));
						break;
					}
				}
			}
			if(!Jgetb(Jobj, "WANConnectionReady")){
			v6AddrJarray = Jget(ipIfaceObj, "IPv6Address");
			if(v6AddrJarray != NULL){
				len2 = json_object_array_length(v6AddrJarray);
				for(j=0;j<len2;j++){
					v6AddrObj = json_object_array_get_idx(v6AddrJarray, j);
					if(isEmptyIns(v6AddrObj))
						continue;
					IPAddress = json_object_get_string(json_object_object_get(v6AddrObj,"IPAddress"));
					Status = json_object_get_string(json_object_object_get(v6AddrObj, "Status"));
					if(IPAddress != NULL && Status != NULL && strcmp(IPAddress, "") && !strcmp(Status, "Enabled")){
						json_object_object_add(Jobj, "WANConnectionReady", json_object_new_boolean(true));
						break;
					}
				}
			}
			}
		}


		int count = 0;
		const char *HigherLayer = "IP.Interface.";
		while(count < 4){
			count++;
			if(LowerLayers == NULL){
				json_object_put(Jobj);
				Jobj = NULL;
				break;
			}
			if(!strncmp(HigherLayer, "IP.Interface.", 13)){
				if(!strncmp(LowerLayers, "PPP.Interface.", 14)){
					sscanf(LowerLayers, "PPP.Interface.%hhu", &idx);
					pppIfaceObj = json_object_array_get_idx(pppIfaceJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(pppIfaceObj, "LowerLayers"));
				}
				else if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));

					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoE"));
				}
				else if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &idx);
					ethLinkObj = json_object_array_get_idx(ethLinkJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9) && !strcmp("IP_Routed", X_ZYXEL_ConnectionType)){
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("IPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp(HigherLayer, "PPP.Interface.", 14)){
				Jadds(Jobj, "PppIfacePath", HigherLayer);
				//replaceParam(Jobj, "pppUsername", pppIfaceObj, "Username");
				//replaceParam(Jobj, "pppPassword", pppIfaceObj, "Password");

				if(!strncmp("Ethernet.VLANTermination.", LowerLayers, 25)){
					sscanf(LowerLayers, "Ethernet.VLANTermination.%hhu", &idx);
					vlanTermObj = json_object_array_get_idx(vlanTermJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(vlanTermObj, "LowerLayers"));
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoE"));
				}
				else if(!strncmp("ATM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "Encapsulation", json_object_new_string("PPPoA"));
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					Status = json_object_get_string(json_object_object_get(pppIfaceObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					Status = json_object_get_string(json_object_object_get(pppIfaceObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.VLANTermination.", HigherLayer, 25)){
				if(!strncmp("Ethernet.Link.", LowerLayers, 14)){
					sscanf(LowerLayers, "Ethernet.Link.%hhu", &idx);
					ethLinkObj = json_object_array_get_idx(ethLinkJarray, idx-1);
					HigherLayer = LowerLayers;
					LowerLayers = json_object_get_string(json_object_object_get(ethLinkObj, "LowerLayers"));
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
			else if(!strncmp("Ethernet.Link.", HigherLayer, 14)){
				if(!strncmp("ATM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ATM"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("PTM.Link.", LowerLayers, 9)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PTM"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Ethernet.Interface.", LowerLayers, 19)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Optical.Interface.", LowerLayers, 18)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("PON"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("USB.Interface.", LowerLayers, 14)){
					json_object_object_add(Jobj, "LinkType", json_object_new_string("USB"));
					Status = json_object_get_string(json_object_object_get(ethLinkObj, "Status"));
					if(Status != NULL && strstr(Status, "Up"))
						json_object_object_add(Jobj, "LowerLayerUp", json_object_new_boolean(true));
					break;
				}
				else if(!strncmp("Bridging.Bridge.", LowerLayers, 16)){
					sscanf(LowerLayers, "Bridging.Bridge.%hhu.Port.1", &idx);
					bridgeBrObj = json_object_array_get_idx(bridgeBrJarray, idx-1);
					X_ZYXEL_BridgeName = json_object_get_string(json_object_object_get(bridgeBrObj, "X_ZYXEL_BridgeName"));
					if(X_ZYXEL_BridgeName == NULL){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					if(X_ZYXEL_Group_WAN_IpIface == NULL)
						X_ZYXEL_Group_WAN_IpIface = "";

					sprintf(BridgingBrPath, "Bridging.Bridge.%u", idx);
					json_object_object_add(Jobj, "Name", json_object_new_string(X_ZYXEL_BridgeName));
					json_object_object_add(Jobj, "Type", json_object_new_string("LAN"));
					json_object_object_add(Jobj, "BridgingBrPath", json_object_new_string(BridgingBrPath));
					json_object_object_add(Jobj, "Group_WAN_IpIface", json_object_new_string(X_ZYXEL_Group_WAN_IpIface));
					strcat(intfGrpWANList, X_ZYXEL_Group_WAN_IpIface);
					strcat(intfGrpWANList, ",");
					break;
				}
				else if(!strcmp("", LowerLayers)){
					if(json_object_get_boolean(json_object_object_get(ethLinkObj, "X_ZYXEL_OrigEthWAN")) == false){
						json_object_put(Jobj);
						Jobj = NULL;
						break;
					}
					json_object_object_add(Jobj, "LinkType", json_object_new_string("ETH"));
					break;
				}
				else{
					json_object_put(Jobj);
					Jobj = NULL;
					break;
				}
			}
		}

		if(Jobj != NULL)
			json_object_array_add(*Jarray, Jobj);
	}

	if(strcmp(intfGrpWANList, "")){
		intf = strtok_r(intfGrpWANList, ",", &tmp_ptr);
		while(intf != NULL){
			if(*intf != '\0'){
				len = json_object_array_length(*Jarray);
				for(i=0;i<len;i++){
					Jobj = json_object_array_get_idx(*Jarray, i);
					IpIfacePath = json_object_get_string(json_object_object_get(Jobj, "IpIfacePath"));
					if(!strcmp(IpIfacePath, intf)){
						json_object_object_add(Jobj, "BindToIntfGrp", json_object_new_boolean(true));
					}
				}
			}
			intf = strtok_r(NULL, ",", &tmp_ptr);
		}
	}
	//printf("*Jarray=%s\n", json_object_to_json_string(*Jarray));
	return ZCFG_SUCCESS;
}
#endif
/*
 * return WAN and LAN list in Jason Array.
 * Format:Jarray.i.NAME = Name
 *           Jarray.i.BrPortPath = Bridging.Bridge.i.Port.i
 *           Jarray.i.IntfPath : Ethernet.Interface.i|WiFi.SSID.i
 *           Jarray.i.Type : ETHLAN|WiFiSSID
 */
zcfgRet_t getLanPortList(struct json_object **Jarray){
	objIndex_t iid;
	struct json_object *Obj = NULL;
	struct json_object *mappingObj = NULL;
	struct json_object *Jobj = NULL;
	char intfPath[64] = {0}, brPortPath[64] = {0};
	const char *LowerLayers, *X_ZYXEL_LanPort, *SSID, *BrPortPath;


	*Jarray = json_object_new_array();

	mappingObj = json_object_new_object();
	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_BRIDGING_BR_PORT, &iid, &Obj) == ZCFG_SUCCESS) {
		LowerLayers = json_object_get_string(json_object_object_get(Obj, "LowerLayers"));
		if(!strncmp(LowerLayers, "Ethernet.Interface.", 19) || !strncmp(LowerLayers, "WiFi.SSID.", 10)){
			sprintf(brPortPath, "Bridging.Bridge.%u.Port.%u", iid.idx[0], iid.idx[1]);
			json_object_object_add(mappingObj, LowerLayers, json_object_new_string(brPortPath));
		}
		json_object_put(Obj);
	}

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ETH_IFACE, &iid, &Obj) == ZCFG_SUCCESS) {
		if(json_object_get_boolean(json_object_object_get(Obj, "Upstream")) || json_object_get_boolean(json_object_object_get(Obj, "X_ZYXEL_Upstream")))
			continue;

		sprintf(intfPath, "Ethernet.Interface.%u", iid.idx[0]);
		X_ZYXEL_LanPort = json_object_get_string(json_object_object_get(Obj, "X_ZYXEL_LanPort"));
		BrPortPath = json_object_get_string(json_object_object_get(mappingObj, intfPath));
		if(BrPortPath == NULL)
			BrPortPath = "";
		if(X_ZYXEL_LanPort == NULL)
			X_ZYXEL_LanPort = "";

		Jobj = json_object_new_object();
		json_object_object_add(Jobj, "Name", json_object_new_string(X_ZYXEL_LanPort));
		json_object_object_add(Jobj, "BrPortPath", json_object_new_string(BrPortPath));
		json_object_object_add(Jobj, "IntfPath", json_object_new_string(intfPath));
		json_object_object_add(Jobj, "Type", json_object_new_string("ETHLAN"));
		json_object_array_add(*Jarray, Jobj);

		json_object_put(Obj);
	}

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &iid, &Obj) == ZCFG_SUCCESS) {
		sprintf(intfPath, "WiFi.SSID.%u", iid.idx[0]);
		SSID = json_object_get_string(json_object_object_get(Obj, "SSID"));
		BrPortPath = json_object_get_string(json_object_object_get(mappingObj, intfPath));
		if(BrPortPath == NULL)
			BrPortPath = "";
		Jobj = json_object_new_object();
		json_object_object_add(Jobj, "Name", json_object_new_string(SSID));
		json_object_object_add(Jobj, "BrPortPath", json_object_new_string(BrPortPath));
		json_object_object_add(Jobj, "IntfPath", json_object_new_string(intfPath));
		json_object_object_add(Jobj, "Type", json_object_new_string("WiFiSSID"));
		json_object_array_add(*Jarray, Jobj);

		json_object_put(Obj);
	}
	json_object_put(mappingObj);
	return ZCFG_SUCCESS;
}


zcfgRet_t getRouteIntfList(struct json_object **Jarray){
	objIndex_t intfIid;
	struct json_object *obj = NULL;
	struct json_object *intfObj = NULL;
	const char *connection = NULL;
	const char *name = NULL;
	const char *type = NULL;
	int count = 1;
	*Jarray = json_object_new_array();
	IID_INIT(intfIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_IP_IFACE, &intfIid, &intfObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		connection = json_object_get_string(json_object_object_get(intfObj, "X_ZYXEL_ConnectionType"));
		type = json_object_get_string(json_object_object_get(intfObj, "Name"));
		name = json_object_get_string(json_object_object_get(intfObj, "X_ZYXEL_SrvName"));
		json_object_object_add(obj, "Name", json_object_new_string(name));
		json_object_object_add(obj, "ConnectionType", json_object_new_string(connection));
		json_object_object_add(obj, "Type", json_object_new_string(type));
		json_object_object_add(obj, "Number", json_object_new_int(count));
		json_object_array_add(*Jarray, obj);

		json_object_put(intfObj);
		count++;
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getClassList(struct json_object **Jarray){
	objIndex_t classIid;
	struct json_object *obj = NULL;
	struct json_object *classObj = NULL;
	const char *policer = NULL;
	const char *name = NULL;
	*Jarray = json_object_new_array();
	IID_INIT(classIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_QOS_CLS, &classIid, &classObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		policer = json_object_get_string(json_object_object_get(classObj, "Policer"));
		name = json_object_get_string(json_object_object_get(classObj, "X_ZYXEL_Name"));
		json_object_object_add(obj, "Name", json_object_new_string(name));
		json_object_object_add(obj, "Policer", json_object_new_string(policer));
		json_object_array_add(*Jarray, obj);

		json_object_put(classObj);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getQueueList(struct json_object **Jarray){
	objIndex_t queueIid;
	struct json_object *obj = NULL;
	struct json_object *queueObj = NULL;
	const char *trafficClasses = NULL;
	const char *name = NULL;
	int queueIdx = 0;
	*Jarray = json_object_new_array();
	IID_INIT(queueIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_QOS_QUE, &queueIid, &queueObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		trafficClasses = json_object_get_string(json_object_object_get(queueObj, "TrafficClasses"));
		name = json_object_get_string(json_object_object_get(queueObj, "X_ZYXEL_Name"));
		queueIdx = queueIid.idx[0];
		json_object_object_add(obj, "Name", json_object_new_string(name));
		json_object_object_add(obj, "TrafficClasses", json_object_new_string(trafficClasses));
		json_object_object_add(obj, "QueueIdx", json_object_new_int(queueIdx));
		json_object_array_add(*Jarray, obj);

		json_object_put(queueObj);
	}
	return ZCFG_SUCCESS;
}


zcfgRet_t getVoipMultiObj(struct json_object **VoipMultiObj){
	objIndex_t voipIid;
	struct json_object *voipObj = NULL, *obj = NULL;
	char voipline[64] = {0}, sipnumber[64] = {0}, sip[8] = {0}, boundList[64] = {0};
	int count = 1;
	const char *directoryNumber = NULL;

	*VoipMultiObj = json_object_new_array();

	IID_INIT(voipIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_VOICE_LINE, &voipIid, &voipObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		directoryNumber = json_object_get_string(json_object_object_get(voipObj, "DirectoryNumber"));
		sprintf(sip,"SIP%d",count);
		//sprintf(voipline,"VoiceProfile.%d.Line.%d",voipIid.idx[1],voipIid.idx[2]);
		//sprintf(boundList,"Device.Services.VoiceService.1.VoiceProfile.%d.Line.%d",voipIid.idx[1],voipIid.idx[2]);
		sprintf(voipline,"Device.Services.VoiceService.1.VoiceProfile.%d.Line.%d",voipIid.idx[1],voipIid.idx[2]);
		sprintf(boundList,"VoiceProfile.%d.Line.%d",voipIid.idx[1],voipIid.idx[2]);
		//sprintf(sipnumber,"%d.SIP%d",count,count);
		sprintf(sipnumber,"SIP%d",count);
		json_object_object_add(obj, "SIP", json_object_new_string(sip));
		json_object_object_add(obj, "VoipLine", json_object_new_string(voipline));
		json_object_object_add(obj, "BoundList", json_object_new_string(boundList));
		json_object_object_add(obj, "SIPNumber", json_object_new_string(sipnumber));
		json_object_object_add(obj, "DirectoryNumber", JSON_OBJ_COPY(json_object_object_get(voipObj, "DirectoryNumber")));
		json_object_object_add(obj, "PhyReferenceFXSList", JSON_OBJ_COPY(json_object_object_get(voipObj, "X_ZYXEL_PhyReferenceFXSList")));
		json_object_array_add(*VoipMultiObj, obj);
		json_object_put(voipObj);
		count++;
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getVoipProviderObj(struct json_object **VoipProviderObj){
	objIndex_t voipProviderIid;
	struct json_object *voipProviderObj = NULL, *obj = NULL;
	const char *name = NULL;
	char voipspname[64] = {0};
	int count = 1;

	*VoipProviderObj = json_object_new_array();
	IID_INIT(voipProviderIid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_VOICE_PROF, &voipProviderIid, &voipProviderObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		name = json_object_get_string(json_object_object_get(voipProviderObj, "Name"));
		sprintf(voipspname,"%d.%s",count,name);
		//json_object_object_add(obj, "VoipServiceProvider", json_object_new_string(voipspname));
		json_object_object_add(obj, "VoipServiceProvider", json_object_new_string(name));
		json_object_array_add(*VoipProviderObj, obj);
		json_object_put(voipProviderObj);
		count++;
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getAccountObj(struct json_object **AccountObj){
	//This function jsut get admin and user level account, for usb file share
	struct json_object *accountObj = NULL, *obj = NULL;
	objIndex_t accountIid = {0};
	accountIid.level = 2;
	accountIid.idx[0] = 2;
	*AccountObj = json_object_new_array();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &accountIid, &accountObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		if(json_object_object_get(accountObj, "Enabled") && json_object_object_get(accountObj, "Username")){
			json_object_object_add(obj, "Status", JSON_OBJ_COPY(json_object_object_get(accountObj, "Enabled")));
			json_object_object_add(obj, "Username", JSON_OBJ_COPY(json_object_object_get(accountObj, "Username")));
			json_object_array_add(*AccountObj, obj);
		}
		json_object_put(accountObj);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getUSBinfoObj(struct json_object **UsbObj){
	//This function jsut get admin and user level account, for usb file share
	struct json_object *usbObj = NULL, *obj = NULL;
	objIndex_t usbIid = {0};
	usbIid.level = 3;
	usbIid.idx[0] = 1;
	usbIid.idx[1] = 2;

	*UsbObj = json_object_new_array();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_USB_USBHOSTS_HOST_DEV_PAR, &usbIid, &usbObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		json_object_object_add(obj, "PartitionName", JSON_OBJ_COPY(json_object_object_get(usbObj, "PartitionName")));
		json_object_object_add(obj, "Capacity", JSON_OBJ_COPY(json_object_object_get(usbObj, "Capacity")));
		json_object_object_add(obj, "UsedSpace", JSON_OBJ_COPY(json_object_object_get(usbObj, "UsedSpace")));
		json_object_array_add(*UsbObj, obj);
		json_object_put(usbObj);
	}
	return ZCFG_SUCCESS;
}

zcfgRet_t getVlanWithoutBrRefKeyinfoObj(struct json_object **VlanObj){
	struct json_object *vlanObj = NULL;
	struct json_object *obj = NULL;
	objIndex_t vlanIid = {0};
	const char *vlantmp = NULL;
	char vlanName[256] = {0};
	int len = 0;

	*VlanObj = json_object_new_array();
	obj = json_object_new_object();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_VLAN_GROUP, &vlanIid, &vlanObj) == ZCFG_SUCCESS){
		if(!strcmp(json_object_get_string(json_object_object_get(vlanObj, "BrRefKey")),"")){
			vlantmp = json_object_get_string(json_object_object_get(vlanObj, "GroupName"));
			strcat(vlanName,vlantmp);
			strcat(vlanName,",");
		}
		json_object_put(vlanObj);
		vlanObj = NULL;
	}
	len = strlen(vlanName);
	if(len >0){
		if(vlanName[len-1] == ',')
			vlanName[len-1] = '\0';
	}
	if(!strcmp(vlanName,"")){
		strcat(vlanName, "Vlan type cannot support now, please add a vlan rule.");
	}
	json_object_object_add(obj, "VlanName", json_object_new_string(vlanName));
	json_object_array_add(*VlanObj, obj);

	return ZCFG_SUCCESS;
}

zcfgRet_t getwlanModeObj(struct json_object **modeObj){
	struct json_object *obj = NULL;
	struct json_object *radioObj = NULL;
	objIndex_t radioIid = {0};
	const char *band = NULL;
	const char *stardard = NULL;
	char buf[32] = {0}, mode[64] = {0};
	char *tmp = NULL, *b = NULL, *g = NULL, *n = NULL, *a = NULL, *ac = NULL;
	int len = 0;
	*modeObj = json_object_new_array();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_RADIO, &radioIid, &radioObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		mode[0] = '\0';
		band = json_object_get_string(json_object_object_get(radioObj, "SupportedFrequencyBands"));
		stardard = json_object_get_string(json_object_object_get(radioObj, "SupportedStandards"));
		if(!strcmp(band, "2.4GHz")){
			strcpy(buf, stardard);
			b = strtok_r(buf, ",", &tmp);
			g = strtok_r(tmp, ",", &n);
			if(!strcmp(b, "b")){
				strcat(mode, "802.11");
				strcat(mode,b);
				strcat(mode,",");
			}
			if(!strcmp(g, "g")){
				strcat(mode, "802.11");
				strcat(mode,g);
				strcat(mode,",");
			}
			if(!strcmp(n, "n")){
				strcat(mode, "802.11");
				strcat(mode,n);
				strcat(mode,",");
			}
			if(!strcmp(b, "b") && !strcmp(g, "g")){
				strcat(mode, "802.11");
				strcat(mode,b);
				strcat(mode,"/");
				strcat(mode,g);
				strcat(mode,",");
			}
			if(!strcmp(b, "b") && !strcmp(g, "g") && !strcmp(n, "n")){
				strcat(mode, "802.11");
				strcat(mode,b);
				strcat(mode,"/");
				strcat(mode,g);
				strcat(mode,"/");
				strcat(mode,n);
				strcat(mode,",");
			}
			len = strlen(mode);
			if(len > 0){
				if(mode[len-1] == ',')
					mode[len-1] = '\0';
			}

		}
		else if(!strcmp(band, "5GHz")){
			strcpy(buf, stardard);
			a = strtok_r(buf, ",", &tmp);
			n = strtok_r(tmp, ",", &ac);
			if(!strcmp(a, "a")){
				strcat(mode, "802.11");
				strcat(mode,a);
				strcat(mode,",");
			}
			if(!strcmp(n, "n")){
				strcat(mode, "802.11");
				strcat(mode,n);
				strcat(mode,",");
			}
			if(!strcmp(ac, "ac")){
				strcat(mode, "802.11");
				strcat(mode,ac);
				strcat(mode,",");
			}
			if(!strcmp(a, "a") && !strcmp(n, "n")){
				strcat(mode, "802.11");
				strcat(mode,a);
				strcat(mode,"/");
				strcat(mode,n);
				strcat(mode,",");
			}
			if(!strcmp(a, "a") && !strcmp(n, "n") && !strcmp(ac, "ac")){
				strcat(mode, "802.11");
				strcat(mode,a);
				strcat(mode,"/");
				strcat(mode,n);
				strcat(mode,"/");
				strcat(mode,ac);
				strcat(mode,",");
			}
			len = strlen(mode);
			if(len > 0){
				if(mode[len-1] == ',')
					mode[len-1] = '\0';
			}

		}
		json_object_object_add(obj, "band", json_object_new_string(band));
		json_object_object_add(obj, "mode", json_object_new_string(mode));
		json_object_array_add(*modeObj, obj);

		json_object_put(radioObj);
		radioObj = NULL;
	}

}

zcfgRet_t getwifiSSIDObj(struct json_object **ssidObj){
	struct json_object *wifissidObj = NULL;
	struct json_object *obj = NULL;
	objIndex_t wifissidIid = {0};
	const char *ssid = NULL;

	*ssidObj = json_object_new_array();
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_WIFI_SSID, &wifissidIid, &wifissidObj) == ZCFG_SUCCESS){
		obj = json_object_new_object();
		ssid = json_object_get_string(json_object_object_get(wifissidObj, "SSID"));
		json_object_object_add(obj, "SSID", json_object_new_string(ssid));
		json_object_object_add(obj, "idx", json_object_new_int(wifissidIid.idx[0]));
		json_object_array_add(*ssidObj, obj);
		json_object_put(wifissidObj);
		wifissidObj = NULL;
	}
	return ZCFG_SUCCESS;
}

const char *getLoginUserName(char *userName)
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw)
	{
		strcpy(userName, pw->pw_name);
		return pw->pw_name;
	}
	else{
		strcpy(userName, "");
		return NULL;
	}
}


/*
 * only used for dalcmd, cant be used by httpd, since the httpd user is root.
*/
zcfgRet_t getPrivilegeData(char *loginUserName, char *level, char *list){
	const char *userName = NULL, *userLevel = NULL, *page = NULL;
	objIndex_t iid;
	struct json_object *obj = NULL;
	bool found = false;

	if(getLoginUserName(loginUserName) == NULL)
		return ZCFG_INTERNAL_ERROR;

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNextWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP_ACCOUNT, &iid, &obj) == ZCFG_SUCCESS) {
		userName = json_object_get_string(json_object_object_get(obj, "Username"));
		if(!strcmp(userName, loginUserName)){
			found = true;
			json_object_put(obj);
			break;
		}
		json_object_put(obj);
	}

	if(!found)
		return ZCFG_INTERNAL_ERROR;

	iid.level--;
	iid.idx[iid.level] = 0;
	if(zcfgFeObjJsonGetWithoutUpdate(RDM_OID_ZY_LOG_CFG_GP, &iid, &obj) == ZCFG_SUCCESS){
		userLevel = json_object_get_string(json_object_object_get(obj, "Level"));
		page = json_object_get_string(json_object_object_get(obj, "Page"));
		strcpy(level, userLevel);
		strcpy(list, page);
		json_object_put(obj);
		return ZCFG_SUCCESS;
	}
	else
		return ZCFG_INTERNAL_ERROR;

}

zcfgRet_t zyshPrivilege(char *zyshlist, char *allList){
	char userName[17] = {0}, level[16] = {0}, privilegeList[2049] = {0};
	int i, len = 0, len1 = 0;

	if(getPrivilegeData(userName, level, privilegeList) != ZCFG_SUCCESS)
		return ZCFG_INTERNAL_ERROR;

	for(i=0; dalHandler[i].name != NULL; i++){
		if(dalHandler[i].previlege != NULL && strcmp(dalHandler[i].previlege, "") && (!strcmp(level, "high") || findInList(privilegeList, dalHandler[i].previlege, ","))){
			strcat(zyshlist, dalHandler[i].name);
			strcat(zyshlist, ",");
		}
		strcat(allList, dalHandler[i].name);
		strcat(allList, ",");
	}
	len = strlen(zyshlist);
	if(len>0)
		if(zyshlist[len-1] == ',')
	zyshlist[len-1] = '\0';
	len1 = strlen(allList);
	if(len1>0)
		if(allList[len1-1] == ',')
	allList[len1-1] = '\0';

	return ZCFG_SUCCESS;
}




int isDigitString(char *getString)
{
	while(*getString != '\0'){
		if(!isdigit(*getString)){
			return 0;
		}
		getString++;
	}

	return 1;
}

//find key2 in Jobj2, if it exist add it to Jobj1 as key1
int replaceParam(struct json_object *Jobj1, char *key1, struct json_object *Jobj2, char *key2){
	struct json_object *pramJobj = NULL;

	if(Jobj1 == NULL || Jobj2 == NULL)
		return 0;

	pramJobj = json_object_object_get(Jobj2, key2);
	if(pramJobj){
		json_object_object_add(Jobj1, key1, JSON_OBJ_COPY(pramJobj));
		return 1;
	}
	return 0;
}

/*!
 *  Get parameter value from RDM giving oid, iid, and parameter name
 *
 *  @param[in]     oid        Target oid
 *  @param[in]     objIid     Target iid
 *  @param[in]     key        Target parameter name
 *  @param[out]    value      Parameter value
 *
 *  @return     true  - Get value success
 *              false - Wrong input / Object not found / parameter not found
 */
zcfgRet_t getObjParamStr (zcfg_offset_t oid, objIndex_t *objIid, char *key, char *value)
{
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid;
	struct json_object *Obj = NULL;

	if (key == NULL)
	{
		dbg_printf("%s:%d: key can't be null, please give a specific parameter name \n",__FUNCTION__,__LINE__);
		return ZCFG_REQUEST_REJECT;
	}

	if (value == NULL)
	{
		dbg_printf("%s:%d: value can't be null, please initiate it first \n",__FUNCTION__,__LINE__);
		return ZCFG_REQUEST_REJECT;
	}

	IID_INIT(iid);
	iid.level = 1;
	iid.idx[0] = objIid->idx[0];
	if ((ret = zcfgFeObjJsonGet(oid, &iid, &Obj)) != ZCFG_SUCCESS)
		return ZCFG_NO_SUCH_OBJECT;

	if (!json_object_object_get(Obj, key))
		return ZCFG_NO_SUCH_PARAMETER;

	strcpy(value,json_object_get_string(json_object_object_get(Obj, key)));
	json_object_put(Obj);
	return ret;
}

bool validateEmailAddr(const char *emailAddr){
	char buff[128] = {0}, tmp1[128] = {0}, tmp2[128] = {0}, tmp3[128] = {0};
    char *temp1 = NULL, *temp2 = NULL, *temp3 = NULL;
	int i, count = 0;
    strcpy(buff, emailAddr);
    temp1 = strtok_r(buff, "@", &temp2);
	if(!temp1)
		return false;
	strcpy(tmp1,temp1);
	for(i = 0; tmp1[i] != 0; i++){
		if(tmp1[i] < 45 || tmp1[i] == 47 || (57 < tmp1[i] && tmp1[i] < 65) || (90 < tmp1[i] && tmp1[i] < 95) || tmp1[i] == 96 || tmp1[i] > 122)
        	return false;
    }
    temp3 = strtok_r(temp2, ".", &temp2);
	if(!strcmp(temp2,""))
		return false;
    while(temp3 != NULL){
		if(strlen(temp3) < 2)
			return false;
		strcpy(tmp3,temp3);
		if(count == 0){
			for(i = 0; tmp3[i] != 0; i++){
		        if(tmp3[i] < 45 || tmp3[i] == 46 || tmp3[i] == 47 || (57 < tmp3[i] && tmp3[i] < 65) || (90 < tmp3[i] && tmp3[i] < 95) || tmp3[i] == 96 || tmp3[i] > 122)
					return false;
		    }
		}
		else{
			for(i = 0; tmp3[i] != 0; i++){
		        if(tmp3[i] < 47 || (57 < tmp3[i] && tmp3[i] < 65) || (90 < tmp3[i] && tmp3[i] < 97) || tmp3[i] > 123)
		            return false;
		    }
		}
		count++;
		temp3 = strtok_r(temp2, ".", &temp2);
	}
    return true;
}

#if 1
bool validateMacAddr1(const char *macAddr, const char *delim){
	char buff[32];
    char *p, *tmp;
	int count = 0;

#if 0
	if(strchr(macAddr, '-')){
		delim = "-";
	}
	else if(strchr(macAddr, ':')){
		delim = ":";
	}
	else{
		return false;
	}
#endif

	strcpy(buff, macAddr);
	p = strtok_r(buff, delim, &tmp);
	while(p != NULL){
		if(strlen(p) == 2){
			count++;
			if(count>6){
				return false;
			}
			if((*p<'0' || *p>'9') && (*p<'a' || *p>'f') && (*p<'A' || *p>'F')){
				return false;
			}
			if((*(p+1)<'0' || *(p+1)>'9') && (*(p+1)<'a' || *(p+1)>'f') && (*(p+1)<'A' || *(p+1)>'F')){
				return false;
			}
		}
		else{
			return false;
		}

		p = strtok_r(NULL, delim, &tmp);
	}
	if(count!=6)
		return false;

	return true;
}
#endif

bool validateIPv6(const char *ipStr){

	struct in6_addr ip6addr;
	char *tmp_ptr = NULL, *ip6addr_tmp = NULL;
	char buf[128]={0};

	strcpy(buf, ipStr);
	if(strstr(buf, ",") != NULL)
	{
		ip6addr_tmp = strtok_r(buf, ",", &tmp_ptr);
		while(ip6addr_tmp != NULL)
		{
			if (inet_pton(AF_INET6, ip6addr_tmp, &ip6addr) <= 0)
				return false;

			ip6addr_tmp = strtok_r(NULL, ",", &tmp_ptr);
		}
		return true;
	}

	if (inet_pton(AF_INET6, ipStr, &ip6addr) <= 0)
		return false;

	return true;
}

bool isHexString(char hexStr[]){
	int len = 0, i;

	len = strlen(hexStr);
	if(len > 4)
		return false;
	for(i=0;i<len;i++){
		if((hexStr[i]<'0' || hexStr[i]>'9') && (hexStr[i]<'a' || hexStr[i]>'f') && (hexStr[i]<'A' || hexStr[i]>'F')){
			return false;
		}
	}
	return true;
}

bool IPv6IndentifierType(const char *ipv6indentifiertype){
	char buff[64] = {0};
	char p1[32] = {0}, p2[32] = {0}, p3[32] = {0}, p4[32] = {0};
	int d1, d2, d3, d4;
	strcpy(buff, ipv6indentifiertype);
	if(buff[strlen(ipv6indentifiertype) - 1] == ':')
		return false;
	sscanf(buff, "%[^:]:%[^:]:%[^:]:%s", &p1, &p2, &p3, &p4);
	if(!strcmp(p4,""))
		return false;
	if(isHexString(p1) == false || isHexString(p2) == false || isHexString(p3) == false || isHexString(p4) == false)
		return false;

	return true;
}

bool validateLocalIPv6(const char *ipStr){
	char tmp[5] = {0};
	strncpy(tmp, ipStr, 4);
	if(strcasecmp(tmp, "fe80"))
		return false;
	if(!validateIPv6(ipStr))
		return false;
	return true;
}

bool validateIPv4(const char *ipStr){
	int i, cnt = 0, value;
	const char *cp = ipStr;

	if(ipStr == NULL)
		return false;

	for(; *cp != '\0'; cp++) {
		if (*cp == '.')
			cnt++;
		else if((!isdigit(*cp)) && (*cp != '.')) {
			return false;
		}
	}
	if (cnt !=3)
		return false;

	cnt =0;
	cp = ipStr;
	for(i = 24; i >= 0; i -= 8) {
		if((*cp == '\0') || (*cp == '.'))
			return false;

		value = atoi(cp);
		if((value > 255) || (value < 0))
			return false;

		cnt++;
		if((cp = strchr(cp, '.')) == NULL)
			break;
		cp++;
	}

	if(cnt != 4)
		return false;

	return true;
}

bool validateMacAddr(const char *macaddr){
	char buff[32] = {0};
	char c, d;
	char *macstr = NULL, *tmp = NULL;
	int count = 0;

	strcpy(buff,macaddr);
	macstr = strtok_r(buff, ":", &tmp);
	while(macstr != NULL){
		c = *macstr;
		d = *(macstr+1);
		if(!isxdigit(c) || !isxdigit(d))
			return false;
		macstr = strtok_r(tmp, ":", &tmp);
		count++;
	}
	if(count != 6)
		return false;
	return true;
}

int getLeftMostZeroBitPos(num) {
	int i = 0;
	int numArr[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	for ( i = 0; i < 8; i++ ){
		if((num & numArr[i])== 0)
			return i;
	}
	return 8;
}

int getRightMostOneBitPos(num) {
	int i = 0;
	int numArr[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	int c = 0;
	bool found = false;
	for ( i = 0; i < 8; i++ ){
		c = (num & numArr[i]);
		if(c>>i)
			return (8 - i - 1);
	}
	return -1;
}


bool validMacMask(const char *mask) {
	char buff[32] = {0};
	int zeros = 0;
	int zeroBitPos = 0, oneBitPos = 0;
	bool zeroBitExisted = false;
	char *macstr = NULL, *tmp = NULL;
	int count = 0,num = 0;
	char c, d;
	strcpy(buff,mask);
	macstr = strtok_r(buff, ":", &tmp);
	while(macstr != NULL){
		if(!strcmp(macstr,"00"))
			zeros++;
		c = *macstr;
		d = *(macstr+1);
		if(!isxdigit(c) || !isxdigit(d))
			return false;
		sscanf(macstr,"%x",&num);
		if (zeroBitExisted == true && num != 0)
			return false;
		zeroBitPos = getLeftMostZeroBitPos(num);
		oneBitPos = getRightMostOneBitPos(num);
		if (zeroBitPos < oneBitPos)
			return false;
		if (zeroBitPos < 8)
			zeroBitExisted = true;
		macstr = strtok_r(tmp, ":", &tmp);
		count++;
	}
	if(count != 6)
		return false;
	if(zeros == 6)
		return false;
	return true;
}


bool validateIPv4mask(const char *submask){
	unsigned char oneFound = 0;
	unsigned long int netmask = inet_addr(submask);
	if(!strcmp(submask, "255.255.255.255"))
		return true;
	else if(netmask == -1)
		return false;
	netmask = ntohl(netmask);
	unsigned long int mask;
	for (mask = 1; mask; mask <<= 1){
		if (netmask & mask)
			oneFound++;
		else if(oneFound)
			return false;
	}
	return true;
}

bool validateServerName(const char *servername){
	char buf[128] = {0};
	char *tmp = NULL, *prefix = NULL;
	strcpy(buf,servername);
	prefix = strtok_r(buf, ".", &tmp);
	if(!strcmp(tmp,""))
		return false;
	return true;
}

bool validatetime(const char *time){
	char buf[32] = {0};
	int month = 0, date = 0, hour = 0, minute = 0;

	if(sscanf(time, "%d-%d-%d:%d", &month, &date, &hour, &minute) != 4)
		return false;

	if(month >12 || month < 0)
		return false;
	if(date > 31 || date <1)
		return false;
	if(hour > 23 || hour < 0)
		return false;
	if(minute > 60 || minute <0)
		return false;
	return true;
}



zcfgRet_t insertLoginUserInfo(struct json_object *Jobj, const char *userName, const char *level){
	struct json_object *subJobj;

	if(!Jobj || !userName || !level)
		return ZCFG_INTERNAL_ERROR;

	if(json_object_get_type(Jobj) == json_type_object){
		json_object_object_add(Jobj, DAL_LOGIN_LEVEL, json_object_new_string(level));
		json_object_object_add(Jobj, DAL_LOGIN_USERNAME, json_object_new_string(userName));
		json_object_object_add(Jobj, DAL_LOGIN_TYPE, json_object_new_string("GUI"));
	}
	else if(json_object_get_type(Jobj) == json_type_array){
		subJobj = json_object_array_get_idx(Jobj, 0);
		if(!subJobj)
			return ZCFG_INTERNAL_ERROR;

		json_object_object_add(subJobj, DAL_LOGIN_LEVEL, json_object_new_string(level));
		json_object_object_add(subJobj, DAL_LOGIN_USERNAME, json_object_new_string(userName));
		json_object_object_add(subJobj, DAL_LOGIN_TYPE, json_object_new_string("GUI"));
	}
	else
		return ZCFG_INTERNAL_ERROR;

	return ZCFG_SUCCESS;
}

zcfgRet_t getLoginUserInfo(struct json_object *Jobj, char *userName, char *level){
	struct json_object *subJobj;

	if(!Jobj)
		return ZCFG_INTERNAL_ERROR;

	if(json_object_get_type(Jobj) == json_type_object){
		if(userName) strcpy(userName, json_object_get_string(json_object_object_get(Jobj, DAL_LOGIN_USERNAME)));
		if(level) strcpy(level, json_object_get_string(json_object_object_get(Jobj, DAL_LOGIN_LEVEL)));
	}
	else if(json_object_get_type(Jobj) == json_type_array){
		subJobj = json_object_array_get_idx(Jobj, 0);
		if(!subJobj)
			return ZCFG_INTERNAL_ERROR;

		if(userName) strcpy(userName, json_object_get_string(json_object_object_get(subJobj, DAL_LOGIN_USERNAME)));
		if(level) strcpy(level, json_object_get_string(json_object_object_get(subJobj, DAL_LOGIN_LEVEL)));
	}
	else
		return ZCFG_INTERNAL_ERROR;

	return ZCFG_SUCCESS;
}

bool findDuplicate(char *list, char *duplicate, char *separator){
	char *buf, *ptr, *tmp_ptr = NULL;

	if(list == NULL || separator == NULL)
		return false;

	buf = strdup(list);
	ptr = strtok_r(buf, separator, &tmp_ptr);
	while(ptr != NULL){
		if(findInList(tmp_ptr, ptr, separator)){
			if(duplicate != NULL)
				strcpy(duplicate, ptr);
			free(buf);
			return true;
		}

		ptr = strtok_r(NULL, separator, &tmp_ptr);
	}
	free(buf);
	return false;
}

bool isSameSubNet( char *lanIp, char *lansubnetMask, char *staticIP) {
	unsigned int ip_n[4] = {0}, subnetMask_n[4] = {0}, staticIP_n[4] = {0};
	int count = 0;

	sscanf(lanIp, "%u.%u.%u.%u", &ip_n[3], &ip_n[2], &ip_n[1], &ip_n[0]);
	sscanf(lansubnetMask, "%u.%u.%u.%u", &subnetMask_n[3], &subnetMask_n[2], &subnetMask_n[1], &subnetMask_n[0]);
	sscanf(staticIP, "%u.%u.%u.%u", &staticIP_n[3], &staticIP_n[2], &staticIP_n[1], &staticIP_n[0]);
	for (int i = 0; i < 4; i++) {
		if ((ip_n[i] & subnetMask_n[i]) == (staticIP_n[i] & subnetMask_n[i]))
		  count++;
	}

	if (count == 4)
		return true;
	else
		return false;
}

//support only for char
bool checkValidateParameter (char *ptr, struct json_object **obj, char *key) {
	int len = 0, i;
	struct json_object *tmpObj = NULL;
	char *param = NULL;
	bool found = false;

	len = json_object_array_length(*obj);
	for (i=0; i<len; i++){
		tmpObj = json_object_array_get_idx(*obj, i);
		param = json_object_get_string(json_object_object_get(tmpObj, key));
		if(!strcmp(ptr, param)){
			found = true;
			break;
		}
	}
	if(found)
		return true;
	else
		return false;
}

bool parseValidateParameter(struct json_object *Jobj, char *method, char *handler_name, dal_param_t *paramList, char *invalidParm){
	int i, j, k;
	struct json_object *pramJobj = NULL;
	struct json_object *WanLanListJarray = NULL, *WanLanObj = NULL, *RouteIntfJarray = NULL, *intfObj = NULL, *LanPortListJarray = NULL, *QueueListJarray = NULL, *VoipList = NULL, *certListJarray = NULL;
	struct json_object *obj = NULL, *obj_tmp = NULL, *newObj = NULL;
	const char *stringPtr = NULL, *IpIfacePath = NULL, *Name = NULL, *Type = NULL, *LinkType = NULL, *ConnectionType = NULL, *Group_WAN_IpIface, *Interface = NULL, *BindToIntfGrp = NULL, *LanPortObj = NULL, *IntfPath = NULL, *BrPortPath = NULL, *VoipName = NULL, *timezone = NULL, *timezonevalue = NULL, *Policer = NULL;
	char *ptr = NULL, *ptr_buf = NULL, *tmp_ptr = NULL, *nameList = NULL, *intfName = NULL, *tmp = NULL, *LANName = NULL, *sipname = NULL, *account = NULL, *mode = NULL;
	char buf[64] = {0}, parenCtlServiceList[1024] = {0}, parenCtlService[128], tr181PathList[256] = {0}, route[32] = {0}, duplicateName[128] = {0}, brPath[32] = {0}, intfList[256] = {0}, queue[32] = {0}, voipList[1024] = {0}, usnAccount[256] = {0}, wlmode[16] = {0}, buff[128] = {0};
	int intValue = 0, len = 0, valid = 0, count = 0, len1 = 0, Num = 0, CLIidx = 0, icmptye = 0, icmptypecode = 0, def = 0;
	bool ret = true, changetype = false, foundIntf = false, RtWanWWANDefault = false;
	bool AtmUsed = false, PtmUsed = false, EthUsed = false, PonUsed = false, UsbUsed = false, CellUsed = false, ReUsed = false, wanIntf = false, found = false;
	char *macAddrDelim = NULL; // MAC address delimiter
	char allDays[] = "Mon,Tue,Wed,Thu,Fri,Sat,Sun";
	int startHour, startMin, stopHour, stopMin;
	char Days[32] = {0};
	unsigned char brIndex;

	if(Jobj == NULL || paramList == NULL)
		return true;

	if(json_object_object_get(Jobj, "cmdtype")){	// GUI show Jobj, but CLI cannot show Jobj. Rex add 20170918
		if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI"))
	dbg_printf("Jobj=%s\n", json_object_to_json_string(Jobj));
	}

	for(i=0;paramList[i].paraName;i++){

        if(invalidParm != NULL)
        {   strcpy(invalidParm, paramList[i].paraName);   }

        if((pramJobj = json_object_object_get(Jobj, paramList[i].paraName)) != NULL){
			if(paramList[i].validate != NULL){
				ret = paramList[i].validate(paramList[i].type);
				return ret;
			}
			else{
				switch(paramList[i].type){
					case dalType_string:
						stringPtr = json_object_get_string(pramJobj);
						len = strlen(stringPtr);
						if(paramList[i].enumeration != NULL){
							sprintf(buf, "%s", stringPtr);
							if(findInList(paramList[i].enumeration, buf, "|") == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						if(paramList[i].min || paramList[i].max){ // if min ==0 and max ==0, skip
							if(len<paramList[i].min || len>paramList[i].max){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						break;
					case dalType_int:
						if(json_object_get_type(pramJobj) == 9){
							stringPtr = json_object_get_string(pramJobj);
							intValue = atoi(stringPtr);
							changetype = true;
						}
						else
						intValue = json_object_get_int(pramJobj);

						if(paramList[i].enumeration != NULL){
							sprintf(buf, "%d", intValue);
							if(findInList(paramList[i].enumeration, buf, "|") == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						else if(paramList[i].min || paramList[i].max){
							if(intValue<paramList[i].min || intValue>paramList[i].max){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						if(changetype){
							json_object_object_del(Jobj, paramList[i].paraName);
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_int(intValue));
							changetype = false;
						}
						break;
					case dalType_boolean:
						if(json_object_get_type(pramJobj) == 9){
							stringPtr = json_object_get_string(pramJobj);
							intValue = atoi(stringPtr);
							if(strcmp(stringPtr, "0") && strcmp(stringPtr, "1")){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								printf("Available value is <0|1>\n");
								return false;
							}
							changetype = true;
						}
						else
						intValue = json_object_get_boolean(pramJobj);

						if(changetype){
							json_object_object_del(Jobj, paramList[i].paraName);
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_boolean(intValue));
							changetype = false;
						}
						break;
					case dalType_v4Addr:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateIPv4(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_v4Mask:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateIPv4mask(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_v6Addr:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateIPv6(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_v6AddrPrefix: //ex. 2001:123::11:22:33/64
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						strcpy(buf, stringPtr);
						ptr = strchr(buf, '/');
						*ptr = '\0';
						intValue = atoi(ptr+1);
						if(intValue<0 || intValue>128){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						if(validateIPv6(buf) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_v4v6Addr:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateIPv4(stringPtr) == false && validateIPv6(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_v4v6AddrDomainName:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateIPv4(stringPtr) == false && validateIPv6(stringPtr) == false && validateServerName(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_PortRange:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						else{
							int startPort, endPort;
							ptr_buf = strdup(stringPtr);
							ptr = strchr(ptr_buf, ':');
							if(ptr != NULL){
								*ptr = '\0';
								if(!strcmp(ptr_buf, "") || (strcmp(ptr_buf, "-1") && !isDigitString(ptr_buf))){
									printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
									free(ptr_buf);
									return false;
								}
								if(!strcmp(ptr+1, "") || (strcmp(ptr+1, "-1") && !isDigitString(ptr+1))){
									printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
									free(ptr_buf);
									return false;
								}
								sscanf(stringPtr, "%d:%d", &startPort, &endPort);
								if(startPort == -1){
									endPort = -1;
								}
							}
							else{
								if(!strcmp(ptr_buf, "") || (strcmp(ptr_buf, "-1") && !isDigitString(ptr_buf))){
									printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
									free(ptr_buf);
									return false;
								}
								startPort = atoi(stringPtr);
								endPort = -1;
							}
							free(ptr_buf);
							if(startPort == -1){
								//pass case, do nothing
							}
							else if(startPort < -1 || startPort > 65535){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								printf("Start port must between -1 and 65535\n");
								return false;
							}
							else if(endPort < -1 || endPort > 65535){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								printf("End port must between -1 and 65535\n");
								return false;
							}
							else if(endPort != -1 && (startPort >= endPort)){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								printf("End port must larger than start port\n");
								return false;
							}
						}
						break;
					case dalType_vpivci:
						valid = 1;
						stringPtr = json_object_get_string(pramJobj);
						ptr_buf = strdup(stringPtr);
						ptr = strchr(ptr_buf, '/');
						if(ptr == NULL){
							valid = 0;
						}

						if(ptr){
							int vpi, vci;
							*ptr = '\0';
							if(!strcmp(ptr_buf, "") || !isDigitString(ptr_buf)){
								valid = 0;
							}
							if(!strcmp(ptr+1, "") || !isDigitString(ptr+1)){
								valid = 0;
							}
							sscanf(stringPtr, "%d/%d", &vpi, &vci);
							if(vpi<0 || vpi>255 || vci<32 || vci>65535){
								valid = 0;
							}
						}
						free(ptr_buf);
						if(!valid){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
						case dalType_IPPrefix:
							stringPtr = json_object_get_string(pramJobj);
							if(paramList[i].enumeration != NULL){
								if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
									break;
								}
							ptr_buf = strdup(stringPtr);
							ptr = strchr(ptr_buf, '/');
							if(ptr == NULL || !strcmp(ptr+1, "") || !isDigitString(ptr+1)){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								free(ptr_buf);
								return false;
							}
							*ptr = '\0';
							intValue = atoi(ptr+1);
							if(strchr(ptr_buf, ':') != NULL){ //IPv6 Prefix
								if(intValue<0 || intValue>128 || validateIPv6(ptr_buf) == false){
									printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
									free(ptr_buf);
									return false;
								}
							}
							else{ //IPv4 Prefix
								if(intValue<0 || intValue>32 || validateIPv4(ptr_buf) == false){
									printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
									free(ptr_buf);
									return false;
								}
							}
							free(ptr_buf);
							break;
					case dalType_emailAddr:
					case dalType_emailList:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateEmailAddr(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_MacAddr:
						macAddrDelim = ":";
					case dalType_MacAddrHyphen:
						macAddrDelim = "-";
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;

						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						if(validateMacAddr1(stringPtr, macAddrDelim) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_MacAddrList:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						ptr_buf = strdup(stringPtr);
						count = 0;
						ptr = strtok_r(ptr_buf, ",", &tmp_ptr);
						while(ptr != NULL){
							if(validateMacAddr1(ptr, ":") == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								free(ptr_buf);
								return false;
							}
							count++;
							ptr = strtok_r(NULL, ",", &tmp_ptr);
						}
						free(ptr_buf);
						if(paramList[i].min || paramList[i].max){
							if((count > paramList[i].max) || (count < paramList[i].min)){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						break;
					case dalType_DayofWeek:
						Days[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						ptr_buf = strdup(stringPtr);
						count = 0;
						ptr = strtok_r(ptr_buf, ",", &tmp_ptr);
						while(ptr != NULL){
							if(findInList(allDays, ptr, ",") == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								free(ptr_buf);
								return false;
							}
							if(findInList(Days, ptr, ",") == false){ //prevent duplicate case
								count++;
								strcat(Days, ptr);
								strcat(Days, ",");
							}
							ptr = strtok_r(NULL, ",", &tmp_ptr);
						}
						free(ptr_buf);
						if(count == 0){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						Days[strlen(Days)-1]='\0';
						break;
					case dalType_TimeRangeList:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						ptr_buf = strdup(stringPtr);
						count = 0;
						ptr = strtok_r(ptr_buf, ",", &tmp_ptr);
						while(ptr != NULL){
							if(parseTimeRange(ptr, &startHour, &startMin, &stopHour, &stopMin) == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								free(ptr_buf);
								return false;
							}
							count++;
							ptr = strtok_r(NULL, ",", &tmp_ptr);
						}
						free(ptr_buf);
						if(paramList[i].min || paramList[i].max){
							if((count > paramList[i].max) || (count < paramList[i].min)){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								printf("The number of time range must between %d and %d\n", paramList[i].min, paramList[i].max);
								return false;
							}
						}
						else if(count == 0){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
						}
						break;
					case dalType_URLFilterList:
						stringPtr = json_object_get_string(pramJobj);
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						len = strlen(stringPtr);
						if(paramList[i].min || paramList[i].max){ // if min ==0 and max ==0, skip
							if(len<paramList[i].min || len>paramList[i].max){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						break;
					case dalType_ParenCtlServiceList:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true)
								break;
						}
						ptr_buf = strdup(stringPtr);
						count = 0;
						memset(parenCtlServiceList, 0, sizeof(parenCtlServiceList));
						ptr = strtok_r(ptr_buf, "/", &tmp_ptr);
						while(ptr != NULL){
							if(parseParenService(ptr, parenCtlService) == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								free(ptr_buf);
								return false;
							}
							else{
								json_object_object_del(Jobj, "NetworkServiceList");
								json_object_object_add(Jobj, "NetworkServiceList", json_object_new_string(parenCtlService));
							}

							count++;
							if(count>paramList[i].max)
								break;
							ptr = strtok_r(NULL, "/", &tmp_ptr);
						}
						free(ptr_buf);
						if(paramList[i].max || paramList[i].min){
							if(count > paramList[i].max || count < paramList[i].min){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						else if(count == 0){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_LanRtWanWWan:
					case dalType_LanRtWan:
					case dalType_RtWanWWan:
					case dalType_MultiRtWanWWan:
					case dalType_RtWan:
					case dalType_BrWan:
					case dalType_Lan:
					case dalType_LanName:
						tr181PathList[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(!strncmp("IP.Interface.", stringPtr, 13)){
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
							break;
						}
						if(stringPtr == NULL || !strcmp(stringPtr, ""))
							break;
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true){
								if(paramList[i].type == dalType_RtWanWWan && !strcmp(stringPtr, "Default")){
									json_object_object_del(Jobj, paramList[i].paraName);
									json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(""));
								}
								break;
							}
						}
						if(WanLanListJarray == NULL)
							getWanLanList(&WanLanListJarray);
						if(paramList[i].type == dalType_MultiRtWanWWan){
							len = json_object_array_length(WanLanListJarray);
							nameList = strdup(stringPtr);
							intfName = strtok_r(nameList, ",", &tmp_ptr);

							while(intfName != NULL){
								for(j=0; j<len; j++){
									WanLanObj = json_object_array_get_idx(WanLanListJarray, j);
									IpIfacePath = json_object_get_string(json_object_object_get(WanLanObj, "IpIfacePath"));
									Name = json_object_get_string(json_object_object_get(WanLanObj, "Name"));
									if(!strcmp(intfName,Name)){
										strcat(tr181PathList,IpIfacePath);
										strcat(tr181PathList,",");
									}
								}
								intfName = strtok_r(tmp_ptr, ",", &tmp_ptr);
							}
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(tr181PathList));
							free(nameList);
							break;
						}

						foundIntf = false;
						len = json_object_array_length(WanLanListJarray);
						for(j=0; j<len; j++){
							WanLanObj = json_object_array_get_idx(WanLanListJarray, j);
							IpIfacePath = json_object_get_string(json_object_object_get(WanLanObj, "IpIfacePath"));
							Name = json_object_get_string(json_object_object_get(WanLanObj, "Name"));
							Type = json_object_get_string(json_object_object_get(WanLanObj, "Type"));
							LinkType = json_object_get_string(json_object_object_get(WanLanObj, "LinkType"));
							ConnectionType = json_object_get_string(json_object_object_get(WanLanObj, "ConnectionType"));
							if(paramList[j].type == dalType_LanRtWanWWan){
								if(!strcmp(Type, "WAN") && !strcmp(ConnectionType, "IP_Bridged"))
									continue;
							}
							else if(paramList[j].type == dalType_LanRtWan){
								if(!strcmp(Type, "WAN") && !strcmp(ConnectionType, "IP_Bridged"))
									continue;
								if(!strcmp(Type, "WAN") && !strcmp(LinkType, "USB"))
									continue;
							}
							else if(paramList[j].type == dalType_RtWanWWan){
								if(!strcmp(Type, "LAN"))
									continue;
								if(!strcmp(Type, "WAN") && !strcmp(ConnectionType, "IP_Bridged"))
									continue;
							}
							else if(paramList[j].type == dalType_RtWan){
								if(!strcmp(Type, "LAN"))
									continue;
								if(!strcmp(Type, "WAN") && !strcmp(ConnectionType, "IP_Bridged"))
									continue;
								if(!strcmp(Type, "WAN") && !strcmp(LinkType, "USB"))
									continue;
							}
							else if(paramList[j].type == dalType_BrWan){
								if(!strcmp(Type, "LAN"))
									continue;
								if(!strcmp(Type, "WAN") && !strcmp(ConnectionType, "IP_Routed"))
									continue;
							}
							else if(paramList[j].type == dalType_Lan || paramList[j].type == dalType_LanName){
								if(!strcmp(Type, "WAN"))
									continue;
							}
							if(!strcmp(Name, stringPtr)){
								foundIntf = true;
								if(paramList[j].type == dalType_LanName){
									json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								}
								else{
									if(strstr(handler_name,"nat")!= NULL){
										if(!strcmp(stringPtr, "Default")){
											json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(""));
										}
										else{
											json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(IpIfacePath));
										}
									}
									else{
										json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(IpIfacePath));
									}
								}
							}
						}
						json_object_put(WanLanListJarray);
						WanLanListJarray = NULL;
						if(!foundIntf){
							printf("Can't find inerface: parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_RouteIntf:	// froe here do not check
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						intfName = strdup(stringPtr);
						if(RouteIntfJarray == NULL)
							getRouteIntfList(&RouteIntfJarray);
						foundIntf = false;
						len = json_object_array_length(RouteIntfJarray);
						for(j=0; j<len; j++){
							intfObj= json_object_array_get_idx(RouteIntfJarray, j);
							Name = json_object_get_string(json_object_object_get(intfObj, "Name"));
							Num = json_object_get_int(json_object_object_get( intfObj, "Number"));
							if(!strcmp(Name, intfName)){
								foundIntf = true;
								sprintf(route,"%d",Num);
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(route));
								break;
							}
						}
						if(!foundIntf){
							if(!strcmp(intfName, "Unchange")){
								Num = 0;
								sprintf(route,"%d",Num);
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(route));
							}
							else{
								printf("Can't find inerface: parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						free(intfName);
						json_object_put(RouteIntfJarray);
						RouteIntfJarray = NULL;
						break;
					case dalType_IntfGrpWanList:
						tr181PathList[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(!strncmp("IP.Interface.", stringPtr, 13)){
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
						}
						nameList = strdup(stringPtr);
						if(findDuplicate(nameList, duplicateName, ",")){
							printf("Duplicate WAN interface name %s\n", duplicateName);
							free(nameList);
							return false;
						}
						Group_WAN_IpIface = NULL;
						getWanLanList(&WanLanListJarray);
						obj = json_object_new_array();
						zcfgFeDalIntrGrpGet(NULL, obj, NULL);
						if(!strcmp(method,"edit")){
							len = json_object_array_length(WanLanListJarray);
							CLIidx = json_object_get_int(json_object_object_get(Jobj, "Index"));
							for(k = 0; k<json_object_array_length(obj); k++){
								obj_tmp = json_object_array_get_idx(obj, k);
								if(CLIidx == json_object_get_int(json_object_object_get(obj_tmp, "Index")))
									Interface = json_object_get_string(json_object_object_get(obj_tmp, "wanIntf"));
							}
							for(j=0;j<len;j++){
								WanLanObj = json_object_array_get_idx(WanLanListJarray, j);
								if(!strcmp(nameList,json_object_get_string(json_object_object_get(WanLanObj, "Name")))){
									if(json_object_get_boolean(json_object_object_get(WanLanObj, "BindToIntfGrp"))){
										if(findInList(Interface, json_object_get_string(json_object_object_get(WanLanObj, "IpIfacePath")),",")){
											wanIntf = true;
											continue;
										}
										printf("wanIntf error.\n");
										return false;
									}
								}
							}
					 	}
						intfName = strtok_r(nameList, ",", &tmp_ptr);
						while(intfName != NULL){
							if(*intfName != '\0'){
								foundIntf = false;
								len = json_object_array_length(WanLanListJarray);
								for(j=0; j<len; j++){
									WanLanObj = json_object_array_get_idx(WanLanListJarray, j);
									IpIfacePath = json_object_get_string(json_object_object_get(WanLanObj, "IpIfacePath"));
									Name = json_object_get_string(json_object_object_get(WanLanObj, "Name"));
									Type = json_object_get_string(json_object_object_get(WanLanObj, "Type"));
									LinkType = json_object_get_string(json_object_object_get(WanLanObj, "LinkType"));
									BindToIntfGrp = json_object_get_boolean(json_object_object_get( WanLanObj, "BindToIntfGrp"));

									if(!findInList(Group_WAN_IpIface, IpIfacePath, ",")){
										if(!strcmp(Type, "LAN"))
											continue;
										if(!strcmp(method,"edit")){
											if(!strcmp(Type, "WAN") && BindToIntfGrp){
												if(!findInList(Interface,IpIfacePath,","))
													continue;
											}
										}
										else if(!strcmp(method,"add")){
										if(!strcmp(Type, "WAN") && BindToIntfGrp)
											continue;
										}
									}
									if(!strcmp(Name, intfName)){
										if(!strcmp(LinkType, "ATM")){
											if(!AtmUsed) AtmUsed = 1;
											else ReUsed = true;
										}
										else if(!strcmp(LinkType, "PTM")){
											if(!PtmUsed) PtmUsed = 1;
											else ReUsed = true;
										}
										else if(!strcmp(LinkType, "ETH")){
											if(!EthUsed) EthUsed = 1;
											else ReUsed = true;
										}
										else if(!strcmp(LinkType, "PON")){
											if(!PonUsed) PonUsed = 1;
											else ReUsed = true;
										}
										else if(!strcmp(LinkType, "USB")){
											if(!UsbUsed) UsbUsed = 1;
											else ReUsed = true;
										}
										else if(!strcmp(LinkType, "CELL")){
											if(!CellUsed) CellUsed = 1;
											else ReUsed = true;
										}

										if(ReUsed){
											printf("Only allow one WAN interface for each link type.");
											ret = false;
										}

										foundIntf = true;
										strcat(tr181PathList, IpIfacePath);
										strcat(tr181PathList, ",");
									}
								}
								if(wanIntf)
									foundIntf = true;
								if(!foundIntf){
									printf("Can't find inerface: %s\n", intfName);
									ret = false;
								}
							}
							intfName = strtok_r(NULL, ",", &tmp_ptr);
						}
						free(nameList);
						json_object_put(WanLanListJarray);
						WanLanListJarray = NULL;
						len = strlen(tr181PathList);
						if(len>0){
							if(tr181PathList[len-1] == ',')
								tr181PathList[len-1] = '\0';
						}
						if(!ret)
							return ret;

						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(tr181PathList));
						break;
					case dalType_IntfGrpLanPortList:
						tr181PathList[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(!strncmp("Bridging.Bridge.", stringPtr, 16)){
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
							break;
						}
						nameList = strdup(stringPtr);
						if(findDuplicate(nameList, duplicateName, ",")){
							printf("Duplicate LAN port name %s\n", duplicateName);
							free(nameList);
							return false;
						}
						getLanPortList(&LanPortListJarray);
						if(!strcmp(method, "edit")){
							brIndex = json_object_get_int(json_object_object_get(Jobj, "Index"));
							sprintf(brPath, "Bridging.Bridge.%u.", brIndex);
						}
						intfName = strtok_r(nameList, ",", &tmp_ptr);
						while(intfName != NULL){
							if(*intfName != '\0'){
								foundIntf = false;
								len = json_object_array_length(LanPortListJarray);
								for(k=0;k<len;k++){
									LanPortObj = json_object_array_get_idx(LanPortListJarray, k);
									IntfPath = json_object_get_string(json_object_object_get(LanPortObj, "IntfPath"));
									BrPortPath = json_object_get_string(json_object_object_get(LanPortObj, "BrPortPath"));
									Name = json_object_get_string(json_object_object_get(LanPortObj, "Name"));

									if(!strcmp(method, "edit") && !strncmp(brPath, BrPortPath, strlen(brPath)) && !strcmp(Name, intfName)){
										foundIntf = true;
										strcat(tr181PathList, BrPortPath);
										strcat(tr181PathList, ",");
									}
									else if(!strcmp(Name, intfName) && !strncmp(BrPortPath, "Bridging.Bridge.1.", 18)){
										foundIntf = true;
										strcat(tr181PathList, BrPortPath);
										strcat(tr181PathList, ",");
									}
								}

								if(!foundIntf){
									printf("Can't find inerface: %s\n", intfName);
									ret = false;
								}

							}
							intfName = strtok_r(NULL, ",", &tmp_ptr);
						}
						free(nameList);
						json_object_put(LanPortListJarray);
						LanPortListJarray = NULL;
						len = strlen(tr181PathList);
						if(len>0){
							if(tr181PathList[len-1] == ',')
								tr181PathList[len-1] = '\0';
						}
						if(!ret){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return ret;
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(tr181PathList));
						break;
					case dalType_LanPortListBr0NoWiFi:
						stringPtr = json_object_get_string(pramJobj);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_IntfGrpLanPortListNoWiFi:
						intfList[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						ptr_buf = strdup(stringPtr);
						nameList = strtok_r(ptr_buf, ",", &tmp_ptr);
						while(nameList != NULL){
							tmp = strtok_r(nameList, ".", &LANName);
							strcat(intfList, nameList);
							strcat(intfList, ",");
							nameList = strtok_r(tmp_ptr, ",", &tmp_ptr);
						}
						if(strlen(intfList) > 0){
							if(intfList[strlen(intfList)-1] == ",")
								intfList[strlen(intfList)-1] == "\0";
						}
						free(ptr_buf);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(intfList));
						break;
					case dalType_ClassIntf:
						tr181PathList[0] = '\0';
						ret = true;
						stringPtr = json_object_get_string(pramJobj);
						if(!strncmp("Bridging.Bridge.", stringPtr, 16)){
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
							break;
						}
						intfName = strdup(stringPtr);
						getLanPortList(&LanPortListJarray);
						if(intfName != NULL){
							foundIntf = false;
							len = json_object_array_length(LanPortListJarray);
							for(k=0;k<len;k++){
								LanPortObj = json_object_array_get_idx(LanPortListJarray, k);
								IntfPath = json_object_get_string(json_object_object_get(LanPortObj, "IntfPath"));
								BrPortPath = json_object_get_string(json_object_object_get(LanPortObj, "BrPortPath"));
								Name = json_object_get_string(json_object_object_get(LanPortObj, "Name"));
								if(!strcmp(Name, intfName)){
									foundIntf = true;
									strcat(tr181PathList, BrPortPath);
									break;
								}
							}
							if(!foundIntf){
								if(!strcmp("LAN",intfName))
									strcat(tr181PathList, intfName);
								else if(!strcmp("Local",intfName) || !strcmp("lo",intfName))
									strcat(tr181PathList, "lo");
								else{
									printf("Can't find inerface: %s\n", intfName);
									ret = false;
								}
							}
						}

						free(intfName);
						json_object_put(LanPortListJarray);
						LanPortListJarray = NULL;
						if(!ret){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return ret;
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(tr181PathList));
						break;
					case dalType_ClassList:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
							}
						}
						getClassList(&obj);
						newObj = json_object_new_array();
						len = json_object_array_length(obj);
						for(k=0;k<len;k++){
							obj_tmp = json_object_array_get_idx(obj, k);
							Policer= json_object_get_string(json_object_object_get(obj_tmp, "Policer"));
							if(!strcmp(Policer, "")){
								json_object_array_add(newObj, obj_tmp);
							}
						}
						if(checkValidateParameter(stringPtr, &newObj, "Name") == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								json_object_put(obj);
								ret = false;
							}
						break;
					case dalType_QueueList:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						intfName = strdup(stringPtr);
						getQueueList(&QueueListJarray);
						if(intfName != NULL){
							foundIntf = false;
							len = json_object_array_length(QueueListJarray);
							for(k=0;k<len;k++){
								obj = json_object_array_get_idx(QueueListJarray, k);
								Name = json_object_get_string(json_object_object_get(obj, "Name"));
								if(!strcmp(Name, intfName)){
									foundIntf = true;
									Num = json_object_get_int(json_object_object_get(obj, "QueueIdx"));
									sprintf(queue,"%d",Num);
									break;
								}
							}
							if(!foundIntf){
#ifdef ZYXEL_WEB_GUI_ECONET_QoS_STYLE
								printf("Can't find inerface: %s\n", intfName);
								ret = false;
#else
								if(!strcmp("default_queue",intfName)){
									def = 1;
									Num = 0;
									sprintf(queue,"%d",Num);
								}
								else{
									printf("Can't find inerface: %s\n", intfName);
									ret = false;
								}
#endif
							}
						}

						free(intfName);
						json_object_put(QueueListJarray);
						QueueListJarray = NULL;
						if(!ret && def == 0)
							return ret;
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(queue));
						break;
					case dalType_VoipRegion:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						for(j = 0; dalVoipRegion[j].Region != NULL; j++){
							if(!strcmp(dalVoipRegion[j].Region,stringPtr)){
								Name = dalVoipRegion[j].simplifyRegion;
								found = true;
								break;
							}
						}
						if(found)
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(Name));
						else{
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						break;
					case dalType_VoipLine:
					case dalType_MultiVoipLine:
						voipList[0] = '\0';
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						ptr_buf = strdup(stringPtr);
						getVoipMultiObj(&VoipList);
						len = json_object_array_length(VoipList);
						nameList = strtok_r(ptr_buf, ",", &tmp_ptr);
						if(paramList[i].type == dalType_VoipLine){
							if(strcmp(tmp_ptr,"")){
								printf("Too many input parameter of %s\n", paramList[i].paraName);
								return false;
							}
						}
						while(nameList != NULL){
							for(j=0;j<len;j++){
								obj = json_object_array_get_idx(VoipList, j);
								if(!strcmp(nameList,json_object_get_string(json_object_object_get(obj, "SIPNumber")))){
									if(paramList[i].type == dalType_VoipLine){
										VoipName = json_object_get_string(json_object_object_get(obj, "VoipLine"));
									}
									else if(paramList[i].type == dalType_MultiVoipLine){
										VoipName = json_object_get_string(json_object_object_get(obj, "BoundList"));
									}
									strcat(voipList, VoipName);
									strcat(voipList, ",");
									break;
								}
							}
							nameList = strtok_r(tmp_ptr, ",", &tmp_ptr);
						}

						if(voipList[strlen(voipList) - 1] == ',')
							voipList[strlen(voipList) - 1] = '\0';
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(voipList));
						break;
					case dalType_VoipProvider:
						stringPtr = json_object_get_string(pramJobj);
						getVoipProviderObj(&VoipList);
						if(checkValidateParameter(stringPtr, &VoipList, "VoipServiceProvider") == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							json_object_put(VoipList);
							return NULL;
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_ICMPv6:
						stringPtr = json_object_get_string(pramJobj);
						for(k = 0; dalFirewallICMP6[k].type != NULL; k++){
							if(!strcmp(dalFirewallICMP6[k].type,stringPtr)){
								icmptye= atoi(dalFirewallICMP6[k].icmptype);
								icmptypecode = atoi(dalFirewallICMP6[k].icmptypecode);
							}
						}
						json_object_object_add(Jobj, "ICMPTypeCode", json_object_new_int(icmptypecode));
						json_object_object_add(Jobj, "ICMPType", json_object_new_int(icmptye));
						break;
					case dalType_LocalCert:
						stringPtr = json_object_get_string(pramJobj);
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						foundIntf = false;
						if(!strcmp(stringPtr, "")){
							foundIntf = true;
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						}
						else{
							getCertList(&certListJarray);
							len = json_object_array_length(certListJarray);
							for(j=0; j<len; j++){
								obj = json_object_array_get_idx(certListJarray, k);
								Name = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_Name"));
								Type = json_object_get_string(json_object_object_get(obj, "X_ZYXEL_Type"));
								if(!strcmp(Name, stringPtr) && !strcmp(Type, "signed")){
									foundIntf = true;
									json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
									break;
								}
							}
							json_object_put(certListJarray);
							certListJarray = NULL;
						}
						if(!foundIntf){
							printf("Can't find local certificate: %s\n", stringPtr);
							return false;
						}
						break;
					case dalType_timezone:
						stringPtr = json_object_get_string(pramJobj);
						for(k = 0; dalTimeZone[k].timezoneCLI != NULL; k++){
							if(!strcmp(dalTimeZone[k].timezoneCLI,stringPtr)){
								timezone= dalTimeZone[k].timezone;
								timezonevalue = dalTimeZone[k].timezonevalue;
								found = true;
								break;
							}
						}
						if(found){
						json_object_object_add(Jobj, "X_ZYXEL_TimeZone", json_object_new_string(timezonevalue));
						json_object_object_add(Jobj, "X_ZYXEL_Location", json_object_new_string(timezone));
						}
						else{
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}

						break;
					case dalType_HourMin:
						stringPtr = json_object_get_string(pramJobj);
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(validatetime(stringPtr) == false){
							printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_serveraddr:
						stringPtr = json_object_get_string(pramJobj);
						if(paramList[i].enumeration != NULL){
							if(findInList(paramList[i].enumeration, stringPtr, "|") == true){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						if(strcmp(stringPtr, "None")){
							if(validateServerName(stringPtr) == false){
								printf("Wrong parameter value <%s> of %s\n", stringPtr, paramList[i].paraName);
								return false;
							}
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_sambafolder:
						stringPtr = json_object_get_string(pramJobj);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_usrAccount:
						stringPtr = json_object_get_string(pramJobj);
						ptr_buf = strdup(stringPtr);
						account = strtok_r(ptr_buf, ",", &tmp);
						while(account != NULL){
							strcat(usnAccount, account);
							strcat(usnAccount, " ");
							account = strtok_r(NULL, ",", &tmp);
						}
						free(ptr_buf);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(usnAccount));
						break;
					case dalType_usbInfo:
						stringPtr = json_object_get_string(pramJobj);
						if(strstr(stringPtr, "..")){
							printf("Wrong usb path value <%s> of %s\n", stringPtr, paramList[i].paraName);
							return false;
						}
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_intfVlan:
						stringPtr = json_object_get_string(pramJobj);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					case dalType_wlanMode:
						stringPtr = json_object_get_string(pramJobj);
						wlmode[0] = '\0';
						if(json_object_object_get(Jobj, "cmdtype")){
							if(!strcmp(json_object_get_string(json_object_object_get(Jobj, "cmdtype")),"GUI")){
								json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
								break;
							}
						}
						ptr_buf = strdup(stringPtr);
						strcpy(buff, ptr_buf);
						if(strstr(&buff[6],"/")){
							strcat(wlmode, "11");
							mode = strtok_r(&buff[6], "/",&tmp);
							while(mode != NULL){
								strcat(wlmode, mode);
								mode = strtok_r(NULL, "/", &tmp);
							}
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(wlmode));
						}
						else
							json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(&buff[4]));
						free(ptr_buf);
						break;
					case dalType_wifiSSID:
						stringPtr = json_object_get_string(pramJobj);
						json_object_object_add(Jobj, paramList[i].paraName, json_object_new_string(stringPtr));
						break;
					default:
						break;
				}
			}
		}
	}

	return true;
}

/*
 * input:handlerName,  method, Jobj
 * output: Jarray, replyMsg
*/
zcfgRet_t zcfgFeDalHandler(const char *handlerName, const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int i = 0, found = 0;

	if(handlerName == NULL)
		return ret;

	for(i=0; dalHandler[i].name != NULL; i++){
		if(!strcmp(handlerName, dalHandler[i].name)){
			found =1;
			break;
		}
	}

	printf("handlerName=%s method=%s i=%d\n", handlerName, method, i);

	if(!found){
		printf("Unknown handlerName:%s\n", handlerName);
		return ret;
	}
	if(parseValidateParameter(Jobj, method, dalHandler[i].name, dalHandler[i].parameter,replyMsg) == false){
		return ZCFG_INVALID_PARAM_VALUE;
	}

	if(dalHandler[i].handler != NULL)
		ret = dalHandler[i].handler(method, Jobj, Jarray, replyMsg);
	return ret;
}


uint32_t CharToIntConv(char chr) {
	uint32_t res = 0;

	if(('0' <= chr) && (chr <= '9')) {
		res = (chr)-0x30;
	}
	else {
		printf("CharToIntConv Error: inupt is not from 0~9, input = %c\n", chr);
	}

	return res;
}

uint32_t StrToIntConv(char *str) {
	uint32_t res = 0;
	short len = strlen(str);
	short i = 0;
	uint32_t unit = 1;

	//printf("StrToIntConv: %s, len = %d\n", str, len);
	for(i=len-1; i>=0; i--) {
		res += ((CharToIntConv(str[i]))*unit);
		unit*=10;
	}

	return res;
}

/*
Function Name: ip4Compare
Description  : Compare 2 IP strings
Returns      : 0 (ip1 = ip2), 1 (ip1 < ip2), 2 (ip1 > ip2), -1 error*/
int ip4Compare(char *ip1, char *ip2)
{
	int i, ret=0;
	char ipaddr1[20] = {0}, ipaddr2[20] = {0};
	int ip11=0,ip12=0,ip13=0,ip14=0;
	int ip21=0,ip22=0,ip23=0,ip24=0;

	if (ip1==NULL || ip2==NULL)
		return -1;

	strcpy(ipaddr1, ip1);
	strcpy(ipaddr2, ip2);
	sscanf(ipaddr1, "%d.%d.%d.%d", &ip11, &ip12, &ip13, &ip14);
	sscanf(ipaddr2, "%d.%d.%d.%d", &ip21, &ip22, &ip23, &ip24);

	if (ip11<ip21)
		return 1;
	else if (ip11>ip21)
		return 2;

	if (ip12<ip22)
		return 1;
	else if (ip12>ip22)
		return 2;

	if (ip13<ip23)
		return 1;
	else if (ip13>ip23)
		return 2;

	if (ip14<ip24)
		return 1;
	else if (ip14>ip24)
		return 2;
	return 0;
}
