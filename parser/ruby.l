%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>

  YY_BUFFER_STATE current_buffer = NULL;
%}

%option yywrap stack

%x context_finish starting_block_args arglist_begins index_begins
%x next_literally
%s could_be_starting_block_args
%s block_args

%x dbstr sstr regex interpolate_begin backtickstr here_document

%{
  std::string string_buf, here_doc;
  bool doing_symbol = false;
  bool hyphen_here = false;

  typedef struct {
    const char *identifier;
    int literal;
  } keyword_t;

  /* Only bother with alpha/num here. */
  const keyword_t keyword_listing[] = {
    { "do", DO },
    { "begin", BEGIN_SECTION }, { "ensure", ENSURE },
    { "rescue", RESCUE },
    { "end", END },

    { "def", DEF },
    { "if", IF }, { "then", THEN }, { "else", ELSE }, { "elsif", ELSIF }, { "unless", UNLESS },
    { "yield", YIELD },

    { "and", LOGICAL_AND_WORD }, { "or", LOGICAL_OR_WORD },
    { "not", LOGICAL_NOT_WORD },

    { "while", WHILE },

    { "class", CLASS }, { "module", MODULE },

    { "", 0 }
  };

  int keyword_search(const char *_s) {
    const keyword_t *s = keyword_listing;
    while (s->literal) {
      if (strcmp(_s, s->identifier) == 0)
	return s->literal;
      ++s;
    }
    return 0;
  }
%}


%%
    /* TODO: DRY this, maybe with a stack which we can push onto when we start a string/backtick/etc. */
    if (string_lit_rs) {
      string_lit_rs = false;
      string_buf = ""; doing_symbol = false; BEGIN(dbstr);
    } else if (backtick_lit_rs) {
      backtick_lit_rs = false;
      string_buf = ""; BEGIN(backtickstr);
    } else if (regex_lexing) {
      regex_lexing = false;
      string_buf = "";
      doing_symbol = false;
      yy_push_state(regex);
    }

#[^\n]*		{ }


<regex>\/		{
		  yy_pop_state();
		  yylval.regex_literal = new StringLiteralExpr(string_buf);
		  return REGEX;
		}

<regex>[^\\/]+	{ string_buf += yytext; }
<regex>\\\/	{ string_buf += '/'; }
<regex>\\[^/]	{ string_buf += yytext; /* This should only add \ for things which need it, like `.', etc. */ }

\"			{
		  string_buf = "";
		  doing_symbol = false;
		  yy_push_state(dbstr);
		}
<dbstr>\"		{ /* all done */
		  yy_pop_state();
		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return STRING_LITERAL;
		}
<dbstr>#/\{		{ /* string interpolation */
		  BEGIN(interpolate_begin);

		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return STRING_LITERAL;
		}

<interpolate_begin>\{	{ BEGIN(INITIAL); return INTERPOLATION_START; }

<dbstr>\n	string_buf += '\n';
<dbstr>\\a	string_buf += '\a';
<dbstr>\\b	string_buf += '\b';
<dbstr>\\t	string_buf += '\t';
<dbstr>\\n	string_buf += '\n';
<dbstr>\\v	string_buf += '\v';
<dbstr>\\f	string_buf += '\f';
<dbstr>\\r	string_buf += '\r';

<dbstr>\\x[0-9a-fA-F]{1,2}	string_buf += (char)strtol(yytext + 2, NULL, 16);
<dbstr>\\[0-7]{1,3}		string_buf += (char)strtol(yytext + 1, NULL, 8);

<dbstr>\\(.|\n)	string_buf += yytext[1];

  /* we also omit # in this next line so the entirety of `#{' can make it to the
   * rule above to start string interpolation. we also have a rule just for `#',
   * but the interpolation rule will be picked due to its being longer. */

<dbstr>[^\\\n\"#]+	string_buf += yytext;
<dbstr>#		string_buf += yytext[0];

'			{
		  string_buf = "";
		  doing_symbol = false;
		  yy_push_state(sstr);
		}

<sstr>'			{
		  yy_pop_state();
		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return STRING_LITERAL;
		}
<sstr>\n		string_buf += '\n';
<sstr>\\('|\\)		string_buf += yytext[1];
<sstr>\\[^'\\]		string_buf += yytext;
<sstr>[^\\\n\']+	string_buf += yytext;

  /* ` backtick */
`			{
		  string_buf = "";
		  yy_push_state(backtickstr);
		}
    /* The below is almost entirely copied from dbstr.
     * Please DRY this !! XXX */

<backtickstr>#/\{		{
		  BEGIN(interpolate_begin);
		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return BACKTICK_LITERAL;
		}

<backtickstr>\n	string_buf += '\n';
<backtickstr>\\a	string_buf += '\a';
<backtickstr>\\b	string_buf += '\b';
<backtickstr>\\t	string_buf += '\t';
<backtickstr>\\n	string_buf += '\n';
<backtickstr>\\v	string_buf += '\v';
<backtickstr>\\f	string_buf += '\f';
<backtickstr>\\r	string_buf += '\r';
<backtickstr>\\x[0-9a-fA-F]{1,2}	string_buf += (char)strtol(yytext + 2, NULL, 16);
<backtickstr>\\[0-7]{1,3}		string_buf += (char)strtol(yytext + 1, NULL, 8);
<backtickstr>\\(.|\n)	string_buf += yytext[1];
<backtickstr>[^\\\n`#]+	string_buf += yytext;
<backtickstr>#		string_buf += yytext[0];
<backtickstr>`		{
		  yy_pop_state();
		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return BACKTICK_LITERAL;
		}

    /* While these would be nice to have as keywords as defined below,
     * they require some more logic. Note that `yield' is defined below,
     * but here's `yield('. */

true		{ yylval.boolean_literal = new BooleanLiteralExpr(true); return BOOLEAN_LITERAL; }
false		{ yylval.boolean_literal = new BooleanLiteralExpr(false); return BOOLEAN_LITERAL; }
nil		{ yylval.nil_literal = new NilLiteralExpr(); return NIL_LITERAL; }
yield/\(		{ return YIELD; }


    /* Blocks. */

do[\t ]*/\|	{ yy_push_state(starting_block_args); return DO; }
\{[\t ]*/\|	{ yy_push_state(starting_block_args); return '{'; }



do[\t {]?			{
		  /* no args (if args are given, the args version will match - it's longer, so flex will pick it).
		   * inject empty arglist into stream, as the parser always sees args coming. */
		  yyless(2);	// return everything except `do'
		  unput('|'); unput('|');
		  yy_push_state(starting_block_args);
		  return DO;
		}
end			{
		  if (in_context() && context_lines == context_depths) {
		    IF_DEBUG printf("Going to block_finish (end).");
		    yy_push_state(context_finish);
		    yyless(0);
		    return CONTEXT_FINISH;
		  }
		  return END;
		}

	  /* These first two are deliberately BEGIN: we want to replace the current state, not go down another level. */

<starting_block_args>\|			{ BEGIN(block_args); return BLOCK_ARGUMENT_START; }
<could_be_starting_block_args>\|	{ BEGIN(block_args); return BLOCK_ARGUMENT_START; }
<could_be_starting_block_args>[^ \t]	{ yy_pop_state(); yyless(0); }
<block_args>\|				{ yy_pop_state(); return BLOCK_ARGUMENT_END; }

\{			{ /* see "do" */
		  yy_push_state(could_be_starting_block_args);
		  return '{';
		}
\}			{
		  if (in_context() && context_lines == context_depths) {
		    IF_DEBUG printf("Going to context_finish (}).");
		    yy_push_state(context_finish);
		    yyless(0);	// whoops. yyless returns all BUT first `n' chars.
		    return CONTEXT_FINISH;
		  }
		  return '}';
		}


<context_finish>end	{
		  IF_DEBUG printf("Hear `end', returning to initial.\n");
		  yy_pop_state();
		  return END;
		}
<context_finish>\}	{
		  IF_DEBUG printf("Hear }, returning to initial.\n");
		  yy_pop_state();
		  return '}';
		}

\$/[":]		{ yy_push_state(next_literally); return '$'; }
<next_literally>.	{ yy_pop_state(); return yytext[0]; }

\<=>		{ return SPACESHIP; }
==		{ return EQ; }
===		{ return EQQ; }
!=		{ return NEQ; }
\<=		{ return LE; }
>=		{ return GE; }

=>		{ return ASSOC; }

\+=		{ return ADD_ASSIGN; }
-=		{ return SUB_ASSIGN; }
\*=		{ return MUL_ASSIGN; }
\/=		{ return DIV_ASSIGN; }

::		{ return SCOPE; }
\<\<		{ return LEFT_SHIFT; }
\>\>		{ return RIGHT_SHIFT; }
\.\.\.		{ return RANGE_THREE; }
\.\.		{ return RANGE_TWO; }

&&		{ return LOGICAL_AND; }
\|\|		{ return LOGICAL_OR; }
\*\*		{ return EXP; }

\<\<[a-zA-Z_0-9]+$	{ yy_push_state(here_document); here_doc = yytext + 2; string_buf = ""; hyphen_here = false; }
\<\<-[a-zA-Z_0-9]+$	{ yy_push_state(here_document); here_doc = yytext + 3; string_buf = ""; hyphen_here = true; }
<here_document>^.+$		{
			  const char *yytext_check = yytext;
			  if (hyphen_here) 
			    while (*yytext_check == ' ' || *yytext_check == '\t')
			      ++yytext_check;

			  if (yytext_check == here_doc) {
			    yy_pop_state();
			    yylval.string_literal = new StringLiteralExpr(string_buf);
			    return STRING_LITERAL;
			  } else {
			    string_buf += yytext;
			    string_buf += '\n';
			  }
			}

		/* There's gotta be a more succint way to do these. */

  /*
[ \t]-?[0-9][0-9_]*[0-9]\.[0-9][0-9_]*[0-9]	{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }
[ \t]-?[0-9]\.[0-9][0-9_]*[0-9]		{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }
[ \t]-?[0-9][0-9_]*[0-9]\.[0-9]		{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }
[ \t]-?[0-9]\.[0-9]				{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }

[ \t]-?[0-9][0-9_]*[0-9]			{ yylval.integer_literal = new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); return INTEGER_LITERAL; }
[ \t]-?[0-9]					{ yylval.integer_literal = new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); return INTEGER_LITERAL; }
  */

  /* This is one way by which we can get seemingly correct results for
   * `n-3', `n -3', `n - 3', etc. But here, it's horribly ugly and
   * probably not done right. TODO: The above code is for allowing
   * underscores in number literals. We need to recombine these rules
   * (i.e. refactor both above and below somehow, then combine) without
   * things going everywhere. */

[0-9]+\.[0-9]+			{ yylval.floating_literal = 	new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); 	return FLOATING_LITERAL; }
[0-9]+				{ yylval.integer_literal = 	new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); 	return INTEGER_LITERAL; }
^-[0-9]+\.[0-9]+		{ yylval.floating_literal = 	new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); 	return FLOATING_LITERAL; }
^-[0-9]+			{ yylval.integer_literal = 	new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); 	return INTEGER_LITERAL; }
[ \t]-[0-9]+\.[0-9]+		{ yylval.floating_literal = 	new FloatingLiteralExpr(atof(filter_underscores(yytext + 1).c_str())); 	return FLOATING_LITERAL; }
[ \t]-[0-9]+			{ yylval.integer_literal = 	new IntegerLiteralExpr(atoi(filter_underscores(yytext + 1).c_str())); 	return INTEGER_LITERAL; }

      /* XXX: should we include keyword checks here? */

[a-zA-Z][a-zA-Z0-9_]*[!?]\(?	{
			  std::string s = yytext;
			  if (s[s.length() - 1] == '(') {
			    s = s.substr(0, s.length() - 1);
			    yy_push_state(arglist_begins);
			    unput('(');
			  }
			  yylval.identifier = new IdentifierExpr(s);
			  return FUNCTION_CALL;
			}
[a-zA-Z][a-zA-Z0-9_]*/\(		{
			  std::string s = yytext;
			  yylval.identifier = new IdentifierExpr(s);
			  yy_push_state(arglist_begins);
			  return FUNCTION_CALL;

			}

[a-zA-Z][a-zA-Z0-9_]*/\[	{
			  std::string s = yytext;
			  yylval.identifier = new IdentifierExpr(s);
			  yy_push_state(index_begins);
			  return INDEX_CALL;
			}
<index_begins>\[		{
			  yy_pop_state();
			  return INDEX_BRACKET;
			}

<arglist_begins>\(		{
			  yy_pop_state();
			  return ARG_BRACKET;
			}

[a-zA-Z_][a-zA-Z0-9_]*[!?]?[ \t]*\{	{ 
			std::string s = yytext;
			s = s.substr(0, s.length() - 1);
			while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')
			  s = s.substr(0, s.length() - 1);

			int kwm;
			if ((kwm = keyword_search(s.c_str())) != 0) {
			  std::cerr << "Returning " << s << " by backdoor" << std::endl; // XXX XXX
			  unput('{');
			  return kwm;
			}

			yylval.identifier = new IdentifierExpr(s);
			unput('{'); unput(')'); unput('(');
			yy_push_state(arglist_begins);
			return FUNCTION_CALL;
		      }

[a-zA-Z_][a-zA-Z0-9_]*			{
		  /* Return the proper token if this is a keyword. */
		  int kwm;
		  if ((kwm = keyword_search(yytext)) != 0)
		    return kwm;

		  yylval.identifier = new IdentifierExpr(std::string(yytext));
		  return IDENTIFIER;
		}

:[a-zA-Z_][a-zA-Z0-9_]*[!?]?	{ yylval.symbol = new SymbolExpr(std::string(yytext + 1)); return SYMBOL; }
:\"			{
		  string_buf = "";
		  doing_symbol = true;
		  yy_push_state(dbstr);
		}
:'			{
		  string_buf = "";
		  doing_symbol = true;
		  yy_push_state(sstr);
		}

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

%%

void destroy_current_buffer() {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

void destroy_old_buffer() {
  if (current_buffer) {
    yy_delete_buffer(current_buffer);
    current_buffer = NULL;
  }
}

void assign_new_buffer() {
  current_buffer = YY_CURRENT_BUFFER;
}

