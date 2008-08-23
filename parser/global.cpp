#include "global.h"
#include <iostream>

int context_depths = 0, context_lines = 0;
bool is_block_mode = false;

std::vector<brace_t> braces_stack;

void set_block(bool _is)
{
  is_block_mode = _is;
}

void enter_context()
{
  ++context_depths;
  IF_DEBUG std::cout << "Entering context (new depth: " << context_depths << ")" << std::endl;
}

void exit_context()
{
  --context_depths;
  IF_DEBUG std::cout << "Exiting context (new depth: " << context_depths << ")" << std::endl;
}

void enter_context_line()
{
  ++context_lines;
  IF_DEBUG std::cout << "Entering context line (lines now: " << context_lines << ")" << std::endl;
}

void exit_context_line()
{
  --context_lines;
  IF_DEBUG std::cout << "Exiting context line (lines now: " << context_lines << ")" << std::endl;
}

bool in_context()
{
  return (context_depths > 0);
}

void enter_brace(brace_t b)
{
  if (b == B_HASH)
    std::cerr << "entering B_HASH" << std::endl;
  else
    std::cerr << "entering B_BLOCK" << std::endl;
  braces_stack.push_back(b);
}

bool last_brace_is(brace_t b)
{
  if (braces_stack.size() == 0)
    return false;
  return *braces_stack.rbegin() == b;
}

brace_t pop_last_brace()
{
  if (braces_stack.size() == 0) {
    std::cerr << "tried to pop_last_brace(), nothing to pop, assuming hash" << std::endl;
    return B_HASH;
  }

  brace_t b = *braces_stack.rbegin();
  braces_stack.pop_back();
  if (b == B_HASH)
    std::cerr << "popping B_HASH" << std::endl;
  else
    std::cerr << "popping B_BLOCK" << std::endl;

  return b;
}

std::string filter_underscores(const std::string &_in)
{
  std::string s = _in;
  unsigned long idx;
  while ((idx = s.find('_')) != std::string::npos)
    s.erase(idx, 1);
  return s;
}

