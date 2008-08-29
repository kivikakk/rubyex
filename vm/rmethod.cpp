#include "rmethod.h"
#include "rexception.h"
#include <iostream>

RubyMethod::~RubyMethod()
{ }

RubyMethod *RubyMethod::Create(RCMethodBlockNoArgs _function)
{ return new RubyMethodBlockNoArgs(_function); }

RubyMethod *RubyMethod::Create(RCMethodNoArgs _function)
{ return new RubyMethodNoArgs(_function); }

RubyMethod *RubyMethod::Create(RCMethodBlockArgs _function, int _args)
{ return new RubyMethodBlockArgs(_function, _args); }

RubyMethod *RubyMethod::Create(RCMethodArgs _function, int _args)
{ return new RubyMethodArgs(_function, _args); }

bool RubyMethod::verify_args(int _base, int _their)
{
  if (_base >= 0)
    return (_base == _their);
  return (_their >= (-_base - 1));
}

RubyValue RubyMethod::call(linked_ptr<Binding> &_b, RubyValue _self)
{
  return call(_b, _self, std::vector<RubyValue>());
}

RubyValue RubyMethod::call(linked_ptr<Binding> &_b, RubyValue _self, Block &_block)
{
  return call(_b, _self, std::vector<RubyValue>(), _block);
}

// block, no args.

RubyMethodBlockNoArgs::RubyMethodBlockNoArgs(RCMethodBlockNoArgs _function): function(_function)
{ }

RubyValue RubyMethodBlockNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, Block &_block)
{
  return function(_b, _self, _block);
}

RubyValue RubyMethodBlockNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &)
{
  throw WorldException(_b, _b->environment.ArgumentError, "no block given");
}

RubyValue RubyMethodBlockNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  if (_args.size() > 0)
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for 0)");

  return function(_b, _self, _block);
}

// no block, no args.

RubyMethodNoArgs::RubyMethodNoArgs(RCMethodNoArgs _function): function(_function)
{ }

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self)
{
  return function(_b, _self);
}

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (_args.size() > 0)
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for 0)");
    
  return function(_b, _self);
}

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &)
{
  if (_args.size() > 0)
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for 0)");

  return function(_b, _self);
}

// block, args.

RubyMethodBlockArgs::RubyMethodBlockArgs(RCMethodBlockArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodBlockArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  throw WorldException(_b, _b->environment.ArgumentError, "no block given");
}

RubyValue RubyMethodBlockArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  if ((int)_args.size() != args)
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for XXX)");

  return function(_b, _self, _args, _block);
}

// no block, args.

RubyMethodArgs::RubyMethodArgs(RCMethodArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (!verify_args(args, (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for XXX)");

  return function(_b, _self, _args);
}

RubyValue RubyMethodArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &)
{
  if (!verify_args(args, (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for XXX)");

  // block discarded.
  return function(_b, _self, _args);
}

// byte ccode

RubyBytecodeMethod::RubyBytecodeMethod(RubyModule *_def_target): code(_def_target, NULL /* no pre-supplied block */, NULL /* no outer context */)
{ }

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (!verify_args(code.args.size(), (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for XXX)");

  // XXX: I notice self is being lost here.
  return code.call(_b, _args);
}

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  if (!verify_args(code.args.size(), (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments (XXX for XXX)");
  //
  // XXX: Here too!
  return code.call(_b, _args, _block);
}

