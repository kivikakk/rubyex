#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <iostream>

RubyValue kernel_print(RubyEnvironment &, const std::vector<RubyValue> &);
RubyValue kernel_puts(RubyEnvironment &, const std::vector<RubyValue> &);
RubyValue kernel_p(RubyEnvironment &, const std::vector<RubyValue> &);

void RubyKernelEI::init(RubyEnvironment &e)
{
  RubyModule *rb_mKernel = new RubyModule(e, "Kernel");
  rb_mKernel->add_method("print", CMETHOD(kernel_print, ARGS_ARBITRARY));
  rb_mKernel->add_method("puts", CMETHOD(kernel_puts, ARGS_ARBITRARY));
  rb_mKernel->add_method("p", CMETHOD(kernel_p, ARGS_ARBITRARY));

  e.add_module("Kernel", rb_mKernel);
}

RubyValue kernel_print(RubyEnvironment &e, const std::vector<RubyValue> &args)
{
  std::cerr << "Kernel::print called with " << args.size() << " arguments." << std::endl;
  return RubyValue::from_object(e.NIL);
}

RubyValue kernel_puts(RubyEnvironment &e, const std::vector<RubyValue> &args)
{
  std::cerr << "Kernel::puts called with " << args.size() << " arguments." << std::endl;
  kernel_print(e, args);
  return RubyValue::from_object(e.NIL);
}

RubyValue kernel_p(RubyEnvironment &e, const std::vector<RubyValue> &args)
{
  std::cerr << "Kernel::p called with " << args.size() << " arguments." << std::endl;
  kernel_puts(e, args);
  return RubyValue::from_object(e.NIL);
}

