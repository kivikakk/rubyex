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
  _e.String = rb_cString;
}

RubyValue string_reverse(RubyEnvironment &_e, RubyValue _self)
{
  RubyString *r = dynamic_cast<RubyString *>(_self.object);
  return RubyValue::from_object(_e.gc.track(new RubyString(_e, std::string(r->string_value.rbegin(), r->string_value.rend()))));
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

