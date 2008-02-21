#include "ast.h"

void IdentifierExpr::p() const {
  std::cout << this->id;
}

void IdentifierExpr::emit(std::ostream &o) const {
  emit_string(o, id);
}

instruction_t IdentifierExpr::instruction() const {
  return I_IDENTIFIER;
}

void SymbolExpr::p() const {
  std::cout << ":" << this->symbol;
}

void SymbolExpr::emit(std::ostream &o) const {
  emit_string(o, symbol);
}

instruction_t SymbolExpr::instruction() const {
  return I_SYMBOL;
}

void IntegerLiteralExpr::p() const {
  std::cout << value;
}

void IntegerLiteralExpr::emit(std::ostream &o) const {
  emit_int32(o, value);
}

instruction_t IntegerLiteralExpr::instruction() const {
  return I_INTEGER_LITERAL;
}

void FloatingLiteralExpr::p() const {
  std::cout << value;
}

void FloatingLiteralExpr::emit(std::ostream &o) const {
  emit_float(o, value);
}

instruction_t FloatingLiteralExpr::instruction() const {
  return I_FLOATING_LITERAL;
}

void BooleanLiteralExpr::p() const {
  std::cout << (value ? "true" : "false");
}

void BooleanLiteralExpr::emit(std::ostream &o) const {
  emit_bool(o, value);
}

instruction_t BooleanLiteralExpr::instruction() const {
  return I_BOOLEAN_LITERAL;
}

void StringLiteralExpr::p() const {
  std::cout << "\"";
  for (unsigned int i = 0; i < value.length(); ++i) {
    if (value[i] == '"')
      std::cout << "\\\"";
    else
      std::cout << value[i];
  }
  std::cout << "\"";
}

void StringLiteralExpr::emit(std::ostream &o) const {
  emit_text(o, value); // using emit_text; maybe we have a NUL?
}

instruction_t StringLiteralExpr::instruction() const {
  return I_STRING_LITERAL;
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

void ArgListExpr::emit(std::ostream &o) const {
  std::cerr << "<ArgListExpr -- XXX: I am being emitted! Whyyyy!?>" << std::endl;
}

instruction_t ArgListExpr::instruction() const {
  std::cerr << "<ArgListExpr -- XXX: you may not have my instruction_t!>" << std::endl;
  throw std::exception();
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

void DefListExpr::emit(std::ostream &o) const {
  emit_uint32(o, args.size());
  for (std::list<IdentifierExpr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it)
    (*it)->emit(o);
}

instruction_t DefListExpr::instruction() const {
  return I_DEF_LIST;
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

void BlockExpr::emit(std::ostream &o) const {
  emit_bool(o, args);
  if (args)
    args->emit(o);
  emit_uint32(o, expressions.size());
  for (std::list<Expr *>::const_iterator it = this->expressions.begin(); it != this->expressions.end(); ++it) {
    emit_instruction(o, (*it)->instruction());
    (*it)->emit(o);
  }
}

instruction_t BlockExpr::instruction() const {
  return I_BLOCK;
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

void FuncCallExpr::emit(std::ostream &o) const {
  emit_bool(o, target);
  if (target)
    target->emit(o);
  emit_string(o, name);
  emit_uint32(o, args.size());
  for (std::list<Expr *>::const_iterator it = args.begin(); it != args.end(); ++it) {
    emit_instruction(o, (*it)->instruction());
    (*it)->emit(o);
  }
  emit_bool(o, block);
  if (block)
    block->emit(o);
}

instruction_t FuncCallExpr::instruction() const {
  return I_FUNC_CALL;
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

void AssignmentExpr::emit(std::ostream &o) const {
  emit_string(o, name);
  emit_instruction(o, value->instruction());
  value->emit(o);
}

instruction_t AssignmentExpr::instruction() const {
  return I_ASSIGNMENT;
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

void Program::emit(std::ostream &o) const {
  emit_uint32(o, expressions.size());
  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it) {
    emit_instruction(o, (*it)->instruction());
    (*it)->emit(o);
  }
}

instruction_t Program::instruction() const {
  std::cerr << "<Program -- XXX: you may not have my instruction_t!>" << std::endl;
  throw std::exception();
}

