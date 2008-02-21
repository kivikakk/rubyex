#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <list>
#include "pretty.h"
#include "emitter.h"

class Expr : public PrettyPrint, public Emitter
{
  public:
    Expr() { }
    virtual ~Expr() { }
};

class IdentifierExpr : public Expr
{
  public:
    IdentifierExpr(const std::string &_id): id(_id) { }

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::string id;
};

class SymbolExpr : public Expr
{
  public:
    SymbolExpr(const std::string &_symbol): symbol(_symbol) { }

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::string symbol;
};

class LiteralExpr : public Expr { };

template <typename T> class LiteralTypedExpr : public LiteralExpr
{
  public:
    LiteralTypedExpr<T>(T _value): value(_value) { }

    T value;
};

#define TypedClassDefinition(name, type) \
  class name : public LiteralTypedExpr<type> \
  { \
    public: \
      name(type _value): LiteralTypedExpr<type>(_value) { } \
      void p() const; \
      void emit(std::ostream &) const; \
      instruction_t instruction() const; \
  }

TypedClassDefinition(IntegerLiteralExpr, int);
TypedClassDefinition(FloatingLiteralExpr, double);
TypedClassDefinition(BooleanLiteralExpr, bool);
TypedClassDefinition(StringLiteralExpr, std::string);

class ArgListExpr : public Expr
{
  public:
    ArgListExpr(Expr *);
    ArgListExpr(ArgListExpr *, Expr *);

    void p() const; 
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::list<Expr *> args;
};

class DefListExpr : public Expr
{
  public:
    DefListExpr(IdentifierExpr *);
    DefListExpr(DefListExpr *, IdentifierExpr *);

    void p() const; 
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::list<IdentifierExpr *> args;
};

class BlockExpr : public Expr
{
  public:
    BlockExpr(): args(NULL) { }

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    DefListExpr *args;
    std::list<Expr *> expressions;
};

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(Expr *, IdentifierExpr *, ArgListExpr *, BlockExpr *);

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    Expr *target;
    std::string name;
    std::list<Expr *> args;
    BlockExpr *block;
};

class AssignmentExpr : public Expr
{
  public:
    AssignmentExpr(IdentifierExpr *, Expr *);

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::string name;
    Expr *value;
};

class Program : public PrettyPrint, public Emitter
{
  public:
    Program() { }
    Expr *operator[](int);

    void add_expression(Expr *);

    void p() const;
    void emit(std::ostream &) const;
    instruction_t instruction() const;

    std::list<Expr *> expressions;
};

#endif

