#ifndef EVAL_HOOK_H
#define EVAL_HOOK_H

#include "renvironment.h"
#include "rvalue.h"

RubyValue eval_hook(RubyEnvironment &, RubyValue, const std::string &);

#endif

