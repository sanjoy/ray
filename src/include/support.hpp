/// support.hpp: Random helpers that don't belong anywhere else.

#ifndef RAY_SUPPORT_HPP
#define RAY_SUPPORT_HPP

#include <memory>
#include <utility>
#include <string>
#include <sstream>

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

template <typename T0>
static inline std::string generate_description_string(const char *kind,
                                                      const char *field_name_0,
                                                      const T0 &field_val_0) {
  std::string result;
  std::stringstream ss(result);
  ss << "(kind " << field_name_0 << ": " << field_val_0 << ")";
  return result;
}

template <typename T0, typename T1>
static inline std::string
generate_description_string(const char *kind, const char *field_name_0,
                            const T0 &field_val_0, const char *field_name_1,
                            const T1 &field_val_1) {
  std::string result;
  std::stringstream ss(result);
  ss << "(kind " << field_name_0 << ": " << field_val_0 << " " << field_name_1
     << ": " << field_val_1 << ")";
  return result;
}

template <typename T0, typename T1, typename T2>
static inline std::string
generate_description_string(const char *kind, const char *field_name_0,
                            const T0 &field_val_0, const char *field_name_1,
                            const T1 &field_val_1, const char *field_name_2,
                            const T2 &field_val_2) {
  std::string result;
  std::stringstream ss(result);
  ss << "(kind " << field_name_0 << ": " << field_val_0 << " " << field_name_1
     << ": " << field_val_1 << " " << field_name_2 << ": " << field_val_2
     << ")";
  return result;
}

template <typename T0, typename T1, typename T2, typename T3>
static inline std::string generate_description_string(
    const char *kind, const char *field_name_0, const T0 &field_val_0,
    const char *field_name_1, const T1 &field_val_1, const char *field_name_2,
    const T2 &field_val_2, const char *field_name_3, const T3 &field_val_3) {
  std::string result;
  std::stringstream ss(result);
  ss << "(kind " << field_name_0 << ": " << field_val_0 << " " << field_name_1
     << ": " << field_val_1 << " " << field_name_2 << ": " << field_val_2 << " "
     << field_name_3 << ": " << field_val_3 << ")";
  return result;
}
}

#endif
