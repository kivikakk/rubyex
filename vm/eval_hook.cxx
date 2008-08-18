#include "eval_hook.h"
#include <iostream>

RubyValue eval_hook(RubyEnvironment &_e, linked_ptr<Binding> _binding, RubyValue _self, const std::string &_code)
{
  std::cerr << "error: no eval() in VM" << std::endl;
  return _e.NIL;
}
