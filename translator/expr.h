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

class SymbolExpr : public Expr
{
  public:
    SymbolExpr(const std::string &_symbol): symbol(_symbol) { }

    void p(int) const;

    std::string symbol;
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

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(Expr *, IdentifierExpr *, ArgListExpr *);

    void p(int) const;

    Expr *target;
    std::string name;
    std::list<Expr *> args;
};

class AssignmentExpr : public Expr
{
  public:
    AssignmentExpr(IdentifierExpr *, Expr *);

    void p(int) const;

    std::string name;
    Expr *value;
};

class Program : public PrettyPrint
{
  public:
    Program() { }

    void add_expression(Expr *);

    void p(int) const;

    std::list<Expr *> expressions;
};

#endif

