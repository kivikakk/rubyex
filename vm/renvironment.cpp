#include <iostream>
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
#include "global.h"

RubyValue main_to_s(linked_ptr<Binding> &, RubyValue);

RubyEnvironment::RubyEnvironment()
{
  this->Object = new RubyClass(*this, "Object", NULL);	// Object<nil, NOT Object<Object(!!)
  this->Kernel = new RubyModule(*this, "Kernel");
  this->Module = new RubyClass(*this, "Module", this->Object); 	// I'm just explicitly mentioning Module < Object.. so Class<Module<Object<nil
  this->Class = new RubyClass(*this, "Class", this->Module);

  // Object's parent is itself, technically.
  this->Object->set_parent(this->Object);

  // Since Module and Class didn't exist when Object and Kernel were made,
  // they need to be reset explicitly.
  this->Kernel->set_class(this->Module);
  this->Object->set_class(this->Class);

  this->Object->include_module(this->Kernel);

  this->set_global_by_name("Object", this->Object);
  this->set_global_by_name("Kernel", this->Kernel);
  this->set_global_by_name("Module", this->Module);
  this->set_global_by_name("Class", this->Class);

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

  this->set_global_by_name("VERSION", this->get_string(RX_VERSION));
  this->set_global_by_name("RUBY_PLATFORM", this->get_string(RX_PLATFORM));
  this->set_global_by_name("RUBY_RELEASE_DATE", this->get_string(RX_RELEASE_DATE));

  main = new RubyObject(this->Object);
  main->add_metaclass_method(*this, "to_s", RubyMethod::Create(main_to_s));
  main->add_metaclass_method(*this, "inspect", RubyMethod::Create(main_to_s));
}

RubyValue main_to_s(linked_ptr<Binding> &_b, RubyValue _self) {
  return _b->environment.get_string("main");
}

bool RubyEnvironment::global_exists(const std::string &_name) const
{
  if (_name[0] != '$')
    return this->Object->has_constant(_name);

  return (globals.find(_name) != globals.end());
}

RubyValue RubyEnvironment::get_global_by_name(const std::string &_name) const
{
  if (_name[0] != '$')
    return this->Object->get_constant(_name);

  if (!global_exists(_name))
    throw CannotFindGlobalError();

  return globals.find(_name)->second;
}

void RubyEnvironment::set_global_by_name(const std::string &_name, RubyValue _val)
{
  if (_name[0] != '$')
    return this->Object->set_constant(_name, _val);

  // XXX GC concerns of overwriting?
  globals[_name] = _val;
}

void RubyEnvironment::set_global_by_name(const std::string &_name, RubyObject *_val)
{
  return set_global_by_name(_name, O2V(_val));
}

const std::string &RubyEnvironment::get_name_by_global(RubyValue _global) const
{
  return this->Object->get_name_by_constant(_global);
  /*
  std::map<std::string, RubyValue>::const_iterator obj =
    std::find_if(globals.begin(), globals.end(), second_equal_to<std::map<std::string, RubyValue>::value_type>(_global));
  if (obj != globals.end())
    return obj->first;

  throw CannotFindGlobalError();
  */
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
  return O2V(SystemCallError).call(_b, "new", F2V(_no), get_string(_msg)).object;
}

