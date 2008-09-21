#ifndef RMETHOD_H
#define RMETHOD_H

#include <string>
#include <vector>
#include "binding.h"
#include "rvalue.h"
#include "block.h"

typedef RubyValue (*RCMethodBlockNoArgs)(linked_ptr<Binding> &, RubyValue, Block &);
typedef RubyValue (*RCMethodNoArgs)(linked_ptr<Binding> &, RubyValue);
typedef RubyValue (*RCMethodBlockArgs)(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);
typedef RubyValue (*RCMethodArgs)(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);

class RubyMethod
{
  public:
    virtual ~RubyMethod();

    static linked_ptr<RubyMethod> Create(RCMethodBlockNoArgs);
    static linked_ptr<RubyMethod> Create(RCMethodNoArgs);
    static linked_ptr<RubyMethod> Create(RCMethodBlockArgs, int);
    static linked_ptr<RubyMethod> Create(RCMethodArgs, int);

    static bool verify_args(int, int);
    static std::string args_error_message(int, int);

    virtual RubyValue call(linked_ptr<Binding> &, RubyValue);			// adds blank arglist.
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, Block &);		// adds blank arglist.
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &) = 0;
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &) = 0;
};

class RubyCMethod : public RubyMethod
{
  public:
    RubyCMethod(RCMethodBlockNoArgs);
    RubyCMethod(RCMethodNoArgs);
    RubyCMethod(RCMethodBlockArgs, int);
    RubyCMethod(RCMethodArgs, int);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

    void *function;
    bool has_block;
    int no_of_args;
};

class RubyMultiCMethod : public RubyMethod
{
  public:
    RubyMultiCMethod(const linked_ptr<RubyCMethod> &, const linked_ptr<RubyCMethod> &);
    RubyMultiCMethod(const linked_ptr<RubyCMethod> &, const linked_ptr<RubyCMethod> &, const linked_ptr<RubyCMethod> &);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

    std::vector< linked_ptr<RubyCMethod> > methods;
};

class RubyBytecodeMethod : public RubyMethod
{
  public:
    RubyBytecodeMethod(RubyModule *);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

    Block code;
};

// These should be used to emulate the effect of a final *args
// only, really.
#define ARGS_ARBITRARY (-1)
#define ARGS_MINIMAL(x) (-((x) + 1))

#endif

