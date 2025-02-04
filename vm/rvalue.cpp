#include <iostream>
#include "rvalue.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rexception.h"

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

bool RubyValue::operator <(const RubyValue &_rhs) const {
  if (type != _rhs.type)
    return type < _rhs.type;
  return fixnum < _rhs.fixnum;
}

bool RubyValue::operator ==(const RubyValue &_rhs) const {
  return (type == _rhs.type) && (fixnum == _rhs.fixnum);
}

bool RubyValue::operator !=(const RubyValue &_rhs) const {
  return (type != _rhs.type) || (fixnum != _rhs.fixnum);
}

RubyValue RubyValue::from_fixnum(long _value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_symbol(RubySymbol *_value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_object(RubyObject *_value)
{ return RubyValue(_value); }

RubyClass *RubyValue::get_class(RubyEnvironment &_e) const {
  switch (type) {
    case RubyValue::RV_FIXNUM: return _e.Fixnum;
    case RubyValue::RV_SYMBOL: return _e.Symbol;
    case RubyValue::RV_OBJECT: return object->get_class();
    default: throw SevereInternalError("RubyValue::get_class doesn't know its own type");
  }
}

// the inclusion of `environment' here as a parameter seems a cop-out
const linked_ptr<RubyMethod> &RubyValue::get_method(linked_ptr<Binding> &_b, const std::string &_name) const {
  // We need to find a method by this name in our context. We go through the class's ::ancestors.
  // Tradition has it:
  // 	[MyMetaClass, [XXX more MetaClass related?],
  // 		MyClass, MyClassIncludedModules, ...
  // 		SuperClass, SuperClassIncludedModules ...,
  // 		SuperSuperClass, SuperSuperClassIncludedModules ...,
  // 		...]

    try {
    switch (type) {
      case RubyValue::RV_FIXNUM: return _b->environment.Fixnum->find_method(_name);
      case RubyValue::RV_SYMBOL: return _b->environment.Symbol->find_method(_name);
      case RubyValue::RV_OBJECT: {
	// MyMetaClass, MyMetaClassIncludedModules, MyMetaClassSuperClass, ..
	RubyClass *c = object->get_metaclass_read();
	if (c)
	  try {
	    return c->find_method(_name);
	  } catch (ClassHasNoSuchMethodException) { }
	
	// MyClass, MyClassIncludedModules, SuperClass, SuperClassIncludedModules, SuperSuperClass,
	// SuperSuperClassIncludedModules, ..., ...
	return object->get_class()->find_method(_name);
	break;
      }
      default: throw SevereInternalError("RubyValue::get_method doesn't know its own type");
    }
  } catch (ClassHasNoSuchMethodException) {
    throw WorldException(_b, _b->environment.NoMethodError,
      "undefined method `" + _name + "' for " + 
      this->inspect(_b) + ":" +
      this->get_class(_b->environment)->get_name());
  }
}

bool RubyValue::has_instance(const std::string &_name) const {
  if (type != RV_OBJECT)
    throw SevereInternalError("RubyValue::get_instance(): not implemented for non-objects yet.");
  return object->has_instance(_name);
}

RubyValue RubyValue::get_instance(const std::string &_name) const {
  if (type != RV_OBJECT)
    throw SevereInternalError("RubyValue::get_instance(): not implemented for non-objects yet.");
  return object->get_instance(_name);
}

void RubyValue::set_instance(const std::string &_name, RubyValue _value) {
  if (type != RV_OBJECT)
    throw SevereInternalError("RubyValue::get_instance(): not implemented for non-objects yet.");
  object->set_instance(_name, _value);
}

bool RubyValue::ruby_eq_op(linked_ptr<Binding> &_b, RubyValue _rhs) const {
  // what if == doesn't return a boolean?
  return call(_b, "==", _rhs).truthy(_b->environment);
}

bool RubyValue::truthy(RubyEnvironment &_e) const {
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

long RubyValue::get_fixnum() const {
  if (type != RubyValue::RV_FIXNUM)
    throw SevereInternalError("RubyValue::get_fixnum is called on not a fixnum");
  return fixnum;
}

std::string RubyValue::to_s(linked_ptr<Binding> &_b) const {
  return call(_b, "to_s").get_string();
}

std::string RubyValue::inspect(linked_ptr<Binding> &_b) const {
  return call(_b, "inspect").get_string();
}

const std::string &RubyValue::get_string() const {
  if (!is_a<RubyString>())
    throw SevereInternalError("RubyValue::get_string is called on not a string");
  return get_special<RubyString>()->string_value;
}

void RubyValue::set_string(const std::string &_rhs) const {
  if (!is_a<RubyString>())
    throw SevereInternalError("RubyValue::set_string is called on not a string");
  get_special<RubyString>()->string_value = _rhs;
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name) const
{ return get_method(_b, _name)->call(_b, *this); }

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0) const {
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  return get_method(_b, _name)->call(_b, *this, args);
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0, RubyValue _arg1) const {
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  return get_method(_b, _name)->call(_b, *this, args);
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, RubyValue _arg0, RubyValue _arg1, RubyValue _arg2) const {
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  args.push_back(_arg2);
  return get_method(_b, _name)->call(_b, *this, args);
}

RubyValue RubyValue::call(linked_ptr<Binding> &_b, const std::string &_name, const std::vector<RubyValue> &_args) const {
  return get_method(_b, _name)->call(_b, *this, _args);
}


RubyValue::RubyValue(long _value): type(RV_FIXNUM), fixnum(_value)
{ }

RubyValue::RubyValue(RubySymbol *_value): type(RV_SYMBOL), symbol(_value)
{ }

RubyValue::RubyValue(RubyObject *_value): type(RV_OBJECT), object(_value)
{ }
