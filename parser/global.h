#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef DEBUG
#define DEBUG false
#endif

#define IF_DEBUG if (DEBUG)

#include "ast.h"
#include <string>

int yylex (void);
struct yy_buffer_state;
yy_buffer_state *yy_scan_string(const char *);

typedef union {
  Expr *expr;
  ArgListExpr *arglist;
  DefListExpr *deflist;
  BlockExpr *block;

  StringLiteralExpr *string_literal;
  IntegerLiteralExpr *integer_literal;
  FloatingLiteralExpr *floating_literal;
  BooleanLiteralExpr *boolean_literal;
  NilLiteralExpr *nil_literal;

  IdentifierExpr *identifier;
  SymbolExpr *symbol;
  LiteralExpr *literal;
  FuncCallExpr *funccall;
  FuncDefExpr *funcdef;
  ConditionalExpr *conditional;

  Procedure *procedure;
} expu;

#define YYSTYPE expu

extern int context_depths, context_lines;

void enter_context(); void enter_context_line();
void exit_context(); void exit_context_line();
bool in_context();

#endif

