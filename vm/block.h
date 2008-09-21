#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include "rvalue.h"
#include "binding.h"

class Context;

class BlockArg
{
  public:
    BlockArg(const std::string &);
    BlockArg(Block *, const std::string &, const std::string &);
    ~BlockArg();

    std::string name;
    Block *default_code;
};

class Block
{
  public:
    Block(RubyModule *, Context *, Block *);
    ~Block();

    RubyValue call(linked_ptr<Binding> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue);
    RubyValue call(Context *);
    RubyValue call(Context *, const std::vector<RubyValue> &);
    RubyValue call(Context *, linked_ptr<Binding> &);
    RubyValue call(Context *, linked_ptr<Binding> &, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &, Block &);
    RubyValue call(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &, Block &);

    std::string code;

    std::vector<BlockArg *> args;
    bool has_splat; std::string splat_name;

    RubyModule *def_target;
    Context *caller_context;
    Block *caller_block;

  protected:
    void assign_args(linked_ptr<Binding> &, Context *, const std::vector<RubyValue> &) const;
};

#endif

