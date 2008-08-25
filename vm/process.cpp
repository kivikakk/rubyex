#include "process.h"
#include "stack.h"
#include "rvalue.h"
#include "rstring.h"
#include "rnumeric.h"
#include "rmethod.h"

RubyValue process(RubyEnvironment &e, Reader &r, Context *context, Block *yield_block)
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
	  case T_NIL_LITERAL: last_value = e.NIL; break;
	  default: std::cerr << "I_EXECUTE: unknown_type(" << t << ")" << std::endl; throw;
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
	Block block(NULL, NULL, NULL);	// purposefully temporary.

	if (is_target) target = s.pop_value(context);
	if (is_block) block = s.pop_block();

	std::vector<RubyValue> arguments;
	while (arg_count--)
	  arguments.push_back(s.pop_value(context));

	// if we have a target, we need to be more direct
	RubyMethod *method = is_target ? target.get_method(name, e) : context->get_method(name);

	last_value = is_block ? 
	  method->call(context->binding, is_target ? target : context->binding->context, arguments, block) :
	  method->call(context->binding, is_target ? target : context->binding->context, arguments);	// boom
	break;
      }

      case I_CONSTRUCT_BLOCK: {
	Block block(context->binding->def_target, context, yield_block);	// takes def_target of surroundings.

	uint32 arg_count = r.read_uint32();
	while (arg_count--)
	  block.args.push_back(r.read_string());

	uint32 byte_count = r.read_uint32();
	block.code = r.read_bytes(byte_count);

	s.push_block(block);

	break;
      }
      // I_TARGET_DEF
      case I_DEF: {
	std::string name = r.read_string();
	uint32 arg_count = r.read_uint32();

	RubyBytecodeMethod *method = new RubyBytecodeMethod(context->binding->def_target);		// no target, runs in same context as here

	while (arg_count--)
	  method->code.args.push_back(r.read_string());
	
	uint32 byte_count = r.read_uint32();
	method->code.code = r.read_bytes(byte_count);

	context->binding->def_target->def_method(name, method);
	break;
      }
      case I_YIELD: {
	if (!yield_block) {
	  std::cerr << "I_YIELD: LocalJumpError: no block given" << std::endl;	// XXX LocalJumpError
	  throw;
	}

	uint32 arg_count = r.read_uint32();

	std::vector<RubyValue> arguments;
	while (arg_count--)
	  arguments.push_back(s.pop_value(context));

	// here we yield to our yield_block.
	// but we need to pass the block of the place that yielded to us.
	// def a(n)
	// 	yield 1
	// 	a(n + 1) {|e| yield e}	<- this yield should yield the same as the line above
	// end
	last_value = yield_block->call(context->binding, arguments);

	break;
      }
      case I_INTERPOL: {
	uint32 arg_count = r.read_uint32();
	
	std::vector<RubyValue> components;
	while (arg_count--)
	  components.push_back(s.pop_value(context));
	
	std::string r;
	for (std::vector<RubyValue>::iterator it = components.begin(); it != components.end(); ++it)
	  r += it->call(context->binding, "to_s").get_special<RubyString>()->string_value;

	last_value = e.get_string(r);
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
	  case T_NIL_LITERAL: s.push_object(e.NIL.object); break;
	  case T_BLOCK: /* XXX complain */ std::cerr << "I_PUSH: push a block?" << std::endl; throw;
	  default: std::cerr << "I_PUSH: don't know what we're pushing (" << t << ")" << std::endl; throw;
	}
	break;
      }
      case I_PUSH_LAST: {
	switch (last_value.type) {
	  case RubyValue::RV_FIXNUM: s.push_integer(last_value.fixnum); break;
	  case RubyValue::RV_SYMBOL: s.push_symbol(last_value.symbol->value); break;
	  case RubyValue::RV_OBJECT: s.push_object(/* XXX gc track? */ last_value.object); break;
	  case RubyValue::RV_NOTHING: /* XXX what? */ std::cerr << "I_PUSH_LAST: given a RV_NOTHING RubyValue (uninitialised RubyValue?)" << std::endl; throw;
	  default: std::cerr << "I_PUSH_LAST, but unknown RubyValue " << "(" << last_value.type << ")" << std::endl; throw;
	}
	break;
      }
      case I_JMP: {
	int32 jump_len = r.read_int32();
	r.skip_bytes(jump_len);
	break;
      }

      case I_IF: {
	RubyValue ev = s.pop_value(context);
	uint32 true_branch_len = r.read_uint32();

	if (ev.truthy(e))
	  break;	// Don't skip over true branch.
	
	last_value = e.NIL;
	r.skip_bytes(true_branch_len);
	break;
      }

      case I_EXCEPTION_BLOCK: {
	uint8 flags = r.read_uint8();
	uint8 no_of_catches = r.read_uint8();
	std::vector<RubyValue> catches;

	Block main_clause = s.pop_block(), rescue(NULL, NULL, NULL), ensure(NULL, NULL, NULL);

	if (flags & E_RESCUE) {
	  rescue = s.pop_block();
	  while (no_of_catches--)
	    catches.push_back(s.pop_value(context));
	}

	if (flags & E_ENSURE)
	  ensure = s.pop_block();
  
	std::cerr << "I_EXCEPTION_BLOCK ";
	if (flags & E_RESCUE) std::cerr << "E_RESCUE ";
	if (flags & E_ENSURE) std::cerr << "E_ENSURE ";

	break;
      }

      default:
	std::cerr << "unknown(" << in << ")" << std::endl; throw;
    }

  }

  return last_value;
}

