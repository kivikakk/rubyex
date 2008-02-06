%{
  #include "ruby.tab.h"
%}

%option noyywrap

%x str

%{
  char *string_buf, *string_buf_ptr; int string_consumed, string_size;
  void append_string_buf(char);
%}

%%

\"			{
		  string_buf = malloc(40);
		  string_buf_ptr = string_buf;
		  string_consumed = 0; string_size = 40;
		  BEGIN(str);
		}
<str>\"			{ /* all done */
		  BEGIN(INITIAL); // XXX TODO: fix the below lines to use the function
		  printf("We're done here. NUL-terminating string.\n");
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
[a-zA-Z][a-zA-Z0-9_]*	{ yylval.string = strdup(yytext); return IDENTIFIER; }

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

%%

void append_string_buf(char c)
{
  char *new_ptr;
  if (string_consumed >= string_size - 1) {
    printf("Used %d out of %d, resizing to %d.\n", string_consumed, string_size, string_size * 2);
    new_ptr = realloc(string_buf, string_size * 2);
    if (!new_ptr) {
      fprintf(stderr, "Ran out of memory in append_string_buf?!\n");
      exit(1);
    }
    string_size *= 2;
    string_buf_ptr = new_ptr + (string_buf_ptr - string_buf);
    string_buf = new_ptr;
    printf("Done.\n");
  }
  printf("Appending '%c' to string.\n", c);
  *string_buf_ptr++ = c;
}

