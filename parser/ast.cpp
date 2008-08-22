#include "ast.h"

void Procedure::p() const {
  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it) {
    if (it != expressions.begin()) std::cout << "; ";
    (*it)->p();
  }
}

void Procedure::emit(std::ostream &o) const {
  // N.B. Procedure::emit doesn't give the length: you have to do it yourself.
  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
    (*it)->emit(o);
}

unsigned long Procedure::length() const {
  unsigned long bytes = 0;

  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
    bytes += (*it)->emit_length();
  
  return bytes;
}

void IdentifierExpr::p() const {
  std::cout << this->id;
}

void IdentifierExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_IDENTIFIER);
  emit_string(o, id);
}

void IdentifierExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_IDENTIFIER);
  emit_string(o, id);
}

void SymbolExpr::p() const {
  std::cout << ":" << this->symbol;
}

void SymbolExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_SYMBOL);
  emit_string(o, symbol);
}

void SymbolExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_SYMBOL);
  emit_string(o, symbol);
}

void IntegerLiteralExpr::p() const {
  std::cout << value;
}

void IntegerLiteralExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_INTEGER_LITERAL);
  emit_int32(o, value);
}

void IntegerLiteralExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_INTEGER_LITERAL);
  emit_int32(o, value);
}

void FloatingLiteralExpr::p() const {
  std::cout << value;
}

void FloatingLiteralExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_FLOATING_LITERAL);
  emit_float(o, value);
}

void FloatingLiteralExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_FLOATING_LITERAL);
  emit_float(o, value);
}

void BooleanLiteralExpr::p() const {
  std::cout << (value ? "true" : "false");
}

void BooleanLiteralExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_BOOLEAN_LITERAL);
  emit_bool(o, value);
}

void BooleanLiteralExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_BOOLEAN_LITERAL);
  emit_bool(o, value);
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
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_STRING_LITERAL);
  emit_text(o, value);	// maybe we have a NUL? -> use emit_text, not emit_string
}

void StringLiteralExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_STRING_LITERAL);
  emit_text(o, value);
}

void NilLiteralExpr::p() const {
  std::cout << "nil";
}

void NilLiteralExpr::emit(std::ostream &o) const {
  emit_instruction(o, I_EXECUTE);
  emit_type(o, T_NIL_LITERAL);
}

void NilLiteralExpr::push(std::ostream &o) const {
  emit_instruction(o, I_PUSH);
  emit_type(o, T_NIL_LITERAL);
}

ExprList::ExprList()
{ }

ExprList::ExprList(Expr *first) {
  this->args.push_back(first);
  // FuncCallExpr will be responsible for this->args' members later.
}

ExprList::ExprList(ExprList *combine, Expr *also) {
  for (std::list<Expr *>::iterator it = combine->args.begin(); it != combine->args.end(); ++it)
    this->args.push_back(*it);
  this->args.push_back(also);

  delete combine;
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

void DefListExpr::p() const
{
  for (std::list<IdentifierExpr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
    if (it != this->args.begin()) std::cout << ", ";
    (*it)->p();
  }
}

BlockExpr::BlockExpr(): args(NULL), proc(NULL)
{ }

BlockExpr::BlockExpr(Procedure *_proc): args(NULL), proc(_proc)
{ }

void BlockExpr::p() const
{
  if (args) {
    std::cout << "|";
    args->p();
    std::cout << "| ";
  }

  proc->p();
}

void BlockExpr::push(std::ostream &o) const
{
  emit_instruction(o, I_CONSTRUCT_BLOCK);
  
  if (args) {
    emit_uint32(o, args->args.size());
    for (std::list<IdentifierExpr *>::const_iterator it = args->args.begin(); it != args->args.end(); ++it)
      emit_string(o, (*it)->id);
  } else
    emit_uint32(o, 0);

  emit_uint32(o, proc->length());
  proc->emit(o);
}

YieldExpr::YieldExpr(ExprList *_args)
{
  if (_args)
    this->args = _args->args;	// we take responsibility here.
}

void YieldExpr::p() const
{
  std::cout << "yield";
  if (this->args.size() > 0) {
    std::cout << "(";
    for (std::list<Expr *>::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
      if (it != this->args.begin()) std::cout << ", ";
      (*it)->p();
    }
    std::cout << ")";
  }
}

void YieldExpr::emit(std::ostream &o) const
{
  for (std::list<Expr *>::const_reverse_iterator it = args.rbegin(); it != args.rend(); ++it)
    (*it)->push(o);

  emit_instruction(o, I_YIELD); 
  emit_uint32(o, args.size());
}

void YieldExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}


FuncCallExpr::FuncCallExpr(Expr *_target, IdentifierExpr *_name, ExprList *_args, BlockExpr *_block): target(_target), block(_block)
{
  this->name = _name->id;
  if (_args)
    this->args = _args->args;	// we take responsibility here.

  delete _name;
}

void FuncCallExpr::p() const
{
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

void FuncCallExpr::emit(std::ostream &o) const
{
  for (std::list<Expr *>::const_reverse_iterator it = args.rbegin(); it != args.rend(); ++it)
    (*it)->push(o);

  if (block)
    block->push(o);

  if (target)
    target->push(o);
  
  emit_instruction(o, target ? (block ? I_TARGET_CALL_BLOCK : I_TARGET_CALL) : (block ? I_CALL_BLOCK : I_CALL));
  emit_string(o, name);
  emit_uint32(o, args.size());
}

void FuncCallExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
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
  value->push(o);
  emit_instruction(o, I_ASSIGNMENT);
  emit_string(o, name);
}

void AssignmentExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

FuncDefExpr::FuncDefExpr(Expr *_target, IdentifierExpr *_name, DefListExpr *_args, Procedure *_proc): target(_target), name(_name), args(_args), proc(_proc)
{ }

void FuncDefExpr::p() const
{
  std::cout << "def ";
  if (target) {
    std::cout << "(";
    target->p();
    std::cout << ").";
  }
  name->p();
  if (args) {
    std::cout << "(";
    for (std::list<IdentifierExpr *>::const_iterator it = args->args.begin(); it != args->args.end(); ++it) {
      if (it != args->args.begin()) std::cout << ", ";
      (*it)->p();
    }
    std::cout << ")";
  }
  std::cout << std::endl;
  proc->p();
  std::cout << std::endl << "end" << std::endl;
}

void FuncDefExpr::emit(std::ostream &o) const
{
  if (target)
    target->push(o);

  emit_instruction(o, target ? I_TARGET_DEF : I_DEF);
  emit_string(o, name->id);
  // if these change from identifiers, might we need to push them instead? I'm not sure what's semantically more correct.
  if (args) {
    emit_uint32(o, args->args.size());
    for (std::list<IdentifierExpr *>::const_iterator it = args->args.begin(); it != args->args.end(); ++it)
      emit_string(o, (*it)->id);
  } else
    emit_uint32(o, 0);

  emit_uint32(o, proc->length());
  proc->emit(o);
}

// ConditionalExpr

ConditionalExpr::ConditionalExpr(Expr *_condition, Procedure *_on_true, Procedure *_on_false): condition(_condition), on_true(_on_true), on_false(_on_false)
{ }

void ConditionalExpr::p() const
{
  if (this->on_true)
    std::cout << "if";
  else if (this->on_false)
    std::cout << "unless";
  else
    std::cout << "no_conditional??";

  std::cout << " (";
  this->condition->p();
  std::cout << ")" << std::endl << "  ";

  if (this->on_true) {
    this->on_true->p();
    std::cout << std::endl;

    if (this->on_false) {
      std::cout << "else" << std::endl << "  ";
      this->on_false->p();
      std::cout << std::endl;
    }
  } else {
    this->on_false->p();
    std::cout << std::endl;
  }
  std::cout << "end" << std::endl;
}

void ConditionalExpr::emit(std::ostream &o) const
{
  // XXX: unless? (we could just invert the result and switch on_t/on_f)

  condition->push(o);
  emit_instruction(o, I_IF);

  if (!on_false)
    emit_uint32(o, on_true->length());
  else
    // If there's an `else' section, we need to omit it in the true branch with a jump (I_JMP+len).
    // Don't use sizeof(instruction_t) -- it's 4. (can't change enum underlying type)
    emit_uint32(o, on_true->length() + 1 + sizeof(uint32));

  on_true->emit(o);

  if (on_false) {
    emit_instruction(o, I_JMP);
    emit_uint32(o, on_false->length());
    on_false->emit(o);
  }
}

// Program

void Program::add_expression(Expr *expression) {
  if (expression) {
    if (emit_as_we_go) {
      emitted_yet = true;
      expression->emit(*emitter_stream);
    } else
      this->expressions.push_back(expression);
  }
}

Program::Program(): emit_as_we_go(false)
{ }

Program::Program(std::ostream &o): emit_as_we_go(true), emitted_yet(false), emitter_stream(&o)
{ }

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
  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
    (*it)->emit(o);
}

bool Program::emitted_flag() const
{
  return emitted_yet;
}

void Program::reset_emitted_flag()
{
  emitted_yet = false;
}

