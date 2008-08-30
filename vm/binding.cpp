#include "binding.h"
#include "rvalue.h"
#include <iostream>
#include <sstream>

Binding::Binding(RubyEnvironment &_environment, RubyValue _context, RubyModule *_def_target): environment(_environment), context(_context), def_target(_def_target)
{ }

Binding::~Binding()
{ }

std::string Binding::get_representation(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args)
{
  std::ostringstream args;
  args << "[";
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    if (it != _args.begin()) args << ", ";
    args << it->call(_b, "inspect").get_special<RubyString>()->string_value;
  }
  args << "]";

  return args.str();
}

