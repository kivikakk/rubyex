#include "pretty.h"
#include <typeinfo>
#include <iostream>

PrettyPrint::~PrettyPrint() { }

void PrettyPrint::p() const
{
  std::cerr << "Error! " << typeid(*this).name() << " doesn't know how to p()." << std::endl;
}
