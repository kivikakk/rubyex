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
    RubyIO(linked_ptr<Binding> &, int, const char *);
    ~RubyIO();

    static std::string rv_to_mode(linked_ptr<Binding> &, RubyValue);

    void init(linked_ptr<Binding> &, int, const char *);
    std::string read(linked_ptr<Binding> &);
    std::string read(linked_ptr<Binding> &, int);
    int write(linked_ptr<Binding> &, const std::string &);
    void flush(linked_ptr<Binding> &);
    void close(linked_ptr<Binding> &);

    bool sync;
    FILE *file;

  protected:
    void _check(linked_ptr<Binding> &);
};

class IOEOFError : public std::exception
{ };

#endif

