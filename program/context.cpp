#include "context.h"
#include <iostream>

Context::Context(RubyEnvironment *_environment, RubyObject *_context): environment(_environment), context(_context)
{ }

RubyEnvironment *Context::get_environment() const
{
  return environment;
}

RubyObject *Context::get_context() const
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

void Context::assign(const std::string &_name, RubyValue _value)
{
  if (locals.find(_name) != locals.end()) {
    // XXX We're going to overwrite something.
    // XXX GC concerns?
    ;
  }

  locals[_name] = _value;
}

RubyValue Context::entry_to_value(const Stack::StackEntry &entry)
{
  switch (entry.type) {
    case Stack::SE_IDENTIFIER:
      std::cerr << "not yet - need some serious changes - probably pass in the environment to e_t_v?" << std::endl;
      throw;
    case Stack::SE_SYMBOL: //return RubyValue::from_symbol(entry.
      std::cerr << "not yet - this is also in the context of the environment right? whoops." << std::endl;
      throw;
    case Stack::SE_INTEGER: return RubyValue::from_fixnum(entry.integer);
    case Stack::SE_OBJECT: return RubyValue::from_object(entry.object);
    case Stack::SE_BLOCK:
      std::cerr << ".. error?" << std::endl;
      throw;
  }
  throw;
}

