#ifndef RCLASS_H
#define RCLASS_H

#include "rmodule.h"
#include "lazyclass.h"

class RubyClass : public RubyModule, public LazyClass
{
  public:
    RubyClass(RubyEnvironment &, LazyClass *, const std::string &);

    static RubyClass *create_class(RubyEnvironment &, const std::string &);
    static RubyClass *create_class_with_super(RubyEnvironment &, const std::string &, LazyClass *);

  protected:
    LazyClass *superklass;
};

#endif

