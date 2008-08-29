#include "eval_hook.h"
#include "rexception.h"
#include <iostream>

RubyValue eval_hook(linked_ptr<Binding> &_b, RubyValue, const std::string &)
{
  throw WorldException(_b, _b->environment.RuntimeError, "no eval() in VM");
}
