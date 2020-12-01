// File: crn_types.h
// See Copyright Notice and license at the end of inc/crnlib.h
#pragma once

#include <cstdint>

#ifdef __APPLE__
#include <sys/types.h>
#endif

namespace crnlib
{
   typedef std::uint8_t uint8;
   typedef std::int8_t int8;
   typedef std::uint16_t uint16;
   typedef std::int16_t int16;
   typedef std::uint32_t uint32;
   typedef std::int32_t int32;
   typedef std::uint64_t uint64;
   typedef std::int64_t int64;

   const uint8  cUINT8_MIN  = 0;
   const uint8  cUINT8_MAX  = 0xFFU;
   const uint16 cUINT16_MIN = 0;
   const uint16 cUINT16_MAX = 0xFFFFU;
   const uint32 cUINT32_MIN = 0;
   const uint32 cUINT32_MAX = 0xFFFFFFFFU;
   const uint64 cUINT64_MIN = 0;
   const uint64 cUINT64_MAX = 0xFFFFFFFFFFFFFFFFULL; //0xFFFFFFFFFFFFFFFFui64;

   const int8  cINT8_MIN  = -128;
   const int8  cINT8_MAX  = 127;
   const int16 cINT16_MIN = -32768;
   const int16 cINT16_MAX = 32767;
   const int32 cINT32_MIN = (-2147483647 - 1);
   const int32 cINT32_MAX = 2147483647;
   const int64 cINT64_MIN = (int64)0x8000000000000000ULL; //(-9223372036854775807i64 - 1);
   const int64 cINT64_MAX = (int64)0x7FFFFFFFFFFFFFFFULL; // 9223372036854775807i64;

#if CRNLIB_64BIT_POINTERS
   typedef uint64 uint_ptr;
   typedef uint64 uint32_ptr;
   typedef int64 signed_size_t;
   typedef uint64 ptr_bits_t;
#else
   typedef unsigned int uint_ptr;
   typedef unsigned int uint32_ptr;
   typedef signed int signed_size_t;
   typedef uint32 ptr_bits_t;
#endif

   enum eVarArg { cVarArg };
   enum eClear { cClear };
   enum eNoClamp { cNoClamp };
   enum { cInvalidIndex = -1 };

   const uint cIntBits = 32;

   struct empty_type { };

} // namespace crnlib
