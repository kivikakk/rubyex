#include "gc.h"
#include "robject.h"
#include <iostream>

GarbageCollector::GarbageCollector()
{ }

RubyObject *GarbageCollector::track(RubyObject *_object)
{
  // std::cerr << "GC: tracking object " << typeid(_object).name() << std::endl;
  objects.push_back(_object);
  return _object;
}

