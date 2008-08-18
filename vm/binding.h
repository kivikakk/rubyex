#ifndef BINDING_H
#define BINDING_H

#include <map>
#include <string>
#include "rvalue.h"

class Binding
{
  public:
    Binding(RubyValue);
    ~Binding();

    RubyValue get_context() const;

    std::map<std::string, RubyValue> locals;

  protected:
    RubyValue context;
};

#endif
