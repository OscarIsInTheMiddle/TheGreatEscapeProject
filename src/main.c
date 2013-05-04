/* The Great Escap Project 

   Bypass firewall by putting packets into ICMP (Echo/Request) data 

Author: Gregoire Convenant
mail: oscar.is.in.the.middle@gmail.com
license: don't know it yet, ... maybe GPL or LGPL.... */

#include "linux/module.h"
#include "linux/kernel.h"

int init_module(void){
    printk(KERN_NOTICE "COUCOU\n");
    return 0;
}

void clean_up_module(void){
    printk(KERN_NOTICE "ENDED\n");
}
