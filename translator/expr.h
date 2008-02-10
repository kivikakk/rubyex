#ifndef EXPR_H
#define EXPR_H

#include <iostream>
#include <string>
#include <list>
#include "pretty.h"

class Expr : public PrettyPrint
{
  public:
    Expr() { }
    virtual ~Expr() { }
};

class IdentifierExpr : public Expr
{
  public:
    IdentifierExpr(const std::string &_id): id(_id) { }

    void p(int) const;

    std::string id;
};

class LiteralExpr : public Expr { };

template <typename T> class LiteralTypedExpr : public LiteralExpr
{
  public:
    LiteralTypedExpr<T>(T _value): value(_value) { }

    void p(int tabs) const {
      std::cout << p_tabs(tabs) << "LiteralTypedExpr<" << typeid(T).name() << ">: " << value << std::endl;
    }

    T value;
};

class ArgListExpr : public Expr
{
  public:
    ArgListExpr(Expr *);
    ArgListExpr(ArgListExpr *, Expr *);

    void p(int) const; 

    std::list<Expr *> args;
};

typedef enum {
  NEGATE
} UnaryOp;

extern const char *UnaryOpStrings[];

class UnaryOpExpr : public Expr
{
  public:
    UnaryOpExpr(UnaryOp _op, Expr *_expr): op(_op), expr(_expr) { }

    void p(int) const;

    UnaryOp op;
    Expr *expr;
};

typedef enum {
  ADD, SUBTRACT, MULTIPLY, DIVIDE, POWER
} BinaryOp;

extern const char *BinaryOpStrings[];

class BinaryOpExpr : public Expr
{
  public:
    BinaryOpExpr(BinaryOp _op, Expr *_left, Expr *_right): op(_op), left(_left), right(_right) { }

    void p(int) const;

    BinaryOp op;
    Expr *left, *right;
};

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(Expr *, IdentifierExpr *, ArgListExpr *);

    void p(int) const;

    Expr *target;
    std::string name;
    std::list<Expr *> args;
};

class Statement : public PrettyPrint
{
  public:
    Statement() { }
    virtual ~Statement() { }
};

class AssignmentStatement : public Statement
{
  public:
    AssignmentStatement(IdentifierExpr *, Expr *);

    void p(int) const;

    std::string name;
    Expr *value;
};

class ExprStatement : public Statement
{
  public:
    ExprStatement(Expr *_value): value(_value) { }

    void p(int) const;

    Expr *value;
};

class Program : public PrettyPrint
{
  public:
    Program() { }

    void add_statement(Statement *);

    void p(int) const;

    std::list<Statement *> statements;
};

#endif

