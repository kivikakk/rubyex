#ifndef RENVIRONMENT_H
#define RENVIRONMENT_H

#include <string>
#include <map>
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

    RubySymbol *get_symbol(const std::string &);

    RubyClass *Object, *Module, *Class, *Binding, *Symbol, *Fixnum, *String;
    RubyModule *Kernel;

    RubyObject *main;
    RubyValue TRUE, FALSE, NIL;

    GarbageCollector gc;

  protected:
    std::map<std::string, RubyClass *> classes;
    std::map<std::string, RubyModule *> modules;
    std::map<std::string, RubySymbol *> symbols;
};

#endif

