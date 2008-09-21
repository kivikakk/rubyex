#ifndef ROBJECT_H
#define ROBJECT_H

#include <map>
#include <string>
#include "rvalue.h"
#include "rei.h"

class RubyClass;

class RubyObject
{
  public:
    RubyObject(RubyClass *);
    virtual ~RubyObject();

    void add_metaclass_method(RubyEnvironment &, const std::string &, const linked_ptr<RubyMethod> &);

    void set_class(RubyClass *);

    RubyClass *get_class() const;
    RubyClass *get_metaclass_read() const;

    bool has_instance(const std::string &) const;
    RubyValue get_instance(const std::string &) const;
    void set_instance(const std::string &, RubyValue);

    RubyClass *get_metaclass(RubyEnvironment &);

  protected:
    RubyClass *klass;
    RubyClass *metaklass;

    std::map<std::string, RubyValue> instance_variables;
};

class RubyObjectEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

