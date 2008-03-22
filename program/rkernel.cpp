#include "rkernel.h"

void RubyKernelEI::init(RubyEnvironment &e)
{
  RubyModule *rb_mKernel = new RubyModule(e, "Kernel");

  e.add_module("Kernel", rb_mKernel);
}

