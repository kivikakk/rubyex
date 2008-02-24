#ifndef STRING_H
#define STRING_H

#include "renvironment.h"

class RubyString : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

#endif

