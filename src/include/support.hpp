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

namespace internal {

static inline void print_recursive(std::stringstream &ss) {}

template <typename K, typename V, typename... Args>
static inline void print_recursive(std::stringstream &ss, K key, V val,
                                   Args... args) {
  ss << " " << key << ": " << val;
  print_recursive(ss, args...);
}

}

template <typename... Args>
static inline std::string generate_description_string(const char *kind,
                                                      Args... args) {
  static_assert(sizeof...(args) % 2 == 0, "Must be K/V pairs!");

  std::stringstream ss;
  ss << "(" << kind;

  internal::print_recursive(ss, args...);

  ss << ")";

  return ss.str();
}

enum class IndentActionTy { indent = 42 };
constexpr IndentActionTy indent = IndentActionTy::indent;

class ActiveLogger {
  std::stringstream _stream;
  bool _enabled;
  unsigned _indent = 0;

public:
  static constexpr bool kIsEnabled = true;

  explicit ActiveLogger(bool enabled) : _enabled(enabled) {}
  std::stringstream &stream() {
    assert(_enabled);
    return _stream;
  }
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

class LogTag {
  const char *_tag;

public:
  LogTag(const char *s) : _tag(s) {}
  const char *tag() const { return _tag; };
};

template <typename T>
inline ActiveLogger &operator<<(ActiveLogger &out, const T &t) {
  if (out.is_enabled())
    out.stream() << t;
  return out;
}

template <>
inline ActiveLogger &operator<<(ActiveLogger &out, const IndentActionTy &t) {
  if (out.is_enabled())
    out.insert_indent();
  return out;
}

template <>
inline ActiveLogger &operator<<(ActiveLogger &out, const LogTag &t) {
  if (out.is_enabled())
    out.stream() << "[" << t.tag() << "] ";
  return out;
}

struct InactiveLogger {
  static constexpr bool kIsEnabled = true;

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
#define LOGGING_ONLY(x) x
#define NO_LOGGING(x)
#else
typedef InactiveLogger Logger;
#define LOGGING_ONLY(x)
#define NO_LOGGING(x) x
#endif

typedef GenericIndentScope<Logger> IndentScope;
}

#endif
