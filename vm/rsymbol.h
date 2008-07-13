#ifndef RSYMBOL_H
#define RSYMBOL_H

#include <string>

class RubySymbol
{
  public:
    RubySymbol(const std::string &);

    const std::string &get_value() const;

  protected:
    std::string value;
};

#endif

