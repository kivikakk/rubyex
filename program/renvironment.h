#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
#include "rclass.h"

class RubyEnvironment
{
  public:
    RubyEnvironment();

    std::map<std::string, RubyClass> classes;
};

#endif

