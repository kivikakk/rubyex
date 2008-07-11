#include "context.h"
#include <iostream>
#include <exception>

Context::Context(RubyEnvironment *_environment, RubyValue _context): environment(_environment), context(_context)
{ }

RubyEnvironment *Context::get_environment() const
{
  return environment;
}

RubyValue Context::get_context() const
{
  return context;
}

RubyValue Context::entry_to_value(const Stack::StackEntry &_entry) const
{
  // We take control of StackEntry's memory. See a note in stack.cpp:pop_value about how we should do this better.

  switch (_entry.type) {
    case Stack::SE_IDENTIFIER: {
      std::string id = *_entry.identifier;
      delete _entry.identifier;
      
      // first look at locals.
      {
	std::map<std::string, RubyValue>::const_iterator iter = locals.find(id);
	if (iter != locals.end())
	  return iter->second;
      }

      // how about environment globals?
      try {
	return RubyValue::from_object(environment->get_global_by_name(id));
      } catch (std::exception)
      { }

      std::cerr << "Context::entry_to_value failing" << std::endl;
      throw;
    }

    case Stack::SE_SYMBOL: {
      std::string symbol = *_entry.symbol;
      delete _entry.symbol;
      return RubyValue::from_symbol(environment->get_symbol(symbol));
    }

    case Stack::SE_INTEGER: return RubyValue::from_fixnum(_entry.integer);
    case Stack::SE_OBJECT: return RubyValue::from_object(_entry.object);
    case Stack::SE_BLOCK:
      std::cerr << ".. error?" << std::endl;
      throw;
  }
  throw;
}

RubyMethod *Context::get_method(const std::string &_name)
{
  return context.get_method(_name, *environment);
}

void Context::assign(const std::string &_name, RubyValue _value)
{
  if (locals.find(_name) != locals.end()) {
    // XXX We're going to overwrite something.
    // XXX GC concerns?
    ;
  }

  locals[_name] = _value;
}

