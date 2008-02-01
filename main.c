#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>			// fs
#include <asm/uaccess.h>		// put_user
#include "globals.h"

int rubyex_open(struct inode *, struct file *);
int rubyex_release(struct inode *, struct file *);
ssize_t rubyex_read(struct file *, char *, size_t, loff_t *);
ssize_t rubyex_write(struct file *, const char *, size_t, loff_t *);

static int control_open = 0;

struct file_operations rubyex_fops = {
  .read = rubyex_read,
  .write = rubyex_write,
  .open = rubyex_open,
  .release = rubyex_release
};

static char state[160], command[80];
static char *state_read;
static int command_written = 0;

int rubyex_open(struct inode *inode, struct file *file)
{
  int minor = inode->i_rdev & 0xff;	// obviously.
  printk(KERN_INFO "open: minor %d\n", minor);

  if (minor == 0) { // `Control' port.
    if (control_open) return -EBUSY;

    if (command_written > 0)
      sprintf(state, "no. of cells: %d\ncommand: %s\n", cell_list_length(rubyex_cells), command);
    else
      sprintf(state, "no. of cells: %d\n", cell_list_length(rubyex_cells));

    state_read = state;
    command_written = 0;
    printk(KERN_INFO "%s\n", state);

    ++control_open;
    try_module_get(THIS_MODULE);
    return SUCCESS;
  }

  return -1;	// ??
}

int rubyex_release(struct inode *inode, struct file *file)
{
  int minor = inode->i_rdev & 0xff;

  if (minor == 0) {
    if (control_open == 0)
      printk(KERN_ALERT "rubyex: what? Trying to close control (minor 0), but already closed.\n");
    control_open = 0;

    module_put(THIS_MODULE);
    return SUCCESS;
  }

  return -1;	 // confused.
}

ssize_t rubyex_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;

  if (*state_read == 0)	// NUL, i.e. EOF
    return 0;

  while (length && *state_read) {
    put_user(*(state_read++), buffer++);	// buffer is in userspace

    length--;
    bytes_read++;
  }

  return bytes_read;
}

ssize_t rubyex_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  long not_copied, copied;
  
  printk(KERN_INFO "command_write, buffer, length: %d, %p, %d\n", command_written, buffer, length);
  
  not_copied = copy_from_user(command + command_written, buffer, length);
  copied = length - not_copied;

  printk(KERN_INFO "copied: %ld\n", copied);
  command_written += copied;

  command[command_written] = (char)0;

  return copied;
}
