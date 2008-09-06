#include "rmodule.h"
#include <iostream>
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "rexception.h"

RubyModule::RubyModule(RubyEnvironment &_e, const std::string &_name): RubyObject(new NamedLazyClass(_e, "Module")), name(_name)
{ }

RubyModule::RubyModule(LazyClass *_klass, const std::string &_name): RubyObject(_klass), name(_name)
{ }

RubyModule::~RubyModule()
{
  for (std::map<std::string, RubyMethod *>::iterator it = methods.begin(); it != methods.end(); ++it)
    delete it->second;
}

const std::string &RubyModule::get_name() const
{
  return name;
}

void RubyModule::add_method(const std::string &_name, RubyMethod *_method)
{
  if (has_method(_name))
    throw SevereInternalError("tried to add method `" + _name + "' yet one exists already");

  methods[_name] = _method;
}

void RubyModule::def_method(const std::string &_name, RubyMethod *_method)
{
  if (has_method(_name))
    delete methods[_name];	// XXX: is this safe? Will the same method never be referred to twice?
  methods[_name] = _method;
}

void RubyModule::remove_method(const std::string &_name)
{
  if (!has_method(_name))
    throw SevereInternalError("tried to remove method `" + _name + "' yet one doesn't exist already");
  
  methods.erase(_name);
}

void RubyModule::add_module_method(RubyEnvironment &_e, const std::string &_name, RubyMethod *_method)
{
  add_method(_name, _method);		// TODO: visibility? this should be private.
  add_metaclass_method(_e, _name, _method);
}

void RubyModule::include_module(RubyModule *_module)
{
  for (std::list<RubyModule *>::const_iterator it = includes.begin(); it != includes.end(); ++it)
    if (*it == _module) {
      std::cerr << "XXX: yet to implement re-including of the same module" << std::endl;
      return;
    }

  // new takes precedent.
  includes.push_front(_module);
}

bool RubyModule::has_method(const std::string &_name) const {
  return (methods.find(_name) != methods.end());
}

RubyMethod *RubyModule::get_method(const std::string &_name) const {
  std::map<std::string, RubyMethod *>::const_iterator it = methods.find(_name);
  if (it == methods.end())
    throw SevereInternalError(this->name + " missing requested method `" + _name + "'");
  return it->second;
}

bool RubyModule::has_class_variable(const std::string &_name) const {
  return (class_variables.find(_name) != class_variables.end());
}

RubyValue RubyModule::get_class_variable(const std::string &_name) const {
  std::map<std::string, RubyValue>::const_iterator it = class_variables.find(_name);
  if (it == class_variables.end())
    throw SevereInternalError(this->name + " missing requested class variable `" + _name + "'");
  return it->second;
}

void RubyModule::set_class_variable(const std::string &_name, RubyValue _value) {
  class_variables[_name] = _value;
}

bool RubyModule::has_constant(const std::string &_name) const {
  return (constants.find(_name) != constants.end());
}

RubyValue RubyModule::get_constant(const std::string &_name) const {
  std::map<std::string, RubyValue>::const_iterator it = constants.find(_name);
  if (it == constants.end())
    throw SevereInternalError(this->name + " missing requested constant `" + _name + "'");
  return it->second;
}

void RubyModule::set_constant(const std::string &_name, RubyValue _value) {
  constants[_name] = _value;
}

void RubyModuleEI::init(RubyEnvironment &_e)
{ }
