#include "process.h"
#include "stack.h"
#include "rvalue.h"
#include "rstring.h"
#include "rnumeric.h"
#include "rmethod.h"

RubyValue process(RubyEnvironment &e, Reader &r, Context *context)
{
  Stack s;
  RubyValue last_value = e.NIL;

  while (true) {
    instruction_t in = r.read_instruction();
    if (!in || r.eof())
      break;

    switch (in) {
      case I_ASSIGNMENT: {
	std::string name = r.read_string();
	RubyValue rval = s.pop_value(context);
	context->assign(name, rval);
	last_value = rval;
	break;
      }
      case I_EXECUTE: {
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: last_value = context->resolve_identifier(r.read_string()); break;
	  case T_SYMBOL: last_value = RubyValue::from_symbol(e.get_symbol(r.read_string())); break;
	  case T_INTEGER_LITERAL: last_value = RubyValue::from_fixnum(r.read_int32()); break;
	  case T_FLOATING_LITERAL: last_value = RubyValue::from_object(e.gc.track(new RubyFloating(e, r.read_floating()))); break;
	  case T_BOOLEAN_LITERAL: last_value = r.read_bool() ? e.TRUE : e.FALSE; break;
	  case T_STRING_LITERAL: last_value = RubyValue::from_object(e.gc.track(new RubyString(e, r.read_text()))); break;
	  default: std::cerr << "EXECUTE unknown_type(" << t << ")" << std::endl;
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

	std::string name = r.read_string();
	uint32 arg_count = r.read_uint32();

	RubyValue target;
	Block block; 

	if (is_target) target = s.pop_value(context);
	if (is_block) block = s.pop_block();

	std::vector<RubyValue> arguments;
	while (arg_count--)
	  arguments.push_back(s.pop_value(context));

	// if we have a target, we need to be more direct
	RubyMethod *method = is_target ? target.get_method(name, e) : context->get_method(name);

	last_value = method->call(context->binding, is_target ? target : context->binding->context, arguments);	// boom
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
	// XXX define
	break;
      }

      case I_END:
	std::cerr << "END unimplemented." << std::endl;
	break;
      case I_POP:
	std::cerr << "POP unimplemented." << std::endl;
	break;
      case I_PUSH: {
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: s.push_identifier(r.read_string()); break;
	  case T_SYMBOL: s.push_symbol(r.read_string()); break;
	  case T_INTEGER_LITERAL: s.push_integer(r.read_int32()); break;
	  case T_FLOATING_LITERAL: s.push_object(e.gc.track(new RubyFloating(e, r.read_floating()))); break;
	  case T_BOOLEAN_LITERAL: s.push_object(r.read_bool() ? e.TRUE.object : e.FALSE.object); break;
	  case T_STRING_LITERAL: s.push_object(e.gc.track(new RubyString(e, r.read_text()))); break;
	  case T_BLOCK: /* XXX complain */ std::cerr << "push a block?" << std::endl; break;
	}
	break;
      }
      case I_PUSH_LAST:
	switch (last_value.type) {
	  case RubyValue::RV_FIXNUM: s.push_integer(last_value.fixnum); break;
	  case RubyValue::RV_SYMBOL: s.push_symbol(last_value.symbol->get_value()); break;
	  case RubyValue::RV_OBJECT: s.push_object(/* XXX gc track? */ last_value.object); break;
	  case RubyValue::RV_NOTHING: /* XXX what? */ std::cerr << "I_PUSH_LAST instruction, but was given a RV_NOTHING RubyValue (uninitialised RubyValue somewhere?); confused!" << std::endl; break;
	}
	break;

      default:
	std::cerr << "unknown(" << in << ")" << std::endl;
	break;
    }
  }

  return last_value;
}

