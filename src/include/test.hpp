/// test.hpp - Utilities for writing tests.
///

#ifndef RAY_TEST_HPP
#define RAY_TEST_HPP

#include <vector>
#include <iostream>

namespace ray {

class TestProperties {
  unsigned _max_failures = 0;
  std::string _run_only;
  std::string _exec_name;

public:
  void set_max_failures(unsigned mf) { _max_failures = mf; }
  unsigned max_failures() const { return _max_failures; }

  void set_run_only(std::string s) { _run_only = std::move(s); }
  const std::string &run_only() const { return _run_only; }

  void set_exec_name(std::string s) { _exec_name = std::move(s); }
  const std::string &exec_name() const { return _exec_name; }

  static TestProperties parse_from_args(int argc, char **argv, bool &success);
};

namespace internal {
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

class MaybeStdErr {
  bool _enabled;

public:
  explicit MaybeStdErr(bool enabled) : _enabled(enabled) {}
  bool enabled() const { return _enabled; }
};

template <typename T>
const MaybeStdErr &operator<<(const MaybeStdErr &out, const T &val) {
  if (out.enabled())
    std::cerr << val;
  return out;
};

MaybeStdErr check_helper(bool pred, const char *pred_str, const char *msg,
                         const char *file_name, unsigned line_num,
                         TestDecl &test_decl);
}
}

#define CHECK(pred, msg)                                                       \
  ray::internal::check_helper(pred, #pred, msg, __FILE__, __LINE__,            \
                              __current_test_decl)

#define CHECK0(pred)                                                           \
  ray::internal::check_helper(pred, #pred, "", __FILE__, __LINE__,             \
                              __current_test_decl)

#define RAY_INIT_TEST(name) ray::internal::TestDecl __current_test_decl(name);

#endif
