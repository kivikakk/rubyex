#ifndef RKERNEL_H
#define RKERNEL_H

#include "renvironment.h"
#include "rei.h"

class RubyKernelEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

extern const char *RUBY_REQUIRE_EXTENSIONS[];
extern const char *SHARED_REQUIRE_EXTENSIONS[];
bool kernel_load(linked_ptr<Binding> &, const std::string &);
bool kernel_load(linked_ptr<Binding> &, const std::string &, bool);

#endif


