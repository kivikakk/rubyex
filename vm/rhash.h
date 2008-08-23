#ifndef RHASH_H
#define RHASH_H

#include "rei.h"
#include "robject.h"
#include <map>

class RubyHashEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyHash : public RubyObject
{
  public:
    RubyHash(RubyEnvironment &);
    RubyHash(RubyEnvironment &, RubyValue);
    RubyHash(RubyEnvironment &, const std::vector<RubyValue> &);

    void set(linked_ptr<Binding> &, RubyValue, RubyValue);
    RubyValue get(linked_ptr<Binding> &, RubyValue);

    typedef std::map<RubyValue, RubyValue> internal_t;

    internal_t::iterator begin();
    internal_t::iterator end();
    
  protected:
    internal_t data;
    RubyValue default_value;

    internal_t::iterator matching_key(linked_ptr<Binding> &, RubyValue);
};

#endif
