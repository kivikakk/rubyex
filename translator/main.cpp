#include "main.h"

int main()
{
  Program p;

  int r = yyparse(&p);
  if (r != 0)
    return r;

  printf("parse success. Program object %p.\n", &p);
  p.p(0);

  return 0;
}

void yyerror(Program *p, char const *s)
{
  fprintf (stderr, "%s\n", s);
}

