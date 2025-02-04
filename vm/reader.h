#ifndef READER_H
#define READER_H

#include "bytecode.h"
#include <iostream>
#include <string>

// copied from emitter.h: fix?
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
    double read_floating();
    bool read_bool();
    std::string read_text();
    std::string read_string();
    std::string read_bytes(unsigned long);

    void skip_bytes(long);

  protected:
    std::istream &input;
};

#endif

