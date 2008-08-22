#include "binding.h"
#include "rvalue.h"
#include <iostream>

Binding::Binding(RubyEnvironment &_environment, RubyValue _context): environment(_environment), context(_context)
{ }

Binding::~Binding()
{ }

