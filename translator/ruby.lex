%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option noyywrap

%x dbstr block_finish starting_block_args
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
<dbstr>\"			{ /* all done */
		  BEGIN(INITIAL);
		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new LiteralTypedExpr<std::string>(string_buf);
		  return STRING_LITERAL;
		}
<dbstr>\n		string_buf += '\n';
<dbstr>\\n	string_buf += '\n';
<dbstr>\\t	string_buf += '\t';
<dbstr>\\r	string_buf += '\r';
<dbstr>\\b	string_buf += '\b';
<dbstr>\\f	string_buf += '\f';

<dbstr>\\(.|\n)	string_buf += yytext[1];

<dbstr>[^\\\n\"]+		{
		  char *yptr = yytext;
		  while (*yptr)
		    string_buf += *yptr++;
		}

true		{ yylval.boolean_literal = new LiteralTypedExpr<bool>(true); return BOOLEAN_LITERAL; }
false		{ yylval.boolean_literal = new LiteralTypedExpr<bool>(false); return BOOLEAN_LITERAL; }

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
		  if (in_block() && block_lines == block_depths) {
		    IF_DEBUG printf("Going to block_finish (end).");
		    BEGIN(block_finish);
		    yyless(0);
		    return BLOCK_FINISH;
		  }
		  return END;
		}

\{		{ /* see "do" */ unput('|'); unput('|'); BEGIN(starting_block_args); return '{'; }
\}			{
		  if (in_block() && block_lines == block_depths) {
		    IF_DEBUG printf("Going to block_finish (}).");
		    BEGIN(block_finish);
		    yyless(0);	// whoops. yyless returns all BUT first `n' chars.
		    return BLOCK_FINISH;
		  }
		  return '}';
		}

<starting_block_args>\|	{ BEGIN(block_args); return BLOCK_ARGUMENT_START; }
<block_args>\|	{ BEGIN(INITIAL); return BLOCK_ARGUMENT_END; }


<block_finish>end	{
		  IF_DEBUG printf("Hear `end', returning to initial.\n");
		  BEGIN(INITIAL);
		  return END;
		}
<block_finish>\}	{
		  IF_DEBUG printf("Hear }, returning to initial.\n");
		  BEGIN(INITIAL);
		  return '}';
		}

-?[0-9]+\.[0-9]+	{ yylval.floating_literal = new LiteralTypedExpr<double>(atof(yytext)); return FLOATING_LITERAL; }

-?[0-9]+			{ yylval.integer_literal = new LiteralTypedExpr<int>(atoi(yytext)); return INTEGER_LITERAL; }

[a-zA-Z][a-zA-Z0-9_]*[!?]	{ yylval.identifier = new IdentifierExpr(std::string(yytext)); return FUNCTION_CALL; }
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

