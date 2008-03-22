#include "rmodule.h"
#include <iostream>

RubyModule::RubyModule(RubyEnvironment &_e, const std::string &_name): RubyObject(new NamedLazyClass(_e, "Module")), name(_name)
{ }

RubyModule::RubyModule(RubyEnvironment &_e, LazyClass *_klass, const std::string &_name): RubyObject(_klass), name(_name)
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

void RubyModule::add_method(const std::string &name, RubyMethod *method)
{
  if (methods.find(name) != methods.end()) {
    std::cerr << "ERROR: added a method where one exists already." << std::endl;
    throw std::exception();
  }

  methods[name] = method;
}
