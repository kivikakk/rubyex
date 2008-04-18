#ifndef GC_H
#define GC_H

#include <vector>

class RubyObject;

class GarbageCollector
{
  public:
    GarbageCollector();

    void track(RubyObject *);

  protected:
    std::vector<RubyObject *> objects;
};

#endif

