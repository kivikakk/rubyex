#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include "rvalue.h"
#include "binding.h"

class Block
{
  public:
    Block();

    RubyValue call(linked_ptr<Binding> &);
    RubyValue call(linked_ptr<Binding> &, RubyValue);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &);
    RubyValue call(linked_ptr<Binding> &, const std::vector<RubyValue> &, Block &);

    std::string code;
    std::vector<std::string> args;
};

#endif

