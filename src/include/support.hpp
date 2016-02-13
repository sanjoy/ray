/// support.hpp: Random helpers that don't belong anywhere else.

#ifndef RAY_SUPPORT_HPP
#define RAY_SUPPORT_HPP

#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>

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

enum class IndentActionTy { indent = 42 };
constexpr IndentActionTy indent = IndentActionTy::indent;

class ActiveLogger {
  std::stringstream _stream;
  bool _enabled;
  unsigned _indent = 0;

public:
  explicit ActiveLogger(bool enabled) : _enabled(enabled) {}
  std::stringstream &stream() { return _stream; }
  bool is_enabled() const { return _enabled; }

  void increase_indent() {
    assert(_indent < 500 && "Sanity!");
    _indent++;
  }
  void decrease_indent() {
    assert(_indent != 0 && "Underflow!");
    _indent--;
  }

  void insert_indent() {
    for (unsigned i = 0, e = _indent; i != e; ++i)
      _stream << ' ';
  }

  std::string get_log() { return _stream.str(); }
};

template <typename T>
inline ActiveLogger &operator<<(ActiveLogger &out, const T &t) {
  if (out.is_enabled())
    out.stream() << t;
  return out;
}

template <>
inline ActiveLogger &operator<<<IndentActionTy>(ActiveLogger &out,
                                                const IndentActionTy &t) {
  if (out.is_enabled())
    out.insert_indent();
  return out;
}

struct InactiveLogger {
  explicit InactiveLogger(bool enabled) {}

  void increase_indent() {}
  void decrease_indent() {}
  void insert_indent() {}

  std::string &get_log() { return _always_empty; }

private:
  std::string _always_empty;
};

template <typename T>
inline InactiveLogger &operator<<(InactiveLogger &out, const T &t) {
  return out;
}

template <typename LoggerTy> class GenericIndentScope {
  LoggerTy &_logger;

public:
  explicit GenericIndentScope(LoggerTy &logger)
      : _logger(logger) {
    _logger.increase_indent();
  }

  ~GenericIndentScope() { _logger.decrease_indent(); }
};

#define ENABLE_LOGGING

#ifdef ENABLE_LOGGING
typedef ActiveLogger Logger;
#else
typedef InactiveLogger Logger;
#endif

typedef GenericIndentScope<Logger> IndentScope;
}

#endif
