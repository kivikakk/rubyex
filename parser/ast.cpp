#include "ast.h"

// Procedure

Procedure::Procedure()
{ }

Procedure::Procedure(Expr *_first) {
  expressions.push_back(_first);
}

Procedure::~Procedure() {
  for (std::list<Expr *>::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
    delete *it;
}

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

// IdentifierExpr

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

// SymbolExpr

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

// IntegerLiteralExpr

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

// FloatingLiteralExpr

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

// BooleanLiteralExpr

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

// StringLiteralExpr

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

// NilLiteralExpr

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

// FalsityExpr

FalsityExpr::FalsityExpr(Expr *_expr): expr(_expr)
{ }

FalsityExpr::~FalsityExpr() {
  delete expr;
}

void FalsityExpr::p() const {
  std::cout << "!";
  expr->p();
}

void FalsityExpr::emit(std::ostream &o) const {
  expr->push(o);
  emit_instruction(o, I_FALSITY);
}

void FalsityExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// ExprList

ExprList::ExprList()
{ }

ExprList::ExprList(Expr *first) {
  this->args.push_back(first);
  // FuncCallExpr will be responsible for this->args' members later.
}

ExprList::ExprList(Expr *first, Expr *second) {
  this->args.push_back(first); this->args.push_back(second);
}

ExprList::ExprList(Expr *first, Expr *second, Expr *third) {
  this->args.push_back(first); this->args.push_back(second); this->args.push_back(third);
}

ExprList::ExprList(ExprList *combine, Expr *also) {
  this->args = combine->args;
  this->args.push_back(also);

  delete combine;
}

ExprList::ExprList(ExprList *combine, Expr *also, Expr *and_even) {
  this->args = combine->args;
  this->args.push_back(also);
  this->args.push_back(and_even);

  delete combine;
}

ExprList::ExprList(ExprList *combine, Expr *also, Expr *and_even, Expr *also_features) {
  this->args = combine->args;
  this->args.push_back(also);
  this->args.push_back(and_even);
  this->args.push_back(also_features);

  delete combine;
}

ExprList::~ExprList() {
  // NOTE: WE DO NOTHING!
  // this->args is taken directly by all of the objects that use us!
  // If we eliminated our arguments, we'd be in trouble when combining with
  // ourselves also (since that `delete's the one we combined with,
  // see ExprList(ExprList*, Expr*) above.
}

// FuncDefListEntity

FuncDefListEntity::FuncDefListEntity(IdentifierExpr *_id): id(_id->id), is_catch_all(false), default_value(NULL) {
  delete _id;
}

FuncDefListEntity::FuncDefListEntity(IdentifierExpr *_id, bool _is_catch_all): id(_id->id), is_catch_all(_is_catch_all), default_value(NULL) {
  delete _id;
}

FuncDefListEntity::FuncDefListEntity(IdentifierExpr *_id, Procedure *_default_value): id(_id->id), default_value(_default_value) {
  delete _id;
}

FuncDefListEntity::~FuncDefListEntity() {
  delete default_value;
}

void FuncDefListEntity::p() const {
  if (is_catch_all)
    std::cout << "*";

  std::cout << id;
  if (default_value) {
    std::cout << "=";
    default_value->p();
  }
}

void FuncDefListEntity::emit(std::ostream &o) const {
  emit_string(o, id);
  if (default_value) {
    emit_uint32(o, default_value->length());
    default_value->emit(o);
  }
}

// FuncDefList

FuncDefList::FuncDefList(FuncDefListEntity *_entity) {
  args.push_back(_entity);
}

FuncDefList::~FuncDefList() {
  for (std::list<FuncDefListEntity *>::iterator it = args.begin(); it != args.end(); ++it)
    delete *it;
}

void FuncDefList::add(FuncDefListEntity *_entity) {
  args.push_back(_entity);
}

bool FuncDefList::valid_syntax() const {
  for (std::list<FuncDefListEntity *>::const_reverse_iterator it = args.rbegin(); it != args.rend(); ++it)
    if ((*it)->is_catch_all && it != args.rbegin())
      return false;
  bool switched = false;
  for (std::list<FuncDefListEntity *>::const_iterator it = args.begin(); it != args.end(); ++it) {
    if ((*it)->default_value)
      switched = true;
    else if (!(*it)->default_value && !(*it)->is_catch_all && switched)
      return false;
  }
  return true;
}

int FuncDefList::normal_args() const {
  int c = 0;
  for (std::list<FuncDefListEntity *>::const_iterator it = args.begin(); it != args.end(); ++it)
    if (!(*it)->default_value && !(*it)->is_catch_all)
      c++;
  return c;
}

int FuncDefList::opt_args() const {
  int c = 0;
  for (std::list<FuncDefListEntity *>::const_iterator it = args.begin(); it != args.end(); ++it)
    if ((*it)->default_value)
      c++;
  return c;
}

bool FuncDefList::has_splat_arg() const {
  for (std::list<FuncDefListEntity *>::const_reverse_iterator it = args.rbegin(); it != args.rend(); ++it)
    if ((*it)->default_value)
      return true;
    else
      return false;
  return false;
}

void FuncDefList::p() const {
  std::cout << "(";
  for (std::list<FuncDefListEntity *>::const_iterator it = args.begin(); it != args.end(); ++it) {
    if (it != args.begin()) std::cout << ", ";
    (*it)->p();
  }
  std::cout << ")";
}

void FuncDefList::emit(std::ostream &o) const {
  emit_uint32(o, normal_args());
  emit_uint32(o, opt_args());
  emit_bool(o, has_splat_arg());
  for (std::list<FuncDefListEntity *>::const_iterator it = args.begin(); it != args.end(); ++it)
    (*it)->emit(o);
}

void FuncDefList::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// DefListExpr

DefListExpr::DefListExpr(IdentifierExpr *first) {
  this->args.push_back(first);
}

DefListExpr::DefListExpr(DefListExpr *combine, IdentifierExpr *also) {
  this->args = combine->args;
  this->args.push_back(also);

  delete combine;
}

DefListExpr::~DefListExpr() {
  // See ~ExprList() for an explanation.
}

// IdListExpr

IdListExpr::IdListExpr(IdentifierExpr *first) {
  this->args.push_back(first);
}

IdListExpr::IdListExpr(IdListExpr *combine, IdentifierExpr *also) {
  this->args = combine->args;
  this->args.push_back(also);

  delete combine;
}

IdListExpr::~IdListExpr() {
  // See ~ExprList() for an explanation.
}

// BlockExpr

BlockExpr::BlockExpr(): proc(NULL)
{ }

BlockExpr::BlockExpr(Procedure *_proc): proc(_proc)
{ }

BlockExpr::~BlockExpr() {
  for (std::list<IdentifierExpr *>::iterator it = args.begin(); it != args.end(); ++it)
    delete *it;
  delete proc;
}

void BlockExpr::take_deflist(DefListExpr *_dl)
{
  // this may be called with NULLs. (see `block:' section of ruby.y)
  if (_dl) {
    args = _dl->args;		// responsibility.
    delete _dl;
  }
}

void BlockExpr::p() const
{
  if (args.size() > 0) {
    std::cout << "|";
    for (std::list<IdentifierExpr *>::const_iterator it = args.begin(); it != args.end(); ++it) {
      if (it != args.begin()) std::cout << ", ";
      (*it)->p();
    }
    std::cout << "| ";
  }

  proc->p();
}

void BlockExpr::push(std::ostream &o) const
{
  emit_instruction(o, I_CONSTRUCT_BLOCK);
  
  emit_uint32(o, args.size());
  for (std::list<IdentifierExpr *>::const_iterator it = args.begin(); it != args.end(); ++it)
    emit_string(o, (*it)->id);

  emit_uint32(o, proc->length());
  proc->emit(o);
}

// YieldExpr

YieldExpr::YieldExpr(ExprList *_args)
{
  if (_args) {
    this->args = _args->args;	// we take responsibility here.
    delete _args;
  }
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

// ClassDefExpr

ClassDefExpr::ClassDefExpr(IdentifierExpr *_name, IdentifierExpr *_super, BlockExpr *_proc): name(_name), super(_super), proc(_proc)
{ }

void ClassDefExpr::p() const {
  std::cout << "class ";
  name->p();
  if (super) {
    std::cout << "< ";
    super->p();
  }
  std::cout << std::endl << "  ";
  proc->p();
  std::cout << std::endl << "end";
}

void ClassDefExpr::emit(std::ostream &o) const {
  proc->push(o);
  if (super)
    super->push(o);
  name->push(o);

  emit_instruction(o, super ? I_CLASS_INHERIT : I_CLASS);
}

void ClassDefExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// ModuleExpr

ModuleDefExpr::ModuleDefExpr(IdentifierExpr *_name, BlockExpr *_proc): name(_name), proc(_proc)
{ }

void ModuleDefExpr::p() const {
  std::cout << "module ";
  name->p();
  std::cout << std::endl << "  ";
  proc->p();
  std::cout << std::endl << "end";
}

void ModuleDefExpr::emit(std::ostream &o) const {
  proc->push(o);
  name->push(o);

  emit_instruction(o, I_MODULE);
}

void ModuleDefExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// FuncCallExpr

FuncCallExpr::FuncCallExpr(Expr *_target, IdentifierExpr *_name, ExprList *_args, BlockExpr *_block): target(_target), block(_block)
{
  this->name = _name->id;
  if (_args) {
    this->args = _args->args;	// we take responsibility here.
    delete _args;
  }

  delete _name;
}

FuncCallExpr::~FuncCallExpr()
{
  delete target;
  for (std::list<Expr *>::iterator it = this->args.begin(); it != this->args.end(); ++it)
    delete *it;
  delete block;
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

// AssignmentExpr

AssignmentExpr::AssignmentExpr(IdentifierExpr *_name, Expr *_value) {
  this->name = _name->id;
  this->value = _value;

  delete _name;
}

AssignmentExpr::~AssignmentExpr() {
  delete value;
}

void AssignmentExpr::p() const {
  std::cout << "(" << this->name << " = ";
  this->value->p();
  std::cout << ")";
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

// FuncDefExpr

FuncDefExpr::FuncDefExpr(Expr *_target, IdentifierExpr *_name, FuncDefList *_args, Procedure *_proc): target(_target), name(_name), args(_args), proc(_proc)
{ }

FuncDefExpr::~FuncDefExpr()
{
  delete target;
  delete name;
  delete args;
  delete proc;
}
  
void FuncDefExpr::p() const
{
  std::cout << "def ";
  if (target) {
    std::cout << "(";
    target->p();
    std::cout << ").";
  }
  name->p();
  if (args)
    args->p();
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
  if (args) {
    emit_bool(o, true);
    args->emit(o);
  } else
    emit_bool(o, false);

  emit_uint32(o, proc->length());
  proc->emit(o);
}

// ConditionalExpr

ConditionalExpr::ConditionalExpr(Expr *_condition, Procedure *_on_true, Procedure *_on_false): condition(_condition), on_true(_on_true), on_false(_on_false)
{ }

ConditionalExpr::~ConditionalExpr()
{
  delete condition;
  delete on_true;
  delete on_false;
}

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
    emit_uint32(o, on_true->length() + 1 + sizeof(int32));

  on_true->emit(o);

  if (on_false) {
    emit_instruction(o, I_JMP);
    emit_int32(o, on_false->length());
    on_false->emit(o);
  }
}

void ConditionalExpr::push(std::ostream &o) const
{
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// WhileExpr

WhileExpr::WhileExpr(Expr *_condition, Procedure *_action): condition(_condition), action(_action)
{ }

WhileExpr::~WhileExpr()
{
  delete condition; delete action;
}

void WhileExpr::p() const
{
  std::cout << "while (";
  condition->p();
  std::cout << ")" << std::endl << "  ";
  action->p();
  std::cout << std::endl << "end" << std::endl;
}

void WhileExpr::emit(std::ostream &o) const
{
  long while_begin = (long)o.tellp();

  condition->push(o);

  emit_instruction(o, I_IF);
  // If the condition's false, we need to jump the action length + the jump back.
  emit_uint32(o, action->length() + 1 + sizeof(int32));

  action->emit(o);

  // The jump will be considered at the point immediately after the jump length.
  // We can't calculate it ourself because condition->push could be any length.
  // We work out where the first command after the jump is (where the jump will
  // start from), and then go back to while_begin from there.

  // It's here + the I_JMP and arg.
  long right_after_jmp =
    (long)o.tellp() + 1 + sizeof(int32);
  emit_instruction(o, I_JMP);
  emit_int32(o, while_begin - right_after_jmp);
}

// BeginSectionExpr

BeginSectionExpr::BeginSectionExpr(BlockExpr *_main_clause, Rescue *_rescue, BlockExpr *_else_clause, BlockExpr *_ensure_clause): main_clause(_main_clause), rescue(_rescue), else_clause(_else_clause), ensure_clause(_ensure_clause)
{ }

BeginSectionExpr::~BeginSectionExpr() {
  delete main_clause;
  delete rescue;
  delete else_clause;
  delete ensure_clause;
}

void BeginSectionExpr::p() const {
  std::cout << "begin" << std::endl << "  ";
  main_clause->p();
  std::cout << std::endl;

  if (rescue) rescue->p();

  if (else_clause) {
    std::cout << "else" << std::endl << "  ";
    else_clause->p();
    std::cout << std::endl;
  }

  if (ensure_clause) {
    std::cout << "ensure" << std::endl << "  ";
    ensure_clause->p();
    std::cout << std::endl;
  }

  std::cout << "end";
}

void BeginSectionExpr::emit(std::ostream &o) const {
  // if we have a rescue or ensure part, we add it to the VM stack.
  if (!rescue && !ensure_clause) {
    // Nothing special. The return value is the last thing in `main', or `else' if there is one.
    main_clause->proc->emit(o);
    if (else_clause)
      else_clause->proc->emit(o);
  } else {
    // needs to be blocked.
    if (ensure_clause)
      ensure_clause->push(o);
    if (else_clause)
      else_clause->push(o);
    if (rescue) {
      for (std::list<IdentifierExpr *>::const_reverse_iterator it = rescue->exceptions.rbegin();
	it != rescue->exceptions.rend(); ++it)
	(*it)->push(o);
      rescue->clause->push(o);
    }

    main_clause->push(o);

    emit_instruction(o, I_EXCEPTION_BLOCK);
    emit_uint8(o, (rescue ? E_RESCUE : 0) | (ensure_clause ? E_ENSURE : 0) | (else_clause ? E_ELSE : 0));
    emit_uint8(o, rescue ? rescue->exceptions.size() : 0);
  }
}

void BeginSectionExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// Rescue

Rescue::Rescue(IdListExpr *_exceptions, IdentifierExpr *_save_to, BlockExpr *_clause): clause(_clause) {
  if (_exceptions) {
    exceptions = _exceptions->args;
    delete _exceptions;
  }

  if (_save_to)
    clause->args.push_back(_save_to);
}

Rescue::~Rescue() {
  for (std::list<IdentifierExpr *>::iterator it = exceptions.begin(); it != exceptions.end(); ++it)
    delete *it;
  delete clause;
}

void Rescue::p() const {
  std::cout << "rescue";

  if (exceptions.size() > 0) {
    std::cout << " ";
    for (std::list<IdentifierExpr *>::const_iterator it = exceptions.begin(); it != exceptions.end(); ++it) {
      if (it != exceptions.begin()) std::cout << ", ";
      (*it)->p();
    }
  }
  if (clause->args.size() == 1) {
    std::cout << " => ";
    (*clause->args.begin())->p();
  }

  std::cout << std::endl << "  ";
  clause->proc->p();

  std::cout << std::endl;
}

// InterpolateExpr

InterpolateExpr::InterpolateExpr()
{ }

InterpolateExpr::~InterpolateExpr() {
  for (std::list<InterpolateExpr::_int_base *>::iterator it = data.begin(); it != data.end(); ++it)
    delete *it;
}

void InterpolateExpr::append(StringLiteralExpr *_str) {
  data.push_back(new _int_str(_str));
}

void InterpolateExpr::append(Procedure *_proc) {
  data.push_back(new _int_proc(_proc));
}

void InterpolateExpr::p() const {
  std::cout << '\"';
  for (std::list<InterpolateExpr::_int_base *>::const_iterator it = data.begin(); it != data.end(); ++it)
    (*it)->p();
  std::cout << '\"';
}

void InterpolateExpr::emit(std::ostream &o) const {
  // push them on reverse.
  for (std::list<InterpolateExpr::_int_base *>::const_reverse_iterator it = data.rbegin(); it != data.rend(); ++it)
    (*it)->push(o);
  emit_instruction(o, I_INTERPOL);
  emit_uint32(o, data.size());
}

void InterpolateExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

InterpolateExpr::_int_base::~_int_base()
{ }

InterpolateExpr::_int_str::_int_str(StringLiteralExpr *_str): str(_str)
{ }

InterpolateExpr::_int_str::~_int_str()
{
  delete str;
}

void InterpolateExpr::_int_str::p() const {
  std::cout << str->value;
}

void InterpolateExpr::_int_str::push(std::ostream &o) const {
  str->push(o);
}

InterpolateExpr::_int_proc::_int_proc(Procedure *_proc): proc(_proc)
{ }

InterpolateExpr::_int_proc::~_int_proc()
{
  delete proc;
}

void InterpolateExpr::_int_proc::p() const {
  std::cout << "#{";
  proc->p();
  std::cout << "}";
}

void InterpolateExpr::_int_proc::push(std::ostream &o) const {
  proc->emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// ScopeExpr

ScopeExpr::ScopeExpr(Expr *_context, IdentifierExpr *_constant): context(_context), constant(_constant)
{ }

void ScopeExpr::p() const {
  if (context) {
    std::cout << "(";
    context->p();
    std::cout << ")";
  }
  std::cout << "::";
  constant->p();
}

void ScopeExpr::emit(std::ostream &o) const {
  if (context)
    context->push(o);
  emit_instruction(o, context ? I_SCOPE_CONTEXT : I_SCOPE_ROOT);
  emit_string(o, constant->id);
}

void ScopeExpr::push(std::ostream &o) const {
  emit(o);
  emit_instruction(o, I_PUSH_LAST);
}

// Program

void Program::add_expression(Expr *expression) {
  if (expression) {
    if (emit_as_we_go) {
      emitted_yet = true;
      expression->emit(*emitter_stream);
      delete expression;
    } else
      this->expressions.push_back(expression);
  }
}

Program::Program(): emit_as_we_go(false)
{ }

Program::Program(std::ostream &o): emit_as_we_go(true), emitted_yet(false), emitter_stream(&o)
{ }

Program::~Program()
{
  for (std::list<Expr *>::iterator it = expressions.begin(); it != expressions.end(); ++it)
    delete *it;
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

