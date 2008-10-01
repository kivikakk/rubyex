#include "rregexp.h"
#include <sstream>

RubyValue regexp_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue regexp_match(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue regexp_inspect(linked_ptr<Binding> &, RubyValue);

void RubyRegexpEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cRegexp = _e.gc.track<RubyClass>(new RubyClass(_e, "Regexp"));

  rb_cRegexp->add_method("initialize", RubyMethod::Create(regexp_initialize, 1));
  rb_cRegexp->add_method("match", RubyMethod::Create(regexp_match, 1));
  rb_cRegexp->add_method("inspect", RubyMethod::Create(regexp_inspect));

  _e.set_global_by_name("Regexp", rb_cRegexp);
  _e.Regexp = rb_cRegexp;
}

RubyValue regexp_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyRegexp *exp = _self.get_special<RubyRegexp>();
  exp->initialize(_b, _args[0].get_string());
  return _b->environment.NIL;
}

RubyValue regexp_match(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyRegexp *exp = _self.get_special<RubyRegexp>();
  const std::string &str = _args[0].get_string();

  int result = onig_match(	exp->reg, (UChar *)str.c_str(), (UChar *)str.c_str() + str.size(),
				(UChar *)str.c_str(), NULL /* XXX: MatchData */, 0);
  if (result == ONIG_MISMATCH)
    return _b->environment.NIL;
  return _b->environment.TRUE;	/* XXX CONTINUE here! */
}

RubyValue regexp_inspect(linked_ptr<Binding> &_b, RubyValue _self) {
  RubyRegexp *exp = _self.get_special<RubyRegexp>();
  std::string &phrase = exp->phrase;

  std::ostringstream oss;
  oss << '/';
  for (unsigned int i = 0; i < phrase.size(); ++i)
    if (phrase[i] == '/')
      oss << "\\/";
    else
      oss << phrase[i];
  oss << '/';
  return _b->environment.get_string(oss.str());
}

RubyRegexp::RubyRegexp(RubyEnvironment &_e): RubyObject(_e.Regexp)
{ }

RubyRegexp::RubyRegexp(linked_ptr<Binding> &_b, const std::string &_phrase): RubyObject(_b->environment.Regexp) {
  initialize(_b, _phrase);
}

RubyRegexp::~RubyRegexp() {
  onig_free(reg);
}

void RubyRegexp::initialize(linked_ptr<Binding> &_b, const std::string &_phrase) {
  phrase = _phrase;
  OnigErrorInfo oei;

  int new_res =
    onig_new(	&reg, (UChar *)phrase.c_str(), (UChar *)phrase.c_str() + phrase.size(),
		ONIG_OPTION_NONE, ONIG_ENCODING_ASCII /* XXX later. */,
		ONIG_SYNTAX_RUBY, &oei);
  if (new_res != ONIG_NORMAL) {
    UChar *cb = new UChar[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(cb, new_res, oei);
    WorldException ex(_b, _b->environment.SyntaxError, (const char *)cb);
    delete [] cb;
    throw ex;
  }
}

