
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
%token ARG_BRACKET INDEX_BRACKET
%token NL DO END CONTEXT_FINISH
%token END_OF_FILE 0 "$end"
%token <symbol> SYMBOL

%token DEF IF ELSE ELSIF UNLESS THEN
%token BEGIN_SECTION
%token RESCUE ENSURE
%token WHILE
%token CLASS MODULE

%token <string_literal> STRING_LITERAL BACKTICK_LITERAL
%token <integer_literal> INTEGER_LITERAL
%token <floating_literal> FLOATING_LITERAL
%token <boolean_literal> BOOLEAN_LITERAL
%token <nil_literal> NIL_LITERAL
%token <yield> YIELD

%type <expr> line expr compiled_expr expr_nob
%type <block> block opt_block
%type <exprlist> exprlist opt_exprlist hashlist
%type <idlist> idlist opt_idlist
%type <deflist> deflist block_arguments opt_block_arguments block_argument_contents
%type <literal> literal
%type <classdef> classdef
%type <moduledef> moduledef
%type <funccall> funccall
%type <funcdeflist> funcdef_args funcdeflist
%type <funcdef> funcdef
%type <funcdeflistentity> funcdeflistentity
%type <conditional> conditional
%type <while_loop> while_loop
%type <begin_section> begin_section
%type <rescue> opt_rescue
%type <identifier> opt_rescue_target
%type <interpolated_expr> interpolated_string interpolated_backtick_string

%type <identifier> function_name function_name_identifier sigiled_variable global_variable

%type <procedure> sub_content sub_line if_otherwise unless_otherwise
%type <block> opt_rescue_else opt_rescue_ensure

%nonassoc INTERPOLATION_START

/* Note this implies '=', RANGE, etc. get applied *after* everything else is in groups delimited by them. */
%nonassoc TERN '?' ':'
%left LOGICAL_AND_WORD LOGICAL_OR_WORD
%left LOGICAL_NOT_WORD
%right '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%nonassoc RANGE_TWO RANGE_THREE
%left LOGICAL_OR
%left LOGICAL_AND
%nonassoc EQ NEQ EQQ SPACESHIP EQREX NEQREX
%left '<' LE GE '>'
%left '|' '^'
%left '&'
%left LEFT_SHIFT RIGHT_SHIFT
%left '+' '-'
%left '*' '/' '%'
%left NEG '!' '~'
%right EXP

%left SCOPE

%nonassoc <identifier> IDENTIFIER FUNCTION_CALL INDEX_CALL
%nonassoc '.' ASSOC
%left '(' ')'
%left DO END
%left '{' '}'
%left '[' ']'

// Collected these by seeing what things are allocated with `new' in ruby.l and finding the symbols they return.
%destructor { delete $$; } IDENTIFIER FUNCTION_CALL INTEGER_LITERAL STRING_LITERAL BACKTICK_LITERAL FLOATING_LITERAL BOOLEAN_LITERAL NIL_LITERAL SYMBOL INDEX_CALL /*
    */ expr funccall exprlist funcdeflist funcdeflistentity deflist idlist hashlist block funcdef conditional if_otherwise while_loop sub_line interpolated_string /*
    */ interpolated_backtick_string begin_section opt_rescue opt_rescue_else opt_rescue_ensure classdef moduledef

%%

input:  	/* empty */
	      | input line { if ($2) program->add_expression($2); }
;

line_separator:	NL
	      |	';'
;

line: 		line_separator		{ $$ = NULL; }
	      | expr line_separator	{ $$ = $1; }
	      | CONTEXT_FINISH 		{ $$ = NULL; IF_DEBUG printf("CONTEXT_FINISH\n"); }
	      | expr CONTEXT_FINISH	{ $$ = $1; IF_DEBUG printf("CONTEXT_FINISH\n"); }
	      | expr END_OF_FILE 	{ $$ = $1; }
;

expr:		expr_nob				{ $$ = $1; }
	      | '(' expr ')'				{ $$ = $2; }
;

expr_nob:      	YIELD					{ $$ = new YieldExpr(NULL); }
	      | YIELD exprlist				{ $$ = new YieldExpr($2); }
	      | YIELD '(' opt_exprlist ')'		{ $$ = new YieldExpr($3); }
	      | classdef				{ $$ = $1; }
	      | moduledef				{ $$ = $1; }
	      |	funccall 				{ $$ = $1; }
	      | funcdef					{ $$ = $1; }
	      | sigiled_variable			{ $$ = $1; }
	      | sigiled_variable '=' expr		{ $$ = new AssignmentExpr($1, $3); }
	      |	IDENTIFIER				{ $$ = $1; /* Do not amalgamate this and `IDENTIFIER block' with opt_block - need to be separate for precedence, etc. */ }
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
	      | SCOPE IDENTIFIER			{ $$ = new ScopeExpr(NULL, $2); }
	      | expr SCOPE IDENTIFIER			{ $$ = new ScopeExpr($1, $3); }
	      |	SYMBOL					{ $$ = $1; }
	      | literal					{ $$ = $1; }
	      | interpolated_string			{ $$ = $1; }
	      | interpolated_backtick_string		{ $$ = new FuncCallExpr(NULL, new IdentifierExpr("`"), new ExprList($1), NULL); }
	      | BACKTICK_LITERAL			{ $$ = new FuncCallExpr(NULL, new IdentifierExpr("`"), new ExprList($1), NULL); }
	      | INDEX_CALL INDEX_BRACKET exprlist ']'		{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]"), $3, NULL); }
	      | INDEX_CALL INDEX_BRACKET exprlist ']' '=' expr	{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]="), new ExprList($3, $6), NULL); }
	      | expr '[' exprlist ']'			{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]"), $3, NULL); }
	      | expr '[' exprlist ']' '=' expr		{ $$ = new FuncCallExpr($1, new IdentifierExpr("[]="), new ExprList($3, $6), NULL); }
	      | expr '.' funccall 			{ $3->target = $1; $$ = $3; }
	      | expr '.' function_name opt_block  	{ $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '.' function_name '=' expr	  	{ $3->id += '='; $$ = new FuncCallExpr($1, $3, new ExprList($5), NULL); }
	      | expr '.' FUNCTION_CALL opt_block	{ $$ = new FuncCallExpr($1, $3, NULL, $4); }
	      | expr '+' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("+"), new ExprList($3), NULL); }
	      | expr '-' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("-"), new ExprList($3), NULL); }
	      | expr '*' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("*"), new ExprList($3), NULL); }
	      | expr '/' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("/"), new ExprList($3), NULL); }
	      | '-' expr %prec NEG			{ $$ = new FuncCallExpr($2, new IdentifierExpr("-@"), NULL, NULL); }
	      | '~' expr %prec NEG			{ $$ = new FuncCallExpr($2, new IdentifierExpr("~"), NULL, NULL); }
	      | '!' expr %prec NEG			{ $$ = new FalsityExpr($2); }
	      | LOGICAL_NOT_WORD expr			{ $$ = new FalsityExpr($2); }
	      | expr '^' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("^"), new ExprList($3), NULL); }
	      | expr SPACESHIP expr			{ $$ = new FuncCallExpr($1, new IdentifierExpr("<=>"), new ExprList($3), NULL); }
	      | expr EQ expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("=="), new ExprList($3), NULL); }
	      | expr EQQ expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("==="), new ExprList($3), NULL); }
	      | expr NEQ expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("!="), new ExprList($3), NULL); }
	      | expr '<' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("<"), new ExprList($3), NULL); }
	      | expr '>' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr(">"), new ExprList($3), NULL); }
	      | expr LE expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("<="), new ExprList($3), NULL); }
	      | expr GE expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr(">="), new ExprList($3), NULL); }
	      | expr EXP expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("**"), new ExprList($3), NULL); }
	      | expr '&' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("&"), new ExprList($3), NULL); }
	      | expr '|' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("|"), new ExprList($3), NULL); }
	      | expr '%' expr				{ $$ = new FuncCallExpr($1, new IdentifierExpr("%"), new ExprList($3), NULL); }
	      | expr LOGICAL_AND expr			{ $$ = new FuncCallExpr($1, new IdentifierExpr("&&"), new ExprList($3), NULL); }
	      | expr LOGICAL_OR expr			{ $$ = new FuncCallExpr($1, new IdentifierExpr("||"), new ExprList($3), NULL); }
	      | expr LOGICAL_AND_WORD expr		{ $$ = new FuncCallExpr($1, new IdentifierExpr("&&"), new ExprList($3), NULL); }
	      | expr LOGICAL_OR_WORD expr		{ $$ = new FuncCallExpr($1, new IdentifierExpr("||"), new ExprList($3), NULL); }
	      | expr RANGE_TWO expr			{ $$ = new FuncCallExpr(new IdentifierExpr("Range"), new IdentifierExpr("new"), new ExprList($1, $3), NULL); }
	      | expr RANGE_THREE expr			{ $$ = new FuncCallExpr(new IdentifierExpr("Range"), new IdentifierExpr("new"), new ExprList($1, $3, new BooleanLiteralExpr(true)), NULL); }
	      | expr LEFT_SHIFT expr			{ $$ = new FuncCallExpr($1, new IdentifierExpr("<<"), new ExprList($3), NULL); }
	      | expr RIGHT_SHIFT expr			{ $$ = new FuncCallExpr($1, new IdentifierExpr(">>"), new ExprList($3), NULL); }
	      | '[' opt_exprlist ']'			{ $$ = new FuncCallExpr(new IdentifierExpr("Array"), new IdentifierExpr("[]"), $2, NULL); }
	      | '{' hashlist CONTEXT_FINISH '}'		{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("[]"), $2, NULL); }
	      | '{' hashlist '}'			{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("[]"), $2, NULL); }
	      |	'{' CONTEXT_FINISH '}'			{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("new"), NULL, NULL); }
	      |	'{' '}'					{ $$ = new FuncCallExpr(new IdentifierExpr("Hash"), new IdentifierExpr("new"), NULL, NULL); }
	      | conditional				{ $$ = $1; }
	      |	expr '?' expr ':' expr %prec TERN	{ $$ = new ConditionalExpr($1, new Procedure($3), new Procedure($5)); }
	      | begin_section				{ $$ = $1; }
	      | while_loop				{ $$ = $1; }
;

function_name_identifier:
		IDENTIFIER		{ $$ = $1; }
	      |	CLASS			{ $$ = new IdentifierExpr("class"); }
	      | MODULE			{ $$ = new IdentifierExpr("module"); }
	      | DEF			{ $$ = new IdentifierExpr("def"); }
;

function_name:	function_name_identifier	{ $$ = $1; }
	      | '`'			{ $$ = new IdentifierExpr("`"); }
	      | '+'			{ $$ = new IdentifierExpr("+"); }
	      | '-'			{ $$ = new IdentifierExpr("-"); }
	      | '*'			{ $$ = new IdentifierExpr("*"); }
	      | '/'			{ $$ = new IdentifierExpr("/"); }
	      | '-' '@'			{ $$ = new IdentifierExpr("-@"); }
	      | '~'			{ $$ = new IdentifierExpr("~"); }
	      | '^'			{ $$ = new IdentifierExpr("^"); }
	      | SPACESHIP		{ $$ = new IdentifierExpr("<=>"); }
	      | EQ			{ $$ = new IdentifierExpr("=="); }
	      | EQQ			{ $$ = new IdentifierExpr("==="); }
	      | NEQ			{ $$ = new IdentifierExpr("!="); }
	      | '<'			{ $$ = new IdentifierExpr("<"); }
	      | '>'			{ $$ = new IdentifierExpr(">"); }
	      | LE			{ $$ = new IdentifierExpr("<="); }
	      | GE			{ $$ = new IdentifierExpr(">="); }
	      | EXP			{ $$ = new IdentifierExpr("**"); }
	      | '&'			{ $$ = new IdentifierExpr("&"); }
	      | '|'			{ $$ = new IdentifierExpr("|"); }
	      | '%'			{ $$ = new IdentifierExpr("%"); }
	      | LOGICAL_AND		{ $$ = new IdentifierExpr("&&"); }
	      | LOGICAL_OR		{ $$ = new IdentifierExpr("||"); }
	      | LEFT_SHIFT		{ $$ = new IdentifierExpr("<<"); }
	      | RIGHT_SHIFT		{ $$ = new IdentifierExpr("||"); }
;

sigiled_variable:
		'@' function_name_identifier		{ $2->id.insert(0, "@"); $$ = $2; }
	      | '@' '@' function_name_identifier	{ $3->id.insert(0, "@@"); $$ = $3; }
	      |	'$' function_name_identifier		{ $2->id.insert(0, "$"); $$ = $2; }
	      | '$' global_variable			{ $2->id.insert(0, "$"); $$ = $2; }
;

global_variable:
		':'	{ $$ = new IdentifierExpr(":"); }
	      |	'"'	{ $$ = new IdentifierExpr("\""); }
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

funcdeflist:	funcdeflistentity			{ $$ = new FuncDefList($1); }
	      |	funcdeflist ',' funcdeflistentity	{ $$ = $1; $$->add($3); if (!$$->valid_syntax()) { delete $$;  yyerror(program, "syntax error, invalid argument list"); YYERROR; } }
;

funcdeflistentity:
		IDENTIFIER				{ $$ = new FuncDefListEntity($1); }
	      |	IDENTIFIER '=' expr			{ $$ = new FuncDefListEntity($1, new Procedure($3)); }
	      |	'*' IDENTIFIER				{ $$ = new FuncDefListEntity($2, true); }
;

/* deflist is for block argument definitions. one or more. */
deflist:	IDENTIFIER		{ $$ = new DefListExpr($1); }
	      | deflist ',' IDENTIFIER	{ $$ = new DefListExpr($1, $3); }
;

idlist:		IDENTIFIER		{ $$ = new IdListExpr($1); }
	      |	idlist ',' IDENTIFIER	{ $$ = new IdListExpr($1, $3); }
;

opt_idlist:	/* empty */		{ $$ = NULL; }
	      | idlist			{ $$ = $1; }
;

hashlist:	compiled_expr ASSOC compiled_expr			{ $$ = new ExprList($1, $3); }
	      | hashlist ',' compiled_expr ASSOC compiled_expr		{ $$ = new ExprList($1, $3, $5); }
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

funcdef:	DEF FUNCTION_CALL funcdef_args line_separator sub_content END			{ $$ = new FuncDefExpr(NULL, $2, $3, $5); }
	      |	DEF function_name funcdef_args line_separator sub_content END 			{ $$ = new FuncDefExpr(NULL, $2, $3, $5); }
	      |	DEF function_name_identifier '=' funcdef_args line_separator sub_content END 	{ $2->id += '='; $$ = new FuncDefExpr(NULL, $2, $4, $6); }
;

funcdef_args:	/* empty */		{ $$ = NULL; }
	      | ARG_BRACKET ')'		{ $$ = NULL; }
	      | ARG_BRACKET funcdeflist ')'	{ $$ = $2; }
	      | funcdeflist			{ $$ = $1; }
;

conditional:	IF expr then_or_separator sub_content if_otherwise END 		{ $$ = new ConditionalExpr($2, $4, $5); }
	      |	UNLESS expr then_or_separator sub_content unless_otherwise END 	{ $$ = new ConditionalExpr(new FalsityExpr($2), $4, $5); }
;

if_otherwise:	/* empty */							{ $$ = NULL; }
	      | ELSE sub_content						{ $$ = $2; }
	      | ELSIF expr then_or_separator sub_content if_otherwise	{ $$ = new Procedure(); $$->expressions.push_back(new ConditionalExpr($2, $4, $5)); }
;

unless_otherwise:
		/* empty */		{ $$ = NULL; }
	      |	ELSE sub_content	{ $$ = $2; }
;

then_or_separator:
		line_separator
	      |	THEN
;

while_loop:	WHILE expr line_separator sub_content END		{ $$ = new WhileExpr($2, $4); }
;

sub_content:	{ enter_context(); } sub_line { exit_context(); } { $$ = $2; }
;

sub_line:	/* empty */	{ $$ = new Procedure(); }
	      |	sub_line	{ enter_context_line(); }
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

	      |	interpolated_string INTERPOLATION_START sub_content '}' {restart_string_literal();} STRING_LITERAL
		{ $$ = $1; $$->append($3); $$->append($6); }
;

interpolated_backtick_string:
		BACKTICK_LITERAL INTERPOLATION_START sub_content '}' {restart_backtick_literal();} BACKTICK_LITERAL
		{ $$ = new InterpolateExpr(); $$->append($1); $$->append($3); $$->append($6); }

	      |	interpolated_backtick_string INTERPOLATION_START sub_content '}' {restart_backtick_literal();} BACKTICK_LITERAL
		{ $$ = $1; $$->append($3); $$->append($6); }
;

begin_section:	BEGIN_SECTION sub_content opt_rescue opt_rescue_else opt_rescue_ensure END 	{ $$ = new BeginSectionExpr(new BlockExpr($2), $3, $4, $5); }
;

opt_rescue:
		/* empty */					{ $$ = NULL; }
	      |	RESCUE opt_idlist opt_rescue_target sub_content	{ $$ = new Rescue($2, $3, new BlockExpr($4)); }
;

opt_rescue_else:
		/* empty */					{ $$ = NULL; }
	      | ELSE sub_content				{ $$ = new BlockExpr($2); }
;

opt_rescue_ensure:
		/* empty */					{ $$ = NULL; }
	      | ENSURE sub_content				{ $$ = new BlockExpr($2); }
;

opt_rescue_target:
		/* empty */		{ $$ = NULL; }
	      |	ASSOC IDENTIFIER	{ $$ = $2; }
;

classdef:	CLASS IDENTIFIER sub_content END		{ $$ = new ClassDefExpr($2, NULL, new BlockExpr($3)); }
	      |	CLASS IDENTIFIER '<' IDENTIFIER sub_content END	{ $$ = new ClassDefExpr($2, $4, new BlockExpr($5)); }
;

moduledef:	MODULE IDENTIFIER sub_content END		{ $$ = new ModuleDefExpr($2, new BlockExpr($3)); }
;

