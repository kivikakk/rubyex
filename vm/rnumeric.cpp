#include "rnumeric.h"
#include <iostream>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue fixnum_add(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue fixnum_multiply(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue fixnum_inspect_to_s(linked_ptr<Binding> &, RubyValue);
RubyValue fixnum_times(linked_ptr<Binding> &, RubyValue, Block &);
RubyValue fixnum_upto(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

void RubyNumericEI::init(RubyEnvironment &_e)
{
  // TODO: the whole Numeric, Fixnum, Bignum, etc. class hierarchy.
  RubyClass *rb_cFixnum = RubyClass::create_class(_e, "Fixnum");
  // TODO: undefine Fixnum's `new'? or just redefine Numeric#new to throw a method not found error?
  rb_cFixnum->add_method("+", RubyMethod::Create(fixnum_add, 1));
  rb_cFixnum->add_method("*", RubyMethod::Create(fixnum_multiply, 1));

  rb_cFixnum->add_method("inspect", RubyMethod::Create(fixnum_inspect_to_s));
  rb_cFixnum->add_method("to_s", RubyMethod::Create(fixnum_inspect_to_s));
  rb_cFixnum->add_method("times", RubyMethod::Create(fixnum_times));
  rb_cFixnum->add_method("upto", RubyMethod::Create(fixnum_upto, 1));

  _e.add_class("Fixnum", rb_cFixnum);
  _e.Fixnum = rb_cFixnum;

  RubyClass *rb_cFloat = RubyClass::create_class(_e, "Float");
  _e.add_class("Float", rb_cFloat);
}

RubyValue fixnum_add(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(_self.fixnum + _operand[0].fixnum); // XXX bignum
}

RubyValue fixnum_multiply(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_operand)
{
  // XXX: typecheck `operand'
  return RubyValue::from_fixnum(_self.fixnum * _operand[0].fixnum); // XXX bignum
}

RubyValue fixnum_inspect_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  std::ostringstream oss;
  oss << _self.fixnum;
  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, oss.str())));
}

RubyValue fixnum_times(linked_ptr<Binding> &_b, RubyValue _self, Block &_block)
{
  for (int i = 0; i < _self.fixnum; ++i)
    _block.call(_b);
  return _self;
}

RubyValue fixnum_upto(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_top, Block &_block)
{
  long top = _top[0].fixnum;
  if (_top[0].type != RubyValue::RV_FIXNUM) {	// XXX: Bignum?
    std::cerr << "fixnum_upto: not a numeric argument" << std::endl;	// XXX: exception
    throw;
  }

  for (long i = _self.fixnum; i <= top; ++i)
    _block.call(_b, RubyValue::from_fixnum(i));
  return _self;
}

RubyFloating::RubyFloating(RubyEnvironment &_e, double _floating_value): RubyObject(new NamedLazyClass(_e, "Float")), floating_value(_floating_value)
{ }

