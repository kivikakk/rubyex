#include "emitter.h"
#include <iostream>
#include <typeinfo>
#include <sstream>

void Emitter::push(std::ostream &) const
{
  std::cerr << "Error! " << typeid(*this).name() << " doesn't know how to push()." << std::endl;
}

void Emitter::emit(std::ostream &) const
{
  std::cerr << "Error! " << typeid(*this).name() << " doesn't know how to emit()." << std::endl;
}

unsigned long Emitter::emit_length() const
{
  std::ostringstream oss;
  emit(oss);
  return oss.tellp();
}

unsigned long Emitter::push_length() const
{
  std::ostringstream oss;
  push(oss);
  return oss.tellp();
}

void Emitter::emit_type(std::ostream &o, type_t t)
{
  emit_uint8(o, (uint8)t);
}

void Emitter::emit_instruction(std::ostream &o, instruction_t i)
{
  emit_uint8(o, (uint8)i);
}

void Emitter::emit_uint8(std::ostream &o, uint8 i)
{
  o.write((char *)&i, sizeof(uint8));
}

void Emitter::emit_uint32(std::ostream &o, uint32 i)
{
  uint32 conv = HTONL(i);
  o.write((char *)&conv, sizeof(uint32));
}

void Emitter::emit_int32(std::ostream &o, int32 i)
{
  uint32 conv = HTONL((uint32)i);
  o.write((char *)&conv, sizeof(uint32));
}

void Emitter::emit_float(std::ostream &o, double f)
{
  std::cerr << "TODO: emit floats" << std::endl;
}

void Emitter::emit_bool(std::ostream &o, bool b)
{
  emit_uint8(o, b ? 1 : 0);
}

void Emitter::emit_text(std::ostream &o, const std::string &s)
{
  emit_uint32(o, s.length());
  o.write(s.c_str(), s.length());
}

void Emitter::emit_string(std::ostream &o, const std::string &s)
{
  // Please don't feed me a string with a NUL in it. I'll hate you. Promise.
  o.write(s.c_str(), s.length() + 1 /* include NUL */);
}

