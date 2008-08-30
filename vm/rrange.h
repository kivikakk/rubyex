#ifndef RRANGE_H
#define RRANGE_H

#include "rei.h"

class RubyRangeEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

