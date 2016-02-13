#ifndef RAY_SUPPORT_INLINE_HPP
#define RAY_SUPPORT_INLINE_HPP

namespace ray {

namespace internal {
inline void print_recursive(std::stringstream &ss) {}
template <typename K, typename V, typename... Args>
void print_recursive(std::stringstream &ss, K key, V val, Args... args) {
  ss << " " << key << ": " << val;
  print_recursive(ss, args...);
}
} // namespace internal

template <typename... Args>
std::string generate_description_string(const char *kind, Args... args) {
  static_assert(sizeof...(args) % 2 == 0, "Must be K/V pairs!");
  std::stringstream ss;
  ss << "(" << kind;
  internal::print_recursive(ss, args...);
  ss << ")";
  return ss.str();
}

} // namespace ray

#endif
