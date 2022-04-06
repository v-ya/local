#ifndef _cmath_largeN_h_
#define _cmath_largeN_h_

#include <refer.h>

typedef struct largeN_s largeN_s;
typedef struct largeN_s largeN_store_s;
typedef struct largeN_cache_s largeN_cache_s;

largeN_cache_s* largeN_cache_alloc(void);

largeN_s* largeN_alloc(uintptr_t bits);
largeN_s* largeN_alloz(uintptr_t bits);

largeN_s* largeN_from_le(largeN_s *restrict dst, const largeN_store_s *restrict src);
largeN_s* largeN_from_be(largeN_s *restrict dst, const largeN_store_s *restrict src);
largeN_store_s* largeN_to_le(largeN_store_s *restrict dst, const largeN_s *restrict src);
largeN_store_s* largeN_to_be(largeN_store_s *restrict dst, const largeN_s *restrict src);
uint8_t* largeN_store_data(const largeN_store_s *restrict src, uintptr_t *restrict rsize);

uintptr_t largeN_bits_allow(const largeN_s *restrict src);
uintptr_t largeN_bits_effective(const largeN_s *restrict src);
largeN_s* largeN_zero(largeN_s *restrict dst);
largeN_s* largeN_set(largeN_s *restrict dst, const largeN_s *restrict src);

#endif
