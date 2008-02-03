#include "globals.h"

int rubyex_major;
struct cell_list *rubyex_cells = NULL;

int puts(const char *log_level, const char *method, const char *fmt, ...)
{
  va_list args; int r;
  char *message;
  int length = 0;
  
  length = strlen(log_level) + strlen(fmt) + 8;	// 8: "rubyexa: "

  if (method) length += strlen(method) + 2;

  message = vmalloc(length + 1);
  strcpy(message, log_level);
  strcat(message, "rubyexa: ");
  strcat(message, method);
  strcat(message, fmt);

  va_start(args, fmt);
  r = vprintk(message, args);
  va_end(args);

  vfree(message);

  return r;
}
