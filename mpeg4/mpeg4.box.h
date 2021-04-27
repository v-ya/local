#ifndef _mpeg4_box_h_
#define _mpeg4_box_h_

#include <stdint.h>

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define mpeg4_n16(_n)  __builtin_bswap16(_n)
#define mpeg4_n32(_n)  __builtin_bswap32(_n)
#define mpeg4_n64(_n)  __builtin_bswap64(_n)
#else
#define mpeg4_n16(_n)  (_n)
#define mpeg4_n32(_n)  (_n)
#define mpeg4_n64(_n)  (_n)
#endif

// 1 => enable extend size, 0 => extend to end of file
typedef uint32_t mpeg4_box_size_t;

typedef uint64_t mpeg4_box_extend_size_t;

// 'uuid' => enable extend type
typedef union mpeg4_box_type_t {
	uint8_t c[4];
	uint32_t v;
} mpeg4_box_type_t;

typedef union mpeg4_box_extend_type_t {
	uint8_t c[16];
	uint32_t l[4];
	uint64_t q[2];
} mpeg4_box_extend_type_t;

typedef struct mpeg4_box_t {
	mpeg4_box_size_t size;
	mpeg4_box_type_t type;
	// mpeg4_box_extend_size_t extend_size;
	// mpeg4_box_extend_type_t extend_type;
} mpeg4_box_t;

// mpeg4_full_box_t => {
// 	mpeg4_box_t;
// 	mpeg4_full_box_suffix_t;
// }
typedef union mpeg4_full_box_suffix_t {
	struct {
		uint8_t version;
		uint8_t flags[3];
	};
	uint32_t v;
} mpeg4_full_box_suffix_t;

#define mpeg4_fix_point(_n, _a, _b)  (_n * (1.0 / (1ul << _b)))

uint64_t mpeg4_box_border_parse(const uint8_t *restrict data, uint64_t size, mpeg4_box_extend_size_t *restrict rsize, mpeg4_box_type_t *restrict rtype);
uint64_t mpeg4_box_border_build(uint8_t *restrict data, uint64_t box_size, mpeg4_box_type_t type);

#endif
