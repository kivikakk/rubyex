#include "gc.h"
#include "robject.h"

GarbageCollector::GarbageCollector()
{ }

void GarbageCollector::track(RubyObject *object)
{
  objects.push_back(object);
}

