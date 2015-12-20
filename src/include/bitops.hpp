#ifndef RAY_BITOPS_HPP
#define RAY_BITOPS_HPP

/// bitops.hpp: bit manipulation helpers.

#include <cstdint>

namespace ray {

struct Bitops {
#ifdef __BIG_ENDIAN__
  static const bool IS_LITTLE_ENDIAN = false;
  static const bool IS_BIG_ENDIAN = true;
#endif

#ifdef __LITTLE_ENDIAN__
  static const bool IS_LITTLE_ENDIAN = true;
  static const bool IS_BIG_ENDIAN = false;
#endif

  static void encode_le(uint32_t val, uint8_t *out) {
    static_assert(IS_LITTLE_ENDIAN, "big endian unimplemented!");
    *reinterpret_cast<uint32_t *>(out) = val;
  }
};
}

#endif
