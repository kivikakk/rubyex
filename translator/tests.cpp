#include "main.h"
#include "global.h"
#include "tests.h"

Test::Test(const std::string &_name, const test_pointer &_test): name(_name), test(_test) { }

int assertions, assertion_successes;

int tests_all()
{
  Test **ptr = tests;
  int tests_run = 0, tests_succeed = 0;
  assertions = 0; assertion_successes = 0;

  while (*ptr) {
    Test test = **ptr;
    bool success = true;
    std::string failure_message;

    std::cout << "test: \"" << test.name << "\"" << std::endl;
    try {
      (*test.test)();
    } catch (std::exception &e) {
      failure_message = e.what();
      success = false;
    } catch (...) {
      success = false;
    }

    if (success)
      tests_succeed++;
    else {
      std::cerr << "test: \"" << test.name << "\": failure";
      if (failure_message.length() > 0)
	std::cerr << ": " << failure_message;
      std::cerr << std::endl;
    }

    ptr++, tests_run++;
  }

  std::cerr << tests_run << " test(s) run, " << tests_succeed << " test(s) succeeded." << std::endl;
  std::cerr << assertions << " assertion(s), " << assertion_successes << " assertion(s) succeeded." << std::endl;
  if (tests_run != tests_succeed)
    return 1;
  return 0;
}

void assert_throw(int line, const char *exp, bool eval)
{
  ++assertions;
  std::cout << "asserting: " << exp << std::endl;
  if (!eval)
    throw AssertionFailureException(line, exp);
  ++assertion_successes;
}

void _literals()
{
  BEGIN(integer, "42", 1);
  $(LiteralTypedExpr<int>, integer_expr, integer[0]);
  ASSERT(integer_expr->value == 42);

  BEGIN(floating, "93.12", 1);
  $(LiteralTypedExpr<double>, floating_expr, floating[0]);
  ASSERT(floating_expr->value == 93.12);	// this is quite brittle. what if it's 93.11999...?

  BEGIN(dbl_string, "\"hi!\"", 1);
  $(LiteralTypedExpr<std::string>, dbl_string_expr, dbl_string[0]);
  ASSERT(dbl_string_expr->value == "hi!");

  BEGIN(dbl_string_nl, "\"hi!\nhow are you?\"", 1);
  $(LiteralTypedExpr<std::string>, dbl_string_nl_expr, dbl_string_nl[0]);
  ASSERT(dbl_string_nl_expr->value == "hi!\nhow are you?");
}

void _identifier()
{
  BEGIN(p, "abc", 1);
  $(IdentifierExpr, id, p[0]);
  ASSERT(id->id == "abc");
}

void _symbol()
{
  BEGIN(p_regular, ":bob", 1);
  $(SymbolExpr, sym_regular, p_regular[0]);
  ASSERT(sym_regular->symbol == "bob");

  BEGIN(p_dbq, ":\"double quoted string\"", 1);
  $(SymbolExpr, sym_dbq, p_dbq[0]);
  ASSERT(sym_dbq->symbol == "double quoted string");

  BEGIN(p_nl, ":\"with\nnewline\"", 1);
  $(SymbolExpr, sym_nl, p_nl[0]);
  ASSERT(sym_nl->symbol == "with\nnewline");
}

void _call()
{
  BEGIN(p_explicit, "joe()", 1);
  $(FuncCallExpr, func_explicit, p_explicit[0]);
  ASSERT(func_explicit->name == "joe");
  ASSERT(func_explicit->args.size() == 0);
}

Test *tests[] = {
  TEST(literals),
  TEST(identifier),
  TEST(symbol),
  TEST(call),
  NULL
};
