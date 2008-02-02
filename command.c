#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include "globals.h"
#include "command.h"

char *get_next_nonblank_token(char **);
void cell_new(char **);

void evaluate_command(const char *command)
{
  char *our_command;
  char *ssptr, *token;

  our_command = vmalloc(strlen(command));
  strcpy(our_command, command);

  ssptr = our_command;
  token = get_next_nonblank_token(&ssptr);

  if (strcmp(token, "cell.new") == 0)
    cell_new(&ssptr);
    
  vfree(our_command);
}

void cell_new(char **ssptr)
{
  struct cell_list *it;
  struct cell *cell;
  char *token = get_next_nonblank_token(ssptr);

  if (!token) {
    printk(KERN_WARNING "rubyex: cell.new: requires argument (the name of the new cell)\n");
    return;
  }

  // make sure the cell name isn't in use.
  it = rubyex_cells;
  while (it) {
    cell = it->cell;
    if (strcmp(cell->name, token) == 0) {
      printk(KERN_WARNING "rubyex: cell.new: name \"%s\" already in use\n", token);
      return;
    }
    it = it->next;
  }

  cell = cell_allocate(token);
  cell_list_append(&rubyex_cells, cell);
}

char *get_next_nonblank_token(char **ssptr)
{
  char *token = NULL;
  do {
    token = strsep(ssptr, " ");
    if (!token)
      return NULL;
    if (strlen(token) == 0)
      token = NULL;
  } while (!token);
  return token;
}

