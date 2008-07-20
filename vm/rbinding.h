#ifndef RBINDING_H
#define RBINDING_H

#include "rei.h"
#include "robject.h"

class RubyBindingEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyBinding : public RubyObject
{
  public:
    RubyBinding(RubyEnvironment &);
};

#endif

