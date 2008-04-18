#include "rstring.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

RubyValue *string_reverse(RubyValue *);

void RubyStringEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cString = RubyClass::create_class(e, "String");
  rb_cString->add_method("reverse", CMETHOD(string_reverse, 0));

  e.add_class("String", rb_cString);
}

RubyValue *string_reverse(RubyValue *self)
{
  std::cerr << "string_reverse called" << std::endl;
  // ?
}

