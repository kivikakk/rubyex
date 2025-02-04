#ifndef RVALUE_H
#define RVALUE_H

#include <string>
#include <vector>
#include <iostream>
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

    bool operator <(const RubyValue &) const;	// needed to use RV as a STL map key
    bool operator ==(const RubyValue &) const;	// bit for bit
    bool operator !=(const RubyValue &) const;	// " " "

    static RubyValue from_fixnum(long);
    static RubyValue from_symbol(RubySymbol *);
    static RubyValue from_object(RubyObject *);

    RubyClass *get_class(RubyEnvironment &) const;
    const linked_ptr<RubyMethod> &get_method(linked_ptr<Binding> &, const std::string &) const;

    bool has_instance(const std::string &) const;
    RubyValue get_instance(const std::string &) const;
    void set_instance(const std::string &, RubyValue);

    bool ruby_eq_op(linked_ptr<Binding> &, RubyValue) const;

    bool truthy(RubyEnvironment &) const;
    long get_fixnum() const;
    std::string to_s(linked_ptr<Binding> &) const;
    std::string inspect(linked_ptr<Binding> &) const;

    const std::string &get_string() const;
    void set_string(const std::string &) const;

    template <typename X> bool is_a() const {
      if (type != RV_OBJECT) return false;
      return dynamic_cast<X *>(object) != NULL;
    }
    template <typename X> X *get_special() const {
      if (type != RV_OBJECT) return NULL;
      return dynamic_cast<X *>(object);		/* dynamic_cast returns NULL on failure */
    }

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

#define F2V(x) RubyValue::from_fixnum((x))
#define S2V(x) RubyValue::from_symbol((x))
#define O2V(x) RubyValue::from_object((x))

#endif

