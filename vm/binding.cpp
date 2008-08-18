#include "binding.h"
#include <iostream>

Binding::Binding(RubyValue _context): context(_context)
{
  std::cerr << "Binding()" << std::endl;
}

Binding::~Binding()
{
  std::cerr << "~Binding()" << std::endl;
}

RubyValue Binding::get_context() const
{
  return context;
}

