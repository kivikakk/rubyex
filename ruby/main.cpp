#include <sstream>
#include <iostream>
#include "vm/renvironment.h"
#include "vm/process.h"
#include "parser/ast.h"
#include "parser/main.h"

bool omit_errors = false; 	// required for parser.

int main()
{
  std::ostringstream oss;
  Program p(oss);
  int r = yyparse(&p);
  if (r != 0)
    return r;

  std::istringstream iss(oss.str());
  RubyEnvironment e;
  Reader reader(iss);
  std::vector<Context *> cs;
  Context *c = new Context(&e, RubyValue::from_object(e.main));

  process(e, reader, cs, c);

  return 0;
}

