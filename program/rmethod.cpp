#include "rmethod.h"

RubyCMethod::RubyCMethod(void *_function, int _args): function(_function), args(_args)
{ }

RubyBytecodeMethod::RubyBytecodeMethod(int _args): args(_args)
{ }
