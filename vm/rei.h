#ifndef REI_H
#define REI_H

class RubyEnvironment;

class RubyEnvironmentInitializer
{
  public:
    virtual ~RubyEnvironmentInitializer();

    virtual void init(RubyEnvironment &) = 0;
};

#endif
