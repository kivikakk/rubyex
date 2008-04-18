#include "robject.h"
#include "rclass.h"
#include "renvironment.h"

RubyObject::RubyObject(LazyClass *_klass): klass(_klass), metaklass(NULL)
{ }

RubyObject::~RubyObject()
{ }

void RubyObjectEI::init(RubyEnvironment &e)
{
  RubyClass *rb_cObject = RubyClass::create_class(e, "Object");

  e.add_class("Object", rb_cObject);
}
