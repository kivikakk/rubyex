#include "rregexp.h"

RubyValue regexp_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyRegexpEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cRegexp = _e.gc.track<RubyClass>(new RubyClass(_e, "Regexp"));

  rb_cRegexp->add_method("initialize", RubyMethod::Create(regexp_initialize, 1));

  _e.set_global_by_name("Regexp", rb_cRegexp);
  _e.Regexp = rb_cRegexp;
}

RubyValue regexp_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  return _b->environment.NIL;
}

