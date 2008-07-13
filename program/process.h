#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <vector>
#include "renvironment.h"
#include "reader.h"
#include "context.h"

void process(RubyEnvironment &, Reader &, std::vector<Context *> &, Context *);

#endif
