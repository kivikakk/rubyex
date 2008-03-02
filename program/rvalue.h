#ifndef RVALUE_H
#define RVALUE_H

class RubyValue
{
  public:
    RubyValue();
    virtual ~RubyValue();

    virtual long get_object_id() const = 0;
};

class RubyFixnumValue : public RubyValue
{
  public:
    RubyFixnumValue(long);

    long get_object_id() const;

    long value;
};

class RubyStringValue : public RubyValue
{
  public:
    RubyStringValue(const std::string &);

    long get_object_id() const;
    const std::string &get_value() const;

  protected:
    std::string value;
};

class RubyObjectValue : public RubyValue
{ 
  public:
    RubyObjectValue(RubyObject *);

    long get_object_id() const;

    RubyObject *value;
};

class RubyNilValue : public RubyValue
{
  public:
    long get_object_id() const;
};

class RubyTrueValue : public RubyValue
{
  public:
    long get_object_id() const;
};

class RubyFalseValue : public RubyValue
{
  public:
    long get_object_id() const;
};


#endif

