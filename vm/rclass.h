#ifndef RCLASS_H
#define RCLASS_H

#include "rmodule.h"
#include "lazyclass.h"
#include "rei.h"

// RubyClass implements LazyClass itself, so as to be compatible with the "LazyClass protocol"
// per se. (i.e. it can be used to resolve itself without issue when a LazyClass is expected)
// .. has that even been implemented yet?

class RubyClass : public RubyModule, public LazyClass
{
  public:
    static RubyClass *create_class(RubyEnvironment &, const std::string &);
    static RubyClass *create_class_with_super(RubyEnvironment &, const std::string &, LazyClass *);

    RubyMethod *find_method(const std::string &) const;

    RubyObject *new_instance(RubyEnvironment &);

  protected:
    LazyClass *superklass;

    explicit RubyClass(RubyEnvironment &, LazyClass *, const std::string &);
};

class RubyClassEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

