#include "irb.h"
#include "yywrap.h"
#include "vm/renvironment.h"
#include "vm/process.h"
#include "vm/rstring.h"
#include "vm/rexception.h"
#include "parser/ast.h"
#include "parser/main.h"
#include "parser/global.h"
#include <iostream>
#include <iomanip>
#include <sstream>

int irb_yywrap();

static Program *pb;
static int line_count;

int irb(int, char **)
{
  ruby_yywrap_delegate = irb_yywrap;

  std::ostringstream bytecode;
  Program p(bytecode);
  pb = &p;

  RubyEnvironment e;
  Context *c = new Context(e, RubyValue::from_object(e.main), e.Object, NULL);

  line_count = 0;
  while (true) {
    std::cout << "rxi(main):" << std::setfill('0') << std::setw(3) << ++line_count << ":" << context_depths << "> ";

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
    int r = yyparse(pb);
    if (r != 0) {
      std::cerr << "Syntax error: " << syntax_error << std::endl;
      continue;
    }
    
    std::istringstream iss(bytecode.str());
    Reader reader(iss);

    RubyValue result;
    try {
      result = process(e, reader, c, NULL);

      std::cout << "=> ";
      RubyValue inspection = result.call(c->binding, "inspect");
      std::cout << dynamic_cast<RubyString *>(inspection.object)->string_value
		<< std::endl;
    } catch (WorldException &w) {
      RubyString *msg = w.exception->get_instance(e, "message").get_special<RubyString>();
      std::string cname = w.exception->get_class()->get_name();
      if (msg)
	std::cerr << msg->string_value << " (" << cname << ")" << std::endl;
      else
	std::cerr << cname << std::endl;
    }
  }

  return 0;
}

int irb_yywrap()
{
  if (pb->emitted_flag())
    return 1;

  std::cout << "rxi(main):" << std::setfill('0') << std::setw(3) << ++line_count << ":" << context_depths << "* ";
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

