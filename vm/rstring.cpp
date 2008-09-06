#include "rstring.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "rvalue.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rexception.h"

RubyValue string_new(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_eq(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_add(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_reverse(linked_ptr<Binding> &, RubyValue);
RubyValue string_length(linked_ptr<Binding> &, RubyValue);
RubyValue string_strip(linked_ptr<Binding> &, RubyValue);
RubyValue string_strip_bang(linked_ptr<Binding> &, RubyValue);
RubyValue string_capitalize(linked_ptr<Binding> &, RubyValue);
RubyValue string_index(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue string_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue string_to_s(linked_ptr<Binding> &, RubyValue);
RubyValue string_to_sym(linked_ptr<Binding> &, RubyValue);

void RubyStringEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cString = RubyClass::create_class(_e, "String");
  rb_cString->add_metaclass_method(_e, "new", RubyMethod::Create(string_new, ARGS_ARBITRARY));

  rb_cString->add_method("==", RubyMethod::Create(string_eq, 1));
  rb_cString->add_method("+", RubyMethod::Create(string_add, 1));
  rb_cString->add_method("reverse", RubyMethod::Create(string_reverse));
  rb_cString->add_method("length", RubyMethod::Create(string_length));
  rb_cString->add_method("strip", RubyMethod::Create(string_strip));
  rb_cString->add_method("strip!", RubyMethod::Create(string_strip_bang));
  rb_cString->add_method("capitalize", RubyMethod::Create(string_capitalize));
  rb_cString->add_method("index", RubyMethod::Create(string_index, 1));
  rb_cString->add_method("inspect", RubyMethod::Create(string_inspect));
  rb_cString->add_method("to_s", RubyMethod::Create(string_to_s));
  rb_cString->add_method("to_sym", RubyMethod::Create(string_to_sym));

  _e.set_global_by_name("String", rb_cString);
  _e.String = rb_cString;
}

RubyValue string_new(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (_args.size() == 0)
    return _b->environment.get_string(std::string());
  else if (_args.size() == 1)
    return _b->environment.get_string(_args[0].get_string());
  
  throw WorldException(_b, _b->environment.RuntimeError, "string_new: given invalid number of args -- big problem");
}

RubyValue string_eq(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyString *s1 = _self.get_special<RubyString>(),
	     *s2 = _args[0].get_special<RubyString>();
  /* TODO: string interning? */

  return _b->environment.get_truth(s2 ? (s1->string_value == s2->string_value) : false);
}

RubyValue string_add(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return _b->environment.get_string(_self.get_string() + _args[0].get_string());
}

RubyValue string_reverse(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyString *r = _self.get_special<RubyString>();
  return _b->environment.get_string(std::string(r->string_value.rbegin(), r->string_value.rend()));
}

RubyValue string_length(linked_ptr<Binding> &_b, RubyValue _self)
{
  return F2V(_self.get_string().length());
}

RubyValue string_strip(linked_ptr<Binding> &_b, RubyValue _self)
{
  std::string ns = _self.get_special<RubyString>()->string_value;
  std::string::size_type first = ns.find_first_not_of(" \t\r\n");
  ns = (first == std::string::npos) ? std::string() : ns.substr(first, ns.find_last_not_of(" \t\r\n") - first + 1);
  return _b->environment.get_string(ns);
}

RubyValue string_strip_bang(linked_ptr<Binding> &_b, RubyValue _self)
{
  /* `optimized' by not just calling #strip. right. */
  std::string ns = _self.get_special<RubyString>()->string_value;
  std::string::size_type first = ns.find_first_not_of(" \t\r\n");
  _self.get_special<RubyString>()->string_value = (first == std::string::npos) ? std::string() : ns.substr(first, ns.find_last_not_of(" \t\r\n") - first + 1);
  return _self;
}

RubyValue string_capitalize(linked_ptr<Binding> &_b, RubyValue _self)
{
  std::string s = _self.get_string();
  s[0] = toupper(s[0]);
  return _b->environment.get_string(s);
}

RubyValue string_index(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  unsigned int i = _self.get_string().find(_args[0].get_string());
  if (i == std::string::npos)
    return _b->environment.NIL;
  return F2V(i);
}

RubyValue string_inspect(linked_ptr<Binding> &_b, RubyValue _self)
{
  const std::string &v = _self.get_string();
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
  return _b->environment.get_string(o.str());
}

RubyValue string_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  return _self;
}

RubyValue string_to_sym(linked_ptr<Binding> &_b, RubyValue _self)
{
  return S2V(_b->environment.get_symbol(_self.get_string()));
}

RubyString::RubyString(RubyEnvironment &_e, const std::string &_string_value): RubyObject(new NamedLazyClass(_e, "String")), string_value(_string_value)
{ }

