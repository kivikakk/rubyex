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

    static RubyMethod *Create(RCMethodBlockNoArgs);
    static RubyMethod *Create(RCMethodNoArgs);
    static RubyMethod *Create(RCMethodBlockArgs, int);
    static RubyMethod *Create(RCMethodArgs, int);

    static bool verify_args(int, int);

    virtual RubyValue call(linked_ptr<Binding> &, RubyValue);			// adds blank arglist.
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, Block &);		// adds blank arglist.
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &) = 0;
    virtual RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &) = 0;
};

class RubyMethodBlockNoArgs : public RubyMethod
{
  public:
    RubyMethodBlockNoArgs(RCMethodBlockNoArgs);

    RubyValue call(linked_ptr<Binding> &, RubyValue, Block &);					// actual impl.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);		// error.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);	// discard args.

  protected:
    RCMethodBlockNoArgs function;
};

class RubyMethodNoArgs : public RubyMethod
{
  public:
    RubyMethodNoArgs(RCMethodNoArgs);

    RubyValue call(linked_ptr<Binding> &, RubyValue);						// actual impl.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);		// discard args.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);	// discard block, args.

  protected:
    RCMethodNoArgs function;
};

class RubyMethodBlockArgs : public RubyMethod
{
  public:
    RubyMethodBlockArgs(RCMethodBlockArgs, int);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);		// error.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);	// actual impl.

  protected:
    RCMethodBlockArgs function;
    int args;
};

class RubyMethodArgs : public RubyMethod
{
  public:
    RubyMethodArgs(RCMethodArgs, int);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);		// actual impl.
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);	// discard block.

  protected:
    RCMethodArgs function;
    int args;
};

class RubyBytecodeMethod : public RubyMethod
{
  public:
    RubyBytecodeMethod(RubyModule *);

    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

    Block code;
};

#define ARGS_ARBITRARY (-1)
#define ARGS_MINIMAL(x) (-1 - (x))

#endif

