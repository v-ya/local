#ifndef _jpeg_h_
#define _jpeg_h_

#include <refer.h>
#include <mlog.h>
#include "jpeg.type.h"

#define bits_bswap16(_v)  __builtin_bswap16(_v)
#define bits_bswap32(_v)  __builtin_bswap32(_v)
#define bits_bswap64(_v)  __builtin_bswap64(_v)

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define bits_n2le_16(_n) (_n)
	#define bits_n2be_16(_n) bits_bswap16(_n)
	#define bits_n2le_32(_n) (_n)
	#define bits_n2be_32(_n) bits_bswap32(_n)
	#define bits_n2le_64(_n) (_n)
	#define bits_n2be_64(_n) bits_bswap64(_n)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define bits_n2le_16(_n) bits_bswap16(_n)
	#define bits_n2be_16(_n) (_n)
	#define bits_n2le_32(_n) bits_bswap32(_n)
	#define bits_n2be_32(_n) (_n)
	#define bits_n2le_64(_n) bits_bswap64(_n)
	#define bits_n2be_64(_n) (_n)
#endif

#endif
