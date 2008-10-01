#ifndef RREGEXP_H
#define RREGEXP_H

#include <oniguruma.h>
#include "rei.h"
#include "robject.h"
#include "renvironment.h"
#include "rmethod.h"

class RubyRegexpEI : public RubyEnvironmentInitializer
{
  public:
   void init(RubyEnvironment &);
};

class RubyRegexp : public RubyObject
{
  public:
    RubyRegexp(RubyEnvironment &);
    RubyRegexp(linked_ptr<Binding> &, const std::string &);
    virtual ~RubyRegexp();

    void initialize(linked_ptr<Binding> &, const std::string &);
    
    regex_t *reg;
    std::string phrase;
};

#endif

