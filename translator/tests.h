#ifndef TESTS_H
#define TESTS_H

#include <sstream>
#include <string>
#include <exception>
#define TEST(name) new Test (#name, _##name)
#define BEGIN(name, code, length) Program name = parse_code(code, __LINE__, length)
#define ASSERT(exp) do { if (!assert_throw(__LINE__, #exp, (exp))) return; } while(false)
#define ASSERT_LEN(program, line, lines) do { if (!assert_throw((line), #program " expression count match", (program).expressions.size() == (lines))) return; } while (false)
#define ASSERT_LEN_RETURN(program, line, lines, ret) do { if (!assert_throw((line), #program " expression count match", (program).expressions.size() == (lines))) return(ret); } while (false)
#define ASSERT_TYPE(type, exp) do { if (!assert_throw(__LINE__, "Expression " #exp " of type " #type, dynamic_cast<type*>(exp))) return; } while (false)
#define $(type, name, exp) do { if (!assert_throw(__LINE__, "Expression " #exp " of type " #type, dynamic_cast<type*>(exp))) return; } while(false); type *name = dynamic_cast<type*>(exp)
#define ASSERT_NOPARSE(code) do { bool __anp = false, __anp_op = omit_errors; omit_errors = true; try { parse_code(code); } catch (...) { __anp = true; } omit_errors = __anp_op; if (!assert_throw(__LINE__, "Code is unable to be parsed: \"" code "\"", __anp)) return; } while (false)
// TEST(n) defines a new test, giving the name as `n', pointing to a function named `_n'
// BEGIN(n,c,l) creates a Program object `n', with code `c', and asserts it contains `l' expressions.
// ASSERT(e) calls assert_throw with the current line, the textual code `e' and the expression `e'
// ASSERT_LEN(p,li,l) asserts the number of lines in the Program `p' is `l', and reports it comes from line `li'
// ASSERT_TYPE(t,e) casts tentatively from expression `e' to type `t', asserting the dynamic_cast works.
// $(t,n,e) does as above, but also creates a pointer called `n'. $ is not safe for use in non-enclosed conditionals/similar, as it is creating a variable.
typedef void (*test_pointer)();

int tests_all();
bool assert_throw(int, const char *, bool);

class Test
{
  public:
    Test(const std::string &, const test_pointer &);

    std::string name;
    test_pointer test;
};

class AssertionFailureException : public std::exception
{
  public:
    AssertionFailureException(): line(-1), message() { }
    AssertionFailureException(int _line, const std::string &_message): line(_line), message(_message) { }
    ~AssertionFailureException() throw() { }

    const char *what() const throw() {
      std::ostringstream oss;
      oss << "Assertion failed";
      if (line != -1)
	oss << " on line " << line;
      if (message.length() > 0)
	oss << ": " << message;
      return oss.str().c_str();
    }

    int line;
    std::string message;
};

extern Test *tests[];

#endif

