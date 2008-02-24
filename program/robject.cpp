#include "robject.h"

RubyObject::RubyObject()
{ }

RubyObject::~RubyObject()
{
  for (std::map<std::string, RubyValue *>::iterator it = instance_variables.begin(); it != instance_variables.end(); ++it)
    delete it->second;
}

