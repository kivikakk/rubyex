#ifndef RVALUE_H
#define RVALUE_H

class RubyValue
{
  public:
    RubyValue();

    typedef enum {
      VT_FIXNUM, 		// SUBCLASS RubyValue?
      VT_STRING,
      VT_OBJECT
    } value_type_t;
};

class RubyNilValue : public RubyValue
{ };

class RubyTrueValue : public RubyValue
{ };

class RubyFalseValue : public RubyValue
{ };


#endif

