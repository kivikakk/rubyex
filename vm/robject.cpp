#include "robject.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include <sstream>
#include <iomanip>
#include <iostream>

RubyObject::RubyObject(RubyClass *_klass): klass(_klass), metaklass(NULL)
{ }

RubyObject::~RubyObject()
{ }

void RubyObject::add_metaclass_method(RubyEnvironment &_e, const std::string &_name, const linked_ptr<RubyMethod> &_method)
{
  get_metaclass(_e)->add_method(_name, _method);
}

void RubyObject::set_class(RubyClass *_klass)
{
  klass = _klass;
}

RubyClass *RubyObject::get_class() const
{
  return klass;
}

RubyClass *RubyObject::get_metaclass_read() const
{
  // Caller had best be careful about NULL.
  if (!metaklass) {
    const RubyClass *this_class = dynamic_cast<const RubyClass *>(this);
    if (this_class && this_class->superklass)
      return this_class->superklass->get_metaclass_read();
  }
  return metaklass;
}

bool RubyObject::has_instance(const std::string &_name) const
{
  return (instance_variables.find(_name) != instance_variables.end());
}

RubyValue RubyObject::get_instance(const std::string &_name) const
{
  if (instance_variables.find(_name) == instance_variables.end())
    throw SevereInternalError("get_instance(): tried to get inexistant instance variable `" + _name + "'");
  return instance_variables.find(_name)->second;
}

void RubyObject::set_instance(const std::string &_name, RubyValue _value)
{
  instance_variables[_name] = _value;
}

RubyClass *RubyObject::get_metaclass(RubyEnvironment &_e)
{
  if (metaklass == NULL) {
    // XXX at the moment metaclasses will be created as inheriting from Object.
    // this is not good. TODO: research the proper method for determining the superclass.
    // Metaclass land is scary.
    RubyClass *this_class = dynamic_cast<RubyClass *>(this);
    if (this_class && this_class->superklass)
      metaklass = _e.gc.track(new RubyClass(_e, "<some inheriting metaclass>", this_class->superklass->get_metaclass(_e)));
    else
      metaklass = _e.gc.track(new RubyClass(_e, "<some metaclass>"));
  }

  return metaklass;
}

RubyValue object_inspect_to_s(linked_ptr<Binding> &, RubyValue);
RubyValue object_send(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_class(linked_ptr<Binding> &, RubyValue);
RubyValue object_eq_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_logical_and_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_logical_or_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_eql(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_neql(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue object_nil(linked_ptr<Binding> &, RubyValue);

void RubyObjectEI::init(RubyEnvironment &_e) {
  _e.Object->add_method("inspect", RubyMethod::Create(object_inspect_to_s));
  _e.Object->add_method("to_s", RubyMethod::Create(object_inspect_to_s));
  _e.Object->add_method("send", RubyMethod::Create(object_send, ARGS_ARBITRARY));

  _e.Object->add_method("==", RubyMethod::Create(object_eq_op, 1));
  _e.Object->add_method("&&", RubyMethod::Create(object_logical_and_op, 1));
  _e.Object->add_method("||", RubyMethod::Create(object_logical_or_op, 1));
  _e.Object->add_method("eql?", RubyMethod::Create(object_eql, 1));
  _e.Object->add_method("!=", RubyMethod::Create(object_neql, 1));

  _e.Object->add_method("nil?", RubyMethod::Create(object_nil));

  _e.Object->add_method("class", RubyMethod::Create(object_class));
}

RubyValue object_inspect_to_s(linked_ptr<Binding> &_b, RubyValue _self) {
  try {
    return _b->environment.get_string(_b->environment.get_name_by_global(O2V(_self.object)));
  } catch (CannotFindGlobalError)
  { }

  std::ostringstream oss;
  oss << "#<"
    << O2V(_self.get_class(_b->environment)).inspect(_b)
    << ":";
  oss << std::dec << _self.object;
  oss << ">";

  return _b->environment.get_string(oss.str());
}

RubyValue object_send(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyValue a = _args[0];
  a = a.call(_b, "to_sym");

  std::string function_name = a.symbol->value;
  std::vector<RubyValue> rest = std::vector<RubyValue>(_args.begin() + 1, _args.end());
  return _self.call(_b, function_name, rest);
}

RubyValue object_class(linked_ptr<Binding> &_b, RubyValue _self)
{ return O2V(_self.get_class(_b->environment)); }

RubyValue object_eq_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyValue comp = _args[0];
  if (_self.type != comp.type)
    return _b->environment.FALSE;
  return _b->environment.get_truth(_self.fixnum == comp.fixnum);
}

RubyValue object_logical_and_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  bool self_truthy = _self.truthy(_b->environment), arg_truthy = _args[0].truthy(_b->environment);
  if (self_truthy && arg_truthy)
    return _args[0];
  return !self_truthy ? _self : _args[0];
}

RubyValue object_logical_or_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  return _self.truthy(_b->environment) ? _self : _args[0];
}

RubyValue object_eql(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{ return _self.call(_b, "==", _args); /* make sure it goes through method dispatch */ }

RubyValue object_neql(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{ return _b->environment.get_truth(!_self.call(_b, "==", _args).truthy(_b->environment)); }

RubyValue object_nil(linked_ptr<Binding> &_b, RubyValue _self)
{ return _b->environment.FALSE; }

