#include "context.h"
#include <iostream>
#include <exception>

Context::Context(RubyEnvironment &_environment, RubyValue _context): binding(linked_ptr<Binding>(new Binding(_environment, _context)))
{ }

Context::Context(linked_ptr<Binding> &_binding): binding(_binding)
{ }

RubyValue Context::entry_to_value(const Stack::StackEntry &_entry) const
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
    case Stack::SE_BLOCK:
      std::cerr << ".. error?" << std::endl;
      throw;
  }
  throw;
}

RubyValue Context::resolve_identifier(const std::string &_identifier) const
{
  // first look at locals.
  {
    std::map<std::string, RubyValue>::const_iterator iter = binding->locals.find(_identifier);
    if (iter != binding->locals.end())
      return iter->second;
  }

  // how about environment globals? (<< XXX seems conceptually incorrect - should they be exposed any other way, logically?)
  try {
    return RubyValue::from_object(binding->environment.get_global_by_name(_identifier));
  } catch (std::exception)
  { }

  // methods

  std::cerr << "Context::entry_to_value failing" << std::endl;
  throw;
}

RubyMethod *Context::get_method(const std::string &_name)
{
  return binding->context.get_method(_name, binding->environment);
}

void Context::assign(const std::string &_name, RubyValue _value)
{
  if (binding->locals.find(_name) != binding->locals.end()) {
    // XXX We're going to overwrite something.
    // XXX GC concerns?
    ;
  }

  binding->locals[_name] = _value;
}

