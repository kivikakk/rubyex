#include "reader.h"

Reader::Reader(std::istream &_input): input(_input)
{ }

bool Reader::eof() const
{
  return input.eof();
}


instruction_t Reader::read_instruction()
{
  return (instruction_t)read_uint8();
}

type_t Reader::read_type()
{
  return (type_t)read_uint8();
}

uint8 Reader::read_uint8()
{
  uint8 o;
  input.read((char *)&o, sizeof(uint8));
  return o;
}

uint32 Reader::read_uint32()
{
  uint32 o;
  input.read((char *)&o, sizeof(uint32));
  return NTOHL(o);
}

int32 Reader::read_int32()
{
  int32 o;
  input.read((char *)&o, sizeof(int32));
  return NTOHL(o);
}

double Reader::read_float()
{
  double o;
  input.read((char *)&o, sizeof(double));
  return o;
}

bool Reader::read_bool()
{
  return read_uint8() == 1;
}

std::string Reader::read_text()
{
  uint32 l = read_uint32();
  return read_bytes(l);
}

std::string Reader::read_string()
{
  // XXX: optimize. read in blocks, then reverse maybe?
  std::string s;
  char c;
  while ((c = input.get()))
    s += c;
  return s;
}

std::string Reader::read_bytes(unsigned long l)
{
  char *c = new char[l];
  input.read(c, l);
  std::string s = std::string(c, l);
  delete [] c;
  return s;
}
