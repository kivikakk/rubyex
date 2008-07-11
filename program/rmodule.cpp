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
