#ifndef BYTECODE_H
#define BYTECODE_H

typedef enum
{
  I_IDENTIFIER		= 0x01,
  I_SYMBOL		= 0x02,
  I_INTEGER_LITERAL	= 0x03,
  I_FLOATING_LITERAL	= 0x04,
  I_BOOLEAN_LITERAL	= 0x05,
  I_STRING_LITERAL	= 0x06,
  I_DEF_LIST		= 0x07,
  I_BLOCK		= 0x08,
  I_FUNC_CALL		= 0x09,
  I_ASSIGNMENT		= 0x0a,
} instruction_t;

#endif

