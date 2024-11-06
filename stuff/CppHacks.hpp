#pragma once

#ifdef _MSC_VER
#define INLINE inline __forceinline
#else
#define INLINE inline __attribute__((always_inline))
#endif
