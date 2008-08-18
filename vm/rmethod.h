#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>
#include <vector>
#include "binding.h"
#include "rvalue.h"

typedef RubyValue (*RCMethodNoArgs)(linked_ptr<Binding> &, RubyValue);
typedef RubyValue (*RCMethodArgs)(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

class RubyMethod
{
  public:
    static RubyMethod *Create(RCMethodNoArgs);
    static RubyMethod *Create(RCMethodArgs, int);

    virtual RubyValue call(linked_ptr<Binding> &, RubyValue);
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &) = 0;
};

class RubyMethodNoArgs : public RubyMethod
{
  public:
    RubyMethodNoArgs(RCMethodNoArgs);

    RubyValue call(linked_ptr<Binding> &, RubyValue);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

  protected:
    RCMethodNoArgs function;
};

class RubyMethodArgs : public RubyMethod
{
  public:
    RubyMethodArgs(RCMethodArgs, int);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

  protected:
    RCMethodArgs function;
    int args;
};

class RubyBytecodeMethod : public RubyMethod
{
  public:
    RubyBytecodeMethod(int);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

    std::string data;

  protected:
    int args;
};

#define ARGS_ARBITRARY (-1)
#define ARGS_MINIMAL(x) (-1 - (x))

#endif

