#ifndef RMODULE_H
#define RMODULE_H

#include <string>
#include <map>
#include <vector>
#include "robject.h"

class RubyMethod;

class RubyModule : public RubyObject
{
  public:
    RubyModule(RubyEnvironment &, const std::string &);
    RubyModule(RubyEnvironment &, LazyClass *, const std::string &);
    virtual ~RubyModule();

    void add_method(const std::string &, RubyMethod *);
    void include_module(RubyModule *);

    const std::string &get_name() const;
    RubyMethod *get_method(const std::string &) const;

  protected:
    std::string name;
    std::map<std::string, RubyMethod *> methods;
    std::vector<RubyModule *> includes;
};

class RubyModuleEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

