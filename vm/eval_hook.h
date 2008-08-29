#ifndef EVAL_HOOK_H
#define EVAL_HOOK_H

#include "renvironment.h"
#include "rvalue.h"
#include "linked_ptr.h"
#include "binding.h"

RubyValue eval_hook(linked_ptr<Binding> &, RubyValue, const std::string &);

#endif

