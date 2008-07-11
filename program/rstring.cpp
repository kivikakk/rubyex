#include "rstring.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue string_reverse(RubyEnvironment &, RubyValue);

void RubyStringEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cString = RubyClass::create_class(_e, "String");
  rb_cString->add_method("reverse", RubyMethod::Create(string_reverse));

  _e.add_class("String", rb_cString);
}

RubyValue string_reverse(RubyEnvironment &_e, RubyValue _self)
{
  std::cerr << "string_reverse called" << std::endl;
  return RubyValue::from_object(_e.NIL);
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

