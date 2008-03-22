#include "rkernel.h"

void RubyKernel::init(RubyEnvironment &e)
{
  RubyModule *rb_mKernel = new RubyModule(e, "Kernel");
}

