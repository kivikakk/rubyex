#include "rtri.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

void RubyTriEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cTrueClass = RubyClass::create_class(e, "TrueClass");
  RubyClass *rb_cFalseClass = RubyClass::create_class(e, "FalseClass");
  RubyClass *rb_cNilClass = RubyClass::create_class(e, "NilClass");

  e.add_class("TrueClass", rb_cTrueClass);
  e.add_class("FalseClass", rb_cFalseClass);
  e.add_class("NilClass", rb_cNilClass);

  // TODO: e.NIL, e.FALSE, e.TRUE
}

