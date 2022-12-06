#ifndef _media_bits_huffman_h_
#define _media_bits_huffman_h_

#include "bits.h"
#include <exbuffer.h>

typedef uint32_t media_huffman_index_t;

#define media_huffman_decode_empty  (~(media_huffman_index_t) 0)
#define media_huffman_decode_tail   (~(media_huffman_index_t) 1)

struct media_huffman_jumper_t {
	media_huffman_index_t last_jump4;
	media_huffman_index_t bits_count;
	media_huffman_index_t bit_jump2[2];
	const void *bit_value[2];
};

struct media_huffman_decode_t {
	media_huffman_index_t root_jump2;
	media_huffman_index_t edge_jump2;
	media_huffman_index_t bits_count;
	media_huffman_index_t jumper_number;
};

struct media_huffman_decode_s {
	exbuffer_t jumper_buffer;
	struct media_huffman_jumper_t *jumper;
	media_huffman_index_t jumper_number;
	media_huffman_index_t decode_number;
	struct media_huffman_decode_t decode[];
};

struct media_huffman_decode_s* media_huffman_decode_alloc(media_huffman_index_t decode_number);
struct media_huffman_decode_s* media_huffman_decode_add_bits(struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index);
struct media_huffman_decode_s* media_huffman_decode_add_value(struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index, const void *value);
const void* media_huffman_decode_bits(const struct media_huffman_decode_s *restrict hmdc, media_huffman_index_t index, struct media_bits_reader_t *restrict reader);

#endif
