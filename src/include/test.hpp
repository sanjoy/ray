/// test.hpp - Utilities for writing tests.
///

#ifndef RAY_TEST_HPP
#define RAY_TEST_HPP

#include <vector>
#include <iostream>

namespace ray {

class TestDecl {
  unsigned _failed_count = 0;
  unsigned _passed_count = 0;
  const char* _name;

public:
  explicit TestDecl(const char* name) : _name(name) {}

  void mark_as_failed() {
    _failed_count++;
  }

  void mark_as_passed() {
    _passed_count++;
  }

  ~TestDecl() {
    if (!_failed_count) {
      std::cout << _name << ": all (" << _passed_count << ") tests passed"
                << std::endl;
    } else {
      std::cerr << _name << ": " << _failed_count << " tests failed" << std::endl;
      std::cerr << _name << ": " << _passed_count << " tests passed" << std::endl;
    }
  }
};

}

#define CHECK(pred, msg)                                                 \
  do {                                                                   \
    if (!(pred)) {                                                       \
      std::cerr << #pred << " failed in " << __FILE__ << ":" << __LINE__ \
                << std::endl;                                            \
      std::cerr << msg << std::endl;                                     \
      __current_test_decl.mark_as_failed();                              \
    } else {                                                             \
      __current_test_decl.mark_as_passed();                              \
    }                                                                    \
  } while (false)

#define CHECK0(pred)                                                     \
  do {                                                                   \
    if (!(pred)) {                                                       \
      std::cerr << #pred << " failed in " << __FILE__ << ":" << __LINE__ \
                << std::endl;                                            \
      __current_test_decl.mark_as_failed();                              \
    } else {                                                             \
      __current_test_decl.mark_as_passed();                              \
    }                                                                    \
  } while (false)


#define RAY_INIT_TEST(name) TestDecl __current_test_decl(name);

#endif
