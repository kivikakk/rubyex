#include <sstream>
#include "rrange.h"
#include "rclass.h"
#include "rmethod.h"

RubyValue range_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue range_initialize_excl(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue range_each(linked_ptr<Binding> &, RubyValue, Block &);
RubyValue range_inspect(linked_ptr<Binding> &, RubyValue);

void RubyRangeEI::init(RubyEnvironment &_e) {
  RubyClass *rb_cRange = new RubyClass(_e, "Range");
  rb_cRange->add_method("initialize", new RubyMultiCMethod(
    new RubyCMethod(range_initialize, 2),
    new RubyCMethod(range_initialize_excl, 3)));
  rb_cRange->add_method("each", RubyMethod::Create(range_each));
  rb_cRange->add_method("inspect", RubyMethod::Create(range_inspect));
  rb_cRange->add_method("to_s", RubyMethod::Create(range_inspect));

  _e.set_global_by_name("Range", rb_cRange);
  _e.Range = rb_cRange;
}

RubyValue range_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  _self.set_instance("start", _args[0]);
  _self.set_instance("end", _args[1]);
  _self.set_instance("exclusive", _b->environment.get_truth(false));
  return _b->environment.NIL;
}

RubyValue range_initialize_excl(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  _self.set_instance("start", _args[0]);
  _self.set_instance("end", _args[1]);
  _self.set_instance("exclusive", _args[2]);
  return _b->environment.NIL;
}

RubyValue range_each(linked_ptr<Binding> &_b, RubyValue _self, Block &_block) {
  RubyValue s = _self.get_instance("start"), end = _self.get_instance("end");
  bool exclusive = _self.get_instance("exclusive").truthy(_b->environment);
  int ss;

  do {
    _block.call(_b, s);
    s = s.call(_b, "succ");

    ss = s.call(_b, "<=>", end).get_fixnum();
  } while (ss < 0 || (ss == 0 && !exclusive));

  return _self;
}

RubyValue range_inspect(linked_ptr<Binding> &_b, RubyValue _self) {
  std::ostringstream oss;
  oss << _self.get_instance("start").inspect(_b);
  if (_self.get_instance("exclusive").truthy(_b->environment))
    oss << "...";
  else
    oss << "..";
  oss << _self.get_instance("end").inspect(_b);
  return _b->environment.get_string(oss.str());
}

