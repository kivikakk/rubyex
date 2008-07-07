#ifndef RNUMERIC_H
#define RNUMERIC_H

#include "rei.h"
#include "robject.h"

class RubyNumericEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyFloating : public RubyObject
{
  public:
    RubyFloating(RubyEnvironment &, double);

  protected:
    double floating_value;
};

#endif

