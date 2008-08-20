#include <sstream>
#include "block.h"
#include "reader.h"
#include "context.h"
#include "process.h"

Block::Block()
{ }

RubyValue Block::call(linked_ptr<Binding> &_b)
{
  return call(_b, std::vector<RubyValue>());
}

RubyValue Block::call(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args)
{
  std::istringstream iss(data);
  Reader r = Reader(iss);

  // N.B. new Context's `self' is the same as the one from outside.
  // Hence `self' in a block still refers to the `self' from outside,
  // which makes sense.
  Context *c = new Context(_b->environment, _b->context);
  // XXX!! This context should have a binding which somehow keeps
  // the variables from outer scope in, while new variables
  // created within this scope die afterward.

  RubyValue ret_val = process(_b->environment, r, c);

  delete c;

  return ret_val;
}

