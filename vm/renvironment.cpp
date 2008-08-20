#include "renvironment.h"
#include "rstring.h"
#include "rnumeric.h"
#include "rkernel.h"
#include "rbinding.h"
#include "rtri.h"
#include "stlext.h"
#include <iostream>
#include <algorithm>
#include <functional>

RubyEnvironment::RubyEnvironment()
{
  // Let's bring this online.
  RubyKernelEI().init(*this);

  // Object includes Kernel, so we put it here ...
  RubyObjectEI().init(*this);
  RubyModuleEI().init(*this);
  RubyClassEI().init(*this);
  RubyBindingEI().init(*this);
  RubyTriEI().init(*this);
  RubySymbolEI().init(*this);

  RubyNumericEI().init(*this);
  RubyStringEI().init(*this);

  main = new RubyObject(new NamedLazyClass(*this, "Object"));
}

bool RubyEnvironment::global_exists(const std::string &_name) const
{
  return class_exists(_name) || module_exists(_name);
}

RubyObject *RubyEnvironment::get_global_by_name(const std::string &_name) const
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

RubyClass *RubyEnvironment::get_class_by_name(const std::string &_name) const
{
  if (!class_exists(_name)) {
    std::cerr << "ERROR: tried to get an inexistant class." << std::endl;
    throw std::exception();
  }

  return classes.find(_name)->second;
}

bool RubyEnvironment::module_exists(const std::string &_name) const
{
  return (modules.find(_name) != modules.end());
}

RubyModule *RubyEnvironment::get_module_by_name(const std::string &_name) const
{
  if (!module_exists(_name)) {
    std::cerr << "ERROR: tried to get an inexistant module." << std::endl;
    throw std::exception();
  }

  return modules.find(_name)->second;
}

const std::string &RubyEnvironment::get_name_by_global(RubyObject *_global) const
{
  std::map<std::string, RubyClass *>::const_iterator cls =
    std::find_if(classes.begin(), classes.end(), second_equal_to<std::map<std::string, RubyObject *>::value_type>(_global));
  if (cls != classes.end())
    return cls->first;

  std::map<std::string, RubyModule *>::const_iterator mod =
    std::find_if(modules.begin(), modules.end(), second_equal_to<std::map<std::string, RubyObject *>::value_type>(_global));
  if (mod != modules.end())
    return mod->first;

  throw CannotFindGlobalError();
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

