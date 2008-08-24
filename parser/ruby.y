
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
%token NL DO END CONTEXT_FINISH
%token END_OF_FILE 0 "end of file"
%token <symbol> SYMBOL

%token DEF IF ELSE ELSIF UNLESS
%token WHILE

%token <string_literal> STRING_LITERAL 
%token <integer_literal> INTEGER_LITERAL
%token <floating_literal> FLOATING_LITERAL
%token <boolean_literal> BOOLEAN_LITERAL
%token <nil_literal> NIL_LITERAL
%token <yield> YIELD

%type <expr> line expr compiled_expr
%type <block> block opt_block
%type <exprlist> exprlist opt_exprlist hashlist
%type <deflist> deflist block_arguments opt_block_arguments block_argument_contents funcdef_args
%type <literal> literal
%type <funccall> funccall
%type <funcdef> funcdef
%type <conditional> conditional
%type <while_loop> while_loop
%type <interpolated_string> interpolated_string

%type <procedure> sub_content sub_line otherwise

%nonassoc INTERPOLATION_START

/* Note this implies EQ/NEQ, and lastly '=' get applied *after* everything else
 * is collapsed - our wanted behaviour. */
%right '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN

%nonassoc EQ NEQ
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/'
%left NEG
%right EXP


%nonassoc <identifier> IDENTIFIER FUNCTION_CALL
%nonassoc '.' ASSOC
%left '(' ')'
%left DO END
%left '{' '}'
%left '[' ']'

%%

input:  	/* empty */
	      | input line { if ($2) program->add_expression($2); }
;

line: 		NL	{ $$ = NULL; }
	      | expr NL	{ $$ = $1; }
	      | ';' { $$ = NULL; }
	      | expr ';' { $$ = $1; }
	      | CONTEXT_FINISH { $$ = NULL; IF_DEBUG printf("CONTEXT_FINISH\n"); }
	      | expr CONTEXT_FINISH { $$ = $1; IF_DEBUG printf("CONTEXT_FINISH\n"); }
	      | expr END_OF_FILE { $$ = $1; }
;

expr:	      	YIELD					{ $$ = new YieldExpr(NULL); }
	      | YIELD exprlist				{ $$ = new YieldExpr($2); }
	      | YIELD '(' opt_exprlist ')'		{ $$ = new YieldExpr($3); }
	      |	funccall 				{ $$ = $1; }
	      | funcdef					{ $$ = $1; }
	      |	IDENTIFIER				{ $$ = $1; /* Do not amalgamate this and `IDENTIFIER block' - this needs to be separate for precedence, etc. */ }
	      |	IDENTIFIER block 			{ $$ = new FuncCallExpr(NULL, $1, NULL, $2); }
	      |	FUNCTION_CALL opt_block 		{ $$ = new FuncCallExpr(NULL, $1, NULL, $2); }
	      | IDENTIFIER '=' expr 			{ $$ = new AssignmentExpr($1, $3); }
	      | IDENTIFIER ADD_ASSIGN expr		{ $$ = new AssignmentExpr($1,
							    new FuncCallExpr(new IdentifierExpr(*$1) /* copy since we kill it */,
							    new IdentifierExpr("+"), new ExprList($3), NULL));
							}
	      | IDENTIFIER SUB_ASSIGN expr		{ $$ = new AssignmentExpr($1, new FuncCallExpr(new IdentifierExpr(*$1), new IdentifierExpr("-"), new ExprList($3), NULL)); }
	      | IDENTIFIER MUL_ASSIGN expr		{ $$ = new AssignmentExpr($1, new FuncCallExpr(new IdentifierExpr(*$1), new IdentifierExpr("*"), new ExprList($3), NULL)); }
	      | IDENTIFIER DIV_ASSIGN expr		{ $$ = new AssignmentExpr($1, new FuncCallExpr(new IdentifierExpr(*$1), new IdentifierExpr("/"), new ExprList($3), NULL)); }
	      |	SYMBOL					{ $$ = $1; }
	      | literal					{ $$ = $1; }
	      | interpolated_string			{ $$ = $1; }
	      | IDENTIFIER '[' exprlist ']'		{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]"), $3, NULL); }
	      | IDENTIFIER '[' exprlist ']' '=' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]="), new ExprList($3, $6), NULL); }
	      | expr '[' exprlist ']'			{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]"), $3, NULL); }
	      | expr '[' exprlist ']' '=' expr		{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]="), new ExprList($3, $6), NULL); }
	      | expr '.' funccall 			{ $3->target = $1; $$ = $3; }
	      | expr '.' IDENTIFIER opt_block  		{ $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '.' FUNCTION_CALL opt_block	{ $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '+' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("+"), new ExprList($3), NULL); }
	      | expr '-' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("-"), new ExprList($3), NULL); }
	      | expr '*' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("*"), new ExprList($3), NULL); }
	      | expr '/' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("/"), new ExprList($3), NULL); }
	      | '-' expr %prec NEG			{ $$ = new FuncCallExpr($2, new IdentifierExpr("-@"), NULL, NULL); }
	      | expr '^' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("^"), new ExprList($3), NULL); }
	      | expr EQ expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("=="), new ExprList($3), NULL); }
	      | expr NEQ expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("!="), new ExprList($3), NULL); }
	      | expr '<' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("<"), new ExprList($3), NULL); }
	      | expr '>' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr(">"), new ExprList($3), NULL); }
	      | expr LE expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("<="), new ExprList($3), NULL); }
	      | expr GE expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr(">="), new ExprList($3), NULL); }
	      | '(' expr ')'				{ $$ = $2; }
	      | '[' opt_exprlist ']'			{ $$ = new FuncCallExpr(new IdentifierExpr("Array"), new IdentifierExpr("[]"), $2, NULL); }
	      | '{' hashlist CONTEXT_FINISH '}'	{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("[]"), $2, NULL); }
	      | '{' hashlist '}'	{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("[]"), $2, NULL); }
	      |	'{' CONTEXT_FINISH '}'					{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("new"), NULL, NULL); }
	      |	'{' '}'					{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("new"), NULL, NULL); }
	      | conditional				{ $$ = $1; }
	      | while_loop				{ $$ = $1; }
;

compiled_expr:	expr
;

/* An explicit function call, i.e. that has specified no parameters,
 * or has some parameters. Any inferred function call (e.g. 'gets')
 * will be treated like an IDENTIFIER in `expr', and we work it out
 * later. */
funccall: 	IDENTIFIER exprlist opt_block				{ $$ = new FuncCallExpr(NULL, $1, $2, $3); }
	      |	FUNCTION_CALL exprlist opt_block			{ $$ = new FuncCallExpr(NULL, $1, $2, $3); }
	      | IDENTIFIER '(' ')' opt_block				{ $$ = new FuncCallExpr(NULL, $1, NULL, $4); }
	      | IDENTIFIER '(' exprlist ')' opt_block			{ $$ = new FuncCallExpr(NULL, $1, $3, $5); }
	      | FUNCTION_CALL ARG_BRACKET ')' opt_block			{ $$ = new FuncCallExpr(NULL, $1, NULL, $4); }
	      | FUNCTION_CALL ARG_BRACKET exprlist ')' opt_block	{ $$ = new FuncCallExpr(NULL, $1, $3, $5); }
;

/* exprlist is one or more, in line with funccall. */
exprlist:	compiled_expr			{ $$ = new ExprList($1); }
	      |	exprlist ',' compiled_expr	{ $$ = new ExprList($1, $3); }
;

opt_exprlist:	/* empty */			{ $$ = NULL; }
	      | exprlist			{ $$ = $1; }
;

/* deflist is for function/block argument definitions. one or more. */
deflist:	IDENTIFIER		{ $$ = new DefListExpr($1); }
	      | deflist ',' IDENTIFIER	{ $$ = new DefListExpr($1, $3); }
;

hashlist:	compiled_expr ASSOC compiled_expr		{ $$ = new ExprList($1, $3); }
	      | hashlist ',' compiled_expr ASSOC compiled_expr	{ $$ = new ExprList($1, $3, $5); }
	      | hashlist ',' NL compiled_expr ASSOC compiled_expr	{ $$ = new ExprList($1, $4, $6); }
;

block:		DO opt_block_arguments sub_content END				{ $$ = new BlockExpr($3); $$->take_deflist($2); }
	      |	'{' opt_block_arguments sub_content '}'				{ $$ = new BlockExpr($3); $$->take_deflist($2); }
;

opt_block:	/* empty */	{ $$ = NULL; }
	      | block		{ $$ = $1; }
;

block_arguments:
	      BLOCK_ARGUMENT_START block_argument_contents BLOCK_ARGUMENT_END	{ $$ = $2; }
;

opt_block_arguments:
		/* empty */	{ $$ = NULL; }
	      |	block_arguments	{ $$ = $1; }
;

block_argument_contents:
		/* empty */	{ $$ = NULL; }
	      | deflist		{ $$ = $1; }
;

funcdef:	DEF FUNCTION_CALL funcdef_args sub_content END 	{ $$ = new FuncDefExpr(NULL, $2, $3, $4); }
	      |	DEF IDENTIFIER funcdef_args ';' sub_content END { $$ = new FuncDefExpr(NULL, $2, $3, $5); }
	      |	DEF IDENTIFIER funcdef_args NL sub_content END 	{ $$ = new FuncDefExpr(NULL, $2, $3, $5); }
;

funcdef_args:	/* empty */		{ $$ = NULL; }
	      | ARG_BRACKET ')'		{ $$ = NULL; }
	      | ARG_BRACKET deflist ')'	{ $$ = $2; }
	      | deflist			{ $$ = $1; }
;

conditional:	IF expr ';' sub_content otherwise END 	{ $$ = new ConditionalExpr($2, $4, $5); }
	      |	IF expr NL sub_content otherwise END	{ $$ = new ConditionalExpr($2, $4, $5); }
;

otherwise:	/* empty */				{ $$ = NULL; }
	      | ELSE sub_content			{ $$ = $2; }
	      | ELSIF expr ';' sub_content otherwise	{ $$ = new Procedure(); $$->expressions.push_back(new ConditionalExpr($2, $4, $5)); }
	      | ELSIF expr NL sub_content otherwise	{ $$ = new Procedure(); $$->expressions.push_back(new ConditionalExpr($2, $4, $5)); }
;

while_loop:	WHILE expr ';' sub_content END		{ $$ = new WhileExpr($2, $4); }
	      | WHILE expr NL sub_content END		{ $$ = new WhileExpr($2, $4); }
;

sub_content:	{ enter_context(); } sub_line { exit_context(); } { $$ = $2; }
;

sub_line:	/* empty */		{ $$ = new Procedure(); }
	      |	sub_line
				{ enter_context_line(); }
				line
				{ exit_context_line(); }
				{ if ($3) $1->expressions.push_back($3); $$ = $1; }
;

literal:	STRING_LITERAL		{ $$ = $1; }
	      |	INTEGER_LITERAL		{ $$ = $1; }
	      | FLOATING_LITERAL	{ $$ = $1; }
	      | BOOLEAN_LITERAL		{ $$ = $1; }
	      | NIL_LITERAL		{ $$ = $1; }
;

interpolated_string:
		STRING_LITERAL INTERPOLATION_START sub_content '}' {restart_string_literal();} STRING_LITERAL
		{ $$ = new InterpolateExpr(); $$->append($1); $$->append($3); $$->append($6); }
;
