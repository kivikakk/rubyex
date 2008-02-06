
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
%token <floating> FLOATING_LITERAL

%left '+' '-'
%left '*' '/'
%left NEG
%right '^'

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
	      | expr '+' expr
	      | expr '-' expr
	      | expr '*' expr
	      | expr '/' expr
	      | '-' expr %prec NEG
	      | expr '^' expr
	      | '(' expr ')'
;

literal:	STRING_LITERAL { free($1); /* for now. to stop our test cases leaking everywhere */ }
	      |	INTEGER_LITERAL
	      | FLOATING_LITERAL
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

