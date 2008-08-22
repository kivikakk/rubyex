#include <sstream>
#include <algorithm>
#include "block.h"
#include "reader.h"
#include "context.h"
#include "process.h"

Block::Block(): def_target(NULL)
{ }

RubyValue Block::call(linked_ptr<Binding> &_b)
{ return call(_b, std::vector<RubyValue>()); }

RubyValue Block::call(linked_ptr<Binding> &_b, RubyValue _arg1)
{
  std::vector<RubyValue> args;
  args.push_back(_arg1);

  return call(_b, args);
  }

RubyValue Block::call(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args)
{
  std::istringstream iss(this->code);
  Reader r = Reader(iss);

  // N.B. new Context's `self' is the same as the one from outside.
  // Hence `self' in a block still refers to the `self' from outside,
  // which makes sense.
  Context *c = new Context(_b->environment, _b->context, def_target);
  // XXX!! This context should have a binding which somehow keeps
  // the variables from outer scope in, while new variables
  // created within this scope die afterward.


  // We now create the block variable arguments in this context.
  // In true MRI fashion, we ignore extra arguments that aren't being used
  // in the block, and add `nil's if we haven't enough.
  unsigned int given_args_to_add = std::min(this->args.size(), _args.size());
  for (unsigned int i = 0; i < given_args_to_add; ++i)
    c->assign(this->args[i], _args[i]);
  
  for (unsigned int i = given_args_to_add; i < this->args.size(); ++i)
    c->assign(this->args[i], _b->environment.NIL);

  RubyValue ret_val = process(_b->environment, r, c, NULL);

  delete c;

  return ret_val;
}

RubyValue Block::call(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args, Block &_block)
{
  std::istringstream iss(this->code);
  Reader r = Reader(iss);

  Context *c = new Context(_b->environment, _b->context, def_target);

  unsigned int given_args_to_add = std::min(this->args.size(), _args.size());
  for (unsigned int i = 0; i < given_args_to_add; ++i)
    c->assign(this->args[i], _args[i]);
  
  for (unsigned int i = given_args_to_add; i < this->args.size(); ++i)
    c->assign(this->args[i], _b->environment.NIL);

  RubyValue ret_val = process(_b->environment, r, c, &_block);

  delete c;
  return ret_val;
}

