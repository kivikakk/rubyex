#ifndef RCLASS_H
#define RCLASS_H

#include "rmodule.h"
#include "lazyclass.h"
#include "rei.h"

class RubyClass : public RubyModule, public LazyClass
{
  public:
    static RubyClass *create_class(RubyEnvironment &, const std::string &);
    static RubyClass *create_class_with_super(RubyEnvironment &, const std::string &, LazyClass *);

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

