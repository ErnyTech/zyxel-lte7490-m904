#define ZHTTP_OID_DICTIONARY_NUM 602

typedef struct _zHttpOidParallel{
	char* abbreviation;
	zcfg_offset_t oid;
}zHttpOidParallel;

zHttpOidParallel oidDictionary[] = {
	{"RDM_OID_ADD_CERTIFICATE",		128724},
	{"RDM_OID_ANI",		145524},
	{"RDM_OID_APP_SUPPORT_INFO",		18180},
	{"RDM_OID_ATM",		35940},
	{"RDM_OID_ATM_DIAG",		37188},
	{"RDM_OID_ATM_DIAG_F4_LO",		37524},
	{"RDM_OID_ATM_DIAG_F5_LO",		37224},
	{"RDM_OID_ATM_LINK",		36000},
	{"RDM_OID_ATM_LINK_QOS",		37032},
	{"RDM_OID_ATM_LINK_ST",		36540},
	{"RDM_OID_AUTO_PROV",		133128},
	{"RDM_OID_BRIDGING",		68772},
	{"RDM_OID_BRIDGING_BR",		69048},
	{"RDM_OID_BRIDGING_BR_PORT",		69348},
	{"RDM_OID_BRIDGING_BR_PORT_ST",		69816},
	{"RDM_OID_BRIDGING_BR_VLAN",		70212},
	{"RDM_OID_BRIDGING_BR_VLANPORT",		70344},
	{"RDM_OID_BRIDGING_FILTER",		70500},
	{"RDM_OID_BT",		44004},
	{"RDM_OID_BT_INTF",		44088},
	{"RDM_OID_BULK_DATA",		121824},
	{"RDM_OID_BULK_DATA_PROF",		122076},
	{"RDM_OID_BULK_DATA_PROFILE_H_T_T_P",		122760},
	{"RDM_OID_BULK_DATA_PROF_PARAM",		123168},
	{"RDM_OID_BULK_DATA_PROF_PARAM_PUSH",		123108},
	{"RDM_OID_CALL_POLICY_BOOK",		12588},
	{"RDM_OID_CALL_POLICY_BOOK_LIST",		12672},
	{"RDM_OID_CANCEL",		125268},
	{"RDM_OID_CAPABILITY",		146124},
	{"RDM_OID_CAPT_PORTAL",		88668},
	{"RDM_OID_CELL",		44244},
	{"RDM_OID_CELL_ACCESS_POINT",		50016},
	{"RDM_OID_CELL_INTF",		44520},
	{"RDM_OID_CELL_INTF_ST",		46320},
	{"RDM_OID_CELL_INTF_USIM",		45876},
	{"RDM_OID_CELL_INTF_ZY_BUDGET_CONTROL",		46716},
	{"RDM_OID_CELL_INTF_ZY_IP_PASS_THRU",		47376},
	{"RDM_OID_CELL_INTF_ZY_NBR_CELL",		48864},
	{"RDM_OID_CELL_INTF_ZY_NR_NSA",		49116},
	{"RDM_OID_CELL_INTF_ZY_PCI_LOCK",		48156},
	{"RDM_OID_CELL_INTF_ZY_PCI_LOCKED_LIST",		48264},
	{"RDM_OID_CELL_INTF_ZY_PCI_LOCK_NR",		48684},
	{"RDM_OID_CELL_INTF_ZY_QSCAN",		48348},
	{"RDM_OID_CELL_INTF_ZY_QSCAN_LIST",		48456},
	{"RDM_OID_CELL_INTF_ZY_RESET_CM",		50808},
	{"RDM_OID_CELL_INTF_ZY_SCC",		47784},
	{"RDM_OID_CELL_INTF_ZY_SMS",		49416},
	{"RDM_OID_CELL_INTF_ZY_SMS_INBOX",		49548},
	{"RDM_OID_CELL_INTF_ZY_SMS_OUTBOX",		49728},
	{"RDM_OID_CELL_INTF_ZY_SMS_STORAGE",		49908},
	{"RDM_OID_CELL_INTF_ZY_UI_WAIT_ST",		47604},
	{"RDM_OID_CELL_ZY_GNSS_LOC",		50460},
	{"RDM_OID_CELL_ZY_NBR",		50868},
	{"RDM_OID_CHANGE_ICON_NAME",		140988},
	{"RDM_OID_CNGSTCFG",		147912},
	{"RDM_OID_COMMON",		146628},
	{"RDM_OID_CONFIG_BACKUP",		15672},
	{"RDM_OID_CONFIG_RESTORE",		15780},
	{"RDM_OID_CONGEST_THRED_VALUE",		148140},
	{"RDM_OID_C_S_V_ENCODING",		122520},
	{"RDM_OID_DELETE",		125460},
	{"RDM_OID_DEVICE",		0 },
	{"RDM_OID_DEV_INFO",		14496},
	{"RDM_OID_DEV_INFO_FIRMWARE_IMAGE",		15132},
	{"RDM_OID_DEV_INFO_LOC",		17940},
	{"RDM_OID_DEV_INFO_LOCAL_AGENT_TRANSFER_COMPLETE",		123744},
	{"RDM_OID_DEV_INFO_MEM_STAT",		16116},
	{"RDM_OID_DEV_INFO_POWER_STAT",		18096},
	{"RDM_OID_DEV_INFO_PROCESSOR",		17484},
	{"RDM_OID_DEV_INFO_PROXIER_INFO",		17808},
	{"RDM_OID_DEV_INFO_PS_STAT",		16248},
	{"RDM_OID_DEV_INFO_PS_STAT_CPU",		16512},
	{"RDM_OID_DEV_INFO_PS_STAT_LOAD",		16332},
	{"RDM_OID_DEV_INFO_PS_STAT_PS",		16716},
	{"RDM_OID_DEV_INFO_SUP_DATA_MODEL",		15984},
	{"RDM_OID_DEV_INFO_VENDOR_LOG_FILE",		17568},
	{"RDM_OID_DEV_INFO_VEND_CONF_FILE",		15516},
	{"RDM_OID_DEV_LOCAL_AGENT_CERTIFICATE",		125304},
	{"RDM_OID_DEV_LOCAL_AGENT_CERT_ROLE_MAPPING_LIST",		125496},
	{"RDM_OID_DEV_LOCAL_AGENT_CONTROLLER",		126888},
	{"RDM_OID_DEV_LOCAL_AGENT_CONTROLLER_TRUST",		125604},
	{"RDM_OID_DEV_LOCAL_AGENT_M_T_P",		124128},
	{"RDM_OID_DEV_LOCAL_AGENT_M_T_P_CO_A_P",		124284},
	{"RDM_OID_DEV_LOCAL_AGENT_M_T_P_M_Q_T_T",		124680},
	{"RDM_OID_DEV_LOCAL_AGENT_M_T_P_S_T_O_M_P",		124440},
	{"RDM_OID_DEV_LOCAL_AGENT_M_T_P_WEB_SOCKET",		124548},
	{"RDM_OID_DEV_LOCAL_AGENT_REQUEST",		125112},
	{"RDM_OID_DEV_LOCAL_AGENT_SUBSCRIPTION",		124812},
	{"RDM_OID_DEV_LOCAL_AGENT_SUB_TOPIC_LIST",		124020},
	{"RDM_OID_DEV_SEC",		117720},
	{"RDM_OID_DEV_SEC_CERT",		117780},
	{"RDM_OID_DHCPV4",		102084},
	{"RDM_OID_DHCPV4_CLIENT",		102144},
	{"RDM_OID_DHCPV4_RELAY",		104736},
	{"RDM_OID_DHCPV4_RELAY_FWD",		104844},
	{"RDM_OID_DHCPV4_REQ_OPT",		102744},
	{"RDM_OID_DHCPV4_SENT_OPT",		102612},
	{"RDM_OID_DHCPV4_SRV",		102900},
	{"RDM_OID_DHCPV4_SRV_CUS_DHCP_OPT",		104628},
	{"RDM_OID_DHCPV4_SRV_POOL",		103008},
	{"RDM_OID_DHCPV4_SRV_POOL_CLIENT",		104124},
	{"RDM_OID_DHCPV4_SRV_POOL_CLIENT_OPT",		104364},
	{"RDM_OID_DHCPV4_SRV_POOL_CLIENT_V4_ADDR",		104280},
	{"RDM_OID_DHCPV4_SRV_POOL_OPT",		103992},
	{"RDM_OID_DHCPV4_SRV_POOL_STATIC_ADDR",		103860},
	{"RDM_OID_DHCPV4_SRV_SUBNET",		104448},
	{"RDM_OID_DHCPV6",		105288},
	{"RDM_OID_DHCPV6_CLIENT",		105348},
	{"RDM_OID_DHCPV6_CLIENT_RECEIVED_OPT",		106032},
	{"RDM_OID_DHCPV6_CLIENT_SENT_OPT",		105900},
	{"RDM_OID_DHCPV6_CLIENT_SRV",		105792},
	{"RDM_OID_DHCPV6_RELAY",		107712},
	{"RDM_OID_DHCPV6_RELAY_INTF_SET",		107844},
	{"RDM_OID_DHCPV6_SRV",		106140},
	{"RDM_OID_DHCPV6_SRV_POOL",		106224},
	{"RDM_OID_DHCPV6_SRV_POOL_CLIENT",		107076},
	{"RDM_OID_DHCPV6_SRV_POOL_CLIENT_OPT",		107472},
	{"RDM_OID_DHCPV6_SRV_POOL_CLIENT_V6_ADDR",		107256},
	{"RDM_OID_DHCPV6_SRV_POOL_CLIENT_V6_PREFIX",		107364},
	{"RDM_OID_DHCPV6_SRV_POOL_OPT",		107556},
	{"RDM_OID_DLNA",		111132},
	{"RDM_OID_DLNA_CAPB",		111312},
	{"RDM_OID_DNS",		99012},
	{"RDM_OID_DNS_CLIENT",		99168},
	{"RDM_OID_DNS_CLIENT_SRV",		99276},
	{"RDM_OID_DNS_DIAG",		99840},
	{"RDM_OID_DNS_DIAG_NS_LOOKUP_DIAG",		99876},
	{"RDM_OID_DNS_DIAG_NS_LOOKUP_DIAG_RESULT",		100128},
	{"RDM_OID_DNS_ENTRY",		132624},
	{"RDM_OID_DNS_RELAY",		99552},
	{"RDM_OID_DNS_RELAY_FWD",		99660},
	{"RDM_OID_DNS_RT_ENTRY",		132396},
	{"RDM_OID_DOWNLOAD",		15312},
	{"RDM_OID_DSL",		23256},
	{"RDM_OID_DSL_BOND_GP",		31680},
	{"RDM_OID_DSL_BOND_GP_BONDED_CHANNEL",		32196},
	{"RDM_OID_DSL_BOND_GP_BONDED_CHANNEL_ETH",		32280},
	{"RDM_OID_DSL_BOND_GP_BONDED_CHANNEL_ETH_ST",		32316},
	{"RDM_OID_DSL_BOND_GP_ETH",		33912},
	{"RDM_OID_DSL_BOND_GP_ETH_ST",		33948},
	{"RDM_OID_DSL_BOND_GP_ST",		32544},
	{"RDM_OID_DSL_BOND_GP_ST_CURRENT_DAY",		33312},
	{"RDM_OID_DSL_BOND_GP_ST_QTR_HR",		33612},
	{"RDM_OID_DSL_BOND_GP_ST_TOTAL",		33012},
	{"RDM_OID_DSL_CHANNEL",		29916},
	{"RDM_OID_DSL_CHANNEL_ST",		30432},
	{"RDM_OID_DSL_CHANNEL_ST_CUR_DAY",		31320},
	{"RDM_OID_DSL_CHANNEL_ST_LAST_SHOWTIME",		31140},
	{"RDM_OID_DSL_CHANNEL_ST_QUAR_HR",		31500},
	{"RDM_OID_DSL_CHANNEL_ST_SHOWTIME",		30960},
	{"RDM_OID_DSL_CHANNEL_ST_TOTAL",		30780},
	{"RDM_OID_DSL_DIAG",		34368},
	{"RDM_OID_DSL_DIAG_ADSL_LINE_TEST",		34404},
	{"RDM_OID_DSL_LINE",		23364},
	{"RDM_OID_DSL_LINE_ST",		27360},
	{"RDM_OID_DSL_LINE_ST_LAST_CUR_DAY",		28776},
	{"RDM_OID_DSL_LINE_ST_LAST_QUAR_HR",		29268},
	{"RDM_OID_DSL_LINE_ST_LAST_SHOWTIME",		28692},
	{"RDM_OID_DSL_LINE_ST_SHOWTIME",		28200},
	{"RDM_OID_DSL_LINE_ST_TOTAL",		27708},
	{"RDM_OID_DSL_LINE_TEST_PARAMS",		29352},
	{"RDM_OID_DS_LITE",		92172},
	{"RDM_OID_DS_LITE_INTF_SET",		92256},
	{"RDM_OID_D_DNS",		132708},
	{"RDM_OID_EMAIL_NOTIFY",		130740},
	{"RDM_OID_EOGRE_CFG",		133956},
	{"RDM_OID_ETH",		38844},
	{"RDM_OID_ETH_IFACE",		39540},
	{"RDM_OID_ETH_INTF_ST",		40008},
	{"RDM_OID_ETH_LINK",		40404},
	{"RDM_OID_ETH_LINK_ST",		40728},
	{"RDM_OID_ETH_RMON_ST",		38976},
	{"RDM_OID_ETH_VLAN_TERM",		41124},
	{"RDM_OID_ETH_VLAN_TERM_ST",		41424},
	{"RDM_OID_FAP",		118248},
	{"RDM_OID_FAP_APP_PLATF",		119520},
	{"RDM_OID_FAP_APP_PLATF_CAPB",		119676},
	{"RDM_OID_FAP_APP_PLATF_CTRL",		120024},
	{"RDM_OID_FAP_APP_PLATF_CTRL_FEMTO_AWARE",		120108},
	{"RDM_OID_FAP_APP_PLATF_CTRL_MMS",		120540},
	{"RDM_OID_FAP_APP_PLATF_CTRL_SMS",		120336},
	{"RDM_OID_FAP_APP_PLATF_CTRL_TERM_LOC",		120744},
	{"RDM_OID_FAP_APP_PLATF_MONT",		120972},
	{"RDM_OID_FAP_APP_PLATF_MONT_FEMTO_AWARE",		121104},
	{"RDM_OID_FAP_APP_PLATF_MONT_MMS",		121464},
	{"RDM_OID_FAP_APP_PLATF_MONT_SMS",		121284},
	{"RDM_OID_FAP_APP_PLATF_MONT_TERM_LOC",		121644},
	{"RDM_OID_FAP_GPS",		118284},
	{"RDM_OID_FAP_GPS_AGPS_SRV_CONF",		119028},
	{"RDM_OID_FAP_GPS_CONTS_GPS_STATUS",		118656},
	{"RDM_OID_FAP_PERF_MGMT",		119256},
	{"RDM_OID_FAP_PERF_MGMT_CONF",		119316},
	{"RDM_OID_FAULT_MGMT",		116280},
	{"RDM_OID_FAULT_MGMT_CURRENT_ALARM",		116616},
	{"RDM_OID_FAULT_MGMT_EXPEDITED_EVENT",		117168},
	{"RDM_OID_FAULT_MGMT_HISTORY_EVENT",		116892},
	{"RDM_OID_FAULT_MGMT_QUED_EVENT",		117444},
	{"RDM_OID_FAULT_MGMT_SUPT_ALARM",		116460},
	{"RDM_OID_FEATURE_FLAG",		144132},
	{"RDM_OID_FIREWALL",		111624},
	{"RDM_OID_FIREWALL_CHAIN",		112200},
	{"RDM_OID_FIREWALL_CHAIN_RULE",		112356},
	{"RDM_OID_FIREWALL_LEVEL",		111948},
	{"RDM_OID_GATEWAY_INFO",		21360},
	{"RDM_OID_GEMPORT",		147276},
	{"RDM_OID_GHN",		56568},
	{"RDM_OID_GHN_INTF",		56628},
	{"RDM_OID_GHN_INTF_ASSOC_DEV",		57588},
	{"RDM_OID_GHN_INTF_ST",		57192},
	{"RDM_OID_GPON",		144780},
	{"RDM_OID_GPON_INFO",		140244},
	{"RDM_OID_GRE",		86892},
	{"RDM_OID_GRE_FILTER",		88416},
	{"RDM_OID_GRE_TUNNEL",		86976},
	{"RDM_OID_GRE_TUNNEL_IFACE",		87888},
	{"RDM_OID_GRE_TUNNEL_IFACE_STAT",		88188},
	{"RDM_OID_GRE_TUNNEL_STAT",		87612},
	{"RDM_OID_GUI_CUSTOMIZATION",		143556},
	{"RDM_OID_HOMEPLUG_INTF",		57804},
	{"RDM_OID_HOMEPLUG_INTF_ASSOC_DEV",		58740},
	{"RDM_OID_HOMEPLUG_INTF_ST",		58224},
	{"RDM_OID_HOME_PLUG",		57744},
	{"RDM_OID_HOSTS",		97584},
	{"RDM_OID_HOSTS_HOST",		97668},
	{"RDM_OID_HOSTS_HOST_V4_ADDR",		98592},
	{"RDM_OID_HOSTS_HOST_V6_ADDR",		98652},
	{"RDM_OID_HPNA",		50928},
	{"RDM_OID_HPNA_DIAG",		52728},
	{"RDM_OID_HPNA_DIAG_PERF_MONITOR_CHANNELS",		54096},
	{"RDM_OID_HPNA_DIAG_PERF_MONITOR_CHANNELS_CHANNEL",		54180},
	{"RDM_OID_HPNA_DIAG_PERF_MONITOR_NODES",		53352},
	{"RDM_OID_HPNA_DIAG_PERF_MONITOR_NODES_NODE",		53460},
	{"RDM_OID_HPNA_DIAG_PERF_MONT",		53244},
	{"RDM_OID_HPNA_DIAG_PHY_THROUGHPUT",		52764},
	{"RDM_OID_HPNA_DIAG_PHY_THROUGHPUT_RESULT",		53040},
	{"RDM_OID_HPNA_INTF",		50988},
	{"RDM_OID_HPNA_INTF_ASSOC_DEV",		52500},
	{"RDM_OID_HPNA_INTF_QOS",		52068},
	{"RDM_OID_HPNA_INTF_QOS_FLOW_SPEC",		52128},
	{"RDM_OID_HPNA_INTF_ST",		51672},
	{"RDM_OID_IEEE8021AG",		131520},
	{"RDM_OID_IEEE8021X",		108072},
	{"RDM_OID_IEEE8021X_SUPPLICANT",		108132},
	{"RDM_OID_IEEE8021X_SUPPLICANT_EAPMD5",		108900},
	{"RDM_OID_IEEE8021X_SUPPLICANT_EAPTLS",		108984},
	{"RDM_OID_IEEE8021X_SUPPLICANT_STAT",		108600},
	{"RDM_OID_IEEE8023AH",		132012},
	{"RDM_OID_IFACE_STACK",		23100},
	{"RDM_OID_INCREMENTAL_RESULT",		78624},
	{"RDM_OID_INPUT_CHECK_LIST",		143952},
	{"RDM_OID_INSTALL_D_U",		114264},
	{"RDM_OID_IP",		73164},
	{"RDM_OID_IPERF",		151872},
	{"RDM_OID_IPHOST",		146952},
	{"RDM_OID_IPSEC",		83484},
	{"RDM_OID_IPSEC_FILTER",		84180},
	{"RDM_OID_IPSEC_IKEV2SA",		85800},
	{"RDM_OID_IPSEC_IKEV2SA_CHILD_SA",		86460},
	{"RDM_OID_IPSEC_IKEV2SA_CHILD_SA_STAT",		86616},
	{"RDM_OID_IPSEC_IKEV2SA_RECEIVED_CP_ATTR",		86376},
	{"RDM_OID_IPSEC_IKEV2SA_STAT",		86148},
	{"RDM_OID_IPSEC_PROFILE",		84744},
	{"RDM_OID_IPSEC_PROFILE_SENT_CP_ATTR",		85380},
	{"RDM_OID_IPSEC_STAT",		83856},
	{"RDM_OID_IPSEC_TUNNEL",		85512},
	{"RDM_OID_IPSEC_TUNNEL_STAT",		85644},
	{"RDM_OID_IPV6RD",		91788},
	{"RDM_OID_IPV6RD_INTF",		91872},
	{"RDM_OID_IP_ACTIVE_PORT",		76380},
	{"RDM_OID_IP_DIAG",		76536},
	{"RDM_OID_IP_DIAG_DL_DIAG",		77640},
	{"RDM_OID_IP_DIAG_IPPING",		76644},
	{"RDM_OID_IP_DIAG_IP_LAYER_CAP_METRICS",		80268},
	{"RDM_OID_IP_DIAG_IP_LAYER_INCRE_RES",		82404},
	{"RDM_OID_IP_DIAG_IP_LAYER_MODAL",		82080},
	{"RDM_OID_IP_DIAG_NUT_TCP_DIAG",		82980},
	{"RDM_OID_IP_DIAG_SVR_SELECT_DIAG",		82656},
	{"RDM_OID_IP_DIAG_TRACERT_RT_HOPS",		77508},
	{"RDM_OID_IP_DIAG_TRACE_RT",		77136},
	{"RDM_OID_IP_DIAG_UDP_ECHO_CFG",		79200},
	{"RDM_OID_IP_DIAG_UDP_ECHO_DIAG",		79524},
	{"RDM_OID_IP_DIAG_UDP_ECHO_PAK_RST",		80040},
	{"RDM_OID_IP_DIAG_UL_DIAG",		78780},
	{"RDM_OID_IP_IFACE",		73416},
	{"RDM_OID_IP_IFACE_STAT",		75984},
	{"RDM_OID_IP_IFACE_V4_ADDR",		74916},
	{"RDM_OID_IP_IFACE_V6_ADDR",		75168},
	{"RDM_OID_IP_IFACE_V6_PREFIX",		75540},
	{"RDM_OID_I_P_CONN_TRACK",		83376},
	{"RDM_OID_J_S_O_N_ENCODING",		122676},
	{"RDM_OID_L2_T_PV3",		152004},
	{"RDM_OID_LANDING_PAGE",		149796},
	{"RDM_OID_LANG",		137388},
	{"RDM_OID_LAN_CONF_SEC",		97524},
	{"RDM_OID_LAN_DEV",		134136},
	{"RDM_OID_LINKCFG",		146868},
	{"RDM_OID_LOCAL_AGENT",		123252},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_ADD_MY_CERTIFICATE",		127404},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_BOOT_PARAMETER",		128328},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_E2_E_SESSION",		128436},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_E2_E_SESSION_RESET",		128688},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P",		127524},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_CO_A_P",		127632},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_M_Q_T_T",		128136},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_S_T_O_M_P",		127764},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_WEB_SOCKET",		127848},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_M_T_P_WEB_SOCKET_RESET",		128100},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_SCHEDULE_TIMER",		127284},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_SEND_ON_BOARD_REQUEST",		127488},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TIMER",		127344},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRANSFER_COMPLETE_POLICY",		128268},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_CHALLENGE",		126588},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_CHALLENGE_RESP",		125988},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_CREDENTIAL",		126432},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_REQ_CHALLENGE",		125808},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_ROLE",		126072},
	{"RDM_OID_LOCAL_AGENT_CONTROLLER_TRUST_ROLE_PERMISSION",		126204},
	{"RDM_OID_LOG_CATEGORY",		140808},
	{"RDM_OID_LOG_CLASSIFY",		140700},
	{"RDM_OID_LOG_SETTING",		140448},
	{"RDM_OID_LOG_UPLOAD",		17700},
	{"RDM_OID_LOIDAUTH",		146232},
	{"RDM_OID_MAIL_EVENT_CFG",		131004},
	{"RDM_OID_MAIL_SERVICE",		130776},
	{"RDM_OID_MGMT_SRV",		18312},
	{"RDM_OID_MGMT_SRV_AUTO_TRANS_COMPLETE_POL",		20304},
	{"RDM_OID_MGMT_SRV_DL_AVAIL",		20436},
	{"RDM_OID_MGMT_SRV_DL_AVAIL_ANNCMNT",		20472},
	{"RDM_OID_MGMT_SRV_DL_AVAIL_ANNCMNT_GROUP",		20580},
	{"RDM_OID_MGMT_SRV_DL_AVAIL_QUERY",		20712},
	{"RDM_OID_MGMT_SRV_DU_STATE_CHANGE_COMPL_POL",		20820},
	{"RDM_OID_MGMT_SRV_EMBEDED_DEV",		20952},
	{"RDM_OID_MGMT_SRV_MGAB_DEV",		20124},
	{"RDM_OID_MGMT_SRV_VIRTUAL_DEV",		21204},
	{"RDM_OID_MOCA",		54480},
	{"RDM_OID_MOCA_INTF",		54540},
	{"RDM_OID_MOCA_INTF_ASSOC_DEV",		56124},
	{"RDM_OID_MOCA_INTF_QOS",		55836},
	{"RDM_OID_MOCA_INTF_QOS_FLOW_ST",		55944},
	{"RDM_OID_MOCA_INTF_ST",		55440},
	{"RDM_OID_MQTT",		128808},
	{"RDM_OID_MQTT_CAPABILITY",		128892},
	{"RDM_OID_MQTT_CLIENT",		128976},
	{"RDM_OID_MQTT_CLIENT_PUB_CONF",		130020},
	{"RDM_OID_MQTT_CLIENT_STATS",		130152},
	{"RDM_OID_NAT",		100308},
	{"RDM_OID_NAT_ADDR_MAPPING",		101580},
	{"RDM_OID_NAT_INTF_SETTING",		100584},
	{"RDM_OID_NAT_PORT_MAPPING",		100740},
	{"RDM_OID_NAT_PORT_TRIGGERING",		101256},
	{"RDM_OID_NAT_SESSION_CTL",		101904},
	{"RDM_OID_NEIGHBOR_DISCOVERY",		90600},
	{"RDM_OID_NEIGHBOR_DISCOVERY_INTF_SET",		90684},
	{"RDM_OID_NETWORK_PROP",		17400},
	{"RDM_OID_NETWORK_SRV",		139704},
	{"RDM_OID_NETWORK_SRV_GP",		139764},
	{"RDM_OID_OI_WIZARD",		150660},
	{"RDM_OID_OLT",		145992},
	{"RDM_OID_OMCI",		147072},
	{"RDM_OID_OMCI_CFG",		147036},
	{"RDM_OID_ONE_CONNECT_INTERNET_ACCESS_MASTER_SWITCH",		148620},
	{"RDM_OID_ONE_CONNECT_INTERNET_ACCESS_RULE",		148704},
	{"RDM_OID_ONE_CONNECT_P_C_GENERAL",		148836},
	{"RDM_OID_ONE_CONNECT_P_C_RULE",		148944},
	{"RDM_OID_ONLINE_F_W_UPGRADE",		149220},
	{"RDM_OID_ONLINE_MODULE_UPGRADE",		149520},
	{"RDM_OID_ONT_GENERIC",		148452},
	{"RDM_OID_ONU",		144816},
	{"RDM_OID_OPT",		35304},
	{"RDM_OID_OPT_INTF",		35364},
	{"RDM_OID_OPT_INTF_ST",		35712},
	{"RDM_OID_PACKAGE",		144},
	{"RDM_OID_PACKAGE_INSTALL",		324},
	{"RDM_OID_PAREN_CTL",		131184},
	{"RDM_OID_PAREN_CTL_PROF",		131244},
	{"RDM_OID_PERD_STATISTICS",		113400},
	{"RDM_OID_PERD_STATISTICS_SAMPLE_SET",		113508},
	{"RDM_OID_PERD_STATISTICS_SAMPLE_SET_PARAM",		113856},
	{"RDM_OID_PER_CONNECTION_RESULT",		78396},
	{"RDM_OID_PM",		146472},
	{"RDM_OID_PORT_MIRROR",		141120},
	{"RDM_OID_POWER_MGMT",		149880},
	{"RDM_OID_POWER_SCHEDULE",		150276},
	{"RDM_OID_PPP",		71328},
	{"RDM_OID_PPP_IFACE",		71412},
	{"RDM_OID_PPP_IFACE_IPCP",		72528},
	{"RDM_OID_PPP_IFACE_PPPOA",		72360},
	{"RDM_OID_PPP_IFACE_PPPOE",		72396},
	{"RDM_OID_PPP_IFACE_STAT",		72768},
	{"RDM_OID_PPP_INTF_IPV6CP",		72684},
	{"RDM_OID_PPTP_CFG",		133656},
	{"RDM_OID_PTM",		37824},
	{"RDM_OID_PTM_LINK",		37884},
	{"RDM_OID_PTM_LINK_ST",		38448},
	{"RDM_OID_QOS",		92604},
	{"RDM_OID_QOSCFG",		147672},
	{"RDM_OID_QOS_APP",		95340},
	{"RDM_OID_QOS_CLS",		93216},
	{"RDM_OID_QOS_FLOW",		95616},
	{"RDM_OID_QOS_POLICER",		95940},
	{"RDM_OID_QOS_QUE",		96504},
	{"RDM_OID_QOS_QUE_STAT",		97044},
	{"RDM_OID_QOS_SHAPER",		97344},
	{"RDM_OID_QOS_VALUE",		147852},
	{"RDM_OID_QUEUE",		148392},
	{"RDM_OID_REMO_MGMT",		137448},
	{"RDM_OID_REMO_MGMT_IP_PASS_THRU",		138588},
	{"RDM_OID_REMO_MGMT_TRUST_DOMAIN",		138348},
	{"RDM_OID_REMO_MGMT_TRUST_DOMAIN_IP_PASS_THRU",		139464},
	{"RDM_OID_REMO_SRV",		137532},
	{"RDM_OID_REMO_SRV_IP_PASS_THRU",		138672},
	{"RDM_OID_REMO_SRV_TRUST_DOMAIN",		137832},
	{"RDM_OID_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU",		138948},
	{"RDM_OID_ROM_CONVERT",		130404},
	{"RDM_OID_ROUTING",		88800},
	{"RDM_OID_ROUTING_RIP",		89880},
	{"RDM_OID_ROUTING_RIP_INTF_SET",		89988},
	{"RDM_OID_ROUTING_ROUTER",		88860},
	{"RDM_OID_ROUTING_ROUTER_V4_FWD",		89088},
	{"RDM_OID_ROUTING_ROUTER_V6_FWD",		89604},
	{"RDM_OID_ROUTING_RT_INFO",		90240},
	{"RDM_OID_ROUTING_RT_INFO_INTF_SET",		90324},
	{"RDM_OID_RT_ADV",		90984},
	{"RDM_OID_RT_ADV_INTF_SET",		91068},
	{"RDM_OID_RT_ADV_INTF_SET_OPT",		91656},
	{"RDM_OID_SCHEDULE",		130464},
	{"RDM_OID_SELF_TEST_DIAG",		111540},
	{"RDM_OID_SERVICE",		108},
	{"RDM_OID_SMART_CARD_READERS",		109308},
	{"RDM_OID_SMART_CARD_READERS_SCR",		109368},
	{"RDM_OID_SMART_CARD_READERS_SCR_SC",		109596},
	{"RDM_OID_SOFTIMAGE",		146340},
	{"RDM_OID_SPEED_DIAL_BOOK",		12348},
	{"RDM_OID_SPEED_DIAL_BOOK_LIST",		12408},
	{"RDM_OID_SPEED_TEST",		133356},
	{"RDM_OID_SP_REMO_SRV",		137940},
	{"RDM_OID_SP_REMO_SRV_IP_PASS_THRU",		139056},
	{"RDM_OID_SP_REMO_SRV_TRUST_DOMAIN",		138216},
	{"RDM_OID_SP_REMO_SRV_TRUST_DOMAIN_IP_PASS_THRU",		139332},
	{"RDM_OID_SP_TRUST_DOMAIN",		138456},
	{"RDM_OID_SP_TRUST_DOMAIN_IP_PASS_THRU",		139572},
	{"RDM_OID_SW_MODULE",		114156},
	{"RDM_OID_SW_MODULE_DEPLOYMENT_UNIT",		115164},
	{"RDM_OID_SW_MODULE_EXEC_ENV",		114672},
	{"RDM_OID_SW_MODULE_EXEC_UNIT",		115680},
	{"RDM_OID_SYSTEM_INFO",		140016},
	{"RDM_OID_S_T_B_VENDOR_I_D",		141432},
	{"RDM_OID_S_W_MOD_D_U_STATE_CHANGE",		114420},
	{"RDM_OID_S_W_MOD_E_U_SET_REQUESTED_STATE",		116220},
	{"RDM_OID_S_W_MOD_UNINSTALL",		115644},
	{"RDM_OID_S_W_MOD_UPDATE",		115536},
	{"RDM_OID_TCONT",		147432},
	{"RDM_OID_TEMP_STAT",		16896},
	{"RDM_OID_TEMP_STAT_TEMP_SENSOR",		16956},
	{"RDM_OID_TIME",		21468},
	{"RDM_OID_TR064",		141648},
	{"RDM_OID_TRAP_ADDR",		134892},
	{"RDM_OID_TRTCM",		147516},
	{"RDM_OID_TRTCMCFG",		148200},
	{"RDM_OID_TRTCM_VALUE",		148332},
	{"RDM_OID_T_F_T_P_SRV_NAME",		141588},
	{"RDM_OID_UPA",		58944},
	{"RDM_OID_UPA_DIAG",		60504},
	{"RDM_OID_UPA_DIAG_INTF_MEASUREMENT",		60540},
	{"RDM_OID_UPA_INTF",		59004},
	{"RDM_OID_UPA_INTF_ACTIVE_NOTCH",		60240},
	{"RDM_OID_UPA_INTF_ASSOC_DEV",		59940},
	{"RDM_OID_UPA_INTF_BRIDGE_FOR",		60396},
	{"RDM_OID_UPA_INTF_ST",		59544},
	{"RDM_OID_UPNP",		109752},
	{"RDM_OID_UPNP_DEV",		109788},
	{"RDM_OID_UPNP_DEV_CAPB",		110112},
	{"RDM_OID_UPNP_DISC",		110436},
	{"RDM_OID_UPNP_DISC_DEV",		110748},
	{"RDM_OID_UPNP_DISC_ROOT_DEV",		110544},
	{"RDM_OID_UPNP_DISC_SERVICE",		110952},
	{"RDM_OID_USB",		41820},
	{"RDM_OID_USB_INTF",		41904},
	{"RDM_OID_USB_INTF_STAT",		42204},
	{"RDM_OID_USB_PORT",		42600},
	{"RDM_OID_USB_USBHOSTS",		42828},
	{"RDM_OID_USB_USBHOSTS_HOST",		42888},
	{"RDM_OID_USB_USBHOSTS_HOST_DEV",		43116},
	{"RDM_OID_USB_USBHOSTS_HOST_DEV_CFG",		43788},
	{"RDM_OID_USB_USBHOSTS_HOST_DEV_CFG_INTF",		43872},
	{"RDM_OID_USB_USBHOSTS_HOST_DEV_PAR",		43632},
	{"RDM_OID_USRS",		109068},
	{"RDM_OID_USRS_USR",		109128},
	{"RDM_OID_USR_INTF",		22128},
	{"RDM_OID_USR_INTF_LOCAL_DISP",		22872},
	{"RDM_OID_USR_INTF_REMO_ACESS",		22740},
	{"RDM_OID_VC_AUTO_HUNT",		136764},
	{"RDM_OID_VC_HUNT_INFO",		136920},
	{"RDM_OID_VC_HUNT_RULE",		137004},
	{"RDM_OID_VLAN_AUTO_HUNT",		137088},
	{"RDM_OID_VLAN_GROUP",		141252},
	{"RDM_OID_VLAN_HUNT_INFO",		137220},
	{"RDM_OID_VLAN_HUNT_RULE",		137304},
	{"RDM_OID_VOICE_CALL_STATUS",		14268},
	{"RDM_OID_VOICE_CAPB",		468},
	{"RDM_OID_VOICE_CAPB_CODEC",		1716},
	{"RDM_OID_VOICE_CAPB_H323",		1632},
	{"RDM_OID_VOICE_CAPB_MGCP",		1572},
	{"RDM_OID_VOICE_CAPB_SIP",		1272},
	{"RDM_OID_VOICE_COMMON",		11940},
	{"RDM_OID_VOICE_DECT",		13392},
	{"RDM_OID_VOICE_DECT_B_S",		13452},
	{"RDM_OID_VOICE_DECT_H_S",		13944},
	{"RDM_OID_VOICE_FXO",		13056},
	{"RDM_OID_VOICE_FXO_PORT",		13116},
	{"RDM_OID_VOICE_INTERNAL_PHONE_NUMBER_BOOK",		12852},
	{"RDM_OID_VOICE_INTERNAL_PHONE_NUMBER_BOOK_IF_LIST",		12912},
	{"RDM_OID_VOICE_INTERNAL_PHONE_NUMBER_BOOK_PORT_LIST",		12996},
	{"RDM_OID_VOICE_LINE",		6864},
	{"RDM_OID_VOICE_LINE_CALLING_FEATURE",		8100},
	{"RDM_OID_VOICE_LINE_CODEC",		9756},
	{"RDM_OID_VOICE_LINE_CODEC_LIST",		9960},
	{"RDM_OID_VOICE_LINE_PROCESSING",		9600},
	{"RDM_OID_VOICE_LINE_RINGER",		7620},
	{"RDM_OID_VOICE_LINE_RINGER_DESCRIPTION",		7812},
	{"RDM_OID_VOICE_LINE_RINGER_EVENT",		7728},
	{"RDM_OID_VOICE_LINE_RINGER_PATTERN",		7968},
	{"RDM_OID_VOICE_LINE_SESSION",		10212},
	{"RDM_OID_VOICE_LINE_SIP",		7236},
	{"RDM_OID_VOICE_LINE_SIP_EVENT_SUBS",		7512},
	{"RDM_OID_VOICE_LINE_STATS",		10368},
	{"RDM_OID_VOICE_PHONE_BOOK",		12312},
	{"RDM_OID_VOICE_PHY_INTF",		11148},
	{"RDM_OID_VOICE_PROF",		1896},
	{"RDM_OID_VOICE_PROF_FAX_T38",		6684},
	{"RDM_OID_VOICE_PROF_NUM_PLAN",		5436},
	{"RDM_OID_VOICE_PROF_NUM_PLAN_PREFIX_INFO",		5688},
	{"RDM_OID_VOICE_PROF_RTP",		4788},
	{"RDM_OID_VOICE_PROF_RTP_REDUNDANCY",		5208},
	{"RDM_OID_VOICE_PROF_RTP_RTCP",		4968},
	{"RDM_OID_VOICE_PROF_RTP_SRTP",		5076},
	{"RDM_OID_VOICE_PROF_SIP",		3000},
	{"RDM_OID_VOICE_PROF_SIP_RESP_MAP_OBJ",		4680},
	{"RDM_OID_VOICE_PROF_SIP_SUBSCRIBE_OBJ",		4524},
	{"RDM_OID_VOICE_PROF_SRV_PRO_INFO",		2868},
	{"RDM_OID_VOICE_PROF_TONE",		5916},
	{"RDM_OID_VOICE_PROF_TONE_DESCRIPTION",		6108},
	{"RDM_OID_VOICE_PROF_TONE_EVENT",		6024},
	{"RDM_OID_VOICE_PROF_TONE_PATTERN",		6312},
	{"RDM_OID_VOICE_PSTN",		11760},
	{"RDM_OID_VOICE_SRV",		384},
	{"RDM_OID_WIFI",		60720},
	{"RDM_OID_WIFI_ACCESS_POINT",		63984},
	{"RDM_OID_WIFI_ACCESS_POINT_ACCOUNTING",		65376},
	{"RDM_OID_WIFI_ACCESS_POINT_ASSOC_DEV",		66048},
	{"RDM_OID_WIFI_ACCESS_POINT_ASSOC_DEV_ST",		66324},
	{"RDM_OID_WIFI_ACCESS_POINT_SEC",		64356},
	{"RDM_OID_WIFI_ACCESS_POINT_WPS",		65604},
	{"RDM_OID_WIFI_AP_STEERING",		68004},
	{"RDM_OID_WIFI_DIAGNOSTIC",		67452},
	{"RDM_OID_WIFI_DIAGNOSTIC_RESULTS",		67560},
	{"RDM_OID_WIFI_END_POINT",		66660},
	{"RDM_OID_WIFI_END_POINT_PROFILE",		67032},
	{"RDM_OID_WIFI_END_POINT_PROFILE_SEC",		67212},
	{"RDM_OID_WIFI_END_POINT_SEC",		66972},
	{"RDM_OID_WIFI_END_POINT_ST",		66840},
	{"RDM_OID_WIFI_ONE_SSID",		60900},
	{"RDM_OID_WIFI_QSTEER",		133464},
	{"RDM_OID_WIFI_RADIO",		60960},
	{"RDM_OID_WIFI_RADIO_ST",		62676},
	{"RDM_OID_WIFI_SSID",		62928},
	{"RDM_OID_WIFI_SSID_STAT",		63588},
	{"RDM_OID_WIFI_STA_FILTER",		66576},
	{"RDM_OID_WLAN_SCHEDULER",		68448},
	{"RDM_OID_WLAN_SCHEDULER_ACCESS",		136656},
	{"RDM_OID_WPS",		67344},
	{"RDM_OID_XMPP",		151140},
	{"RDM_OID_XMPP_CONN",		151224},
	{"RDM_OID_XMPP_CONN_SRV",		151692},
	{"RDM_OID_XPON",		146712},
	{"RDM_OID_ZY_DATA_USAGE",		150864},
	{"RDM_OID_ZY_DATA_USAGE_LAN",		150924},
	{"RDM_OID_ZY_DATA_USAGE_WAN",		151032},
	{"RDM_OID_ZY_EXT",		132264},
	{"RDM_OID_ZY_HOSTS_HOST_EXT",		98712},
	{"RDM_OID_ZY_HOST_NAME_REPLACE",		133596},
	{"RDM_OID_ZY_IGMP",		134244},
	{"RDM_OID_ZY_IPP_CFG",		144648},
	{"RDM_OID_ZY_LOG_CFG",		135420},
	{"RDM_OID_ZY_LOG_CFG_GP",		135528},
	{"RDM_OID_ZY_LOG_CFG_GP_ACCOUNT",		135684},
	{"RDM_OID_ZY_LOOP_GUARD",		150432},
	{"RDM_OID_ZY_MAC_FILTER",		136440},
	{"RDM_OID_ZY_MAC_FILTER_WHITE_LIST",		136548},
	{"RDM_OID_ZY_MLD",		135000},
	{"RDM_OID_ZY_ONE_CONNECT",		148512},
	{"RDM_OID_ZY_OPTION125",		144456},
	{"RDM_OID_ZY_PROXIMITY_SENSOR",		150540},
	{"RDM_OID_ZY_RESET_ADMIN",		150600},
	{"RDM_OID_ZY_SAMBA",		143220},
	{"RDM_OID_ZY_SAMBA_DIR",		143304},
	{"RDM_OID_ZY_SNMP",		134736},
	{"RDM_OID_ZY_VENDOR_SPECIFIC",		144516},
	{"RDM_OID_ZY_W_W_A_N_BACKUP",		141708},
	{"RDM_OID_ZY_W_W_A_N_BUDGET_CONTROL",		142308},
	{"RDM_OID_ZY_W_W_A_N_PING_CHECK",		142152},
	{"RDM_OID_ZY_W_W_A_N_STAT",		142992},
	{ NULL, NULL }
};

/*oid function*/
int32_t zHttpOidGet(char *objNameAbbr);

