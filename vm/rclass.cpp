#include <iostream>
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include "rarray.h"
#include "rhash.h"
#include "rio.h"
#include "rregexp.h"

RubyClass::RubyClass(RubyEnvironment &_e, const std::string &_name): RubyModule(_e, _e.Class, _name), superklass(_e.Object)
{ }

RubyClass::RubyClass(RubyEnvironment &_e, const std::string &_name, RubyClass *_superklass): RubyModule(_e, _e.Class, _name), superklass(_superklass)
{ }

RubyClass::RubyClass(RubyEnvironment &_e, const std::string &_name, RubyClass *_superklass, RubyModule *_parent): RubyModule(_e.Class, _name, _parent), superklass(_superklass)
{ }

const linked_ptr<RubyMethod> &RubyClass::find_method(const std::string &_name) const
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
    sagasu = sagasu->superklass;
  }

  throw ClassHasNoSuchMethodException();
}

bool RubyClass::has_ancestor(RubyClass *_check) const {
  if (this == _check)
    return true;

  RubyClass *ptr = superklass;

  while (ptr)
    if (ptr == _check)
      return true;
    else
      ptr = ptr->superklass;

  return false;
}

RubyObject *RubyClass::new_instance(RubyEnvironment &_e)
{
  // XXX TODO: refactor me later.
  // XXX: this whole thing is so fishy.
  //
  if (has_ancestor(_e.Class))
    return _e.gc.track(new RubyClass(_e, "" /* XXX */));
  else if (has_ancestor(_e.Module))
    return _e.gc.track(new RubyModule(_e, "" /* XXX */));
  else if (has_ancestor(_e.String))
    return _e.gc.track(new RubyString(_e, "" /* XXX */));
  else if (has_ancestor(_e.Array))
    return _e.gc.track(new RubyArray(_e));
  else if (has_ancestor(_e.Hash))
    return _e.gc.track(new RubyHash(_e));
  else if (has_ancestor(_e.IO))
    return _e.gc.track(new RubyIO(_e));
  else if (has_ancestor(_e.Regexp))
    return _e.gc.track(new RubyRegexp(_e));
  else if (has_ancestor(_e.MatchData))
    return _e.gc.track(new RubyMatchData(_e));
  else
    return _e.gc.track(new RubyObject(this));
} 

RubyValue class_new(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyClassEI::init(RubyEnvironment &_e)
{
  _e.Class->add_method("new", RubyMethod::Create(class_new, ARGS_ARBITRARY));
}

RubyValue class_new(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyClass *s = _self.get_special<RubyClass>();
  RubyObject *i = s->new_instance(_b->environment);
  i->set_class(s);	// Explicitly, in the case of derived classes - their root ones would assign themselves as the class in the ctor init'r.

  RubyValue v = O2V(i);
  try {
    s->find_method("initialize");
    v.call(_b, "initialize", _args);
  } catch (ClassHasNoSuchMethodException)
  { }

  return v;
}

