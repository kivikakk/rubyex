#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
#include <exception>
#include "rclass.h"
#include "rmodule.h"
#include "gc.h"

class RubyEnvironment
{
  public:
    RubyEnvironment();

    void add_class(const std::string &, RubyClass *);
    void add_module(const std::string &, RubyModule *);

    bool global_exists(const std::string &) const;
    RubyObject *get_global_by_name(const std::string &) const;
    bool class_exists(const std::string &) const;
    RubyClass *get_class_by_name(const std::string &) const;
    bool module_exists(const std::string &) const;
    RubyModule *get_module_by_name(const std::string &) const;

    const std::string &get_name_by_global(RubyObject *) const;
    inline RubyValue get_truth(bool _t) const { return _t ? TRUE : FALSE; }

    RubySymbol *get_symbol(const std::string &);

    RubyClass *Object, *Module, *Class;
    RubyClass *Binding, *Symbol, *Fixnum, *String;
    RubyClass *Array, *Hash;
    RubyModule *Kernel;

    RubyObject *main;
    RubyValue TRUE, FALSE, NIL;

    GarbageCollector gc;

  protected:
    std::map<std::string, RubyClass *> classes;
    std::map<std::string, RubyModule *> modules;
    std::map<std::string, RubySymbol *> symbols;
};

class CannotFindGlobalError : public std::exception
{ };

#endif

