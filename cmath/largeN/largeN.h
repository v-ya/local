#ifndef _cmath_largeN_inner_h_
#define _cmath_largeN_inner_h_

#include "../largeN.h"
#include <exbuffer.h>

typedef uintptr_t unit_t;
#define unit_bits __INTPTR_WIDTH__

#if (__INTPTR_WIDTH__ == 64)
	typedef __uint128_t full_t;
	#define bits2unit_shift 6
	#define unit_bswap __builtin_bswap64
#elif (__INTPTR_WIDTH__ == 32)
	typedef uint64_t full_t;
	#define bits2unit_shift 5
	#define unit_bswap __builtin_bswap32
#endif

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define order_n2le(_n) (_n)
	#define order_n2be(_n) unit_bswap(_n)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define order_n2le(_n) unit_bswap(_n)
	#define order_n2be(_n) (_n)
#endif

#define bits2unit(_bits)  (((_bits) + (__INTPTR_WIDTH__ - 1)) >> bits2unit_shift)

struct largeN_s {
	unit_t n;
	unit_t le[];
};

struct largeN_cache_s {
	exbuffer_t c;
};

#endif
