#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include "rvalue.h"
#include "binding.h"

class Context;

class Block
{
  public:
    Block(RubyClass *, Context *, Block *);

    RubyValue call(linked_ptr<Binding> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue);
    RubyValue call(Context *, linked_ptr<Binding> &);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &, Block &);

    std::string code;
    std::vector<std::string> args;
    RubyClass *def_target;
    Context *caller_context;
    Block *caller_block;
};

#endif

