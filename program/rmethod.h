#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>

class RubyMethod
{ };

class RubyCMethod : public RubyMethod
{
  public:
    RubyCMethod(void *, int, bool);

  protected:
    void *function;
    int args;
    bool self_is_object;
};

class RubyBytecodeMethod : public RubyMethod
{
  public:
    RubyBytecodeMethod(int);

    std::string data;

  protected:
    int args;
};

#define CVMETHOD(fn, args) (new RubyCMethod((void *)(fn), (args), false))
#define COMETHOD(fn, args) (new RubyCMethod((void *)(fn), (args), true))

#endif

