#include "expr.h"

void IdentifierExpr::p() const {
  std::cout << this->id;
}

void SymbolExpr::p() const {
  std::cout << ":" << this->symbol;
}

void IntegerLiteralExpr::p() const {
  std::cout << value;
}

void FloatingLiteralExpr::p() const {
  std::cout << value;
}

void BooleanLiteralExpr::p() const {
  std::cout << (value ? "true" : "false");
}

void StringLiteralExpr::p() const {
  std::cout << "\"";
  for (int i = 0; i < value.length(); ++i) {
    if (value[i] == '"')
      std::cout << "\\\"";
    else
      std::cout << value[i];
  }
  std::cout << "\"";
}

ArgListExpr::ArgListExpr(Expr *first) {
  this->args.push_back(first);
  // FuncCallExpr will be responsible for this->args' members later.
}

ArgListExpr::ArgListExpr(ArgListExpr *combine, Expr *also) {
  for (std::list<Expr *>::iterator it = combine->args.begin(); it != combine->args.end(); ++it)
    this->args.push_back(*it);
  this->args.push_back(also);

  delete combine;
}

void ArgListExpr::p() const { 
  std::cout << "<ArgListExpr -- XXX: this should never be seen.>";
}

DefListExpr::DefListExpr(IdentifierExpr *first) {
  this->args.push_back(first);
}

DefListExpr::DefListExpr(DefListExpr *combine, IdentifierExpr *also) {
  for (std::list<IdentifierExpr *>::iterator it = combine->args.begin(); it != combine->args.end(); ++it)
    this->args.push_back(*it);
  this->args.push_back(also);

  delete combine;
}

void DefListExpr::p() const { 
  for (std::list<IdentifierExpr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
    if (it != this->args.begin()) std::cout << ", ";
    (*it)->p();
  }
}

void BlockExpr::p() const { 
  if (args) {
    std::cout << "|";
    args->p();
    std::cout << "| ";
  }

  for (std::list<Expr *>::const_iterator it = this->expressions.begin(); it != this->expressions.end(); ++it) {
    if (it != this->expressions.begin()) std::cout << "; ";
    (*it)->p();
  }
}

FuncCallExpr::FuncCallExpr(Expr *_target, IdentifierExpr *_name, ArgListExpr *_args, BlockExpr *_block): target(_target), block(_block) {
  this->name = _name->id;
  if (_args)
    this->args = _args->args;	// we take responsibility here.

  delete _name;
}

void FuncCallExpr::p() const {
  if (target) {
    target->p();
    std::cout << ".";
  }
  std::cout << this->name;
  if (this->args.size() > 0) {
    std::cout << "(";
    for (std::list<Expr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
      if (it != this->args.begin()) std::cout << ", ";
      (*it)->p();
    }
    std::cout << ")";
  }

  if (block) {
    std::cout << " {";
    block->p();
    std::cout << "}";
  }
}

AssignmentExpr::AssignmentExpr(IdentifierExpr *_name, Expr *_value) {
  this->name = _name->id;
  this->value = _value;

  delete _name;
}

void AssignmentExpr::p() const {
  std::cout << this->name << " = ";
  this->value->p();
}

void Program::add_expression(Expr *expression) {
  if (expression)
    this->expressions.push_back(expression);
}

Expr *Program::operator[](int index) {
  std::list<Expr *>::iterator it = expressions.begin();
  while (index > 0)
    ++it, --index;
  return *it;
}

void Program::p() const {
  std::cout << "Program: " << this->expressions.size() << " expression(s)." << std::endl;

  for (std::list<Expr *>::const_iterator it = this->expressions.begin(); it != this->expressions.end(); ++it) {
    (*it)->p();
    std::cout << std::endl;
  }
}

