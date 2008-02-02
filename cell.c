#include "cell.h"
#include <linux/vmalloc.h>

struct cell *cell_allocate(const char *name)
{
  struct cell *cell = vmalloc(sizeof(struct cell));
  cell->name = vmalloc(strlen(name) + 1);
  strcpy(cell->name, name);
  cell->classes = NULL;
 
  return cell;
}

