#include <string.h>
#include "rio.h"
#include "rclass.h"
#include "rmethod.h"
#include "rexception.h"

RubyValue io_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue io_write(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_read(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_flush(linked_ptr<Binding> &, RubyValue);
RubyValue io_sync(linked_ptr<Binding> &, RubyValue);
RubyValue io_sync_set(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue file_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyIOEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cIO = RubyClass::create_class(_e, "IO");
  rb_cIO->add_method("initialize", RubyMethod::Create(io_initialize, ARGS_MINIMAL(1)));

  rb_cIO->add_method("write", RubyMethod::Create(io_write, 1));
  rb_cIO->add_method("read", RubyMethod::Create(io_read, ARGS_ARBITRARY));
  rb_cIO->add_method("flush", RubyMethod::Create(io_flush));
  rb_cIO->add_method("sync", RubyMethod::Create(io_sync));
  rb_cIO->add_method("sync=", RubyMethod::Create(io_sync_set, 1));

  _e.add_class("IO", rb_cIO);
  _e.IO = rb_cIO;

  RubyClass *rb_cFile = RubyClass::create_class_with_super(_e, "File", rb_cIO);
  rb_cFile->add_method("initialize", RubyMethod::Create(file_initialize, ARGS_MINIMAL(1)));

  _e.add_class("File", rb_cFile);
  _e.File = rb_cFile;
}

RubyValue io_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  int fd = _args[0].get_fixnum();
  std::string mode = "r";

  if (_args.size() == 2) {
    mode = RubyIO::rv_to_mode(_b, _args[1]);
  } else if (_args.size() != 1)
    throw WorldException(_b, _b->environment.ArgumentError, "incorrect number of arguments (expected 1 or 2)");

  self->init(fd, mode.c_str());
  return _b->environment.NIL;
}

RubyValue io_write(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  std::string &s = _args[0].get_special<RubyString>()->string_value;

  return RubyValue::from_fixnum(self->write(s));
}

RubyValue io_read(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  int to_read = -1;
  if (_args.size() >= 1) {
    RubyValue a0 = _args[0];
    if (a0.type == RubyValue::RV_FIXNUM)
      to_read = a0.fixnum;
    else if (a0.type == RubyValue::RV_OBJECT && a0.object == _b->environment.NIL.object)
      to_read = -1;	// just reinforcing that.
    else
      throw WorldException(_b, _b->environment.ArgumentError, "first argument should be Fixnum or nil");
  }

  std::string r = self->read(to_read);
  if (_args.size() == 2) {
    _args[1].get_special<RubyString>()->string_value = r;
    return _args[1];
  }
  return _b->environment.get_string(r);
}

RubyValue io_flush(linked_ptr<Binding> &_b, RubyValue _self)
{
  _self.get_special<RubyIO>()->flush();
  return _self;
}

RubyValue io_sync(linked_ptr<Binding> &_b, RubyValue _self)
{
  return _b->environment.get_truth(_self.get_special<RubyIO>()->sync);
}

RubyValue io_sync_set(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return _b->environment.get_truth(_self.get_special<RubyIO>()->sync = _args[0].truthy(_b->environment));
}

RubyValue file_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  std::string mode = "r";

  if (_args.size() == 2)
    mode = RubyIO::rv_to_mode(_b, _args[1]);
  else if (_args.size() != 1)
    throw WorldException(_b, _b->environment.ArgumentError, "incorrect number of arguments (expected 1 or 2)");

  self->file = fopen(_args[0].get_special<RubyString>()->string_value.c_str(), mode.c_str());
  return _b->environment.NIL;
}

RubyIO::RubyIO(RubyEnvironment &_e): RubyObject(new NamedLazyClass(_e, "IO")), sync(false), file(NULL)
{ }

RubyIO::RubyIO(RubyEnvironment &_e, int _fd, const char *_mode): RubyObject(new NamedLazyClass(_e, "IO")), sync(false)
{
  init(_fd, _mode);
}

void RubyIO::init(int _fd, const char *_mode)
{
  this->file = fdopen(_fd, _mode);
}

RubyIO::~RubyIO()
{
  fclose(this->file);
}

std::string RubyIO::rv_to_mode(linked_ptr<Binding> &_b, RubyValue _val)
{
  char mode[10]; mode[0] = 0;

  if (_val.type == RubyValue::RV_OBJECT) {
    RubyString *a1s = _val.get_special<RubyString>();
    if (a1s) {
      strncpy(mode, a1s->string_value.c_str(), 9);
      mode[a1s->string_value.length()] = 0;
    } else if (_val.object == _b->environment.NIL.object)
      strcpy(mode, "r");
    else
      throw WorldException(_b, _b->environment.TypeError, "io_initialize doesn't know what to do for second argument");
  } else if (_val.type == RubyValue::RV_FIXNUM) {
    if (_val.fixnum & 2) {
      if (_val.fixnum & 1024)
	strcpy(mode, "a+");
      else
	strcpy(mode, "r+");
    } else if (_val.fixnum & 1) {
      if (_val.fixnum & 1024)
	strcpy(mode, "a");
      else
	strcpy(mode, "w");	// fdopen doesn't truncate w/ `w' or `w+'. see fopen(3).
    } else
      strcpy(mode, "r");
  } else
      throw WorldException(_b, _b->environment.TypeError, "io_initialize can't recognise second argument");

  return mode;
}

std::string RubyIO::read()
{
  char buf[1024];
  std::string s;
  int readen;
  do {
    readen = fread(buf, sizeof(char), 1024, this->file);
    if (readen > 0)
      s += std::string(buf, readen);
  } while (readen == 1024);
  return s;
}

std::string RubyIO::read(int _length)
{
  if (_length == -1)
    return this->read();

  char buf[_length];
  int readen = fread(buf, sizeof(char), _length, this->file);
  if (readen >= 0) 
    return std::string(buf, readen);
  return "";	// XXX this is error condition!
}

int RubyIO::write(const std::string &_data)
{
  int wr = fwrite(_data.c_str(), sizeof(char), _data.length(), this->file);
  if (sync)
    flush();
  return wr;
}

void RubyIO::flush()
{
  fflush(this->file);
}

