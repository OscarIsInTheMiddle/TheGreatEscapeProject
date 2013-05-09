/* The Great Escap Project 

   Bypass firewall by putting packets into ICMP (Echo/Request) data 

Author: Gregoire Convenant
mail: oscar.is.in.the.middle@gmail.com
license: don't know it yet, ... maybe GPL or LGPL.... */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>

#define DRIVER_AUTHOR "Convenant Grégoire"
#define DRIVER_DESC   "IP over ICMP"

static struct iphdr tunnel_ip;
static struct icmphdr tunnel_icmp;
static struct nf_hook_ops hook;

static void init_icmp_header(__u8 icmp_type);
static unsigned int output_packet(struct sk_buff* sk);
static void init_ip_header(void);


static void init_ip_header(void){
    tunnel_ip.version=4;
    tunnel_ip.ihl=5;
    tunnel_ip.tos=0;
    tunnel_ip.tot_len=0;
    tunnel_ip.frag_off=0;
    tunnel_ip.id=0;
    tunnel_ip.ttl=0xff;
    tunnel_ip.protocol=1;
    //	tunnel_ip.saddr=;
    //	tunnel_ip.daddr=;
}

static void init_icmp_header(__u8 icmp_type){
    tunnel_icmp.type=icmp_type;
    tunnel_icmp.code=0;
    tunnel_icmp.checksum=0;
}

/* This function aims at adding an IP/ICMP header to the data
   no matter of mtu yet....*/
static unsigned int output_packet(struct sk_buff* skb){
    struct iphdr* encapsulated_ip;
    struct icmphdr* encapsulated_icmp;

    encapsulated_ip=(struct iphdr*)skb_network_header(skb);
    encapsulated_icmp=(struct icmphdr*)skb_transport_header(skb);
    printk(KERN_NOTICE "HERE IS ONE PACKET!\n");
    return 0;
}


static unsigned int hook_function(
		unsigned int hooknum,
		struct sk_buff* skb,
		const struct net_device* in,
		const struct net_device* out,
		int (*okfn)(struct sk_buff *)){
    output_packet(skb);
    return NF_ACCEPT;
}


int init_module(void){
    init_ip_header();
    init_icmp_header(ICMP_ECHOREPLY); //or ICMP_ECHO

    printk(KERN_NOTICE "BEFORE HOOK\n");
    hook.hook=hook_function;
    hook.hooknum=3;//NF_IP_LOCAL_OUT;
    hook.pf=PF_INET;
    hook.priority=NF_IP_PRI_FIRST;
    nf_register_hook(&hook);

    return 0;
}

void clean_up_module(void){
    printk(KERN_NOTICE "ENDED\n");
}
