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

void cell_deallocate(struct cell *cell)
{
  vfree(cell->name);

  // TODO: free classes

  vfree(cell);	// goodbye, sweet one
}

void cell_reallocate_data(struct cell *cell)
{
  // reallocate classes.
}

struct cell *cell_get_by_name(const char *name)
{
  struct cell_list_node *it;

  it = rubyex_cells->head;
  while (it) {
    if (strcmp(it->cell->name, name) == 0) {
      return it->cell;
      break;
    }
    it = it->next;
  }

  return NULL;
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

