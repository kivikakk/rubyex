#ifndef GC_H
#define GC_H

#include <vector>

class RubyObject;

class GarbageCollector
{
  public:
    GarbageCollector();

    RubyObject *track(RubyObject *);

  protected:
    std::vector<RubyObject *> objects;
};

#endif

