#ifndef CONTEXT_H
#define CONTEXT_H

// A context for local variables, etc.
// Experimental concept; not sure if this is the correct way of representing it.

#include <map>
#include <string>
#include "stack.h"
#include "rvalue.h"
#include "renvironment.h"

class Context
{
  public:
    Context(RubyEnvironment *, RubyValue);

    RubyEnvironment *get_environment() const;
    RubyValue get_context() const;
    void _report() const;
    RubyValue entry_to_value(const Stack::StackEntry &) const;

    void assign(const std::string &, RubyValue);
    
    std::map<std::string, RubyValue> locals;

  protected:
    RubyEnvironment *environment;
    RubyValue context;
};

#endif

