#ifndef BINDING_H
#define BINDING_H

#include <map>
#include <string>
#include "renvironment.h"
#include "rvalue.h"

class Binding
{
  public:
    Binding(RubyEnvironment *, RubyValue);
    ~Binding();

    RubyEnvironment *get_environment() const;
    RubyValue get_context() const;

    std::map<std::string, RubyValue> locals;

  protected:
    RubyEnvironment *environment;
    RubyValue context;
};

#endif
