#ifndef MAIN_H
#define MAIN_H

#include "expr.h"
#include <exception>

extern bool omit_errors;
extern int yydebug;

int yyparse(Program *program);
void yyerror(Program *, char const *);
Program parse_code(const char *);
Program parse_code(const char *, int, int);

class ParseFailureException : public std::exception
{
  public:
    ParseFailureException();
    ParseFailureException(const std::string &);
    ~ParseFailureException() throw();

    const char *what() const throw();

  protected:
    std::string code;
};

#endif

