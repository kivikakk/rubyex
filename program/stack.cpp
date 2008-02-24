#include <iostream>
#include "stack.h"

Stack::Stack()
{ }

void Stack::push_identifier(const std::string &identifier)
{
  StackEntry e;
  e.type = T_IDENTIFIER;
  e.identifier = new std::string(identifier);
  push(e);
}

void Stack::push_symbol(const std::string &symbol)
{
  StackEntry e;
  e.type = T_SYMBOL;
  e.symbol = new std::string(symbol);
  push(e);
}

void Stack::push_integer(int integer)
{
  StackEntry e;
  e.type = T_INTEGER_LITERAL;
  e.integer = integer;
  push(e);
}

void Stack::push_floating(double floating)
{
  StackEntry e;
  e.type = T_FLOATING_LITERAL;
  e.floating = floating;
  push(e);
}

void Stack::push_boolean(bool boolean)
{
  StackEntry e;
  e.type = T_BOOLEAN_LITERAL;
  e.boolean = boolean;
  push(e);
}

void Stack::push_string(const std::string &string)
{
  StackEntry e;
  e.type = T_STRING_LITERAL;
  e.string = new std::string(string);
  push(e);
}

void Stack::push_block(const Block &block)
{
  StackEntry e;
  e.type = T_BLOCK;
  e.block = new Block(block);
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
    case T_IDENTIFIER: { std::string id = *top.identifier; delete top.identifier; return dynamic_cast<T>(id); }
    case T_SYMBOL: { std::string sym = *top.symbol; delete top.symbol; return dynamic_cast<T>(sym); }
    case T_INTEGER_LITERAL: return dynamic_cast<T>(top.integer);
    case T_FLOATING_LITERAL: return dynamic_cast<T>(top.floating);
    case T_BOOLEAN_LITERAL: return dynamic_cast<T>(top.boolean);
    case T_STRING_LITERAL: { std::string str = *top.string; delete top.string; return dynamic_cast<T>(str); }
    case T_BLOCK: { Block block = *top.block; delete top.block; return dynamic_cast<T>(block); }
  }

  std::cerr << "Unknown stack entry type: " << top.type << std::endl;
  throw std::exception();
}
