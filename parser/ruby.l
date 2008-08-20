%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option yywrap

%x dbstr context_finish starting_block_args arglist_begins
%s block_args

%{
  std::string string_buf; bool doing_symbol = false;
%}

%%

\"			{
		  string_buf = "";
		  doing_symbol = false;
		  BEGIN(dbstr);
		}
<dbstr>\"		{ /* all done */
		  BEGIN(INITIAL);
		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new StringLiteralExpr(string_buf);
		  return STRING_LITERAL;
		}
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

<dbstr>[^\\\n\"]+		{
		  char *yptr = yytext;
		  while (*yptr)
		    string_buf += *yptr++;
		}

def		{ return DEF; }

true		{ yylval.boolean_literal = new BooleanLiteralExpr(true); return BOOLEAN_LITERAL; }
false		{ yylval.boolean_literal = new BooleanLiteralExpr(false); return BOOLEAN_LITERAL; }
nil		{ yylval.nil_literal = new NilLiteralExpr(); return NIL_LITERAL; }

do[\t ]*\|	{ unput('|'); BEGIN(starting_block_args); return DO; }
\{[\t ]*\|	{ unput('|'); BEGIN(starting_block_args); return '{'; }


do			{
		  /* no args (by precendence). inject empty arglist into stream,
		   * as the parser always sees the args coming. */
		  unput('|'); unput('|');
		  BEGIN(starting_block_args);
		  return DO;
		}
end			{
		  if (in_context() && context_lines == context_depths) {
		    IF_DEBUG printf("Going to block_finish (end).");
		    BEGIN(context_finish);
		    yyless(0);
		    return CONTEXT_FINISH;
		  }
		  return END;
		}

\{		{ /* see "do" */ unput('|'); unput('|'); BEGIN(starting_block_args); return '{'; }
\}			{
		  if (in_context() && context_lines == context_depths) {
		    IF_DEBUG printf("Going to context_finish (}).");
		    BEGIN(context_finish);
		    yyless(0);	// whoops. yyless returns all BUT first `n' chars.
		    return CONTEXT_FINISH;
		  }
		  return '}';
		}

<starting_block_args>\|	{ BEGIN(block_args); return BLOCK_ARGUMENT_START; }
<block_args>\|	{ BEGIN(INITIAL); return BLOCK_ARGUMENT_END; }


<context_finish>end	{
		  IF_DEBUG printf("Hear `end', returning to initial.\n");
		  BEGIN(INITIAL);
		  return END;
		}
<context_finish>\}	{
		  IF_DEBUG printf("Hear }, returning to initial.\n");
		  BEGIN(INITIAL);
		  return '}';
		}

-?[0-9]+\.[0-9]+	{ yylval.floating_literal = new FloatingLiteralExpr(atof(yytext)); return FLOATING_LITERAL; }

-?[0-9]+			{ yylval.integer_literal = new IntegerLiteralExpr(atoi(yytext)); return INTEGER_LITERAL; }




[a-zA-Z][a-zA-Z0-9_]*[!?]\(?	{
			  std::string s = yytext;
			  if (s[s.length() - 1] == '(') {
			    s = s.substr(0, s.length() - 1);
			    BEGIN(arglist_begins);
			    unput('(');
			  }
			  yylval.identifier = new IdentifierExpr(s);
			  return FUNCTION_CALL;
			}
[a-zA-Z][a-zA-Z0-9_]*\(		{
			  std::string s = yytext;
			  s = s.substr(0, s.length() - 1);
			  yylval.identifier = new IdentifierExpr(s);
			  BEGIN(arglist_begins);
			  unput('(');
			  return FUNCTION_CALL;
			}

<arglist_begins>\(		{
			  BEGIN(INITIAL);
			  return ARG_BRACKET;
			}

[a-zA-Z][a-zA-Z0-9_]*	{ yylval.identifier = new IdentifierExpr(std::string(yytext)); return IDENTIFIER; }

:[a-zA-Z][a-zA-Z0-9_]*[!?]?	{ yylval.symbol = new SymbolExpr(std::string(yytext + 1)); return SYMBOL; }
:\"			{
		  string_buf = "";
		  doing_symbol = true;
		  BEGIN(dbstr);
		}

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

