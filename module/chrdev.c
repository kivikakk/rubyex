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

static ssize_t rubyex_write_command(struct file *, const char *, size_t, loff_t *);
static ssize_t rubyex_write_cell(int, struct file *, const char *, size_t, loff_t *);
static void prepare_state(void);

static inline int get_minor(struct inode *inode)
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

#define COMMAND_BUF_LENGTH 80

static char *state, command[COMMAND_BUF_LENGTH];
static char *state_read;
static int command_written = 0;

int rubyex_open(struct inode *inode, struct file *file)
{
  int minor = get_minor(inode);

  if (minor == 0) { // `Control' port.
    if (control_open) return -EBUSY;

    prepare_state();
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
      puts(KERN_ALERT, "trying to close control (minor 0), but already closed.\n");

    vfree(state);
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
  int minor = get_minor(filp->f_dentry->d_inode);

  if (minor == 0)
    return rubyex_write_command(filp, buffer, length, offset);
  else 
    return rubyex_write_cell(minor, filp, buffer, length, offset);

  return -EINVAL;
}

static ssize_t rubyex_write_command(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  char *c, *nl_start; int c_len;
  long not_copied, copied = 0;
  size_t to_copy = length;

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

static ssize_t rubyex_write_cell(int minor, struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  struct cell *cell;

  if (!(cell = cell_get_by_minor(minor))) {
    puts(KERN_ALERT, "trying to write with minor %d, but no recorded cell with that minor exists!\n", minor);
    return -EINVAL;
  }

  puts(KERN_WARNING, "There.\n");

  return 0;
}

static void prepare_state(void)
{
  int written, position, size, list_length;
  struct cell_list_node *n;

  list_length = cell_list_length(rubyex_cells);

  size = 80;
  
  while (true) {
    state = vmalloc(size); position = 0;

    written = snprintf(state + position, size - position, "%d\n", list_length);
    position += written;
  
    n = rubyex_cells->head;
    while (n) {
      written = snprintf(state + position, size - position, "%d %s\n", n->cell->minor, n->cell->name);
      if (written >= size - position) {
	size += 60; goto _retry;
      }
      position += written;
      n = n->next;
    }

    break;

_retry:
    vfree(state);
  }

  state_read = state;
}
