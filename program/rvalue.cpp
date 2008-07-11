#include "rvalue.h"
#include "renvironment.h"
#include <iostream>

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

RubyValue RubyValue::from_fixnum(long _value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_symbol(RubySymbol *_value)
{ return RubyValue(_value); }

RubyValue RubyValue::from_object(RubyObject *_value)
{ return RubyValue(_value); }

// the inclusion of `environment' here as a parameter seems a cop-out
RubyMethod *RubyValue::get_method(const std::string &_name, RubyEnvironment &_e)
{
  // We need to find a method by this name in our context. We go through the class's ::ancestors.
  // Tradition has it:
  // 	[MyMetaClass, [XXX more MetaClass related?],
  // 		MyClass, MyClassIncludedModules, ...
  // 		SuperClass, SuperClassIncludedModules ...,
  // 		SuperSuperClass, SuperSuperClassIncludedModules ...,
  // 		...]

  switch (type) {
    case RubyValue::RV_FIXNUM: return _e.get_class_by_name("Fixnum")->get_method(_name); // XXX these will need to fall back too, since `4.puts' works in MRI
    case RubyValue::RV_SYMBOL: return _e.get_class_by_name("Symbol")->get_method(_name);
    case RubyValue::RV_OBJECT: {
      // MyMetaClass
      RubyClass *mc = object->get_metaclass_read();
      if (mc && mc->has_method(_name))
	return mc->get_method(_name);
      
      // MyClass
      // XXX TODO

      std::cerr << "RubyValue::get_method: we never found a method?" << std::endl;
      throw;
      break;
    }
    default: std::cerr << "what in the world?" << std::endl; throw; break;
  }
}

RubyValue::RubyValue(long _value): type(RV_FIXNUM), fixnum(_value)
{ }

RubyValue::RubyValue(RubySymbol *_value): type(RV_SYMBOL), symbol(_value)
{ }

RubyValue::RubyValue(RubyObject *_value): type(RV_OBJECT), object(_value)
{ }
