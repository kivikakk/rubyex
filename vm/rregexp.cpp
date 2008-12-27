#include "rregexp.h"
#include <sstream>

RubyValue regexp_initialize(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue regexp_match(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue regexp_source(linked_ptr<Binding> &, RubyValue);
RubyValue regexp_inspect(linked_ptr<Binding> &, RubyValue);

RubyValue match_data_new(linked_ptr<Binding> &, RubyValue);
RubyValue match_data_begin(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue match_data_end(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue match_data_index_op(linked_ptr<Binding> &, RubyValue, const std::vector<RubyValue> &);
RubyValue match_data_captures(linked_ptr<Binding> &, RubyValue);

void RubyRegexpEI::init(RubyEnvironment &_e)
{
  RubyClass *rb_cRegexp = _e.gc.track<RubyClass>(new RubyClass(_e, "Regexp"));

  rb_cRegexp->add_method("initialize", RubyMethod::Create(regexp_initialize, 1));
  rb_cRegexp->add_method("match", RubyMethod::Create(regexp_match, 1));
  rb_cRegexp->add_method("source", RubyMethod::Create(regexp_source));
  rb_cRegexp->add_method("inspect", RubyMethod::Create(regexp_inspect));

  _e.set_global_by_name("Regexp", rb_cRegexp);
  _e.Regexp = rb_cRegexp;

  RubyClass *rb_cMatchData = _e.gc.track<RubyClass>(new RubyClass(_e, "MatchData"));

  rb_cMatchData->add_metaclass_method(_e, "new", RubyMethod::Create(match_data_new));

  rb_cMatchData->add_method("[]", RubyMethod::Create(match_data_index_op, 1));
  rb_cMatchData->add_method("begin", RubyMethod::Create(match_data_begin, 1));
  rb_cMatchData->add_method("end", RubyMethod::Create(match_data_end, 1));
  rb_cMatchData->add_method("captures", RubyMethod::Create(match_data_captures));

  _e.set_global_by_name("MatchData", rb_cMatchData);
  _e.MatchData = rb_cMatchData;
}

RubyValue regexp_initialize(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyRegexp *exp = _self.get_special<RubyRegexp>();
  exp->initialize(_b, _args[0].get_string());
  return _b->environment.NIL;
}

RubyValue regexp_match(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  RubyRegexp *exp = _self.get_special<RubyRegexp>();
  const std::string &str = _args[0].get_string();
  OnigRegion *mr = onig_region_new();

  int result = onig_search(	exp->reg, (UChar *)str.c_str(), (UChar *)str.c_str() + str.size(),
				(UChar *)str.c_str(), (UChar *)str.c_str() + str.size(), mr, 0);
  if (result == ONIG_MISMATCH)
    return _b->environment.NIL;

  // XXX I probably need to use this: onig_capture_tree_traverse ?

  RubyMatchData *md = _b->environment.gc.track<RubyMatchData>(new RubyMatchData(_b->environment));
  for (int i = 0; i < mr->num_regs; ++i)
    md->add_region(str.substr(mr->beg[i], mr->end[i] - mr->beg[i]), mr->beg[i], mr->end[i]);

  onig_region_free(mr, 1);

  return O2V(md);
}

RubyValue regexp_source(linked_ptr<Binding> &_b, RubyValue _self) {
  return _b->environment.get_string(_self.get_special<RubyRegexp>()->phrase);
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

RubyValue match_data_new(linked_ptr<Binding> &_b, RubyValue _self) {
  throw WorldException(_b, _b->environment.NoMethodError, "undefined method `new': XXX - use undef/remove");
  // XXX
}

RubyValue match_data_begin(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  // RESUME XXX CONTINUE
}

RubyValue match_data_end(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
}

RubyValue match_data_index_op(linked_ptr<Binding> &_b, RubyValue _self, const std::vector<RubyValue> &_args) {
  // XXX bounds-checking wtf?
  return _b->environment.get_string(_self.get_special<RubyMatchData>()->regions[_args[0].get_fixnum()].data);
}

RubyValue match_data_captures(linked_ptr<Binding> &_b, RubyValue _self) {
  std::vector<RubyValue> captures;
  std::vector<ruby_match_region_t> &regions = _self.get_special<RubyMatchData>()->regions;
  for (std::vector<ruby_match_region_t>::const_iterator it = regions.begin() + 1; it != regions.end(); ++it)
    captures.push_back(_b->environment.get_string(it->data));

  return O2V(_b->environment.gc.track(new RubyArray(_b->environment, captures)));
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
		ONIG_OPTION_NONE, ONIG_ENCODING_ASCII /* XXX later: encodings? */,
		ONIG_SYNTAX_RUBY, &oei);
  if (new_res != ONIG_NORMAL) {
    UChar *cb = new UChar[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(cb, new_res, oei);
    WorldException ex(_b, _b->environment.SyntaxError, (const char *)cb);
    delete [] cb;
    throw ex;
  }
}

RubyMatchData::RubyMatchData(RubyEnvironment &_e): RubyObject(_e.MatchData)
{ }

void RubyMatchData::add_region(const std::string &_data, int _beg, int _end) {
  ruby_match_region_t mr = {_data, _beg, _end};
  regions.push_back(mr);
}

