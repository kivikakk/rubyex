#ifndef RBINDING_H
#define RBINDING_H

#include "rei.h"
#include "robject.h"
#include "binding.h"

class RubyBindingEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyBinding : public RubyObject
{
  public:
    RubyBinding(linked_ptr<Binding> &);

    linked_ptr<Binding> binding;
};

#endif

