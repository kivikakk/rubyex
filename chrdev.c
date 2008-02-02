#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>			// fs
#include <asm/uaccess.h>		// put_user
#include <linux/vmalloc.h>
#include "globals.h"
#include "command.h"

int rubyex_open(struct inode *, struct file *);
int rubyex_release(struct inode *, struct file *);
ssize_t rubyex_read(struct file *, char *, size_t, loff_t *);
ssize_t rubyex_write(struct file *, const char *, size_t, loff_t *);

inline int get_minor(struct inode *inode)
{
  // sometimes I wonder why I bother to write a helper function for something this obvious.
  return inode->i_rdev & 0xff;
}

static int control_open = 0;

struct file_operations rubyex_fops = {
  .read = rubyex_read,
  .write = rubyex_write,
  .open = rubyex_open,
  .release = rubyex_release
};

#define STATE_BUF_LENGTH 80
#define COMMAND_BUF_LENGTH 80

static char state[STATE_BUF_LENGTH], command[COMMAND_BUF_LENGTH];
static char *state_read;
static int command_written = 0;

int rubyex_open(struct inode *inode, struct file *file)
{
  int minor = get_minor(inode);

  if (minor == 0) { // `Control' port.
    if (control_open) return -EBUSY;

    sprintf(state, "no. of cells: %d\n", cell_list_length(rubyex_cells));

    state_read = state;
    command_written = 0;

    ++control_open;
    try_module_get(THIS_MODULE);
    return SUCCESS;
  }

  return -EINVAL;	// ??
}

int rubyex_release(struct inode *inode, struct file *file)
{
  int minor = get_minor(inode);

  if (minor == 0) {
    if (control_open == 0)
      printk(KERN_ALERT "rubyex: what? Trying to close control (minor 0), but already closed.\n");
    control_open = 0;

    module_put(THIS_MODULE);
    return SUCCESS;
  }

  return -EINVAL;	 // confused.
}

ssize_t rubyex_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  int bytes_read = 0;
  int minor = get_minor(filp->f_dentry->d_inode);

  if (minor == 0) {
    if (*state_read == 0)	// NUL, i.e. EOF
      return 0;

    while (length && *state_read) {
      put_user(*(state_read++), buffer++);

      length--;
      bytes_read++;
    }

    return bytes_read;
  }

  return -EINVAL;
}

ssize_t rubyex_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  char *c, *nl_start; int c_len;
  long not_copied, copied = 0;
  int minor = get_minor(filp->f_dentry->d_inode);

  size_t to_copy = length;

  if (minor == 0) {
    if (command_written + to_copy >= COMMAND_BUF_LENGTH) {
      to_copy = COMMAND_BUF_LENGTH - command_written - 1;	// max
      if (to_copy == 0)
	return -EINVAL;		// too long!
    }

    not_copied = copy_from_user(command + command_written, buffer, to_copy);
    copied = to_copy - not_copied;

    command_written += copied;
    command[command_written] = (char)0;

    nl_start = strchr(command, '\n');
    if (nl_start) {
      c_len = nl_start - command;
      c = vmalloc(c_len + 1);
      strlcpy(c, command, c_len + 1);
      // now we have everything (except the \n) in `c'. note that the following memmove
      // starts at nl_start+1, to avoid taking it with us.
      memmove(command, nl_start + 1, strlen(nl_start + 1));
      command_written -= c_len + 1;
      evaluate_command(c);
      vfree(c);
    }

    return copied;
  }

  return -EINVAL;
}

