#include "twophase.h"
#include "irb.h"
#include <string>
#include <list>

bool omit_errors = false; 	// required for parser.

// This is copied from parser/main.cxx .. DRY? But where?
template <class I, class L, class E> bool find_remove(L &list, const E &element)
{
  I it = std::find(list.begin(), list.end(), element);
  if (it != list.end()) {
    list.erase(it);
    return true;
  }
  return false;
}

bool find_remove_string(std::list<std::string> &list, const std::string &element)
{
  return find_remove<std::list<std::string>::iterator>(list, element);
}

int main(int argc, char **argv)
{
  std::string invoked_as = argv[0];
  std::list<std::string> arguments;
  for (int i = 1; i < argc; ++i)
    arguments.push_back(argv[i]);

  if (find_remove_string(arguments, "-irb"))
    return irb(argc, argv);

  return twophase(argc, argv);
}

