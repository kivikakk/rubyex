#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include "globals.h"
#include "command.h"

void cell_new(char **);
void cell_list(char **);
void cell_remove(char **);
void cell_reset(char **);
char *get_next_nonblank_token(char **);

void evaluate_command(const char *command)
{
  char *our_command;
  char *ssptr, *token;

  our_command = vmalloc(strlen(command) + 1);
  strcpy(our_command, command);

  ssptr = our_command;
  token = get_next_nonblank_token(&ssptr);

  // XXX: do something CircleMUD commandish with this
  if (!token)
    ;
  else if (strcmp(token, "cell.new") == 0)
    cell_new(&ssptr);
  else if (strcmp(token, "cell.list") == 0)
    cell_list(&ssptr);
  else if (strcmp(token, "cell.remove") == 0)
    cell_remove(&ssptr);
  else if (strcmp(token, "cell.reset") == 0)
    cell_reset(&ssptr);
    
  vfree(our_command);
}

void cell_new(char **ssptr)
{
  struct cell *cell;
  char *token = get_next_nonblank_token(ssptr);

  if (!token) {
    puts(KERN_WARNING, "requires argument (the name of the new cell)\n");
    return;
  }

  // make sure the cell name isn't in use.
  
  if ((cell = cell_get_by_name(token))) {
    puts(KERN_WARNING, "name \"%s\" already in use (minor %d)\n", token, cell->minor);
    return;
  }

  cell = cell_allocate(token);
  cell_select_minor(cell);
  cell_list_append(rubyex_cells, cell);
}

void cell_list(char **ssptr)
{
  struct cell_list_node *it = rubyex_cells->head;
  puts(KERN_INFO, "begin\n");
  while (it) {
    puts(KERN_INFO, "{:name => \"%s\", :minor => %d}\n", it->cell->name, it->cell->minor);
    it = it->next;
  }
  puts(KERN_INFO, "end\n");
}

void cell_remove(char **ssptr)
{
  struct cell *cell = NULL;
  char *token = get_next_nonblank_token(ssptr);

  if (!token) {
    puts(KERN_WARNING, "requires argument (the name of the cell to remove)\n");
    return;
  }

  if (!(cell = cell_get_by_name(token))) {
    puts(KERN_WARNING, "could not find cell named \"%s\"\n", token);
    return;
  }

  cell_list_remove(rubyex_cells, cell);
  cell_deallocate(cell);
}

void cell_reset(char **ssptr)
{
  struct cell *cell = NULL;
  char *token = get_next_nonblank_token(ssptr);

  if (!token) {
    puts(KERN_WARNING, "requires argument (the name of the cell to reset)\n");
    return;
  }

  if (!(cell = cell_get_by_name(token))) {
    puts(KERN_WARNING, "could not find cell named \"%s\"\n", token);
    return;
  }

  cell_reallocate_data(cell);
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

