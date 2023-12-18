#ifndef __SKB_DEFINEDS_H__
#define __SKB_DEFINEDS_H__

/* queue = mark[4:0] */
#define SKBMARK_Q_S             0
#define SKBMARK_Q_M             (0x1F << SKBMARK_Q_S)
#define SKBMARK_GET_Q(MARK)     ((MARK & SKBMARK_Q_M) >> SKBMARK_Q_S)
#define SKBMARK_SET_Q(MARK, Q)  ((MARK & ~SKBMARK_Q_M) | (Q << SKBMARK_Q_S))

/* guest_wlan_id = mark[9:9] */
#define SKBMARK_GWL_ID_S         9
#define SKBMARK_GWL_ID_M         (0x1 << SKBMARK_GWL_ID_S)
#define SKBMARK_GET_GWL_ID(MARK) \
	((MARK & SKBMARK_GWL_ID_M) >> SKBMARK_GWL_ID_S)
#define SKBMARK_SET_GWL_ID(MARK, TC) \
	((MARK & ~SKBMARK_GWL_ID_M) | (TC << SKBMARK_GWL_ID_S))

/* parental_control_id = mark[10:10] */
/* reuse for Voice port reserve */
#define SKBMARK_PC_ID_S         10
#define SKBMARK_PC_ID_M         (0x1 << SKBMARK_PC_ID_S)
#define SKBMARK_GET_PC_ID(MARK) ((MARK & SKBMARK_PC_ID_M) >> SKBMARK_PC_ID_S)
#define SKBMARK_SET_PC_ID(MARK, TC) \
    ((MARK & ~SKBMARK_PC_ID_M) | (TC << SKBMARK_PC_ID_S))

/* flow_id = mark[18:11]; for QoS Classifier */
#define SKBMARK_FLOW_ID_S       11
#define SKBMARK_FLOW_ID_M       (0xFF << SKBMARK_FLOW_ID_S)
#define SKBMARK_GET_FLOW_ID(MARK) \
    ((MARK & SKBMARK_FLOW_ID_M) >> SKBMARK_FLOW_ID_S)
#define SKBMARK_SET_FLOW_ID(MARK, FLOW) \
    ((MARK & ~SKBMARK_FLOW_ID_M) | (FLOW << SKBMARK_FLOW_ID_S))

/* VID_ACTION = mark[21:20], Change the bits used for Qos vlan operation. Original bits and WMM overlap */
#define SKBMARK_VID_ACTION_S       20
#define SKBMARK_VID_ACTION_M       (0x03 << SKBMARK_VID_ACTION_S)
#define SKBMARK_GET_VID_ACTION(MARK) \
	((MARK & SKBMARK_VID_ACTION_M) >> SKBMARK_VID_ACTION_S)
#define SKBMARK_SET_VID_ACTION(MARK, FLOW) \
	((MARK & ~SKBMARK_VID_ACTION_M) | (FLOW << SKBMARK_VID_ACTION_S))

/* POLICER = mark[23:22], Change the bits used for Qos policer */
#define SKBMARK_POLICER_S       22
#define SKBMARK_POLICER_M       (0x03 << SKBMARK_POLICER_S)
#define SKBMARK_GET_POLICER(MARK) \
    ((MARK & SKBMARK_POLICER_M) >> SKBMARK_POLICER_S)
#define SKBMARK_SET_POLICER(MARK, FLOW) \
    ((MARK & ~SKBMARK_POLICER_M) | (FLOW << SKBMARK_POLICER_S))

/* service route = mark[27:24]; for service route*/
#define SKBMARK_SERVICEROUTE_S          24
#define SKBMARK_SERVICEROUTE_M          (0x0f << SKBMARK_SERVICEROUTE_S)
#define SKBMARK_GET_SERVICEROUTE(MARK) \
	((MARK & SKBMARK_SERVICEROUTE_M) >> SKBMARK_SERVICEROUTE_S)
#define SKBMARK_SET_SERVICEROUTE(MARK, SERVICEROUTE) \
	((MARK & ~SKBMARK_SERVICEROUTE_M) | (SERVICEROUTE << SKBMARK_SERVICEROUTE_S))

/* iffwan_mark = mark[28]; check packet is form WAN interface */
#define SKBMARK_IFFWAN_MARK_S    28
#define SKBMARK_IFFWAN_MARK_M    (0x01 << SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_GET_IFFWAN_MARK(MARK) \
    ((MARK & SKBMARK_IFFWAN_MARK_M) >> SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_SET_IFFWAN_MARK(MARK, IFFWAN_MARK) \
    ((MARK & ~SKBMARK_IFFWAN_MARK_M) | (IFFWAN_MARK << SKBMARK_IFFWAN_MARK_S))

#endif