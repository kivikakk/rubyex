#include "rtri.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue tfn_eql(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue true_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue false_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue nil_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue nil_to_s(linked_ptr<Binding> &, RubyValue);
RubyValue nil_nil(linked_ptr<Binding> &, RubyValue);

void RubyTriEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cTrueClass = RubyClass::create_class(_e, "TrueClass"),
	    *rb_cFalseClass = RubyClass::create_class(_e, "FalseClass"),
	    *rb_cNilClass = RubyClass::create_class(_e, "NilClass");

  rb_cTrueClass->add_method("==", RubyMethod::Create(tfn_eql, 1));
  rb_cFalseClass->add_method("==", RubyMethod::Create(tfn_eql, 1));
  rb_cNilClass->add_method("==", RubyMethod::Create(tfn_eql, 1));

  rb_cTrueClass->add_method("inspect", RubyMethod::Create(true_inspect));
  rb_cFalseClass->add_method("inspect", RubyMethod::Create(false_inspect));
  rb_cNilClass->add_method("inspect", RubyMethod::Create(nil_inspect));
  rb_cNilClass->add_method("to_s", RubyMethod::Create(nil_to_s));
  rb_cNilClass->add_method("nil?", RubyMethod::Create(nil_nil));

  _e.add_class("TrueClass", rb_cTrueClass);
  _e.add_class("FalseClass", rb_cFalseClass);
  _e.add_class("NilClass", rb_cNilClass);

  RubyObject *rb_oTrue =  rb_cTrueClass->new_instance(_e),
	     *rb_oFalse = rb_cFalseClass->new_instance(_e),
	     *rb_oNil =   rb_cNilClass->new_instance(_e);
  
  _e.TRUE = RubyValue::from_object(rb_oTrue);
  _e.FALSE = RubyValue::from_object(rb_oFalse);
  _e.NIL = RubyValue::from_object(rb_oNil);
}

RubyValue tfn_eql(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{ return _b->environment.get_truth(_self.object == _args[0].object); }

RubyValue true_inspect(linked_ptr<Binding> &_b, RubyValue)
{ return _b->environment.get_string("true"); }

RubyValue false_inspect(linked_ptr<Binding> &_b, RubyValue)
{ return _b->environment.get_string("false"); }

RubyValue nil_inspect(linked_ptr<Binding> &_b, RubyValue)
{ return _b->environment.get_string("nil"); }

RubyValue nil_to_s(linked_ptr<Binding> &_b, RubyValue)
{ return _b->environment.get_string(std::string()); }

RubyValue nil_nil(linked_ptr<Binding> &_b, RubyValue)
{ return _b->environment.TRUE; }

