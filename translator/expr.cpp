#include "expr.h"

void IdentifierExpr::p(int tabs) const {
  std::cout << p_tabs(tabs) << "IdentifierExpr: " << this->id << std::endl;
}

void SymbolExpr::p(int tabs) const {
  std::cout << p_tabs(tabs) << "SymbolExpr: :" << this->symbol << std::endl;
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

void ArgListExpr::p(int tabs) const { 
  std::cout << p_tabs(tabs) << "ArgListExpr -- XXX: this should never be seen." << std::endl;
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

void DefListExpr::p(int tabs) const { 
  std::cout << p_tabs(tabs) << "DefListExpr: " << this->args.size() << " identifier(s)." << std::endl;
  for (std::list<IdentifierExpr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it)
    (*it)->p(tabs + 1);
}

void BlockExpr::p(int tabs) const { 
  std::cout << p_tabs(tabs) << "BlockExpr: ";
  if (args) std::cout << "|DefListExpr| ";
  std::cout << this->expressions.size() << " expression(s)." << std::endl;

  if (args)
    args->p(tabs + 1);

  for (std::list<Expr *>::const_iterator it = this->expressions.begin(); it != this->expressions.end(); ++it)
    (*it)->p(tabs + 1);
}

FuncCallExpr::FuncCallExpr(Expr *_target, IdentifierExpr *_name, ArgListExpr *_args, BlockExpr *_block): target(_target), block(_block) {
  this->name = _name->id;
  if (_args)
    this->args = _args->args;	// we take responsibility here.

  delete _name;
}

void FuncCallExpr::p(int tabs) const {
  std::cout << p_tabs(tabs) << "FuncCallExpr: ";
  if (target)
    std::cout << "Expr.";
  std::cout << this->name << "(arguments)";
  if (block)
    std::cout << " { BlockExpr }";
  std::cout << std::endl;

  if (target)
    target->p(tabs + 1);
  for (std::list<Expr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it)
    (*it)->p(tabs + 1);
  if (block)
    block->p(tabs + 1);
}

AssignmentExpr::AssignmentExpr(IdentifierExpr *_name, Expr *_value) {
  this->name = _name->id;
  this->value = _value;

  delete _name;
}

void AssignmentExpr::p(int tabs) const {
  std::cout << p_tabs(tabs) << "AssignmentExpr: " << this->name << " = Expr" << std::endl;
  this->value->p(tabs + 1);
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

void Program::p(int tabs) const {
  std::cout << p_tabs(tabs) << "Program: " << this->expressions.size() << " expression(s)." << std::endl;

  for (std::list<Expr *>::const_iterator it = this->expressions.begin(); it != this->expressions.end(); ++it)
    (*it)->p(tabs + 1);
}

