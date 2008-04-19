#include "context.h"

Context::Context(RubyObject *_context): context(_context)
{ }

RubyObject *Context::get_context() const
{
  return context;
}

