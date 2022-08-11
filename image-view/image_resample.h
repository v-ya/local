#ifndef _image_view_image_resample_h_
#define _image_view_image_resample_h_

#include <refer.h>
#include <mtask.h>

typedef struct image_resample_s {
	const float *src;
	uint32_t *dst;
	mtask_inst_s *mtask;
	uint64_t d_size;
	uint32_t s_width;
	uint32_t s_height;
	uint32_t d_width;
	uint32_t d_height;
	float matrix[9];
	uint32_t bgcolor;
	uint32_t mc_have_min_pixels;
} image_resample_s;

image_resample_s* image_resample_alloc(uint32_t n_multicalc, uint32_t bgcolor);
image_resample_s* image_resample_set_src(image_resample_s *restrict r, const uint32_t *restrict data, uint32_t width, uint32_t height);
image_resample_s* image_resample_set_dst(image_resample_s *restrict r, uint32_t *restrict data, uint32_t width, uint32_t height);
void image_resample_m_mul(image_resample_s *restrict r, float k[9]);
void image_resample_m_tran(image_resample_s *restrict r, float x, float y);
void image_resample_m_rota(image_resample_s *restrict r, float x, float y, float rad);
void image_resample_m_scale(image_resample_s *restrict r, float x, float y, float k);
void image_resample_m_reset(image_resample_s *restrict r);
image_resample_s* image_resample_get_dst(image_resample_s *restrict r);

#endif
