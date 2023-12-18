/*
  Module Name:
  ra_nat.c

  Abstract:

  Revision History:
  Who         When            What
  --------    ----------      ----------------------------------------------
  Name        Date            Modification logs
  Steven Liu  2011-11-11      Support MT7620 Cache mechanism
  Steven Liu  2011-06-01      Support MT7620
  Steven Liu  2011-04-11      Support RT6855A
  Steven Liu  2011-02-08      Support IPv6 over PPPoE
  Steven Liu  2010-11-25      Fix double VLAN + PPPoE header bug
  Steven Liu  2010-11-24      Support upstream/downstream/bi-direction acceleration
  Steven Liu  2010-11-17      Support Linux 2.6.36 kernel
  Steven Liu  2010-07-13      Support DSCP to User Priority helper
  Steven Liu  2010-06-03      Support skb headroom/tailroom/cb to keep HNAT information
  Kurtis Ke   2010-03-30      Support HNAT parameter can be changed by application
  Steven Liu  2010-04-08      Support RT3883 + RT309x concurrent AP
  Steven Liu  2010-03-01      Support RT3352
  Steven Liu  2009-11-26      Support WiFi pseudo interface by using VLAN tag
  Steven Liu  2009-07-21      Support IPV6 Forwarding
  Steven Liu  2009-04-02      Support RT3883/RT3350
  Steven Liu  2008-03-19      Support RT3052
  Steven Liu  2007-09-25      Support RT2880 MP
  Steven Liu  2006-10-06      Initial version
 *
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/if_vlan.h>
#include <net/ipv6.h>
#include <net/ip.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/pci.h>
#include <linux/errno.h>
#include <linux/inetdevice.h>
#include <net/rtnetlink.h>
#include <net/netevent.h>

#include "ra_nat.h"
#include "foe_fdb.h"
#include "frame_engine.h"
#include "sys_rfrw.h"
#include "policy.h"
#include "util.h"
#include "hwnat_ioctl.h"

#if !defined (CONFIG_HNAT_V2)
#include "acl_ioctl.h"
#include "ac_ioctl.h"
#include "acl_policy.h"
#include "mtr_policy.h"
#include "ac_policy.h"
#endif
struct timer_list hwnat_clear_entry_timer;
static void hwnat_clear_entry(unsigned long data)
{
	printk("HW_NAT work normally\n");
	RegModifyBits(PPE_FOE_CFG, FWD_CPU_BUILD_ENTRY, 4, 2);
	//del_timer_sync(&hwnat_clear_entry_timer);

}
void foe_clear_entry(struct neighbour *neigh);
#if defined (CONFIG_RAETH_QDMA) && defined  (CONFIG_PPE_MCAST)
#include "mcast_tbl.h"
#endif

#include "../../../drivers/net/raeth/ra_ioctl.h"
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
#include "fast_path.h"
#endif
extern int (*ra_sw_nat_hook_rx) (struct sk_buff * skb);
extern int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, int gmac_no);
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
extern void (*ppe_dev_register_hook) (struct net_device *dev);
extern void (*ppe_dev_unregister_hook) (struct net_device *dev);
#endif
extern uint8_t		bind_dir;
extern uint16_t		wan_vid;
extern uint16_t		lan_vid;
extern uint32_t		DebugLevel;
#if defined (CONFIG_RAETH_QDMA)
extern unsigned int     M2Q_table[64];
extern unsigned int lan_wan_separate;
#endif
#if defined (CONFIG_HW_SFQ)
extern unsigned int web_sfq_enable;
#define HwSfqQUp 3
#define HwSfqQDl 1
#endif
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
uint32_t pptp_fast_path = 0;
EXPORT_SYMBOL(pptp_fast_path);
uint32_t l2tp_fast_path = 0;
EXPORT_SYMBOL(l2tp_fast_path);
int fast_bind = 0;
uint8_t hash_cnt = 0;
#endif
struct FoeEntry		*PpeFoeBase;
EXPORT_SYMBOL(PpeFoeBase);
dma_addr_t		PpePhyFoeBase;
#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
struct PsEntry    *PpePsBase;
dma_addr_t	  PpePhyPsBase;
#endif
struct net_device	*DstPort[MAX_IF_NUM];
PktParseResult		PpeParseResult;
#if defined (CONFIG_SUPPORT_WLAN_OPTIMIZE)
PktRxParseResult	PpeRxParseResult;
#endif
#ifdef CONFIG_RA_HW_NAT_ACCNT_MAINTAINER
struct hwnat_ac_args    ac_info[64]; //1 for LAN, 2 for WAN
#endif
#ifndef CONFIG_PSEUDO_SUPPORT //Web UI used
#ifndef CONFIG_RAETH_HW_VLAN_TX 
struct net_device *dev;
#endif	
#endif
//#define DSCP_REMARK_TEST
//#define PREBIND_TEST
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
int DPORT_GMAC1 = -1;
int DPORT_GMAC2 = -1;
#endif
#if 0
static struct class	*hnat_class;
#endif

#if 0
void skb_dump(struct sk_buff* sk) {
        unsigned int i;

        printk("\nskb_dump: from %s with len %d (%d) headroom=%d tailroom=%d\n",
                sk->dev?sk->dev->name:"ip stack",sk->len,sk->truesize,
                skb_headroom(sk),skb_tailroom(sk));

        for(i=(unsigned int)sk->head;i<(unsigned int)sk->tail;i++) {
                if((i % 16) == 0)
		    printk("\n");

                if(i==(unsigned int)sk->head) printk("@h");
                if(i==(unsigned int)sk->data) printk("@d");
                if(i==(unsigned int)LAYER2_HEADER(sk)) printk("*");
                printk("%02X-",*((unsigned char*)i));
        }
        printk("\n");
}
#endif

#if defined (CONFIG_RA_HW_NAT_IP_PASSTHROUGH)
static int IsForward(struct sk_buff * skb);
static int IsForward(struct sk_buff * skb)
{
        struct ethhdr *eth = NULL;
        struct iphdr *iph = NULL;
        struct tcphdr *th = NULL;
        struct udphdr *uh = NULL;
        uint16_t eth_type;
        void *retval;
        //static int deg=5000;
        retval=skb->data;
        //skb_push(skb, ETH_HLEN);
        skb->data = skb->mac_header;
        eth = (struct ethhdr *)skb->data;
        eth_type = eth->h_proto;
        if (eth_type == htons(ETH_P_IP)){
                u16 sport=0, dport=0;
                u32 saddr=0, daddr=0;
                struct in_device* in_dev=NULL;
                struct in_ifaddr *ifa_list=NULL;

                iph=(struct iphdr *)(skb->data + ETH_HLEN);
                skb->data=retval;

                saddr = ntohl(iph->saddr);
                daddr = ntohl(iph->daddr);
                if (iph->protocol == IPPROTO_TCP) {
                      th = (struct tcphdr *)((uint8_t *) iph + (iph->ihl * 4));
                      if(iph->frag_off & htons(IP_MF|IP_OFFSET)) {
                                return 1;
                      }
                       sport = ntohs(th->source);
                       dport = ntohs(th->dest);
                } else if (iph->protocol == IPPROTO_UDP) {
                      uh = (struct udphdr *)((uint8_t *) iph + (iph->ihl * 4));
                      if(iph->frag_off & htons(IP_MF|IP_OFFSET)) {
                                return 1;
                      }
                       sport = ntohs(uh->source);
                       dport = ntohs(uh->dest);
                }
                in_dev = (struct in_device *)skb->dev->ip_ptr;
                if(in_dev) ifa_list=in_dev->ifa_list;

                //printk("Info(dest_ip=%pI4h,sport=%d,dport=%d to dev=%s,%pI4h\n", &daddr,sport, dport,skb->dev->name,&daddr);
                if(ifa_list){
                         if(ifa_list->ifa_address == 0){ //router mode or usb not active
                        //      printk("forward not enable(%p,%x)\n",ifa_list,ifa_list->ifa_address);
                                return 0;
                         }
                         //printk("forward is enable\n");
                }else{
                        //printk("forward not enable\n");
                        return 0;
               }
                if(dport == 67 && sport == 68){
                      //printk("Get packaet(dest_ip=%pI4h,sport=%d,dport=%d to dev=%s,%pI4h\n", &daddr,sport, dport,skb->dev->name,&daddr);
                                //printk("Get DHCP traffic to %pI4h\n",&daddr);
                                return 0;
                } else {
                     struct net_device *br=ra_dev_get_by_name("br0");
                     if(br){
                        in_dev = (struct in_device *)br->ip_ptr;
                        if(in_dev) ifa_list=in_dev->ifa_list;
                        //if(ifa_list)
                          //printk("Get bro ip=%pI4h daddr=%pI4h\n",&ifa_list->ifa_address,&daddr);

                        if(ifa_list && daddr == ntohl(ifa_list->ifa_address) /*&& dport == 80*/){
                                //printk("Get Http traffic to %pI4h\n",&daddr);
                                return 0;
                        }
                     }
                }

        } else {
                skb->data=retval;
                return 0;
        }
        return 1;
}
#endif

uint16_t RemoveVlanTag(struct sk_buff **skb)
{
	struct ethhdr *eth;
	struct vlan_ethhdr *veth;
	uint16_t VirIfIdx;

	veth = (struct vlan_ethhdr *)LAYER2_HEADER(*skb);

	//something wrong
	if ((veth->h_vlan_proto != htons(ETH_P_8021Q)) && (veth->h_vlan_proto != 0x5678)){
		if (printk_ratelimit())
			printk("HNAT: veth->h_vlan_proto = %d,  Reentry packet is untagged frame?\n", veth->h_vlan_proto);
		return 65535;
	}

	VirIfIdx = ntohs(veth->h_vlan_TCI);

	if (skb_cloned(*skb) || skb_shared(*skb)) {

		struct sk_buff *new_skb;
		new_skb = skb_copy(*skb, GFP_ATOMIC);
		kfree_skb(*skb);
		if (!new_skb)
			return 65535;
		*skb = new_skb;
	}

	/* remove VLAN tag */
	(*skb)->data = LAYER2_HEADER(*skb);
	LAYER2_HEADER(*skb) += VLAN_HLEN;
	memmove(LAYER2_HEADER(*skb), (*skb)->data, ETH_ALEN * 2);
	
	skb_pull(*skb, VLAN_HLEN);
	(*skb)->data += ETH_HLEN;	//pointer to layer3 header
	eth = (struct ethhdr *)LAYER2_HEADER(*skb);
	(*skb)->protocol = eth->h_proto;

	return VirIfIdx;

}

static int FoeAllocTbl(uint32_t NumOfEntry)
{
	uint32_t FoeTblSize;
#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
	uint32_t PsTblSize;
#endif
#if defined (CONFIG_RA_HW_NAT_IPV6) && defined (CONFIG_RALINK_MT7621)
	struct FoeEntry *foe_entry;
	int boundary_entry_offset[7] = {12, 25, 38, 51, 76, 89, 102};/*these entries are bad every 128 entries*/
	int entry_base = 0;
	int bad_entry, i, j;
#endif
	dma_addr_t PpePhyFoeBase_tmp;

	FoeTblSize = NumOfEntry * sizeof(struct FoeEntry);

	PpePhyFoeBase_tmp = RegRead(PPE_FOE_BASE);

	if (PpePhyFoeBase_tmp) {
		PpePhyFoeBase = PpePhyFoeBase_tmp;
		PpeFoeBase = ioremap(PpePhyFoeBase_tmp, FoeTblSize);
		if(PpeFoeBase == NULL) {
			printk("PpeFoeBase ioremap fail!!!!\n");
			return 0;
		}
	} else {
		PpeFoeBase = dma_alloc_coherent(NULL, FoeTblSize,
						&PpePhyFoeBase, GFP_KERNEL);

		if(PpeFoeBase == NULL) {
			return 0;
		}
	}

	RegWrite(PPE_FOE_BASE, PpePhyFoeBase);
	memset(PpeFoeBase, 0, FoeTblSize);

#if defined (CONFIG_RA_HW_NAT_IPV6) && defined (CONFIG_RALINK_MT7621)
	for(i=0; entry_base < NumOfEntry; i++) {
		/* set bad entries as static*/
		for(j=0;j<7;j++){
			bad_entry = entry_base + boundary_entry_offset[j];
			foe_entry = &PpeFoeBase[bad_entry];
			foe_entry->udib1.sta = 1;
		}
		entry_base = (i+1)*128;
	}
#endif

#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
	PsTblSize = NumOfEntry * sizeof(struct PsEntry);
	//printk("PsTblSize is 0x%x \n", PsTblSize);
        PpePsBase = dma_alloc_coherent(NULL, FoeTblSize, &PpePhyPsBase, GFP_KERNEL);
	if(PpePsBase == NULL) {
		return 0;
	}

	RegWrite(PS_TB_BASE, PpePhyPsBase);
	memset(PpePsBase, 0, PsTblSize);
#endif
	return 1;
}

#ifdef NAT_DEBUG
static uint8_t *ShowCpuReason(struct sk_buff *skb)
{
	static uint8_t Buf[32];

	switch (FOE_AI(skb)) {
#if defined (CONFIG_HNAT_V2)
	case TTL_0:
		return ("IPv4(IPv6) TTL(hop limit)\n");
	case HAS_OPTION_HEADER:
		return ("Ipv4(IPv6) has option(extension) header\n");
	case NO_FLOW_IS_ASSIGNED:
		return ("No flow is assigned\n");
	case IPV4_WITH_FRAGMENT:
		return ("IPv4 HNAT doesn't support IPv4 /w fragment\n");
	case IPV4_HNAPT_DSLITE_WITH_FRAGMENT:
		return ("IPv4 HNAPT/DS-Lite doesn't support IPv4 /w fragment\n");
	case IPV4_HNAPT_DSLITE_WITHOUT_TCP_UDP:
		return ("IPv4 HNAPT/DS-Lite can't find TCP/UDP sport/dport\n");
	case IPV6_5T_6RD_WITHOUT_TCP_UDP:
		return ("IPv6 5T-route/6RD can't find TCP/UDP sport/dport\n");
	case TCP_FIN_SYN_RST:
		return ("Ingress packet is TCP fin/syn/rst\n");
	case UN_HIT:
		return ("FOE Un-hit\n");
	case HIT_UNBIND:
		return ("FOE Hit unbind\n");
	case HIT_UNBIND_RATE_REACH:
		return ("FOE Hit unbind & rate reach\n");
	case HIT_BIND_TCP_FIN:
		return ("Hit bind PPE TCP FIN entry\n");
	case HIT_BIND_TTL_1:
		return ("Hit bind PPE entry and TTL(hop limit) = 1 and TTL(hot limit) - 1\n");
	case HIT_BIND_WITH_VLAN_VIOLATION:
		return ("Hit bind and VLAN replacement violation\n");
	case HIT_BIND_KEEPALIVE_UC_OLD_HDR:
		return ("Hit bind and keep alive with unicast old-header packet\n");
	case HIT_BIND_KEEPALIVE_MC_NEW_HDR:
		return ("Hit bind and keep alive with multicast new-header packet\n");
	case HIT_BIND_KEEPALIVE_DUP_OLD_HDR:
		return ("Hit bind and keep alive with duplicate old-header packet\n");
	case HIT_BIND_FORCE_TO_CPU:
		return ("FOE Hit bind & force to CPU\n");
	case HIT_BIND_EXCEED_MTU:
		return ("Hit bind and exceed MTU\n");
	case HIT_BIND_MULTICAST_TO_CPU:
		return ("Hit bind multicast packet to CPU\n");
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	case HIT_BIND_MULTICAST_TO_GMAC_CPU:
		return ("Hit bind multicast packet to GMAC & CPU\n");
	case HIT_PRE_BIND:
		return ("Pre bind\n");
#endif
#else
	case TTL_0:		/* 0x80 */
		return ("TTL=0\n");
	case FOE_EBL_NOT_IPV4_HLEN5:	/* 0x90 */
		return ("FOE enable & not IPv4h5nf\n");
	case FOE_EBL_NOT_TCP_UDP_L4_READY:	/* 0x91 */
		return ("FOE enable & not TCP/UDP/L4_read\n");
	case TCP_SYN_FIN_RST:	/* 0x92 */
		return ("TCP SYN/FIN/RST\n");
	case UN_HIT:		/* 0x93 */
		return ("Un-hit\n");
	case HIT_UNBIND:	/* 0x94 */
		return ("Hit unbind\n");
	case HIT_UNBIND_RATE_REACH:	/* 0x95 */
		return ("Hit unbind & rate reach\n");
	case HIT_FIN:		/* 0x96 */
		return ("Hit fin\n");
	case HIT_BIND_TTL_1:	/* 0x97 */
		return ("Hit bind & ttl=1 & ttl-1\n");
	case HIT_BIND_KEEPALIVE:	/* 0x98 */
		return ("Hit bind & keep alive\n");
	case HIT_BIND_FORCE_TO_CPU:	/* 0x99 */
		return ("Hit bind & force to CPU\n");
	case ACL_FOE_TBL_ERR:	/* 0x9A */
		return ("acl link foe table error (!static & !unbind)\n");
	case ACL_TBL_TTL_1:	/* 0x9B */
		return ("acl link FOE table & TTL=1 & TTL-1\n");
	case ACL_ALERT_CPU:	/* 0x9C */
		return ("acl alert cpu\n");
	case NO_FORCE_DEST_PORT:	/* 0xA0 */
		return ("No force destination port\n");
	case ACL_FORCE_PRIORITY0:	/* 0xA8 */
		return ("ACL FORCE PRIORITY0\n");
	case ACL_FORCE_PRIORITY1:	/* 0xA9 */
		return ("ACL FORCE PRIORITY1\n");
	case ACL_FORCE_PRIORITY2:	/* 0xAA */
		return ("ACL FORCE PRIORITY2\n");
	case ACL_FORCE_PRIORITY3:	/* 0xAB */
		return ("ACL FORCE PRIORITY3\n");
	case ACL_FORCE_PRIORITY4:	/* 0xAC */
		return ("ACL FORCE PRIORITY4\n");
	case ACL_FORCE_PRIORITY5:	/* 0xAD */
		return ("ACL FORCE PRIORITY5\n");
	case ACL_FORCE_PRIORITY6:	/* 0xAE */
		return ("ACL FORCE PRIORITY6\n");
	case ACL_FORCE_PRIORITY7:	/* 0xAF */
		return ("ACL FORCE PRIORITY7\n");
	case EXCEED_MTU:	/* 0xA1 */
		return ("Exceed mtu\n");
#endif
	}

	sprintf(Buf, "CPU Reason Error - %X\n", FOE_AI(skb));
	return (Buf);
}
#endif
uint32_t FoeDumpPkt(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry = &PpeFoeBase[FOE_ENTRY_NUM(skb)];

	NAT_PRINT("\nRx===<FOE_Entry=%d>=====\n", FOE_ENTRY_NUM(skb));
	NAT_PRINT("RcvIF=%s\n", skb->dev->name);
	NAT_PRINT("FOE_Entry=%d\n", FOE_ENTRY_NUM(skb));
	NAT_PRINT("CPU Reason=%s", ShowCpuReason(skb));
	NAT_PRINT("ALG=%d\n", FOE_ALG(skb));
	NAT_PRINT("SP=%d\n", FOE_SP(skb));
	
	//some special alert occurred, so entry_num is useless (just skip it)
	if(FOE_ENTRY_NUM(skb)==0x3fff || FOE_ENTRY_NUM(skb) >= FOE_4TB_SIZ) {
		return 1;
	}

	/* PPE: IPv4 packet=IPV4_HNAT IPv6 packet=IPV6_ROUTE */
	if (IS_IPV4_GRP(foe_entry)) {
		NAT_PRINT("Information Block 1=%x\n", foe_entry->ipv4_hnapt.info_blk1);
		NAT_PRINT("SIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.sip));
		NAT_PRINT("DIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.dip));
		NAT_PRINT("SPORT=%d\n", foe_entry->ipv4_hnapt.sport);
		NAT_PRINT("DPORT=%d\n", foe_entry->ipv4_hnapt.dport);
		NAT_PRINT("Information Block 2=%x\n",
			  foe_entry->ipv4_hnapt.info_blk2);
	}
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
	else if (IS_IPV6_GRP(foe_entry)) {
		NAT_PRINT("Information Block 1=%x\n", foe_entry->ipv6_5t_route.info_blk1);
		NAT_PRINT("IPv6_SIP=%08X:%08X:%08X:%08X\n",
			  foe_entry->ipv6_5t_route.ipv6_sip0,
			  foe_entry->ipv6_5t_route.ipv6_sip1,
			  foe_entry->ipv6_5t_route.ipv6_sip2,
			  foe_entry->ipv6_5t_route.ipv6_sip3);
		NAT_PRINT("IPv6_DIP=%08X:%08X:%08X:%08X\n",
			  foe_entry->ipv6_5t_route.ipv6_dip0,
			  foe_entry->ipv6_5t_route.ipv6_dip1,
			  foe_entry->ipv6_5t_route.ipv6_dip2,
			  foe_entry->ipv6_5t_route.ipv6_dip3);
		if(IS_IPV6_FLAB_EBL()) {
			NAT_PRINT("Flow Label=%08X\n", (foe_entry->ipv6_5t_route.sport << 16) | 
							(foe_entry->ipv6_5t_route.dport));
		} else {
			NAT_PRINT("SPORT=%d\n", foe_entry->ipv6_5t_route.sport);
			NAT_PRINT("DPORT=%d\n", foe_entry->ipv6_5t_route.dport);
		}
		NAT_PRINT("Information Block 2=%x\n",
			  foe_entry->ipv6_5t_route.info_blk2);
	}
#endif
#endif
	else {
		NAT_PRINT("unknown Pkt_type=%d\n", foe_entry->bfib1.pkt_type);
	}

	NAT_PRINT("==================================\n");
	return 1;

}
#if defined (CONFIG_SUPPORT_WLAN_OPTIMIZE)
int getBrLan = 0;
uint32_t brNetmask;
uint32_t br0Ip;
char br0_mac_address[6];
void get_bridge_info(void)
{
	struct net_device *br0_dev;
	struct in_device *br0_in_dev;

#if defined (CONFIG_SUPPORT_OPENWRT)
	br0_dev = dev_get_by_name_rcu(&init_net,"br-lan");
#else
	br0_dev = dev_get_by_name_rcu(&init_net,"br0");
#endif
	br0_in_dev = in_dev_get(br0_dev);
	brNetmask = ntohl(br0_in_dev->ifa_list->ifa_mask);
	br0Ip = ntohl(br0_in_dev->ifa_list->ifa_address);
	//memcpy(&br0_mac_address,br0_dev->dev_addr,6);

	if(br0_dev != NULL) {
		dev_put(br0_dev);
		//dev_put(br0_in_dev);
	} else
		printk("br0_dev = NULL\n");
	printk("br0Ip = %x, brNetmask = %x\n", br0Ip, brNetmask);
	getBrLan = 1;

}
int BridgeShortCutTx(struct sk_buff * skb)
{
	struct iphdr *iph = NULL;
	uint32_t daddr = 0;
	uint32_t saddr = 0;
	u32 eth_type = 0;
	u32 ppp_tag = 0;
	struct vlan_hdr *vh = NULL;
	struct ethhdr *eth = NULL;
	struct pppoe_hdr *peh = NULL;
	u8 vlan1_gap = 0;
	u8 vlan2_gap = 0;
	u8 pppoe_gap = 0;

#ifdef CONFIG_RAETH_HW_VLAN_TX
	struct vlan_hdr pseudo_vhdr;
#endif

	eth = (struct ethhdr *)skb->data;
	if(is_multicast_ether_addr(&eth->h_dest[0]))
		return 0;
	eth_type = eth->h_proto;
	if ((eth_type == htons(ETH_P_8021Q)) ||
	    (((eth_type) && 0x00FF) == htons(ETH_P_8021Q)) || vlan_tx_tag_present(skb)) {

#ifdef CONFIG_RAETH_HW_VLAN_TX
		pseudo_vhdr.h_vlan_TCI = htons(vlan_tx_tag_get(skb));
		pseudo_vhdr.h_vlan_encapsulated_proto = eth->h_proto;
		vh = (struct vlan_hdr *)&pseudo_vhdr;
		vlan1_gap = VLAN_HLEN;
#else
		vlan1_gap = VLAN_HLEN;
		vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
#endif

		/* VLAN + PPPoE */
		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			pppoe_gap = 8;
			eth_type = vh->h_vlan_encapsulated_proto;
			/* Double VLAN = VLAN + VLAN */
		} else if ((vh->h_vlan_encapsulated_proto == htons(ETH_P_8021Q)) ||
			   ((vh->h_vlan_encapsulated_proto) && 0x00FF) == htons(ETH_P_8021Q)) {

			vlan2_gap = VLAN_HLEN;
			vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + VLAN_HLEN);
			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
				pppoe_gap = 8;
				eth_type = vh->h_vlan_encapsulated_proto;
			}else
				eth_type = vh->h_vlan_encapsulated_proto;
		}
	} else if (ntohs(eth_type) == ETH_P_PPP_SES) {
		/* PPPoE + IP */
		pppoe_gap = 8;
		peh = (struct pppoe_hdr *)(skb->data + ETH_HLEN + vlan1_gap);
		ppp_tag = peh->tag[0].tag_type;
	}

	if (getBrLan == 0)
		get_bridge_info();
	/* set layer4 start addr */
	if ((eth_type == htons(ETH_P_IP)) || (eth_type == htons(ETH_P_PPP_SES) && ppp_tag == htons(PPP_IP))) {
		iph = (struct iphdr *)(skb->data + ETH_HLEN + vlan1_gap + vlan2_gap + pppoe_gap);
		daddr = ntohl(iph->daddr);
		saddr = ntohl(iph->saddr);
	}

	if (((br0Ip & brNetmask) == (daddr & brNetmask)) && ((daddr & brNetmask) == (saddr & brNetmask))){
		return 1;
	}else
		return 0;

}

int BridgeShortCutRx(struct sk_buff * skb)
{
	struct iphdr *iph = NULL;
	uint32_t daddr;

	if (getBrLan == 0)
		get_bridge_info();

	iph = (struct iphdr *)(skb->data);
	daddr = ntohl(iph->daddr);

	if ((br0Ip & brNetmask) == (daddr & brNetmask))
		return 1;
	else
		return 0;
}
#endif
#ifdef CONFIG_OPTIMIZED_MEMCPY_IN_GOBINET
static inline int __skb_cow_1(struct sk_buff *skb, unsigned int headroom,
			    int cloned)
{
	int delta = 0;

	if (headroom > skb_headroom(skb))
		delta = headroom - skb_headroom(skb);
	if (delta /*|| cloned*/){
		return pskb_expand_head(skb, ALIGN(delta, NET_SKB_PAD), 0,
					GFP_ATOMIC);
	}
	return 0;
}

static inline int skb_cow_head_1(struct sk_buff *skb, unsigned int headroom)
{
	return __skb_cow_1(skb, headroom, skb_header_cloned(skb));
}

static inline struct sk_buff *vlan_insert_tag_1(struct sk_buff *skb,
                                              __be16 vlan_proto, u16 vlan_tci)
{
        struct vlan_ethhdr *veth;

        if (skb_cow_head_1(skb, VLAN_HLEN) < 0) {
                kfree_skb(skb);
                return NULL;
        }
        veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

        /* Move the mac addresses to the beginning of the new header. */
        memmove(skb->data, skb->data + VLAN_HLEN, 2 * ETH_ALEN);
        skb->mac_header -= VLAN_HLEN;

        /* first, the ethernet type */
        veth->h_vlan_proto = vlan_proto;

        /* now, the TCI */
        veth->h_vlan_TCI = htons(vlan_tci);

        return skb;
}

static inline struct sk_buff *__vlan_put_tag_1(struct sk_buff *skb,
                                             __be16 vlan_proto, u16 vlan_tci)
{
        skb = vlan_insert_tag_1(skb, vlan_proto, vlan_tci);
        if (skb)
                skb->protocol = vlan_proto;
        return skb;
}
#endif
/* push different VID for WiFi pseudo interface or USB external NIC */
uint32_t PpeExtIfRxHandler(struct sk_buff * skb)
{
#if defined  (CONFIG_RA_HW_NAT_WIFI) || defined (CONFIG_RA_HW_NAT_NIC_USB)
	uint16_t VirIfIdx = 0;
	struct ethhdr *eth = (struct ethhdr *)LAYER2_HEADER(skb);
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	int i=0;
	int dev_match = 0;
#endif
	/* PPE can only handle IPv4/IPv6/PPP packets */
	if (((skb->protocol != htons(ETH_P_8021Q)) &&
	     (skb->protocol != htons(ETH_P_IP)) && (skb->protocol != htons(ETH_P_IPV6)) && 
	     (skb->protocol != htons(ETH_P_PPP_SES)) && (skb->protocol != htons(ETH_P_PPP_DISC))) ||
	     is_multicast_ether_addr(&eth->h_dest[0])) {
		return 1;
	}

#if !defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)	
	if (skb->dev == DstPort[DP_RA0]) {
		VirIfIdx = DP_RA0;
	}
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RTPCI_AP_MBSS) || defined (CONFIG_MBSS_SUPPORT)
	else if (skb->dev == DstPort[DP_RA1]) {
		VirIfIdx = DP_RA1;
	} else if (skb->dev == DstPort[DP_RA2]) {
		VirIfIdx = DP_RA2;
	} else if (skb->dev == DstPort[DP_RA3]) {
		VirIfIdx = DP_RA3;
	} else if (skb->dev == DstPort[DP_RA4]) {
		VirIfIdx = DP_RA4;
	} else if (skb->dev == DstPort[DP_RA5]) {
		VirIfIdx = DP_RA5;
	} else if (skb->dev == DstPort[DP_RA6]) {
		VirIfIdx = DP_RA6;
	} else if (skb->dev == DstPort[DP_RA7]) {
		VirIfIdx = DP_RA7;
	} else if (skb->dev == DstPort[DP_RA8]) {
		VirIfIdx = DP_RA8;
	} else if (skb->dev == DstPort[DP_RA9]) {
		VirIfIdx = DP_RA9;
	} else if (skb->dev == DstPort[DP_RA10]) {
		VirIfIdx = DP_RA10;
	} else if (skb->dev == DstPort[DP_RA11]) {
		VirIfIdx = DP_RA11;
	} else if (skb->dev == DstPort[DP_RA12]) {
		VirIfIdx = DP_RA12;
	} else if (skb->dev == DstPort[DP_RA13]) {
		VirIfIdx = DP_RA13;
	} else if (skb->dev == DstPort[DP_RA14]) {
		VirIfIdx = DP_RA14;
	} else if (skb->dev == DstPort[DP_RA15]) {
		VirIfIdx = DP_RA15;
	}
#endif // CONFIG_RT2860V2_AP_MBSS || CONFIG_RTPCI_AP_MBSS //
#if defined (CONFIG_RT2860V2_AP_WDS) || defined (CONFIG_RTPCI_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	else if (skb->dev == DstPort[DP_WDS0]) {
		VirIfIdx = DP_WDS0;
	} else if (skb->dev == DstPort[DP_WDS1]) {
		VirIfIdx = DP_WDS1;
	} else if (skb->dev == DstPort[DP_WDS2]) {
		VirIfIdx = DP_WDS2;
	} else if (skb->dev == DstPort[DP_WDS3]) {
		VirIfIdx = DP_WDS3;
	}
#endif
#if defined (CONFIG_RT2860V2_AP_APCLI) || defined (CONFIG_RTPCI_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	else if (skb->dev == DstPort[DP_APCLI0]) {
		VirIfIdx = DP_APCLI0;
	}
#endif // CONFIG_RT2860V2_AP_APCLI //
#if defined (CONFIG_RT2860V2_AP_MESH) || defined (CONFIG_RTPCI_AP_MESH)
	else if (skb->dev == DstPort[DP_MESH0]) {
		VirIfIdx = DP_MESH0;
	}
#endif // CONFIG_RT2860V2_AP_MESH //
#if defined (CONFIG_RTDEV_MII) || defined (CONFIG_RTDEV_USB) || \
    defined (CONFIG_RTDEV_PCI) || defined (CONFIG_RTDEV) || \
    defined (CONFIG_RLT_AP_SUPPORT)
	else if (skb->dev == DstPort[DP_RAI0]) {
		VirIfIdx = DP_RAI0;
	}
#if defined (CONFIG_RT3090_AP_MBSS) || defined (CONFIG_RT5392_AP_MBSS) || \
    defined (CONFIG_RT3572_AP_MBSS) || defined (CONFIG_RT5572_AP_MBSS) || \
    defined (CONFIG_RT5592_AP_MBSS) || defined (CONFIG_RT3593_AP_MBSS) || \
    defined (CONFIG_RTPCI_AP_MBSS)  || defined (CONFIG_MBSS_SUPPORT)
	else if (skb->dev == DstPort[DP_RAI1]) {
		VirIfIdx = DP_RAI1;
	} else if (skb->dev == DstPort[DP_RAI2]) {
		VirIfIdx = DP_RAI2;
	} else if (skb->dev == DstPort[DP_RAI3]) {
		VirIfIdx = DP_RAI3;
	} else if (skb->dev == DstPort[DP_RAI4]) {
		VirIfIdx = DP_RAI4;
	} else if (skb->dev == DstPort[DP_RAI5]) {
		VirIfIdx = DP_RAI5;
	} else if (skb->dev == DstPort[DP_RAI6]) {
		VirIfIdx = DP_RAI6;
	} else if (skb->dev == DstPort[DP_RAI7]) {
		VirIfIdx = DP_RAI7;
	} else if (skb->dev == DstPort[DP_RAI8]) {
		VirIfIdx = DP_RAI8;
	} else if (skb->dev == DstPort[DP_RAI9]) {
		VirIfIdx = DP_RAI9;
	} else if (skb->dev == DstPort[DP_RAI10]) {
		VirIfIdx = DP_RAI10;
	} else if (skb->dev == DstPort[DP_RAI11]) {
		VirIfIdx = DP_RAI11;
	} else if (skb->dev == DstPort[DP_RAI12]) {
		VirIfIdx = DP_RAI12;
	} else if (skb->dev == DstPort[DP_RAI13]) {
		VirIfIdx = DP_RAI13;
	} else if (skb->dev == DstPort[DP_RAI14]) {
		VirIfIdx = DP_RAI14;
	} else if (skb->dev == DstPort[DP_RAI15]) {
		VirIfIdx = DP_RAI15;
	}
#endif // CONFIG_RTDEV_AP_MBSS || CONFIG_RTPCI_AP_MBSS //
#endif // CONFIG_RTDEV_MII || CONFIG_RTDEV_USB || CONFIG_RTDEV_PCI
#if defined (CONFIG_RT3090_AP_APCLI) || defined (CONFIG_RT5392_AP_APCLI) || \
    defined (CONFIG_RT3572_AP_APCLI) || defined (CONFIG_RT5572_AP_APCLI) || \
    defined (CONFIG_RT5592_AP_APCLI) || defined (CONFIG_RT3593_AP_APCLI) || \
    defined (CONFIG_MT7610_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	else if (skb->dev == DstPort[DP_APCLII0]) {
		VirIfIdx = DP_APCLII0;
	}
#endif // CONFIG_RTDEV_AP_APCLI //
#if defined (CONFIG_RT3090_AP_WDS) || defined (CONFIG_RT5392_AP_WDS) || \
    defined (CONFIG_RT3572_AP_WDS) || defined (CONFIG_RT5572_AP_WDS) || \
    defined (CONFIG_RT5592_AP_WDS) || defined (CONFIG_RT3593_AP_WDS) || \
    defined (CONFIG_MT7610_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	else if (skb->dev == DstPort[DP_WDSI0]) {
		VirIfIdx = DP_WDSI0;
	} else if (skb->dev == DstPort[DP_WDSI1]) {
		VirIfIdx = DP_WDSI1;
	} else if (skb->dev == DstPort[DP_WDSI2]) {
		VirIfIdx = DP_WDSI2;
	} else if (skb->dev == DstPort[DP_WDSI3]) {
		VirIfIdx = DP_WDSI3;
	}
#endif // CONFIG_RTDEV_AP_APCLI //
#if defined (CONFIG_RT3090_AP_MESH) || defined (CONFIG_RT5392_AP_MESH) || \
    defined (CONFIG_RT3572_AP_MESH) || defined (CONFIG_RT5572_AP_MESH) || \
    defined (CONFIG_RT5592_AP_MESH) || defined (CONFIG_RT3593_AP_MESH) || \
    defined (CONFIG_MT7610_AP_MESH)
	else if (skb->dev == DstPort[DP_MESHI0]) {
		VirIfIdx = DP_MESHI0;
	}
#endif // CONFIG_RTDEV_AP_MESH //
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
	else if (skb->dev == DstPort[DP_PCI]) {
		VirIfIdx = DP_PCI;
	}
	else if (skb->dev == DstPort[DP_USB]) {
		VirIfIdx = DP_USB;
	}
#endif	
	else {
		if (printk_ratelimit())  /* __MSTC__, Vincent: Remove lot of print */
		printk("HNAT: The interface %s is unknown\n", skb->dev->name);
	}

#endif

	LAYER3_HEADER(skb) = skb->data;

#if defined (CONFIG_SUPPORT_WLAN_OPTIMIZE_RX)
	//PpeRxParseLayerInfo(skb);
	if (BridgeShortCutRx(skb)) return 1;//Bridge ==> sw path (rps)
#endif
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	for (i=0; i<MAX_IF_NUM; i++){
		if(DstPort[i] == skb->dev){
			VirIfIdx = i;
			dev_match = 1;
			//printk("%s : Interface=%s, VirIfIdx=%x\n", __func__, skb->dev, VirIfIdx);
			break;
		}
	}
	if (dev_match == 0){
		if (printk_ratelimit())
			printk("%s UnKnown Interface, VirIfIdx=%x\n", __func__, VirIfIdx);
		kfree_skb(skb);//MSTC ////kfree_skb(skb);
		return 0;//MSTC //return 1;
	}
#endif

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header before calling hard_start_xmit
	//push vlan tag to stand for actual incoming interface,
	//so HNAT module can know the actual incoming interface from vlan id.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	skb->vlan_proto= htons(ETH_P_8021Q);
#ifdef CONFIG_RAETH_HW_VLAN_TX
	skb->vlan_tci |= VLAN_TAG_PRESENT;
	skb->vlan_tci |= VirIfIdx;
#else
#ifdef CONFIG_OPTIMIZED_MEMCPY_IN_GOBINET
	skb = __vlan_put_tag_1(skb, skb->vlan_proto, VirIfIdx);
#else	
	skb = __vlan_put_tag(skb, skb->vlan_proto, VirIfIdx);
#endif
#endif
#else
#ifdef CONFIG_RAETH_HW_VLAN_TX
	skb->vlan_tci |= VLAN_TAG_PRESENT;
	skb->vlan_tci |= VirIfIdx;
#else
#ifdef CONFIG_OPTIMIZED_MEMCPY_IN_GOBINET
	skb = __vlan_put_tag_1(skb, VirIfIdx);
#else	
	skb = __vlan_put_tag(skb, VirIfIdx);
#endif
#endif
#endif

	//redirect to PPE
	FOE_AI(skb) = UN_HIT;
	FOE_MAGIC_TAG(skb) = FOE_MAGIC_PPE;
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	skb->dev = DstPort[DPORT_GMAC1];	//we use GMAC1 to send the packet to PPE
#else
	skb->dev = DstPort[DP_GMAC1];	//we use GMAC1 to send the packet to PPE
#endif
	dev_queue_xmit(skb);
	return 0;
#else

	return 1;
#endif // CONFIG_RA_HW_NAT_WIFI || CONFIG_RA_HW_NAT_NIC_USB //

}

uint32_t PpeExtIfPingPongHandler(struct sk_buff * skb)
{
#if defined (CONFIG_RA_HW_NAT_WIFI) || defined (CONFIG_RA_HW_NAT_NIC_USB)
	struct ethhdr *eth = NULL;
	uint16_t VirIfIdx = 0;
	struct net_device *dev;
	VirIfIdx = RemoveVlanTag(&skb);

	//recover to right incoming interface
	if (VirIfIdx < MAX_IF_NUM && DstPort[VirIfIdx]!=NULL) {
		skb->dev = DstPort[VirIfIdx];
	} else {
		if (printk_ratelimit())
			printk("HNAT: %s unknow interface (VirIfIdx=%d)\n",  __func__, VirIfIdx);
		return 1;
	}

	eth = (struct ethhdr *)LAYER2_HEADER(skb);

	if (eth->h_dest[0] & 1) {
		if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0) {
			skb->pkt_type = PACKET_BROADCAST;
		} else {
			skb->pkt_type = PACKET_MULTICAST;
		}
	} else {

		skb->pkt_type = PACKET_OTHERHOST;
		for(VirIfIdx=0; VirIfIdx < MAX_IF_NUM; VirIfIdx++) {
			dev = DstPort[VirIfIdx];
			if (dev !=NULL && memcmp(eth->h_dest, dev->dev_addr, ETH_ALEN) == 0) {
				skb->pkt_type = PACKET_HOST;
				break;
			}
		}
	}

#endif
	return 1;

}

uint32_t PpeKeepAliveHandler(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	struct ethhdr *eth = NULL;
	uint16_t eth_type = ntohs(skb->protocol);
	uint32_t vlan1_gap = 0;
	uint32_t vlan2_gap = 0;
	uint32_t pppoe_gap = 0;
	struct vlan_hdr *vh;
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;

	/*
	 * try to recover to original SMAC/DMAC, but we don't have such information.
	 * just use SMAC as DMAC and set Multicast address as SMAC.
	 */
	eth = (struct ethhdr *)(skb->data - ETH_HLEN);

	memcpy(eth->h_dest, eth->h_source, ETH_ALEN);
	memcpy(eth->h_source, eth->h_dest, ETH_ALEN);
	eth->h_source[0] = 0x1;	//change to multicast packet, make bridge not learn this packet
	if (eth_type == ETH_P_8021Q) {
		vlan1_gap = VLAN_HLEN;
		vh = (struct vlan_hdr *)skb->data;

		if (ntohs(vh->h_vlan_TCI) == wan_vid) {
			/* It make packet like coming from LAN port */
			vh->h_vlan_TCI = htons(lan_vid);

		} else {
			/* It make packet like coming from WAN port */
			vh->h_vlan_TCI = htons(wan_vid);
		}

		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			pppoe_gap = 8;
		} else if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_8021Q) {
			vlan2_gap = VLAN_HLEN;
			vh = (struct vlan_hdr *)(skb->data + VLAN_HLEN);

			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) ==
			    ETH_P_PPP_SES) {
				pppoe_gap = 8;
			} else {
				/* VLAN + VLAN + IP */
				eth_type = ntohs(vh->h_vlan_encapsulated_proto);
			}
		} else {
			/* VLAN + IP */
			eth_type = ntohs(vh->h_vlan_encapsulated_proto);
		}
	}

	/* Only Ipv4 NAT need KeepAlive Packet to refresh iptable */
	if (eth_type == ETH_P_IP) {
		iph =
		    (struct iphdr *)(skb->data + vlan1_gap + vlan2_gap +
				     pppoe_gap);

		//Recover to original layer 4 header 
		if (iph->protocol == IPPROTO_TCP) {
			th = (struct tcphdr *)((uint8_t *) iph + iph->ihl * 4);
			FoeToOrgTcpHdr(foe_entry, iph, th);

		} else if (iph->protocol == IPPROTO_UDP) {
			uh = (struct udphdr *)((uint8_t *) iph + iph->ihl * 4);
			FoeToOrgUdpHdr(foe_entry, iph, uh);
		}
		//Recover to original layer 3 header 
		FoeToOrgIpHdr(foe_entry, iph);
	} else if (eth_type == ETH_P_IPV6) {
		/* Nothing to do */
	} else {
		return 1;
	}

	/*
	 * Ethernet driver will call eth_type_trans() to update skb->pkt_type.
	 * If(destination mac != my mac) 
	 *   skb->pkt_type=PACKET_OTHERHOST;
	 *
	 * In order to pass ip_rcv() check, we change pkt_type to PACKET_HOST here
	 */
	skb->pkt_type = PACKET_HOST;
	return 1;

}

int PpeHitBindForceToCpuHandler(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
#if defined (CONFIG_RAETH_QDMA)
        uint16_t VirIfIdx = 0;
         
	VirIfIdx = RemoveVlanTag(&skb);
	foe_entry = &PpeFoeBase[VirIfIdx];
#endif
	if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)) {
		skb->dev = DstPort[foe_entry->ipv4_hnapt.act_dp];
	}
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
	else if (IS_IPV4_DSLITE(foe_entry)) {
		skb->dev = DstPort[foe_entry->ipv4_dslite.act_dp];
	} else if (IS_IPV6_3T_ROUTE(foe_entry)) {
		skb->dev = DstPort[foe_entry->ipv6_3t_route.act_dp];
	} else if (IS_IPV6_5T_ROUTE(foe_entry)) {
		skb->dev = DstPort[foe_entry->ipv6_5t_route.act_dp];
	} else if (IS_IPV6_6RD(foe_entry)) {
		skb->dev = DstPort[foe_entry->ipv6_6rd.act_dp];
	} else {
		return 1;
	}
#endif
#endif
	/* interface is unkown */
	if(skb->dev == NULL) {
		if (printk_ratelimit())
			printk("%s, interface is unkown\n", __func__);
		kfree_skb(skb);
		return 0;			
	}

	LAYER3_HEADER(skb) = skb->data;
	skb_push(skb, ETH_HLEN);	//pointer to layer2 header
	dev_queue_xmit(skb);
	return 0;
}

#if defined (CONFIG_HNAT_V2)
int PpeHitBindForceMcastToWiFiHandler(struct sk_buff *skb)
{
	
#if defined (CONFIG_RA_HW_NAT_WIFI) || defined (CONFIG_RA_HW_NAT_NIC_USB)
	int i = 0;
	struct sk_buff *skb2;

#if !defined (CONFIG_RAETH_GMAC2)
        /*if we only use GMAC1, we need to use vlan id to identify LAN/WAN port.
          otherwise, CPU send untag packet to switch so we don't need to remove vlan tag before sending to WiFi interface
        */
        RemoveVlanTag(&skb); //pointer to layer3 header
#endif

	LAYER3_HEADER(skb) = skb->data;
	skb_push(skb, ETH_HLEN);	//pointer to layer2 header

#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	for (i=0; i < MAX_IF_NUM; i++) {
		if(DstPort[i] != NULL && strncmp(DstPort[i]->name, "eth", 3)
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
			&& strncmp(DstPort[i]->name, "wwan", 4)
#endif
		) {
			skb2 = skb_clone(skb, GFP_ATOMIC);

			if (!skb2)
				return -ENOMEM;

			skb2->dev = DstPort[i];
			dev_queue_xmit(skb2);
		}
	}
#else
#if defined (CONFIG_RA_HW_NAT_WIFI)
	for(i= DP_RA0 ;i < MAX_WIFI_IF_NUM; i++) {
		if(DstPort[i] != NULL) {
			skb2 = skb_clone(skb, GFP_ATOMIC);

			if (!skb2)
				return -ENOMEM;

			skb2->dev = DstPort[i];
			dev_queue_xmit(skb2);
		}
	}
#elif defined (CONFIG_RA_HW_NAT_NIC_USB)
	if(DstPort[DP_PCI] != NULL) {
		skb2 = skb_clone(skb, GFP_ATOMIC);

		if (!skb2)
			return -ENOMEM;

		skb2->dev = DstPort[DP_PCI];
		dev_queue_xmit(skb2);
	}
#endif
#endif //CONFIG_RA_HW_NAT_WIFI_NEW_ARCH
#endif
	kfree_skb(skb);
	
	return 0;
}
#endif

#if defined (CONFIG_RA_HW_NAT_ACL2UP_HELPER)
uint32_t PpeGetUpFromACLRule(struct sk_buff *skb)
{
	struct ethhdr *eth = NULL;
	uint16_t eth_type = 0;
	uint32_t vlan1_gap = 0;
	uint32_t vlan2_gap = 0;
	uint32_t pppoe_gap = 0;
	struct vlan_hdr *vh;
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;

	AclClassifyKey NewRateReach;
	eth = (struct ethhdr *)(skb->data - ETH_HLEN);

	memset(&NewRateReach, 0, sizeof(AclClassifyKey));
	memcpy(NewRateReach.Mac, eth->h_source, ETH_ALEN);
	NewRateReach.Ethertype = eth_type;	//Ethertype
	if (eth_type == ETH_P_8021Q) {
		vlan1_gap = VLAN_HLEN;
		vh = (struct vlan_hdr *)skb->data;
		NewRateReach.Vid = ntohs(vh->h_vlan_TCI);	//VID
		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			pppoe_gap = 8;
		} else if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_8021Q) {
			vlan2_gap = VLAN_HLEN;
			vh = (struct vlan_hdr *)(skb->data + VLAN_HLEN);

			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) ==
			    ETH_P_PPP_SES) {
				pppoe_gap = 8;
			} else {
				/* VLAN + VLAN + IP */
				eth_type = ntohs(vh->h_vlan_encapsulated_proto);
			}
		} else {
			/* VLAN + IP */
			eth_type = ntohs(vh->h_vlan_encapsulated_proto);
		}
	}

	/*IPv4 */
	if (eth_type == ETH_P_IP) {
		iph =
		    (struct iphdr *)(skb->data + vlan1_gap + vlan2_gap +
				     pppoe_gap);

		NewRateReach.Sip = ntohl(iph->saddr);
		NewRateReach.Dip = ntohl(iph->daddr);
		NewRateReach.Tos = iph->tos;	//TOS
		if (iph->protocol == IPPROTO_TCP) {
			th = (struct tcphdr *)((uint8_t *) iph + iph->ihl * 4);
			NewRateReach.Sp = ntohs(th->source);
			NewRateReach.Dp = ntohs(th->dest);
			NewRateReach.Proto = ACL_PROTO_TCP;
		} else if (iph->protocol == IPPROTO_UDP) {
			uh = (struct udphdr *)((uint8_t *) iph + iph->ihl * 4);
			NewRateReach.Sp = ntohs(uh->source);
			NewRateReach.Dp = ntohs(uh->dest);
			NewRateReach.Proto = ACL_PROTO_UDP;

		}

	}

	/*classify user priority */
	return AclClassify(&NewRateReach);

}
#endif

int32_t PpeRxHandler(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry = &PpeFoeBase[FOE_ENTRY_NUM(skb)];

#if defined (CONFIG_HNAT_V2)
//MTK //#if defined (CONFIG_RALINK_MT7620)
	struct ethhdr *eth = (struct ethhdr *)(skb->data - ETH_HLEN);
//MTK //#endif
#if defined (CONFIG_RAETH_QDMA)
	struct vlan_ethhdr *veth;
#endif
#endif
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	unsigned int addr = 0;
	unsigned int src_ip = 0;
#endif
	if (DebugLevel >= 7) {
		FoeDumpPkt(skb);
	}

/*FP to-lan on/off*/	
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	if(pptp_fast_path && (skb->len != (124 - 14))){
	    int ret = 1000;
		/*remove pptp/l2tp header, tx to PPE*/
		ret = HnatPptpLan(skb);
		/*ret 0, remove header ok*/
		if(ret == 0)
			return ret;
	}
	if(l2tp_fast_path && (skb->len != (124 - 14))){
	    int ret = 1000;
		/*remove pptp/l2tp header, tx to PPE*/
		ret = HnatL2tpLan(skb);
		if(ret == 0)
			return ret;
	}
#endif

#if defined (CONFIG_RAETH_QDMA)
	/* QDMA QoS remove CPU reason, we use special tag to identify force to CPU
	 * Notes: CPU reason & Entry ID fileds are invalid at this moment 
	 */
	if (FOE_SP(skb) == 5){ 
		veth = (struct vlan_ethhdr *)LAYER2_HEADER(skb);
		if (veth->h_vlan_proto == 0x5678){
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
			uint16_t VirIfIdx = 0;
			VirIfIdx = RemoveVlanTag(&skb);
			foe_entry = &PpeFoeBase[VirIfIdx];
#else
			return PpeHitBindForceToCpuHandler(skb, foe_entry);
#endif
		}
	}
#endif

	//the incoming packet is from PCI or WiFi interface
	if (((FOE_MAGIC_TAG(skb) == FOE_MAGIC_PCI)
	     || (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN))) {

		return PpeExtIfRxHandler(skb);

	} else if ((FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU)) {
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	    if(pptp_fast_path){
		/*to ppp0, add pptp/l2tp header and send out*/
		int ret = 1000;
		//printk("PPTP LAN->WAN  get bind packet!!\n");
		ret = HnatPptpWan(skb);
		return ret;
	    }
	    else if(l2tp_fast_path){
		/*to ppp0, add pptp/l2tp header and send out*/
		int ret = 1000;
		ret = HnatL2tpWan(skb);
		//printk("L2TP LAN->WAN fast send bind packet and return %d!!\n", ret);
		return ret;
	    }
	    else
#endif
		return PpeHitBindForceToCpuHandler(skb, foe_entry);

	/* handle the incoming packet which came back from PPE */
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	} else if ((IS_MAGIC_TAG_VALID(skb) && ((FOE_SP(skb) == 0)||(FOE_SP(skb) == 5))) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_UC_OLD_HDR) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_MC_NEW_HDR) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_DUP_OLD_HDR)) {
#else
	} else if ((FOE_SP(skb) == 6) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_UC_OLD_HDR) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_MC_NEW_HDR) && 
			(FOE_AI(skb) != HIT_BIND_KEEPALIVE_DUP_OLD_HDR)) {
#endif
#else
	} else if (FOE_SP(skb) == 0 && (FOE_AI(skb) != HIT_BIND_KEEPALIVE)) {
#endif
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	    /* PPTP/L2TP use this pingpong tech */
	    //printk("get fast_path ping pong skb_length is %d\n", skb->len);
	    if((pptp_fast_path || l2tp_fast_path) && ((skb->len == 110) || (skb->len == 98))){
		if(FOE_AI(skb) == UN_HIT){
		    dev_kfree_skb_any(skb);/*avoid memory leak*/
		    return 0;
		}
		/* wan->lan ping-pong, pass up to tx handler for binding */
		//printk("Parse ping pong packets FOE_AI(skb)= 0x%2x!!\n", FOE_AI(skb));
		/* get start addr for each layer */
		if (PptpToLanParseLayerInfo(skb)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
		        dev_kfree_skb_any(skb);
			return 0;
		}
		if (DebugLevel >= 1) {
			printk("PpeFillInL2Info \n");
		}
		/* Set Layer2 Info */
		if (PpeFillInL2Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			dev_kfree_skb_any(skb);
			return 0;
		}

		if (DebugLevel >= 1) {
		    printk("PpeFillInL3Info \n");
		}
		/* Set Layer3 Info */
		if (PpeFillInL3Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			dev_kfree_skb_any(skb);
			return 0;
		}

		if (DebugLevel >= 1) {
			printk("PpeFillInL4Info \n");
		}
		/* Set Layer4 Info */
		if (PpeFillInL4Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			dev_kfree_skb_any(skb);
			return 0;
		}

		/* Set force port info to 1  */
		//if (PpeSetForcePortInfo(skb, foe_entry, gmac_no)) {
		if (PpeSetForcePortInfo(skb, foe_entry, 1)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			dev_kfree_skb_any(skb);
			return 0;
		}
#if defined (CONFIG_RALINK_MT7621)
		foe_entry->ipv4_hnapt.iblk2.fqos = 0; /*not goes to QoS*/
#endif
		/* Enter binding state */
		PpeSetEntryBind(skb, foe_entry);
		fast_bind = 1;
		addr = ((htons(foe_entry->ipv4_hnapt.sport) << 16)|htons(foe_entry->ipv4_hnapt.dport));
		src_ip = (htonl(foe_entry->ipv4_hnapt.sip));
		pptp_l2tp_fdb_update(PpeParseResult.iph.protocol, addr, src_ip,  FOE_ENTRY_NUM(skb));
		/*free pingpong packet*/		
		dev_kfree_skb_any(skb);
		return 0;
	    }
	    else
#endif
		return PpeExtIfPingPongHandler(skb);
#if defined (CONFIG_HNAT_V2)
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_UC_OLD_HDR) {
		if (DebugLevel >= 3) {
			printk("Got HIT_BIND_KEEPALIVE_UC_OLD_HDR packet (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	    if(pptp_fast_path){
		dev_kfree_skb_any(skb);
		return 0;
	    }else
#endif
		return 1;
	} else if (FOE_AI(skb) == HIT_BIND_MULTICAST_TO_CPU ||
		   FOE_AI(skb) == HIT_BIND_MULTICAST_TO_GMAC_CPU) {

		return PpeHitBindForceMcastToWiFiHandler(skb);
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR) {
		if (DebugLevel >= 3) {
			printk("Got HIT_BIND_KEEPALIVE_MC_NEW_HDR packet (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
		if (PpeKeepAliveHandler(skb, foe_entry)) {
			return 1;
		}
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR) {
		if (DebugLevel >= 3) {
			printk("Rx : Got HIT_BIND_KEEPALIVE_DUP_OLD_HDR packe (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
//MTK //#if defined (CONFIG_RALINK_MT7620)
		eth->h_source[0] = 0x1; //change to multicast packet, make bridge not learn this packet
//MTK //#endif
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	    if(pptp_fast_path){
		dev_kfree_skb_any(skb);
		return 0;
	    }else
#endif
		return 1;
#else
	} else if ((FOE_AI(skb) == HIT_BIND_KEEPALIVE) && (DFL_FOE_KA == 0)) {
		if (PpeKeepAliveHandler(skb, foe_entry)) {
			return 1;
		}
#endif
	}
#if defined (CONFIG_RA_HW_NAT_ACL2UP_HELPER)
	else if ((FOE_AI(skb) == HIT_UNBIND_RATE_REACH)) {
		FOE_SP(skb) = PpeGetUpFromACLRule(skb);
	}
#endif
#if defined (CONFIG_RA_HW_NAT_IP_PASSTHROUGH)
	if(IsForward(skb)){
		//forward to wan
		struct net_device *wan=ra_dev_get_by_name("wwan0");
		if(wan) {
			skb->dev=wan;
			LAYER3_HEADER(skb) = skb->data;
			skb_push(skb, ETH_HLEN);        //pointer to layer2 header
			dev_queue_xmit(skb);
			return 0;
		}
	}
#endif
	return 1;
}

int32_t
GetPppoeSid(struct sk_buff * skb, uint32_t vlan_gap,
	    uint16_t * sid, uint16_t * ppp_tag)
{
	struct pppoe_hdr *peh = NULL;

	peh = (struct pppoe_hdr *)(skb->data + ETH_HLEN + vlan_gap);

	if (DebugLevel >= 6) {
		NAT_PRINT("\n==============\n");
		NAT_PRINT(" Ver=%d\n", peh->ver);
		NAT_PRINT(" Type=%d\n", peh->type);
		NAT_PRINT(" Code=%d\n", peh->code);
		NAT_PRINT(" sid=%x\n", ntohs(peh->sid));
		NAT_PRINT(" Len=%d\n", ntohs(peh->length));
		NAT_PRINT(" tag_type=%x\n", ntohs(peh->tag[0].tag_type));
		NAT_PRINT(" tag_len=%d\n", ntohs(peh->tag[0].tag_len));
		NAT_PRINT("=================\n");
	}

	*ppp_tag = peh->tag[0].tag_type;
#if defined (CONFIG_RA_HW_NAT_IPV6)
	if (peh->ver != 1 || peh->type != 1
	    || (*ppp_tag != htons(PPP_IP) && *ppp_tag != htons(PPP_IPV6))) {
#else
	if (peh->ver != 1 || peh->type != 1 || *ppp_tag != htons(PPP_IP)) {
#endif
		return 1;
	}

	*sid = peh->sid;
	return 0;
}

/* HNAT_V2 can push special tag */
int32_t isSpecialTag(uint16_t eth_type)
{
	/* Please modify this function to speed up the packet with special tag
	 * Ex: 
	 *    Ralink switch = 0x81xx
	 *    Realtek switch = 0x8899 
	 */
#if defined (CONFIG_HNAT_V2)
	if ((eth_type && 0x00FF) == htons(ETH_P_8021Q)) { //Ralink Special Tag: 0x81xx
		PpeParseResult.vlan_tag = eth_type;
		return 1;
	} else {
		return 0;
	}
#else
	return 0;
#endif
}

int32_t is8021Q(uint16_t eth_type)
{
	if (eth_type == htons(ETH_P_8021Q)) {
		PpeParseResult.vlan_tag = eth_type;
		return 1;
	} else {
		return 0;
	}
}

int32_t isHwVlanTx(struct sk_buff *skb)
{
#ifdef CONFIG_RAETH_HW_VLAN_TX
	if(vlan_tx_tag_present(skb)) {
		PpeParseResult.vlan_tag = htons(ETH_P_8021Q);
		return 1;
	} else {
		return 0;
	}
#else
	return 0;
#endif
}

int32_t PpeParseLayerInfo(struct sk_buff * skb)
{

	struct vlan_hdr *vh = NULL;
	struct ethhdr *eth = NULL;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;
#ifdef CONFIG_RAETH_HW_VLAN_TX
	struct vlan_hdr pseudo_vhdr;
#endif
	
	memset(&PpeParseResult, 0, sizeof(PpeParseResult));

	eth = (struct ethhdr *)skb->data;
	memcpy(PpeParseResult.dmac, eth->h_dest, ETH_ALEN);
	memcpy(PpeParseResult.smac, eth->h_source, ETH_ALEN);
	PpeParseResult.eth_type = eth->h_proto;

	// we cannot speed up multicase packets because both wire and wireless PCs might join same multicast group.
#if (defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)) && defined  (CONFIG_PPE_MCAST)
	if(is_multicast_ether_addr(&eth->h_dest[0])) {
		PpeParseResult.is_mcast = 1;
	}else {
		PpeParseResult.is_mcast = 0;
	}
#else
	if(is_multicast_ether_addr(&eth->h_dest[0])) {
		return 1;
	}
#endif

	if (is8021Q(PpeParseResult.eth_type) || isSpecialTag(PpeParseResult.eth_type) || isHwVlanTx(skb)) {

#ifdef CONFIG_RAETH_HW_VLAN_TX
		PpeParseResult.vlan1_gap = 0;
		PpeParseResult.vlan_layer++;
		pseudo_vhdr.h_vlan_TCI = htons(vlan_tx_tag_get(skb));
		pseudo_vhdr.h_vlan_encapsulated_proto = eth->h_proto;
		vh = (struct vlan_hdr *)&pseudo_vhdr;
#else
		PpeParseResult.vlan1_gap = VLAN_HLEN;
		PpeParseResult.vlan_layer++;
		vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
#endif
		PpeParseResult.vlan1 = vh->h_vlan_TCI;

		/* VLAN + PPPoE */
		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			PpeParseResult.pppoe_gap = 8;
			if (GetPppoeSid(skb, PpeParseResult.vlan1_gap,
					&PpeParseResult.pppoe_sid,
					&PpeParseResult.ppp_tag)) {
				return 1;
			}
			PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;
			/* Double VLAN = VLAN + VLAN */
		} else if ( is8021Q(vh->h_vlan_encapsulated_proto) || 
			isSpecialTag(vh->h_vlan_encapsulated_proto)) {
			PpeParseResult.vlan2_gap = VLAN_HLEN;
			PpeParseResult.vlan_layer++;
			vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + PpeParseResult.vlan1_gap);
			PpeParseResult.vlan2 = vh->h_vlan_TCI;

			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
				PpeParseResult.pppoe_gap = 8;
				if (GetPppoeSid
				    (skb,
				     (PpeParseResult.vlan1_gap + PpeParseResult.vlan2_gap),
				     &PpeParseResult.pppoe_sid, &PpeParseResult.ppp_tag)) {
					return 1;
				}
				PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;
#if defined (CONFIG_HNAT_V2)
			} else if (is8021Q(vh->h_vlan_encapsulated_proto)) {
				/* VLAN + VLAN + VLAN */
				PpeParseResult.vlan_layer++;
				vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + PpeParseResult.vlan1_gap + VLAN_HLEN);

				/* VLAN + VLAN + VLAN */
				if (is8021Q(vh->h_vlan_encapsulated_proto)) {
					PpeParseResult.vlan_layer++;
				}
#endif
			} else {
				/* VLAN + VLAN + IP */
				PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;
			}
		} else {
			/* VLAN + IP */
			PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;
		}
	} else if (ntohs(PpeParseResult.eth_type) == ETH_P_PPP_SES) {
		/* PPPoE + IP */
		PpeParseResult.pppoe_gap = 8;
		if (GetPppoeSid(skb, PpeParseResult.vlan1_gap,
					&PpeParseResult.pppoe_sid,
					&PpeParseResult.ppp_tag)) {
			return 1;
		}
	}

	/* set layer2 start addr */
	LAYER2_HEADER(skb) = skb->data;

	/* set layer3 start addr */
	LAYER3_HEADER(skb) =
	    (skb->data + ETH_HLEN + PpeParseResult.vlan1_gap +
	     PpeParseResult.vlan2_gap + PpeParseResult.pppoe_gap);

	/* set layer4 start addr */
	if ((PpeParseResult.eth_type == htons(ETH_P_IP)) || (PpeParseResult.eth_type == htons(ETH_P_PPP_SES)
		&& PpeParseResult.ppp_tag == htons(PPP_IP))) {
		iph = (struct iphdr *)LAYER3_HEADER(skb);

		//prepare layer3/layer4 info
		memcpy(&PpeParseResult.iph, iph, sizeof(struct iphdr));
		if (iph->protocol == IPPROTO_TCP) {
			LAYER4_HEADER(skb) = ((uint8_t *) iph + (iph->ihl * 4));
			th = (struct tcphdr *)LAYER4_HEADER(skb);
			memcpy(&PpeParseResult.th, th, sizeof(struct tcphdr));
			PpeParseResult.pkt_type = IPV4_HNAPT;

			if(iph->frag_off & htons(IP_MF|IP_OFFSET)) {
				return 1;
			}
		} else if (iph->protocol == IPPROTO_UDP) {
			LAYER4_HEADER(skb) = ((uint8_t *) iph + iph->ihl * 4);
			uh = (struct udphdr *)LAYER4_HEADER(skb);
			memcpy(&PpeParseResult.uh, uh, sizeof(struct udphdr));
			PpeParseResult.pkt_type = IPV4_HNAPT;
			
			if(iph->frag_off & htons(IP_MF|IP_OFFSET)) {
				return 1;
			}
		}
#if defined (CONFIG_HNAT_V2)
		else if (iph->protocol == IPPROTO_GRE) {
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
#else
			/* do nothing */
			return 1;
#endif
		}
#if defined (CONFIG_RA_HW_NAT_IPV6)
		else if (iph->protocol == IPPROTO_IPV6) {
			ip6h = (struct ipv6hdr *)((uint8_t *) iph + iph->ihl * 4);
			memcpy(&PpeParseResult.ip6h, ip6h, sizeof(struct ipv6hdr));

			if (ip6h->nexthdr == NEXTHDR_TCP) {
				LAYER4_HEADER(skb) = ((uint8_t *) ip6h + sizeof(struct ipv6hdr));
				th = (struct tcphdr *)LAYER4_HEADER(skb);
				memcpy(&PpeParseResult.th.source, &th->source, sizeof(th->source));
				memcpy(&PpeParseResult.th.dest, &th->dest, sizeof(th->dest));
			} else if (ip6h->nexthdr == NEXTHDR_UDP) {
				LAYER4_HEADER(skb) = ((uint8_t *) ip6h + sizeof(struct ipv6hdr));
				uh = (struct udphdr *)LAYER4_HEADER(skb);
				memcpy(&PpeParseResult.uh.source, &uh->source, sizeof(uh->source));
				memcpy(&PpeParseResult.uh.dest, &uh->dest, sizeof(uh->dest));
			}
			PpeParseResult.pkt_type = IPV6_6RD;
#if defined (CONFIG_RALINK_MT7620) || defined (CONFIG_RALINK_MT7621)
			/* identification field in outer ipv4 header is zero after erntering binding state.
			 * some 6rd relay router will drop the packet */
			return 1;
#endif
		}
#endif
#endif
		else {
			/* Packet format is not supported */
			return 1;
		}

	} else if (PpeParseResult.eth_type == htons(ETH_P_IPV6) || 
			(PpeParseResult.eth_type == htons(ETH_P_PPP_SES) &&
		        PpeParseResult.ppp_tag == htons(PPP_IPV6))) {
		ip6h = (struct ipv6hdr *)LAYER3_HEADER(skb);
		memcpy(&PpeParseResult.ip6h, ip6h, sizeof(struct ipv6hdr));

		if (ip6h->nexthdr == NEXTHDR_TCP) {
			LAYER4_HEADER(skb) = ((uint8_t *) ip6h + sizeof(struct ipv6hdr));
			th = (struct tcphdr *)LAYER4_HEADER(skb);
			memcpy(&PpeParseResult.th, th, sizeof(struct tcphdr));
			PpeParseResult.pkt_type = IPV6_5T_ROUTE;
		} else if (ip6h->nexthdr == NEXTHDR_UDP) {
			LAYER4_HEADER(skb) = ((uint8_t *) ip6h + sizeof(struct ipv6hdr));
			uh = (struct udphdr *)LAYER4_HEADER(skb);
			memcpy(&PpeParseResult.uh, uh, sizeof(struct udphdr));
			PpeParseResult.pkt_type = IPV6_5T_ROUTE;
		}
		else if (ip6h->nexthdr == NEXTHDR_IPIP) {
			memcpy(&PpeParseResult.iph, ip6h + sizeof(struct ipv6hdr),
			       sizeof(struct iphdr));
			PpeParseResult.pkt_type = IPV4_DSLITE;
		} else {
#if defined (CONFIG_HNAT_V2)
			PpeParseResult.pkt_type = IPV6_3T_ROUTE;
#else
			PpeParseResult.pkt_type = IPV6_1T_ROUTE;
#endif
		}

	} else {
		return 1;
	}

	if (DebugLevel >= 6) {
		printk("--------------\n");
		printk("DMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
		       PpeParseResult.dmac[0], PpeParseResult.dmac[1],
		       PpeParseResult.dmac[2], PpeParseResult.dmac[3],
		       PpeParseResult.dmac[4], PpeParseResult.dmac[5]);
		printk("SMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
		       PpeParseResult.smac[0], PpeParseResult.smac[1],
		       PpeParseResult.smac[2], PpeParseResult.smac[3],
		       PpeParseResult.smac[4], PpeParseResult.smac[5]);
		printk("Eth_Type=%x\n", PpeParseResult.eth_type);
		if (PpeParseResult.vlan1_gap > 0) {
			printk("VLAN1 ID=%x\n", ntohs(PpeParseResult.vlan1));
		}

		if (PpeParseResult.vlan2_gap > 0) {
			printk("VLAN2 ID=%x\n", ntohs(PpeParseResult.vlan2));
		}

		if (PpeParseResult.pppoe_gap > 0) {
			printk("PPPOE Session ID=%x\n",
			       PpeParseResult.pppoe_sid);
			printk("PPP Tag=%x\n", ntohs(PpeParseResult.ppp_tag));
		}
#if defined (CONFIG_HNAT_V2)
		printk("PKT_TYPE=%s\n",
		       PpeParseResult.pkt_type ==
		       0 ? "IPV4_HNAT" : PpeParseResult.pkt_type ==
		       1 ? "IPV4_HNAPT" : PpeParseResult.pkt_type ==
		       3 ? "IPV4_DSLITE" : PpeParseResult.pkt_type ==
		       4 ? "IPV6_ROUTE" : PpeParseResult.pkt_type ==
		       5 ? "IPV6_6RD" : "Unknown");
#else
		printk("PKT_TYPE=%s\n",
		       PpeParseResult.pkt_type ==
		       0 ? "IPV4_HNAPT" : PpeParseResult.pkt_type ==
		       1 ? "IPV4_HNAT" : PpeParseResult.pkt_type ==
		       2 ? "IPV6_ROUTE" : "Unknown");
#endif

		if (PpeParseResult.pkt_type == IPV4_HNAT) {
			printk("SIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph.tos));
		} else if (PpeParseResult.pkt_type == IPV4_HNAPT) {
			printk("SIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph.tos));
			
			if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
			    printk("TCP SPORT=%d\n", ntohs(PpeParseResult.th.source));
			    printk("TCP DPORT=%d\n", ntohs(PpeParseResult.th.dest));
			}else if(PpeParseResult.iph.protocol == IPPROTO_UDP) {
			    printk("UDP SPORT=%d\n", ntohs(PpeParseResult.uh.source));
			    printk("UDP DPORT=%d\n", ntohs(PpeParseResult.uh.dest));
			}
		}
#if defined (CONFIG_HNAT_V2)
		else if (PpeParseResult.pkt_type == IPV6_6RD) {
			/* fill in ipv4 6rd entry */
			printk("SIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
			       Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("Checksum=%x\n",
			       ntohs(PpeParseResult.iph.check));
			printk("Flag=%x\n", ntohs(PpeParseResult.iph.frag_off) >> 13);
			printk("TTL=%x\n", PpeParseResult.iph.ttl);
			printk("TOS=%x\n", PpeParseResult.iph.tos);
		}
#endif
	}

	return 0;
}

int32_t PpeFillInL2Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	//if this entry is already in binding state, skip it 
	if (foe_entry->bfib1.state == BIND) {
		return 1;
	}



	/* Set VLAN Info - VLAN1/VLAN2 */
#if defined (CONFIG_HNAT_V2)
	/* Set Layer2 Info - DMAC, SMAC */
	if ((PpeParseResult.pkt_type == IPV4_HNAT) || (PpeParseResult.pkt_type == IPV4_HNAPT)) {

		if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) { //DS-Lite WAN->LAN
#if defined (CONFIG_RA_HW_NAT_IPV6)
			FoeSetMacHiInfo(foe_entry->ipv4_dslite.dmac_hi, PpeParseResult.dmac);
			FoeSetMacLoInfo(foe_entry->ipv4_dslite.dmac_lo, PpeParseResult.dmac);
			FoeSetMacHiInfo(foe_entry->ipv4_dslite.smac_hi, PpeParseResult.smac);
			FoeSetMacLoInfo(foe_entry->ipv4_dslite.smac_lo, PpeParseResult.smac);
			foe_entry->ipv4_dslite.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_dslite.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_dslite.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_dslite.etype = ntohs(PpeParseResult.vlan_tag);
#else
			return 1;
#endif
		}else { //IPv4 WAN<->LAN
			FoeSetMacHiInfo(foe_entry->ipv4_hnapt.dmac_hi, PpeParseResult.dmac);
			FoeSetMacLoInfo(foe_entry->ipv4_hnapt.dmac_lo, PpeParseResult.dmac);
			FoeSetMacHiInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac);
			FoeSetMacLoInfo(foe_entry->ipv4_hnapt.smac_lo, PpeParseResult.smac);
			foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1);
#ifdef VPRI_REMARK_TEST
			//VPRI=0x7
			foe_entry->ipv4_hnapt.vlan1 |= (7 << 13);
#endif
			foe_entry->ipv4_hnapt.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_hnapt.etype = ntohs(PpeParseResult.vlan_tag);
		}
	} 
	else {
#if defined (CONFIG_RA_HW_NAT_IPV6)
		FoeSetMacHiInfo(foe_entry->ipv6_5t_route.dmac_hi, PpeParseResult.dmac);
		FoeSetMacLoInfo(foe_entry->ipv6_5t_route.dmac_lo, PpeParseResult.dmac);
		FoeSetMacHiInfo(foe_entry->ipv6_5t_route.smac_hi, PpeParseResult.smac);
		FoeSetMacLoInfo(foe_entry->ipv6_5t_route.smac_lo, PpeParseResult.smac);
		foe_entry->ipv6_5t_route.vlan1 = ntohs(PpeParseResult.vlan1);
		foe_entry->ipv6_5t_route.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
		foe_entry->ipv6_5t_route.vlan2 = ntohs(PpeParseResult.vlan2);
		foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.vlan_tag);
#else
		return 1;
#endif
	}

	/* 
	 * VLAN Layer:
	 * 0: outgoing packet is untagged packet
	 * 1: outgoing packet is tagged packet
	 * 2: outgoing packet is double tagged packet
	 * 3: outgoing packet is triple tagged packet
	 * 4: outgoing packet is fourfold tagged packet
	 */
	foe_entry->bfib1.vlan_layer = PpeParseResult.vlan_layer;

#ifdef VLAN_LAYER_TEST
	//outgoing packet is triple tagged packet
	foe_entry->bfib1.vlan_layer = 3;
	foe_entry->ipv4_hnapt.vlan1 = 2;
	foe_entry->ipv4_hnapt.vlan2 = 1;
#endif
	if (PpeParseResult.pppoe_gap) {
		foe_entry->bfib1.psn = 1;
	} else {
		foe_entry->bfib1.psn = 0;
	}
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	foe_entry->ipv4_hnapt.bfib1.vpm = 1; //0x8100
#else
#ifdef FORCE_UP_TEST	
	foe_entry->ipv4_hnapt.bfib1.dvp = 0; //let switch decide VPRI
#else
	/* we set VID and VPRI in foe entry already, so we have to inform switch of keeping VPRI */
	foe_entry->ipv4_hnapt.bfib1.dvp = 1;
#endif
#endif
#else
	FoeSetMacHiInfo(foe_entry->ipv4_hnapt.dmac_hi, PpeParseResult.dmac);
	FoeSetMacLoInfo(foe_entry->ipv4_hnapt.dmac_lo, PpeParseResult.dmac);
	FoeSetMacHiInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac);
	FoeSetMacLoInfo(foe_entry->ipv4_hnapt.smac_lo, PpeParseResult.smac);
	foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1);
	foe_entry->ipv4_hnapt.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
	foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2);
	
	/* 
	 * PPE support SMART VLAN/PPPoE Tag Push/PoP feature
	 *
	 *         | MODIFY | INSERT | DELETE
	 * --------+--------+--------+----------
	 * Tagged  | modify | modify | delete
	 * Untagged| no act | insert | no act
	 *
	 */

	if (PpeParseResult.vlan1_gap) {
		foe_entry->bfib1.v1 = INSERT;
	} else {
		foe_entry->bfib1.v1 = DELETE;
	}

	if (PpeParseResult.vlan2_gap) {
		foe_entry->bfib1.v2 = INSERT;
	} else {
		foe_entry->bfib1.v2 = DELETE;
	}

	if (PpeParseResult.pppoe_gap) {
		foe_entry->bfib1.pppoe = INSERT;
	} else {
		foe_entry->bfib1.pppoe = DELETE;
	}
#endif

	return 0;
}

#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
static uint16_t PpeGetChkBase(struct iphdr *iph)
{
	uint16_t org_chksum = ntohs(iph->check);
	uint16_t org_tot_len = ntohs(iph->tot_len);
	uint16_t org_id = ntohs(iph->id);
	uint16_t chksum_tmp, tot_len_tmp, id_tmp;
	uint32_t tmp = 0;
	uint16_t chksum_base = 0;

	chksum_tmp = ~(org_chksum);
	tot_len_tmp = ~(org_tot_len);
	id_tmp = ~(org_id);
	tmp = chksum_tmp + tot_len_tmp + id_tmp;
	tmp = ((tmp >> 16) & 0x7) + (tmp & 0xFFFF);
	tmp = ((tmp >> 16) & 0x7) + (tmp & 0xFFFF);
	chksum_base = tmp & 0xFFFF;

	return chksum_base;
}
#endif
#endif

int32_t PpeFillInL3Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	/* IPv4 or IPv4 over PPPoE */
	if ((PpeParseResult.eth_type == htons(ETH_P_IP)) || 
		(PpeParseResult.eth_type == htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag == htons(PPP_IP))) {

		if ((PpeParseResult.pkt_type == IPV4_HNAT) || (PpeParseResult.pkt_type == IPV4_HNAPT)) {

			if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) { //DS-Lite WAN->LAN
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
#if defined (CONFIG_RALINK_MT7620)
				foe_entry->ipv4_dslite.bfib1.drm = 1;	//switch will keep dscp
#endif
				foe_entry->ipv4_dslite.bfib1.rmt = 1; //remove outer IPv6 header
				foe_entry->ipv4_dslite.iblk2.dscp = PpeParseResult.iph.tos;
#endif
#endif

			} else {
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RALINK_MT7620)
				foe_entry->ipv4_hnapt.bfib1.drm = 1;	//switch will keep dscp
#endif
#endif
				foe_entry->ipv4_hnapt.new_sip = ntohl(PpeParseResult.iph.saddr);
				foe_entry->ipv4_hnapt.new_dip = ntohl(PpeParseResult.iph.daddr);
				foe_entry->ipv4_hnapt.iblk2.dscp = PpeParseResult.iph.tos;
#ifdef DSCP_REMARK_TEST
				foe_entry->ipv4_hnapt.iblk2.dscp = 0xff;
#endif
			}
		}
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
		else if (PpeParseResult.pkt_type == IPV6_6RD) {
			/* fill in ipv4 6rd entry */
			foe_entry->ipv6_6rd.tunnel_sipv4 = ntohl(PpeParseResult.iph.saddr);
			foe_entry->ipv6_6rd.tunnel_dipv4 = ntohl(PpeParseResult.iph.daddr);
			foe_entry->ipv6_6rd.hdr_chksum = PpeGetChkBase(&PpeParseResult.iph);
			foe_entry->ipv6_6rd.flag = (ntohs(PpeParseResult.iph.frag_off) >> 13);
			foe_entry->ipv6_6rd.ttl = PpeParseResult.iph.ttl;
			foe_entry->ipv6_6rd.dscp = PpeParseResult.iph.tos;

#if defined (CONFIG_ARCH_MT7623)
			RegModifyBits(PPE_HASH_SEED, ntohl(PpeParseResult.iph.id), 0, 16);
			foe_entry->ipv6_6rd.per_flow_6rd_id = 1;
#endif
			/* IPv4 DS-Lite and IPv6 6RD shall be turn on by SW during initialization */
			foe_entry->bfib1.pkt_type = IPV6_6RD;
		}
#endif
#endif
	}
#if defined (CONFIG_RA_HW_NAT_IPV6)
	/* IPv6 or IPv6 over PPPoE */
	else if (PpeParseResult.eth_type == htons(ETH_P_IPV6) || 
		(PpeParseResult.eth_type == htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag == htons(PPP_IPV6))) {
#if defined (CONFIG_HNAT_V2)
		if (PpeParseResult.pkt_type == IPV6_3T_ROUTE || PpeParseResult.pkt_type == IPV6_5T_ROUTE) {
			
			// incoming packet is 6RD and need to remove outer IPv4 header
			if(foe_entry->bfib1.pkt_type == IPV6_6RD) {
#if defined (CONFIG_RALINK_MT7620)
				foe_entry->ipv6_3t_route.bfib1.drm = 1;	//switch will keep dscp
#endif
				foe_entry->ipv6_3t_route.bfib1.rmt = 1;
				foe_entry->ipv6_3t_route.iblk2.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));
			} else {
				/* fill in ipv6 routing entry */
#if defined (CONFIG_RALINK_MT7620)
				foe_entry->ipv6_3t_route.bfib1.drm = 1;	//switch will keep dscp
#endif
				foe_entry->ipv6_3t_route.ipv6_sip0 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_sip1 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_sip2 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_sip3 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[3]);

				foe_entry->ipv6_3t_route.ipv6_dip0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_dip1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_dip2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_dip3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
				foe_entry->ipv6_3t_route.iblk2.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));
#ifdef DSCP_REMARK_TEST
				foe_entry->ipv6_3t_route.iblk2.dscp = 0xff;
#endif
			}

		} else if (PpeParseResult.pkt_type == IPV4_DSLITE) {
			/* fill in DSLite entry */
			foe_entry->ipv4_dslite.tunnel_sipv6_0 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[0]);
			foe_entry->ipv4_dslite.tunnel_sipv6_1 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[1]);
			foe_entry->ipv4_dslite.tunnel_sipv6_2 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[2]);
			foe_entry->ipv4_dslite.tunnel_sipv6_3 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[3]);

			foe_entry->ipv4_dslite.tunnel_dipv6_0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
			foe_entry->ipv4_dslite.tunnel_dipv6_1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
			foe_entry->ipv4_dslite.tunnel_dipv6_2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
			foe_entry->ipv4_dslite.tunnel_dipv6_3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);

			memcpy(foe_entry->ipv4_dslite.flow_lbl, PpeParseResult.ip6h.flow_lbl, sizeof(PpeParseResult.ip6h.flow_lbl));
			foe_entry->ipv4_dslite.priority = PpeParseResult.ip6h.priority;
			foe_entry->ipv4_dslite.hop_limit = PpeParseResult.ip6h.hop_limit;
			/* IPv4 DS-Lite and IPv6 6RD shall be turn on by SW during initialization */
			foe_entry->bfib1.pkt_type = IPV4_DSLITE;
		}
#else
		/* Nothing to do */
		;
#endif
	}
#endif // CONFIG_RA_HW_NAT_IPV6 //
	else {
		return 1;
	}

	return 0;
}

int32_t PpeFillInL4Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{

#if defined (CONFIG_RALINK_RT3052)
	uint32_t phy_val;
#endif

	if (PpeParseResult.pkt_type == IPV4_HNAPT) {
#if defined (CONFIG_HNAT_V2)
		// DS-LIte WAN->LAN
		if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) {
			return 0;
		}
#endif

		/* Set Layer4 Info - NEW_SPORT, NEW_DPORT */
		if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.th.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.th.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = TCP;
		} else if (PpeParseResult.iph.protocol == IPPROTO_UDP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.uh.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.uh.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = UDP;
#if defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7621) ||  defined (CONFIG_ARCH_MT7623)
			/* do nothing */
#elif defined (CONFIG_RALINK_RT3352)
			if (RegRead(0xB000000C) <= 0x0104) {
			//	if(foe_entry->ipv4_hnapt.new_sport==4500 && foe_entry->ipv4_hnapt.new_dport==4500)  
					return 1;
			}
#elif defined (CONFIG_RALINK_RT3052)
			rw_rf_reg(0, 0, &phy_val);
			phy_val = phy_val & 0xFF;

			if (phy_val <= 0x53) {
			//	if(foe_entry->ipv4_hnapt.new_sport==4500 && foe_entry->ipv4_hnapt.new_dport==4500)  
					return 1;
			}
#elif defined (CONFIG_RALINK_MT7620)
			if ((RegRead(0xB000000C) & 0xf)< 0x5) {
					return 1;
			}
#else
			/* if UDP checksum is zero, it cannot be accelerated by HNAT */
			/* we found some protocols, such as IPSEC-NAT-T, are possible to hybrid 
			 * udp zero checksum and non-zero checksum in the same session, 
			 * so we disable HNAT acceleration for all UDP flows 
			 */
			//if(foe_entry->ipv4_hnapt.new_sport==4500 && foe_entry->ipv4_hnapt.new_dport==4500)  
				return 1;
#endif
		}
	} else if (PpeParseResult.pkt_type == IPV4_HNAT) {
		/* do nothing */
	} else if (PpeParseResult.pkt_type == IPV6_1T_ROUTE) {
		/* do nothing */
#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
	} else if (PpeParseResult.pkt_type == IPV6_3T_ROUTE) {
		/* do nothing */
	} else if (PpeParseResult.pkt_type == IPV6_5T_ROUTE) {
		/* do nothing */
#endif
#endif
	}

	return 0;
}

#if !defined(CONFIG_HNAT_V2)
int32_t PpeSetAGInfo(uint16_t index, uint16_t vlan_id)
{
	struct l2_rule L2Rule;
	uint32_t *p = (uint32_t *) & L2Rule;

	memset(&L2Rule, 0, sizeof(L2Rule));

	L2Rule.others.vid = vlan_id;
	L2Rule.others.v = 1;

	L2Rule.com.rt = L2_RULE;
	L2Rule.com.pn = PN_DONT_CARE;
	L2Rule.com.match = 1;

	L2Rule.com.ac.ee = 1;
	L2Rule.com.ac.ag = index;

	L2Rule.com.dir = OTHERS;
	RegWrite(POLICY_TBL_BASE + index * 8, *p);	/* Low bytes */
	RegWrite(POLICY_TBL_BASE + index * 8 + 4, *(p + 1));	/* High bytes */

	return 1;
}
#else

/* token_rate: unit= KB/S */
int32_t PpeSetMtrByteInfo(uint16_t MgrIdx, uint32_t TokenRate, uint32_t MaxBkSize)
{
	uint32_t MtrEntry = 0;

        MtrEntry = ((TokenRate << 3) | (MaxBkSize << 1));
        
#if defined (CONFIG_RALINK_MT7620)
	RegWrite(METER_BASE + MgrIdx * 4, MtrEntry);

	printk("Meter Table Base=%08X Offset=%d\n", METER_BASE, MgrIdx * 4);
        printk("%08X: %08X\n", METER_BASE + MgrIdx * 4, MtrEntry);
#elif defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	RegWrite(METER_BASE + MgrIdx * 16 + 12, MtrEntry);
	printk("Meter Table Base=%08X Offset=%d\n", METER_BASE, MgrIdx * 16 + 12);
        printk("%08X: %08X\n", METER_BASE + MgrIdx * 12, MtrEntry);
#endif
	
	return 1;
}

/*
 * MtrIntval:
 * 0x0 = 1
 * 0x1 = 10
 * 0x2 = 50
 * 0x3 = 100
 * 0x4 = 500
 * 0x5 = 1000
 * 0x6 = 5000
 * 0x7 = 10000
 */
int32_t PpeSetMtrPktInfo(uint16_t MgrIdx, uint32_t MtrIntval, uint32_t MaxBkSize)
{
	uint32_t MtrEntry = 0;

	MtrEntry = ((MtrIntval << 8) | (MaxBkSize << 1) | 1);

        RegWrite(METER_BASE + MgrIdx * 4, MtrEntry);

	printk("Meter Table Base=%08X Offset=%d\n", METER_BASE, MgrIdx * 4);
        printk("%08X: %08X\n", METER_BASE + MgrIdx * 4, MtrEntry);

	return 1;
}

static void PpeSetInfoBlk2(struct _info_blk2 *iblk2, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag)
{
#ifdef FORCE_UP_TEST	
	uint32_t reg;
	
	iblk2->fp = 1;
	iblk2->up = 7;

	//Replace 802.1Q priority by user priority
	reg = RegRead(RALINK_ETH_SW_BASE + 0x2704);
	reg |= (0x1 << 11);
	RegWrite(RALINK_ETH_SW_BASE + 0x2704, reg);
#endif

#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	/* we need to lookup another multicast table if this is multicast flow */
	if(PpeParseResult.is_mcast) {
		iblk2->mcast = 1;
	} else {
		iblk2->mcast = 0;
	}

	// 0:PSE,1:GSW, 2:GMAC,4:PPE,5:QDMA,7=DROP */
	iblk2->dp = fpidx;
#elif defined (CONFIG_RALINK_MT7620)
	iblk2->fpidx = fpidx;
#endif

#if !defined (CONFIG_RAETH_QDMA) && (defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623))
	iblk2->fqos = 0; /* PDMA MODE should not goes to QoS*/
#endif

	iblk2->port_mg = port_mg;
	iblk2->port_ag = port_ag;
}
#endif

/*for 16 queue test*/
unsigned char queue_number = 0;
/* Set force port info */
int32_t
PpeSetForcePortInfo(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, int gmac_no)
{
#if !defined(CONFIG_HNAT_V2)
	foe_entry->ipv4_hnapt.iblk2.fd = 1;
#endif

#if defined (CONFIG_RAETH_QDMA)
 	if (IS_IPV4_GRP(foe_entry)) {
#if defined (CONFIG_IMQ)
		foe_entry->ipv4_hnapt.iblk2.qid = M2Q_table[skb->mark];
#ifdef CONFIG_PSEUDO_SUPPORT		
		if(lan_wan_separate==1 && gmac_no==2){ 
			foe_entry->ipv4_hnapt.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)			
			if(web_sfq_enable ==1 &&(skb->mark == 2)){ 
				foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQUp;
			}
#endif				
		} 
		if((lan_wan_separate==1) && (gmac_no==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQDl;	
			}
#endif
		}
#endif //end CONFIG_PSEUDO_SUPPORT

#ifdef CONFIG_RAETH_HW_VLAN_TX
	if(lan_wan_separate==1 && vlan_tx_tag_get(skb)==2){ 
			foe_entry->ipv4_hnapt.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)			
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQUp;
			}
#endif				
		} 
		if((lan_wan_separate==1) && (vlan_tx_tag_get(skb)==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQDl;	
			}
#endif
		}
#endif// end CONFIG_RAETH_HW_VLAN_TX

#ifndef CONFIG_PSEUDO_SUPPORT
#ifndef CONFIG_RAETH_HW_VLAN_TX 
	if(lan_wan_separate==1){
		struct vlan_hdr *vh = NULL;
    unsigned short vlanid = 0;
    unsigned short vlan_TCI;
		vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
		vlan_TCI = vh->h_vlan_TCI;
    vlanid = (vlan_TCI & VLAN_VID_MASK)>>8;
		if(vlanid == 2)//to wan
		{
			foe_entry->ipv4_hnapt.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)			
				if(web_sfq_enable==1 &&(skb->mark == 2)){ 
					foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQUp;
				}
#endif						
		}else if(vlanid == 1){ // to lan
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv4_hnapt.iblk2.qid = HwSfqQDl;	
			}
#endif
		}
	}
#endif //CONFIG_PSEUDO_SUPPORT
#endif //CONFIG_RAETH_HW_VLAN_TX 
#else
		struct iphdr *iph = (struct iphdr *)skb_network_header(skb);
		if (iph->tos == 0x00) foe_entry->ipv4_hnapt.iblk2.qid = 0x00;
		else if (iph->tos == 0x10) foe_entry->ipv4_hnapt.iblk2.qid = 0x01;
		else if (iph->tos == 0x20) foe_entry->ipv4_hnapt.iblk2.qid = 0x02;
		else if (iph->tos == 0x30) foe_entry->ipv4_hnapt.iblk2.qid = 0x03;
		else if (iph->tos == 0x40) foe_entry->ipv4_hnapt.iblk2.qid = 0x04;
		else if (iph->tos == 0x50) foe_entry->ipv4_hnapt.iblk2.qid = 0x05;
		else if (iph->tos == 0x60) foe_entry->ipv4_hnapt.iblk2.qid = 0x06;
		else if (iph->tos == 0x70) foe_entry->ipv4_hnapt.iblk2.qid = 0x07;
		else if (iph->tos == 0x80) foe_entry->ipv4_hnapt.iblk2.qid = 0x08;
		else if (iph->tos == 0x90) foe_entry->ipv4_hnapt.iblk2.qid = 0x09;
		else if (iph->tos == 0xa0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0a;
		else if (iph->tos == 0xb0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0b;
		else if (iph->tos == 0xc0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0c;
		else if (iph->tos == 0xd0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0d;
		else if (iph->tos == 0xe0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0e;
		else if (iph->tos == 0xf0) foe_entry->ipv4_hnapt.iblk2.qid = 0x0f;
#endif
#if 0		/*16 queue - round-robin*/
		foe_entry->ipv4_hnapt.iblk2.qid = queue_number = (queue_number++) % 16;
#endif
	}
#if defined (CONFIG_RA_HW_NAT_IPV6)
	else if (IS_IPV6_GRP(foe_entry)) {
#if defined (CONFIG_IMQ)
#ifdef CONFIG_PSEUDO_SUPPORT
		foe_entry->ipv6_3t_route.iblk2.qid = M2Q_table[skb->mark];
		if(lan_wan_separate==1 && gmac_no==2){ 
			foe_entry->ipv6_3t_route.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQUp;	
			}
#endif
		} 
		if((lan_wan_separate==1) && (gmac_no==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQDl;	
			}
#endif
		}
#endif		
		
		
#ifdef CONFIG_RAETH_HW_VLAN_TX
	if(lan_wan_separate==1 && vlan_tx_tag_get(skb)==2){ 
			foe_entry->ipv6_3t_route.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQUp;	
			}
#endif			
		} 
		if((lan_wan_separate==1) && (vlan_tx_tag_get(skb)==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQDl;	
			}
#endif
		}
#endif// end CONFIG_RAETH_HW_VLAN_TX

#ifndef CONFIG_PSEUDO_SUPPORT
#ifndef CONFIG_RAETH_HW_VLAN_TX
	if(lan_wan_separate==1){ 
		char *netdevWan, *netdevLan;
		netdevWan = ra_dev_get_by_name("eth2.2");
		netdevLan = ra_dev_get_by_name("eth2.1");
		if(!strcmp(netdevWan, "eth2.2"))//eth2.2 to wan
		{
			
				foe_entry->ipv6_3t_route.iblk2.qid +=8;
#if defined (CONFIG_HW_SFQ)			
				if(web_sfq_enable==1 &&(skb->mark == 2)){ 
					foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQUp;	
				}
#endif			
			
		}else if(!strcmp(netdevLan, "eth2.1")){ // eth2.1 to lan
#if defined (CONFIG_HW_SFQ)
				if(web_sfq_enable==1 &&(skb->mark == 2)){ 
					foe_entry->ipv6_3t_route.iblk2.qid = HwSfqQDl;	
				}
			
#endif
		}
	}
#endif //CONFIG_PSEUDO_SUPPORT
#endif //CONFIG_RAETH_HW_VLAN_TX 		
#else
		struct ipv6hdr *ip6h = (struct ipv6hdr *)skb_network_header(skb);
		char traffic_class = (ip6h->priority << 4) | (ip6h->flow_lbl[0]>>4);
		if (traffic_class == 0x00) foe_entry->ipv6_3t_route.iblk2.qid = 0x00;
		else if (traffic_class == 0x10) foe_entry->ipv6_3t_route.iblk2.qid = 0x01;
		else if (traffic_class == 0x20) foe_entry->ipv6_3t_route.iblk2.qid = 0x02;
		else if (traffic_class == 0x30) foe_entry->ipv6_3t_route.iblk2.qid = 0x03;
		else if (traffic_class == 0x40) foe_entry->ipv6_3t_route.iblk2.qid = 0x04;
		else if (traffic_class == 0x50) foe_entry->ipv6_3t_route.iblk2.qid = 0x05;
		else if (traffic_class == 0x60) foe_entry->ipv6_3t_route.iblk2.qid = 0x06;
		else if (traffic_class == 0x70) foe_entry->ipv6_3t_route.iblk2.qid = 0x07;
		else if (traffic_class == 0x80) foe_entry->ipv6_3t_route.iblk2.qid = 0x08;
		else if (traffic_class == 0x90) foe_entry->ipv6_3t_route.iblk2.qid = 0x09;
		else if (traffic_class == 0xa0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0a;
		else if (traffic_class == 0xb0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0b;
		else if (traffic_class == 0xc0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0c;
		else if (traffic_class == 0xd0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0d;
		else if (traffic_class == 0xe0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0e;
		else if (traffic_class == 0xf0) foe_entry->ipv6_3t_route.iblk2.qid = 0x0f;
#endif
#if 0		/*16 queue - round-robin*/
		foe_entry->ipv6_3t_route.iblk2.qid = queue_number = (queue_number++) % 16;
#endif
	}
#endif // CONFIG_RA_HW_NAT_IPV6 //
#endif // CONFIG_RAETH_QDMA //

	/* CPU need to handle traffic between WLAN/PCI and GMAC port */
	if ((strncmp(skb->dev->name, "ra", 2) == 0) ||
	    (strncmp(skb->dev->name, "wds", 3) == 0) ||
	    (strncmp(skb->dev->name, "mesh", 4) == 0) ||
	    (strncmp(skb->dev->name, "apcli", 5) == 0) ||
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	    (strncmp(skb->dev->name, "pci", 3) == 0) ||
	    (strncmp(skb->dev->name, "wwan", 4) == 0) ||
#else
	    (skb->dev == DstPort[DP_PCI]) || 
	    (skb->dev == DstPort[DP_USB]) ||
#endif
#endif
	    (gmac_no == 777)) {
#if defined  (CONFIG_RA_HW_NAT_WIFI) || defined (CONFIG_RA_HW_NAT_NIC_USB) || defined (CONFIG_RA_HW_NAT_PPTP_L2TP)
/*PPTP/L2TP LAN->WAN  bind to CPU*/
#if defined(CONFIG_RALINK_MT7620)
 	if (IS_IPV4_GRP(foe_entry)) {
		PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 6, 0x3F, 0x3F); //force to cpu
	}
#if defined (CONFIG_RA_HW_NAT_IPV6)
	else if (IS_IPV6_GRP(foe_entry)) {
		PpeSetInfoBlk2(&foe_entry->ipv6_3t_route.iblk2, 6, 0x3F, 0x3F); //force to cpu
	}
#endif

#elif defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
 	if (IS_IPV4_GRP(foe_entry)) {
#if defined (CONFIG_RAETH_QDMA)
		foe_entry->ipv4_hnapt.bfib1.vpm = 0; //etype remark
		foe_entry->ipv4_hnapt.iblk2.fqos = 1;
#if defined (CONFIG_RAETH_QDMATX_QDMARX)	
		PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 5, 0x3F, 0x3F); //5=QDMA
#else
		PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 0, 0x3F, 0x3F); //0=PDMA
#endif
		if (FOE_SP(skb) == 5){ // wifi to wifi not go to pse port6
			foe_entry->ipv4_hnapt.iblk2.fqos = 0;
		}else {
			foe_entry->ipv4_hnapt.iblk2.fqos = 1;
		}

		if(PpeParseResult.vlan1 == 0)
		{
			foe_entry->ipv4_hnapt.vlan1 = FOE_ENTRY_NUM(skb);
			foe_entry->ipv4_hnapt.etype = ntohs(0x5678);
			foe_entry->bfib1.vlan_layer = 1;
		}
		else if (PpeParseResult.vlan2 == 0)
		{
			foe_entry->ipv4_hnapt.vlan1 = FOE_ENTRY_NUM(skb);
			foe_entry->ipv4_hnapt.etype = ntohs(0x5678);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan1);
			foe_entry->bfib1.vlan_layer = 2;
		}
		else
		{
			foe_entry->ipv4_hnapt.vlan1 = FOE_ENTRY_NUM(skb);
			foe_entry->ipv4_hnapt.etype = ntohs(0x5678);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan1);
			foe_entry->bfib1.vlan_layer = 3;

		}
	
#else
		PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 0, 0x3F, 0x3F); //0=CPU
		foe_entry->ipv4_hnapt.iblk2.fqos = 0; /* PDMA MODE should not goes to QoS*/
#endif
	}

#if defined (CONFIG_RA_HW_NAT_IPV6)
	else if (IS_IPV6_GRP(foe_entry)) {
#if defined (CONFIG_RAETH_QDMA)
#if defined (CONFIG_RAETH_QDMATX_QDMARX)	
	    PpeSetInfoBlk2(&foe_entry->ipv6_3t_route.iblk2, 5, 0x3F, 0x3F); //5=QDMA
#else
	    PpeSetInfoBlk2(&foe_entry->ipv6_3t_route.iblk2, 0, 0x3F, 0x3F); //0=PDMA
	    if (FOE_SP(skb) == 5){
            	foe_entry->ipv6_3t_route.iblk2.fqos = 0; //wifi to wifi not go to pse port6
	    } else {
	    	foe_entry->ipv6_3t_route.iblk2.fqos = 1;
	    }
#endif
	    if(PpeParseResult.vlan1 == 0)
	    {
		    foe_entry->ipv6_3t_route.vlan1 = FOE_ENTRY_NUM(skb);
		    foe_entry->ipv6_3t_route.etype = ntohs(0x5678);
		    foe_entry->bfib1.vlan_layer = 1;
	    }
	    else if (PpeParseResult.vlan2 == 0)
	    {
		    foe_entry->ipv6_3t_route.vlan1 = FOE_ENTRY_NUM(skb);
		    foe_entry->ipv6_3t_route.etype = ntohs(0x5678);
			foe_entry->ipv6_3t_route.vlan2 = ntohs(PpeParseResult.vlan1);
			foe_entry->bfib1.vlan_layer = 2;
	    }
	    else
	    {
		    foe_entry->ipv6_3t_route.vlan1 = FOE_ENTRY_NUM(skb);
		    foe_entry->ipv6_3t_route.etype = ntohs(0x5678);
		    foe_entry->ipv6_3t_route.vlan2 = ntohs(PpeParseResult.vlan1);
		    foe_entry->bfib1.vlan_layer = 3;

	    }
#else
	    PpeSetInfoBlk2(&foe_entry->ipv6_3t_route.iblk2, 0, 0x3F, 0x3F); //0=CPU
	    foe_entry->ipv6_3t_route.iblk2.fqos = 0; /* PDMA MODE should not goes to QoS*/  //Landen: customer's change
#endif
	}
#endif
#else
	foe_entry->ipv4_hnapt.iblk2.dp = 0;	/* cpu */
#endif // CONFIG_RALINK_MT7620 //
#else
	return 1;
#endif // CONFIG_RA_HW_NAT_WIFI || CONFIG_RA_HW_NAT_NIC_USB //

	} else {
#if defined (CONFIG_RAETH_QDMA)
		if(IS_IPV4_GRP(foe_entry)) {
			if (((FOE_MAGIC_TAG(skb) == FOE_MAGIC_PCI) || 
		   	    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN))) {
				foe_entry->ipv4_hnapt.iblk2.fqos = 1;
			} else {
				if (FOE_SP(skb) == 5){
					foe_entry->ipv4_hnapt.iblk2.fqos = 0;
				}else {
					foe_entry->ipv4_hnapt.iblk2.fqos = 1;
				}
			}
		}
#if defined (CONFIG_RA_HW_NAT_IPV6)
		else if(IS_IPV6_GRP(foe_entry)) {
			    if (((FOE_MAGIC_TAG(skb) == FOE_MAGIC_PCI) || 
			         (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN))) {
				    foe_entry->ipv6_5t_route.iblk2.fqos = 1;
			    } else {
				    if (FOE_SP(skb) == 5){
					    foe_entry->ipv6_5t_route.iblk2.fqos = 0;
				    } else {
					    foe_entry->ipv6_5t_route.iblk2.fqos = 1;
				    }

			    }
		}
#endif // CONFIG_RA_HW_NAT_IPV6 //
#endif // CONFIG_RAETH_QDMA //

	    /* RT3883/MT7621 with 2xGMAC - Assuming GMAC2=WAN  and GMAC1=LAN */
#if defined (CONFIG_RAETH_GMAC2)
	    if (gmac_no == 1) {
		if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) ||  defined (CONFIG_ARCH_MT7623)
		    if(IS_IPV4_GRP(foe_entry)) {
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 1, 0x3F, 1);
		    } 
#if defined (CONFIG_RA_HW_NAT_IPV6)
		    else if(IS_IPV6_GRP(foe_entry)) {
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 1, 0x3F, 1);
		    }
#endif
#else
		    foe_entry->ipv4_hnapt.iblk2.dp = 1;
#endif
		} else {
		    return 1;
		}
	    } else if (gmac_no == 2) {
		if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
		    if(IS_IPV4_GRP(foe_entry)) {
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 2, 0x3F, 2);
		    } 
#if defined (CONFIG_RA_HW_NAT_IPV6)
		    else if(IS_IPV6_GRP(foe_entry)) {
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 2, 0x3F, 2);
		    }
#endif
#else
		    foe_entry->ipv4_hnapt.iblk2.dp = 2;
#endif
		} else {
		    return 1;
		}
	    }

	    /* RT2880, RT3883 */
#elif defined (CONFIG_RALINK_RT2880) || defined (CONFIG_RALINK_RT3883)
	    if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == lan_vid) {
		if ((bind_dir == DOWNSTREAM_ONLY)
			|| (bind_dir == BIDIRECTION)) {
		    foe_entry->ipv4_hnapt.iblk2.dp = 1;
		} else {
		    return 1;
		}
	    } else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == wan_vid) {
		if ((bind_dir == UPSTREAM_ONLY)
			|| (bind_dir == BIDIRECTION)) {
		    foe_entry->ipv4_hnapt.iblk2.dp = 1;
		} else {
		    return 1;
		}
	    }

#elif defined (CONFIG_HNAT_V2)
	    if(IS_IPV4_GRP(foe_entry)) {
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
		if ( ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 1) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 4) ) {
#else
		if ( ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 4) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 5) ) {
#endif
#else
		if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == lan_vid) {
#endif
		    if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 1, 0x3F, 1);
#else // MT7620
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 8, 0x3F, 1);
#endif
		    } else {
			return 1;
		    }
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
		} else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 5) {
#else
		} else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 1) {
#endif
#else
		} else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == wan_vid) {
#endif
		    if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 1, 0x3F, 2);
#else // MT7620
			PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 8, 0x3F, 2);
#endif
		    } else {
			return 1;
		    }
		} else { //one-arm
		    PpeSetInfoBlk2(&foe_entry->ipv4_hnapt.iblk2, 8, 0x3F, 1);
		}
	    } 

#if defined (CONFIG_RA_HW_NAT_IPV6)
	    else if(IS_IPV6_GRP(foe_entry)) {
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
		if ( ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 1) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 4) ) {
#else
		if ( ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 4) ||
	             ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 5) ) {
#endif
#else
		if ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == lan_vid) {
#endif
		    if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 1, 0x3F, 1);
#else //MT7620
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 8, 0x3F, 1);
#endif
		    } else {
			return 1;
		    }
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
		} else if ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 5) {
#else
		} else if ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == 1) {
#endif
#else
		} else if ((foe_entry->ipv6_5t_route.vlan1 & VLAN_VID_MASK) == wan_vid) {
#endif
		    if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 1, 0x3F, 2);
#else //MT7620
			PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 8, 0x3F, 2);
#endif
		    } else {
			return 1;
		    }
		} else { //one-arm
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	  	    PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 1, 0x3F, 1);
#else
		    PpeSetInfoBlk2(&foe_entry->ipv6_5t_route.iblk2, 8, 0x3F, 1);
#endif
		}
	    }
#endif // CONFIG_RA_HW_NAT_IPV6 //

#else
	    /*  RT3052, RT335x */
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
		if ( ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 1) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 4) ) {
#else
		if ( ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 2) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 3) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 4) ||
	             ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 5) ) {
#endif
#else
	    if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == lan_vid) {
#endif
		if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
		    foe_entry->ipv4_hnapt.iblk2.dp = 1;	/* LAN traffic use VirtualPort1 in GMAC1 */
		} else {
		    return 1;
		}
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#ifdef CONFIG_WAN_AT_P4
	    } else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 5) {
#else
	    } else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == 1) {
#endif
#else
	    } else if ((foe_entry->ipv4_hnapt.vlan1 & VLAN_VID_MASK) == wan_vid) {
#endif
		if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
		    foe_entry->ipv4_hnapt.iblk2.dp = 2;	/* WAN traffic use VirtualPort2 in GMAC1 */
		} else {
		    return 1;
		}
	    } else {
		/* for one arm NAT test -> no vlan tag */
		foe_entry->ipv4_hnapt.iblk2.dp = 1;
	    }
#endif
	}

	return 0;
}

uint32_t PpeSetExtIfNum(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	

#if defined  (CONFIG_RA_HW_NAT_WIFI) || defined  (CONFIG_RA_HW_NAT_NIC_USB)
    uint32_t offset = 0;
#if !defined  (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
    /* This is ugly soultion to support WiFi pseudo interface.
     * Please double check the definition is the same as include/rt_linux.h 
     */
#define CB_OFF  10
#define RTMP_GET_PACKET_IF(skb)                 skb->cb[CB_OFF+6]
#define MIN_NET_DEVICE_FOR_MBSSID               0x00
#define MIN_NET_DEVICE_FOR_WDS                  0x10
#define MIN_NET_DEVICE_FOR_APCLI                0x20
#define MIN_NET_DEVICE_FOR_MESH                 0x30

    /* Set actual output port info */
#if defined (CONFIG_RTDEV_MII) || defined (CONFIG_RTDEV_USB) || \
    defined (CONFIG_RTDEV_PCI) || defined (CONFIG_RTDEV)
    if (strncmp(skb->dev->name, "rai", 3) == 0) {
#if defined (CONFIG_RT3090_AP_MESH) || defined (CONFIG_RT5392_AP_MESH) || \
	defined (CONFIG_RT3572_AP_MESH) || defined (CONFIG_RT5572_AP_MESH) || \
	defined (CONFIG_RT5592_AP_MESH) || defined (CONFIG_RT3593_AP_MESH)
	if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_MESH) {
	    offset =
		(RTMP_GET_PACKET_IF(skb) - MIN_NET_DEVICE_FOR_MESH +
		 DP_MESHI0);
	} else
#endif // CONFIG_RTDEV_AP_MESH //

#if defined (CONFIG_RT3090_AP_APCLI) || defined (CONFIG_RT5392_AP_APCLI) || \
	    defined (CONFIG_RT3572_AP_APCLI) || defined (CONFIG_RT5572_AP_APCLI) || \
	    defined (CONFIG_RT5592_AP_APCLI) || defined (CONFIG_RT3593_AP_APCLI) || \
	    defined (CONFIG_MT7610_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	    if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_APCLI) {
		offset =
		    (RTMP_GET_PACKET_IF(skb) -
		     MIN_NET_DEVICE_FOR_APCLI + DP_APCLII0);
	    } else
#endif // CONFIG_RTDEV_AP_APCLI //
#if defined (CONFIG_RT3090_AP_WDS) || defined (CONFIG_RT5392_AP_WDS) || \
		defined (CONFIG_RT3572_AP_WDS) || defined (CONFIG_RT5572_AP_WDS) || \
		defined (CONFIG_RT5592_AP_WDS) || defined (CONFIG_RT3593_AP_WDS) || \
		defined (CONFIG_MT7610_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
		if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_WDS) {
		    offset =
			(RTMP_GET_PACKET_IF(skb) - MIN_NET_DEVICE_FOR_WDS +
			 DP_WDSI0);
		} else
#endif // CONFIG_RTDEV_AP_WDS //
		{
		    offset = RTMP_GET_PACKET_IF(skb) + DP_RAI0;
		}
    } else
#endif // CONFIG_RTDEV_MII || CONFIG_RTDEV_USB || CONFIG_RTDEV_PCI || CONFIG_RTDEV

	if (strncmp(skb->dev->name, "ra", 2) == 0) {
#if defined (CONFIG_RT2860V2_AP_MESH)
	    if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_MESH) {
		offset =
		    (RTMP_GET_PACKET_IF(skb) - MIN_NET_DEVICE_FOR_MESH +
		     DP_MESH0);
	    } else
#endif // CONFIG_RT2860V2_AP_MESH //
#if defined (CONFIG_RT2860V2_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
		if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_APCLI) {
		    offset =
			(RTMP_GET_PACKET_IF(skb) -
			 MIN_NET_DEVICE_FOR_APCLI + DP_APCLI0);
		} else
#endif // CONFIG_RT2860V2_AP_APCLI //
#if defined (CONFIG_RT2860V2_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
		    if (RTMP_GET_PACKET_IF(skb) >= MIN_NET_DEVICE_FOR_WDS) {
			offset =
			    (RTMP_GET_PACKET_IF(skb) - MIN_NET_DEVICE_FOR_WDS +
			     DP_WDS0);
		    } else
#endif // CONFIG_RT2860V2_AP_WDS //
		    {
			offset = RTMP_GET_PACKET_IF(skb) + DP_RA0;
		    }
	}
#if defined (CONFIG_SUPPORT_OPENWRT)
	else if (strncmp(skb->dev->name, "eth0", 4) == 0) {
	    offset = DP_GMAC1;
	}
#ifdef CONFIG_RAETH_GMAC2
	else if (strncmp(skb->dev->name, "eth1", 4) == 0) {
	    offset = DP_GMAC2;
	}
#endif
#else
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
	else if (strncmp(skb->dev->name, "eth0", 4) == 0) {
	    offset = DP_PCI;
	}
	else if (strncmp(skb->dev->name, "eth1", 4) == 0) {
	    offset = DP_USB;
	}
#endif // CONFIG_RA_HW_NAT_NIC_USB //
	else if (strncmp(skb->dev->name, "eth2", 4) == 0) {
	    offset = DP_GMAC1;
	}
#ifdef CONFIG_RAETH_GMAC2
	else if (strncmp(skb->dev->name, "eth3", 4) == 0) {
	    offset = DP_GMAC2;
	}
#endif
#endif
	else {
		if (printk_ratelimit())
	 		printk("HNAT: unknow interface %s\n", skb->dev->name);
	    return 1;
	}
#endif // !(CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
#if defined  (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	uint32_t i = 0;
	int dev_match = 0;
	for (i=0; i<MAX_IF_NUM; i++){
		if(DstPort[i] == skb->dev){
			offset = i;
			dev_match = 1;
			if (DebugLevel >= 1) {
				printk("dev match offset, name=%s ifined=%x\n", skb->dev->name,i);
			}
			break;
		}
	}
	if (dev_match == 0){
		if (printk_ratelimit())
			printk("%s UnKnown Interface, offset =%x\n", __func__, i);
		return 1;
	}
#endif// (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)

	    if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)) {
		foe_entry->ipv4_hnapt.act_dp = offset;
	    }

#if defined (CONFIG_HNAT_V2)
#if defined (CONFIG_RA_HW_NAT_IPV6)
	    else if (IS_IPV4_DSLITE(foe_entry)) {
		foe_entry->ipv4_dslite.act_dp = offset;
	    } else if (IS_IPV6_3T_ROUTE(foe_entry)) {
		foe_entry->ipv6_3t_route.act_dp = offset;
	    } else if (IS_IPV6_5T_ROUTE(foe_entry)) {
		foe_entry->ipv6_5t_route.act_dp = offset;
	    } else if (IS_IPV6_6RD(foe_entry)) {
		foe_entry->ipv6_6rd.act_dp = offset;
	    } else {
		return 1;
	    }
#endif // CONFIG_RA_HW_NAT_IPV6 //
#endif // CONFIG_HNAT_V2 //
#endif // CONFIG_RA_HW_NAT_WIFI || CONFIG_RA_HW_NAT_NIC_USB //

	    return 0;
}

void PpeSetEntryBind(struct sk_buff *skb, struct FoeEntry *foe_entry)
{

    uint32_t current_time;
    /* Set Current time to time_stamp field in information block 1 */
    current_time = RegRead(FOE_TS) & 0xFFFF;
    foe_entry->bfib1.time_stamp = (uint16_t) current_time;

    /* Ipv4: TTL / Ipv6: Hot Limit filed */
    foe_entry->ipv4_hnapt.bfib1.ttl = DFL_FOE_TTL_REGEN;
#if defined (CONFIG_HNAT_V2)
    /* enable cache by default */
    foe_entry->ipv4_hnapt.bfib1.cah = 1;
#endif

#if defined (CONFIG_RA_HW_NAT_ACL2UP_HELPER)
    /*set user priority */
    foe_entry->ipv4_hnapt.iblk2.up = FOE_SP(skb);
    foe_entry->ipv4_hnapt.iblk2.fp = 1;
#endif


#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
    foe_entry->ipv4_hnapt.bfib1.ps = 1;
#endif

#if defined (CONFIG_RA_HW_NAT_PREBIND)
    foe_entry->udib1.preb = 1;
#else
    /* Change Foe Entry State to Binding State */
    foe_entry->bfib1.state = BIND;
    /* Dump Binding Entry */
    if (DebugLevel >= 1) {
	FoeDumpEntry(FOE_ENTRY_NUM(skb));
    }
#endif

}
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
/* __MSTC__, Smith, add semaphore for PpeDevRegHandler & PpeDevUnRegHandler */
DEFINE_SEMAPHORE(PpeDevRegSem);
void PpeDevRegHandler(struct net_device *dev)
{
	int i = 0;
	int empty = MAX_IF_NUM;

	down(&PpeDevRegSem);
	for(i=0; i<MAX_IF_NUM; i++)
	{
		if(DstPort[i] == dev)
		{
			printk("%s ifindex = %x\n",dev->name, i);
			break;
		}
		if(DstPort[i] == NULL)
		{
			if(empty == MAX_IF_NUM)
				empty = i;
		}
	}
	if(i == MAX_IF_NUM)
	{
		if(empty != MAX_IF_NUM)
		{
			DstPort[empty] = dev;
			printk("%s ifindex = %x\n",dev->name, empty);
		}
		else
			printk("***********FAIL to register, full now\n");
	}
	up(&PpeDevRegSem);
}

void PpeDevUnRegHandler(struct net_device *dev)
{
	int i = 0;

	down(&PpeDevRegSem);
	for(i=0; i<MAX_IF_NUM; i++)
	{
		if(DstPort[i] == dev)
		{
			DstPort[i] = NULL;
			break;
		}
	}
	up(&PpeDevRegSem);
}
#endif
int32_t PpeTxHandler(struct sk_buff *skb, int gmac_no)
{
    struct FoeEntry *foe_entry = &PpeFoeBase[FOE_ENTRY_NUM(skb)];
#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
    struct PsEntry *ps_entry = &PpePsBase[FOE_ENTRY_NUM(skb)];
#endif

    if(FOE_ENTRY_NUM(skb)==0x3fff || FOE_ENTRY_NUM(skb) >= FOE_4TB_SIZ) {
	    return 1;
    }

    /* 
     * Packet is interested by ALG?
     * Yes: Don't enter binind state
     * No: If flow rate exceed binding threshold, enter binding state.
     */
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
        if(pptp_fast_path){
	    if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_FASTPATH)
	    {
		FOE_MAGIC_TAG(skb) = 0;
		if (DebugLevel >= 1) {
			//printk("PpeTxHandler FOE_MAGIC_FASTPATH\n");
		}
		hash_cnt ++;
		if((FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (skb->len > 128) && (hash_cnt % 32 == 1)){
	    	        SendHashPkt(skb);
		}
		memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	        return 1;
	    }
	 }

/*WAN->LAN accelerate*/
        if(l2tp_fast_path){
	    if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_FASTPATH)
	    {
		
		FOE_MAGIC_TAG(skb) = 0;
		if (DebugLevel >= 1) 
		{
			//printk("PpeTxHandler FOE_MAGIC_FASTPATH\n");
		}
		hash_cnt ++;
		//if((FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (skb->len > 1360))
		if((FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (skb->len > 1360) && (hash_cnt % 32 == 1)){
		    //printk("hash_cnt is %d\n", hash_cnt);
	    	    SendL2TPHashPkt(skb);
		}
		memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	        return 1;
	    }
	 }
	if (IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (FOE_ALG(skb) == 0) ) {
		
	    struct iphdr *iph = NULL;
	    struct udphdr *uh = NULL;
	    
	    iph = (struct iphdr *)(skb->data + 14 + VLAN_LEN); 
	    uh = (struct udphdr *)(skb->data + 14 + VLAN_LEN + 20); 

	   //printk("iph->protocol is 0x%2x\n",iph->protocol);
	   //printk("uh->dest is %4x\n",uh->dest);
	   //printk("uh->source is %4x\n",uh->source);
	   //printk("FOE_AI(skb) is 0x%x\n",FOE_AI(skb));

	    //skb_dump(skb);
	if((iph->protocol == IPPROTO_GRE) || (ntohs(uh->dest)==1701))
	{
		/*BIND flow using pptp/l2tp packets info*/
		//skb_dump(skb);
	        //printk("LAN->WAN  TxH HIT_UNBIND_RATE_REACH\n");
		//FoeDumpEntry(FOE_ENTRY_NUM(skb));
		if (DebugLevel >= 1) 
		{
			printk("LAN->WAN  TxH PptpToWanParseLayerInfo\n");
		}

		if(pptp_fast_path){
			if (PptpToWanParseLayerInfo(skb)) {
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				return 1;
			}
		}

		if(l2tp_fast_path){
			if (L2tpToWanParseLayerInfo(skb)) {
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				return 1;
			}
		}

		/*layer2 keep original*/
		if (DebugLevel >= 1) 
		{
			printk("Lan -> Wan PpeFillInL2Info \n");
		}
		/* Set Layer2 Info */
		if (PpeFillInL2Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			/*already bind packet, return 1 to go out*/
			return 1;
		}

		/* Set Layer3 Info */
		if (PpeFillInL3Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			return 1;
		}

		/* Set Layer4 Info */
		if (PpeFillInL4Info(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			return 1;
		}

		/* Set force port to CPU!!!  */
		if (PpeSetForcePortInfo(skb, foe_entry, 777)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			return 1;
		}
		if (DebugLevel >= 1) {
			printk("PpeSetEntryBind \n");
		}
		/* Set Pseudo Interface info in Foe entry */
		/*
		if (PpeSetExtIfNum(skb, foe_entry)) {
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			return 0;
		}
		*/
		/* Enter binding state */
		PpeSetEntryBind(skb, foe_entry);
		return 1;
	    }
	}
#endif
    if (IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (FOE_ALG(skb) == 0) ) {
#if defined (CONFIG_SUPPORT_WLAN_OPTIMIZE_TX)
	if (strncmp(skb->dev->name, "ra", 2) == 0)
		if (BridgeShortCutTx(skb)) return 1; ////Bridge ==> sw path (rps)
#endif
	/* get start addr for each layer */
	if (PpeParseLayerInfo(skb)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

	/* Set Layer2 Info */
	if (PpeFillInL2Info(skb, foe_entry)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

	/* Set Layer3 Info */
	if (PpeFillInL3Info(skb, foe_entry)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

	/* Set Layer4 Info */
	if (PpeFillInL4Info(skb, foe_entry)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

	/* Set force port info */
	if (PpeSetForcePortInfo(skb, foe_entry, gmac_no)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

	/* Set Pseudo Interface info in Foe entry */
	if (PpeSetExtIfNum(skb, foe_entry)) {
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 1;
	}

#if defined (CONFIG_RAETH_QDMA) && defined  (CONFIG_PPE_MCAST)
	if(PpeParseResult.is_mcast) {
		foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, M2Q_table[skb->mark]);
#ifdef CONFIG_PSEUDO_SUPPORT
		if(lan_wan_separate==1 && gmac_no==2){ 
			foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, M2Q_table[skb->mark] + 8);
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQUp); // queue3
			}
#endif
		} 
		if((lan_wan_separate==1) && (gmac_no==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQDl); // queue0
			}
#endif
		}
#endif//CONFIG_PSEUDO_SUPPORT

#ifdef CONFIG_RAETH_HW_VLAN_TX
	if(lan_wan_separate==1 && vlan_tx_tag_get(skb)==2){ 
			foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, M2Q_table[skb->mark] + 8);
#if defined (CONFIG_HW_SFQ)			
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
					foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQUp); // queue1
			}
#endif				
		} 
		if((lan_wan_separate==1) && (vlan_tx_tag_get(skb)==1)){
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQDl); // queue0	
			}
#endif
		}
#endif// end CONFIG_RAETH_HW_VLAN_TX

#ifndef CONFIG_PSEUDO_SUPPORT
#ifndef CONFIG_RAETH_HW_VLAN_TX 
	if(lan_wan_separate==1){
		struct vlan_hdr *vh = NULL;
    unsigned short vlanid = 0;
    unsigned short vlan_TCI;
		vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
		vlan_TCI = vh->h_vlan_TCI;
    vlanid = (vlan_TCI & VLAN_VID_MASK)>>8;
		if(vlanid == 2)//to wan
		{
			foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, M2Q_table[skb->mark] + 8);
#if defined (CONFIG_HW_SFQ)			
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQUp); // queue3
			}
#endif			
			
		}else if(vlanid == 1){ // to lan
#if defined (CONFIG_HW_SFQ)
			if(web_sfq_enable==1 &&(skb->mark == 2)){ 
				foe_mcast_entry_qid(PpeParseResult.vlan1, PpeParseResult.dmac, HwSfqQDl); // queue0	
			}
#endif
		}
  }
#endif //CONFIG_PSEUDO_SUPPORT
#endif //CONFIG_RAETH_HW_VLAN_TX 
	}
#endif

	/* Enter binding state */
	PpeSetEntryBind(skb, foe_entry);
#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
/*add sampling policy here*/	
	ps_entry->en = 0x1 << 1;
	ps_entry->pkt_cnt = 0x10;
#endif

#if defined (CONFIG_ARCH_MT7623)
    } else if (IS_MAGIC_TAG_VALID(skb)
	    && (FOE_AI(skb) == HIT_BIND_PACKET_SAMPLING)){
	/* this is duplicate packet in PS function, 
	 * just drop it */
	//printk("PS drop#%d\n", FOE_ENTRY_NUM(skb));
	memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	return 0;
#endif
#if defined (CONFIG_HNAT_V2)
    } else if (IS_MAGIC_TAG_VALID(skb)
	    && (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR
		|| (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR))) {
		if (DebugLevel >= 3) {
			printk("Tx : Got HIT_BIND_KEEPALIVE_DUP_OLD_HDR packe (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
#else
    } else if (IS_MAGIC_TAG_VALID(skb)
	    && (FOE_AI(skb) == HIT_BIND_KEEPALIVE)
	    && (DFL_FOE_KA == 0)) {
#endif
	/* this is duplicate packet in keepalive new header mode, 
	 * just drop it */
	memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	return 0;
    } else if (IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH) && (FOE_ALG(skb) == 1)) {
	if (DebugLevel >= 3) {
	    NAT_PRINT ("FOE_ALG=1 (Entry=%d)\n", FOE_ENTRY_NUM(skb));
	}
#if defined (CONFIG_RA_HW_NAT_PREBIND)
    } else if((FOE_AI(skb) == HIT_PRE_BIND)) {
#ifdef PREBIND_TEST
	 if(jiffies %2==0) {
	    printk("drop prebind packet jiffies=%lu\n",jiffies);
	    memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	    return 0;
	}
#endif

	if(foe_entry->udib1.preb && foe_entry->bfib1.state != BIND) {
	    foe_entry->bfib1.state = BIND;
	    foe_entry->udib1.preb = 0;
	    /* Dump Binding Entry */
	    if (DebugLevel >= 1) {
		FoeDumpEntry(FOE_ENTRY_NUM(skb));
	    }
	} else {
		//drop duplicate prebind notify packet
		memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
		return 0; 
	}
#endif
    }

    return 1;
}

void PpeSetFoeEbl(uint32_t FoeEbl)
{
    uint32_t PpeFlowSet = 0;

    PpeFlowSet = RegRead(PPE_FLOW_SET);

    /* FOE engine need to handle unicast/multicast/broadcast flow */
    if (FoeEbl == 1) {
	PpeFlowSet |= (BIT_IPV4_NAPT_EN | BIT_IPV4_NAT_EN);
#if defined (CONFIG_HNAT_V2)
	PpeFlowSet |= (BIT_IPV4_NAT_FRAG_EN); //ip fragment
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
#else
	PpeFlowSet |= (BIT_IPV4_HASH_GREK);
#endif
#if defined(CONFIG_RA_HW_NAT_IPV6)
	PpeFlowSet |= (BIT_IPV4_DSL_EN | BIT_IPV6_6RD_EN | BIT_IPV6_3T_ROUTE_EN | BIT_IPV6_5T_ROUTE_EN);
	//		PpeFlowSet |= (BIT_IPV6_HASH_FLAB); // flow label
	PpeFlowSet |= (BIT_IPV6_HASH_GREK);
#endif

#else
	PpeFlowSet |= (BIT_FUC_FOE | BIT_FMC_FOE | BIT_FBC_FOE);
#if defined(CONFIG_RA_HW_NAT_IPV6)
	PpeFlowSet |= (BIT_IPV6_FOE_EN);
#endif

#endif
    } else {
	PpeFlowSet &= ~(BIT_IPV4_NAPT_EN | BIT_IPV4_NAT_EN);
#if defined (CONFIG_HNAT_V2)
	PpeFlowSet &= ~(BIT_IPV4_NAT_FRAG_EN);
#if defined(CONFIG_RA_HW_NAT_IPV6)
	PpeFlowSet &= ~(BIT_IPV4_DSL_EN | BIT_IPV6_6RD_EN | BIT_IPV6_3T_ROUTE_EN | BIT_IPV6_5T_ROUTE_EN);
	//		PpeFlowSet &= ~(BIT_IPV6_HASH_FLAB);
	PpeFlowSet &= ~(BIT_IPV6_HASH_GREK);
#else
	PpeFlowSet &= ~(BIT_FUC_FOE | BIT_FMC_FOE | BIT_FBC_FOE);
#if defined(CONFIG_RA_HW_NAT_IPV6)
	PpeFlowSet &= ~(BIT_IPV6_FOE_EN);
#endif

#endif
#endif
    }

    RegWrite(PPE_FLOW_SET, PpeFlowSet);
}

static int PpeSetFoeHashMode(uint32_t HashMode)
{

    /* Allocate FOE table base */
    if(!FoeAllocTbl(FOE_4TB_SIZ)) {
	return 0;
    }

    switch (FOE_4TB_SIZ) {
    case 1024:
	RegModifyBits(PPE_FOE_CFG, FoeTblSize_1K, 0, 3);
	break;
    case 2048:
	RegModifyBits(PPE_FOE_CFG, FoeTblSize_2K, 0, 3);
	break;
    case 4096:
	RegModifyBits(PPE_FOE_CFG, FoeTblSize_4K, 0, 3);
	break;
    case 8192:
	RegModifyBits(PPE_FOE_CFG, FoeTblSize_8K, 0, 3);
	break;
    case 16384:
	RegModifyBits(PPE_FOE_CFG, FoeTblSize_16K, 0, 3);
	break;
    }

    /* Set Hash Mode */
#if defined (CONFIG_HNAT_V2)
    RegModifyBits(PPE_FOE_CFG, HashMode, 14, 2);
    RegWrite(PPE_HASH_SEED, HASH_SEED);
#if defined (CONFIG_RA_HW_NAT_HASH_DBG_IPV6_SIP)
    RegModifyBits(PPE_FOE_CFG, 3, 18, 2); //ipv6_sip
#elif defined (CONFIG_RA_HW_NAT_HASH_DBG_IPV4_SIP)
    RegModifyBits(PPE_FOE_CFG, 2, 18, 2); //ipv4_sip
#elif defined (CONFIG_RA_HW_NAT_HASH_DBG_SPORT)
    RegModifyBits(PPE_FOE_CFG, 1, 18, 2); //sport
#else
    RegModifyBits(PPE_FOE_CFG, 0, 18, 2); //disable
#endif

#if defined (CONFIG_RA_HW_NAT_IPV6)
    RegModifyBits(PPE_FOE_CFG, 1, 3, 1);	//entry size = 80bytes
#else
    RegModifyBits(PPE_FOE_CFG, 0, 3, 1);	//entry size = 64bytes
#endif

#if defined (CONFIG_RA_HW_NAT_PREBIND)
    RegModifyBits(PPE_FOE_CFG, 1, 6, 1);	//pre-bind age enable
#endif

#else
    RegModifyBits(PPE_FOE_CFG, HashMode, 3, 1);
#endif

    /* Set action for FOE search miss */
    RegModifyBits(PPE_FOE_CFG, FWD_CPU_BUILD_ENTRY, 4, 2);

    return 1;
}

static void PpeSetAgeOut(void)
{
#if defined (CONFIG_HNAT_V2)
    /* set Bind Non-TCP/UDP Age Enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_NTU_AGE, 7, 1);
#endif

    /* set Unbind State Age Enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_UNB_AGE, 8, 1);

    /* set min threshold of packet count for aging out at unbind state */
    RegModifyBits(PPE_FOE_UNB_AGE, DFL_FOE_UNB_MNP, 16, 16);

    /* set Delta time for aging out an unbind FOE entry */
    RegModifyBits(PPE_FOE_UNB_AGE, DFL_FOE_UNB_DLTA, 0, 8);

    /* set Bind TCP Age Enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_TCP_AGE, 9, 1);

    /* set Bind UDP Age Enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_UDP_AGE, 10, 1);


    /* set Bind TCP FIN Age Enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_FIN_AGE, 11, 1);

    /* set Delta time for aging out an bind UDP FOE entry */
    RegModifyBits(PPE_FOE_BND_AGE0, DFL_FOE_UDP_DLTA, 0, 16);

#if defined (CONFIG_HNAT_V2)
    /* set Delta time for aging out an bind Non-TCP/UDP FOE entry */
    RegModifyBits(PPE_FOE_BND_AGE0, DFL_FOE_NTU_DLTA, 16, 16);
#endif

    /* set Delta time for aging out an bind TCP FIN FOE entry */
    RegModifyBits(PPE_FOE_BND_AGE1, DFL_FOE_FIN_DLTA, 16, 16);

    /* set Delta time for aging out an bind TCP FOE entry */
    RegModifyBits(PPE_FOE_BND_AGE1, DFL_FOE_TCP_DLTA, 0, 16);
}

static void PpeSetFoeKa(void)
{
    /* set Keep alive packet with new/org header */
#if defined (CONFIG_HNAT_V2)
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_KA, 12, 2);
#else
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_KA, 12, 1);

    /* set Keep alive enable */
    RegModifyBits(PPE_FOE_CFG, DFL_FOE_KA_EN, 13, 1);
#endif

    /* Keep alive timer value */
    RegModifyBits(PPE_FOE_KA, DFL_FOE_KA_T, 0, 16);

    /* Keep alive time for bind FOE TCP entry */
    RegModifyBits(PPE_FOE_KA, DFL_FOE_TCP_KA, 16, 8);

    /* Keep alive timer for bind FOE UDP entry */
    RegModifyBits(PPE_FOE_KA, DFL_FOE_UDP_KA, 24, 8);

#if defined (CONFIG_HNAT_V2)
    /* Keep alive timer for bind Non-TCP/UDP entry */
    RegModifyBits(PPE_BIND_LMT_1, DFL_FOE_NTU_KA, 16, 8);
#endif
#if defined (CONFIG_RA_HW_NAT_PREBIND)
    RegModifyBits(PPE_BIND_LMT_1, DFL_PBND_RD_LMT, 24, 8);
#endif
}

static void PpeSetFoeBindRate(uint32_t FoeBindRate)
{
    /* Allowed max entries to be build during a time stamp unit */

    /* smaller than 1/4 of total entries */
    RegModifyBits(PPE_FOE_LMT1, DFL_FOE_QURT_LMT, 0, 14);

    /* between 1/2 and 1/4 of total entries */
    RegModifyBits(PPE_FOE_LMT1, DFL_FOE_HALF_LMT, 16, 14);

    /* between full and 1/2 of total entries */
    RegModifyBits(PPE_FOE_LMT2, DFL_FOE_FULL_LMT, 0, 14);

    /* Set reach bind rate for unbind state */
    RegModifyBits(PPE_FOE_BNDR, FoeBindRate, 0, 16);
#if defined (CONFIG_RA_HW_NAT_PREBIND)
    RegModifyBits(PPE_FOE_BNDR,  DFL_PBND_RD_PRD, 16, 16);
#endif
}

static void PpeSetFoeGloCfgEbl(uint32_t Ebl)
{
#if defined (CONFIG_RALINK_MT7620)
    uint32_t tpf = 0;
#endif

    if (Ebl == 1) {
#if defined (CONFIG_RALINK_MT7620)
	/* 1. Remove P7 on forwarding ports */
	/* It's chip default setting */

	/* 2. PPE Forward Control Register: PPE_PORT=Port7 */
	RegModifyBits(PFC, 7, 0, 3);

	/* 3. Select P7 as PPE port (PPE_EN=1) */
	RegModifyBits(PFC, 1, 3, 1);

	/* TO_PPE Forwarding Register */	
	tpf = IPV4_PPE_MYUC | IPV4_PPE_MC | IPV4_PPE_IPM | IPV4_PPE_UC | IPV4_PPE_UN;

#if defined (CONFIG_RA_HW_NAT_IPV6)
	tpf |= (IPV6_PPE_MYUC | IPV6_PPE_MC | IPV6_PPE_IPM | IPV6_PPE_UC | IPV6_PPE_UN);
#endif

/*TP PPE off, use ACL instead*/	
#if defined (CONFIG_RALINK_MT7620)
	if ((RegRead(0xB000000C) & 0xf) >= 0x5) {
		RegWrite(TPF0, tpf);
		RegWrite(TPF1, tpf);
		RegWrite(TPF2, tpf);
		RegWrite(TPF3, tpf);
		RegWrite(TPF4, tpf);
		RegWrite(TPF5, tpf);
	}
#else
	RegWrite(TPF0, tpf);
	RegWrite(TPF1, tpf);
	RegWrite(TPF2, tpf);
	RegWrite(TPF3, tpf);
	RegWrite(TPF4, tpf);
	RegWrite(TPF5, tpf);
#endif
	/* Forced Port7 link up, 1Gbps, and Full duplex  */
	RegWrite(PMCR_P7, 0x5e33b);

	/* Disable SA Learning */
	RegModifyBits(PSC_P7, 1, 4, 1);

	/* Use default values on P7 */
#elif defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	/* PPE Engine Enable */
	RegModifyBits(PPE_GLO_CFG, 1, 0, 1);

#if defined (CONFIG_ARCH_MT7623) && defined (CONFIG_RA_HW_NAT_IPV6)
	/* TSID Enable */
	RegModifyBits(PPE_GLO_CFG, 1, 1, 1);
#endif
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
#if defined (CONFIG_PPE_MCAST)
	/* Enable multicast table lookup */
	RegModifyBits(PPE_GLO_CFG, 1, 7, 1);
	RegModifyBits(PPE_GLO_CFG, 1, 12, 2); //Reserve 16 entry for multicast packet
	RegModifyBits(PPE_MCAST_PPSE, 0, 0, 4); //multicast port0 map to PDMA
	RegModifyBits(PPE_MCAST_PPSE, 1, 4, 4); //multicast port1 map to GMAC1
	RegModifyBits(PPE_MCAST_PPSE, 2, 8, 4); //multicast port2 map to GMAC2
	RegModifyBits(PPE_MCAST_PPSE, 5, 12, 4); //multicast port3 map to QDMA
#endif // CONFIG_PPE_MCAST //

#if defined (CONFIG_RAETH_QDMATX_QDMARX)	
	RegWrite(PPE_DFT_CPORT, 0x55555555); //default CPU port is port5 (QDMA)
#else
	RegWrite(PPE_DFT_CPORT, 0); //default CPU port is port0 (PDMA)
#endif // CONFIG_RAETH_QDMA //

#if defined (CONFIG_ARCH_MT7623) && defined (CONFIG_RA_HW_NAT_IPV6)
	RegModifyBits(PPE_DFT_CPORT, 1, 31, 1);
#endif

#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
	//RegWrite(PS_CFG, 1); //Enable PacketSampling
	RegWrite(PS_CFG, 0x3); //Enable PacketSampling, Disable Aging
#endif

#endif // CONFIG_RALINK_MT7621 //

#else
	/* PPE Engine Enable */
	RegModifyBits(PPE_GLO_CFG, 1, 0, 1);

	/* Use VLAN priority tag as priority decision */
	RegModifyBits(PPE_GLO_CFG, DFL_VPRI_EN, 8, 1);

	/* Use DSCP as priority decision */
	RegModifyBits(PPE_GLO_CFG, DFL_DPRI_EN, 9, 1);

	/* Re-generate VLAN priority tag */
	RegModifyBits(PPE_GLO_CFG, DFL_REG_VPRI, 10, 1);

	/* Re-generate DSCP */
	RegModifyBits(PPE_GLO_CFG, DFL_REG_DSCP, 11, 1);

	/* Random early drop mode */
	RegModifyBits(PPE_GLO_CFG, DFL_RED_MODE, 12, 2);

	/* Enable use ACL force priority for hit unbind 
	 * and rate reach packet in CPU reason */
	RegModifyBits(PPE_GLO_CFG, DFL_ACL_PRI_EN, 14, 1);

	/* Disable switch port 6 flow control if HNAT QoS is needed */
	//RegModifyBits(RALINK_ETH_SW_BASE+0xC8, 0x0, 8, 2);
#endif // CONFIG_HNAT_V2 //

	/* PPE Packet with TTL=0 */
	RegModifyBits(PPE_GLO_CFG, DFL_TTL0_DRP, 4, 1);

    } else {
#if defined (CONFIG_RALINK_MT7620)
	/* 1. Select P7 as PPE port (PPE_EN=1) */
	RegModifyBits(PFC, 0, 3, 1);

	/* TO_PPE Forwarding Register */	
	RegWrite(TPF0, 0);
	RegWrite(TPF1, 0);
	RegWrite(TPF2, 0);
	RegWrite(TPF3, 0);
	RegWrite(TPF4, 0);
	RegWrite(TPF5, 0);

	/* Forced Port7 link down */
	RegWrite(PMCR_P7, 0x5e330);

	/* Enable SA Learning */
	RegModifyBits(PSC_P7, 0, 4, 1);

#elif defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
	/* PPE Engine Disable */
	RegModifyBits(PPE_GLO_CFG, 0, 0, 1);
#else
	/* PPE Engine Disable */
	RegModifyBits(PPE_GLO_CFG, 0, 0, 1);
#endif

#if defined (CONFIG_RA_HW_NAT_PACKET_SAMPLING)
	RegWrite(PS_CFG, 0); //Disable PacketSampling
#endif

    }

}

/*
 * - VLAN->UP: Incoming VLAN Priority to User Priority (Fixed)
 * - DSCP->UP: Incoming DSCP to User Priority
 * - UP->xxx : User Priority to VLAN/InDSCP/OutDSCP/AC Priority Mapping
 *
 * VLAN | DSCP |  UP |VLAN Pri|In-DSCP |Out-DSCP| AC | WMM_AC
 * -----+------+-----+--------+--------+--------+----+-------
 *   0	| 00-07|  0  |   0    |  0x00  |  0x00	|  0 |  BE
 *   3	| 24-31|  3  |   3    |  0x18  |  0x10	|  0 |  BE
 *   1	| 08-15|  1  |   1    |  0x08  |  0x00	|  0 |  BG
 *   2  | 16-23|  2  |   2    |  0x10  |  0x08	|  0 |  BG
 *   4	| 32-39|  4  |   4    |  0x20  |  0x18	|  1 |  VI
 *   5  | 40-47|  5  |   5    |  0x28  |  0x20	|  1 |  VI
 *   6  | 48-55|  6  |   6    |  0x30  |  0x28	|  2 |  VO
 *   7	| 56-63|  7  |   7    |  0x38  |  0x30	|  2 |  VO
 * -----+------+-----+--------+--------+--------+----+--------
 *
 */
static void PpeSetUserPriority(void)
{
#if !defined (CONFIG_HNAT_V2)
    /* Set weight of decision in resolution */
    RegWrite(UP_RES, DFL_UP_RES);

    /* Set DSCP to User priority mapping table */
    RegWrite(DSCP0_7_MAP_UP, DFL_DSCP0_7_UP);
    RegWrite(DSCP24_31_MAP_UP, DFL_DSCP24_31_UP);
    RegWrite(DSCP8_15_MAP_UP, DFL_DSCP8_15_UP);
    RegWrite(DSCP16_23_MAP_UP, DFL_DSCP16_23_UP);
    RegWrite(DSCP32_39_MAP_UP, DFL_DSCP32_39_UP);
    RegWrite(DSCP40_47_MAP_UP, DFL_DSCP40_47_UP);
    RegWrite(DSCP48_55_MAP_UP, DFL_DSCP48_55_UP);
    RegWrite(DSCP56_63_MAP_UP, DFL_DSCP56_63_UP);

    /* Set mapping table of user priority to vlan priority */
    RegModifyBits(UP_MAP_VPRI, DFL_UP0_VPRI, 0, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP1_VPRI, 4, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP2_VPRI, 8, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP3_VPRI, 12, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP4_VPRI, 16, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP5_VPRI, 20, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP6_VPRI, 24, 3);
    RegModifyBits(UP_MAP_VPRI, DFL_UP7_VPRI, 28, 3);

    /* Set mapping table of user priority to in-profile DSCP */
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP0_IDSCP, 0, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP1_IDSCP, 8, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP2_IDSCP, 16, 6);
    RegModifyBits(UP0_3_MAP_IDSCP, DFL_UP3_IDSCP, 24, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP4_IDSCP, 0, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP5_IDSCP, 8, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP6_IDSCP, 16, 6);
    RegModifyBits(UP4_7_MAP_IDSCP, DFL_UP7_IDSCP, 24, 6);

    /* Set mapping table of user priority to out-profile DSCP */
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP0_ODSCP, 0, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP1_ODSCP, 8, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP2_ODSCP, 16, 6);
    RegModifyBits(UP0_3_MAP_ODSCP, DFL_UP3_ODSCP, 24, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP4_ODSCP, 0, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP5_ODSCP, 8, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP6_ODSCP, 16, 6);
    RegModifyBits(UP4_7_MAP_ODSCP, DFL_UP7_ODSCP, 24, 6);

    /* Set mapping table of user priority to access category */
    RegModifyBits(UP_MAP_AC, DFL_UP0_AC, 0, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP1_AC, 2, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP2_AC, 4, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP3_AC, 6, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP4_AC, 8, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP5_AC, 10, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP6_AC, 12, 2);
    RegModifyBits(UP_MAP_AC, DFL_UP7_AC, 14, 2);
#endif
}

static void PpeSetHNATProtoType(void)
{
#ifndef CONFIG_RALINK_RT3052_MP
    /* TODO: we should add exceptional case to register to point out the HNAT case here */
#endif
}

#if defined(CONFIG_RALINK_MT7620)
static void FoeFreeTbl(uint32_t NumOfEntry)
{
    uint32_t FoeTblSize;

    FoeTblSize = NumOfEntry * sizeof(struct FoeEntry);
    dma_free_coherent(NULL, FoeTblSize, PpeFoeBase, PpePhyFoeBase);
    RegWrite(PPE_FOE_BASE, 0);
}
#endif

static int32_t PpeEngStart(void)
{
	/* Set PPE Flow Set */
	PpeSetFoeEbl(1);

#if !defined (CONFIG_HNAT_V2)
    /* Set default index in policy table */
    PpeSetPreAclEbl(0);
    PpeSetPreMtrEbl(0);
    PpeSetPostMtrEbl(0);
    PpeSetPreAcEbl(0);
    PpeSetPostAcEbl(0);
#endif

    /* Set Auto Age-Out Function */
    PpeSetAgeOut();

    /* Set PPE FOE KEEPALIVE TIMER */
    PpeSetFoeKa();

    /* Set PPE FOE Bind Rate */
    PpeSetFoeBindRate(DFL_FOE_BNDR);

    /* Set PPE Global Configuration */
    PpeSetFoeGloCfgEbl(1);

    /* Set User Priority related register */
    PpeSetUserPriority();

    /* which protocol type should be handle by HNAT not HNAPT */
    PpeSetHNATProtoType();
    return 0;
}

#if defined(CONFIG_RALINK_MT7620)
static int32_t PpeEngStop(void)
{
    /* Set PPE FOE ENABLE */
    PpeSetFoeGloCfgEbl(0);

    /* Set PPE Flow Set */
    PpeSetFoeEbl(0);

#if !defined (CONFIG_HNAT_V2)
    /* Set default index in policy table */
    PpeSetPreAclEbl(0);
    PpeSetPreMtrEbl(0);
    PpeSetPostMtrEbl(0);
    PpeSetPreAcEbl(0);
    PpeSetPostAcEbl(0);
#endif

    /* Free FOE table */
    FoeFreeTbl(FOE_4TB_SIZ);

    return 0;
}
#endif

struct net_device *ra_dev_get_by_name(const char *name)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
    return dev_get_by_name(&init_net, name);
#else
    return dev_get_by_name(name);
#endif
}

static void PpeSetDstPort(uint32_t Ebl)
{
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	int i = 0;
#endif
    if (Ebl) {
#if !defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
#if defined (CONFIG_RA_HW_NAT_WIFI)
	DstPort[DP_RA0] = ra_dev_get_by_name("ra0");
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RTPCI_AP_MBSS) || defined (CONFIG_MBSS_SUPPORT)
	DstPort[DP_RA1] = ra_dev_get_by_name("ra1");
	DstPort[DP_RA2] = ra_dev_get_by_name("ra2");
	DstPort[DP_RA3] = ra_dev_get_by_name("ra3");
	DstPort[DP_RA4] = ra_dev_get_by_name("ra4");
	DstPort[DP_RA5] = ra_dev_get_by_name("ra5");
	DstPort[DP_RA6] = ra_dev_get_by_name("ra6");
	DstPort[DP_RA7] = ra_dev_get_by_name("ra7");
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || \
	defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7620) || \
	defined (CONFIG_MBSS_SUPPORT)
	DstPort[DP_RA8] = ra_dev_get_by_name("ra8");
	DstPort[DP_RA9] = ra_dev_get_by_name("ra9");
	DstPort[DP_RA10] = ra_dev_get_by_name("ra10");
	DstPort[DP_RA11] = ra_dev_get_by_name("ra11");
	DstPort[DP_RA12] = ra_dev_get_by_name("ra12");
	DstPort[DP_RA13] = ra_dev_get_by_name("ra13");
	DstPort[DP_RA14] = ra_dev_get_by_name("ra14");
	DstPort[DP_RA15] = ra_dev_get_by_name("ra15");
#endif
#endif
#if defined (CONFIG_RT2860V2_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	DstPort[DP_WDS0] = ra_dev_get_by_name("wds0");
	DstPort[DP_WDS1] = ra_dev_get_by_name("wds1");
	DstPort[DP_WDS2] = ra_dev_get_by_name("wds2");
	DstPort[DP_WDS3] = ra_dev_get_by_name("wds3");
#endif
#if defined (CONFIG_RT2860V2_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	DstPort[DP_APCLI0] = ra_dev_get_by_name("apcli0");
#endif
#if defined (CONFIG_RT2860V2_AP_MESH)
	DstPort[DP_MESH0] = ra_dev_get_by_name("mesh0");
#endif
#if defined (CONFIG_RTDEV_MII) || defined (CONFIG_RTDEV_USB) || \
	defined (CONFIG_RTDEV_PCI) || defined (CONFIG_RTDEV)
	DstPort[DP_RAI0] = ra_dev_get_by_name("rai0");
#if defined (CONFIG_RT3090_AP_MBSS) || defined (CONFIG_RT5392_AP_MBSS) || \
	defined (CONFIG_RT3572_AP_MBSS) || defined (CONFIG_RT5572_AP_MBSS) || \
	defined (CONFIG_RT5592_AP_MBSS) || defined (CONFIG_RT3593_AP_MBSS) || \
	defined (CONFIG_RTPCI_AP_MBSS) || defined (CONFIG_MT7610_AP_MBSS) || \
	defined (CONFIG_MBSS_SUPPORT)
	DstPort[DP_RAI1] = ra_dev_get_by_name("rai1");
	DstPort[DP_RAI2] = ra_dev_get_by_name("rai2");
	DstPort[DP_RAI3] = ra_dev_get_by_name("rai3");
	DstPort[DP_RAI4] = ra_dev_get_by_name("rai4");
	DstPort[DP_RAI5] = ra_dev_get_by_name("rai5");
	DstPort[DP_RAI6] = ra_dev_get_by_name("rai6");
	DstPort[DP_RAI7] = ra_dev_get_by_name("rai7");
	DstPort[DP_RAI8] = ra_dev_get_by_name("rai8");
	DstPort[DP_RAI9] = ra_dev_get_by_name("rai9");
	DstPort[DP_RAI10] = ra_dev_get_by_name("rai10");
	DstPort[DP_RAI11] = ra_dev_get_by_name("rai11");
	DstPort[DP_RAI12] = ra_dev_get_by_name("rai12");
	DstPort[DP_RAI13] = ra_dev_get_by_name("rai13");
	DstPort[DP_RAI14] = ra_dev_get_by_name("rai14");
	DstPort[DP_RAI15] = ra_dev_get_by_name("rai15");
#endif // CONFIG_RTDEV_AP_MBSS //
#endif // CONFIG_RTDEV_MII || CONFIG_RTDEV_USB || CONFIG_RTDEV_PCI
#if defined (CONFIG_RT3090_AP_APCLI) || defined (CONFIG_RT5392_AP_APCLI) || \
	defined (CONFIG_RT3572_AP_APCLI) || defined (CONFIG_RT5572_AP_APCLI) || \
	defined (CONFIG_RT5592_AP_APCLI) || defined (CONFIG_RT3593_AP_APCLI) || \
	defined (CONFIG_MT7610_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	DstPort[DP_APCLII0] = ra_dev_get_by_name("apclii0");
#endif // CONFIG_RTDEV_AP_APCLI //
#if defined (CONFIG_RT3090_AP_WDS) || defined (CONFIG_RT5392_AP_WDS) || \
    defined (CONFIG_RT3572_AP_WDS) || defined (CONFIG_RT5572_AP_WDS) || \
    defined (CONFIG_RT5592_AP_WDS) || defined (CONFIG_RT3593_AP_WDS) || \
    defined (CONFIG_MT7610_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	DstPort[DP_WDSI0] = ra_dev_get_by_name("wdsi0");
	DstPort[DP_WDSI1] = ra_dev_get_by_name("wdsi1");
	DstPort[DP_WDSI2] = ra_dev_get_by_name("wdsi2");
	DstPort[DP_WDSI3] = ra_dev_get_by_name("wdsi3");
#endif

#if defined (CONFIG_RT3090_AP_MESH) || defined (CONFIG_RT5392_AP_MESH) || \
	defined (CONFIG_RT3572_AP_MESH) || defined (CONFIG_RT5572_AP_MESH) || \
	defined (CONFIG_RT5592_AP_MESH) || defined (CONFIG_RT3593_AP_MESH) || \
	defined (CONFIG_MT7610_AP_MESH)
	DstPort[DP_MESHI0] = ra_dev_get_by_name("meshi0");
#endif // CONFIG_RTDEV_AP_MESH //
#endif // CONFIG_RA_HW_NAT_WIFI //
#endif // !CONFIG_RA_HW_NAT_WIFI_NEW_ARCH

#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	struct net_device *dev;
#if defined (CONFIG_SUPPORT_OPENWRT)
	dev=ra_dev_get_by_name("eth0");
#else
	dev=ra_dev_get_by_name("eth2");
#endif

	if ( dev != NULL ) {
		dev_put(dev);
		PpeDevRegHandler(dev);
		for(i=0; i<MAX_IF_NUM; i++) {
			if(DstPort[i] == dev) {
				printk("%s ifindex = %x\n", dev->name, i);
				DPORT_GMAC1 = i;
				break;
			}
		}
		if(i == MAX_IF_NUM) {
			printk("***********FAIL to register %s, full now\n", dev->name);
		}
	}
#ifdef CONFIG_RAETH_GMAC2
#if defined (CONFIG_SUPPORT_OPENWRT)
	dev=ra_dev_get_by_name("eth1");
#else
	dev=ra_dev_get_by_name("eth3");
#endif

	if ( dev != NULL ) {
		dev_put(dev);
		PpeDevRegHandler(dev);
		for(i=0; i<MAX_IF_NUM; i++) {
			if(DstPort[i] == dev) {
				printk("%s ifindex = %x\n", dev->name, i);
				DPORT_GMAC2 = i;
				break;
			}
		}
		if(i == MAX_IF_NUM) {
			printk("***********FAIL to register %s, full now\n", dev->name);
		}
	}
#endif
	
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
#if defined (CONFIG_SUPPORT_OPENWRT)
	dev=ra_dev_get_by_name("pci0");
#else
	dev=ra_dev_get_by_name("eth0");
#endif

	if ( dev != NULL ) {
		dev_put(dev);
		PpeDevRegHandler(dev);
		for(i=0; i<MAX_IF_NUM; i++) {
			if(DstPort[i] == dev) {
				printk("%s ifindex = %x\n", dev->name, i);
				break;
			}
		}
		if(i == MAX_IF_NUM) {
			printk("***********FAIL to register %s, full now\n", dev->name);
		}
	}

#if defined (CONFIG_SUPPORT_OPENWRT)
	dev=ra_dev_get_by_name("wwan0");
#else
	dev=ra_dev_get_by_name("wwan0");
#endif

	if ( dev != NULL ) {
		dev_put(dev);
		PpeDevRegHandler(dev);
		for(i=0; i<MAX_IF_NUM; i++) {
			if(DstPort[i] == dev) {
				printk("%s ifindex = %x\n", dev->name, i);
				break;
			}
		}
		if(i == MAX_IF_NUM) {
			printk("***********FAIL to register %s, full now\n", dev->name);
		}
	}


        dev=ra_dev_get_by_name("wwan1");
        if ( dev != NULL ) {
                dev_put(dev);
                PpeDevRegHandler(dev);
                for(i=0; i<MAX_IF_NUM; i++) {
                        if(DstPort[i] == dev) {
                                printk("%s ifindex = %x\n", dev->name, i);
                                break;
                        }
                }
                if(i == MAX_IF_NUM) {
                        printk("***********FAIL to register %s, full now\n", dev->name);
                }
        }

        dev=ra_dev_get_by_name("wwan2");
        if ( dev != NULL ) {
                dev_put(dev);
                PpeDevRegHandler(dev);
                for(i=0; i<MAX_IF_NUM; i++) {
                        if(DstPort[i] == dev) {
                                printk("%s ifindex = %x\n", dev->name, i);
                                break;
                        }
                }
                if(i == MAX_IF_NUM) {
                        printk("***********FAIL to register %s, full now\n", dev->name);
                }
        }

        dev=ra_dev_get_by_name("wwan3");
        if ( dev != NULL ) {
                dev_put(dev);
                PpeDevRegHandler(dev);
                for(i=0; i<MAX_IF_NUM; i++) {
                        if(DstPort[i] == dev) {
                                printk("%s ifindex = %x\n", dev->name, i);
                                break;
                        }
                }
                if(i == MAX_IF_NUM) {
                        printk("***********FAIL to register %s, full now\n", dev->name);
                }
        }

#endif // CONFIG_RA_HW_NAT_NIC_USB //

#else
#if defined (CONFIG_SUPPORT_OPENWRT)
	DstPort[DP_GMAC1] = ra_dev_get_by_name("eth0");
#ifdef CONFIG_RAETH_GMAC2
	DstPort[DP_GMAC2] = ra_dev_get_by_name("eth1");
#endif
#else
	DstPort[DP_GMAC1] = ra_dev_get_by_name("eth2");
#ifdef CONFIG_RAETH_GMAC2
	DstPort[DP_GMAC2] = ra_dev_get_by_name("eth3");
#endif
#endif

#if defined (CONFIG_RA_HW_NAT_NIC_USB)
#if defined (CONFIG_SUPPORT_OPENWRT)
	DstPort[DP_PCI] = ra_dev_get_by_name("pci0");	// PCI interface name
	DstPort[DP_USB] = ra_dev_get_by_name("wwan0");	// USB interface name
#else
	DstPort[DP_PCI] = ra_dev_get_by_name("pci0");	// PCI interface name
	DstPort[DP_USB] = ra_dev_get_by_name("wwan0");	// USB interface name
#endif
#endif // CONFIG_RA_HW_NAT_NIC_USB //
#endif //CONFIG_RA_HW_NAT_WIFI_NEW_ARCH
    } else {
    	
    	
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)   	

	int j = 0;
	for (j=0; j<MAX_IF_NUM; j++){
		if(DstPort[j] != NULL){
			DstPort[j] = NULL;
		}
	}
#endif  	
    
    	
#if !defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
#if defined (CONFIG_RA_HW_NAT_WIFI)
	if (DstPort[DP_RA0] != NULL) {
	    dev_put(DstPort[DP_RA0]);
	}
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RTPCI_AP_MBSS) || \
    defined (CONFIG_MBSS_SUPPORT)
	if (DstPort[DP_RA1] != NULL) {
	    dev_put(DstPort[DP_RA1]);
	}
	if (DstPort[DP_RA2] != NULL) {
	    dev_put(DstPort[DP_RA2]);
	}
	if (DstPort[DP_RA3] != NULL) {
	    dev_put(DstPort[DP_RA3]);
	}
	if (DstPort[DP_RA4] != NULL) {
	    dev_put(DstPort[DP_RA4]);
	}
	if (DstPort[DP_RA5] != NULL) {
	    dev_put(DstPort[DP_RA5]);
	}
	if (DstPort[DP_RA6] != NULL) {
	    dev_put(DstPort[DP_RA6]);
	}
	if (DstPort[DP_RA7] != NULL) {
	    dev_put(DstPort[DP_RA7]);
	}
#if defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT3352) || \
	defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7620) || \
	defined (CONFIG_MBSS_SUPPORT)
	
	if (DstPort[DP_RA8] != NULL) {
	    dev_put(DstPort[DP_RA8]);
	}
	if (DstPort[DP_RA9] != NULL) {
	    dev_put(DstPort[DP_RA9]);
	}
	if (DstPort[DP_RA10] != NULL) {
	    dev_put(DstPort[DP_RA10]);
	}
	if (DstPort[DP_RA11] != NULL) {
	    dev_put(DstPort[DP_RA11]);
	}
	if (DstPort[DP_RA12] != NULL) {
	    dev_put(DstPort[DP_RA12]);
	}
	if (DstPort[DP_RA13] != NULL) {
	    dev_put(DstPort[DP_RA13]);
	}
	if (DstPort[DP_RA14] != NULL) {
	    dev_put(DstPort[DP_RA14]);
	}
	if (DstPort[DP_RA15] != NULL) {
	    dev_put(DstPort[DP_RA15]);
	}
#endif
#endif
#if defined (CONFIG_RT2860V2_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	if (DstPort[DP_WDS0] != NULL) {
	    dev_put(DstPort[DP_WDS0]);
	}
	if (DstPort[DP_WDS1] != NULL) {
	    dev_put(DstPort[DP_WDS1]);
	}
	if (DstPort[DP_WDS2] != NULL) {
	    dev_put(DstPort[DP_WDS2]);
	}
	if (DstPort[DP_WDS3] != NULL) {
	    dev_put(DstPort[DP_WDS3]);
	}
#endif
#if defined (CONFIG_RT2860V2_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	if (DstPort[DP_APCLI0] != NULL) {
	    dev_put(DstPort[DP_APCLI0]);
	}
#endif
#if defined (CONFIG_RT2860V2_AP_MESH)
	if (DstPort[DP_MESH0] != NULL) {
	    dev_put(DstPort[DP_MESH0]);
	}
#endif
#if defined (CONFIG_RTDEV_MII) || defined (CONFIG_RTDEV_USB) || \
	defined (CONFIG_RTDEV_PCI) || defined (CONFIG_RTDEV)
	if (DstPort[DP_RAI0] != NULL) {
	    dev_put(DstPort[DP_RAI0]);
	}
#if defined (CONFIG_RT3090_AP_MBSS) || defined (CONFIG_RT5392_AP_MBSS) || \
	defined (CONFIG_RT3572_AP_MBSS) || defined (CONFIG_RT5572_AP_MBSS) || \
	defined (CONFIG_RT5592_AP_MBSS) || defined (CONFIG_RT3593_AP_MBSS) || \
	defined (CONFIG_MT7610_AP_MBSS) || defined (CONFIG_RTPCI_AP_MBSS)  || \
	defined (CONFIG_MBSS_SUPPORT)
	if (DstPort[DP_RAI1] != NULL) {
	    dev_put(DstPort[DP_RAI1]);
	}
	if (DstPort[DP_RAI2] != NULL) {
	    dev_put(DstPort[DP_RAI2]);
	}
	if (DstPort[DP_RAI3] != NULL) {
	    dev_put(DstPort[DP_RAI3]);
	}
	if (DstPort[DP_RAI4] != NULL) {
	    dev_put(DstPort[DP_RAI4]);
	}
	if (DstPort[DP_RAI5] != NULL) {
	    dev_put(DstPort[DP_RAI5]);
	}
	if (DstPort[DP_RAI6] != NULL) {
	    dev_put(DstPort[DP_RAI6]);
	}
	if (DstPort[DP_RAI7] != NULL) {
	    dev_put(DstPort[DP_RAI7]);
	}
	if (DstPort[DP_RAI8] != NULL) {
	    dev_put(DstPort[DP_RAI8]);
	}
	if (DstPort[DP_RAI9] != NULL) {
	    dev_put(DstPort[DP_RAI9]);
	}
	if (DstPort[DP_RAI10] != NULL) {
	    dev_put(DstPort[DP_RAI10]);
	}
	if (DstPort[DP_RAI11] != NULL) {
	    dev_put(DstPort[DP_RAI11]);
	}
	if (DstPort[DP_RAI12] != NULL) {
	    dev_put(DstPort[DP_RAI12]);
	}
	if (DstPort[DP_RAI13] != NULL) {
	    dev_put(DstPort[DP_RAI13]);
	}
	if (DstPort[DP_RAI14] != NULL) {
	    dev_put(DstPort[DP_RAI14]);
	}
	if (DstPort[DP_RAI15] != NULL) {
	    dev_put(DstPort[DP_RAI15]);
	}
#endif // CONFIG_RTDEV_AP_MBSS //
#endif // CONFIG_RTDEV_MII || CONFIG_RTDEV_USB || CONFIG_RTDEV_PCI
#if defined (CONFIG_RT3090_AP_APCLI) || defined (CONFIG_RT5392_AP_APCLI) || \
	defined (CONFIG_RT3572_AP_APCLI) || defined (CONFIG_RT5572_AP_APCLI) || \
	defined (CONFIG_RT5592_AP_APCLI) || defined (CONFIG_RT3593_AP_APCLI) || \
	defined (CONFIG_MT7610_AP_APCLI) || defined (CONFIG_APCLI_SUPPORT)
	if (DstPort[DP_APCLII0] != NULL) {
	    dev_put(DstPort[DP_APCLII0]);
	}
#endif // CONFIG_RTDEV_AP_APCLI //
#if defined (CONFIG_RT3090_AP_WDS) || defined (CONFIG_RT5392_AP_WDS) || \
    defined (CONFIG_RT3572_AP_WDS) || defined (CONFIG_RT5572_AP_WDS) || \
    defined (CONFIG_RT5592_AP_WDS) || defined (CONFIG_RT3593_AP_WDS) || \
    defined (CONFIG_MT7610_AP_WDS) || defined (CONFIG_WDS_SUPPORT)
	if (DstPort[DP_WDSI0] != NULL) {
	    dev_put(DstPort[DP_WDSI0]);
	}
	if (DstPort[DP_WDSI1] != NULL) {
	    dev_put(DstPort[DP_WDSI1]);
	}
	if (DstPort[DP_WDSI2] != NULL) {
	    dev_put(DstPort[DP_WDSI2]);
	}
	if (DstPort[DP_WDSI3] != NULL) {
	    dev_put(DstPort[DP_WDSI3]);
	}
#endif

#if defined (CONFIG_RT3090_AP_MESH) || defined (CONFIG_RT5392_AP_MESH) || \
	defined (CONFIG_RT3572_AP_MESH) || defined (CONFIG_RT5572_AP_MESH) || \
	defined (CONFIG_RT5592_AP_MESH) || defined (CONFIG_RT3593_AP_MESH) || \
	defined (CONFIG_MT7610_AP_MESH)

	if (DstPort[DP_MESHI0] != NULL) {
	    dev_put(DstPort[DP_MESHI0]);
	}
#endif // CONFIG_RTDEV_AP_MESH //
	if (DstPort[DP_GMAC1] != NULL) {
	    dev_put(DstPort[DP_GMAC1]);
	}
#ifdef CONFIG_RAETH_GMAC2
	if (DstPort[DP_GMAC2] != NULL) {
	    dev_put(DstPort[DP_GMAC2]);
	}
#endif
#endif // CONFIG_RA_HW_NAT_WIFI //
#if defined (CONFIG_RA_HW_NAT_NIC_USB)
	if (DstPort[DP_PCI] != NULL) {
	    dev_put(DstPort[DP_PCI]);
	}
	if (DstPort[DP_USB] != NULL) {
	    dev_put(DstPort[DP_USB]);
	}
#endif // CONFIG_RA_HW_NAT_NIC_USB //
#endif // (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
    }

}



#if defined (CONFIG_RALINK_MT7620)
static void SetAclPattern(uint16_t TP, uint8_t WORD_OFFSET, uint16_t BIT_CMP, uint16_t CMP_PAT, uint8_t INDEX)
{
	unsigned int i, value;

	value = CMP_PAT;
	value |= (BIT_CMP << 16);//compare mask

	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD1, value);
	
	printk("REG_ESW_VLAN_VAWD1 value is 0x%x\n\r", value);
	
	value = 0x3f << 8; //w_port_map
	value |= 0x1 << 19; //enable
	value |= TP << 16; //ip header
	value |= WORD_OFFSET << 1; //word offset
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD2, value);

	printk("REG_ESW_VLAN_VAWD2 value is 0x%x\n\r", value);

	value = (0x80005000 + INDEX);  //w_acl entry 0
	RegWrite(RALINK_ETH_SW_BASE  + REG_ESW_VLAN_VTCR, value);

	printk("REG_ESW_VLAN_VTCR value is 0x%x\n\r\n\r", value);

	for (i = 0; i < 20; i++) {
		value = RegRead(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VTCR);
		if ((value & 0x80000000) == 0 ){ //table busy
			break;
		}
		udelay(1000);
	}
	if (i == 20){
		printk("timeout.\n");
		printk("VTCR is 0x%x \n!!!!!", value);
	}
}


static void SetAclControl(uint32_t HIT_PAT, uint8_t RULE_NUM, uint16_t RULE_ACTION)
{
	unsigned int i, value;

	//set pattern
	value = HIT_PAT; //bit0,1
	//value |= 1;//valid
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD1, value);
	printk("REG_ESW_VLAN_VAWD1 value is 0x%x\n\r", value);
	
	value = 0; //bit32~63
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD2, value);

	printk("REG_ESW_VLAN_VAWD2 value is 0x%x\n\r", value);

	value = (0x80009000 + RULE_NUM);  //w_acl control rule  RULE_NUM
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VTCR, value);

	printk("REG_ESW_VLAN_VTCR value is 0x%x\n\r\n\r", value);

	for (i = 0; i < 20; i++) {
		value = RegRead(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VTCR);
		if ((value & 0x80000000) == 0 ){ //table busy
			break;
		}
		udelay(1000);
	}
	if (i == 20)
		printk("timeout.\n");





	//set action
	value = RULE_ACTION; //
	//value = 0x0; //default. Nodrop
	//value |= 1 << 28;//acl intterupt enable
	//value |= 1 << 27;//acl hit count
	//value |= 6 << 4;//acl UP
	//value |= 6 << 16;//eg-tag tagged
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD1, value);
	printk("RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD1 value is 0x%x\n\r", value);
	
	value = 0; //bit32~63
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VAWD2, value);
	printk("REG_ESW_VLAN_VAWD2 value is 0x%x\n\r", value);


	value = (0x8000b000 + RULE_NUM);  //w_acl rule action control RULE_NUM
	RegWrite(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VTCR, value);

	printk("REG_ESW_VLAN_VTCR value is 0x%x\n\r\n\r", value);

	for (i = 0; i < 20; i++) {
		value = RegRead(RALINK_ETH_SW_BASE + REG_ESW_VLAN_VTCR);
		if ((value & 0x80000000) == 0 ){ //table busy
			break;
		}
		udelay(1000);
	}
	if (i == 20)
		printk("timeout.\n");

}

struct net_device *LanInt;
struct net_device *WanInt;
static void SetAclFwd(uint32_t Ebl)
{
	uint16_t mac_pattern;
	unsigned int i, value;


	if(Ebl){
#if defined (CONFIG_SUPPORT_OPENWRT)
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P4)
		WanInt = ra_dev_get_by_name("eth0.5");
#else
		WanInt = ra_dev_get_by_name("eth0.1");
#endif
#else
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
		WanInt = ra_dev_get_by_name("eth1");
#else
		WanInt = ra_dev_get_by_name("eth0.2");
#endif
#endif
		LanInt = ra_dev_get_by_name("br-lan");
#else
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P4)
		WanInt = ra_dev_get_by_name("eth2.5");
#else
		WanInt = ra_dev_get_by_name("eth2.1");
#endif
#else
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
		WanInt = ra_dev_get_by_name("eth1");
#else
		WanInt = ra_dev_get_by_name("eth2.2");
#endif
#endif
		LanInt = ra_dev_get_by_name("br0");
#endif
		for(i = 0;i< 6;i++){
			value = RegRead(RALINK_ETH_SW_BASE + 0x2004 + (i * 0x100));
			value |= (0x1 << 10);
			RegWrite(RALINK_ETH_SW_BASE + 0x2004 + (i * 0x100), value);
		}
	}
	else{
		for(i = 0;i< 6;i++){
			value = RegRead(RALINK_ETH_SW_BASE + 0x2004 + (i * 0x100));
			value &= ~(0x1 << 10);
			RegWrite(RALINK_ETH_SW_BASE + 0x2004 + (i * 0x100), value);
		}
		dev_put(WanInt);
		dev_put(LanInt);
		return;
	}

	printk("LAN dev address is %2x %2x %2x %2x %2x %2x \n", LanInt->dev_addr[0], LanInt->dev_addr[1], \
		LanInt->dev_addr[2], LanInt->dev_addr[3], LanInt->dev_addr[4], LanInt->dev_addr[5]);
	printk("WAN dev address is %2x %2x %2x %2x %2x %2x \n", WanInt->dev_addr[0], WanInt->dev_addr[1], \
		WanInt->dev_addr[2], WanInt->dev_addr[3], WanInt->dev_addr[4], WanInt->dev_addr[5]);

        mac_pattern = (LanInt->dev_addr[0] << 8)| LanInt->dev_addr[1];
	SetAclPattern(0, 0, 0xffff, mac_pattern, 0);
        mac_pattern = (LanInt->dev_addr[2] << 8)| LanInt->dev_addr[3];
        SetAclPattern(0, 1, 0xffff, mac_pattern, 1);
        mac_pattern = (LanInt->dev_addr[4] << 8)| LanInt->dev_addr[5];
        SetAclPattern(0, 2, 0xffff, mac_pattern, 2);
        SetAclPattern(0, 0, 0x0300, 0x0100, 3);
        mac_pattern = (WanInt->dev_addr[0] << 8)| WanInt->dev_addr[1];
        SetAclPattern(0, 0, 0xffff, mac_pattern, 4);
        mac_pattern = (WanInt->dev_addr[2] << 8)| WanInt->dev_addr[3];
        SetAclPattern(0, 1, 0xffff, mac_pattern, 5);
        mac_pattern = (WanInt->dev_addr[4] << 8)| WanInt->dev_addr[5];
        SetAclPattern(0, 2, 0xffff, mac_pattern, 6);
 
        SetAclPattern(2, 1, 0xfffc, 0x0048, 8);
        SetAclPattern(2, 1, 0x0, 0x0, 9);
        SetAclPattern(3, 2, 0xfffc, 0x0020, 10);
        SetAclPattern(3, 2, 0x0, 0x0, 11);

	SetAclControl(0x107, 0, 0x4080);
	SetAclControl(0x207, 1, 0x8080);
	SetAclControl(0x170, 2, 0x4080);
	SetAclControl(0x270, 3, 0x8080);
	SetAclControl(0x108, 4, 0x7F80);//Multicast inc. CPU
	SetAclControl(0x208, 5, 0x8F80);//Multicast inc. PPE ??????
	SetAclControl(0x407, 6, 0x4080);
	SetAclControl(0x807, 7, 0x8080);
	SetAclControl(0x470, 8, 0x4080);
	SetAclControl(0x870, 9, 0x8080);
	SetAclControl(0x408, 0xa, 0x7F80);///Multicast inc. CPU
	SetAclControl(0x808, 0xb, 0x8F80);//Multicast inc. PPE ??????

}

#endif


uint32_t SetGdmaFwd(uint32_t Ebl)
{
#if defined (CONFIG_RALINK_MT7620)
    uint32_t data = 0;

    data = RegRead(GDM2_FWD_CFG);

    if (Ebl) {
	data &= ~0x7777;
	data |=  GDM1_OFRC_P_CPU;
	data |=  GDM1_MFRC_P_CPU;
	data |=  GDM1_BFRC_P_CPU;
	data |=  GDM1_UFRC_P_CPU;
    }else {
	data |= 0x7777;
    }

    RegWrite(GDM2_FWD_CFG, data);
#else
    uint32_t data = 0;

    data = RegRead(FE_GDMA1_FWD_CFG);

    if (Ebl) {
	data &= ~0x7777;
	//Uni-cast frames forward to PPE
	data |= GDM1_UFRC_P_PPE;
	//Broad-cast MAC address frames forward to PPE
	data |= GDM1_BFRC_P_PPE;
	//Multi-cast MAC address frames forward to PPE
	data |= GDM1_MFRC_P_PPE;
	//Other MAC address frames forward to PPE
	data |= GDM1_OFRC_P_PPE;

    } else {
	data &= ~0x7777;
	//Uni-cast frames forward to CPU
	data |= GDM1_UFRC_P_CPU;
	//Broad-cast MAC address frames forward to CPU
	data |= GDM1_BFRC_P_CPU;
	//Multi-cast MAC address frames forward to CPU
	data |= GDM1_MFRC_P_CPU;
	//Other MAC address frames forward to CPU
	data |= GDM1_OFRC_P_CPU;
    }

    RegWrite(FE_GDMA1_FWD_CFG, data);

#ifdef CONFIG_RAETH_GMAC2
    data = RegRead(FE_GDMA2_FWD_CFG);

    if (Ebl) {
	data &= ~0x7777;
	//Uni-cast frames forward to PPE
	data |= GDM1_UFRC_P_PPE;
	//Broad-cast MAC address frames forward to PPE
	data |= GDM1_BFRC_P_PPE;
	//Multi-cast MAC address frames forward to PPE
	data |= GDM1_MFRC_P_PPE;
	//Other MAC address frames forward to PPE
	data |= GDM1_OFRC_P_PPE;

    } else {
	data &= ~0x7777;
	//Uni-cast frames forward to CPU
	data |= GDM1_UFRC_P_CPU;
	//Broad-cast MAC address frames forward to CPU
	data |= GDM1_BFRC_P_CPU;
	//Multi-cast MAC address frames forward to CPU
	data |= GDM1_MFRC_P_CPU;
	//Other MAC address frames forward to CPU
	data |= GDM1_OFRC_P_CPU;
    }
    RegWrite(FE_GDMA2_FWD_CFG, data);
#endif
#endif

    return 0;
}

#if defined (CONFIG_HNAT_V2)
void PpeSetCacheEbl(void)
{
    //clear cache table before enabling cache
    RegModifyBits(CAH_CTRL, 1, 9, 1);
    RegModifyBits(CAH_CTRL, 0, 9, 1);

    /* Cache enable */
    RegModifyBits(CAH_CTRL, 1, 0, 1);
}

static void PpeSetSwVlanChk(int Ebl)
{
#if defined (CONFIG_RALINK_MT7620)
    uint32_t reg;
    /* port6&7: fall back mode / same port matrix group */
    if(Ebl){
	reg = RegRead(RALINK_ETH_SW_BASE + 0x2604);
	reg |= 0xff0003;
	RegWrite(RALINK_ETH_SW_BASE + 0x2604, reg);

	reg = RegRead(RALINK_ETH_SW_BASE + 0x2704);
	reg |= 0xff0003;
	RegWrite(RALINK_ETH_SW_BASE + 0x2704, reg);
    }else {
	reg = RegRead(RALINK_ETH_SW_BASE + 0x2604);
	reg &= ~0xff0003;
	reg |= 0xc00001;
	RegWrite(RALINK_ETH_SW_BASE + 0x2604, reg);

	reg = RegRead(RALINK_ETH_SW_BASE + 0x2704);
	reg &= ~0xff0003;
	reg |= 0xc00001;
	RegWrite(RALINK_ETH_SW_BASE + 0x2704, reg);
    }
#endif
}

static void PpeSetFpBMAP(void)
{
#if defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
    /*TODO*/
#else
    /* index 0 = force port 0 
     * index 1 = force port 1 
     * ...........
     * index 7 = force port 7
     * index 8 = no force port 
     * index 9 = force to all ports
     */
    RegWrite(PPE_FP_BMAP_0, 0x00020001);
    RegWrite(PPE_FP_BMAP_1, 0x00080004);
    RegWrite(PPE_FP_BMAP_2, 0x00200010);
    RegWrite(PPE_FP_BMAP_3, 0x00800040);
    RegWrite(PPE_FP_BMAP_4, 0x003F0000);
#endif
}

static void PpeSetIpProt(void)
{
    /* IP Protocol Field for IPv4 NAT or IPv6 3-tuple flow */
    /* Don't forget to turn on related bits in PPE_IP_PROT_CHK register if you want to support 
     * another IP protocol. 
     */
    /* FIXME: enable it to support IP fragement */
    RegWrite(PPE_IP_PROT_CHK, 0xFFFFFFFF); //IPV4_NXTH_CHK and IPV6_NXTH_CHK
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
    RegModifyBits(PPE_IP_PROT_0, IPPROTO_GRE, 0, 8);
#endif
    //	RegModifyBits(PPE_IP_PROT_0, IPPROTO_GRE, 0, 8);
    //	RegModifyBits(PPE_IP_PROT_0, IPPROTO_TCP, 8, 8);
    //	RegModifyBits(PPE_IP_PROT_0, IPPROTO_UDP, 16, 8);
    //	RegModifyBits(PPE_IP_PROT_0, IPPROTO_IPV6, 24, 8);
}
#endif

#ifdef CONFIG_RA_HW_NAT_ACCNT_MAINTAINER
void update_foe_ac_timer_handler(unsigned long unused);
DEFINE_TIMER(update_foe_ac_timer, update_foe_ac_timer_handler, 0, 0);

void update_foe_ac_timer_handler(unsigned long unused)
{
#if defined (CONFIG_RALINK_MT7620)
        ac_info[1].ag_byte_cnt += RegRead(AC_BASE + 1 * 8);      /* Low bytes */
        ac_info[1].ag_pkt_cnt  += RegRead(AC_BASE +  1 * 8 + 4); /* High bytes */
        ac_info[2].ag_byte_cnt += RegRead(AC_BASE + 2 * 8);      /* Low bytes */
        ac_info[2].ag_pkt_cnt  += RegRead(AC_BASE +  2 * 8 + 4); /* High bytes */
#elif defined (CONFIG_RALINK_MT7621) || defined (CONFIG_ARCH_MT7623)
        ac_info[1].ag_byte_cnt += RegRead(AC_BASE + 1 * 16);		  /* 64bit bytes cnt */
        ac_info[1].ag_byte_cnt += ((unsigned long long)(RegRead(AC_BASE + 1 * 16 + 4)) << 32);  /* 64bit bytes cnt */
        ac_info[1].ag_pkt_cnt  += RegRead(AC_BASE +  1 * 16 + 8);	  /* 32bites packet cnt */
        ac_info[2].ag_byte_cnt += RegRead(AC_BASE + 2 * 16);		  /* 64bit bytes cnt */
        ac_info[2].ag_byte_cnt += ((unsigned long long)(RegRead(AC_BASE + 2 * 16 + 4)) << 32);  /* 64bit bytes cnt */
        ac_info[2].ag_pkt_cnt  += RegRead(AC_BASE +  2 * 16 + 8);	  /* 32bites packet cnt */
#endif

    update_foe_ac_timer.expires = jiffies + 16 * HZ;
    add_timer(&update_foe_ac_timer);
}
#endif

void foe_ac_update_ebl(int ebl)
{
#ifdef CONFIG_RA_HW_NAT_ACCNT_MAINTAINER
    if(ebl) {
	update_foe_ac_timer.expires = jiffies + HZ;
	add_timer(&update_foe_ac_timer);
    } else {
	if(timer_pending(&update_foe_ac_timer)) {
	    del_timer_sync(&update_foe_ac_timer);
	}
    }
#endif
}
void foe_clear_entry(struct neighbour *neigh)
{
	int hash_index, clear;
	struct FoeEntry *entry;
        u32 *daddr = (u32 *)neigh->primary_key;
	const u8 *addrtmp;
	u8 mac0,mac1,mac2,mac3,mac4,mac5;
	u32 dip;
	dip = (u32)(*daddr);
	clear = 0;
	addrtmp = neigh->ha;
	mac0 = (u8)(*addrtmp);
	mac1 = (u8)(*(addrtmp+1));
	mac2 = (u8)(*(addrtmp+2));
	mac3 = (u8)(*(addrtmp+3));
	mac4 = (u8)(*(addrtmp+4));
	mac5 = (u8)(*(addrtmp+5));

        for (hash_index = 0; hash_index < FOE_4TB_SIZ; hash_index++) {
		entry = &PpeFoeBase[hash_index];
		if(entry->bfib1.state == BIND) {
			if (DebugLevel >= 1) {
				printk("before old mac= %x:%x:%x:%x:%x:%x, new_dip=%x\n",
					entry->ipv4_hnapt.dmac_hi[3],
					entry->ipv4_hnapt.dmac_hi[2],
					entry->ipv4_hnapt.dmac_hi[1],
					entry->ipv4_hnapt.dmac_hi[0],
					entry->ipv4_hnapt.dmac_lo[1],
					entry->ipv4_hnapt.dmac_lo[0], entry->ipv4_hnapt.new_dip);
			}
			if (entry->ipv4_hnapt.new_dip == ntohl(dip)) {
				if ((entry->ipv4_hnapt.dmac_hi[3] != mac0) ||
				    (entry->ipv4_hnapt.dmac_hi[2] != mac1) ||
				    (entry->ipv4_hnapt.dmac_hi[1] != mac2) ||
				    (entry->ipv4_hnapt.dmac_hi[0] != mac3) ||
				    (entry->ipv4_hnapt.dmac_lo[1] != mac4) ||
				    (entry->ipv4_hnapt.dmac_lo[0] != mac5)) {
				    	printk("%s: state=%d\n",__func__,neigh->nud_state);
				    	RegModifyBits(PPE_FOE_CFG, ONLY_FWD_CPU, 4, 2);
				  	entry->ipv4_hnapt.udib1.state = INVALID;
					entry->ipv4_hnapt.udib1.time_stamp = RegRead(FOE_TS) & 0xFF;
					PpeSetCacheEbl();
					mod_timer(&hwnat_clear_entry_timer, jiffies + 3 * HZ);
					printk("delete old entry: dip =%x\n", ntohl(dip));
				    	printk("old mac= %x:%x:%x:%x:%x:%x, dip=%x\n",
				    		entry->ipv4_hnapt.dmac_hi[3],
				    		entry->ipv4_hnapt.dmac_hi[2],
				    		entry->ipv4_hnapt.dmac_hi[1],
				    		entry->ipv4_hnapt.dmac_hi[0],
				    		entry->ipv4_hnapt.dmac_lo[1],
				    		entry->ipv4_hnapt.dmac_lo[0],
				    		ntohl(dip));
				    	printk("new mac= %x:%x:%x:%x:%x:%x, dip=%x\n", mac0, mac1, mac2, mac3, mac4, mac5, ntohl(dip));

				}
			}
		}
	}
}
static int wh2_netevent_handler(struct notifier_block *unused,
                                unsigned long event, void *ptr)
{
        struct net_device *dev = NULL;
        struct neighbour *neigh = NULL;
        int err = 0;

        switch (event) {
        case NETEVENT_NEIGH_UPDATE:
                neigh = ptr;
                dev = neigh->dev;
                if (dev)
			foe_clear_entry(neigh);
                if (err)
                        printk("failed to handle neigh update (err %d)\n", err);
                break;
        }

        return NOTIFY_DONE;
}
static struct notifier_block Hnat_netevent_nb __read_mostly = {
        .notifier_call = wh2_netevent_handler,
};
/*
 * PPE Enabled: GMAC<->PPE<->CPU
 * PPE Disabled: GMAC<->CPU
 */
static int32_t PpeInitMod(void)
{
    NAT_PRINT("Ralink HW NAT Module Enabled\n");
    
    /* Set PPE FOE Hash Mode */
    if(!PpeSetFoeHashMode(DFL_FOE_HASH_MODE)) {
	return -ENOMEM; //memory allocation failed
    }

    //Get net_device structure of Dest Port 
    PpeSetDstPort(1);

    /* Register ioctl handler */
    PpeRegIoctlHandler();

#if !defined (CONFIG_HNAT_V2)
    PpeSetRuleSize(PRE_ACL_SIZE, PRE_MTR_SIZE, PRE_AC_SIZE,
	    POST_MTR_SIZE, POST_AC_SIZE);
    AclRegIoctlHandler();
    AcRegIoctlHandler();
    MtrRegIoctlHandler();

    /* 0~63 Accounting group */
    PpeSetAGInfo(1, 1);	//AG Index1=VLAN1
    PpeSetAGInfo(2, 2);	//AG Index2=VLAN2
#else
    PpeSetFpBMAP();
    PpeSetIpProt();
    PpeSetCacheEbl();
    PpeSetSwVlanChk(0);
    foe_ac_update_ebl(1);

    /* 0~63 Metering group */
    //PpeSetMtrByteInfo(1, 500, 3); //TokenRate=500=500KB/s, MaxBkSize= 3 (32K-1B)
    //PpeSetMtrPktInfo(1, 5, 3);  //1 pkts/sec, MaxBkSize=3 (32K-1B)
#endif


    /* Initialize PPE related register */
    PpeEngStart();

    /* In manual mode, PPE always reports UN-HIT CPU reason, so we don't need to process it */
    /* Register RX/TX hook point */
    ra_sw_nat_hook_tx = PpeTxHandler;
    ra_sw_nat_hook_rx = PpeRxHandler;
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
    ppe_dev_register_hook = PpeDevRegHandler;
    ppe_dev_unregister_hook = PpeDevUnRegHandler;
#endif
    /* Set GMAC fowrards packet to PPE */
#if defined (CONFIG_RALINK_MT7620)
    if ((RegRead(0xB000000C) & 0xf) < 0x5) {
	
	SetAclFwd(1);
	
    } else {
	uint32_t reg;

	/* Turn On UDP Control */
	reg = RegRead(0xB0100F80);
	reg &= ~(0x1 << 30);
	RegWrite(0xB0100F80, reg);
    }
#endif
    SetGdmaFwd(1);

#if 0
    hnat_class = class_create(THIS_MODULE, "hnat");
    device_create(hnat_class, NULL, MKDEV(220, 0), NULL, "hwnat0");
    device_create(hnat_class, NULL, MKDEV(230, 0), NULL, "acl0");
    device_create(hnat_class, NULL, MKDEV(240, 0), NULL, "ac0");
    device_create(hnat_class, NULL, MKDEV(250, 0), NULL, "mtr0");
#endif

#if defined (CONFIG_RALINK_MT7620)
/*For FastPath disable crc check*/
#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	RegWrite(0xb0100600, 0x20010000);
	RegWrite(0xb0100d00, 0x0);
#endif
#endif

#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	HnatPptpL2tpInit();
#endif
	register_netevent_notifier(&Hnat_netevent_nb);
	init_timer(&hwnat_clear_entry_timer);
	hwnat_clear_entry_timer.function = hwnat_clear_entry;
	return 0;
}

static void PpeCleanupMod(void)
{

#if defined  (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	uint32_t i = 0;
	for (i=0; i<MAX_IF_NUM; i++){
		if(DstPort[i] != NULL)
			DstPort[i] = NULL;
	}

#endif// (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)
	NAT_PRINT("Ralink HW NAT Module Disabled\n");
    /* Set GMAC fowrards packet to CPU */
#if defined (CONFIG_RALINK_MT7620)
    if ((RegRead(0xB000000C) & 0xf) < 0x5) {
        SetAclFwd(0);
    }
#endif
    SetGdmaFwd(0);

    /* Unregister RX/TX hook point */
    ra_sw_nat_hook_rx = NULL;
    ra_sw_nat_hook_tx = NULL;
#if defined (CONFIG_RA_HW_NAT_WIFI_NEW_ARCH)    
    ppe_dev_register_hook = NULL;
    ppe_dev_unregister_hook = NULL;
#endif
    /* Restore PPE related register */
#if defined(CONFIG_RALINK_MT7620)
    PpeEngStop();
#endif
    iounmap(PpeFoeBase);

    /* Unregister ioctl handler */
    PpeUnRegIoctlHandler();
#if !defined (CONFIG_HNAT_V2)
    AclUnRegIoctlHandler();
    AcUnRegIoctlHandler();
    MtrUnRegIoctlHandler();
#else
    PpeSetSwVlanChk(1);
    foe_ac_update_ebl(0);
#endif
#if defined (CONFIG_RAETH_QDMA) && defined  (CONFIG_PPE_MCAST)
    foe_mcast_entry_del_all();
#endif

    //Release net_device structure of Dest Port 
    PpeSetDstPort(0);

#if 0
    /* Restore switch port 6 flow control to default on */
    RegModifyBits(RALINK_ETH_SW_BASE + 0xC8, 0x3, 8, 2);
#endif

#if 0
    device_destroy(hnat_class, MKDEV(220, 0));
    device_destroy(hnat_class, MKDEV(230, 0));
    device_destroy(hnat_class, MKDEV(240, 0));
    device_destroy(hnat_class, MKDEV(250, 0));
    class_destroy(hnat_class);
#endif

#if defined (CONFIG_RA_HW_NAT_PPTP_L2TP) 
	HnatPptpL2tpClean();
#endif
    unregister_netevent_notifier(&Hnat_netevent_nb);
}

module_init(PpeInitMod);
module_exit(PpeCleanupMod);

MODULE_AUTHOR("Steven Liu/Kurtis Ke");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ralink Hardware NAT v2.50\n");
