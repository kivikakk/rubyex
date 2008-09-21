#include <typeinfo>
#include <iostream>
#include "gc.h"
#include "robject.h"
#include "rmodule.h"

GarbageCollector::GarbageCollector()
{ }

void GarbageCollector::empty() {
  for (std::vector<RubyObject *>::iterator it = objects.begin(); it != objects.end(); ++it)
    delete *it;
}

