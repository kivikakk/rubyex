#ifndef __RUBYEX_GLOBALS_H__
#define __RUBYEX_GLOBALS_H__

#include <linux/fs.h>
#include "cell.h"

#define SUCCESS 0

#define DEVICE_NAME "rubydev"

extern int rubyex_major;
extern struct file_operations rubyex_fops;
extern struct cell_list *rubyex_cells;

#endif

