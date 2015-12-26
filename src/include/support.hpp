/// support.hpp: Random helpers that don't belong anywhere else.

#ifndef RAY_SUPPORT_HPP
#define RAY_SUPPORT_HPP

#include <memory>
#include <utility>

namespace ray {

/// Abort the program after displaying \p msg.
void unreachable(const char *msg);

/// Port of std::make_unique from C++14
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/// A wrapper around printf that appends a "\n" after the message
/// printf would've printed.
#define printf_cr(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

}

#endif
