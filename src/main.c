/* The Great Escap Project 

   Bypass firewall by putting packets into ICMP (Echo/Request) data 

Author: Gregoire Convenant
mail: oscar.is.in.the.middle@gmail.com
license: don't know it yet, ... maybe GPL or LGPL.... */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/icmp.h>
#include <linux/ip.h>

static struct iphdr tunnel_ip;
static struct icmphdr


int init_module(void){
    init_ip();
    init_icmp(ICMP_ECHOREPLY ICMP_ECHO);

    printk(KERN_NOTICE "COUCOU\n");
    return 0;
}

void clean_up_module(void){
    printk(KERN_NOTICE "ENDED\n");
}

void init_ip_header(){

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

void init_icmp_header(__u8 icmp_type){
    tunnel_icmp.type=icmp_type;
    tunnel_icmp.code=0;
    tunnel_icmp.checksum=0;
}

/* This function aims at adding an IP/ICMP header to the data
   no matter of mtu yet....*/
unsigned int ouput_packet(sk_buff** sk){
    struct iphdr* encapsulated_ip;
    struct icmphdr* encapsulated_icmp;

    encapsulated_ip=(iphdr*)skb_network_header(sk);
    encapsulated_icmp=(icmphdr*)skb_transport_header(sk);


}
