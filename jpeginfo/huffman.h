#ifndef _huffman_h_
#define _huffman_h_

#include <refer.h>
#include <exbuffer.h>

/*
	00    => v0
	010   => v1
	011   => v2
	1000  => v3
	1001  => v4
	1010  => v5
	1011  => v6
	1100  => v7
	select[] = {
		//      j0, j1, v0, v1
		[0] => { 1,  3,  -,  -}, // -
		[1] => {~1,  2, v0,  -}, // 0-
		[2] => {~1, ~1, v1, v2}, // 01-
		[3] => { 4,  7,  -,  -}, // 1-
		[4] => { 5,  6,  -,  -}, // 10-
		[5] => {~1, ~1, v3, v4}, // 100-
		[6] => {~1, ~1, v5, v6}, // 101-
		[7] => { 8, ~0,  -,  -}, // 11-
		[8] => {~1, ~0, v7,  -}, // 110-
	}
*/

typedef struct huffman_jumper_t {
	uintptr_t jumper_index;
	uintptr_t bits_count;
	uintptr_t bit_0_jumpto;
	uintptr_t bit_1_jumpto;
	uintptr_t bit_0_value;
	uintptr_t bit_1_value;
} huffman_jumper_t;

typedef struct huffman_decode_s {
	exbuffer_t jumper_buffer;
	huffman_jumper_t *jumper;
	uintptr_t jumper_number;
	uintptr_t bits_count;
} huffman_decode_s;

huffman_decode_s* huffman_decode_alloc(void);
huffman_decode_s* huffman_decode_add_bits(huffman_decode_s *restrict r);
huffman_decode_s* huffman_decode_add_value(huffman_decode_s *restrict r, uintptr_t value);

#endif
