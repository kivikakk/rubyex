#include <sstream>
#include <iostream>
#include "yywrap.h"
#include "vm/renvironment.h"
#include "vm/process.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"

int twophase_yywrap();

int twophase(int, char **)
{
  ruby_yywrap_delegate = twophase_yywrap;

  std::ostringstream oss;
  Program p(oss);
  int r = yyparse(&p);
  if (r != 0)
    return r;

  std::istringstream iss(oss.str());
  RubyEnvironment e;
  Reader reader(iss);
  Context *c = new Context(e, RubyValue::from_object(e.main));

  process(e, reader, c);

  return 0;
}

int twophase_yywrap()
{
  return 1;
}

