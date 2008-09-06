#include <string.h>
#include "rio.h"
#include "rclass.h"
#include "rmethod.h"
#include "rexception.h"
#include <errno.h>

RubyValue io_open(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_open_block(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

RubyValue io_initialize_fd(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_initialize_fd_mode(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_write(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_read(linked_ptr<Binding> &, RubyValue);
RubyValue io_read_len(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue io_flush(linked_ptr<Binding> &, RubyValue);
RubyValue io_close(linked_ptr<Binding> &, RubyValue);
RubyValue io_sync(linked_ptr<Binding> &, RubyValue);
RubyValue io_sync_set(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue file_initialize_file(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue file_initialize_file_mode(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyIOEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cIO = RubyClass::create_class(_e, "IO");
  rb_cIO->add_metaclass_method(_e, "open",
    new RubyMultiCMethod(
      new RubyCMethod(io_open, ARGS_MINIMAL(1)),
      new RubyCMethod(io_open_block, ARGS_MINIMAL(1))
      ) );

  rb_cIO->add_method("initialize", new RubyMultiCMethod(
    new RubyCMethod(io_initialize_fd, 1), new RubyCMethod(io_initialize_fd_mode, 2)));

  rb_cIO->add_method("write", RubyMethod::Create(io_write, 1));
  rb_cIO->add_method("read", new RubyMultiCMethod(new RubyCMethod(io_read), new RubyCMethod(io_read_len, ARGS_MINIMAL(1))));
  rb_cIO->add_method("flush", RubyMethod::Create(io_flush));
  rb_cIO->add_method("close", RubyMethod::Create(io_close));
  rb_cIO->add_method("sync", RubyMethod::Create(io_sync));
  rb_cIO->add_method("sync=", RubyMethod::Create(io_sync_set, 1));

  _e.set_global_by_name("IO", rb_cIO);
  _e.IO = rb_cIO;

  RubyClass *rb_cFile = RubyClass::create_class_with_super(_e, "File", rb_cIO);
  rb_cFile->add_method("initialize", new RubyMultiCMethod(new RubyCMethod(file_initialize_file, 1), new RubyCMethod(file_initialize_file_mode, 2)));

  _e.set_global_by_name("File", rb_cFile);
  _e.File = rb_cFile;
}

RubyValue io_open(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return _self.call(_b, "new", _args);
}

RubyValue io_open_block(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args, Block &_block)
{
  RubyValue inst = _self.call(_b, "new", _args);

  RubyValue rv;
  try {
    rv = _block.call(_b, inst);
  } catch (WorldException) {
    inst.get_special<RubyIO>()->close(_b);
    throw;
  }

  inst.get_special<RubyIO>()->close(_b);
  return rv;
}

RubyValue io_initialize_fd(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  self->init(_b, _args[0].get_fixnum(), "r");
  return _b->environment.NIL;
}

RubyValue io_initialize_fd_mode(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  int fd = _args[0].get_fixnum();
  std::string mode = RubyIO::rv_to_mode(_b, _args[1]);

  self->init(_b, fd, mode.c_str());
  return _b->environment.NIL;
}

RubyValue io_write(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return F2V(_self.get_special<RubyIO>()->write(_b, _args[0].get_string()));
}

RubyValue io_read(linked_ptr<Binding> &_b, RubyValue _self)
{
  RubyIO *self = _self.get_special<RubyIO>();
  return _b->environment.get_string(self->read(_b));
}

RubyValue io_read_len(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
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

  std::string r = self->read(_b, to_read);
  if (_args.size() == 2) {
    _args[1].set_string(r);
    return _args[1];
  }
  return _b->environment.get_string(r);
}

RubyValue io_flush(linked_ptr<Binding> &_b, RubyValue _self)
{
  _self.get_special<RubyIO>()->flush(_b);
  return _self;
}

RubyValue io_close(linked_ptr<Binding> &_b, RubyValue _self)
{
  _self.get_special<RubyIO>()->close(_b);
  return _b->environment.NIL;
}

RubyValue io_sync(linked_ptr<Binding> &_b, RubyValue _self)
{
  return _b->environment.get_truth(_self.get_special<RubyIO>()->sync);
}

RubyValue io_sync_set(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  return _b->environment.get_truth(_self.get_special<RubyIO>()->sync = _args[0].truthy(_b->environment));
}

RubyValue file_initialize_file(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  self->file = fopen(_args[0].get_string().c_str(), "r");
  if (!self->file) {
    int s_er = errno;
    char *s_ms = strerror(s_er);
    throw WorldException(_b, _b->environment.errno_exception(_b, s_er, s_ms));
  }
  return _b->environment.NIL;
}

RubyValue file_initialize_file_mode(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  RubyIO *self = _self.get_special<RubyIO>();
  std::string mode = RubyIO::rv_to_mode(_b, _args[1]);

  self->file = fopen(_args[0].get_string().c_str(), mode.c_str());
  if (!self->file) {
    int s_er = errno;
    char *s_ms = strerror(s_er);
    throw WorldException(_b, _b->environment.errno_exception(_b, s_er, s_ms));
  }
  return _b->environment.NIL;
}

RubyIO::RubyIO(RubyEnvironment &_e): RubyObject(new NamedLazyClass(_e, "IO")), sync(false), file(NULL)
{ }

RubyIO::RubyIO(linked_ptr<Binding> &_b, int _fd, const char *_mode): RubyObject(new NamedLazyClass(_b->environment, "IO")), sync(false)
{
  init(_b, _fd, _mode);
}

RubyIO::~RubyIO()
{
  if (this->file)
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

void RubyIO::init(linked_ptr<Binding> &_b, int _fd, const char *_mode)
{
  this->file = fdopen(_fd, _mode);
  if (!this->file) {
    int s_er = errno;
    char *s_ms = strerror(s_er);
    throw WorldException(_b, _b->environment.errno_exception(_b, s_er, s_ms));
  }
}

std::string RubyIO::read(linked_ptr<Binding> &_b)
{
  _check(_b);

  if (feof(this->file))
    throw IOEOFError();

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

std::string RubyIO::read(linked_ptr<Binding> &_b, int _length)
{
  if (_length == -1)
    return this->read(_b);

  _check(_b);

  if (feof(this->file))
    throw IOEOFError();

  char buf[_length];
  int readen = fread(buf, sizeof(char), _length, this->file);
  if (readen >= 0) 
    return std::string(buf, readen);

  throw IOEOFError();
}

int RubyIO::write(linked_ptr<Binding> &_b, const std::string &_data)
{
  _check(_b);
  int wr = fwrite(_data.c_str(), sizeof(char), _data.length(), this->file);
  if (sync)
    flush(_b);
  return wr;
}

void RubyIO::flush(linked_ptr<Binding> &_b)
{
  _check(_b);
  fflush(this->file);
}

void RubyIO::close(linked_ptr<Binding> &_b)
{
  _check(_b);
  fclose(this->file);
  this->file = NULL;
}

void RubyIO::_check(linked_ptr<Binding> &_b)
{
  if (!this->file)
    throw WorldException(_b, _b->environment.IOError, "closed stream");
}

