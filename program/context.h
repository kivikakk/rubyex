#ifndef CONTEXT_H
#define CONTEXT_H

// A context for local variables, etc.
// Experimental concept; not sure if this is the correct way of representing it.

#include <map>
#include <string>
#include "rvalue.h"

class Context
{
  public:
    Context(RubyObject *);

    RubyObject *get_context() const;
    
    std::map<std::string, RubyValue> locals;

  protected:
    RubyObject *context;
};

#endif

