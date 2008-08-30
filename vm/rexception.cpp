#include "rexception.h"
#include "rclass.h"
#include "renvironment.h"
#include "rmethod.h"
#include "binding.h"

RubyValue exception_initialize(linked_ptr<Binding> &, RubyValue);
RubyValue exception_initialize_message(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue exception_message(linked_ptr<Binding> &, RubyValue);
RubyValue exception_message_assign(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue systemcallerror_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

#define DEF_EXCEPTION(name, inherit) \
  RubyClass *rb_c##name = RubyClass::create_class_with_super(_e, #name, rb_c##inherit); \
  _e.add_class(#name, rb_c##name); \
  _e.name = rb_c##name;
// Forgive me father, for I have sinned (and used macros).

void RubyExceptionEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cException = RubyClass::create_class(_e, "Exception");
  rb_cException->add_method("initialize", new RubyMultiCMethod(new RubyCMethod(exception_initialize), new RubyCMethod(exception_initialize_message, 1)));
  rb_cException->add_method("message", RubyMethod::Create(exception_message));
  rb_cException->add_method("message=", RubyMethod::Create(exception_message_assign, 1));
  _e.add_class("Exception", rb_cException);
  _e.Exception = rb_cException;

  // ScriptErrors
  DEF_EXCEPTION(ScriptError, Exception)
    DEF_EXCEPTION(NotImplementedError, ScriptError)
    DEF_EXCEPTION(SyntaxError, ScriptError)

  // StandardError
  DEF_EXCEPTION(StandardError, Exception)
    DEF_EXCEPTION(ArgumentError, StandardError)
    DEF_EXCEPTION(IOError, StandardError)
    DEF_EXCEPTION(IndexError, StandardError)
    DEF_EXCEPTION(LocalJumpError, StandardError)
    DEF_EXCEPTION(NameError, StandardError)
      DEF_EXCEPTION(NoMethodError, NameError)
    DEF_EXCEPTION(RuntimeError, StandardError)
    DEF_EXCEPTION(SystemCallError, StandardError)
    DEF_EXCEPTION(TypeError, StandardError)

  rb_cSystemCallError->add_method("initialize", new RubyCMethod(systemcallerror_initialize, 2));
}

RubyValue exception_initialize(linked_ptr<Binding> &_b, RubyValue _self) {
  return _b->environment.NIL;
}

RubyValue exception_initialize_message(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  _self.call(_b, "message=", _args[0]);
  return _b->environment.NIL;
}

RubyValue exception_message(linked_ptr<Binding> &_b, RubyValue _self) {
  return _self.get_special<RubyObject>()->get_instance(_b->environment, "message");
}

RubyValue exception_message_assign(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  _self.get_special<RubyObject>()->set_instance(_b->environment, "message", _args[0]);
  return _args[0];
}

RubyValue systemcallerror_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyObject *o = _self.get_special<RubyObject>();
  o->set_instance(_b->environment, "message", _args[1]);
  o->set_instance(_b->environment, "errno", _args[0]);
  return _b->environment.NIL;
}

WorldException::WorldException(linked_ptr<Binding> &_b, RubyObject *_exception): exception(_exception)
{
  if (dynamic_cast<RubyClass *>(exception))
    exception = RubyValue::from_object(exception).call(_b, "new").get_special<RubyObject>();

  check_exception_object(_b);
}

WorldException::WorldException(linked_ptr<Binding> &_b, RubyObject *_object, const std::string &_message)
{
  RubyClass *_class = dynamic_cast<RubyClass *>(_object);
  if (_class)
    exception = RubyValue::from_object(_class).call(_b, "new", _b->environment.get_string(_message)).get_special<RubyObject>();
  else {
    exception = _object;
    RubyValue::from_object(exception).call(_b, "message=", _b->environment.get_string(_message));
  }

  check_exception_object(_b);
}

void WorldException::check_exception_object(linked_ptr<Binding> &_b) const {
  if (!exception->get_class()->has_ancestor(_b->environment.Exception)) 
    throw WorldException(_b, _b->environment.TypeError, "exception class/object expected");
}

SevereInternalError::SevereInternalError(const std::string &_message): message(_message)
{ }

SevereInternalError::~SevereInternalError() throw ()
{ }

