#include "rmethod.h"
#include <iostream>

RubyMethod *RubyMethod::Create(RCMethodNoArgs _function)
{ return new RubyMethodNoArgs(_function); }

RubyMethod *RubyMethod::Create(RCMethodArgs _function, int _args)
{ return new RubyMethodArgs(_function, _args); }

RubyValue RubyMethod::call(RubyEnvironment &_e, RubyValue _self)
{
  return call(_e, _self, std::vector<RubyValue>());
}

RubyMethodNoArgs::RubyMethodNoArgs(RCMethodNoArgs _function): function(_function)
{ }

RubyValue RubyMethodNoArgs::call(RubyEnvironment &_e, RubyValue _self)
{
  return function(_e, _self);
}

RubyValue RubyMethodNoArgs::call(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &)
{
  return function(_e, _self);
}

RubyMethodArgs::RubyMethodArgs(RCMethodArgs _function, int _args): function(_function), args(_args)
{ }

RubyValue RubyMethodArgs::call(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return function(_e, _self, _args);
}

RubyBytecodeMethod::RubyBytecodeMethod(int _args): args(_args)
{ }

RubyValue RubyBytecodeMethod::call(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  std::cerr << "whoops, RubyBytecodeMethod::call is certainly unimplemented." << std::endl;
  throw;
}
