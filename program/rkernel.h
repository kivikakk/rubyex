#ifndef RKERNEL_H
#define RKERNEL_H

#include "renvironment.h"
#include "rei.h"

class RubyKernelEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif


