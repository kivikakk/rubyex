#include "rtri.h"
#include <iostream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"

void RubyTriEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cTrueClass = RubyClass::create_class(_e, "TrueClass");
  RubyClass *rb_cFalseClass = RubyClass::create_class(_e, "FalseClass");
  RubyClass *rb_cNilClass = RubyClass::create_class(_e, "NilClass");

  _e.add_class("TrueClass", rb_cTrueClass);
  _e.add_class("FalseClass", rb_cFalseClass);
  _e.add_class("NilClass", rb_cNilClass);

  // TODO: e.NIL, e.FALSE, e.TRUE
}

