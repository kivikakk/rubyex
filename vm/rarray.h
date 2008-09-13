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
    RubyArray(RubyEnvironment &, const std::vector<RubyValue> &);

    RubyValue index(linked_ptr<Binding> &, RubyValue) const;
    bool include(linked_ptr<Binding> &, RubyValue) const;

    std::vector<RubyValue> data;
};

#endif
