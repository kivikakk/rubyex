#include <sstream>
#include "rarray.h"
#include "rclass.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue array_new(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_new_length(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_new_copies(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_new_idx(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_index_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_each(linked_ptr<Binding> &, RubyValue, Block &);
RubyValue array_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue array_to_s(linked_ptr<Binding> &, RubyValue);

void RubyArrayEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cArray = RubyClass::create_class(_e, "Array");
  rb_cArray->add_metaclass_method(_e, "new", RubyMethod::Create(array_new, ARGS_ARBITRARY));
  rb_cArray->add_metaclass_method(_e, "[]", RubyMethod::Create(array_new_idx, ARGS_ARBITRARY));

  rb_cArray->add_method("[]", RubyMethod::Create(array_index_op, 1));
  rb_cArray->add_method("each", RubyMethod::Create(array_each));
  rb_cArray->add_method("inspect", RubyMethod::Create(array_inspect));
  rb_cArray->add_method("to_s", RubyMethod::Create(array_to_s));

  _e.add_class("Array", rb_cArray);
  _e.Array = rb_cArray;
}

RubyValue array_new(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  switch (_args.size()) {
    case 0:
      return RubyValue::from_object(_b->environment.gc.track(new RubyArray(_b->environment)));
    case 1:
      return array_new_length(_b, _self, _args);
    case 2:
      return array_new_copies(_b, _self, _args);
  }
  std::cerr << "Array::new: no matching arg count?" << std::endl;
  throw;	// XXX -- we need a better way of handling arg counts in general, right?
}

RubyValue array_new_length(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyArray(_b->environment, _args[0].get_fixnum())));
}

RubyValue array_new_copies(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyArray(_b->environment, _args[0].get_fixnum(), _args[1])));
}

RubyValue array_new_idx(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyArray(_b->environment, _args)));
}

RubyValue array_index_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyArray *arr = _self.get_special<RubyArray>();
  long index = _args[0].get_fixnum();
  long my_size = arr->data.size();

  if (index < 0)
    index += my_size;

  if (index < 0 || index >= my_size)
    return _b->environment.NIL;

  return arr->data[index];
}

RubyValue array_each(linked_ptr<Binding> &_b, RubyValue _self, Block &_block)
{
  RubyArray *arr = _self.get_special<RubyArray>();
  for (std::vector<RubyValue>::iterator it = arr->data.begin(); it != arr->data.end(); ++it)
    _block.call(_b, *it);
  return _self;
}

RubyValue array_inspect(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyArray *arr = _self.get_special<RubyArray>();

  std::ostringstream oss;
  oss << "[";

  bool start = true;
  for (std::vector<RubyValue>::const_iterator it = arr->data.begin(); it != arr->data.end(); ++it) {
    if (!start)
      oss << ", ";
    else
      start = false;

    oss << it->call(_b, "inspect").get_special<RubyString>()->string_value;
  }
  oss << "]";

  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, oss.str())));
}

RubyValue array_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyArray *arr = _self.get_special<RubyArray>();

  std::ostringstream oss;
  for (std::vector<RubyValue>::const_iterator it = arr->data.begin(); it != arr->data.end(); ++it)
      oss << it->call(_b, "to_s").get_special<RubyString>()->string_value;

  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, oss.str())));
}

RubyArray::RubyArray(RubyEnvironment &_e): RubyObject(new NamedLazyClass(_e, "Array"))
{ }

RubyArray::RubyArray(RubyEnvironment &_e, long _length): RubyObject(new NamedLazyClass(_e, "Array")), data(_length)
{ }

RubyArray::RubyArray(RubyEnvironment &_e, long _length, RubyValue _obj): RubyObject(new NamedLazyClass(_e, "Array")), data(_length, _obj)
{ }

RubyArray::RubyArray(RubyEnvironment &_e, const std::vector<RubyValue> &_copy): RubyObject(new NamedLazyClass(_e, "Array")), data(_copy)
{ }
