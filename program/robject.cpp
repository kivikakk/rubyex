#include "robject.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include <sstream>
#include <iomanip>

RubyObject::RubyObject(LazyClass *_klass): klass(_klass), metaklass(NULL)
{ }

RubyObject::~RubyObject()
{ }

void RubyObject::add_metaclass_method(RubyEnvironment &_e, const std::string &_name, RubyMethod *_method)
{
  get_metaclass(_e)->add_method(_name, _method);
}

RubyClass *RubyObject::get_class() const
{
  return klass->resolve();
}

RubyClass *RubyObject::get_metaclass_read() const
{
  // Caller had best be careful about NULL.
  return metaklass;
}

RubyClass *RubyObject::get_metaclass(RubyEnvironment &_e)
{
  if (metaklass == NULL) {
    // XXX at the moment metaclasses will be created as inheriting from Object.
    // this is not good. TODO: research the proper method for determining the superclass.
    // Metaclass land is scary.
    metaklass = RubyClass::create_class(_e, "<some metaclass>");
  }

  return metaklass;
}

RubyValue object_inspect(RubyEnvironment &, RubyValue);

void RubyObjectEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cObject = RubyClass::create_class_with_super(_e, "Object", NULL);
  // Object<nil, NOT Object<Object(!!)
  rb_cObject->add_method("inspect", RubyMethod::Create(object_inspect));

  _e.add_class("Object", rb_cObject);
  _e.Object = rb_cObject;
}

RubyValue object_inspect(RubyEnvironment &_e, RubyValue _self)
{
  std::ostringstream oss;
  oss << "#<" << _self.get_class(_e)->get_name() << ":";
  oss << std::dec << _self.object;
  oss << ">";

  return RubyValue::from_object(new RubyString(_e, oss.str()));
}

