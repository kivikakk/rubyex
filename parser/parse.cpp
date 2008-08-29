#include "parse.h"
#include "main.h"
#include "global.h"
#include "tests.h"

std::string syntax_error;

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
    ASSERT_LEN_RETURN(p, line, (unsigned int)length, p);

  return p;
}

void yyerror(Program *p, char const *s)
{
  syntax_error = s;
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

