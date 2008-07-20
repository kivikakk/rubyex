#include "eval_hook.h"
#include <iostream>

RubyValue eval_hook(RubyEnvironment &_e, RubyValue _self, const std::string &_code)
{
  std::cerr << "error: no eval() in VM" << std::endl;
  return _e.NIL;
}
