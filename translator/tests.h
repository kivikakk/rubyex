#ifndef TESTS_H
#define TESTS_H

#include <sstream>
#include <string>
#include <exception>
#define TEST(name) new Test (#name, _##name)
#define BEGIN(name, code) Program name = parse_code(code)
#define ASSERT(exp) assert_throw(__LINE__, #exp, (exp))
#define ASSERT_LEN(p, l) assert_throw(__LINE__, #p " doesn't have " #l " expression(s)", (p).expressions.size() == (l))
#define ASSERT_TYPE(type, exp) (assert_throw(__LINE__, "Expression " #exp " not of type " #type, dynamic_cast<type*>(exp)), dynamic_cast<type*>(exp))
typedef void (*test_pointer)();

int tests_all();

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

