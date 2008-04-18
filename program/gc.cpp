#include "gc.h"
#include "robject.h"

GarbageCollector::GarbageCollector()
{ }

RubyObject *GarbageCollector::track(RubyObject *object)
{
  objects.push_back(object);
  return object;
}

