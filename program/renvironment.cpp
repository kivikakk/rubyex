#include "renvironment.h"
#include "rstring.h"
#include <iostream>

RubyValue
  *RubyEnvironment::NIL = new RubyNilValue(),
  *RubyEnvironment::TRUE = new RubyTrueValue(),
  *RubyEnvironment::FALSE = new RubyFalseValue();

RubyEnvironment::RubyEnvironment()
{
  // Let's bring this online.
  RubyString().init(*this);
}

void RubyEnvironment::add_class(const std::string &name, RubyClass *klass)
{
  if (classes.find(name) != classes.end()) {
    std::cerr << "ERROR: added a class where one exists already." << std::endl;
    throw std::exception();
  }

  classes[name] = klass;
}

void RubyEnvironment::add_module(const std::string &name, RubyModule *module)
{
  if (modules.find(name) != modules.end()) {
    std::cerr << "ERROR: added a module where one exists already." << std::endl;
    throw std::exception();
  }

  modules[name] = module;
}

RubyEnvironmentInitializer::~RubyEnvironmentInitializer()
{ }

