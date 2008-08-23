#include <sstream>
#include "rhash.h"
#include "block.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue hash_new(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue hash_new_default(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue hash_new_idx(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue hash_index_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue hash_each(linked_ptr<Binding> &, RubyValue, Block &);
RubyValue hash_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue hash_to_s(linked_ptr<Binding> &, RubyValue);

void RubyHashEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cHash = RubyClass::create_class(_e, "Hash");
  rb_cHash->add_metaclass_method(_e, "new", RubyMethod::Create(hash_new, ARGS_ARBITRARY));
  rb_cHash->add_metaclass_method(_e, "[]", RubyMethod::Create(hash_new_idx, ARGS_ARBITRARY));

  rb_cHash->add_method("[]", RubyMethod::Create(hash_index_op, 1));
  rb_cHash->add_method("each", RubyMethod::Create(hash_each));
  rb_cHash->add_method("inspect", RubyMethod::Create(hash_inspect));
  rb_cHash->add_method("to_s", RubyMethod::Create(hash_to_s));

  _e.add_class("Hash", rb_cHash);
  _e.Hash = rb_cHash;
}

RubyValue hash_new(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  switch (_args.size()) {
    case 0:
      return RubyValue::from_object(_b->environment.gc.track(new RubyHash(_b->environment)));
    case 1:
      return hash_new_default(_b, _self, _args);
  }
  std::cerr << "Hash::new: no matching arg count?" << std::endl;
  throw;
}

RubyValue hash_new_default(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyHash(_b->environment, _args[0])));
}

RubyValue hash_new_idx(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyHash(_b->environment, _args)));
}

RubyValue hash_index_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyHash *hash = _self.get_special<RubyHash>();
  RubyValue index = _args[0];

  return hash->get(_b, index);
}

RubyValue hash_each(linked_ptr<Binding> &_b, RubyValue _self, Block &_block)
{
  RubyHash *hash = _self.get_special<RubyHash>();
  for (RubyHash::internal_t::iterator it = hash->begin(); it != hash->end(); ++it) {
    // XXX
  }
  return _self;
}

RubyValue hash_inspect(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyHash *hash = _self.get_special<RubyHash>();

  std::ostringstream oss;
  oss << "{";

  bool start = true;
  for (RubyHash::internal_t::const_iterator it = hash->begin(); it != hash->end(); ++it) {
    if (!start)
      oss << ", ";
    else
      start = false;

    oss << it->first.call(_b, "inspect").get_special<RubyString>()->string_value;
    oss << "=>";
    oss << it->second.call(_b, "inspect").get_special<RubyString>()->string_value;
  }
  oss << "}";

  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, oss.str())));
}

RubyValue hash_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyHash *hash = _self.get_special<RubyHash>();

  std::ostringstream oss;

  for (RubyHash::internal_t::const_iterator it = hash->begin(); it != hash->end(); ++it) {
    oss << it->first.call(_b, "to_s").get_special<RubyString>()->string_value;
    oss << it->second.call(_b, "to_s").get_special<RubyString>()->string_value;
  }

  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, oss.str())));
}

RubyHash::RubyHash(RubyEnvironment &_e): RubyObject(new NamedLazyClass(_e, "Hash")), default_value(_e.NIL)
{ }

RubyHash::RubyHash(RubyEnvironment &_e, RubyValue _default_value): RubyObject(new NamedLazyClass(_e, "Hash")), default_value(_default_value)
{ }

RubyHash::RubyHash(RubyEnvironment &_e, const std::vector<RubyValue> &_values): RubyObject(new NamedLazyClass(_e, "Hash")), default_value(_e.NIL)
{
  if (_values.size() % 2 == 1) {
    std::cerr << "odd number of arguments for Hash" << std::endl;
    throw;
  }

//  for (unsigned int idx = 0; idx < _values.size(); idx += 2)
//    set(_values[idx + 0], _values[idx + 1]);
}

void RubyHash::set(linked_ptr<Binding> &_b, RubyValue _key, RubyValue _value)
{
  internal_t::iterator it = matching_key(_b, _key);
  if (it != data.end()) {
    it->second = _value;
    return;
  }

  data[_key] = _value;
}

RubyValue RubyHash::get(linked_ptr<Binding> &_b, RubyValue _key)
{
  internal_t::iterator it = matching_key(_b, _key);
  if (it != data.end())
    return it->second;
  return default_value;
}

RubyHash::internal_t::iterator RubyHash::begin() { return data.begin(); } 
RubyHash::internal_t::iterator RubyHash::end() { return data.end(); }

RubyHash::internal_t::iterator RubyHash::matching_key(linked_ptr<Binding> &_b, RubyValue _compere)
{
  // pay close attention to the muso/French/? joke in the signature. (no, there is no context)
  for (internal_t::iterator it = data.begin(); it != data.end(); ++it)
    if (_compere.call(_b, "eql?", _compere).truthy(_b->environment))
      return it;
  return data.end();
}

