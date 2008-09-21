#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef DEBUG
#define DEBUG false
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
  IdListExpr *idlist;
  BlockExpr *block;

  StringLiteralExpr *string_literal;
  IntegerLiteralExpr *integer_literal;
  FloatingLiteralExpr *floating_literal;
  BooleanLiteralExpr *boolean_literal;
  NilLiteralExpr *nil_literal;

  IdentifierExpr *identifier;
  SymbolExpr *symbol;
  LiteralExpr *literal;
  ClassDefExpr *classdef;
  ModuleDefExpr *moduledef;
  FuncCallExpr *funccall;
  FuncDefExpr *funcdef;
  FuncDefListEntity *funcdeflistentity;
  FuncDefList *funcdeflist;
  ConditionalExpr *conditional;
  WhileExpr *while_loop;
  BeginSectionExpr *begin_section;
  Rescue *rescue;
  InterpolateExpr *interpolated_expr;

  Procedure *procedure;
} expu;

#define YYSTYPE expu

extern int context_depths, context_lines;
extern bool string_lit_rs;
extern bool backtick_lit_rs;

void restart_string_literal();
void restart_backtick_literal();
void enter_context(); void enter_context_line();
void exit_context(); void exit_context_line();
bool in_context();

// in ruby.l
void destroy_current_buffer();
void destroy_old_buffer();
void assign_new_buffer();
int yylex_destroy();

std::string filter_underscores(const std::string &);

#endif

