#ifndef EVAL_HOOK_H
#define EVAL_HOOK_H

#include "renvironment.h"
#include "rvalue.h"
#include "context.h"

RubyValue eval_hook(RubyEnvironment &, Context *, RubyValue, const std::string &);

#endif

