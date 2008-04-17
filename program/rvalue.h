#ifndef RVALUE_H
#define RVALUE_H

#include <string>

class RubyValue
{
  public:
    RubyValue();
    virtual ~RubyValue();
};

class RubyFixnumValue : public RubyValue
{
  public:
    RubyFixnumValue(long);

    long value;
};

class RubyStringValue : public RubyValue
{
  public:
    RubyStringValue(const std::string &);

    const std::string &get_value() const;

  protected:
    std::string value;
};


class RubyObject;

class RubyObjectValue : public RubyValue
{ 
  public:
    RubyObjectValue(RubyObject *);

    RubyObject *value;
};

class RubyNilValue : public RubyValue
{
  public:
};

class RubyTrueValue : public RubyValue
{
  public:
};

class RubyFalseValue : public RubyValue
{
  public:
};


#endif

