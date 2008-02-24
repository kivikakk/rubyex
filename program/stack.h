#ifndef STACK_H
#define STACK_H

#include "bytecode.h"
#include "block.h"
#include <vector>

class Stack
{
  public:
    Stack();

    typedef struct
    {
      type_t type;
      union {
	std::string *identifier;
	std::string *symbol;
	int integer;
	double floating;
	bool boolean;
	std::string *string;
	Block *block;
      };
    } StackEntry;

    void push_identifier(const std::string &);
    void push_symbol(const std::string &);
    void push_integer(int);
    void push_floating(double);
    void push_boolean(bool);
    void push_string(const std::string &);
    void push_block(const Block &);
    void push(const StackEntry &);

    StackEntry pop_variant();
    template <typename T> T pop();

  protected:
    std::vector<StackEntry> ival;
};

#endif

