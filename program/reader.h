#ifndef READER_H
#define READER_H

#include "bytecode.h"
#include <iostream>
#include <string>

// copied from emitter.h: fix?
// may be platform specific
#include <arpa/inet.h>
#define HTONL(i) htonl(i)
#define NTOHL(i) ntohl(i)
#define HTONS(i) htons(i)
#define NTOHS(i) ntohs(i)
typedef unsigned char uint8;
typedef unsigned long uint32;
typedef long int32;

class Reader
{
  public:
    Reader(std::istream &);

    bool eof() const;

    instruction_t read_instruction();
    type_t read_type();
    uint8 read_uint8();
    uint32 read_uint32();
    int32 read_int32();
    double read_float();
    bool read_bool();
    std::string read_text();
    std::string read_string();
    std::string read_bytes(unsigned long);

  protected:
    std::istream &input;
};

#endif

