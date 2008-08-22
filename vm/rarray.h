#ifndef RARRAY_H
#define RARRAY_H

#include "rei.h"
#include "robject.h"
#include <vector>

class RubyArrayEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyArray : public RubyObject
{
  public:
    RubyArray(RubyEnvironment &);
    RubyArray(RubyEnvironment &, long);
    RubyArray(RubyEnvironment &, long, RubyValue);

    std::vector<RubyValue> data;
};

#endif
