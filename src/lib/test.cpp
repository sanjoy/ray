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

TestProperties TestProperties::parse_from_args(int argc, char **argv,
                                               bool &success) {
  TestProperties props;

  props.set_exec_name(argv[0]);
  argc--;
  argv++;

  bool found_non_pos_arg = false;

  while (argc != 0) {
    char *current = argv[0];
    argc--;
    argv++;

    if (current[0] == '-' && current[1] == '-') {
      if (!strcmp(current, "--max-failures")) {
        if (argc == 0) {
          success = false;
          return props;
        }

        char *max_failures_str = argv[0];
        argc--;
        argv++;

        char *endptr;
        long val = strtol(max_failures_str, &endptr, 10);
        if (endptr != &max_failures_str[strlen(max_failures_str)] || val < 1) {
          success = false;
          return props;
        }
        props.set_max_failures(val);
      } else {
        success = false;
        return props;
      }
    } else {
      if (found_non_pos_arg) {
        success = false;
        return props;
      }
      props.set_run_only(current);
      found_non_pos_arg = true;
    }
  }

  success = true;
  return props;
}
