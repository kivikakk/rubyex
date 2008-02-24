#ifndef ROBJECT_H
#define ROBJECT_H

#include <map>
#include <string>
#include "rvalue.h"

class RubyObject
{
  public:
    RubyObject();
    virtual ~RubyObject();

  protected:
    std::map<std::string, RubyValue *> instance_variables;
};

#endif

