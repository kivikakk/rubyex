#include <linux/kernel.h>
#include "globals.h"
#include "command.h"

void evaluate_command(const char *command)
{
  printk(KERN_INFO "Time to evaluate \"%s\".\n", command);
}
