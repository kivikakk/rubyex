#include "process.h"
#include "stack.h"
#include "rvalue.h"
#include "rstring.h"
#include "rnumeric.h"
#include "rmethod.h"
#include "rexception.h"

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
	  case T_SYMBOL: last_value = S2V(e.get_symbol(r.read_string())); break;
	  case T_INTEGER_LITERAL: last_value = F2V(r.read_int32()); break;
	  case T_FLOATING_LITERAL: last_value = O2V(e.gc.track(new RubyFloating(e, r.read_floating()))); break;
	  case T_BOOLEAN_LITERAL: last_value = r.read_bool() ? e.TRUE : e.FALSE; break;
	  case T_STRING_LITERAL: last_value = e.get_string(r.read_text()); break;
	  case T_NIL_LITERAL: last_value = e.NIL; break;
	  default: throw WorldException(context->binding, e.RuntimeError, "process(): unknown type");
	}
	break;
      }
      case I_SCOPE_ROOT:
      case I_SCOPE_CONTEXT: {
	RubyModule *search = e.Object;
	if (in == I_SCOPE_CONTEXT) {
	  RubyValue v = s.pop_value(context);
	  search = v.get_special<RubyModule>();
	  if (!search)
	    throw WorldException(context->binding, e.TypeError, v.inspect(context->binding) + " is not a class/module");
	}

	std::string name = r.read_string();
	last_value = search->resolve_constant(context->binding, name, false);
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
	RubyMethod *method = is_target ? target.get_method(context->binding, name) : context->get_method(name);

	last_value = is_block ? 
	  method->call(context->binding, is_target ? target : context->binding->context, arguments, block) :
	  method->call(context->binding, is_target ? target : context->binding->context, arguments);	// boom
	break;
      }
      case I_FALSITY: {
	last_value = e.get_truth(!s.pop_value(context).truthy(e));
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
	if (!yield_block)
	  throw WorldException(context->binding, e.LocalJumpError, "no block given");

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
	  r += it->to_s(context->binding);

	last_value = e.get_string(r);
	break;
      }
      case I_CLASS:
      case I_CLASS_INHERIT: {
	bool inherits = in == I_CLASS_INHERIT;

	std::string name = s.pop_identifier();
	RubyClass *super = inherits ? s.pop_value(context).get_special<RubyClass>() : e.Object;
	Block code = s.pop_block();

	if (!super)
	  throw WorldException(context->binding, e.TypeError, "superclass must be a Class (XXX given)");	// XXX minor fix.
	
	// XXX don't use def_target here, but something else?
	bool already_exists = context->binding->def_target->has_constant(name);
	RubyClass *c = already_exists ? context->binding->def_target->get_constant(name).get_special<RubyClass>() : new RubyClass(e, name, super, context->binding->def_target);
	if (!c || c->get_class() != e.Class)
	  throw WorldException(context->binding, e.TypeError, name + " is not a class");

	// Specify a new context, with a new def_target, etc.
	Context *class_def_ctx = new Context(e, O2V(c), c, context);

	try {
	  last_value = code.call(class_def_ctx);
	} catch(WorldException &) {
	  if (!already_exists)
	    delete c;
	  delete class_def_ctx;
	  throw;
	}

	delete class_def_ctx;
	if (!already_exists)
	  context->binding->def_target->set_constant(name, O2V(c));		// XXX what about subclasses, classes of modules, etc.?

	break;
      }
      case I_MODULE: {
	std::string name = s.pop_identifier();
	Block code = s.pop_block();

	// XXX don't use def_target here, but something else?
	bool already_exists = context->binding->def_target->has_constant(name);
	RubyModule *m = already_exists ? context->binding->def_target->get_constant(name).get_special<RubyModule>() : new RubyModule(e, name, context->binding->def_target);
	if (!m || m->get_class() != e.Module)
	  throw WorldException(context->binding, e.TypeError, name + " is not a module");

	// Specify a new context, with a new def_target, etc.
	Context *module_def_ctx = new Context(e, O2V(m), m, context);

	try {
	  last_value = code.call(module_def_ctx);
	} catch(WorldException &) {
	  if (!already_exists)
	    delete m;
	  delete module_def_ctx;
	  throw;
	}

	delete module_def_ctx;
	if (!already_exists)
	  context->binding->def_target->set_constant(name, O2V(m));		// XXX what modules of modules/classes, etc.?

	break;
      }

      case I_PUSH: {
	type_t t = r.read_type();
	switch (t) {
	  case T_IDENTIFIER: s.push_identifier(r.read_string()); break;
	  case T_SYMBOL: s.push_symbol(r.read_string()); break;
	  case T_INTEGER_LITERAL: s.push_integer(r.read_int32()); break;
	  case T_FLOATING_LITERAL: s.push_object(e.gc.track(new RubyFloating(e, r.read_floating()))); break;
	  case T_BOOLEAN_LITERAL: s.push_object(r.read_bool() ? e.TRUE.object : e.FALSE.object); break;
	  case T_STRING_LITERAL: s.push_object(e.get_string(r.read_text()).object); break;
	  case T_NIL_LITERAL: s.push_object(e.NIL.object); break;
	  case T_BLOCK: throw WorldException(context->binding, e.RuntimeError, "process(): trying to push a block");
	  default: throw WorldException(context->binding, e.RuntimeError, "process(): trying to push something we're not sure of");
	}
	break;
      }
      case I_PUSH_LAST: {
	switch (last_value.type) {
	  case RubyValue::RV_FIXNUM: s.push_integer(last_value.fixnum); break;
	  case RubyValue::RV_SYMBOL: s.push_symbol(last_value.symbol->value); break;
	  case RubyValue::RV_OBJECT: s.push_object(/* XXX gc track? */ last_value.object); break;
	  case RubyValue::RV_NOTHING: throw WorldException(context->binding, e.RuntimeError, "process(): given RV_NOTHING to I_PUSH_LAST");
	  default: throw WorldException(context->binding, e.RuntimeError, "process(): given something unknown to I_PUSH_LAST");
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
	std::vector<RubyClass *> catches;
	// I've heard the catches could even be Modules, but how?

	Block main_clause = s.pop_block(), rescue_clause(NULL, NULL, NULL), else_clause(NULL, NULL, NULL), ensure_clause(NULL, NULL, NULL);

	if (flags & E_RESCUE) {
	  rescue_clause = s.pop_block();
	  while (no_of_catches--) {
	    RubyValue v = s.pop_value(context);
	    RubyClass *c = v.get_special<RubyClass>();
	    if (!c)
	      throw WorldException(context->binding, e.TypeError, "class or module required for rescue clause");
	    catches.push_back(c);
	  }
	  if (catches.size() == 0)
	    catches.push_back(e.StandardError);
	}

	if (flags & E_ELSE)
	  else_clause = s.pop_block();

	if (flags & E_ENSURE)
	  ensure_clause = s.pop_block();

	bool handled = false;
	try {
	  last_value = main_clause.call(context);
	} catch (WorldException &w) {
	  for (std::vector<RubyClass *>::iterator it = catches.begin(); it != catches.end(); ++it)
	    if (w.exception->get_class()->has_ancestor(*it)) {
	      // The rescue block handles this.
	      // If we had no rescue block, we'd never get here since there'd be no catches.
	      std::vector<RubyValue> args;
	      args.push_back(O2V(w.exception));

	      last_value = rescue_clause.call(context, args);
	      handled = true;
	      break;
	    }

	  if (!handled) {
	    // Rescue block failed or didn't exist.
	    if (flags & E_ENSURE)
	      ensure_clause.call(context);
	    throw; // Escalate.
	  }
	}

	if (!handled)
	  // No rescue block was called.
	  if (flags & E_ELSE)
	    last_value = else_clause.call(context);

	if (flags & E_ENSURE)
	  ensure_clause.call(context);

	break;
      }

      default:
	throw WorldException(context->binding, e.RuntimeError, "read unknown VM instruction");
    }

  }

  return last_value;
}

