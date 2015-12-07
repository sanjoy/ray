#ifndef RAY_SUPPORT_HPP
#define RAY_SUPPORT_HPP

#include <memory>
#include <utility>

namespace ray {

void unreachable(const char *msg);

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#define TRACE(msg)                              \
  do {                                          \
    if (ENABLE_TRACE)                           \
      printf("trace(%d): %s\n", __LINE__, msg); \
  } while (false)

#define ETRACE(msg)                             \
  do {                                          \
    printf("trace(%d): %s\n", __LINE__, msg);   \
  } while (false)

#define TRACE_ACTION(action)                    \
  do {                                          \
    if (ENABLE_TRACE) {                         \
      action;                                   \
    }                                           \
  } while (false)

#define printf_cr(fmt, ...) printf(fmt "\n", ## __VA_ARGS__)

}

#endif
