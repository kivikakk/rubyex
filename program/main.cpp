#include <iostream>
#include "renvironment.h"
#include "reader.h"
#include "stack.h"

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

  while (true) {
    instruction_t in = r.read_instruction();
    if (!in || r.eof())
      break;

    std::cerr << "Read instruction: ";
    switch (in) {
      case I_ASSIGNMENT: {
	  std::cerr << "ASSIGNMENT to ";
	  std::string name = r.read_string();
	  std::cerr << name << std::endl;
	  Stack::StackEntry rval = s.pop_variant();
	  break;
      }
      case I_EXECUTE:
	std::cerr << "EXECUTE" << std::endl;
	break;

      case I_TARGET_CALL_BLOCK: {
	std::cerr << "TARGET_CALL_BLOCK ";
	std::string name = r.read_string();
	std::cerr << name;
	uint32 arg_count = r.read_uint32();
	std::cerr << " (" << arg_count << " arg/s)" << std::endl;

	Stack::StackEntry target = s.pop_variant();
	Stack::StackEntry block = s.pop_variant();

	while (arg_count > 0) {
	  std::cerr << "argument: ";
	  Stack::StackEntry se = s.pop_variant();
	  std::cerr << "type=" << se.type << std::endl;
	  arg_count--;
	}
	break;
      }
      case I_TARGET_CALL: {
	std::cerr << "TARGET_CALL ";
	std::string name = r.read_string();
	std::cerr << name;
	uint32 arg_count = r.read_uint32();
	std::cerr << " (" << arg_count << " arg/s)" << std::endl;

	Stack::StackEntry target = s.pop_variant();

	while (arg_count > 0) {
	  std::cerr << "argument: ";
	  Stack::StackEntry se = s.pop_variant();
	  std::cerr << "type=" << se.type << std::endl;
	  arg_count--;
	}
	break;
      }
      case I_CALL_BLOCK: {
	std::cerr << "CALL_BLOCK ";
	std::string name = r.read_string();
	std::cerr << name;
	uint32 arg_count = r.read_uint32();
	std::cerr << " (" << arg_count << " arg/s)" << std::endl;

	Stack::StackEntry block = s.pop_variant();

	while (arg_count > 0) {
	  std::cerr << "argument: ";
	  Stack::StackEntry se = s.pop_variant();
	  std::cerr << "type=" << se.type << std::endl;
	  arg_count--;
	}
	break;
      }
      case I_CALL: {
	std::cerr << "CALL ";
	std::string name = r.read_string();
	std::cerr << name;
	uint32 arg_count = r.read_uint32();
	std::cerr << " (" << arg_count << " arg/s)" << std::endl;

	while (arg_count > 0) {
	  std::cerr << "argument: ";
	  Stack::StackEntry se = s.pop_variant();
	  std::cerr << "type=" << se.type << std::endl;
	  arg_count--;
	}
	break;
      }
      case I_SINGLE_CALL:
	std::cerr << "SINGLE_CALL" << std::endl;
	break;

      case I_CONSTRUCT_BLOCK:
	std::cerr << "CONSTRUCT_BLOCK" << std::endl;
	break;

      case I_END:
	std::cerr << "END" << std::endl;
	break;
      case I_POP:
	std::cerr << "POP" << std::endl;
	break;
      case I_PUSH: {
	std::cerr << "PUSH ";
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: std::cerr << "IDENTIFIER"; 		s.push_identifier(r.read_string()); break;
	  case T_SYMBOL: std::cerr << "SYMBOL"; 			s.push_symbol(r.read_string()); break;
	  case T_INTEGER_LITERAL: std::cerr << "INTEGER_LITERAL"; 	s.push_integer(r.read_int32()); break;
	  case T_FLOATING_LITERAL: std::cerr << "FLOATING_LITERAL"; 	s.push_floating(r.read_float()); break;
	  case T_BOOLEAN_LITERAL: std::cerr << "BOOLEAN_LITERAL";  	s.push_boolean(r.read_bool()); break;
	  case T_STRING_LITERAL: std::cerr << "STRING_LITERAL"; 	s.push_string(r.read_text()); break;
	};
	std::cerr << std::endl;
	break;
      }
      case I_PUSH_LAST:
	std::cerr << "PUSH_LAST" << std::endl;
	break;

      default:
	std::cerr << "unknown(" << in << ")" << std::endl;
	break;
    }
  }
}

