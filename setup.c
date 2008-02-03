#include <linux/module.h>		// all mods req
#include <linux/kernel.h>		// KERN_INFO
#include <linux/init.h>			// module macros
#include "globals.h"

#define DEVICE_NAME "rubyexa"

static int __init rubyex_init(void)
{
  rubyex_major = register_chrdev(0, DEVICE_NAME, &rubyex_fops);

  if (rubyex_major < 0) {
    puts(KERN_ALERT, "rubyex_init", "registering character device failed with %d\n", rubyex_major);
    return rubyex_major;
  }

  puts(KERN_INFO, "rubyex_init", "created with major %d.\n", rubyex_major);

  rubyex_cells = cell_list_allocate(NULL);

  return SUCCESS;
}

static void __exit rubyex_exit(void)
{
  int ret = unregister_chrdev(rubyex_major, DEVICE_NAME);
  if (ret < 0)
    puts(KERN_ALERT, "rubyex_exit", "error in unregister_chrdev: %d\n", ret);
  puts(KERN_INFO, "rubyex_exit", "removed from kernel.\n");
}

module_init(rubyex_init);
module_exit(rubyex_exit);

MODULE_LICENSE("Dual BSD/GPL");

MODULE_AUTHOR("Arlen Christian Mart Cuss <celtic@sairyx.org>");
MODULE_DESCRIPTION("Provides Ruby evaluation capability in the kernel.");
