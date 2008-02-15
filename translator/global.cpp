#include "global.h"
#include <iostream>

int block_depths = 0, block_lines = 0;

void enter_block()
{
  ++block_depths;
  IF_DEBUG std::cout << "Entering block (new depth: " << block_depths << ")" << std::endl;
}

void exit_block()
{
  --block_depths;
  IF_DEBUG std::cout << "Exiting block (new depth: " << block_depths << ")" << std::endl;
}

void enter_block_line()
{
  ++block_lines;
  IF_DEBUG std::cout << "Entering block line (lines now: " << block_lines << ")" << std::endl;
}

void exit_block_line()
{
  --block_lines;
  IF_DEBUG std::cout << "Exiting block line (lines now: " << block_lines << ")" << std::endl;
}

bool in_block()
{
  return (block_depths > 0);
}

