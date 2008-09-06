#include "lazyclass.h"
#include "renvironment.h"
#include <iostream>
#include <exception>

LazyClass::LazyClass(): cache(NULL)
{ }

LazyClass::~LazyClass()
{ }

RubyClass *LazyClass::internal_resolve() const {
  std::cerr << "internal_resolve not implemented for this class." << std::endl;
  throw std::exception();
}

RubyClass *LazyClass::resolve() {
  return (cache ? cache : (cache = internal_resolve()));
}

RubyClass *LazyClass::resolve() const {
  return (cache ? cache : internal_resolve());
}

NamedLazyClass::NamedLazyClass(RubyEnvironment &_e, const std::string &_name): e(&_e), name(_name)
{ }

RubyClass *NamedLazyClass::internal_resolve() const {
  return e->get_global_by_name(name).get_special<RubyClass>();
}

