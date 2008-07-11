#include "rnumeric.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue fixnum_add(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);

void RubyNumericEI::init(RubyEnvironment &_e)
{
  // TODO: the whole Numeric, Fixnum, Bignum, etc. class hierarchy.
  RubyClass *rb_cFixnum = RubyClass::create_class(_e, "Fixnum");
  rb_cFixnum->add_method("+", RubyMethod::Create(fixnum_add, 1));
  _e.add_class("Fixnum", rb_cFixnum);

  RubyClass *rb_cFloat = RubyClass::create_class(_e, "Float");
  _e.add_class("Float", rb_cFloat);
}

RubyValue fixnum_add(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(_self.fixnum + _operand[0].fixnum); // XXX bignum
}

RubyFloating::RubyFloating(RubyEnvironment &_e, double _floating_value): RubyObject(new NamedLazyClass(_e, "Float")), floating_value(_floating_value)
{ }

