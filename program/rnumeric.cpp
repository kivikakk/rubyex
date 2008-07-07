#include "rnumeric.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

RubyValue fixnum_add(RubyEnvironment &, RubyValue, RubyValue);

void RubyNumericEI::init(RubyEnvironment &e)
{
  // TODO: the whole Numeric, Fixnum, Bignum, etc. class hierarchy.
  RubyClass *rb_cFixnum = RubyClass::create_class(e, "Fixnum");
  rb_cFixnum->add_method("+", CMETHOD(fixnum_add, 1));
  e.add_class("Fixnum", rb_cFixnum);

  RubyClass *rb_cFloat = RubyClass::create_class(e, "Float");
  e.add_class("Float", rb_cFloat);
}

RubyValue fixnum_add(RubyEnvironment &e, RubyValue self, RubyValue operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(self.fixnum + operand.fixnum); // XXX bignum
}

RubyFloating::RubyFloating(RubyEnvironment &e, double _floating_value): RubyObject(new NamedLazyClass(e, "Float")), floating_value(_floating_value)
{ }

