#include <iostream>
#include "stack.h"

Stack::Stack()
{ }

void Stack::push_identifier(const std::string &identifier)
{
  StackEntry e;
  e.type = T_IDENTIFIER;
  e.identifier = new std::string(identifier);
  ival.push_back(e);
}

void Stack::push_symbol(const std::string &symbol)
{
  StackEntry e;
  e.type = T_SYMBOL;
  e.symbol = new std::string(symbol);
  ival.push_back(e);
}

void Stack::push_integer(int integer)
{
  StackEntry e;
  e.type = T_INTEGER_LITERAL;
  e.integer = integer;
  ival.push_back(e);
}

void Stack::push_floating(double floating)
{
  StackEntry e;
  e.type = T_FLOATING_LITERAL;
  e.floating = floating;
  ival.push_back(e);
}

void Stack::push_boolean(bool boolean)
{
  StackEntry e;
  e.type = T_BOOLEAN_LITERAL;
  e.boolean = boolean;
  ival.push_back(e);
}

void Stack::push_string(const std::string &string)
{
  StackEntry e;
  e.type = T_STRING_LITERAL;
  e.string = new std::string(string);
  ival.push_back(e);
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
  }

  std::cerr << "Unknown stack entry type: " << top.type << std::endl;
  throw std::exception();
}
