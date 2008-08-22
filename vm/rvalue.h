#ifndef RVALUE_H
#define RVALUE_H

#include <string>
#include <vector>
#include "rsymbol.h"
#include "linked_ptr.h"

class RubyEnvironment;
class RubyObject;
class RubyMethod;
class RubyClass;
class Binding;

class RubyValue
{
  public:
    RubyValue();

    static RubyValue from_fixnum(long);
    static RubyValue from_symbol(RubySymbol *);
    static RubyValue from_object(RubyObject *);

    RubyClass *get_class(RubyEnvironment &_e) const;
    RubyMethod *get_method(const std::string &, RubyEnvironment &) const;

    bool truthy(RubyEnvironment &) const;

    RubyValue call(linked_ptr<Binding> &, const std::string &) const;
    RubyValue call(linked_ptr<Binding> &, const std::string &, RubyValue) const;
    RubyValue call(linked_ptr<Binding> &, const std::string &, RubyValue, RubyValue) const;
    RubyValue call(linked_ptr<Binding> &, const std::string &, RubyValue, RubyValue, RubyValue) const;
    RubyValue call(linked_ptr<Binding> &, const std::string &, const std::vector<RubyValue> &) const;

    typedef enum
    {
      RV_NOTHING,
      RV_FIXNUM,
      RV_SYMBOL,
      RV_OBJECT
    } ruby_value_type_t;

    ruby_value_type_t type;
    union {
      long fixnum;
      RubySymbol *symbol;
      RubyObject *object;
    };

  protected:
    explicit RubyValue(long);
    explicit RubyValue(RubySymbol *);
    explicit RubyValue(RubyObject *);
};

#endif

