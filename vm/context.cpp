#include <iostream>
#include <exception>
#include "context.h"
#include "rmethod.h"
#include "rexception.h"

Context::Context(RubyEnvironment &_environment): binding(linked_ptr<Binding>(new Binding(_environment, O2V(_environment.main), _environment.Object))), outer_context(NULL)
{ }

Context::Context(RubyEnvironment &_environment, RubyValue _context, RubyModule *_def_target, Context *_outer_context): binding(linked_ptr<Binding>(new Binding(_environment, _context, _def_target))), outer_context(_outer_context)
{ }

Context::Context(linked_ptr<Binding> &_binding): binding(_binding), outer_context(NULL)
{ }

RubyValue Context::entry_to_value(const Stack::StackEntry &_entry)
{
  // We take control of StackEntry's memory. See a note in stack.cpp:pop_value about how we should do this better.

  switch (_entry.type) {
    case Stack::SE_IDENTIFIER: {
      std::string id = *_entry.identifier;
      delete _entry.identifier;
      return resolve_identifier(id);
    }

    case Stack::SE_SYMBOL: {
      std::string symbol = *_entry.symbol;
      delete _entry.symbol;
      return S2V(binding->environment.get_symbol(symbol));
    }

    case Stack::SE_INTEGER: return F2V(_entry.integer);
    case Stack::SE_OBJECT: return O2V(_entry.object);
    default:
      throw WorldException(binding, binding->environment.RuntimeError, "Context: trying to convert non-id/sym/int to RValue");
  }
}

RubyValue Context::resolve_local(const std::string &_identifier)
{
  std::map<std::string, RubyValue>::const_iterator iter = binding->locals.find(_identifier);
  if (iter != binding->locals.end())
    return iter->second;

  if (outer_context)
    return outer_context->resolve_local(_identifier);

  throw CannotFindLocalError();
}

RubyValue Context::resolve_identifier(const std::string &_identifier)
{
  if (_identifier[0] == '$') {
    if (binding->environment.global_exists(_identifier))
      return binding->environment.get_global_by_name(_identifier);
    return binding->environment.NIL;
  } else if (_identifier[0] == '@') {
    if (_identifier[1] == '@') {
      // @@var
      RubyClass *c = binding->context.get_class(binding->environment);
      if (c->has_class_variable(_identifier))
	return c->get_class_variable(_identifier);
      throw WorldException(binding, binding->environment.NameError, "uninitialized class variable " + _identifier + " in " + c->get_name());
    } else {
      // @var
      if (binding->context.has_instance(_identifier))
	return binding->context.get_instance(_identifier);
      return binding->environment.NIL;
    }
  } else if (isupper(_identifier[0]))
    return resolve_constant(_identifier);

  if (_identifier == "self")
    return binding->context;

  try {
    return resolve_local(_identifier);
  } catch (CannotFindLocalError)
  { }

  // how about environment globals? (<< XXX seems conceptually incorrect - should they be exposed any other way, logically?)
  try {
    return binding->environment.get_global_by_name(_identifier);
  } catch (CannotFindGlobalError)
  { }

  // methods
  try {
    return get_method(_identifier)->call(binding, binding->context, std::vector<RubyValue>());
  } catch (std::exception)	// XXX: better defined exception.
  { }

  throw WorldException(binding, binding->environment.NameError, "undefined local variable or method `" + _identifier + '\'');
}

RubyValue Context::resolve_constant(const std::string &_identifier)
{
  throw WorldException(binding, binding->environment.NameError, "unininitialized constant " + _identifier);
}

RubyMethod *Context::get_method(const std::string &_name)
{
  return binding->context.get_method(binding, _name);
}

void Context::assign(const std::string &_name, RubyValue _value)
{
  if (_name[0] == '$') {
    binding->environment.set_global_by_name(_name, _value);
    return;
  } else if (_name[0] == '@') {
    if (_name[1] == '@')
      binding->context.get_class(binding->environment)->set_class_variable(_name, _value);
    else
      binding->context.set_instance(_name, _value);
    return;
  }

  if (assign_if_exists(_name, _value))
    return;

  if (binding->locals.find(_name) != binding->locals.end()) {
    // XXX We're going to overwrite something.
    // XXX GC concerns?
  }

  binding->locals[_name] = _value;
}

bool Context::assign_if_exists(const std::string &_name, RubyValue _value)
{
  if (binding->locals.find(_name) != binding->locals.end()) {
    // overwriting.
    binding->locals[_name] = _value;
    return true;
  }

  if (outer_context)
    return outer_context->assign_if_exists(_name, _value);

  return false;
}

