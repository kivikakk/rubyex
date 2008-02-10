%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option noyywrap

%x str

%{
  std::string string_buf; bool doing_symbol;
%}

%%

\"			{
		  string_buf = "";
		  doing_symbol = false;
		  BEGIN(str);
		}
<str>\"			{ /* all done */
		  BEGIN(INITIAL);
		  if (doing_symbol) {
		    yylval.symbol = new SymbolExpr(string_buf);
		    return SYMBOL;
		  }

		  yylval.string_literal = new LiteralTypedExpr<std::string>(string_buf);
		  return STRING_LITERAL;
		}
<str>\n		string_buf += '\n';
<str>\\n	string_buf += '\n';
<str>\\t	string_buf += '\t';
<str>\\r	string_buf += '\r';
<str>\\b	string_buf += '\b';
<str>\\f	string_buf += '\f';

<str>\\(.|\n)	string_buf += yytext[1];

<str>[^\\\n\"]+		{
		  char *yptr = yytext;
		  while (*yptr)
		    string_buf += *yptr++;
		}

[0-9]+			{ yylval.integer_literal = new LiteralTypedExpr<int>(atoi(yytext)); return INTEGER_LITERAL; }

[a-zA-Z][a-zA-Z0-9_]*[!?]?	{ yylval.identifier = new IdentifierExpr(std::string(yytext)); return IDENTIFIER; }

:[a-zA-Z][a-zA-Z0-9_]*[!?]?	{ yylval.symbol = new SymbolExpr(std::string(yytext + 1)); return SYMBOL; }
:\"			{
		  string_buf = "";
		  doing_symbol = true;
		  BEGIN(str);
		}

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

