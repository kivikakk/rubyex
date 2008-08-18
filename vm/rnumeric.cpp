#include "rnumeric.h"
#include <iostream>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue fixnum_add(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue fixnum_multiply(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue fixnum_inspect_to_s(Binding &, RubyValue);
RubyValue fixnum_times(Binding &, RubyValue);

void RubyNumericEI::init(RubyEnvironment &_e)
{
  // TODO: the whole Numeric, Fixnum, Bignum, etc. class hierarchy.
  RubyClass *rb_cFixnum = RubyClass::create_class(_e, "Fixnum");
  // TODO: undefine Fixnum's `new'?
  rb_cFixnum->add_method("+", RubyMethod::Create(fixnum_add, 1));
  rb_cFixnum->add_method("*", RubyMethod::Create(fixnum_multiply, 1));

  rb_cFixnum->add_method("inspect", RubyMethod::Create(fixnum_inspect_to_s));
  rb_cFixnum->add_method("to_s", RubyMethod::Create(fixnum_inspect_to_s));
  rb_cFixnum->add_method("times", RubyMethod::Create(fixnum_times));

  _e.add_class("Fixnum", rb_cFixnum);
  _e.Fixnum = rb_cFixnum;

  RubyClass *rb_cFloat = RubyClass::create_class(_e, "Float");
  _e.add_class("Float", rb_cFloat);
}

RubyValue fixnum_add(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(_self.fixnum + _operand[0].fixnum); // XXX bignum
}

RubyValue fixnum_multiply(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(_self.fixnum * _operand[0].fixnum); // XXX bignum
}

RubyValue fixnum_inspect_to_s(Binding &_b, RubyValue _self)
{
  std::ostringstream oss;
  oss << _self.fixnum;
  return RubyValue::from_object(_b.environment.gc.track(new RubyString(_b.environment, oss.str())));
}

RubyValue fixnum_times(Binding &_b, RubyValue _self)
{
  for (int i = 0; i < _self.fixnum; ++i)
    /* XXX yield */ ;
  return _self;
}

RubyFloating::RubyFloating(RubyEnvironment &_e, double _floating_value): RubyObject(new NamedLazyClass(_e, "Float")), floating_value(_floating_value)
{ }

