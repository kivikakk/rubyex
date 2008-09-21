#include <typeinfo>
#include <iostream>
#include "gc.h"
#include "robject.h"
#include "rmodule.h"

GarbageCollector::GarbageCollector()
{ }

void GarbageCollector::empty() {
  for (std::vector<RubyObject *>::iterator it = objects.begin(); it != objects.end(); ++it) {
    std::cout << "Deleting a " << typeid(**it).name() << "... ";
    if (dynamic_cast<RubyModule *>(*it))
      std::cout << "called " << dynamic_cast<RubyModule *>(*it)->get_name();
    std::cout << std::endl;
    delete *it;
  }
}

