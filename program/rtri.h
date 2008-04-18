#ifndef RTRI_H
#define RTRI_H

#include "rei.h"

class RubyTriEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

