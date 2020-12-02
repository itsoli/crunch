// File: crn_atomics.h

#pragma once

#include <atomic>
#include <cstdint>

#include "crn_core.h"

namespace crnlib {
typedef std::atomic_int32_t atomic32_t;
typedef std::atomic_int64_t atomic64_t;

// Returns the resulting incremented value.
inline std::int32_t atomic_increment32(atomic32_t volatile* pDest) {
  CRNLIB_ASSERT((reinterpret_cast<ptr_bits_t>(pDest) & 3) == 0);
  return pDest->fetch_add(1) + 1;
}

// Returns the resulting decremented value.
inline std::int32_t atomic_decrement32(atomic32_t volatile* pDest) {
  CRNLIB_ASSERT((reinterpret_cast<ptr_bits_t>(pDest) & 3) == 0);
  return pDest->fetch_sub(1) - 1;
}

// Returns the original value.
inline std::int32_t atomic_exchange32(atomic32_t volatile* pDest, std::int32_t val) {
  CRNLIB_ASSERT((reinterpret_cast<ptr_bits_t>(pDest) & 3) == 0);
  return std::atomic_exchange(pDest, val);
}

// Returns the resulting value.
inline std::int32_t atomic_add32(atomic32_t volatile* pDest, std::int32_t val) {
  CRNLIB_ASSERT((reinterpret_cast<ptr_bits_t>(pDest) & 3) == 0);
  return pDest->fetch_add(val) + val;
}

// Returns the original value.
inline std::int32_t atomic_exchange_add32(atomic32_t volatile* pDest, std::int32_t val) {
  CRNLIB_ASSERT((reinterpret_cast<ptr_bits_t>(pDest) & 3) == 0);
  return pDest->fetch_add(val);
}

}  // namespace crnlib
