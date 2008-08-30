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
    Binding(RubyEnvironment &, RubyValue, RubyModule *);
    ~Binding();

    static std::string get_representation(linked_ptr<Binding> &, const std::vector<RubyValue> &);

    std::map<std::string, RubyValue> locals;

    RubyEnvironment &environment;
    RubyValue context;
    RubyModule *def_target;		// where `def' points to.
};

#endif
