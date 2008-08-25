#ifndef REXCEPTION_H
#define REXCEPTION_H

#include <exception>
#include "rei.h"
#include "robject.h"

class RubyExceptionEI : public RubyEnvironmentInitializer
{
  public:
    void init(RubyEnvironment &);
};

class WorldException : public std::exception
{
  public:
    WorldException(RubyObject *);

    RubyObject *exception;
};

#endif

