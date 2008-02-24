#ifndef RCLASS_H
#define RCLASS_H

#include "rmodule.h"

class RubyClass : public RubyModule
{
  public:
    RubyClass(const std::string &);
};

#endif

