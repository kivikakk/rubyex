#ifndef __RUBYEX_GLOBALS_H__
#define __RUBYEX_GLOBALS_H__

#include <linux/fs.h>
#include "cell.h"

#define SUCCESS 0
#define DEVICE_NAME "rubyexa"

int puts(const char *, const char *, const char *, ...)
     __attribute__ ((__format__ (__printf__, 3, 4))); // ensure checking is done on 4th argument [or something. it works.]

extern int rubyex_major;
extern struct file_operations rubyex_fops;
extern struct cell_list *rubyex_cells;

#endif

