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
    Block(RubyModule *, Context *, Block *);

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
    std::vector<std::string> args;			// TODO RESUME - extend here. - need an extra var for optionals/splat? or what?
    RubyModule *def_target;
    Context *caller_context;
    Block *caller_block;
};

#endif

