
%{
  #include "global.h"
  #include "main.h"

  #include <stdio.h>
  #include <math.h>
  #include <string>
%}

//%expect 14

%parse-param {Program *program}
%error-verbose

%token BLOCK_ARGUMENT_START BLOCK_ARGUMENT_END
%token ARG_BRACKET
%token NL DO END BLOCK_FINISH
%token END_OF_FILE 0 "end of file"
%token <symbol> SYMBOL

%token <string_literal> STRING_LITERAL 
%token <integer_literal> INTEGER_LITERAL
%token <floating_literal> FLOATING_LITERAL
%token <boolean_literal> BOOLEAN_LITERAL

%type <expr> line expr compiled_expr
%type <block> block block_content block_line
%type <arglist> arglist
%type <deflist> deflist block_arguments block_argument_contents
%type <literal> literal
%type <funccall> funccall

%left '<' '>' '=' NE LE GE
%left '+' '-'
%left '*' '/'
%left NEG
%right '^'
/* this is not exponentation in ruby; TODO */

%nonassoc <identifier> IDENTIFIER FUNCTION_CALL
%nonassoc DO END
%nonassoc '.'
%nonassoc '{' '}' '(' ')'

%%

input:  	/* empty */
	      | input line { if ($2) program->add_expression($2); }
;

line: 		NL	{ $$ = NULL; }
	      | expr NL	{ $$ = $1; }
	      | ';' { $$ = NULL; }
	      | expr ';' { $$ = $1; }
	      | expr BLOCK_FINISH { $$ = $1; IF_DEBUG printf("BLOCK_FINISH\n"); }
	      | expr END_OF_FILE { $$ = $1; }
;

expr:		funccall { $$ = $1; }
	      |	IDENTIFIER	{ $$ = $1; }
	      |	FUNCTION_CALL { $$ = new FuncCallExpr(NULL, $1, NULL, NULL); }
	      |	IDENTIFIER block { $$ = new FuncCallExpr(NULL, $1, NULL, $2); }
	      |	FUNCTION_CALL block { $$ = new FuncCallExpr(NULL, $1, NULL, $2); }
	      | IDENTIFIER '=' expr { $$ = new AssignmentExpr($1, $3); }
	      |	SYMBOL	{ $$ = $1; }
	      | literal	{ $$ = $1; }
	      | expr '.' funccall { $3->target = $1; $$ = $3; }
	      | expr '.' IDENTIFIER  { $$ = new FuncCallExpr($1, $3, NULL, NULL); }
	      | expr '.' FUNCTION_CALL  { $$ = new FuncCallExpr($1, $3, NULL, NULL); }
	      | expr '.' IDENTIFIER block  { $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '.' FUNCTION_CALL block  { $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '+' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("+"), new ArgListExpr($3), NULL); }
	      | expr '-' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("-"), new ArgListExpr($3), NULL); }
	      | expr '*' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("*"), new ArgListExpr($3), NULL); }
	      | expr '/' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("/"), new ArgListExpr($3), NULL); }
	      | '-' expr %prec NEG	{ $$ = new FuncCallExpr($2, new IdentifierExpr("-@"), NULL, NULL); }
	      | expr '^' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("^"), new ArgListExpr($3), NULL); }
	      | '(' expr ')'	{ $$ = $2; }
;

compiled_expr:	expr
;

/* An explicit function call, i.e. that has specified no parameters,
 * or has some parameters. Any inferred function call (e.g. 'gets')
 * will be treated like an IDENTIFIER in `expr', and we work it out
 * later. */
funccall:	IDENTIFIER arglist	{ $$ = new FuncCallExpr(NULL, $1, $2, NULL);  }
	      |	IDENTIFIER arglist block	{ $$ = new FuncCallExpr(NULL, $1, $2, $3); }
	      |	FUNCTION_CALL arglist	{ $$ = new FuncCallExpr(NULL, $1, $2, NULL);  }
	      |	FUNCTION_CALL arglist block	{ $$ = new FuncCallExpr(NULL, $1, $2, $3); }
	      | IDENTIFIER '(' ')'	{ $$ = new FuncCallExpr(NULL, $1, NULL, NULL); }
	      | IDENTIFIER '(' ')' block	{ $$ = new FuncCallExpr(NULL, $1, NULL, $4); }
	      | IDENTIFIER '(' arglist ')'	{ $$ = new FuncCallExpr(NULL, $1, $3, NULL);  }
	      | IDENTIFIER '(' arglist ')' block	{ $$ = new FuncCallExpr(NULL, $1, $3, $5); }
	      | FUNCTION_CALL ARG_BRACKET ')'	{ $$ = new FuncCallExpr(NULL, $1, NULL, NULL); }
	      | FUNCTION_CALL ARG_BRACKET ')' block	{ $$ = new FuncCallExpr(NULL, $1, NULL, $4); }
	      | FUNCTION_CALL ARG_BRACKET arglist ')'	{ $$ = new FuncCallExpr(NULL, $1, $3, NULL); }
	      | FUNCTION_CALL ARG_BRACKET arglist ')' block	{ $$ = new FuncCallExpr(NULL, $1, $3, $5); }
;

/* arglist is one or more, in line with funccall. */
arglist:	compiled_expr			{ $$ = new ArgListExpr($1); }
	      |	arglist ',' compiled_expr	{ $$ = new ArgListExpr($1, $3); }
;

/* deflist is for function/block argument definitions. one or more. */
deflist:	IDENTIFIER		{ $$ = new DefListExpr($1); }
	      | deflist ',' IDENTIFIER	{ $$ = new DefListExpr($1, $3); }
;

block:		
	      DO block_arguments block_content END	{ if ($3) $3->args = $2; $$ = $3; }
	      |	'{' block_arguments block_content '}'	{ if ($3) $3->args = $2; $$ = $3; }
;

block_content:	{ enter_block(); } block_line { exit_block(); } { $$ = $2; }
;

block_line:	/* empty */		{ $$ = new BlockExpr(); }
	      |	block_line { enter_block_line(); } line { exit_block_line(); } { if ($3) $1->expressions.push_back($3); $$ = $1; }
;

block_arguments:
	      BLOCK_ARGUMENT_START
	      block_argument_contents
	      BLOCK_ARGUMENT_END	{ $$ = $2; }
;

block_argument_contents:
		/* empty */	{ $$ = NULL; }
	      | deflist		{ $$ = $1; }
;

literal:	STRING_LITERAL	{ $$ = $1; }
	      |	INTEGER_LITERAL	{ $$ = $1; }
	      | FLOATING_LITERAL	{ $$ = $1; }
	      | BOOLEAN_LITERAL	{ $$ = $1; }
;

