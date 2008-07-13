#ifndef MAIN_H
#define MAIN_H

#include "ast.h"
#include "parse.h"

extern bool omit_errors;
extern int yydebug;

int yyparse(Program *program);
void yyerror(Program *, char const *);

#endif

