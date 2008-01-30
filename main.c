#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>			// fs
#include <linux/init.h>			// module macros
#include <asm/uaccess.h>		// put_user
#include "globals.h"

#define BUF_LEN 80

static int Device_Open = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;

struct file_operations rubyex_fops = {
  .read = rubyex_read,
  .write = rubyex_write,
  .open = rubyex_open,
  .release = rubyex_release
};


int rubyex_open(struct inode *inode, struct file *file)
{
  static int counter = 0;

  if (Device_Open)
    return -EBUSY;

  Device_Open++;
  sprintf(msg, "I already told you %d times Hello world!\n", counter++);
  msg_Ptr = msg;
  try_module_get(THIS_MODULE);

  return SUCCESS;
}

int rubyex_release(struct inode *inode, struct file *file)
{
  Device_Open--;

  module_put(THIS_MODULE);

  return 0;
}

ssize_t rubyex_read(struct file *filp, // see include/linux/fs.h
			    char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;
  if (*msg_Ptr == 0)	// NUL, i.e. EOF
    return 0;

  while (length && *msg_Ptr) {
    put_user(*(msg_Ptr++), buffer++);	// buffer is in userspace

    length--;
    bytes_read++;
  }

  return bytes_read;
}

ssize_t
rubyex_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
  printk(KERN_ALERT "Can't write to me.\n");
  return -EINVAL;
}
