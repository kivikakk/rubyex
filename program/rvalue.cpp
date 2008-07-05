#include "rvalue.h"

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

RubyValue RubyValue::from_fixnum(long value)
{ return RubyValue(value); }

RubyValue RubyValue::from_symbol(RubySymbol *value)
{ return RubyValue(value); }

RubyValue RubyValue::from_object(RubyObject *value)
{ return RubyValue(value); }

RubyValue::RubyValue(long value): type(RV_FIXNUM), fixnum(value)
{ }

RubyValue::RubyValue(RubySymbol *value): type(RV_SYMBOL), symbol(value)
{ }

RubyValue::RubyValue(RubyObject *value): type(RV_FIXNUM), object(value)
{ }
