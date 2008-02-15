%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option noyywrap

%x dbstr block_finish

%{
  std::string string_buf; bool doing_symbol;
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

do		{ return DO; }
end			{
		  if (in_block()) {
		  }
		  return END;
		}

\{		{ return '{'; }
\}			{
		  if (in_block() && block_lines == block_depths) {
		    IF_DEBUG printf("Going to block_finish.");
		    BEGIN(block_finish);
		    yyless(0);	// whoops. yyless returns all BUT first `n' chars.
		    return BLOCK_FINISH;
		  }
		  return '}';
		}
<block_finish>\}	{
		  IF_DEBUG printf("Hear }, returning to initial.\n");
		  BEGIN(INITIAL);
		  return '}';
		}

[0-9]+			{ yylval.integer_literal = new LiteralTypedExpr<int>(atoi(yytext)); return INTEGER_LITERAL; }

[a-zA-Z][a-zA-Z0-9_]*[!?]?	{ yylval.identifier = new IdentifierExpr(std::string(yytext)); return IDENTIFIER; }

:[a-zA-Z][a-zA-Z0-9_]*[!?]?	{ yylval.symbol = new SymbolExpr(std::string(yytext + 1)); return SYMBOL; }
:\"			{
		  string_buf = "";
		  doing_symbol = true;
		  BEGIN(dbstr);
		}

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

