#include "vm/renvironment.h"
#include "vm/rvalue.h"
#include "vm/process.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"
#include <string>
#include <sstream>

RubyValue eval_hook(RubyEnvironment &_e, Context *_context, RubyValue _self, const std::string &_code)
{
  std::ostringstream bytecode;
  Program p(bytecode);

  yy_scan_string(_code.c_str());
  int r = yyparse(&p);
  if (r != 0) {
    std::cerr << "ruby eval_hook: yyparse returned " << r << std::endl;
    throw;
  }
  
  std::istringstream iss(bytecode.str());
  Reader reader(iss);

  return process(_e, reader, _context);
}

