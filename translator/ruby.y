
%{
  #include "global.h"

  #include <stdio.h>
  #include <math.h>
  #include <string>

  int yylex (void);
  void yyerror (char const *);
%}

%expect 14

%token NL
%token <identifier> IDENTIFIER

%token <string_literal> STRING_LITERAL 
%token <integer_literal> INTEGER_LITERAL
%token <floating_literal> FLOATING_LITERAL

%type <arglist> arglist
%type <expr> expr
%type <literal> literal
%type <funccall> funccall

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

expr:		funccall	{ $$ = static_cast<Expr *>($1); }
	      |	IDENTIFIER	{ $$ = static_cast<Expr *>($1); }
	      | literal	{ $$ = static_cast<Expr *>($1); }
	      | expr '+' expr
	      | expr '-' expr
	      | expr '*' expr
	      | expr '/' expr
	      | '-' expr %prec NEG	{ $$ = new UnaryOpExpr(NEGATE, $2); }
	      | expr '^' expr
	      | '(' expr ')'	{ $$ = $2; }
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
arglist:	expr			{ $$ = new ArgListExpr($1); }
	      |	arglist ',' expr	{ $$ = new ArgListExpr($1, $3); delete $1; }
;

literal:	STRING_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
	      |	INTEGER_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
	      | FLOATING_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
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

