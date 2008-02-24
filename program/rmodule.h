#ifndef RMODULE_H
#define RMODULE_H

#include <string>
#include <map>
#include "robject.h"
#include "rmethod.h"

class RubyModule : public RubyObject
{
  public:
    RubyModule(const std::string &);
    virtual ~RubyModule();

    void add_method(const std::string &, RubyMethod *);

    const std::string &get_name() const;

  protected:
    std::string name;
    std::map<std::string, RubyMethod *> methods;
};

#endif

