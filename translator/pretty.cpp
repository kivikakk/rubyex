#include "pretty.h"

PrettyPrint::~PrettyPrint() { }

std::string PrettyPrint::p_tabs(int tabs) const {
  std::string m;
  for (int i = 0; i < tabs; ++i)
    m += "  ";
  return m;
}

