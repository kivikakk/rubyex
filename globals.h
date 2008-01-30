#ifndef __RUBYEX_H__
#define __RUBYEX_H__

#include <linux/fs.h>

#define SUCCESS 0

#define DEVICE_NAME "rubydev"

int rubyex_open(struct inode *, struct file *);
int rubyex_release(struct inode *, struct file *);
ssize_t rubyex_read(struct file *, char *, size_t, loff_t *);
ssize_t rubyex_write(struct file *, const char *, size_t, loff_t *);

extern int rubyex_major;
extern struct file_operations rubyex_fops;

#endif

