#include <sstream>
#include <iostream>
#include <list>
#include <string>
#include "yywrap.h"
#include "vm/renvironment.h"
#include "vm/process.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"

extern FILE *yyin;

int twophase_yywrap();

int twophase(int argc, char **argv)
{
  ruby_yywrap_delegate = twophase_yywrap;

  std::list<std::string> args;
  for (int i = 1; i < argc; ++i)	/* we ignore argv0 */
    args.push_back(argv[i]);

  if (args.size() == 1) {
    yyin = fopen(args.begin()->c_str(), "r");
    args.pop_front();
  }

  std::ostringstream oss;
  Program p(oss);
  int r = yyparse(&p);
  if (r != 0)
    return r;

  std::istringstream iss(oss.str());
  RubyEnvironment e;
  Reader reader(iss);
  Context *c = new Context(e, RubyValue::from_object(e.main), e.Object);

  process(e, reader, c, NULL);

  return 0;
}

int twophase_yywrap()
{
  return 1;
}

