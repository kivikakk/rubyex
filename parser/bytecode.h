#ifndef BYTECODE_H
#define BYTECODE_H

typedef enum
{
  T_IDENTIFIER		= 0x01,
  T_SYMBOL		= 0x02,
  T_INTEGER_LITERAL	= 0x03,
  T_FLOATING_LITERAL	= 0x04,
  T_BOOLEAN_LITERAL	= 0x05,
  T_STRING_LITERAL	= 0x06,
  T_NIL_LITERAL		= 0x07,

  T_BLOCK		= 0xf0
} type_t;

typedef enum
{
  I_ASSIGNMENT		= 0x01,
  I_EXECUTE		= 0x02,

  I_TARGET_CALL_BLOCK	= 0x10,
  I_TARGET_CALL		= 0x11,
  I_CALL_BLOCK 		= 0x12,
  I_CALL		= 0x13,

  I_CONSTRUCT_BLOCK	= 0x20,
  I_TARGET_DEF		= 0x21,
  I_DEF			= 0x22,
  I_YIELD		= 0x23,
  I_INTERPOL		= 0x24,		// I love those guys.

  I_END			= 0x30,
  I_POP			= 0x31,
  I_PUSH		= 0x32,
  I_PUSH_LAST		= 0x33,		// pushes the last val
  I_JMP			= 0x34,

  I_IF			= 0x40,

  I_PUSH_EXCEPTION	= 0x50,
} instruction_t;

#endif

