#ifndef CONTEXT_H
#define CONTEXT_H

// A context for local variables, etc.
// Experimental concept; not sure if this is the correct way of representing it.
//
// Relationship to `Binding'?

#include <string>
#include <memory>
#include "linked_ptr.h"
#include "stack.h"
#include "rvalue.h"
#include "renvironment.h"
#include "binding.h"

class Context
{
  public:
    Context(RubyEnvironment *, RubyValue);
    Context(RubyEnvironment *, RubyValue, linked_ptr<Binding> &);

    RubyEnvironment *get_environment() const;
    RubyValue entry_to_value(const Stack::StackEntry &) const;
    RubyValue resolve_identifier(const std::string &) const;

    RubyMethod *get_method(const std::string &);
    void assign(const std::string &, RubyValue);

    linked_ptr<Binding> binding;
    
  protected:
    RubyEnvironment *environment;
};

#endif

