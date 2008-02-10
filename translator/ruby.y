
%{
  #include "global.h"

  #include <stdio.h>
  #include <math.h>
  #include <string>

  int yylex (void);
  void yyerror (Program *, char const *);
%}

//%expect 14

%parse-param {Program *program}

%token NL
%token <identifier> IDENTIFIER
%token <symbol> SYMBOL

%token <string_literal> STRING_LITERAL 
%token <integer_literal> INTEGER_LITERAL
%token <floating_literal> FLOATING_LITERAL

%type <expr> line

%type <arglist> arglist
%type <expr> expr
%type <literal> literal
%type <funccall> funccall

%left '<' '>' '=' NE LE GE
%left '+' '-'
%left '*' '/'
%left NEG
%right '^'

%nonassoc IDENTIFIER

%%

input:  	/* empty */
	      | input line	{ if ($2) program->add_expression($2); }
;

line: 		NL	{ $$ = NULL; }
	      | expr NL	{ $$ = $1; }
;

expr:		funccall	{ $$ = static_cast<Expr *>($1); }
	      |	IDENTIFIER	{ $$ = static_cast<Expr *>($1); }
	      | IDENTIFIER '=' expr { $$ = new AssignmentExpr($1, $3); }
	      |	SYMBOL	{ $$ = static_cast<Expr *>($1); }
	      | literal	{ $$ = static_cast<Expr *>($1); }
	      | expr '.' funccall { $$ = $3; dynamic_cast<FuncCallExpr *>($$)->target = $1; }
	      | expr '.' IDENTIFIER { $$ = new FuncCallExpr($1, $3, NULL); }
	      | expr '+' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("+"), new ArgListExpr($3)); }
	      | expr '-' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("-"), new ArgListExpr($3)); }
	      | expr '*' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("*"), new ArgListExpr($3)); }
	      | expr '/' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("/"), new ArgListExpr($3)); }
	      | '-' expr %prec NEG	{ $$ = new FuncCallExpr($2, new IdentifierExpr("-@"), NULL); }
	      | expr '^' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("^"), new ArgListExpr($3)); }
	      | '(' expr ')'	{ $$ = $2; }
;

/* An explicit function call, i.e. that has specified no parameters,
 * or has some parameters. Any inferred function call (e.g. 'gets')
 * will be treated like an IDENTIFIER in `expr', and we work it out
 * later. */
funccall:	IDENTIFIER '(' ')'	{ $$ = new FuncCallExpr(NULL, $1, NULL); }
	      | IDENTIFIER '(' arglist ')'	{ $$ = new FuncCallExpr(NULL, $1, $3); }
	      |	IDENTIFIER arglist	{ $$ = new FuncCallExpr(NULL, $1, $2); }
;

/* arglist is one or more, in line with funccall. */
arglist:	expr			{ $$ = new ArgListExpr($1); }
	      |	arglist ',' expr	{ $$ = new ArgListExpr($1, $3); }
;

literal:	STRING_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
	      |	INTEGER_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
	      | FLOATING_LITERAL	{ $$ = static_cast<LiteralExpr *>($1); }
;

%%

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

