#include "rstring.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue string_reverse(RubyEnvironment &, RubyValue);
RubyValue string_inspect(RubyEnvironment &, RubyValue);

void RubyStringEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cString = RubyClass::create_class(_e, "String");
  rb_cString->add_method("reverse", RubyMethod::Create(string_reverse));
  rb_cString->add_method("inspect", RubyMethod::Create(string_inspect));

  _e.add_class("String", rb_cString);
  _e.String = rb_cString;
}

RubyValue string_reverse(RubyEnvironment &_e, RubyValue _self)
{
  RubyString *r = dynamic_cast<RubyString *>(_self.object);
  return RubyValue::from_object(_e.gc.track(new RubyString(_e, std::string(r->string_value.rbegin(), r->string_value.rend()))));
}

RubyValue string_inspect(RubyEnvironment &_e, RubyValue _self)
{
  std::string &v = dynamic_cast<RubyString *>(_self.object)->string_value;
  std::ostringstream o;
  o << '"' << std::hex;
  for (unsigned int i = 0; i < v.length(); ++i)
    if (v[i] < 0x20 || v[i] > 0x7e)
      o << "\\x" << (unsigned int)v[i];
    else if (v[i] == '"')
      o << "\\\"";
    else if (v[i] == '\\')
      o << "\\\\";
    else
      o << (char)v[i];
  o << '"';
  return RubyValue::from_object(_e.gc.track(new RubyString(_e, o.str())));
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

