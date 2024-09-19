#define MODULE
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPLv3");
int init_module(void){
printk("<1> Hello,World\n");
return 0;
}
void cleanup_module(void){
printk("<1> Goodbye.\n");
}