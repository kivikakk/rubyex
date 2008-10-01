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
    RubyRegexp(RubyEnvironment &, const std::string &);
    virtual ~RubyRegexp();
    
    regex_t *reg;
};

#endif

