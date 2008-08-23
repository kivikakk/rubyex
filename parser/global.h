#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef DEBUG
#define DEBUG true
#endif

#define IF_DEBUG if (DEBUG)

#include "ast.h"
#include <string>
#include <vector>

int yylex (void);
struct yy_buffer_state;
yy_buffer_state *yy_scan_string(const char *);

typedef union {
  Expr *expr;
  ExprList *exprlist;
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
extern bool is_block_mode;

typedef enum {
  B_HASH, B_BLOCK
} brace_t;

extern std::vector<brace_t> braces_stack;

void set_block(bool);

void enter_context(); void enter_context_line();
void exit_context(); void exit_context_line();
bool in_context();

void enter_brace(brace_t);
bool last_brace_is(brace_t);
brace_t pop_last_brace();


std::string filter_underscores(const std::string &);

#endif

