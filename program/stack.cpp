#include <iostream>
#include "stack.h"

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

template <typename T> T Stack::pop()
{
  StackEntry top = *ival.rbegin();
  ival.pop_back();

  switch (top.type) {
    case SE_IDENTIFIER: { std::string id = *top.identifier; delete top.identifier; return dynamic_cast<T>(id); }
    case SE_SYMBOL: { std::string sym = *top.symbol; delete top.symbol; return dynamic_cast<T>(sym); }
    case SE_INTEGER: return dynamic_cast<T>(top.integer);
    case SE_BLOCK: { Block block = *top.block; delete top.block; return dynamic_cast<T>(block); }
    case SE_OBJECT: { RubyObject *object = top.object; return dynamic_cast<T>(object); }
  }

  std::cerr << "Unknown stack entry type: " << top.type << std::endl;
  throw std::exception();
}
