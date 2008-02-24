#ifndef RKERNEL_H
#define RKERNEL_H

#include "renvironment.h"

class RubyKernel : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif


