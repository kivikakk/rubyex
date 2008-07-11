#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <iostream>

RubyValue kernel_print(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_puts(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_p(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);

void RubyKernelEI::init(RubyEnvironment &_e)
{
  RubyModule *rb_mKernel = new RubyModule(_e, "Kernel");
  rb_mKernel->add_metaclass_method(_e, "print", RubyMethod::Create(kernel_print, ARGS_ARBITRARY));
  rb_mKernel->add_metaclass_method(_e, "puts", RubyMethod::Create(kernel_puts, ARGS_ARBITRARY));
  rb_mKernel->add_metaclass_method(_e, "p", RubyMethod::Create(kernel_p, ARGS_ARBITRARY));

  _e.add_module("Kernel", rb_mKernel);
}

RubyValue kernel_print(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "Kernel::print called with " << _args.size() << " arguments." << std::endl;
  return RubyValue::from_object(_e.NIL);
}

RubyValue kernel_puts(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "Kernel::puts called with " << _args.size() << " arguments." << std::endl;
  kernel_print(_e, _self, _args);
  return RubyValue::from_object(_e.NIL);
}

RubyValue kernel_p(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "Kernel::p called with " << _args.size() << " arguments." << std::endl;
  kernel_puts(_e, _self, _args);
  return RubyValue::from_object(_e.NIL);
}

