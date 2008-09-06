#ifndef RCLASS_H
#define RCLASS_H

#include <exception>
#include "rmodule.h"
#include "rei.h"

class RubyClass : public RubyModule
{
  public:
    static RubyClass *create_class(RubyEnvironment &, const std::string &);
    static RubyClass *create_class_with_super(RubyEnvironment &, const std::string &, RubyClass *);

    RubyMethod *find_method(const std::string &) const;
    bool has_ancestor(RubyClass *) const;

    RubyObject *new_instance(RubyEnvironment &);

    RubyClass *superklass;

  protected:
    explicit RubyClass(RubyEnvironment &, RubyClass *, const std::string &);
};

class ClassHasNoSuchMethodException : public std::exception
{ };

class RubyClassEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

