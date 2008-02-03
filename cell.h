#ifndef __RUBYEX_CELL_H__
#define __RUBYEX_CELL_H__

#include "class.h"
#include "list.h"

struct cell {
  char *name;
  struct class_list *classes;
  int minor;
};

list_define(cell);
struct cell *cell_allocate(const char *);
void cell_deallocate(struct cell *);
void cell_reallocate_data(struct cell *);
struct cell *cell_get_by_name(const char *);
int cell_select_minor(struct cell *);

#endif

