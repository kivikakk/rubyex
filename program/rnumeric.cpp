#include "rnumeric.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

static RubyClass *rb_cFloat;

void RubyNumericEI::init(RubyEnvironment &e)
{
  // TODO: the whole Numeric, Fixnum, Bignum, etc. class hierarchy.

  rb_cFloat = RubyClass::create_class(e, "Float");

  e.add_class("Float", rb_cFloat);
}

RubyFloating::RubyFloating(double _floating_value): RubyObject(rb_cFloat), floating_value(_floating_value)
{ }

