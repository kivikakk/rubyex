#include "rkernel.h"
#include <vector>
#include <iostream>

RubyValue kernel_print(RubyEnvironment &, const std::vector<RubyObject *> &);
RubyValue kernel_puts(RubyEnvironment &, const std::vector<RubyObject *> &);
RubyValue kernel_p(RubyEnvironment &, const std::vector<RubyObject *> &);

void RubyKernelEI::init(RubyEnvironment &e)
{
  RubyModule *rb_mKernel = new RubyModule(e, "Kernel");
  rb_mKernel->add_method("print", COMETHOD(kernel_print, ARGS_ARBITRARY));
  rb_mKernel->add_method("puts", COMETHOD(kernel_puts, ARGS_ARBITRARY));
  rb_mKernel->add_method("p", COMETHOD(kernel_p, ARGS_ARBITRARY));

  e.add_module("Kernel", rb_mKernel);
}

RubyValue kernel_print(RubyEnvironment &e, const std::vector<RubyObject *> &args)
{
  std::cerr << "Kernel::print called with " << args.size() << " arguments." << std::endl;
  return RubyValue::from_object(e.NIL);
}

RubyValue kernel_puts(RubyEnvironment &e, const std::vector<RubyObject *> &args)
{
  std::cerr << "Kernel::puts called with " << args.size() << " arguments." << std::endl;
  kernel_print(e, args);
  return RubyValue::from_object(e.NIL);
}

RubyValue kernel_p(RubyEnvironment &e, const std::vector<RubyObject *> &args)
{
  std::cerr << "Kernel::p called with " << args.size() << " arguments." << std::endl;
  kernel_puts(e, args);
  return RubyValue::from_object(e.NIL);
}

