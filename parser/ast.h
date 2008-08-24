#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <list>
#include <map>
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
    unsigned long length() const;
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

class ExprList
{
  public:
    ExprList();
    ExprList(Expr *);
    ExprList(Expr *, Expr *);
    ExprList(ExprList *, Expr *);
    ExprList(ExprList *, Expr *, Expr *);
    virtual ~ExprList();

    std::list<Expr *> args;
};

class DefListExpr
{
  public:
    DefListExpr(IdentifierExpr *);
    DefListExpr(DefListExpr *, IdentifierExpr *);
    virtual ~DefListExpr();

    std::list<IdentifierExpr *> args;
};

class BlockExpr : public Expr
{
  public:
    BlockExpr();
    BlockExpr(Procedure *);
    virtual ~BlockExpr();

    void take_deflist(DefListExpr *);

    void p() const;
    void push(std::ostream &) const;

    std::list<IdentifierExpr *> args;
    Procedure *proc;
};

class YieldExpr : public Expr
{
  public:
    YieldExpr(ExprList *);

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

    std::list<Expr *> args;
};

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(Expr *, IdentifierExpr *, ExprList *, BlockExpr *);
    virtual ~FuncCallExpr();

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
    virtual ~AssignmentExpr();

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

    std::string name;
    Expr *value;
};

class FuncDefExpr : public Expr
{
  public:
    FuncDefExpr(Expr *, IdentifierExpr *, DefListExpr *, Procedure *);
    virtual ~FuncDefExpr();

    void p() const;
    void emit(std::ostream &) const;

    Expr *target;
    IdentifierExpr *name;
    std::list<IdentifierExpr *> args;
    Procedure *proc;
};

class ConditionalExpr : public Expr
{
  public:
    ConditionalExpr(Expr *, Procedure *, Procedure *);
    virtual ~ConditionalExpr();

    Expr *condition;
    Procedure *on_true, *on_false;

    void p() const;
    void emit(std::ostream &) const;
};

class WhileExpr : public Expr
{
  public:
    WhileExpr(Expr *, Procedure *);
    virtual ~WhileExpr();

    Expr *condition;
    Procedure *action;

    void p() const;
    void emit(std::ostream &) const;
};

class InterpolateExpr : public Expr
{
  public:
    InterpolateExpr();
    virtual ~InterpolateExpr();

    void append(StringLiteralExpr *);
    void append(Procedure *);

    void p() const;
    void emit(std::ostream &) const;
    void push(std::ostream &) const;

  protected:
    class _int_base {
      public:
	virtual ~_int_base();
	virtual void p() const = 0;
	virtual void push(std::ostream &) const = 0;
    };

    class _int_str : public _int_base {
      public:
	_int_str(StringLiteralExpr *);
	~_int_str();
	void p() const;
	void push(std::ostream &) const;
	StringLiteralExpr *str;
    };

    class _int_proc : public _int_base {
      public:
	_int_proc(Procedure *);
	~_int_proc();
	void p() const;
	void push(std::ostream &) const;
	Procedure *proc;
    };

    std::list<_int_base *> data;
};

class Program : public PrettyPrint, public Emitter
{
  public:
    Program();
    Program(std::ostream &);
    virtual ~Program();
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

