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

    void add_metaclass_method(RubyEnvironment &, const std::string &, RubyMethod *);

    RubyClass *get_class() const;
    RubyClass *get_metaclass_read() const;

    RubyClass *get_metaclass(RubyEnvironment &_e);

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

