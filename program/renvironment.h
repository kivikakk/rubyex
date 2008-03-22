#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
#include "rclass.h"
#include "rmodule.h"

class RubyEnvironment
{
  public:
    RubyEnvironment();

    static RubyValue *NIL, *TRUE, *FALSE;
    // XXX: question; does having these global disable the possible
    // reentrancy of instantiating multiple RubyEnvironments?

    void add_class(const std::string &, RubyClass *);
    void add_module(const std::string &, RubyModule *);

    RubyClass *get_class_by_name(const std::string &);
    RubyModule *get_module_by_name(const std::string &);

    RubyClass *rb_cObject, *rb_cModule, *rb_cClass;

  protected:
    std::map<std::string, RubyClass *> classes;
    std::map<std::string, RubyModule *> modules;
};

class RubyEnvironmentInitializer
{
  public:
    virtual ~RubyEnvironmentInitializer();

    virtual void init(RubyEnvironment &) = 0;
};

#endif

