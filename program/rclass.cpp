#include "rclass.h"
#include "renvironment.h"

RubyClass::RubyClass(RubyEnvironment &_e, LazyClass *_superklass, const std::string &_name): RubyModule(_e, new NamedLazyClass(_e, "Class"), _name), superklass(_superklass)
{
  cache = this;
}
    
RubyClass *RubyClass::create_class(RubyEnvironment &_e, const std::string &_name)
{
  return new RubyClass(_e, new NamedLazyClass(_e, "Object"), _name);
}

RubyClass *RubyClass::create_class_with_super(RubyEnvironment &_e, const std::string &_name, LazyClass *_superklass)
{
  return new RubyClass(_e, _superklass, _name);
}

void RubyClassEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cClass = RubyClass::create_class_with_super(e, "Class", new NamedLazyClass(e, "Module"));

  e.add_class("Class", rb_cClass);
}
