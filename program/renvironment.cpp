#include "renvironment.h"
#include "rstring.h"
#include "rkernel.h"
#include "rtri.h"
#include <iostream>

RubyEnvironment::RubyEnvironment()
{
  // Let's bring this online.
  RubyObjectEI().init(*this);
  RubyModuleEI().init(*this);
  RubyClassEI().init(*this);

  RubyTriEI().init(*this);

  RubyKernelEI().init(*this);

  RubyStringEI().init(*this);

  main = new RubyObject(new NamedLazyClass(*this, "Object"));
}

RubyClass *RubyEnvironment::get_class_by_name(const std::string &name)
{
  if (classes.find(name) == classes.end()) {
    std::cerr << "ERROR: tried to get an inexistant class." << std::endl;
    throw std::exception();
  }

  return classes[name];
}

RubyModule *RubyEnvironment::get_module_by_name(const std::string &name)
{
  if (modules.find(name) == modules.end()) {
    std::cerr << "ERROR: tried to get an inexistant module." << std::endl;
    throw std::exception();
  }

  return modules[name];
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

RubySymbol *RubyEnvironment::get_symbol(const std::string &name)
{
  std::map<std::string, RubySymbol *>::iterator si = symbols.find(name);
  if (si != symbols.end())
    return si->second;
  RubySymbol *sym = new RubySymbol(name);
  return (symbols[name] = sym);
}

