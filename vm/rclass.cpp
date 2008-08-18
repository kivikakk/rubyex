#include <iostream>
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

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

  const RubyClass *sagasu = this;
  while ((sagasu)) {
    if (sagasu->has_method(_name))
      return sagasu->get_method(_name);
    // Included modules?
    for (std::list<RubyModule *>::const_iterator it = sagasu->includes.begin(); it != sagasu->includes.end(); ++it)
      if ((*it)->has_method(_name))
	return (*it)->get_method(_name);

    if (sagasu->superklass)
      sagasu = sagasu->superklass->resolve();
    else
      sagasu = NULL;
  }

  std::cerr << "RubyClass::find_method: cannot find " << get_name() << "#" << _name << std::endl;
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
    

RubyValue class_new(Binding &, RubyValue, const std::vector<RubyValue> &);

void RubyClassEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cClass = RubyClass::create_class_with_super(_e, "Class", new NamedLazyClass(_e, "Module"));
  rb_cClass->add_method("new", RubyMethod::Create(class_new, ARGS_ARBITRARY));

  _e.add_class("Class", rb_cClass);
  _e.Class = rb_cClass;
}

RubyValue class_new(Binding &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyClass *s = dynamic_cast<RubyClass *>(_self.object);
  RubyObject *i = s->new_instance();
  return RubyValue::from_object(i);
}

