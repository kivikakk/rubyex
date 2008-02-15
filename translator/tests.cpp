#include "main.h"
#include "global.h"
#include "tests.h"

Test::Test(const std::string &_name, const test_pointer &_test): name(_name), test(_test) { }

int tests_all()
{
  Test **ptr = tests;
  int tests_run = 0, tests_succeed = 0;

  while (*ptr) {
    Test test = **ptr;
    bool success = true;
    std::string failure_message;

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
  if (tests_run != tests_succeed)
    return 1;
  return 0;
}

void assert_throw(int line, const char *exp, bool eval)
{
  if (!eval)
    throw AssertionFailureException(line, exp);
}

void _identifier()
{
  BEGIN(p, "abc");
  ASSERT_LEN(p, 1);
  IdentifierExpr *id = ASSERT_TYPE(IdentifierExpr, p[0]);
  ASSERT(id->id == "abc");
}

void _symbol()
{
  {
    BEGIN(p, ":bob");
    ASSERT_LEN(p, 1);
    SymbolExpr *sym = ASSERT_TYPE(SymbolExpr, p[0]);
    ASSERT(sym->symbol == "bob");
  }
}

Test *tests[] = {
  TEST(identifier),
  TEST(symbol),
  NULL
};
