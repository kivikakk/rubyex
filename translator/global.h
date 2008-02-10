#ifndef GLOBAL_H
#define GLOBAL_H

#include "expr.h"
#include <string>

typedef union {
  Expr *expr;
  ArgListExpr *arglist;

  LiteralTypedExpr<std::string> *string_literal;
  LiteralTypedExpr<int> *integer_literal;
  LiteralTypedExpr<double> *floating_literal;

  IdentifierExpr *identifier;
  LiteralExpr *literal;
  FuncCallExpr *funccall;
} expu;

#define YYSTYPE expu

#endif

