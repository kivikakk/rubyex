#ifndef RMODULE_H
#define RMODULE_H

#include <string>
#include <map>
#include <list>
#include "robject.h"

class RubyMethod;

class RubyModule : public RubyObject
{
  public:
    RubyModule(RubyEnvironment &, const std::string &);
    RubyModule(LazyClass *, const std::string &);
    virtual ~RubyModule();

    void add_method(const std::string &, RubyMethod *);
    void add_module_method(RubyEnvironment &, const std::string &, RubyMethod *);
    void include_module(RubyModule *);

    const std::string &get_name() const;
    bool has_method(const std::string &) const;
    RubyMethod *get_method(const std::string &) const;

  protected:
    std::string name;
    std::map<std::string, RubyMethod *> methods;
    std::list<RubyModule *> includes;
};

class RubyModuleEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

