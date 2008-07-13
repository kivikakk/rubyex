#include "yywrap.h"
#include "irb.h"
#include <iostream>
#include <exception>
#include "parser/global.h"

ruby_yywrap_delegate_t ruby_yywrap_delegate;

extern "C" int yywrap(void)
{
  if (ruby_yywrap_delegate == NULL) {
    std::cerr << "no yywrap delegate installed" << std::endl;
    throw std::exception();
  }

  return ruby_yywrap_delegate();
}

