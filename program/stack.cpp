#include <iostream>
#include "stack.h"
#include "context.h"

Stack::Stack()
{ }

void Stack::push_identifier(const std::string &_identifier)
{
  StackEntry e;
  e.type = SE_IDENTIFIER;
  e.identifier = new std::string(_identifier);
  push(e);
}

void Stack::push_symbol(const std::string &_symbol)
{
  StackEntry e;
  e.type = SE_SYMBOL;
  e.symbol = new std::string(_symbol);
  push(e);
}

void Stack::push_integer(int _integer)
{
  StackEntry e;
  e.type = SE_INTEGER;
  e.integer = _integer;
  push(e);
}

void Stack::push_block(const Block &_block)
{
  StackEntry e;
  e.type = SE_BLOCK;
  e.block = new Block(_block);
  push(e);
}

void Stack::push_object(RubyObject *_object)
{
  StackEntry e;
  e.type = SE_OBJECT;
  e.object = _object;
  push(e);
}

void Stack::push(const StackEntry &_se)
{
  ival.push_back(_se);
}

Stack::StackEntry Stack::pop_variant()
{
  StackEntry r = *ival.rbegin();
  ival.pop_back();
  return r;
}

std::string Stack::pop_identifier()
{
  StackEntry top = pop_variant();
  std::string id = *top.identifier;
  delete top.identifier;
  return id;
}

std::string Stack::pop_symbol()
{
  StackEntry top = pop_variant();
  std::string sym = *top.symbol;
  delete top.symbol;
  return sym;
}

int Stack::pop_integer()
{
  return pop_variant().integer;
}

Block Stack::pop_block()
{
  StackEntry top = pop_variant();
  Block block = *top.block;
  delete top.block;
  return block;
}

RubyObject *Stack::pop_object()
{
  return pop_variant().object;
}

RubyValue Stack::pop_value(Context *_c)
{
  // entry_to_value deallocates memory allocated in the StackEntry
  // (TODO: if we made StackEntry a class, used a heap-allocated var
  // and had the destructor do it, that'd be great...)
  return _c->entry_to_value(pop_variant());
}

