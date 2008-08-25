#include "rexception.h"
#include "rclass.h"
#include "renvironment.h"
#include "binding.h"

void RubyExceptionEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cException = RubyClass::create_class(_e, "Exception");
  _e.add_class("Exception", rb_cException);
  _e.Exception = rb_cException;

  RubyClass *rb_cStandardError = RubyClass::create_class_with_super(_e, "StandardError", rb_cException);
  _e.add_class("StandardError", rb_cStandardError);
  _e.StandardError = rb_cStandardError;

  RubyClass *rb_cTypeError = RubyClass::create_class_with_super(_e, "TypeError", rb_cException);
  _e.add_class("TypeError", rb_cTypeError);
  _e.TypeError = rb_cTypeError;
}

WorldException::WorldException(linked_ptr<Binding> &_b, RubyObject *_exception): exception(_exception)
{
  if (dynamic_cast<RubyClass *>(exception))
    exception = RubyValue::from_object(exception).call(_b, "new").get_special<RubyObject>();

  check_exception_object(_b);
}

WorldException::WorldException(linked_ptr<Binding> &_b, RubyClass *_class, const std::string &_message)
{
  exception = RubyValue::from_object(_class).call(_b, "new", _b->environment.get_string(_message)).get_special<RubyObject>();
  check_exception_object(_b);
}

void WorldException::check_exception_object(linked_ptr<Binding> &_b) const {
  if (!exception->get_class()->has_ancestor(_b->environment.Exception)) 
    throw WorldException(_b, _b->environment.TypeError, "exception class/object expected");
}

