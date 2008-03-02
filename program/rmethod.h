#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>

class RubyMethod
{ };

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

#endif

