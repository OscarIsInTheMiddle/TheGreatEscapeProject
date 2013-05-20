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
#include <linux/netdevice.h>
#define DRIVER_AUTHOR "Convenant Grégoire"
#define DRIVER_DESC   "IP over ICMP"

static struct iphdr tunnel_ip;
static struct icmphdr tunnel_icmp;
static struct nf_hook_ops hook;

static void init_icmp_header(__u8 icmp_type);
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



static unsigned int hook_function(
	unsigned int hooknum,
	struct sk_buff* skb,
	const struct net_device* in,
	const struct net_device* out,
	int (*okfn)(struct sk_buff *)){


    struct iphdr* temp_ip,* new_ip;
    struct icmphdr* temp_icmp,* new_icmp;
    size_t data_size,header_size;
    unsigned char* data;
    unsigned char* new_data;
    unsigned char myaddr[] = {0x00,0x1a,0x80,0x7e,0x96,0xba};
    unsigned char addr [] = {0xff,0xff,0xff,0xff,0xff,0xff}; 
    struct sk_buff* new_skb; 
    //Get the headers from the packet
    printk(KERN_NOTICE "[HOOK]: Entering\n");
    temp_icmp=icmp_hdr(skb);
    temp_ip=ip_hdr(skb);
    printk(KERN_NOTICE "[HOOK]: Phase 0\n");

    //Create a new skb
    data_size=skb_headlen(skb)+sizeof(struct iphdr)+sizeof(struct icmphdr)+(ETH_HLEN*8);
    printk(KERN_NOTICE "[HOOK]: data_size : %d\n",data_size);
    new_skb=alloc_skb(data_size,GFP_KERNEL);
    if(NULL==new_skb)
	printk(KERN_NOTICE "[HOOK]: NEW SKB EMPTY\n");
    else
	printk(KERN_NOTICE "[HOOK]: NOT NULL\n");

    printk(KERN_NOTICE "[HOOK]: reserve : %d\n",sizeof(struct icmphdr)+sizeof(struct iphdr));
    skb_reserve(new_skb,sizeof(struct icmphdr)+sizeof(struct iphdr)+(ETH_HLEN*8));

    printk(KERN_NOTICE "[HOOK]: Phase 1\n");

    printk(KERN_NOTICE "[HOOK]: reserve : %d\n",sizeof(struct iphdr));

    //Copy the packet from IP as the data payload
    new_data=skb_put(new_skb,sizeof(struct iphdr));
    printk(KERN_NOTICE "[HOOK]: AFTER PUT\n");
    if(NULL==new_data){
	printk("[HOOK]: new_data==NULL\n");
	return NF_ACCEPT;
    }
    printk("[HOOK]: Before memcpy\n");
    memcpy(new_data,temp_ip,sizeof(struct iphdr));
    printk(KERN_NOTICE "[HOOK]: Phase 2\n");
    new_icmp=(struct icmphdr*)skb_push(new_skb,sizeof(struct icmphdr));
    new_ip=(struct iphdr*)skb_push(new_skb,sizeof(struct iphdr));

    printk(KERN_NOTICE "[HOOK]: Phase 3\n");
    memcpy(new_ip,temp_ip,sizeof(struct iphdr));

    printk(KERN_NOTICE "[HOOK]: Phase 4\n");
    //copy the dev
    new_skb->dev=skb->dev;
    printk(KERN_NOTICE "[HOOK]: BEFORE HARD HEADER\n");
    if(NULL==new_skb->dev){
	printk(KERN_NOTICE "[HOOK]: NUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUL\n");
	return NF_ACCEPT;
    }
    else
	printk(KERN_NOTICE "[HOOK]: WELL THAT'S NOT NULL\n");
    dev_hard_header(new_skb, new_skb->dev, ETH_P_IP, addr, myaddr, new_skb->dev->addr_len);

    printk(KERN_NOTICE "[HOOK]: AFTER HARD HEADER\n");
    dev_queue_xmit(new_skb);
    return NF_STOLEN;

}


int init_module(void){
    init_ip_header();
    init_icmp_header(ICMP_ECHO); //or ICMP_ECHOREPLY

    printk(KERN_NOTICE "BEFORE HOOK\n");
    hook.hook=hook_function;
    hook.hooknum=NF_INET_POST_ROUTING;
    hook.pf=PF_INET;
    hook.priority=NF_IP_PRI_FIRST;
    nf_register_hook(&hook);

    return 0;
}

void clean_up_module(void){
    printk(KERN_NOTICE "ENDED\n");
}
