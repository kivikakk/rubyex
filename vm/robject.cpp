#include "robject.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include <sstream>
#include <iomanip>
#include <iostream>

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

RubyValue object_inspect_to_s(RubyEnvironment &, RubyValue);
RubyValue object_send(RubyEnvironment &, RubyValue, const std::vector<RubyValue> &);

void RubyObjectEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cObject = RubyClass::create_class_with_super(_e, "Object", NULL);
  // Object<nil, NOT Object<Object(!!)
  rb_cObject->add_method("inspect", RubyMethod::Create(object_inspect_to_s));
  rb_cObject->add_method("to_s", RubyMethod::Create(object_inspect_to_s));
  rb_cObject->add_method("send", RubyMethod::Create(object_send, ARGS_ARBITRARY));
  rb_cObject->include_module(_e.Kernel);

  _e.add_class("Object", rb_cObject);
  _e.Object = rb_cObject;
}

RubyValue object_inspect_to_s(RubyEnvironment &_e, RubyValue _self)
{
  std::ostringstream oss;
  oss << "#<" << _self.get_class(_e)->get_name() << ":";
  oss << std::dec << _self.object;
  oss << ">";

  return RubyValue::from_object(new RubyString(_e, oss.str()));
}

RubyValue object_send(RubyEnvironment &_e, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyValue a = _args[0];
  if (!(a.type == RubyValue::RV_SYMBOL) && !(a.type == RubyValue::RV_OBJECT && (a.object->get_class() == _e.String))) {
    std::cerr << "Object#send: not given a Symbol or String" << std::endl;
    throw;	// boom. XXX TypeError
  }

  std::string function_name = dynamic_cast<RubyString *>(_args[0].object)->string_value;
  std::vector<RubyValue> rest = std::vector<RubyValue>(_args.begin() + 1, _args.end());
  return _self.call(_e, function_name, rest);
}

