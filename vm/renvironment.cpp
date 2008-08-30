#include <iostream>
#include <algorithm>
#include <functional>
#include "renvironment.h"
#include "rmethod.h"
#include "rstring.h"
#include "rnumeric.h"
#include "rkernel.h"
#include "rbinding.h"
#include "rtri.h"
#include "rarray.h"
#include "rhash.h"
#include "rrange.h"
#include "rexception.h"
#include "rio.h"
#include "stlext.h"

RubyValue main_to_s(linked_ptr<Binding> &, RubyValue);

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
  RubyArrayEI().init(*this);
  RubyHashEI().init(*this);
  RubyRangeEI().init(*this);

  RubyIOEI().init(*this);
  RubyExceptionEI().init(*this);

  main = new RubyObject(new NamedLazyClass(*this, "Object"));
  main->add_metaclass_method(*this, "to_s", RubyMethod::Create(main_to_s));
  main->add_metaclass_method(*this, "inspect", RubyMethod::Create(main_to_s));
}

RubyValue main_to_s(linked_ptr<Binding> &_b, RubyValue _self) {
  return _b->environment.get_string("main");
}

bool RubyEnvironment::global_exists(const std::string &_name) const {
  return global_var_exists(_name) || class_exists(_name) || module_exists(_name);
}

RubyValue RubyEnvironment::get_global_by_name(const std::string &_name) const
{
  // XXX TODO what about constants?
  if (class_exists(_name))
    return RubyValue::from_object(get_class_by_name(_name));
  else if (module_exists(_name))
    return RubyValue::from_object(get_module_by_name(_name));

  throw CannotFindGlobalError();
}

bool RubyEnvironment::global_var_exists(const std::string &_name) const
{
  return (globals.find(_name) != globals.end());
}

RubyValue RubyEnvironment::get_global_var_by_name(const std::string &_name) const
{
  if (!global_var_exists(_name))
    throw SevereInternalError("get_global_var_by_name(): tried to get inexistant global `" + _name + "'");

  return globals.find(_name)->second;
}

void RubyEnvironment::set_global_var_by_name(const std::string &_name, RubyValue _val)
{
  // XXX GC concerns of overwriting?
  globals[_name] = _val;
}

bool RubyEnvironment::class_exists(const std::string &_name) const
{
  return (classes.find(_name) != classes.end());
}

RubyClass *RubyEnvironment::get_class_by_name(const std::string &_name) const
{
  if (!class_exists(_name))
    throw SevereInternalError("get_class_by_name(): tried to get inexistant class `" + _name + "'");

  return classes.find(_name)->second;
}

bool RubyEnvironment::module_exists(const std::string &_name) const
{
  return (modules.find(_name) != modules.end());
}

RubyModule *RubyEnvironment::get_module_by_name(const std::string &_name) const
{
  if (!module_exists(_name))
    throw SevereInternalError("get_module_by_name(): tried to get inexistant module `" + _name + "'");

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
  if (classes.find(_name) != classes.end())
    throw SevereInternalError("add_class(): tried to add class `" + _name + "' where one exists already");

  classes[_name] = _klass;
}

void RubyEnvironment::add_module(const std::string &_name, RubyModule *_module)
{
  if (modules.find(_name) != modules.end())
    throw SevereInternalError("add_module(): tried to add module `" + _name + "' where one exists already");

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

RubyObject *RubyEnvironment::errno_exception(linked_ptr<Binding> &_b, int _no, const char *_msg)
{
  return RubyValue::from_object(SystemCallError).call(_b, "new", RubyValue::from_fixnum(_no), get_string(_msg)).object;
}

