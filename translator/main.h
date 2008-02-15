#ifndef MAIN_H
#define MAIN_H

#include "expr.h"
#include <exception>

int yyparse(Program *program);
void yyerror(Program *, char const *);
Program parse_code(const char *);

class ParseFailureException : public std::exception
{
  const char *what() const throw()
  {
    return "parsing failed";
  }
};

#endif

