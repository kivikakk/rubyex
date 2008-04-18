#include "gc.h"
#include "robject.h"
#include <iostream>

GarbageCollector::GarbageCollector()
{ }

RubyObject *GarbageCollector::track(RubyObject *object)
{
  std::cerr << "GC: tracking object " << typeid(object).name() << std::endl;
  objects.push_back(object);
  return object;
}

