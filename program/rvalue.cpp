#include "rvalue.h"

RubyValue::RubyValue()
{ }

RubyValue::~RubyValue()
{ }

RubyFixnumValue::RubyFixnumValue(long _value): value(_value)
{ }

RubyObjectValue::RubyObjectValue(RubyObject *_value): value(_value)
{ }

