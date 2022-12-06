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

typedef uintptr_t media_bits_t;

struct media_bits_reader_t;

typedef const uint8_t* (*media_bits_reader_next_bytes_f)(struct media_bits_reader_t *restrict reader, uintptr_t *restrict nbyte);
typedef struct media_bits_reader_t* (*media_bits_reader_try_cache_f)(struct media_bits_reader_t *restrict reader);
typedef struct media_bits_reader_t* (*media_bits_reader_read_bits_f)(struct media_bits_reader_t *restrict reader, uint32_t nbits, media_bits_t *restrict vbits);
typedef struct media_bits_reader_t* (*media_bits_reader_push_back_f)(struct media_bits_reader_t *restrict reader, uint32_t nbits, media_bits_t vbits);

struct media_bits_reader_func_t {
	media_bits_reader_next_bytes_f next_bytes;
	media_bits_reader_try_cache_f try_cache;
	media_bits_reader_read_bits_f read_bits;
	media_bits_reader_push_back_f push_back;
};

struct media_bits_reader_t {
	struct media_bits_reader_func_t func;
	const uint8_t *restrict u8;
	uintptr_t size;
	uintptr_t upos;
	media_bits_t cache_value;
	uint32_t cache_bits;
};

#define media_bits_reader_bits_read_bits(_br, _nbits, _vbits)  (_br)->func.read_bits(_br, _nbits, _vbits)
#define media_bits_reader_bits_push_back(_br, _nbits, _vbits)  (_br)->func.push_back(_br, _nbits, _vbits)

struct media_bits_reader_t* media_bits_reader_initial_be(struct media_bits_reader_t *restrict reader, const uint8_t *restrict d, uintptr_t n, media_bits_reader_next_bytes_f next_bytes);
uintptr_t media_bits_reader_bits_pos(struct media_bits_reader_t *restrict reader);
uintptr_t media_bits_reader_bits_res(struct media_bits_reader_t *restrict reader);
struct media_bits_reader_t* media_bits_reader_bits_read_cache(struct media_bits_reader_t *restrict reader, uint32_t *restrict nbits, media_bits_t *restrict vbits);

#endif
