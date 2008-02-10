%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option noyywrap

%x str

%{
  std::string string_buf;
%}

%%

\"			{
		  string_buf = "";
		  BEGIN(str);
		}
<str>\"			{ /* all done */
		  BEGIN(INITIAL);
		  yylval = new LiteralTypedExpr<std::string>(string_buf);
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

[0-9]+			{ yylval = new LiteralTypedExpr<int>(atoi(yytext)); return INTEGER_LITERAL; }

[a-zA-Z][a-zA-Z0-9_]*	{ yylval = new IdentifierExpr(std::string(yytext)); return IDENTIFIER; }

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

