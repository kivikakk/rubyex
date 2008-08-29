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
    WorldException(linked_ptr<Binding> &, RubyObject *);
    WorldException(linked_ptr<Binding> &, RubyObject *, const std::string &);

    RubyObject *exception;

  protected:
    void check_exception_object(linked_ptr<Binding> &) const;
};

class SevereInternalError : public std::exception
{
  public:
    SevereInternalError(const std::string &);
    ~SevereInternalError() throw ();

    std::string message;
};

#endif

