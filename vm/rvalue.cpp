#include <iostream>
#include "rvalue.h"
#include "renvironment.h"
#include "rmethod.h"

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

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
    default: std::cerr << "RubyValue::get_class: don't know my own type" << std::endl; throw;
  }
}

// the inclusion of `environment' here as a parameter seems a cop-out
RubyMethod *RubyValue::get_method(const std::string &_name, RubyEnvironment &_e) const
{
  // We need to find a method by this name in our context. We go through the class's ::ancestors.
  // Tradition has it:
  // 	[MyMetaClass, [XXX more MetaClass related?],
  // 		MyClass, MyClassIncludedModules, ...
  // 		SuperClass, SuperClassIncludedModules ...,
  // 		SuperSuperClass, SuperSuperClassIncludedModules ...,
  // 		...]

  switch (type) {
    case RubyValue::RV_FIXNUM: return _e.Fixnum->find_method(_name);
    case RubyValue::RV_SYMBOL: return _e.Symbol->find_method(_name);
    case RubyValue::RV_OBJECT: {
      // MyMetaClass
      RubyClass *c = object->get_metaclass_read();
      if (c && c->has_method(_name))
	return c->get_method(_name);
      
      // MyClass, MyClassIncludedModules, SuperClass, SuperClassIncludedModules, SuperSuperClass,
      // SuperSuperClassIncludedModules, ..., ...
      return object->get_class()->find_method(_name);
      break;
    }
    default: std::cerr << "RubyValue::get_method: what in the world? my type is not recognisable" << std::endl; throw;
  }
}

RubyValue RubyValue::call(RubyEnvironment &_e, const std::string &_name) const
{ return get_method(_name, _e)->call(_e, *this); }

RubyValue RubyValue::call(RubyEnvironment &_e, const std::string &_name, RubyValue _arg0) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  return get_method(_name, _e)->call(_e, *this, args);
}
RubyValue RubyValue::call(RubyEnvironment &_e, const std::string &_name, RubyValue _arg0, RubyValue _arg1) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  return get_method(_name, _e)->call(_e, *this, args);
}

RubyValue RubyValue::call(RubyEnvironment &_e, const std::string &_name, RubyValue _arg0, RubyValue _arg1, RubyValue _arg2) const
{
  std::vector<RubyValue> args;
  args.push_back(_arg0);
  args.push_back(_arg1);
  args.push_back(_arg2);
  return get_method(_name, _e)->call(_e, *this, args);
}

RubyValue RubyValue::call(RubyEnvironment &_e, const std::string &_name, const std::vector<RubyValue> &_args) const
{
  return get_method(_name, _e)->call(_e, *this, _args);
}


RubyValue::RubyValue(long _value): type(RV_FIXNUM), fixnum(_value)
{ }

RubyValue::RubyValue(RubySymbol *_value): type(RV_SYMBOL), symbol(_value)
{ }

RubyValue::RubyValue(RubyObject *_value): type(RV_OBJECT), object(_value)
{ }
