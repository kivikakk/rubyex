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
    RubyString(RubyEnvironment &, const std::string &);

    std::string string_value;
};

#endif

