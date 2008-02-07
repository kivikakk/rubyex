
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

%expect 14

%token NL
%token <string> IDENTIFIER

%token <string> STRING_LITERAL 
%token <integer> INTEGER_LITERAL
%token <floating> FLOATING_LITERAL

%left '<' '>' '=' NE LE GE
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

expr:		funccall
	      |	IDENTIFIER
	      | literal
	      | expr '+' expr
	      | expr '-' expr
	      | expr '*' expr
	      | expr '/' expr
	      | '-' expr %prec NEG
	      | expr '^' expr
	      | '(' expr ')'
;

/* An explicit function call, i.e. that has specified no parameters,
 * or has some parameters. Any inferred function call (e.g. 'gets')
 * will be treated like an IDENTIFIER in `expr', and we work it out
 * later. */
funccall:	IDENTIFIER '(' ')'
	      | IDENTIFIER '(' arglist ')'
	      |	IDENTIFIER arglist
;

/* arglist is one or more, in line with funccall. */
arglist:	expr			{ printf("arglist: expr\n"); }
	      |	arglist ',' expr	{ printf("arglist: arglist,expr\n"); }
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

