#ifndef YYWRAP_H
#define YYWRAP_H

extern "C" int yywrap(void);

typedef int (*ruby_yywrap_delegate_t)();

extern ruby_yywrap_delegate_t ruby_yywrap_delegate;

#endif
