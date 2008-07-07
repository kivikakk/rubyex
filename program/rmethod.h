#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>

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

