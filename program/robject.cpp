#include "robject.h"
#include "rclass.h"
#include "renvironment.h"

RubyObject::RubyObject(LazyClass *_klass): klass(_klass), metaklass(NULL)
{ }

RubyObject::~RubyObject()
{
  for (std::map<std::string, RubyValue *>::iterator it = instance_variables.begin(); it != instance_variables.end(); ++it)
    delete it->second;
}

void RubyObjectEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cObject = RubyClass::create_class(e, "Object");

  e.add_class("Object", rb_cObject);
}
