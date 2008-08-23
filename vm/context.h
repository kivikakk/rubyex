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
    Context(RubyEnvironment &, RubyValue, RubyClass *);
    Context(linked_ptr<Binding> &);

    RubyMethod *get_method(const std::string &) const;

    RubyValue entry_to_value(const Stack::StackEntry &);
    RubyValue resolve_identifier(const std::string &);
    // these two not const - could result in method call.

    void assign(const std::string &, RubyValue);

    linked_ptr<Binding> binding;
};

#endif

