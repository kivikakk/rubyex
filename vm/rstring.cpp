#include "rstring.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue string_new(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_add(Binding &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_reverse(Binding &, RubyValue);
RubyValue string_inspect(Binding &, RubyValue);
RubyValue string_to_s(Binding &, RubyValue);

void RubyStringEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cString = RubyClass::create_class(_e, "String");
  rb_cString->add_metaclass_method(_e, "new", RubyMethod::Create(string_new, ARGS_ARBITRARY));

  rb_cString->add_method("+", RubyMethod::Create(string_add, 1));
  rb_cString->add_method("reverse", RubyMethod::Create(string_reverse));
  rb_cString->add_method("inspect", RubyMethod::Create(string_inspect));
  rb_cString->add_method("to_s", RubyMethod::Create(string_to_s));

  _e.add_class("String", rb_cString);
  _e.String = rb_cString;
}

RubyValue string_new(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (_args.size() == 0)
    return RubyValue::from_object(new RubyString(_b.environment, ""));
  else if (_args.size() == 1)
    return RubyValue::from_object(new RubyString(_b.environment, dynamic_cast<RubyString *>(_args[0].object)->string_value));
  
  throw std::exception();	// TODO: Throw a real exception (ArgumentError)
}

RubyValue string_add(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyString *s1 = dynamic_cast<RubyString *>(_self.object),
	     *s2 = dynamic_cast<RubyString *>(_args[0].object);
  return RubyValue::from_object(_b.environment.gc.track(new RubyString(_b.environment, s1->string_value + s2->string_value)));
}

RubyValue string_reverse(Binding &_b, RubyValue _self)
{
  RubyString *r = dynamic_cast<RubyString *>(_self.object);
  return RubyValue::from_object(_b.environment.gc.track(new RubyString(_b.environment, std::string(r->string_value.rbegin(), r->string_value.rend()))));
}

RubyValue string_inspect(Binding &_b, RubyValue _self)
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
  return RubyValue::from_object(_b.environment.gc.track(new RubyString(_b.environment, o.str())));
}

RubyValue string_to_s(Binding &_b, RubyValue _self)
{
  return _self;
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

