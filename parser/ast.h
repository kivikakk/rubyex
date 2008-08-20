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

class Procedure : public PrettyPrint, public Emitter
{
  public:
    std::list<Expr *> expressions;

    void p() const;
    void emit(std::ostream &) const;
};

class IdentifierExpr : public Expr
{
  public:
    IdentifierExpr(const std::string &_id): id(_id) { }

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

    std::string id;
};

class SymbolExpr : public Expr
{
  public:
    SymbolExpr(const std::string &_symbol): symbol(_symbol) { }

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

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
      void push(std::ostream &) const; \
  }

TypedClassDefinition(IntegerLiteralExpr, int);
TypedClassDefinition(FloatingLiteralExpr, double);
TypedClassDefinition(BooleanLiteralExpr, bool);
TypedClassDefinition(StringLiteralExpr, std::string);

class NilLiteralExpr : public LiteralExpr
{
  public:
    NilLiteralExpr() { }
    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;
};

class ArgListExpr : public Expr
{
  public:
    ArgListExpr(Expr *);
    ArgListExpr(ArgListExpr *, Expr *);

    std::list<Expr *> args;
};

class DefListExpr : public Expr
{
  public:
    DefListExpr(IdentifierExpr *);
    DefListExpr(DefListExpr *, IdentifierExpr *);

    void p() const; 

    std::list<IdentifierExpr *> args;
};

class BlockExpr : public Expr
{
  public:
    BlockExpr();
    BlockExpr(Procedure *);

    void p() const;
    void push(std::ostream &) const;

    DefListExpr *args;
    Procedure *proc;
};

class YieldExpr : public Expr
{
  public:
    YieldExpr(ArgListExpr *);

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

    std::list<Expr *> args;
};

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(Expr *, IdentifierExpr *, ArgListExpr *, BlockExpr *);

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

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

    std::string name;
    Expr *value;
};

class FuncDefExpr : public Expr
{
  public:
    FuncDefExpr(Expr *, IdentifierExpr *, DefListExpr *, Procedure *);

    void p() const;
    void emit(std::ostream &) const;

    Expr *target;
    IdentifierExpr *name;
    DefListExpr *args;
    Procedure *proc;
};

class Program : public PrettyPrint, public Emitter
{
  public:
    Program();
    Program(std::ostream &);
    Expr *operator[](int);

    void add_expression(Expr *);

    void p() const;
    void emit(std::ostream &) const;

    std::list<Expr *> expressions;

    bool emitted_flag() const;
    void reset_emitted_flag();

  protected:
    bool emit_as_we_go, emitted_yet;
    std::ostream *emitter_stream;
};

#endif

