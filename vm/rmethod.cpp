#include <iostream>
#include <sstream>
#include "rmethod.h"
#include "rexception.h"

RubyMethod::~RubyMethod()
{ }

linked_ptr<RubyMethod> RubyMethod::Create(RCMethodBlockNoArgs _function)
{ return linked_ptr<RubyMethod>(new RubyCMethod(_function)); }

linked_ptr<RubyMethod> RubyMethod::Create(RCMethodNoArgs _function)
{ return linked_ptr<RubyMethod>(new RubyCMethod(_function)); }

linked_ptr<RubyMethod> RubyMethod::Create(RCMethodBlockArgs _function, int _args)
{ return linked_ptr<RubyMethod>(new RubyCMethod(_function, _args)); }

linked_ptr<RubyMethod> RubyMethod::Create(RCMethodArgs _function, int _args)
{ return linked_ptr<RubyMethod>(new RubyCMethod(_function, _args)); }

bool RubyMethod::verify_args(int _base, int _their) {
  if (_base >= 0)
    return (_base == _their);
  return (_their >= (-_base - 1));
}

std::string RubyMethod::args_error_message(int _base, int _their) {
  std::ostringstream oss;
  oss << "wrong number of arguments (";

  if (_base >= 0)
    oss << _their << " for " << _base;
  else
    oss << _their << ", wanted at least " << (-_base - 1);
  oss << ")";
  return oss.str();
}

RubyValue RubyMethod::call(linked_ptr<Binding> &_b, RubyValue _self) {
  return call(_b, _self, std::vector<RubyValue>());
}

RubyValue RubyMethod::call(linked_ptr<Binding> &_b, RubyValue _self, Block &_block) {
  return call(_b, _self, std::vector<RubyValue>(), _block);
}

// actual CMethod implementations

RubyCMethod::RubyCMethod(RCMethodBlockNoArgs _function):
function((void *)_function), has_block(true), no_of_args(0) { }

RubyCMethod::RubyCMethod(RCMethodNoArgs _function):
function((void *)_function), has_block(false), no_of_args(0) { }

RubyCMethod::RubyCMethod(RCMethodBlockArgs _function, int _args):
function((void *)_function), has_block(true), no_of_args(_args) { }

RubyCMethod::RubyCMethod(RCMethodArgs _function, int _args):
function((void *)_function), has_block(false), no_of_args(_args) { }

RubyValue RubyCMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  if (has_block)
    throw WorldException(_b, _b->environment.ArgumentError, "no block given");
  if (!verify_args(no_of_args, (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, args_error_message(no_of_args, (int)_args.size()));
  
  if (no_of_args == 0)
    return ((RCMethodNoArgs)function)(_b, _self);
  else
    return ((RCMethodArgs)function)(_b, _self, _args);
}

RubyValue RubyCMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block) {
  if (!has_block)
    return call(_b, _self, _args);	// just drop the block silently.

  // this code path is only for `has_block'

  if (!verify_args(no_of_args, (int)_args.size()))
    throw WorldException(_b, _b->environment.ArgumentError, args_error_message(no_of_args, (int)_args.size()));
  
  if (no_of_args == 0)
    return ((RCMethodBlockNoArgs)function)(_b, _self, _block);
  else
    return ((RCMethodBlockArgs)function)(_b, _self, _args, _block);
}

// MultiCMethod

RubyMultiCMethod::RubyMultiCMethod(const linked_ptr<RubyCMethod> &_0, const linked_ptr<RubyCMethod> &_1) {
  methods.push_back(_0);
  methods.push_back(_1);
}

RubyMultiCMethod::RubyMultiCMethod(const linked_ptr<RubyCMethod> &_0, const linked_ptr<RubyCMethod> &_1, const linked_ptr<RubyCMethod> &_2) {
  methods.push_back(_0);
  methods.push_back(_1);
  methods.push_back(_2);
}

RubyValue RubyMultiCMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  for (std::vector< linked_ptr<RubyCMethod> >::iterator it = methods.begin(); it != methods.end(); ++it) {
    linked_ptr<RubyCMethod> &cm = *it;
    if (verify_args(cm->no_of_args, _args.size()) && !cm->has_block)
      return cm->call(_b, _self, _args);
  }

  throw WorldException(_b, _b->environment.ArgumentError, "cannot find matching method for " + Binding::get_representation(_b, _args));
}

RubyValue RubyMultiCMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block) {
  for (std::vector< linked_ptr<RubyCMethod> >::iterator it = methods.begin(); it != methods.end(); ++it) {
    linked_ptr<RubyCMethod> &cm = *it;
    if (verify_args(cm->no_of_args, _args.size()) && cm->has_block)
      return cm->call(_b, _self, _args, _block);
  }

  throw WorldException(_b, _b->environment.ArgumentError, "cannot find matching method for " + Binding::get_representation(_b, _args));
}

// BytecodeMethod

RubyBytecodeMethod::RubyBytecodeMethod(RubyModule *_def_target): code(_def_target, NULL /* no pre-supplied block */, NULL /* no outer context */)
{ }

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (!code.can_take_args((int)_args.size()))
    // This error message will be wrong. XXX
    throw WorldException(_b, _b->environment.ArgumentError, "XXX: " + args_error_message(code.args.size(), (int)_args.size()));
  // _self is passed as the new context.
  return code.call(_b, _self, _args);
}

RubyValue RubyBytecodeMethod::call(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  if (!code.can_take_args((int)_args.size()))
    // This error message will be wrong. XXX
    throw WorldException(_b, _b->environment.ArgumentError, "XXX: " + args_error_message(code.args.size(), (int)_args.size()));
  return code.call(_b, _self, _args, _block);
}

