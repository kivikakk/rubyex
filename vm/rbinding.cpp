#include "rbinding.h"
#include "rclass.h"
#include "renvironment.h"

void RubyBindingEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cBinding = RubyClass::create_class(_e, "Binding");

  _e.add_class("Binding", rb_cBinding);
  _e.Binding = rb_cBinding;
}

RubyBinding::RubyBinding(linked_ptr<Binding> &_binding): RubyObject(new NamedLazyClass(_binding->environment, "Binding")), binding(_binding)
{ }

