#ifndef _media_bits_fdct_h_
#define _media_bits_fdct_h_

#include <refer.h>

// 2d

struct media_fdct_2d_i32_s {
	uint32_t rank;
	uint32_t rsh;
	int32_t coefficient[];
};

struct media_fdct_2d_i32_s* media_fdct_2d_i32_alloc(uint32_t rank, uint32_t rsh);
void media_fdct_2d_i32_fdct(const struct media_fdct_2d_i32_s *restrict fdct, int32_t *restrict dst, const int32_t *restrict src);
void media_fdct_2d_i32_idct(const struct media_fdct_2d_i32_s *restrict fdct, int32_t *restrict dst, const int32_t *restrict src);

#endif
