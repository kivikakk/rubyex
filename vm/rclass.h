#ifndef RCLASS_H
#define RCLASS_H

#include <exception>
#include "rmodule.h"
#include "rei.h"

class RubyClass : public RubyModule
{
  public:
    RubyClass(RubyEnvironment &, const std::string &);
    RubyClass(RubyEnvironment &, const std::string &, RubyClass *);
    RubyClass(RubyEnvironment &, const std::string &, RubyClass *, RubyModule *);

    const linked_ptr<RubyMethod> &find_method(const std::string &) const;
    bool has_ancestor(RubyClass *) const;

    RubyObject *new_instance(RubyEnvironment &);

    RubyClass *superklass;
};

class ClassHasNoSuchMethodException : public std::exception
{ };

class RubyClassEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

