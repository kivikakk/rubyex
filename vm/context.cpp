#include <iostream>
#include <exception>
#include "context.h"
#include "rmethod.h"
#include "rexception.h"

Context::Context(RubyEnvironment &_environment, RubyValue _context, RubyClass *_def_target, Context *_outer_context): binding(linked_ptr<Binding>(new Binding(_environment, _context, _def_target))), outer_context(_outer_context)
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
      return RubyValue::from_symbol(binding->environment.get_symbol(symbol));
    }

    case Stack::SE_INTEGER: return RubyValue::from_fixnum(_entry.integer);
    case Stack::SE_OBJECT: return RubyValue::from_object(_entry.object);
    default:
      std::cerr << "Context: trying to convert non-id/sym/int to RValue" << std::endl;
      throw;
  }
  throw;
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
  try {
    return resolve_local(_identifier);
  } catch (CannotFindLocalError)
  { }

  // how about environment globals? (<< XXX seems conceptually incorrect - should they be exposed any other way, logically?)
  try {
    return RubyValue::from_object(binding->environment.get_global_by_name(_identifier));
  } catch (CannotFindGlobalError)
  { }

  // methods
  try {
    return get_method(_identifier)->call(binding, binding->context, std::vector<RubyValue>());
  } catch (std::exception)	// XXX: better defined exception.
  { }

  throw WorldException(binding, binding->environment.NameError, std::string("undefined local variable or method `") + _identifier + '\'');
}

RubyMethod *Context::get_method(const std::string &_name)
{
  return binding->context.get_method(binding, _name);
}

void Context::assign(const std::string &_name, RubyValue _value)
{
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

