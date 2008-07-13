#include "global.h"
#include <iostream>

int context_depths = 0, context_lines = 0;

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

