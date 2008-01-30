#ifndef __RUBYEX_CELL_H__
#define __RUBYEX_CELL_H__

#include "class.h"
#include "list.h"

struct cell {
  struct class_list *classes;
  int id;
};

list_define(cell);

#endif

