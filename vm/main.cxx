#include "process.h"

int main(int argc, char **argv)
{
  RubyEnvironment e;
  Reader r(std::cin);
  Context *c = new Context(e);

  process(e, r, c, NULL);

  return 0;
}

