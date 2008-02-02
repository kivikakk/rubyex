#include "cell.h"
#include "globals.h"
#include <linux/vmalloc.h>

struct cell *cell_allocate(const char *name)
{
  struct cell *cell = vmalloc(sizeof(struct cell));

  cell->name = vmalloc(strlen(name) + 1);
  strcpy(cell->name, name);
  cell->classes = NULL;
 
  return cell;
}

// Select a minor, avoiding ones used in rubyex_cells.
int cell_select_minor(struct cell *cell)
{
  int minor = 1;
  struct cell_list_node *cln = rubyex_cells->head;
  while (cln) {
    if (cln->cell->minor == minor) {
      ++minor; cln = rubyex_cells->head;	// restart search
    } else
      cln = cln->next;
  }
  if (cell)
    cell->minor = minor;
  return minor;
}

