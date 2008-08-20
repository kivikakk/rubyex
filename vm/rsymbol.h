#ifndef RSYMBOL_H
#define RSYMBOL_H

#include <string>
#include "rei.h"


class RubySymbolEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};


class RubySymbol
{
  public:
    RubySymbol(const std::string &);

    const std::string value;
};

#endif

