#ifndef _media_bits_bits_h_
#define _media_bits_bits_h_

#include <refer.h>

#define media_bswap16(_v)  __builtin_bswap16(_v)
#define media_bswap32(_v)  __builtin_bswap32(_v)
#define media_bswap64(_v)  __builtin_bswap64(_v)

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define media_n2le_16(_n) (_n)
	#define media_n2be_16(_n) media_bswap16(_n)
	#define media_n2le_32(_n) (_n)
	#define media_n2be_32(_n) media_bswap32(_n)
	#define media_n2le_64(_n) (_n)
	#define media_n2be_64(_n) media_bswap64(_n)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define media_n2le_16(_n) media_bswap16(_n)
	#define media_n2be_16(_n) (_n)
	#define media_n2le_32(_n) media_bswap32(_n)
	#define media_n2be_32(_n) (_n)
	#define media_n2le_64(_n) media_bswap64(_n)
	#define media_n2be_64(_n) (_n)
#endif

#endif
