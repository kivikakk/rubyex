#include "context.h"

Context::Context(RubyObject *_context): context(_context)
{ }

RubyObject *Context::get_context() const
{
  return context;
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

