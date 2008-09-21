#include <sstream>
#include "rarray.h"
#include "rclass.h"
#include "rmethod.h"
#include "rstring.h"
#include "rexception.h"

RubyValue array_initialize(linked_ptr<Binding> &, RubyValue);
RubyValue array_initialize_length(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_initialize_copies(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_new_idx(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_index_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_index_assign_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue array_each(linked_ptr<Binding> &, RubyValue, Block &);
RubyValue array_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue array_to_s(linked_ptr<Binding> &, RubyValue);

void RubyArrayEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cArray = _e.gc.track<RubyClass>(new RubyClass(_e, "Array"));
  rb_cArray->add_metaclass_method(_e, "[]", RubyMethod::Create(array_new_idx, ARGS_ARBITRARY));

  rb_cArray->add_method("initialize", linked_ptr<RubyMethod>(new RubyMultiCMethod(
    linked_ptr<RubyCMethod>(new RubyCMethod(array_initialize)),
    linked_ptr<RubyCMethod>(new RubyCMethod(array_initialize_length, 1)),
    linked_ptr<RubyCMethod>(new RubyCMethod(array_initialize_copies, 2)))));

  rb_cArray->add_method("[]", RubyMethod::Create(array_index_op, 1));
  rb_cArray->add_method("[]=", RubyMethod::Create(array_index_assign_op, 2));
  rb_cArray->add_method("each", RubyMethod::Create(array_each));
  rb_cArray->add_method("inspect", RubyMethod::Create(array_inspect));
  rb_cArray->add_method("to_s", RubyMethod::Create(array_to_s));

  _e.set_global_by_name("Array", rb_cArray);
  _e.Array = rb_cArray;
}

RubyValue array_initialize(linked_ptr<Binding> &_b, RubyValue _self)
{
  // Nothing to do.
  return _b->environment.NIL;
}

RubyValue array_initialize_length(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyArray *a = _self.get_special<RubyArray>();
  a->data.insert(a->data.begin(), _args[0].get_fixnum(), _b->environment.NIL);
  return _b->environment.NIL;
}

RubyValue array_initialize_copies(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyArray *a = _self.get_special<RubyArray>();
  a->data.insert(a->data.begin(), _args[0].get_fixnum(), _args[1]);
  return _b->environment.NIL;
}

RubyValue array_new_idx(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return O2V(_b->environment.gc.track(new RubyArray(_b->environment, _args)));
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

RubyValue array_index_assign_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyArray *arr = _self.get_special<RubyArray>();
  long index = _args[0].get_fixnum();
  long my_size = arr->data.size();

  if (index < 0)
    index += my_size;

  if (index < 0)
    throw WorldException(_b, _b->environment.IndexError, "index XXX out of array");

  while ((unsigned long)index >= arr->data.size())
    arr->data.push_back(_b->environment.NIL);

  arr->data[index] = _args[1];
  return _args[1];
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

    oss << it->inspect(_b);
  }
  oss << "]";

  return _b->environment.get_string(oss.str());
}

RubyValue array_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyArray *arr = _self.get_special<RubyArray>();

  std::ostringstream oss;
  for (std::vector<RubyValue>::const_iterator it = arr->data.begin(); it != arr->data.end(); ++it)
      oss << it->to_s(_b);

  return _b->environment.get_string(oss.str());
}

RubyArray::RubyArray(RubyEnvironment &_e): RubyObject(_e.Array)
{ }

RubyArray::RubyArray(RubyEnvironment &_e, long _length): RubyObject(_e.Array), data(_length)
{ }

RubyArray::RubyArray(RubyEnvironment &_e, long _length, RubyValue _obj): RubyObject(_e.Array), data(_length, _obj)
{ }

RubyArray::RubyArray(RubyEnvironment &_e, const std::vector<RubyValue> &_copy): RubyObject(_e.Array), data(_copy)
{ }

RubyValue RubyArray::index(linked_ptr<Binding> &_b, RubyValue _find) const {
  int i = 0;
  for (std::vector<RubyValue>::const_iterator it = data.begin(); it != data.end(); ++it, ++i)
    if (it->ruby_eq_op(_b, _find))
      return F2V(i);
  return _b->environment.NIL;
}

bool RubyArray::include(linked_ptr<Binding> &_b, RubyValue _find) const {
  return index(_b, _find) != _b->environment.NIL;
}

