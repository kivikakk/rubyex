#include "global.h"
#include <iostream>

int context_depths = 0, context_lines = 0;

bool string_lit_rs = false, backtick_lit_rs = false;

void restart_string_literal() {
  string_lit_rs = true;
}

void restart_backtick_literal() {
  backtick_lit_rs = true;
}

void enter_context() {
  ++context_depths;
  IF_DEBUG std::cout << "Entering context (new depth: " << context_depths << ")" << std::endl;
}

void exit_context() {
  --context_depths;
  IF_DEBUG std::cout << "Exiting context (new depth: " << context_depths << ")" << std::endl;
}

void enter_context_line() {
  ++context_lines;
  IF_DEBUG std::cout << "Entering context line (lines now: " << context_lines << ")" << std::endl;
}

void exit_context_line() {
  --context_lines;
  IF_DEBUG std::cout << "Exiting context line (lines now: " << context_lines << ")" << std::endl;
}

bool in_context() {
  return (context_depths > 0);
}

std::string filter_underscores(const std::string &_in) {
  std::string s = _in;
  unsigned long idx;
  while ((idx = s.find('_')) != std::string::npos)
    s.erase(idx, 1);
  return s;
}

