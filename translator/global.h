#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef DEBUG
#define DEBUG false
#endif

#define IF_DEBUG if (DEBUG)

#include "expr.h"
#include <string>

typedef union {
  Expr *expr;
  ArgListExpr *arglist;
  BlockExpr *block;

  LiteralTypedExpr<std::string> *string_literal;
  LiteralTypedExpr<int> *integer_literal;
  LiteralTypedExpr<double> *floating_literal;

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

