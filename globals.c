#include "globals.h"

int rubyex_major;
struct cell_list *rubyex_cells = NULL;

int puts(const char *log_level, const char *method, const char *fmt, ...)
{
  va_list args; int r;
  char *message;
  int length = 0;
  
  length = strlen(log_level) + strlen(fmt) + strlen(DEVICE_NAME) + 2;	// +2 for ": "

  if (method) length += strlen(method) + 2;	// +2 for ": "

  message = vmalloc(length + 1);
  strcpy(message, log_level);
  strcat(message, DEVICE_NAME ": ");
  if (method) {
    strcat(message, method);
    strcat(message, ": ");
  }
  strcat(message, fmt);

  va_start(args, fmt);
  r = vprintk(message, args);
  va_end(args);

  vfree(message);

  return r;
}
