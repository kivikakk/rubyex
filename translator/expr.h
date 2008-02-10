#ifndef EXPR_H
#define EXPR_H

#include <string>
#include <list>

class Expr
{
  public:
    Expr() { }
    virtual ~Expr() { }
};

class ExprPtr
{
  public:
    ExprPtr(): ptr(NULL) { }
    ExprPtr(Expr *_ptr): ptr(_ptr) { }
    Expr *ptr;
};

class IdentifierExpr : public Expr
{
  public:
    IdentifierExpr(const std::string &_id): id(_id) { }

    std::string id;
};

class LiteralExpr : public Expr { };

template <typename T> class LiteralTypedExpr : public LiteralExpr
{
  public:
    LiteralTypedExpr<T>(T _value): value(_value) { }

    T value;
};

class ArgListExpr : public Expr
{
  public:
    ArgListExpr(Expr *first) {
      args.push_back(first);
      // FuncCallExpr will be responsible for this->args' members later.
    }

    ArgListExpr(ArgListExpr *combine, Expr *also) {
      for (std::list<Expr *>::iterator it = combine->args.begin(); it != combine->args.end(); ++it)
        args.push_back(*it);
      args.push_back(also);
    }

    std::list<Expr *> args;
};

typedef enum {
  NEGATE
} UnaryOp;

class UnaryOpExpr : public Expr
{
  public:
    UnaryOpExpr(UnaryOp _op, Expr *_expr): op(_op), expr(_expr) { }

    UnaryOp op;
    Expr *expr;
};

class FuncCallExpr : public Expr
{
  public:
    FuncCallExpr(IdentifierExpr *_name, ArgListExpr *_args) {
      name = _name->id;
      args = _args->args;	// we take responsibility here.
    }

    std::string name;
    std::list<Expr *> args;
};

#endif

