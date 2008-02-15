#ifndef TESTS_H
#define TESTS_H

#include <sstream>
#include <string>
#include <exception>
#define TEST(name) new Test (#name, _##name)
#define BEGIN(name, code, length) Program name = parse_code(code, __LINE__, length)
#define ASSERT(exp) assert_throw(__LINE__, #exp, (exp))
#define ASSERT_LEN(program, line, lines) assert_throw((line), #program " expression count mismatch ", (program).expressions.size() == (lines))
#define ASSERT_TYPE(type, exp) (assert_throw(__LINE__, "Expression " #exp " not of type " #type, dynamic_cast<type*>(exp)), dynamic_cast<type*>(exp))
#define $(type, name, exp) type *name = (assert_throw(__LINE__, "Expression " #exp " not of type " #type, dynamic_cast<type*>(exp)), dynamic_cast<type*>(exp))
// TEST(n) defines a new test, giving the name as `n', pointing to a function named `_n'
// BEGIN(n,c,l) creates a Program object `n', with code `c', and asserts it contains `l' expressions.
// ASSERT(e) calls assert_throw with the current line, the textual code `e' and the expression `e'
// ASSERT_LEN(p,li,l) asserts the number of lines in the Program `p' is `l', and reports it comes from line `li'
// ASSERT_TYPE(t,e) casts tentatively from expression `e' to type `t', asserting the dynamic_cast works.
// $(t,n,e) does as above, but also creates a pointer called `n'.
typedef void (*test_pointer)();

int tests_all();
void assert_throw(int, const char *, bool);

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

