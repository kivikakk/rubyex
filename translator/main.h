#ifndef MAIN_H
#define MAIN_H

#include "expr.h"

int yyparse(Program *program);
void yyerror(Program *, char const *);

#endif

