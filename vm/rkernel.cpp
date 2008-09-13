#include "rkernel.h"
#include "rmethod.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include "rstring.h"
#include "eval_hook.h"
#include "rbinding.h"
#include "rexception.h"
#include "global.h"
#include "rio.h"

RubyValue kernel_require(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_load_file(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_load_file_wrap(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_eval(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_binding(linked_ptr<Binding> &, RubyValue);
RubyValue kernel_eval(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_raise(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

RubyValue kernel_print(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_puts(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_p(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue kernel_gets(linked_ptr<Binding> &, RubyValue);

RubyValue kernel_backtick(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

void RubyKernelEI::init(RubyEnvironment &_e)
{
  _e.Kernel->add_module_method(_e, "require", RubyMethod::Create(kernel_require, 1));
  _e.Kernel->add_module_method(_e, "load", new RubyMultiCMethod(new RubyCMethod(kernel_load_file, 1), new RubyCMethod(kernel_load_file_wrap, 2)));

  _e.Kernel->add_module_method(_e, "binding", RubyMethod::Create(kernel_binding));
  _e.Kernel->add_module_method(_e, "eval", RubyMethod::Create(kernel_eval, ARGS_ARBITRARY));
  _e.Kernel->add_module_method(_e, "raise", RubyMethod::Create(kernel_raise, ARGS_ARBITRARY));

  _e.Kernel->add_module_method(_e, "print", RubyMethod::Create(kernel_print, ARGS_ARBITRARY));
  _e.Kernel->add_module_method(_e, "puts", RubyMethod::Create(kernel_puts, ARGS_ARBITRARY));
  _e.Kernel->add_module_method(_e, "p", RubyMethod::Create(kernel_p, ARGS_ARBITRARY));
  _e.Kernel->add_module_method(_e, "gets", RubyMethod::Create(kernel_gets));

  _e.Kernel->add_module_method(_e, "`", RubyMethod::Create(kernel_backtick, 1));
}

const char *REQUIRE_EXTENSIONS[] = {
  ".rb", NULL
};

RubyValue kernel_require_found(linked_ptr<Binding> &_b, RubyValue _self, const std::string &_filename) {
  RubyValue fns = _b->environment.get_string(_filename);
  if (_b->environment.RubyLoaded->include(_b, fns))
    return _b->environment.FALSE;

  _b->environment.RubyLoaded->data.push_back(fns);
  return _b->environment.get_truth(kernel_load(_b, _filename));
}

RubyValue kernel_require(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  std::string filename = _args[0].get_string();
  // XXX: path separators may differ
  bool has_extension = false;
  if (filename.find('/') == std::string::npos)
    has_extension = filename.find('.') != std::string::npos;
  else
    has_extension = filename.find('.', filename.find('/')) != std::string::npos;

  struct stat stat_buf;
  std::string compose;

  for (std::vector<RubyValue>::const_iterator it = _b->environment.RubyPath->data.begin(); it != _b->environment.RubyPath->data.end(); ++it) {
    if (has_extension) {
      if (stat(RubyIO::filename_join(it->get_string(), filename).c_str(), &stat_buf) == 0)
	return kernel_require_found(_b, _self, filename);
    } else {
      const char *itp = *REQUIRE_EXTENSIONS;
      while (itp) {
	compose = filename + itp;
	if (stat(RubyIO::filename_join(it->get_string(), compose).c_str(), &stat_buf) == 0)
	  return kernel_require_found(_b, _self, compose);
	++itp;
      }
    }
  }

  throw WorldException(_b, _b->environment.LoadError, "no such file to load -- " + filename);
}

RubyValue kernel_load_file(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  return _b->environment.get_truth(kernel_load(_b, _args[0].get_string()));
}

RubyValue kernel_load_file_wrap(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  return _b->environment.get_truth(kernel_load(_b, _args[0].get_string(), _args[1].truthy(_b->environment)));
  // XXX should not be _args[1].truthy, need to ascertain it's bool
}

bool kernel_load(linked_ptr<Binding> &_b, const std::string &_filename) {
  return kernel_load(_b, _filename, false);
}

bool kernel_load(linked_ptr<Binding> &_b, const std::string &_filename, bool _wrap) {
  std::cerr << "kernel loading " << _filename << " (wrap:" << _wrap << ")" << std::endl;
  return false;
}

RubyValue kernel_binding(linked_ptr<Binding> &_b, RubyValue _self) {
  return O2V(_b->environment.gc.track(new RubyBinding(_b)));
}

RubyValue kernel_eval(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyValue first = _args[0];
  if (first.object->get_class() != _b->environment.String)
    throw WorldException(_b, _b->environment.TypeError, "tried to eval non-String");

  linked_ptr<Binding> use_binding = _b;

  if (_args.size() == 2) {
    // Add Proc support for 2nd argument.

    RubyValue second = _args[1];
    if (second.object->get_class() != _b->environment._Binding)
      throw WorldException(_b, _b->environment.TypeError, "wrong argument type XXX (expected Binding [in future, Proc])");

    use_binding = second.get_special<RubyBinding>()->binding;
  }

  return eval_hook(use_binding, _self, first.get_string());
}

RubyValue kernel_raise(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  if (_args.size() == 0)
    throw WorldException(_b, _b->environment.NotImplementedError, "Kernel::raise: reraise, or raise RuntimeError");
  else if (_args.size() == 1) {
    RubyString *s = _args[0].get_special<RubyString>();
    if (s)
      throw WorldException(_b, _b->environment.RuntimeError, s->string_value);
    throw WorldException(_b, _args[0].get_special<RubyObject>());
  } else if (_args.size() == 2)
    throw WorldException(_b, _args[0].get_special<RubyObject>(), _args[1].get_string());
  else
    throw WorldException(_b, _b->environment.ArgumentError, "wrong number of arguments");
}

RubyValue kernel_print(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    if (it->object->get_class() == _b->environment.String)	// PlaysForSure
      std::cout << it->get_string();
  }

  return _b->environment.NIL;
}

RubyValue kernel_puts(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    std::vector<RubyValue> to_print;

    RubyValue result_val; RubyString *result;

    if (it->type == RubyValue::RV_OBJECT && it->object->get_class() == _b->environment.String)
      result_val = *it;
    else if (it->type == RubyValue::RV_OBJECT && it->object == _b->environment.NIL.object)
      result_val = it->call(_b, "inspect");
    else
      result_val = it->call(_b, "to_s");

    result = result_val.get_special<RubyString>();

    std::string s = result->string_value;
    if (result->string_value.find("\n") != s.length() - 1)
      result->string_value += "\n";

    to_print.push_back(result_val);
    _self.call(_b, "print", to_print);
  }

  if (_args.size() == 0)
    _self.call(_b, "print", _b->environment.get_string("\n"));

  return _b->environment.NIL;
}

RubyValue kernel_p(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  for (std::vector<RubyValue>::const_iterator it = _args.begin(); it != _args.end(); ++it) {
    RubyValue r = it->call(_b, "inspect");
    std::vector<RubyValue> rca; rca.push_back(r);
    _self.call(_b, "puts", rca);
  }

  return _b->environment.NIL;
}

RubyValue kernel_gets(linked_ptr<Binding> &_b, RubyValue _self)
{
  std::string inp;
  std::getline(std::cin, inp);
  if (!std::cin.eof())
    inp += '\n';
  else if (inp.size() == 0)
    return _b->environment.NIL;

  return _b->environment.get_string(inp);
}

RubyValue kernel_backtick(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args)
{
  FILE *pipe = popen(_args[0].get_string().c_str(), "r");
  std::string r; char buffer[1024]; int ar;

  while (!feof(pipe)) {
    ar = fread(buffer, 1, 1023, pipe);
    if (ar <= 0)
      break;
    buffer[ar] = 0;
    r += buffer;
  }
  ar = pclose(pipe);
  // TODO XXX: set $? to ar
  return _b->environment.get_string(r);
}

