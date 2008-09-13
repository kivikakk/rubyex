#include "rsymbol.h"
#include "rvalue.h"
#include "rclass.h"
#include "rmethod.h"
#include "rstring.h"

RubyValue symbol_inspect(linked_ptr<Binding> &, RubyValue);
RubyValue symbol_to_s(linked_ptr<Binding> &, RubyValue);

void RubySymbolEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cSymbol = _e.gc.track(new RubyClass(_e, "Symbol"));

  rb_cSymbol->add_method("inspect", RubyMethod::Create(symbol_inspect));
  rb_cSymbol->add_method("to_s", RubyMethod::Create(symbol_to_s));

  _e.set_global_by_name("Symbol", rb_cSymbol);
  _e.Symbol = rb_cSymbol;
}

RubyValue symbol_inspect(linked_ptr<Binding> &_b, RubyValue _self)
{
  return _b->environment.get_string(":" + _self.symbol->value);
}

RubyValue symbol_to_s(linked_ptr<Binding> &_b, RubyValue _self)
{
  return _b->environment.get_string(_self.symbol->value);
}

RubySymbol::RubySymbol(const std::string &_value): value(_value)
{ }

