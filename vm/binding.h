#ifndef BINDING_H
#define BINDING_H

#include <map>
#include <string>
#include "renvironment.h"
#include "rvalue.h"

class Block;

class Binding
{
  public:
    Binding(RubyEnvironment &, RubyValue, RubyClass *);
    ~Binding();

    std::map<std::string, RubyValue> locals;

    RubyEnvironment &environment;
    RubyValue context;
    RubyClass *def_target;		// where `def' points to.
};

#endif
