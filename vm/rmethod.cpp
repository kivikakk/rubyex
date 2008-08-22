#include "rmethod.h"
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
  std::cerr << "RubyMethodBlockNoArgs::call: no block given." << std::endl;
  throw; // XXX - ArgumentError
}

RubyValue RubyMethodBlockNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &, Block &_block)
{
  // arguments discarded.
  return function(_b, _self, _block);
}

// no block, no args.

RubyMethodNoArgs::RubyMethodNoArgs(RCMethodNoArgs _function): function(_function)
{ }

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self)
{
  return function(_b, _self);
}

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &)
{
  // arguments discarded.
  return function(_b, _self);
}

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &, Block &)
{
  // arguments discarded.
  return function(_b, _self);
}

// block, args.

RubyMethodBlockArgs::RubyMethodBlockArgs(RCMethodBlockArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodBlockArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "RubyMethodBlockArgs::call: no block given." << std::endl;
  throw; // XXX - ArgumentError
}

RubyValue RubyMethodBlockArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  return function(_b, _self, _args, _block);
}

// no block, args.

RubyMethodArgs::RubyMethodArgs(RCMethodArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return function(_b, _self, _args);
}

RubyValue RubyMethodArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &)
{
  // block discarded.
  return function(_b, _self, _args);
}

// byte ccode

RubyBytecodeMethod::RubyBytecodeMethod(RubyClass *_def_target): code(_def_target)
{ }

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  // XXX: I notice self is being lost here.
  return code.call(_b, _args);
}

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  // XXX: Here too!
  return code.call(_b, _args, _block);
}

