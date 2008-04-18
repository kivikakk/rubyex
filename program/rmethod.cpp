#include "rmethod.h"

RubyCMethod::RubyCMethod(void *_function, int _args, bool _self_is_object): function(_function), args(_args), self_is_object(_self_is_object)
{ }

RubyBytecodeMethod::RubyBytecodeMethod(int _args): args(_args)
{ }
