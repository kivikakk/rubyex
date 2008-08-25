#include "rexception.h"
#include "rclass.h"
#include "renvironment.h"

void RubyExceptionEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cException = RubyClass::create_class(_e, "Exception");
  _e.add_class("Exception", rb_cException);
  _e.Exception = rb_cException;
}

WorldException::WorldException(RubyObject *_exception): exception(_exception)
{

}


