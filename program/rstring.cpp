#include "rstring.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

RubyValue string_reverse(RubyEnvironment &, RubyObject *);

void RubyStringEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cString = RubyClass::create_class(e, "String");
  rb_cString->add_method("reverse", COMETHOD(string_reverse, 0));

  e.add_class("String", rb_cString);
}

RubyValue string_reverse(RubyEnvironment &e, RubyObject *self)
{
  std::cerr << "string_reverse called" << std::endl;
  return RubyValue::from_object(e.NIL);
}

RubyString::RubyString(RubyEnvironment &e, const std::string &_string_value): RubyObject(new NamedLazyClass(e, "String")), string_value(_string_value)
{ }

