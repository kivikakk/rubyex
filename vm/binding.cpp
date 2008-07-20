#include "binding.h"

Binding::Binding(RubyValue _context): context(_context)
{ }

RubyValue Binding::get_context() const
{
  return context;
}

