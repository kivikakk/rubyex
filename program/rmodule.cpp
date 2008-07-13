#include "rmodule.h"
#include <iostream>
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"

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
  if (methods.find(_name) != methods.end()) {
    std::cerr << "ERROR: added a method where one exists already." << std::endl;
    throw std::exception();
  }

  methods[_name] = _method;
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
      std::cerr << "WARNING: " << name << " already included " << _module->get_name() << "?" << std::endl;
      // is this even an error condition?
      return;
    }

  // new takes precedent.
  includes.push_front(_module);
}

bool RubyModule::has_method(const std::string &_name) const
{
  return (methods.find(_name) != methods.end());
}

RubyMethod *RubyModule::get_method(const std::string &_name) const
{
  std::map<std::string, RubyMethod *>::const_iterator it = methods.find(_name);
  if (it == methods.end()) {
    std::cerr << "ERROR: module " << this->name << " does not have method '" << _name << "'. I have " << methods.size() << " method(s)." << std::endl;
    throw std::exception();
  }

  return it->second;
}

void RubyModuleEI::init(RubyEnvironment &_e)
{
  // I'm just explicitly mentioning Module < Object.. so Class<Module<Object<nil
  RubyClass *rb_cModule = RubyClass::create_class_with_super(_e, "Module", new NamedLazyClass(_e, "Object"));

  _e.add_class("Module", rb_cModule);
}
