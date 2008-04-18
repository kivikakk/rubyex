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

class RubySymbolValue : public RubyValue
{
  public:
    RubySymbolValue(const std::string &);

    std::string value;
};

class RubyObject;

class RubyObjectValue : public RubyValue
{ 
  public:
    RubyObjectValue(RubyObject *);

    RubyObject *value;
};

#endif

