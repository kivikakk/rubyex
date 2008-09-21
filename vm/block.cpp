#include <sstream>
#include <algorithm>
#include "block.h"
#include "reader.h"
#include "context.h"
#include "process.h"

Block::Block(RubyModule *_def_target, Context *_caller_context, Block *_caller_block): has_splat(false), def_target(_def_target), caller_context(_caller_context), caller_block(_caller_block)
{ }

Block::~Block() {
  for (std::vector<BlockArg *>::iterator it = args.begin(); it != args.end(); ++it)
    delete *it;
}

RubyValue Block::call(linked_ptr<Binding> &_b)
{ return call(_b, std::vector<RubyValue>()); }

RubyValue Block::call(linked_ptr<Binding> &_b, RubyValue _arg1)
{
  std::vector<RubyValue> args;
  args.push_back(_arg1);

  return call(_b, args);
}

RubyValue Block::call(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args)
{
  return call(_b, _b->context, _args);
}

RubyValue Block::call(linked_ptr<Binding> &_b, RubyValue _context, const std::vector<RubyValue> &_args)
{
  std::istringstream iss(this->code);
  Reader r = Reader(iss);

  // N.B. new Context's `self' is the same as the one from outside.
  // Hence `self' in a block still refers to the `self' from outside,
  // which makes sense.
  Context *c = new Context(_b->environment, _context, def_target, caller_context);

  // We now create the block variable arguments in this context.
  // In true MRI fashion, we ignore extra arguments that aren't being used
  // in the block, and add `nil's if we haven't enough.
  assign_args(_b, c, _args);

  // we use caller_block for the Block, if one's been given.
  RubyValue ret_val = process(_b->environment, r, c, caller_block);

  delete c;

  return ret_val;
}

// Runs the call in a given context. Useful if it's technically not a block. (great, I know.)
RubyValue Block::call(Context *_c)
{
  return call(_c, _c->binding, std::vector<RubyValue>());
}

RubyValue Block::call(Context *_c, const std::vector<RubyValue> &_args)
{
  return call(_c, _c->binding, _args);
}

RubyValue Block::call(Context *_c, linked_ptr<Binding> &_b)
{
  return call(_c, _b, std::vector<RubyValue>());
}

RubyValue Block::call(Context *_c, linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args)
{
  std::istringstream iss(this->code);
  Reader r = Reader(iss);

  assign_args(_b, _c, _args);

  // we use caller_block for the Block, if one's been given.
  return process(_b->environment, r, _c, caller_block);
}

RubyValue Block::call(linked_ptr<Binding> &_b, const std::vector<RubyValue> &_args, Block &_block)
{
  return call(_b, _b->context, _args, _block);
}

RubyValue Block::call(linked_ptr<Binding> &_b, RubyValue _context, const std::vector<RubyValue> &_args, Block &_block)
{
  std::istringstream iss(this->code);
  Reader r = Reader(iss);

  Context *c = new Context(_b->environment, _context, def_target, caller_context);
  assign_args(_b, c, _args);

  RubyValue ret_val = process(_b->environment, r, c, &_block);

  delete c;
  return ret_val;
}

void Block::assign_args(linked_ptr<Binding> &_b, Context *_c, const std::vector<RubyValue> &_args) const
{
  unsigned int given_args_to_add = std::min(this->args.size(), _args.size());
  for (unsigned int i = 0; i < given_args_to_add; ++i)
    _c->assign(this->args[i]->name, _args[i]);
  
  if (given_args_to_add < this->args.size())
    for (unsigned int i = given_args_to_add; i < this->args.size(); ++i)
      _c->assign(this->args[i]->name, this->args[i]->default_code ? this->args[i]->default_code->call(_c, _b) : _b->environment.NIL);
}

BlockArg::BlockArg(const std::string &_name): name(_name), default_code(NULL)
{ }

BlockArg::BlockArg(Block *_parent, const std::string &_name, const std::string &_code) {
  default_code = new Block(_parent->def_target, NULL /* taken at runtime */, NULL);
  default_code->code = _code;
}

BlockArg::~BlockArg() {
  delete default_code;
}

