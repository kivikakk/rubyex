#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>
#include <vector>
#include "renvironment.h"
#include "rvalue.h"

typedef RubyValue (*RCMethodInstanceNoArgs)(RubyEnvironment &, RubyValue);
typedef RubyValue (*RCMethodModuleArgs)(RubyEnvironment &, const std::vector<RubyValue> &);

class RubyMethod
{
  public:
    // `Call' method?
};

class RubyCMethod : public RubyMethod
{
  public:
    RubyCMethod(void *, int);

  protected:
    void *function;
    int args;
};

class RubyBytecodeMethod : public RubyMethod
{
  public:
    RubyBytecodeMethod(int);

    std::string data;

  protected:
    int args;
};

#define CMETHOD(fn, args) (new RubyCMethod((void *)(fn), (args)))

#define ARGS_ARBITRARY (-1)
#define ARGS_MINIMAL(x) (-1 - (x))

#endif

