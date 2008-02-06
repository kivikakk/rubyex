%{
  #include "ruby.tab.h"
%}

%option noyywrap

%x str

%%
		char *string_buf, *string_buf_ptr; int string_consumed, string_size;

\"			{
		  string_buf = malloc(160);
		  string_buf_ptr = string_buf;
		  string_consumed = 0; string_size = 0;
		  BEGIN(str);
		}
<str>\"			{ /* all done */
		  BEGIN(INITIAL);
		  *string_buf_ptr = '\0';
		  yylval.string = string_buf;
		  return STRING_LITERAL;
		}
<str>\n			{
		  /* unterminated string over line */
		}
<str>\\n	*string_buf_ptr++ = '\n';
<str>\\t	*string_buf_ptr++ = '\t';
<str>\\r	*string_buf_ptr++ = '\r';
<str>\\b	*string_buf_ptr++ = '\b';
<str>\\f	*string_buf_ptr++ = '\f';

<str>\\(.|\n)	*string_buf_ptr++ = yytext[1];

<str>[^\\\n\"]+		{
		  char *yptr = yytext;
		  while (*yptr)
		    *string_buf_ptr++ = *yptr++;
		}

[0-9]+		{ yylval.integer = atoi(yytext); return INTEGER_LITERAL; }
[a-zA-Z][a-zA-Z0-9]+	{ yylval.string = strdup(yytext); return IDENTIFIER; }

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }
