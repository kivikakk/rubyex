#ifndef __RUBYEX_CELL_H__
#define __RUBYEX_CELL_H__

#include "class.h"
#include "list.h"

struct cell {
  char *name;
  struct class_list *classes;
};

list_define(cell);
struct cell *cell_allocate(const char *);

#endif

