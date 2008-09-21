#ifndef GC_H
#define GC_H

#include <vector>

class RubyObject;

class GarbageCollector
{
  public:
    GarbageCollector();

    void empty();		// Warning?

    template <typename T> T *track(T *_object) {
      objects.push_back(_object);
      return _object;
    }

  protected:
    std::vector<RubyObject *> objects;
};

#endif

