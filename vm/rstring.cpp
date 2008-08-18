#include "rstring.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue string_add(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_reverse(RubyEnvironment &, RubyValue);
RubyValue string_inspect(RubyEnvironment &, RubyValue);
RubyValue string_to_s(RubyEnvironment &, RubyValue);

void RubyStringEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cString = RubyClass::create_class(_e, "String");
  rb_cString->add_method("+", RubyMethod::Create(string_add, 1));
  rb_cString->add_method("reverse", RubyMethod::Create(string_reverse));
  rb_cString->add_method("inspect", RubyMethod::Create(string_inspect));
  rb_cString->add_method("to_s", RubyMethod::Create(string_to_s));

  _e.add_class("String", rb_cString);
  _e.String = rb_cString;
}

RubyValue string_add(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyString *s1 = dynamic_cast<RubyString *>(_self.object),
	     *s2 = dynamic_cast<RubyString *>(_args[0].object);
  return RubyValue::from_object(_e.gc.track(new RubyString(_e, s1->string_value + s2->string_value)));
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
  o << '"';
  for (unsigned int i = 0; i < v.length(); ++i)
    if (v[i] < 0x20 || v[i] > 0x7e)
      switch (v[i]) {
	case 0x7: o << "\\a"; break;
	case 0x8: o << "\\b"; break;
	case 0x9: o << "\\t"; break;
	case 0xa: o << "\\n"; break;
	case 0xb: o << "\\v"; break;
	case 0xc: o << "\\f"; break;
	case 0xd: o << "\\r"; break;
	default: o << "\\" << std::oct << std::setiosflags(std::ios::right) << std::setfill('0') << std::setw(3) << (unsigned int)(unsigned char)v[i]; break;
      }
    else if (v[i] == '"')
      o << "\\\"";
    else if (v[i] == '\\')
      o << "\\\\";
    else
      o << (char)v[i];
  o << '"';
  return RubyValue::from_object(_e.gc.track(new RubyString(_e, o.str())));
}

RubyValue string_to_s(RubyEnvironment &_e, RubyValue _self)
{
  return _self;
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

