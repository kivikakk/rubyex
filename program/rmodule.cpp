#include "rmodule.h"
#include <iostream>

RubyModule::RubyModule(const std::string &_name): name(_name)
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
