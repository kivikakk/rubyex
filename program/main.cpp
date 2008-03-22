#include <iostream>
#include "renvironment.h"
#include "reader.h"
#include "stack.h"
#include "rvalue.h"

void process(RubyEnvironment &, Reader &);

int main(int argc, char **argv)
{
  std::cerr << "Reading byte-code ..." << std::endl;
  std::cerr << std::hex;

  RubyEnvironment e;
  Reader r(std::cin);
  process(e, r);

  return 0;
}

void process(RubyEnvironment &e, Reader &r)
{
  Stack s;
  RubyValue *last_value = NULL;

  while (true) {
    instruction_t in = r.read_instruction();
    if (!in || r.eof())
      break;

    switch (in) {
      case I_ASSIGNMENT: {
	std::cerr << "ASSIGNMENT" << std::endl;
	std::string name = r.read_string();
	std::cerr << name << std::endl;
	Stack::StackEntry rval = s.pop_variant();
	break;
      }
      case I_EXECUTE: {
	std::cerr << "EXECUTE" << std::endl;
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: std::cerr << r.read_string() << std::endl; break;
	  case T_SYMBOL: std::cerr << r.read_string() << std::endl; break;
	  case T_INTEGER_LITERAL: std::cerr << r.read_int32() << std::endl; break;
	  case T_FLOATING_LITERAL: std::cerr << r.read_float() << std::endl; break;
	  case T_BOOLEAN_LITERAL: std::cerr << (r.read_bool() ? "true" : "false") << std::endl; break;
	  case T_STRING_LITERAL: std::cerr << '"' << r.read_text() << '"' << std::endl; break;
	  default: std::cerr << "unknown_type(" << t << ")" << std::endl;
	}
	break;
      }
      case I_TARGET_CALL_BLOCK:
      case I_TARGET_CALL:
      case I_CALL_BLOCK:
      case I_CALL:
      {
	bool is_target = (in == I_TARGET_CALL_BLOCK) || (in == I_TARGET_CALL),
	  is_block = (in == I_TARGET_CALL_BLOCK) || (in == I_CALL_BLOCK);

	std::cerr << "CALL [ " << (is_target ? "target " : "") << (is_block ? "block " : "") << "]" << std::endl;
	std::string name = r.read_string();
	uint32 arg_count = r.read_uint32();

	Stack::StackEntry target, block; 

	if (is_target) target = s.pop_variant();
	if (is_block) block = s.pop_variant();

	while (arg_count--)
	  Stack::StackEntry se = s.pop_variant();
	break;
      }

      case I_CONSTRUCT_BLOCK: {
	std::cerr << "CONSTRUCT_BLOCK" << std::endl;
	Block block;

	uint32 arg_count = r.read_uint32();
	while (arg_count--)
	  block.args.push_back(r.read_string());

	uint32 byte_count = r.read_uint32();
	block.data = r.read_bytes(byte_count);

	s.push_block(block);
    
	break;
      }
      // I_TARGET_DEF
      case I_DEF: {
	std::cerr << "DEF" << std::endl;
	
	std::string name = r.read_string();
	uint32 arg_count = r.read_uint32();

	RubyBytecodeMethod method(arg_count);
	while (arg_count--)
	  r.read_string();	// XXX
	
	uint32 byte_count = r.read_uint32();
	method.data = r.read_bytes(byte_count);

	break;
      }

      case I_END:
	std::cerr << "END unimplemented." << std::endl;
	break;
      case I_POP:
	std::cerr << "POP unimplemented." << std::endl;
	break;
      case I_PUSH: {
	std::cerr << "PUSH" << std::endl;
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: s.push_identifier(r.read_string()); break;
	  case T_SYMBOL: s.push_symbol(r.read_string()); break;
	  case T_INTEGER_LITERAL: s.push_integer(r.read_int32()); break;
	  case T_FLOATING_LITERAL: s.push_floating(r.read_float()); break;
	  case T_BOOLEAN_LITERAL: s.push_boolean(r.read_bool()); break;
	  case T_STRING_LITERAL: s.push_string(r.read_text()); break;
	  case T_BLOCK: break;
	};
	break;
      }
      case I_PUSH_LAST:
	std::cerr << "PUSH_LAST unimplemented." << std::endl;
	break;

      default:
	std::cerr << "unknown(" << in << ")" << std::endl;
	break;
    }
  }
}

