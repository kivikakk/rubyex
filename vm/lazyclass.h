#ifndef LAZYCLASS_H
#define LAZYCLASS_H

#include <string>
class RubyClass;
class RubyEnvironment;

class LazyClass
{
  public:
    LazyClass();
    virtual ~LazyClass();

    RubyClass *resolve();
    RubyClass *resolve() const;
  
  protected:
    virtual RubyClass *internal_resolve() const;

    RubyClass *cache;
};

class NamedLazyClass : public LazyClass
{
  public:
    NamedLazyClass(RubyEnvironment &, const std::string &);

  protected:
    RubyClass *internal_resolve() const;
  
    RubyEnvironment *e;
    std::string name;
};

#endif

