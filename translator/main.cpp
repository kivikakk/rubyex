#include "main.h"
#include "global.h"
#include "tests.h"
#include <list>
#include <string>
#include <exception>

int main_parser(std::list<std::string> &);
int main_test(std::list<std::string> &);

int main(int argc, char **argv)
{
  std::string invoked_as = argv[0];
  std::list<std::string> arguments;
  for (int i = 1; i < argc; ++i)
    arguments.push_back(argv[i]);

  if (invoked_as.find('/') != std::string::npos)
    invoked_as = invoked_as.substr(invoked_as.find('/') + 1);

  if (invoked_as == "test")
    return main_test(arguments);
  
  return main_parser(arguments);
}

int main_parser(std::list<std::string> &arguments)
{
  Program p;

  int r = yyparse(&p);
  if (r != 0)
    return r;

  printf("parse success. Program object %p.\n", &p);
  p.p(0);

  return 0;
}

int main_test(std::list<std::string> &arguments)
{
  return tests_all();
}

Program parse_code(const char *code)
{
  Program p;

  yy_scan_string(code);
  int r = yyparse(&p);
  if (r != 0)
    throw ParseFailureException();

  return p;
}

void yyerror(Program *p, char const *s)
{
  fprintf (stderr, "%s\n", s);
}

