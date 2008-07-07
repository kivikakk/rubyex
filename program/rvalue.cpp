#include "rvalue.h"
#include "renvironment.h"
#include <iostream>

RubyValue::RubyValue(): type(RV_NOTHING)
{ }

RubyValue RubyValue::from_fixnum(long value)
{ return RubyValue(value); }

RubyValue RubyValue::from_symbol(RubySymbol *value)
{ return RubyValue(value); }

RubyValue RubyValue::from_object(RubyObject *value)
{ return RubyValue(value); }

// the inclusion of `environment' here as a parameter seems a cop-out
RubyMethod *RubyValue::get_method(const std::string &name, RubyEnvironment *environment)
{
  // We need to find a method by this name in our context. We go through the class's ::ancestors.
  // Tradition has it:
  // 	[MyClass, MyClassIncludedModules, ...
  // 		SuperClass, SuperClassIncludedModules ...,
  // 		SuperSuperClass, SuperSuperClassIncludedModules ...,
  // 		...]

  switch (type) {
    case RubyValue::RV_FIXNUM: return environment->get_class_by_name("Fixnum")->get_method(name);
    case RubyValue::RV_SYMBOL: return environment->get_class_by_name("Symbol")->get_method(name);
    case RubyValue::RV_OBJECT: {
      RubyClass *mc = object->get_metaclass();
      if (mc) {
	;
      }
      throw;
      break;
    }
    default: std::cerr << "what in the world?" << std::endl; throw; break;
  }
}

RubyValue::RubyValue(long value): type(RV_FIXNUM), fixnum(value)
{ }

RubyValue::RubyValue(RubySymbol *value): type(RV_SYMBOL), symbol(value)
{ }

RubyValue::RubyValue(RubyObject *value): type(RV_FIXNUM), object(value)
{ }
