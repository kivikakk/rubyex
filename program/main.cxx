#include "process.h"

int main(int argc, char **argv)
{
  RubyEnvironment e;
  Reader r(std::cin);
  std::vector<Context *> cs;
  Context *c = new Context(&e, RubyValue::from_object(e.main));

  process(e, r, cs, c);

  return 0;
}

