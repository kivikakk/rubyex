#ifndef ROBJECT_H
#define ROBJECT_H

#include <map>
#include <string>
#include "rvalue.h"
#include "lazyclass.h"

class RubyClass;

class RubyObject
{
  public:
    RubyObject(LazyClass *);
    virtual ~RubyObject();

  protected:
    LazyClass *klass;
    RubyClass *metaklass;

    std::map<std::string, RubyValue *> instance_variables;
};

#endif

