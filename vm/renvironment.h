#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
#include <exception>
#include "rclass.h"
#include "rarray.h"
#include "rmodule.h"
#include "rstring.h"
#include "rexception.h"
#include "gc.h"

class RubyEnvironment
{
  public:
    RubyEnvironment();
    ~RubyEnvironment();

    bool global_exists(const std::string &) const;
    RubyValue get_global_by_name(const std::string &) const;
    void set_global_by_name(const std::string &, RubyValue);
    void set_global_by_name(const std::string &, RubyObject *);
    void set_readonly_global_by_name(const std::string &, RubyValue);
    void set_readonly_global_by_name(const std::string &, RubyObject *);

    const std::string &get_name_by_global(RubyValue) const;
    inline RubyValue get_truth(bool _t) const { return _t ? TRUE : FALSE; }
    inline RubyValue get_string(const std::string &_s) {
      /* String interning here. Maybe. Or in RubyString's ctor. */
      return O2V(gc.track(new RubyString(*this, _s)));
    }

    RubyObject *errno_exception(linked_ptr<Binding> &, int, const char *);

    RubySymbol *get_symbol(const std::string &);

    RubyClass *Object, *Module, *Class;
    RubyClass *_Binding, *Symbol, *Fixnum, *Float, *String;
    RubyClass *Array, *Hash, *Range, *IO, *File;
    RubyClass *Regexp, *MatchData;

    RubyClass *Exception;
      RubyClass *ScriptError;
	RubyClass *LoadError;
	RubyClass *NotImplementedError;
	RubyClass *SyntaxError;
      RubyClass *StandardError;
	RubyClass *ArgumentError;
	RubyClass *IOError;
	RubyClass *IndexError;
	RubyClass *LocalJumpError;
	RubyClass *NameError;
	  RubyClass *NoMethodError;
	RubyClass *RuntimeError;
	RubyClass *SystemCallError;
	RubyClass *TypeError;

    RubyModule *Kernel, *Comparable;

    RubyArray *RubyPath, *RubyLoaded;

    RubyObject *main;
    RubyValue TRUE, FALSE, NIL;

    GarbageCollector gc;

  protected:
    class _GlobalSettings {
      public:
	_GlobalSettings();
	_GlobalSettings(bool);

	bool readonly;
    };

    std::map<std::string, RubyValue> globals;
    std::map<std::string, _GlobalSettings> global_settings;
    std::map<std::string, RubySymbol *> symbols;
};

class CannotFindGlobalError : public std::exception
{ };

class CannotChangeReadonlyError : public std::exception
{ };

#endif

