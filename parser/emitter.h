#ifndef EMITTER_H
#define EMITTER_H

#include "bytecode.h"
#include <iostream>

// may be platform specific
#include <arpa/inet.h>
#ifndef HTONL
#define HTONL(i) htonl(i)
#endif

#ifndef NTOHL
#define NTOHL(i) ntohl(i)
#endif

#ifndef HTONS
#define HTONS(i) htons(i)
#endif

#ifndef NTOHS
#define NTOHS(i) ntohs(i)
#endif

typedef unsigned char uint8;
typedef unsigned long uint32;
typedef long int32;

class Emitter
{
  public:
    virtual ~Emitter() { }

    virtual void emit(std::ostream &) const;
    virtual void push(std::ostream &) const;

    unsigned long emit_length() const;
    unsigned long push_length() const;

  protected:
    static void emit_type(std::ostream &, type_t);
    static void emit_instruction(std::ostream &, instruction_t);
    static void emit_uint8(std::ostream &, uint8);
    static void emit_uint32(std::ostream &, uint32);
    static void emit_int32(std::ostream &, int32);
    static void emit_float(std::ostream &, double);
    static void emit_bool(std::ostream &, bool);
    static void emit_text(std::ostream &, const std::string &);
    static void emit_string(std::ostream &, const std::string &);
};

#endif

