#include "context.h"
#include <iostream>

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

void Context::_report() const
{
  std::cerr << "Context: " << locals.size() << " local(s)";
  if (locals.size() > 0) {
    std::cerr << ": ";
    for (std::map<std::string, RubyValue>::const_iterator it = locals.begin(); it != locals.end(); ++it) {
      if (it != locals.begin())
	std::cerr << ", ";
      std::cerr << it->first;
    }
  }
  std::cerr << std::endl;
}

RubyValue Context::entry_to_value(const Stack::StackEntry &entry) const
{
  // We take control of StackEntry's memory. See a note in stack.cpp:pop_value about how we should do this better.

  switch (entry.type) {
    case Stack::SE_IDENTIFIER: {
      std::string id = *entry.identifier;
      delete entry.identifier;
      
      // first look at locals.
      {
	std::map<std::string, RubyValue>::const_iterator iter = locals.find(id);
	if (iter != locals.end())
	  return iter->second;
      }

      std::cerr << "Context::entry_to_value failing" << std::endl;
      throw;
    }

    case Stack::SE_SYMBOL: {
      std::string symbol = *entry.symbol;
      delete entry.symbol;
      return RubyValue::from_symbol(environment->get_symbol(symbol));
    }

    case Stack::SE_INTEGER: return RubyValue::from_fixnum(entry.integer);
    case Stack::SE_OBJECT: return RubyValue::from_object(entry.object);
    case Stack::SE_BLOCK:
      std::cerr << ".. error?" << std::endl;
      throw;
  }
  throw;
}

RubyMethod *Context::get_method(const std::string &name)
{
  return context.get_method(name, environment);
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

