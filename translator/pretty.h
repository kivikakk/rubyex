#ifndef PRETTY_H
#define PRETTY_H

#include <string>

class PrettyPrint
{
  public:
    virtual ~PrettyPrint();

    virtual void p() const = 0;
};

#endif

