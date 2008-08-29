#include "binding.h"
#include "rvalue.h"
#include <iostream>

Binding::Binding(RubyEnvironment &_environment, RubyValue _context, RubyModule *_def_target): environment(_environment), context(_context), def_target(_def_target)
{ }

Binding::~Binding()
{ }

