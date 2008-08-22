#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include "renvironment.h"
#include "reader.h"
#include "context.h"

RubyValue process(RubyEnvironment &, Reader &, Context *, Block *);

#endif
