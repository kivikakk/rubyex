#ifndef __RUBYEX_GLOBALS_H__
#define __RUBYEX_GLOBALS_H__

#include <linux/fs.h>
#include "cell.h"

#define SUCCESS 0
#define DEVICE_NAME "rubyexa"

#define puts(log_level, fmt, ...) printk(log_level DEVICE_NAME ": %s: " fmt, __func__, ##__VA_ARGS__)

extern int rubyex_major;
extern struct file_operations rubyex_fops;
extern struct cell_list *rubyex_cells;

#endif

