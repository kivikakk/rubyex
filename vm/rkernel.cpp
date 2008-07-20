#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <iostream>
#include "rstring.h"
#include "eval_hook.h"

RubyValue kernel_binding(RubyEnvironment &, RubyValue);
RubyValue kernel_eval(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_print(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_puts(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_p(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);

void RubyKernelEI::init(RubyEnvironment &_e)
{
  RubyModule *rb_mKernel = new RubyModule(_e, "Kernel");
  rb_mKernel->add_module_method(_e, "binding", RubyMethod::Create(kernel_binding));
  rb_mKernel->add_module_method(_e, "eval", RubyMethod::Create(kernel_eval, ARGS_ARBITRARY));

  rb_mKernel->add_module_method(_e, "print", RubyMethod::Create(kernel_print, ARGS_ARBITRARY));
  rb_mKernel->add_module_method(_e, "puts", RubyMethod::Create(kernel_puts, ARGS_ARBITRARY));
  rb_mKernel->add_module_method(_e, "p", RubyMethod::Create(kernel_p, ARGS_ARBITRARY));

  _e.add_module("Kernel", rb_mKernel);
  _e.Kernel = rb_mKernel;
}

RubyValue kernel_binding(RubyEnvironment &_e, RubyValue _self)
{
  return _e.NIL;
}

RubyValue kernel_eval(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyValue first = _args[0];
  if (first.object->get_class() != _e.String) {
    std::cerr << "Kernel::eval: tried to eval non-String" << std::endl;
    throw;
  }
  return eval_hook(_e, _self, dynamic_cast<RubyString *>(first.object)->string_value);
}

RubyValue kernel_print(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    if (it->object->get_class() == _e.String)	// PlaysForSure
      std::cout << dynamic_cast<RubyString *>(it->object)->string_value;
  }

  return _e.NIL;
}

RubyValue kernel_puts(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    std::vector<RubyValue> to_print;

    RubyValue result_val; RubyString *result;

    if (it->object->get_class() == _e.String)
      result_val = *it;
    else if (it->object == _e.NIL.object)
      result_val = it->call(_e, "inspect");
    else
      result_val = it->call(_e, "to_s");

    result = dynamic_cast<RubyString *>(result_val.object);

    std::string s = result->string_value;
    if (result->string_value.find("\n") != s.length() - 1)
      result->string_value += "\n";

    to_print.push_back(result_val);
    kernel_print(_e, _self, to_print);
  }
  return _e.NIL;
}

RubyValue kernel_p(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    RubyValue r = it->call(_e, "inspect");
    std::vector<RubyValue> rca; rca.push_back(r);
    kernel_puts(_e, _self, rca);
  }

  return _e.NIL;
}

