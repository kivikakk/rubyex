#ifndef PARSE_H
#define PARSE_H

#include "ast.h"
#include <exception>
#include <string>

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

extern std::string syntax_error;

#endif
