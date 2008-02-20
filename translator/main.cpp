#include "main.h"
#include "global.h"
#include "tests.h"
#include <list>
#include <string>
#include <exception>
#include <algorithm>

bool omit_errors = false;

int main_parser(std::list<std::string> &);
int main_test(std::list<std::string> &);

int main(int argc, char **argv)
{
  std::string invoked_as = argv[0];
  std::list<std::string> arguments;
  for (int i = 1; i < argc; ++i)
    arguments.push_back(argv[i]);

  if (std::find(arguments.begin(), arguments.end(), "-t") != arguments.end()) {
    yydebug = 1;
    std::list<std::string>::iterator it = std::find(arguments.begin(), arguments.end(), "-t");
    arguments.erase(it);
  }

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

  p.p(0);

  return 0;
}

int main_test(std::list<std::string> &arguments)
{
  return tests_all();
}

Program parse_code(const char *code)
{
  return parse_code(code, 0, -1);
}

Program parse_code(const char *code, int line, int length)
{
  Program p;

  yy_scan_string(code);
  int r = yyparse(&p);
  if (r != 0)
    throw ParseFailureException(code);

  if (length != -1)
    ASSERT_LEN_RETURN(p, line, length, p);

  return p;
}

void yyerror(Program *p, char const *s)
{
  if (!omit_errors)
    std::cerr << s << std::endl;
}

ParseFailureException::ParseFailureException() { }
ParseFailureException::ParseFailureException(const std::string &_code): code(_code) { }
ParseFailureException::~ParseFailureException() throw() { }

const char *ParseFailureException::what() const throw()
{
  if (code.length() == 0)
    return "parse failure";
  else
    return (std::string("parse failure in: ") + code).c_str();
}

