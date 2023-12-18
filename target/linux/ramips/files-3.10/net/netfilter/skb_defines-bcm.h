/***********************************************************************
 *
 *  Copyright (c) 2006-2007  Broadcom Corporation
 *  All Rights Reserved
 *
 * <:label-BRCM:2011:DUAL/GPL:standard
 * 
 * Unless you and Broadcom execute a separate written software license 
 * agreement governing use of this software, this software is licensed 
 * to you under the terms of the GNU General Public License version 2 
 * (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php, 
 * with the following added to such license:
 * 
 *    As a special exception, the copyright holders of this software give 
 *    you permission to link this software with independent modules, and 
 *    to copy and distribute the resulting executable under terms of your 
 *    choice, provided that you also meet, for each linked independent 
 *    module, the terms and conditions of the license of that module. 
 *    An independent module is a module which is not derived from this
 *    software.  The special exception does not apply to any modifications 
 *    of the software.  
 * 
 * Not withstanding the above, under no circumstances may you combine 
 * this software in any way with any other Broadcom software provided 
 * under a license other than the GPL, without Broadcom's express prior 
 * written consent. 
 * 
 *:>
 *
 ************************************************************************/


#ifndef __SKB_DEFINES_H__
#define __SKB_DEFINES_H__


/*!\file skb_defines.h
 * \brief Header file contains constants and macros used to set or
 * get various bit fields defined for skb->mark.
 *
 */

/* queue = mark[4:0] */
#define SKBMARK_Q_S             0
#define SKBMARK_Q_M             (0x1F << SKBMARK_Q_S)
#define SKBMARK_GET_Q(MARK)     ((MARK & SKBMARK_Q_M) >> SKBMARK_Q_S)
#define SKBMARK_SET_Q(MARK, Q)  ((MARK & ~SKBMARK_Q_M) | (Q << SKBMARK_Q_S))
#if defined(ZCFG_USED) //__ZYXEL__, Mark, Use traffic_class_id [10:9] for guest wlan ap feature
/* traffic_class_id = mark[8:5] */
#define SKBMARK_TC_ID_S         5
#define SKBMARK_TC_ID_M         (0xF << SKBMARK_TC_ID_S)
#define SKBMARK_GET_TC_ID(MARK) ((MARK & SKBMARK_TC_ID_M) >> SKBMARK_TC_ID_S)
#define SKBMARK_SET_TC_ID(MARK, TC) \
    ((MARK & ~SKBMARK_TC_ID_M) | (TC << SKBMARK_TC_ID_S))
/* guest_wlan_id = mark[9:9] */
#define SKBMARK_GWL_ID_S         9
#define SKBMARK_GWL_ID_M         (0x1 << SKBMARK_GWL_ID_S)
#define SKBMARK_GET_GWL_ID(MARK) ((MARK & SKBMARK_GWL_ID_M) >> SKBMARK_GWL_ID_S)
#define SKBMARK_SET_GWL_ID(MARK, TC) \
    ((MARK & ~SKBMARK_GWL_ID_M) | (TC << SKBMARK_GWL_ID_S))
/* parental_control_id = mark[10:10] */
#define SKBMARK_PC_ID_S         10
#define SKBMARK_PC_ID_M         (0x1 << SKBMARK_PC_ID_S)
#define SKBMARK_GET_PC_ID(MARK) ((MARK & SKBMARK_PC_ID_M) >> SKBMARK_PC_ID_S)
#define SKBMARK_SET_PC_ID(MARK, TC) \
    ((MARK & ~SKBMARK_PC_ID_M) | (TC << SKBMARK_PC_ID_S))
#else
/* traffic_class_id = mark[10:5] */
#define SKBMARK_TC_ID_S         5
#define SKBMARK_TC_ID_M         (0x3F << SKBMARK_TC_ID_S)
#define SKBMARK_GET_TC_ID(MARK) ((MARK & SKBMARK_TC_ID_M) >> SKBMARK_TC_ID_S)
#define SKBMARK_SET_TC_ID(MARK, TC) \
    ((MARK & ~SKBMARK_TC_ID_M) | (TC << SKBMARK_TC_ID_S))
#endif
/* flow_id = mark[18:11] */
#define SKBMARK_FLOW_ID_S       11
#define SKBMARK_FLOW_ID_M       (0xFF << SKBMARK_FLOW_ID_S)
#define SKBMARK_GET_FLOW_ID(MARK) \
    ((MARK & SKBMARK_FLOW_ID_M) >> SKBMARK_FLOW_ID_S)
#define SKBMARK_SET_FLOW_ID(MARK, FLOW) \
    ((MARK & ~SKBMARK_FLOW_ID_M) | (FLOW << SKBMARK_FLOW_ID_S))
/* iq_prio = mark[19]; for Ingress QoS used when TX is WLAN */
#define SKBMARK_IQPRIO_MARK_S    19
#define SKBMARK_IQPRIO_MARK_M    (0x01 << SKBMARK_IQPRIO_MARK_S)
#define SKBMARK_GET_IQPRIO_MARK(MARK) \
    ((MARK & SKBMARK_IQPRIO_MARK_M) >> SKBMARK_IQPRIO_MARK_S)
#define SKBMARK_SET_IQPRIO_MARK(MARK, IQPRIO_MARK) \
    ((MARK & ~SKBMARK_IQPRIO_MARK_M) | (IQPRIO_MARK << SKBMARK_IQPRIO_MARK_S))
#if defined(ZCFG_USED) /* use gpon mark values for other feature, ZyXEL, John (porting from MSTC) */
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

#if 0
/*custom feature = mark[26:26]; for custom feature*/
#define SKBMARK_CUSTOM_S          26
#define SKBMARK_CUSTOM_M          (0x01 << SKBMARK_CUSTOM_S)
#define SKBMARK_GET_CUSTOM(MARK) \
	((MARK & SKBMARK_CUSTOM_S) >> SKBMARK_CUSTOM_S)
#define SKBMARK_SET_CUSTOM(MARK, CUSTOM) \
	((MARK & ~SKBMARK_CUSTOM_S) | (CUSTOM << SKBMARK_CUSTOM_S))
#define SKBMARK_CUSTOM_WANDEFTAG  SKBMARK_SET_CUSTOM(0, 1) //QoS default WAN tagging
#endif

/* iffwan_mark = mark[28] --  BRCM defined-- */
#define SKBMARK_IFFWAN_MARK_S    28
#define SKBMARK_IFFWAN_MARK_M    (0x01 << SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_GET_IFFWAN_MARK(MARK) \
    ((MARK & SKBMARK_IFFWAN_MARK_M) >> SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_SET_IFFWAN_MARK(MARK, IFFWAN_MARK) \
    ((MARK & ~SKBMARK_IFFWAN_MARK_M) | (IFFWAN_MARK << SKBMARK_IFFWAN_MARK_S))
/* ipsec_mark = mark[29] */
#define SKBMARK_IPSEC_MARK_S    29
#define SKBMARK_IPSEC_MARK_M    (0x01 << SKBMARK_IPSEC_MARK_S)
#define SKBMARK_GET_IPSEC_MARK(MARK) \
    ((MARK & SKBMARK_IPSEC_MARK_M) >> SKBMARK_IPSEC_MARK_S)
#define SKBMARK_SET_IPSEC_MARK(MARK, IPSEC_MARK) \
    ((MARK & ~SKBMARK_IPSEC_MARK_M) | (IPSEC_MARK << SKBMARK_IPSEC_MARK_S))
/* policy_routing = mark[31:30] */
/*#define SKBMARK_POLICY_RTNG_S   30
#define SKBMARK_POLICY_RTNG_M   (0x03 << SKBMARK_POLICY_RTNG_S)
#define SKBMARK_GET_POLICY_RTNG(MARK)  \
    ((MARK & SKBMARK_POLICY_RTNG_M) >> SKBMARK_POLICY_RTNG_S)
#define SKBMARK_SET_POLICY_RTNG(MARK, POLICY) \
    ((MARK & ~SKBMARK_POLICY_RTNG_M) | (POLICY << SKBMARK_POLICY_RTNG_S))*/

/* preset_SSID_mark = mark[30] --ChihYuan Yang*/
#define SKBMARK_PRESET_MARK_S   30
#define SKBMARK_PRESET_MARK_M   (0x1 << SKBMARK_PRESET_MARK_S)
#define SKBMARK_GET_PRESET(MARK)  \
    ((MARK & SKBMARK_PRESET_MARK_M) >> SKBMARK_PRESET_MARK_S)
#define SKBMARK_SET_PRESET(MARK, POLICY) \
    ((MARK & ~SKBMARK_PRESET_MARK_M) | (POLICY << SKBMARK_PRESET_MARK_S))

/* br0_mark = mark[31] --ChihYuan Yang*/
#define SKBMARK_BR0_MARK_S   31
#define SKBMARK_BR0_MARK_M   (0x1 << SKBMARK_BR0_MARK_S)
#define SKBMARK_GET_BR0(MARK)  \
    ((MARK & SKBMARK_BR0_MARK_M) >> SKBMARK_BR0_MARK_S)
#define SKBMARK_SET_BR0(MARK, POLICY) \
    ((MARK & ~SKBMARK_BR0_MARK_M) | (POLICY << SKBMARK_BR0_MARK_S))

//keep the original setting for gpon preventing undefine reference
/* port = mark[26:20]; for enet driver of gpon port, this is gem_id */
#define SKBMARK_PORT_S          20
#define SKBMARK_PORT_M          (0x7F << SKBMARK_PORT_S)
#define SKBMARK_GET_PORT(MARK) \
	((MARK & SKBMARK_PORT_M) >> SKBMARK_PORT_S)
#define SKBMARK_SET_PORT(MARK, PORT) \
	((MARK & ~SKBMARK_PORT_M) | (PORT << SKBMARK_PORT_S))

#else
/* port = mark[26:20]; for enet driver of gpon port, this is gem_id */
#define SKBMARK_PORT_S          20
#define SKBMARK_PORT_M          (0x7F << SKBMARK_PORT_S)
#define SKBMARK_GET_PORT(MARK) \
    ((MARK & SKBMARK_PORT_M) >> SKBMARK_PORT_S)
#define SKBMARK_SET_PORT(MARK, PORT) \
    ((MARK & ~SKBMARK_PORT_M) | (PORT << SKBMARK_PORT_S))
/* iffwan_mark = mark[27] --  BRCM defined-- */
#define SKBMARK_IFFWAN_MARK_S    27
#define SKBMARK_IFFWAN_MARK_M    (0x01 << SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_GET_IFFWAN_MARK(MARK) \
    ((MARK & SKBMARK_IFFWAN_MARK_M) >> SKBMARK_IFFWAN_MARK_S)
#define SKBMARK_SET_IFFWAN_MARK(MARK, IFFWAN_MARK) \
    ((MARK & ~SKBMARK_IFFWAN_MARK_M) | (IFFWAN_MARK << SKBMARK_IFFWAN_MARK_S))
/* ipsec_mark = mark[28] */
#define SKBMARK_IPSEC_MARK_S    28
#define SKBMARK_IPSEC_MARK_M    (0x01 << SKBMARK_IPSEC_MARK_S)
#define SKBMARK_GET_IPSEC_MARK(MARK) \
    ((MARK & SKBMARK_IPSEC_MARK_M) >> SKBMARK_IPSEC_MARK_S)
#define SKBMARK_SET_IPSEC_MARK(MARK, IPSEC_MARK) \
    ((MARK & ~SKBMARK_IPSEC_MARK_M) | (IPSEC_MARK << SKBMARK_IPSEC_MARK_S))
/* policy_routing = mark[31:29] */
#define SKBMARK_POLICY_RTNG_S   29
#define SKBMARK_POLICY_RTNG_M   (0x07 << SKBMARK_POLICY_RTNG_S)
#define SKBMARK_GET_POLICY_RTNG(MARK)  \
    ((MARK & SKBMARK_POLICY_RTNG_M) >> SKBMARK_POLICY_RTNG_S)
#define SKBMARK_SET_POLICY_RTNG(MARK, POLICY) \
    ((MARK & ~SKBMARK_POLICY_RTNG_M) | (POLICY << SKBMARK_POLICY_RTNG_S))
#endif

#endif /* __SKB_DEFINES_H__ */
