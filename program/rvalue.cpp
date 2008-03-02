#include "rvalue.h"

RubyValue::RubyValue()
{ }

RubyValue::~RubyValue()
{ }

RubyFixnumValue::RubyFixnumValue(long _value): value(_value)
{ }

long RubyFixnumValue::get_object_id() const
{
  return value * 2 + 1;
}

RubyObjectValue::RubyObjectValue(RubyObject *_value): value(_value)
{ }

long RubyObjectValue::get_object_id() const
{
  // objects are usually aligned, right?
  return (unsigned long)value;
}

long RubyNilValue::get_object_id() const
{
  return 4;
}

long RubyTrueValue::get_object_id() const
{
  return 2;
}

long RubyFalseValue::get_object_id() const
{
  return 0;
}

