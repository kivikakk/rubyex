#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <iostream>
#include "rstring.h"
#include "eval_hook.h"
#include "rbinding.h"

RubyValue kernel_binding(linked_ptr<Binding> &, RubyValue);
RubyValue kernel_eval(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_print(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_puts(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_p(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_gets(linked_ptr<Binding> &, RubyValue);

void RubyKernelEI::init(RubyEnvironment &_e)
{
  RubyModule *rb_mKernel = new RubyModule(_e, "Kernel");
  rb_mKernel->add_module_method(_e, "binding", RubyMethod::Create(kernel_binding));
  rb_mKernel->add_module_method(_e, "eval", RubyMethod::Create(kernel_eval, ARGS_ARBITRARY));

  rb_mKernel->add_module_method(_e, "print", RubyMethod::Create(kernel_print, ARGS_ARBITRARY));
  rb_mKernel->add_module_method(_e, "puts", RubyMethod::Create(kernel_puts, ARGS_ARBITRARY));
  rb_mKernel->add_module_method(_e, "p", RubyMethod::Create(kernel_p, ARGS_ARBITRARY));

  rb_mKernel->add_module_method(_e, "gets", RubyMethod::Create(kernel_gets));

  _e.add_module("Kernel", rb_mKernel);
  _e.Kernel = rb_mKernel;
}

RubyValue kernel_binding(linked_ptr<Binding> &_b, RubyValue _self)
{
  return RubyValue::from_object(new RubyBinding(_b));
}

RubyValue kernel_eval(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyValue first = _args[0];
  if (first.object->get_class() != _b->environment.String) {
    std::cerr << "Kernel::eval: tried to eval non-String" << std::endl;
    throw; // XXX Exception
  }

  linked_ptr<Binding> use_binding = _b;

  if (_args.size() == 2) {
    RubyValue second = _args[1];
    if (second.object->get_class() != _b->environment.Binding) {
      std::cerr << "Kernel::eval: second arg not a Binding?" << std::endl;
      throw; // XXX exception
    }

    use_binding = second.get_special<RubyBinding>()->binding;
  }

  return eval_hook(_b->environment, use_binding, _self, first.get_special<RubyString>()->string_value);
}

RubyValue kernel_print(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    if (it->object->get_class() == _b->environment.String)	// PlaysForSure
      std::cout << it->get_special<RubyString>()->string_value;
  }

  return _b->environment.NIL;
}

RubyValue kernel_puts(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    std::vector<RubyValue> to_print;

    RubyValue result_val; RubyString *result;

    if (it->type == RubyValue::RV_OBJECT && it->object->get_class() == _b->environment.String)
      result_val = *it;
    else if (it->type == RubyValue::RV_OBJECT && it->object == _b->environment.NIL.object)
      result_val = it->call(_b, "inspect");
    else
      result_val = it->call(_b, "to_s");

    result = result_val.get_special<RubyString>();

    std::string s = result->string_value;
    if (result->string_value.find("\n") != s.length() - 1)
      result->string_value += "\n";

    to_print.push_back(result_val);
    _self.call(_b, "print", to_print);
  }

  if (_args.size() == 0)
    _self.call(_b, "print", _b->environment.get_string("\n"));

  return _b->environment.NIL;
}

RubyValue kernel_p(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    RubyValue r = it->call(_b, "inspect");
    std::vector<RubyValue> rca; rca.push_back(r);
    _self.call(_b, "puts", rca);
  }

  return _b->environment.NIL;
}

RubyValue kernel_gets(linked_ptr<Binding> &_b, RubyValue _self)
{
  std::string inp;
  std::getline(std::cin, inp);
  if (!std::cin.eof())
    inp += '\n';
  else if (inp.size() == 0)
    return _b->environment.NIL;

  return _b->environment.get_string(inp);
}

