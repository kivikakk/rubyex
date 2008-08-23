%{
  #include "global.h"

  #include "ruby.tab.h"
  #include <string>
%}

%option yywrap

%x dbstr sstr context_finish starting_block_args arglist_begins
%s could_be_starting_block_args
%s block_args

%{
  std::string string_buf; bool doing_symbol = false;
%}

%%

#[^\n]*		{ }

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

<dbstr>[^\\\n\"]+	string_buf += yytext;

'			{
		  string_buf = "";
		  doing_symbol = false;
		  BEGIN(sstr);
		}

<sstr>'			{
		  BEGIN(INITIAL);
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

==		{ return EQ; }
!=		{ return NEQ; }
\<=		{ return LE; }
>=		{ return GE; }

=>		{ return ASSOC; }

def		{ return DEF; }
if		{ return IF; }
else		{ return ELSE; }
elsif		{ return ELSIF; }
unless		{ return UNLESS; }

true		{ yylval.boolean_literal = new BooleanLiteralExpr(true); return BOOLEAN_LITERAL; }
false		{ yylval.boolean_literal = new BooleanLiteralExpr(false); return BOOLEAN_LITERAL; }
nil		{ yylval.nil_literal = new NilLiteralExpr(); return NIL_LITERAL; }

yield		{ return YIELD; }
yield/\(		{ return YIELD; }

do[\t ]*/\|	{ BEGIN(starting_block_args); std::cerr << "Returning DO with args" << std::endl; return DO; }
\{[\t ]*/\|	{ BEGIN(starting_block_args); return '{'; }



do[\t {]?			{
		  /* no args (if args are given, the args version will match - it's longer, so flex will pick it).
		   * inject empty arglist into stream, as the parser always sees args coming. */
		  yyless(2);
		  unput('|'); unput('|');
		  BEGIN(starting_block_args);
		  std::cerr << "Returning DO without args" << std::endl;
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

<starting_block_args>\|			{ BEGIN(block_args); return BLOCK_ARGUMENT_START; }
<could_be_starting_block_args>\|	{ BEGIN(block_args); std::cerr << "starting" << std::endl; return BLOCK_ARGUMENT_START; }
<could_be_starting_block_args>[^ \t]	{ BEGIN(INITIAL); std::cerr << "returning on " << yytext << std::endl; yyless(0); }
<block_args>\|				{ BEGIN(INITIAL); return BLOCK_ARGUMENT_END; }

\{			{ /* see "do" */
		  /* if (!is_block_mode) {
		    unput('|'); unput('|');
		    BEGIN(starting_block_args);
		  } */
		  BEGIN(could_be_starting_block_args);
		  std::cerr << "process: '{'" << std::endl;
		  return '{';
		}
\}			{
		  std::cerr << "process: '}'" << std::endl;
		  if (/* last_brace_is(B_BLOCK) && */ in_context() && context_lines == context_depths) {
		    IF_DEBUG printf("Going to context_finish (}).");
		    BEGIN(context_finish);
		    yyless(0);	// whoops. yyless returns all BUT first `n' chars.
		    pop_last_brace();
		    return CONTEXT_FINISH;
		  }
		  pop_last_brace();
		  return '}';
		}


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

		/* There's gotta be a more succint way to do these. */

-?[0-9][0-9_]*[0-9]\.[0-9][0-9_]*[0-9]	{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; /* Not happy with this. */ }
-?[0-9]\.[0-9][0-9_]*[0-9]		{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }
-?[0-9][0-9_]*[0-9]\.[0-9]		{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }
-?[0-9]\.[0-9]				{ yylval.floating_literal = new FloatingLiteralExpr(atof(filter_underscores(yytext).c_str())); return FLOATING_LITERAL; }

-?[0-9][0-9_]*[0-9]			{ yylval.integer_literal = new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); return INTEGER_LITERAL; }
-?[0-9]					{ yylval.integer_literal = new IntegerLiteralExpr(atoi(filter_underscores(yytext).c_str())); return INTEGER_LITERAL; }


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
[a-zA-Z][a-zA-Z0-9_]*/\(		{
			  std::string s = yytext;
			  yylval.identifier = new IdentifierExpr(s);
			  BEGIN(arglist_begins);
			  return FUNCTION_CALL;
			}

<arglist_begins>\(		{
			  BEGIN(INITIAL);
			  return ARG_BRACKET;
			}

[a-zA-Z_][a-zA-Z0-9_]*[!?]?[ \t]*\{	{ 
			std::string s = yytext;
			s = s.substr(0, s.length() - 1);
			while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')
			  s = s.substr(0, s.length() - 1);
			yylval.identifier = new IdentifierExpr(s);
			if (s == "do") {
			  std::cerr << "Returning DO by backdoor" << std::endl; // XXX XXX
			  return DO;
			}
			unput('{'); unput(')'); unput('(');
			BEGIN(arglist_begins);
			return FUNCTION_CALL;
		      }

[a-zA-Z_][a-zA-Z0-9_]*	{ yylval.identifier = new IdentifierExpr(std::string(yytext)); return IDENTIFIER; }

:[a-zA-Z_][a-zA-Z0-9_]*[!?]?	{ yylval.symbol = new SymbolExpr(std::string(yytext + 1)); return SYMBOL; }
:\"			{
		  string_buf = "";
		  doing_symbol = true;
		  BEGIN(dbstr);
		}
:'			{
		  string_buf = "";
		  doing_symbol = true;
		  BEGIN(sstr);
		}

[\t ]+		{ }
.		{ return yytext[0]; }
\n		{ return NL; }

