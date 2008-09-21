#include "vm/renvironment.h"
#include "vm/rvalue.h"
#include "vm/process.h"
#include "vm/rexception.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"
#include "parser/parse.h"
#include "vm/binding.h"
#include <string>
#include <sstream>

RubyValue eval_hook(linked_ptr<Binding> &_b, RubyValue _self, const std::string &_code)
{
  std::ostringstream bytecode;
  Program p(bytecode);

  yy_scan_string(_code.c_str());
  int r = yyparse(&p);
  if (r != 0) {
    RubyValue rv = RubyValue::from_object(_b->environment.SyntaxError).call(_b, "new");
    rv.set_instance("message", _b->environment.get_string(std::string("compile error (") + syntax_error + ")"));
    rv.set_instance("underlying", _b->environment.get_string(syntax_error));
    throw WorldException(_b, rv.object);
  }
  
  std::istringstream iss(bytecode.str());
  Reader reader(iss);

  Context *c = new Context(_b);
  RubyValue ret = process(_b->environment, reader, c, NULL);
  delete c;

  return ret;
}

