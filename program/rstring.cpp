#include "rstring.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue string_reverse(RubyEnvironment &, RubyValue);

void RubyStringEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cString = RubyClass::create_class(e, "String");
  rb_cString->add_method("reverse", CMETHOD(string_reverse, 0));

  e.add_class("String", rb_cString);
}

RubyValue string_reverse(RubyEnvironment &e, RubyValue self)
{
  std::cerr << "string_reverse called" << std::endl;
  return RubyValue::from_object(e.NIL);
}

RubyString::RubyString(RubyEnvironment &e, const std::string &_string_value): RubyObject(new NamedLazyClass(e, "String")), string_value(_string_value)
{ }

