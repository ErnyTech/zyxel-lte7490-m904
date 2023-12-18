/* Kernel module to match the port-ranges, trigger related port-ranges,
  * and alters the destination to a local IP address.
  *
  * Copyright (C) 2003, CyberTAN Corporation
  * All Rights Reserved.
  *
  * Description:
  *   This is kernel module for port-triggering.
  *
  *   The module follows the Netfilter framework, called extended packet
  *   matching modules.
  */

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>

#include <linux/netfilter_ipv4.h>
#ifdef CONFIG_NF_NAT_NEEDED
#include <net/netfilter/nf_nat_rule.h>
#else
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#endif
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

DEFINE_RWLOCK(ip_conntrack_lock);

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

#ifdef CONFIG_NETFILTER_DEBUG
#define IP_NF_ASSERT(x)		WARN_ON(!(x))
#else
#define IP_NF_ASSERT(x)
#endif

struct ipt_trigger {
        struct list_head list;          /* Trigger list */
        struct timer_list timeout;      /* Timer for list destroying */
        u_int32_t srcip;                /* Outgoing source address */
        u_int32_t dstip;                /* Outgoing destination address */
        u_int16_t mproto;               /* Trigger protocol */
        u_int16_t rproto;               /* Related protocol */
        struct ipt_trigger_ports ports; /* Trigger and related ports */
       u_int8_t reply;                 /* Confirm a reply connection */
};

LIST_HEAD(trigger_list);
//DECLARE_LOCK(ip_trigger_lock);

static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
   DEBUGP("%s: \n", __FUNCTION__);
    IP_NF_ASSERT(trig);
	  write_lock_bh(&ip_conntrack_lock);
    /* Need del_timer for race avoidance (may already be dying). */
     if (del_timer(&trig->timeout)) {
        trig->timeout.expires = jiffies + extra_jiffies;
        add_timer(&trig->timeout);
    }

		write_unlock_bh(&ip_conntrack_lock);
}

static void __del_trigger(struct ipt_trigger *trig)
{
    DEBUGP("%s: \n", __FUNCTION__);
    IP_NF_ASSERT(trig);

     /* delete from 'trigger_list' */
    list_del(&trig->list);
    kfree(trig);
}

static void trigger_timeout(unsigned long ul_trig)
{
    struct ipt_trigger *trig= (void *) ul_trig;

    DEBUGP("trigger list %p timed out\n", trig);
		write_lock_bh(&ip_conntrack_lock);
    __del_trigger(trig);
	  write_unlock_bh(&ip_conntrack_lock);
}

static unsigned int
add_new_trigger(struct ipt_trigger *trig)
{
    struct ipt_trigger *new;

    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__);
	  write_lock_bh(&ip_conntrack_lock);
    new = (struct ipt_trigger *)kmalloc(sizeof(struct ipt_trigger), GFP_ATOMIC);

    if (!new) {
		  write_unlock_bh(&ip_conntrack_lock);
        DEBUGP("%s: OOM allocating trigger list\n", __FUNCTION__);
        return -ENOMEM;
    }
    memset(new, 0, sizeof(*trig));
    INIT_LIST_HEAD(&new->list);
    memcpy(new, trig, sizeof(*trig));

    /* add to global table of trigger */
	list_add (&new->list, &trigger_list);
    /* add and start timer if required */
    init_timer(&new->timeout);
    new->timeout.data = (unsigned long)new;
    new->timeout.function = trigger_timeout;
    new->timeout.expires = jiffies + (TRIGGER_TIMEOUT * HZ);
    add_timer(&new->timeout);

 	write_unlock_bh(&ip_conntrack_lock);
	return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport)
{
     DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
    DEBUGP("%s: Got one, mproto= %d, mport[0..1]=%d, %d.\n", __FUNCTION__,
            i->mproto, i->ports.mport[0], i->ports.mport[1]);

    return ((i->mproto == proto) && (i->ports.mport[0] <= dport)
            && (i->ports.mport[1] >= dport));
}

static unsigned int
trigger_out(struct sk_buff *skb,
                unsigned int hooknum,
                const struct net_device *in,
                const struct net_device *out,
                const void *targinfo)
{
    const struct ipt_trigger_info *info = targinfo;
    struct ipt_trigger trig, *found;
    const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */

    DEBUGP("############# %s ############\n", __FUNCTION__);
    /* Check if the trigger range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list){
		if (trigger_out_matched(found, iph->protocol,ntohs(tcph->dest))) {
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
        /* In order to allow multiple hosts use the same port range, we update
           the 'saddr' after previous trigger has a reply connection. */
        if (found->reply)
            found->srcip = iph->saddr;
		return XT_CONTINUE;        /* We don't block any packet. */
		}
	}
	DEBUGP("############# %s ############  not found the entry\n", __FUNCTION__);
		/* Create new trigger */
		memset(&trig, 0, sizeof(trig));
		trig.srcip = iph->saddr;
		trig.mproto = iph->protocol;
		trig.rproto = info->proto;
		memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));
		add_new_trigger(&trig); /* Add the new 'trig' to list 'trigger_list'. */
	return XT_CONTINUE;        /* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i,
        const u_int16_t proto, const u_int16_t dport)
{
    u_int16_t rproto = i->rproto;

 DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
 DEBUGP("%s: Got one, rproto= %d, rport[0..1]=%d, %d.\n", __FUNCTION__,
		i->rproto, i->ports.rport[0], i->ports.rport[1]);
    if (!rproto)
        rproto = proto;

    return ((rproto == proto) && (i->ports.rport[0] <= dport)
            && (i->ports.rport[1] >= dport));
}

static unsigned int
trigger_in(struct sk_buff *skb,
                unsigned int hooknum,
                const struct net_device *in,
                const struct net_device *out,
                const void *targinfo)
{
    struct ipt_trigger *found;
    const struct iphdr *iph = ip_hdr(skb);/*(skb)->nh.iph; */
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */
    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list){
	        if (trigger_in_matched(found, iph->protocol,ntohs(tcph->dest))) {
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
        trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
        return NF_ACCEPT;       /* Accept it, or the imcoming packet could be
                                   dropped in the FORWARD chain */
		}
	}
    return XT_CONTINUE;        /* Our job is the interception. */
}

 #define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

 #define NIPQUAD_FMT "%u.%u.%u.%u"
static unsigned int
trigger_dnat(struct sk_buff *skb,
               unsigned int hooknum,
                const struct net_device *in,
                const struct net_device *out,
                const void *targinfo)
{
    struct ipt_trigger *found;
    const struct iphdr *iph = ip_hdr(skb);/*(skb)->nh.iph; */
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;     /* Might be TCP, UDP */
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;
    const struct nf_nat_ipv4_multi_range_compat *mr=targinfo;
	struct nf_nat_ipv4_range newrange;

    IP_NF_ASSERT(hooknum == NF_IP_PRE_ROUTING);
    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list){
		if (trigger_in_matched(found, iph->protocol,ntohs(tcph->dest))) {
			if (!found->srcip)
				return XT_CONTINUE;
    found->reply = 1;   /* Confirm there has been a reply connection. */
    ct = nf_ct_get(skb, &ctinfo);
    IP_NF_ASSERT(ct && (ctinfo == IP_CT_NEW));

    DEBUGP("%s: got ", __FUNCTION__);

	/* Alter the destination of incoming packet. */
 	newrange= ((struct nf_nat_ipv4_range)
		{ mr->range[0].flags | NF_NAT_RANGE_MAP_IPS,
		  found->srcip, found->srcip,
		  mr->range[0].min, mr->range[0].max });
    DEBUGP("%s: found->srcip " NIPQUAD_FMT, __FUNCTION__, NIPQUAD(found->srcip));
	/* Hand modified range to generic setup. */
	return nf_nat_setup_info(ct, &newrange,  HOOK2MANIP(hooknum));
		}
	}

	return XT_CONTINUE;    /* We don't block any packet. */
}

static int trigger_tg_check(const struct xt_tgchk_param *par)
{
        const struct ipt_trigger_info *info = (struct ipt_trigger_info*)par->targinfo;
        struct list_head *cur_item, *tmp_item;
		const char *tablename = par->table;
		unsigned int hook_mask = par->hook_mask;


        if ((strcmp(tablename, "mangle") == 0)) {
                DEBUGP("trigger_check: bad table `%s'.\n", tablename);
                return -EINVAL;
        }
        if (hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD))) {
                DEBUGP("trigger_check: bad hooks %x.\n", hook_mask);
                return -EINVAL;
        }
        if (info->proto) {
            if (info->proto != IPPROTO_TCP && info->proto != IPPROTO_UDP) {
                DEBUGP("trigger_check: bad proto %d.\n", info->proto);
                return -EINVAL;
            }
        }
        if (info->type == IPT_TRIGGER_OUT) {
            if (!info->ports.mport[0] || !info->ports.rport[0]) {
                DEBUGP("trigger_check: Try 'iptbles -j TRIGGER -h' for help.\n");
               return -EINVAL;
            }
        }
        /* Empty the 'trigger_list' */
        list_for_each_safe(cur_item, tmp_item, &trigger_list) {
            struct ipt_trigger *trig = (void *)cur_item;

            DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
            del_timer(&trig->timeout);
            __del_trigger(trig);
        }
        return 0;
}

static unsigned int
trigger_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
  	const void *targinfo = par->targinfo;
 	const struct ipt_trigger_info *info = (struct ipt_trigger_info *)targinfo;
    const struct iphdr *iph = /*(skb)->nh.iph;*/ip_hdr(skb);
    const struct net_device *in = par->in;
    const struct net_device *out = par->out;
	unsigned int hooknum = par->hooknum;
    /*DEBUGP("%s: type = %s\n", __FUNCTION__,
            (info->type == IPT_TRIGGER_DNAT) ? "dnat" :
            (info->type == IPT_TRIGGER_IN) ? "in" : "out"); */

    /* The Port-trigger only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
        return XT_CONTINUE;
    if (info->type == IPT_TRIGGER_OUT)
        return trigger_out(skb, hooknum, in, out, targinfo);
    else if (info->type == IPT_TRIGGER_IN)
        return trigger_in(skb, hooknum, in, out, targinfo);
    else if (info->type == IPT_TRIGGER_DNAT)
        return trigger_dnat(skb, hooknum, in, out, targinfo);
    return XT_CONTINUE;
}

static struct xt_target trigger_tg_reg __read_mostly = {
	.name		= "TRIGGER",
	.family		= NFPROTO_IPV4,
	.target		= trigger_tg,
	.targetsize	= sizeof(struct ipt_trigger_info),
	.checkentry	= trigger_tg_check,
	.me		= THIS_MODULE,
};

static int __init trigger_tg_init(void)
{
	return xt_register_target(&trigger_tg_reg);
}

static void __exit trigger_tg_exit(void)
{
	xt_unregister_target(&trigger_tg_reg);
}

module_init(trigger_tg_init);
module_exit(trigger_tg_exit);
