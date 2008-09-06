#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
#include <exception>
#include "rclass.h"
#include "rmodule.h"
#include "rstring.h"
#include "rexception.h"
#include "gc.h"

class RubyEnvironment
{
  public:
    RubyEnvironment();

    bool global_exists(const std::string &) const;
    RubyValue get_global_by_name(const std::string &) const;
    void set_global_by_name(const std::string &, RubyValue);
    void set_global_by_name(const std::string &, RubyObject *);

    const std::string &get_name_by_global(RubyValue) const;
    inline RubyValue get_truth(bool _t) const { return _t ? TRUE : FALSE; }
    inline RubyValue get_string(const std::string &_s) {
      /* String interning here. Maybe. Or in RubyString's ctor. */
      return O2V(gc.track(new RubyString(*this, _s)));
    }

    RubyObject *errno_exception(linked_ptr<Binding> &, int, const char *);

    RubySymbol *get_symbol(const std::string &);

    RubyClass *Object, *Module, *Class;
    RubyClass *_Binding, *Symbol, *Fixnum, *String;
    RubyClass *Array, *Hash, *Range, *IO, *File;

    RubyClass *Exception;
      RubyClass *ScriptError;
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

    RubyObject *main;
    RubyValue TRUE, FALSE, NIL;

    GarbageCollector gc;

  protected:
    std::map<std::string, RubyValue> globals;
    std::map<std::string, RubySymbol *> symbols;
};

class CannotFindGlobalError : public std::exception
{ };

#endif

