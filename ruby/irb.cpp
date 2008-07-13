#include "irb.h"
#include "yywrap.h"
#include "vm/renvironment.h"
#include "vm/process.h"
#include "vm/rstring.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"
#include <iostream>
#include <iomanip>
#include <sstream>

int irb_yywrap();

Program *pb;
int line_count;

int irb(int, char **)
{
  ruby_yywrap_delegate = irb_yywrap;

  std::ostringstream bytecode;
  Program p(bytecode);
  pb = &p;

  RubyEnvironment e;
  Context *c = new Context(&e, RubyValue::from_object(e.main));

  line_count = 0;
  while (true) {
    std::cout << "rxi(main):" << std::setfill('0') << std::setw(3) << ++line_count << ":0> ";

    std::string input;
    std::getline(std::cin, input);
    if (std::cin.eof()) {
      std::cout << std::endl;
      return 0;
    }

    input += "\n";

    yy_scan_string(input.c_str());
    bytecode.str("");
    p.reset_emitted_flag();
    int r = yyparse(&p);
    if (r != 0)
      return r;
    
    std::istringstream iss(bytecode.str());
    Reader reader(iss);

    RubyValue result = process(e, reader, c);
    std::cout << "=> ";
    RubyValue inspection = result.call(e, "inspect");
    std::cout << dynamic_cast<RubyString *>(inspection.object)->string_value
	      << std::endl;
  }

  return 0;
}

int irb_yywrap()
{
  if (pb->emitted_flag())
    return 1;

  std::cout << "rxi(main):" << std::setfill('0') << std::setw(3) << ++line_count << ":0* ";
  std::string input;
  std::getline(std::cin, input);
  if (std::cin.eof()) {
    std::cout << std::endl;
    return 1;
  }

  input += "\n";
  yy_scan_string(input.c_str());

  return 0;
}

