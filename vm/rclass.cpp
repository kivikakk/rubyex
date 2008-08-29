#include <iostream>
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include "rarray.h"
#include "rhash.h"
#include "rio.h"

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

  throw ClassHasNoSuchMethodException();
}

bool RubyClass::has_ancestor(RubyClass *_check) const {
  if (this == _check)
    return true;

  RubyClass *ptr = superklass->resolve();

  while (ptr)
    if (ptr == _check)
      return true;
    else
      ptr = ptr->superklass ? ptr->superklass->resolve() : NULL;

  return false;
}

RubyObject *RubyClass::new_instance(RubyEnvironment &_e)
{
  // XXX TODO: refactor me later.
  // XXX: this whole thing is so fishy.
  //
  if (has_ancestor(_e.Class))
    return RubyClass::create_class(_e, "" /* XXX */);
  else if (has_ancestor(_e.Module))
    return new RubyModule(_e, "" /* XXX */);
  else if (has_ancestor(_e.String))
    return _e.gc.track(new RubyString(_e, "" /* XXX */));
  else if (has_ancestor(_e.Array))
    return _e.gc.track(new RubyArray(_e));
  else if (has_ancestor(_e.Hash))
    return _e.gc.track(new RubyHash(_e));
  else if (has_ancestor(_e.IO))
    return _e.gc.track(new RubyIO(_e));
  else
    return new RubyObject(this);
}

RubyClass::RubyClass(RubyEnvironment &_e, LazyClass *_superklass, const std::string &_name): RubyModule(new NamedLazyClass(_e, "Class"), _name), superklass(_superklass)
{
  cache = this;
}
    

RubyValue class_new(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyClassEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cClass = RubyClass::create_class_with_super(_e, "Class", new NamedLazyClass(_e, "Module"));
  rb_cClass->add_method("new", RubyMethod::Create(class_new, ARGS_ARBITRARY));

  _e.add_class("Class", rb_cClass);
  _e.Class = rb_cClass;
}

RubyValue class_new(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyClass *s = _self.get_special<RubyClass>();
  RubyObject *i = s->new_instance(_b->environment);
  i->set_class(s);	// Explicitly, in the case of derived classes - their root ones would assign themselves as the class in the ctor init'r.

  RubyValue v = RubyValue::from_object(i);
  try {
    s->find_method("initialize");
    v.call(_b, "initialize", _args);
  } catch (ClassHasNoSuchMethodException)
  { }

  return v;
}

