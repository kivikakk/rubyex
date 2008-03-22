#include "robject.h"

RubyObject::RubyObject(LazyClass *_klass): klass(_klass), metaklass(NULL)
{ }

RubyObject::~RubyObject()
{
  for (std::map<std::string, RubyValue *>::iterator it = instance_variables.begin(); it != instance_variables.end(); ++it)
    delete it->second;
}

