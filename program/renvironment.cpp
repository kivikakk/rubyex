#include "renvironment.h"
#include "rstring.h"
#include "rnumeric.h"
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

  RubyNumericEI().init(*this);
  RubyStringEI().init(*this);

  main = new RubyObject(new NamedLazyClass(*this, "Object"));
}

bool RubyEnvironment::global_exists(const std::string &_name) const
{
  return class_exists(_name) || module_exists(_name);
}

RubyObject *RubyEnvironment::get_global_by_name(const std::string &_name)
{
  // XXX TODO what about constants?
  if (class_exists(_name))
    return get_class_by_name(_name);
  if (module_exists(_name))
    return get_module_by_name(_name);

  std::cerr << "ERROR: tried to get an inexistant global." << std::endl;
  throw std::exception();
}

bool RubyEnvironment::class_exists(const std::string &_name) const
{
  return (classes.find(_name) != classes.end());
}

RubyClass *RubyEnvironment::get_class_by_name(const std::string &_name)
{
  if (!class_exists(_name)) {
    std::cerr << "ERROR: tried to get an inexistant class." << std::endl;
    throw std::exception();
  }

  return classes[_name];
}

bool RubyEnvironment::module_exists(const std::string &_name) const
{
  return (modules.find(_name) != modules.end());
}

RubyModule *RubyEnvironment::get_module_by_name(const std::string &_name)
{
  if (!module_exists(_name)) {
    std::cerr << "ERROR: tried to get an inexistant module." << std::endl;
    throw std::exception();
  }

  return modules[_name];
}

void RubyEnvironment::add_class(const std::string &_name, RubyClass *_klass)
{
  if (classes.find(_name) != classes.end()) {
    std::cerr << "ERROR: added a class where one exists already." << std::endl;
    throw std::exception();
  }

  classes[_name] = _klass;
}

void RubyEnvironment::add_module(const std::string &_name, RubyModule *_module)
{
  if (modules.find(_name) != modules.end()) {
    std::cerr << "ERROR: added a module where one exists already." << std::endl;
    throw std::exception();
  }

  modules[_name] = _module;
}

RubySymbol *RubyEnvironment::get_symbol(const std::string &_name)
{
  std::map<std::string, RubySymbol *>::iterator si = symbols.find(_name);
  if (si != symbols.end())
    return si->second;
  RubySymbol *sym = new RubySymbol(_name);
  return (symbols[_name] = sym);
}

