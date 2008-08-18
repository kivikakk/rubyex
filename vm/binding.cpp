#include "binding.h"
#include <iostream>

Binding::Binding(RubyEnvironment *_environment, RubyValue _context): environment(_environment), context(_context)
{
  std::cerr << "Binding()" << std::endl;
}

Binding::~Binding()
{
  std::cerr << "~Binding()" << std::endl;
}

RubyEnvironment *Binding::get_environment() const
{
  return environment;
}

RubyValue Binding::get_context() const
{
  return context;
}

