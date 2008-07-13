#include "main.h"
#include "global.h"
#include "tests.h"
#include <list>
#include <string>
#include <exception>
#include <algorithm>

bool omit_errors = false;

extern "C" int yywrap(void)
{
  return 1;
}

int main_parser(std::list<std::string> &);
int main_test(std::list<std::string> &);

template <class I, class L, class E> bool find_remove(L &list, const E &element) {
  I it = std::find(list.begin(), list.end(), element);
  if (it != list.end()) {
    list.erase(it);
    return true;
  }
  return false;
}

bool find_remove_string(std::list<std::string> &list, const std::string &element) {
  return find_remove<std::list<std::string>::iterator>(list, element);
}

int main(int argc, char **argv)
{
  std::string invoked_as = argv[0];
  std::list<std::string> arguments;
  for (int i = 1; i < argc; ++i)
    arguments.push_back(argv[i]);

  if (find_remove_string(arguments, "-t"))
    yydebug = 1;

  if (invoked_as.find('/') != std::string::npos)
    invoked_as = invoked_as.substr(invoked_as.find('/') + 1);

  if (invoked_as == "test")
    return main_test(arguments);
  
  return main_parser(arguments);
}

int main_parser(std::list<std::string> &arguments)
{
  Program p;

  int r = yyparse(&p);
  if (r != 0)
    return r;

  if (find_remove_string(arguments, "-b")) {
    // emit bytecode
    p.emit(std::cout);	// XXX: not binary safe?
  }
  else
    p.p();

  return 0;
}

int main_test(std::list<std::string> &arguments)
{
  return tests_all();
}

