#include <linux/module.h>		// all mods req
#include <linux/kernel.h>		// KERN_INFO
#include <linux/init.h>			// module macros
#include "globals.h"

static int __init rubyex_init(void)
{
  rubyex_major = register_chrdev(0, DEVICE_NAME, &rubyex_fops);

  if (rubyex_major < 0) {
    printk(KERN_ALERT "rubyex: registering character device failed with %d\n", rubyex_major);
    return rubyex_major;
  }

  printk(KERN_INFO "rubyex: created with major %d. fops at %p.\n", rubyex_major, &rubyex_fops);

  return SUCCESS;
}

static void __exit rubyex_exit(void)
{
  int ret = unregister_chrdev(rubyex_major, DEVICE_NAME);
  if (ret < 0)
    printk(KERN_ALERT "rubyex: error in unregister_chrdev: %d\n", ret);
}

module_init(rubyex_init);
module_exit(rubyex_exit);

MODULE_LICENSE("Dual BSD/GPL");

MODULE_AUTHOR("Arlen Christian Mart Cuss <celtic@sairyx.org>");
MODULE_DESCRIPTION("Provides Ruby evaluation capability in the kernel.");
