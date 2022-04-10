
#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include "linux/version.h"
#include "linux/module.h"
#include "linux/kernel.h"


static int module_number = 0;
module_param(module_number, int, S_IRUGO);

static int output=1;

int init_module(void) {
     printk("Output= %d \n", output);
     return 0;
}

void cleanup_module(void){
     printk("Hello Orthogonal ! \n");
}
