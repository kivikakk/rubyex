#include <iostream>
#include "stack.h"
#include "context.h"

Stack::Stack()
{ }

void Stack::push_identifier(const std::string &identifier)
{
  StackEntry e;
  e.type = SE_IDENTIFIER;
  e.identifier = new std::string(identifier);
  push(e);
}

void Stack::push_symbol(const std::string &symbol)
{
  StackEntry e;
  e.type = SE_SYMBOL;
  e.symbol = new std::string(symbol);
  push(e);
}

void Stack::push_integer(int integer)
{
  StackEntry e;
  e.type = SE_INTEGER;
  e.integer = integer;
  push(e);
}

void Stack::push_block(const Block &block)
{
  StackEntry e;
  e.type = SE_BLOCK;
  e.block = new Block(block);
  push(e);
}

void Stack::push_object(RubyObject *object)
{
  StackEntry e;
  e.type = SE_OBJECT;
  e.object = object;
  push(e);
}

void Stack::push(const StackEntry &se)
{
  ival.push_back(se);
}

Stack::StackEntry Stack::pop_variant()
{
  StackEntry r = *ival.begin();
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

RubyValue Stack::pop_value(Context *c)
{
  return c->entry_to_value(pop_variant());
}

