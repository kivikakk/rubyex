#include "rtri.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue true_inspect(RubyEnvironment &, RubyValue);
RubyValue false_inspect(RubyEnvironment &, RubyValue);
RubyValue nil_inspect(RubyEnvironment &, RubyValue);
RubyValue nil_to_s(RubyEnvironment &, RubyValue);

void RubyTriEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cTrueClass = RubyClass::create_class(_e, "TrueClass"),
	    *rb_cFalseClass = RubyClass::create_class(_e, "FalseClass"),
	    *rb_cNilClass = RubyClass::create_class(_e, "NilClass");

  _e.add_class("TrueClass", rb_cTrueClass);
  _e.add_class("FalseClass", rb_cFalseClass);
  _e.add_class("NilClass", rb_cNilClass);

  RubyObject *rb_oTrue =  rb_cTrueClass->new_instance(),
	     *rb_oFalse = rb_cFalseClass->new_instance(),
	     *rb_oNil =    rb_cNilClass->new_instance();
  
  rb_oTrue->add_metaclass_method(_e, "inspect", RubyMethod::Create(true_inspect));
  rb_oFalse->add_metaclass_method(_e, "inspect", RubyMethod::Create(false_inspect));
  rb_oNil->add_metaclass_method(_e, "inspect", RubyMethod::Create(nil_inspect));
  rb_oNil->add_metaclass_method(_e, "to_s", RubyMethod::Create(nil_to_s));

  _e.TRUE = RubyValue::from_object(rb_oTrue);
  _e.FALSE = RubyValue::from_object(rb_oFalse);
  _e.NIL = RubyValue::from_object(rb_oNil);
}

RubyValue true_inspect(RubyEnvironment &_e, RubyValue)
{ return RubyValue::from_object(new RubyString(_e, "true")); }

RubyValue false_inspect(RubyEnvironment &_e, RubyValue)
{ return RubyValue::from_object(new RubyString(_e, "false")); }

RubyValue nil_inspect(RubyEnvironment &_e, RubyValue)
{ return RubyValue::from_object(new RubyString(_e, "nil")); }

RubyValue nil_to_s(RubyEnvironment &_e, RubyValue)
{ return RubyValue::from_object(new RubyString(_e, "")); }

