#include <linux/module.h>

static int __init test1_init(void) {
	printk(KERN_INFO "insmod! %lld\n", get_jiffies_64());
	return 0;
}

static void __exit test1_exit(void){
	printk(KERN_INFO "rmmod! %lld\n", get_jiffies_64());
}

module_init(test1_init);
module_exit(test1_exit);
MODULE_LICENSE("GPL");
