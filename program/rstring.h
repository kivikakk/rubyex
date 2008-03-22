#ifndef STRING_H
#define STRING_H

#include "rei.h"

class RubyStringEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

