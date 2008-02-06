
%{
  #include <stdio.h>
  #include <math.h>

  int yylex (void);
  void yyerror (char const *);
%}

%union {
  int integer;
  double floating;
  char *string;
}

%token NL
%token <string> IDENTIFIER

%token <string> STRING_LITERAL 
%token <integer> INTEGER_LITERAL

%%

input:  	/* empty */
	      | input line
;

line: 		NL
	      | statement NL
;

statement:	IDENTIFIER '=' expr
	      |	expr
;

expr:		IDENTIFIER
	      | literal
;

literal:	STRING_LITERAL { printf("I understood a string literal: \"%s\".\n", $1); }
	      |	INTEGER_LITERAL
;

%%

int
main (void)
{
  return yyparse ();
}

void
yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}

