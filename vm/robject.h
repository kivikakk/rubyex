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

    void set_class(LazyClass *);

    RubyClass *get_class() const;
    RubyClass *get_metaclass_read() const;

    RubyValue get_instance(RubyEnvironment &, const std::string &) const;
    void set_instance(RubyEnvironment &, const std::string &, RubyValue);

    RubyClass *get_metaclass(RubyEnvironment &);

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

