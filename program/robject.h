#ifndef ROBJECT_H
#define ROBJECT_H

#include <map>
#include <string>
#include "rvalue.h"
#include "lazyclass.h"
#include "rei.h"

class RubyClass;

class RubyObject
{
  public:
    RubyObject(LazyClass *);
    virtual ~RubyObject();

    RubyClass *get_class() const;
    RubyClass *get_metaclass() const;

  protected:
    LazyClass *klass;
    RubyClass *metaklass;

    std::map<std::string, RubyValue> instance_variables;
};

class RubyObjectEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

