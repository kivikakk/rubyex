#include "rclass.h"
#include "renvironment.h"
#include <iostream>

RubyClass *RubyClass::create_class(RubyEnvironment &_e, const std::string &_name)
{
  return new RubyClass(_e, new NamedLazyClass(_e, "Object"), _name);
}

RubyClass *RubyClass::create_class_with_super(RubyEnvironment &_e, const std::string &_name, LazyClass *_superklass)
{
  return new RubyClass(_e, _superklass, _name);
}

RubyMethod *RubyClass::find_method(const std::string &_name) const
{
  // This function locates a method `_name' as when looking for
  // a method to run on an instance of this class. We use the following search path:
  // MyClass, MyClassIncludedModules, ...
  // SuperClass, SuperClassIncludedModules ...,
  // SuperSuperClass, SuperSuperClassIncludedModules ...,
  
  if (this->has_method(_name))
    return this->get_method(_name);

  const RubyClass *sagasu = this;
  while ((sagasu->superklass)) {
    sagasu = sagasu->superklass->resolve();
    if (sagasu->has_method(_name))
      return sagasu->get_method(_name);
  }

  std::cerr << "RubyClass::find_method: failed to find " << _name << std::endl;
  throw;
}

RubyObject *RubyClass::new_instance()
{
  return new RubyObject(this);
}

RubyClass::RubyClass(RubyEnvironment &_e, LazyClass *_superklass, const std::string &_name): RubyModule(new NamedLazyClass(_e, "Class"), _name), superklass(_superklass)
{
  cache = this;
}
    

void RubyClassEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cClass = RubyClass::create_class_with_super(_e, "Class", new NamedLazyClass(_e, "Module"));

  _e.add_class("Class", rb_cClass);
}
