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
  if (r != 0)
    throw WorldException(_b, _b->environment.SyntaxError, "compile error (" + syntax_error + ")");
  
  std::istringstream iss(bytecode.str());
  Reader reader(iss);

  Context *c = new Context(_b);
  return process(_b->environment, reader, c, NULL);
}

