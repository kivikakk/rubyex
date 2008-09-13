#include "rbinding.h"
#include "rclass.h"
#include "renvironment.h"

void RubyBindingEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cBinding = _e.gc.track<RubyClass>(new RubyClass(_e, "Binding"));

  _e.set_global_by_name("Binding", rb_cBinding);
  _e._Binding = rb_cBinding;
}

RubyBinding::RubyBinding(linked_ptr<Binding> &_binding): RubyObject(_binding->environment._Binding), binding(_binding)
{ }

