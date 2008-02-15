#ifndef MAIN_H
#define MAIN_H

#include "expr.h"
#include <exception>

extern bool omit_errors;

int yyparse(Program *program);
void yyerror(Program *, char const *);
Program parse_code(const char *);
Program parse_code(const char *, int, int);

class ParseFailureException : public std::exception
{
  const char *what() const throw()
  {
    return "parsing failed";
  }
};

#endif

