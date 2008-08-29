#include <iostream>
#include "rvalue.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rexception.h"

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

bool RubyValue::operator <(const RubyValue &_rhs) const
{
  if (type != _rhs.type)
    return type < _rhs.type;
  return fixnum < _rhs.fixnum;
}

RubyValue RubyValue::from_fixnum(long _value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_symbol(RubySymbol *_value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_object(RubyObject *_value)
{ return RubyValue(_value); }

RubyClass *RubyValue::get_class(RubyEnvironment &_e) const
{
  switch (type) {
    case RubyValue::RV_FIXNUM: return _e.Fixnum;
    case RubyValue::RV_SYMBOL: return _e.Symbol;
    case RubyValue::RV_OBJECT: return object->get_class();
    default: throw SevereInternalError("RubyValue::get_class doesn't know its own type");
  }
}

// the inclusion of `environment' here as a parameter seems a cop-out
RubyMethod *RubyValue::get_method(linked_ptr<Binding> &_b, const std::string &_name) const
{
  // We need to find a method by this name in our context. We go through the class's ::ancestors.
  // Tradition has it:
  // 	[MyMetaClass, [XXX more MetaClass related?],
  // 		MyClass, MyClassIncludedModules, ...
  // 		SuperClass, SuperClassIncludedModules ...,
  // 		SuperSuperClass, SuperSuperClassIncludedModules ...,
  // 		...]

  switch (type) {
    case RubyValue::RV_FIXNUM: return _b->environment.Fixnum->find_method(_name);
    case RubyValue::RV_SYMBOL: return _b->environment.Symbol->find_method(_name);
    case RubyValue::RV_OBJECT: {
      // MyMetaClass
      RubyClass *c = object->get_metaclass_read();
      if (c && c->has_method(_name))
	return c->get_method(_name);
      
      // MyClass, MyClassIncludedModules, SuperClass, SuperClassIncludedModules, SuperSuperClass,
      // SuperSuperClassIncludedModules, ..., ...
      try {
	return object->get_class()->find_method(_name);
      } catch (ClassHasNoSuchMethodException) {
	throw WorldException(_b, _b->environment.NoMethodError,
	  std::string("undefined method `") + _name + "' for " + 
	  this->call(_b, "inspect").get_special<RubyString>()->string_value + ":" +
	  this->get_class(_b->environment)->get_name());
      }
      break;
    }
    default: throw SevereInternalError("RubyValue::get_method doesn't know its own type");
  }
}

bool RubyValue::truthy(RubyEnvironment &_e) const
{
  switch (type) {
    case RubyValue::RV_FIXNUM: return true;
    case RubyValue::RV_SYMBOL: return true;
    case RubyValue::RV_OBJECT: {
      if (object == _e.FALSE.object)
	return false;
      else if (object == _e.NIL.object)
	return false;

      // Every other object is truthy.
      return true;
    }
    default: throw SevereInternalError("RubyValue::truthy doesn't know its own type");
  }
}

long RubyValue::get_fixnum() const
{
  if (type != RubyValue::RV_FIXNUM)
    throw SevereInternalError("RubyValue::get_fixnum is called on not a fixnum");

  return fixnum;
}
RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name) const
{ return get_method(_b, _name)->call(_b, *this); }

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  return get_method(_b, _name)->call(_b, *this, args);
}
RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0, RubyValue _arg1) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  return get_method(_b, _name)->call(_b, *this, args);
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0, RubyValue _arg1, RubyValue _arg2) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  args.push_back(_arg2);
  return get_method(_b, _name)->call(_b, *this, args);
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, const std::vector<RubyValue> &_args) const
{
  return get_method(_b, _name)->call(_b, *this, _args);
}


RubyValue::RubyValue(long _value): type(RV_FIXNUM), fixnum(_value)
{ }

RubyValue::RubyValue(RubySymbol *_value): type(RV_SYMBOL), symbol(_value)
{ }

RubyValue::RubyValue(RubyObject *_value): type(RV_OBJECT), object(_value)
{ }
