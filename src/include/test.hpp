/// test.hpp - Utilities for writing tests.
///

#ifndef RAY_TEST_HPP
#define RAY_TEST_HPP

#include <vector>
#include <iostream>

namespace ray {

class TestDecl {
  unsigned _failed_count = 0;

public:
  explicit TestDecl(const char* name) {
    std::cout << "Starting '" << name << "'" << std::endl;
  }

  void mark_failed() {
    _failed_count++;
  }

  ~TestDecl() {
    if (!_failed_count)
      std::cout << "All tests passed" << std::endl;
    else
      std::cerr << _failed_count << " tests failed" << std::endl;
  }
};

}

#define CHECK(pred, msg)                                                       \
  do {                                                                         \
    if (!(pred)) {                                                             \
      std::cerr << #pred << " failed in " << __FILE__ << ":" << __LINE__       \
                << std::endl;                                                  \
      std::cerr << msg << std::endl;                                           \
      __current_test_decl.mark_failed();                                       \
    }                                                                          \
  } while (false)

#define CHECK0(pred)                                                           \
  do {                                                                         \
    if (!(pred)) {                                                             \
      std::cerr << #pred << " failed in " << __FILE__ << ":" << __LINE__       \
                << std::endl;                                                  \
      __current_test_decl.mark_failed();                                       \
    }                                                                          \
  } while (false)


#define RAY_INIT_TEST(name) TestDecl __current_test_decl(name);

#endif
