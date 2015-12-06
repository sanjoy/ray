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
}

#endif
