#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <iostream>
#include "rstring.h"
#include "eval_hook.h"

RubyValue kernel_binding(Binding &, RubyValue);
RubyValue kernel_eval(Binding &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_print(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_puts(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_p(Binding &, RubyValue, const std::vector<RubyValue> &);

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

RubyValue kernel_binding(Binding &_b, RubyValue _self)
{
  return _b.environment.NIL;
}

RubyValue kernel_eval(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyValue first = _args[0];
  if (first.object->get_class() != _b.environment.String) {
    std::cerr << "Kernel::eval: tried to eval non-String" << std::endl;
    throw;
  }
  return eval_hook(_b.environment, /* XXX */ NULL, _self, dynamic_cast<RubyString *>(first.object)->string_value);
}

RubyValue kernel_print(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    if (it->object->get_class() == _b.environment.String)	// PlaysForSure
      std::cout << dynamic_cast<RubyString *>(it->object)->string_value;
  }

  return _b.environment.NIL;
}

RubyValue kernel_puts(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    std::vector<RubyValue> to_print;

    RubyValue result_val; RubyString *result;

    if (it->type == RubyValue::RV_OBJECT && it->object->get_class() == _b.environment.String)
      result_val = *it;
    else if (it->type == RubyValue::RV_OBJECT && it->object == _b.environment.NIL.object)
      result_val = it->call(_b, "inspect");
    else
      result_val = it->call(_b, "to_s");

    result = dynamic_cast<RubyString *>(result_val.object);

    std::string s = result->string_value;
    if (result->string_value.find("\n") != s.length() - 1)
      result->string_value += "\n";

    to_print.push_back(result_val);
    kernel_print(_b, _self, to_print);
  }
  return _b.environment.NIL;
}

RubyValue kernel_p(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    RubyValue r = it->call(_b, "inspect");
    std::vector<RubyValue> rca; rca.push_back(r);
    kernel_puts(_b, _self, rca);
  }

  return _b.environment.NIL;
}

