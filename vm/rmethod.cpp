#include "rmethod.h"
#include <iostream>

RubyMethod *RubyMethod::Create(RCMethodNoArgs _function)
{ return new RubyMethodNoArgs(_function); }

RubyMethod *RubyMethod::Create(RCMethodArgs _function, int _args)
{ return new RubyMethodArgs(_function, _args); }

RubyValue RubyMethod::call(linked_ptr<Binding> &_b, RubyValue _self)
{
  return call(_b, _self, std::vector<RubyValue>());
}

RubyMethodNoArgs::RubyMethodNoArgs(RCMethodNoArgs _function): function(_function)
{ }

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self)
{
  return function(_b, _self);
}

RubyValue RubyMethodNoArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &)
{
  return function(_b, _self);
}

RubyMethodArgs::RubyMethodArgs(RCMethodArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodArgs::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return function(_b, _self, _args);
}

RubyBytecodeMethod::RubyBytecodeMethod(int _args): args(_args)
{ }

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "whoops, RubyBytecodeMethod::call is certainly unimplemented." << std::endl;
  throw;
}
