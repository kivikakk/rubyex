#include "rsymbol.h"

RubySymbol::RubySymbol(const std::string &_value): value(_value)
{ }

const std::string &RubySymbol::get_value() const
{
  return value;
}

