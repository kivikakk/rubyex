#include "rsymbol.h"
#include "rvalue.h"
#include "rclass.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue symbol_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue symbol_to_s(linked_ptr<Binding> &, RubyValue);

void RubySymbolEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cSymbol = RubyClass::create_class(_e, "Symbol");

  rb_cSymbol->add_method("inspect", RubyMethod::Create(symbol_inspect));
  rb_cSymbol->add_method("to_s", RubyMethod::Create(symbol_to_s));

  _e.add_class("Symbol", rb_cSymbol);
  _e.Symbol = rb_cSymbol;
}

RubyValue symbol_inspect(linked_ptr<Binding> &_b, RubyValue _self)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, ":" + _self.symbol->value)));
}

RubyValue symbol_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  return RubyValue::from_object(_b->environment.gc.track(new RubyString(_b->environment, _self.symbol->value)));
}

RubySymbol::RubySymbol(const std::string &_value): value(_value)
{ }

