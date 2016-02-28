#include "test.hpp"

using namespace ray;
using namespace ray::internal;

MaybeStdErr ray::internal::check_helper(bool pred, const char *pred_str,
                                        const char *msg, const char *file_name,
                                        unsigned line_num,
                                        TestDecl &test_decl) {
  if (pred) {
    test_decl.mark_as_passed();
    return MaybeStdErr(false);
  }
  std::cerr << pred_str << " failed in " << file_name << ":" << line_num
            << std::endl;
  if (msg[0])
    std::cerr << msg << std::endl;
  test_decl.mark_as_failed();
  return MaybeStdErr(true);
}
