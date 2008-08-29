#ifndef RIO_H
#define RIO_H

#include <stdio.h>
#include "rei.h"
#include "robject.h"
#include "renvironment.h"

class RubyIOEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class RubyIO : public RubyObject
{
  public:
    RubyIO(RubyEnvironment &);
    RubyIO(RubyEnvironment &, int, const char *);
    ~RubyIO();

    static std::string rv_to_mode(linked_ptr<Binding> &, RubyValue);

    void init(int, const char *);
    std::string read();
    std::string read(int);
    int write(const std::string &);
    void flush();
    void close();

    bool sync;
    FILE *file;
};

#endif

