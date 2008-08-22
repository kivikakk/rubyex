#include "process.h"

int main(int argc, char **argv)
{
  RubyEnvironment e;
  Reader r(std::cin);
  Context *c = new Context(e, RubyValue::from_object(e.main), e.Object);

  process(e, r, c, NULL);

  return 0;
}

