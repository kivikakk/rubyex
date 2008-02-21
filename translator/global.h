#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef DEBUG
#define DEBUG false
#endif

#define IF_DEBUG if (DEBUG)

#include "ast.h"
#include <string>

int yylex (void);
typedef struct yy_buffer_state;
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

  IdentifierExpr *identifier;
  SymbolExpr *symbol;
  LiteralExpr *literal;
  FuncCallExpr *funccall;
} expu;

#define YYSTYPE expu

extern int block_depths, block_lines;

void enter_block(); void enter_block_line();
void exit_block(); void exit_block_line();
bool in_block();

#endif

