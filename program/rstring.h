#ifndef RSTRING_H
#define RSTRING_H

#include "rei.h"
#include "robject.h"

class RubyStringEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyString : public RubyObject
{
  public:
    RubyString(const std::string &);

  protected:
    std::string string_value;
};

#endif

