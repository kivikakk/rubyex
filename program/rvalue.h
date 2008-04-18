#ifndef RVALUE_H
#define RVALUE_H

#include <string>
#include "rsymbol.h"

class RubyObject;

class RubyValue
{
  public:
    static RubyValue from_fixnum(long);
    static RubyValue from_symbol(RubySymbol *);
    static RubyValue from_object(RubyObject *);

    typedef enum
    {
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
    RubyValue(long);
    RubyValue(RubySymbol *);
    RubyValue(RubyObject *);
};

#endif

