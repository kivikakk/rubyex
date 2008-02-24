#include "rstring.h"
#include <iostream>

RubyValue *string_reverse(RubyValue *);

void RubyString::init(RubyEnvironment &e)
{
  RubyClass *rb_cString = new RubyClass("String");
  rb_cString->add_method("reverse", CMETHOD(string_reverse, 0));

  e.add_class("String", rb_cString);
}

RubyValue *string_reverse(RubyValue *self)
{
  std::cerr << "string_reverse called" << std::endl;
  return RubyEnvironment::NIL;
}

