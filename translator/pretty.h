#ifndef PRETTY_H
#define PRETTY_H

#include <string>

class PrettyPrint
{
  public:
    virtual ~PrettyPrint();

    virtual void p(int) const = 0;

  protected:
    std::string p_tabs(int) const;
};

#endif

