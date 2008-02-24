#ifndef RMETHOD_H
#define RMETHOD_H

class RubyMethod
{
};

class RubyCMethod : public RubyMethod
{
  public:
    RubyCMethod(void *, int);

  protected:
    void *function;
    int args;
};

#define CMETHOD(fn, args) (new RubyCMethod((void *)(fn), (args)))

#endif

