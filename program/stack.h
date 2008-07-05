#ifndef STACK_H
#define STACK_H

#include "bytecode.h"
#include "block.h"
#include <vector>
#include "rvalue.h"

class RubyObject;

class Stack
{
  public:
    Stack();

    typedef enum
    {
      SE_IDENTIFIER,
      SE_SYMBOL,
      SE_INTEGER,
      SE_BLOCK,
      SE_OBJECT
    } stack_entry_type_t;

    typedef struct
    {
      stack_entry_type_t type;
      union {
	std::string *identifier;
	std::string *symbol;
	int integer;
	Block *block;
	RubyObject *object;
      };
    } StackEntry;

    static RubyValue entry_to_value(const StackEntry &);

    void push_identifier(const std::string &);
    void push_symbol(const std::string &);
    void push_integer(int);
    void push_block(const Block &);
    void push_object(RubyObject *);
    void push(const StackEntry &);

    StackEntry pop_variant();
    template <typename T> T pop();

  protected:
    std::vector<StackEntry> ival;
};

#endif

