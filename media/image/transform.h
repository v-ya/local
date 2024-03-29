#ifndef _media_image_transform_h_
#define _media_image_transform_h_

#include <refer.h>
#include "../0core/media.h"
#include "../0core/transform.h"
#include "../0core/component.h"
#include "../media.frame.h"

struct media_transform_id_s* media_transform_create_image__bgra32_bgr24(const struct media_s *restrict media);
struct media_transform_id_s* media_transform_create_image__bgr24_bgra32(const struct media_s *restrict media);
struct media_transform_id_s* media_transform_create_image__yuv_8_411_bgra32(const struct media_s *restrict media);
struct media_transform_id_s* media_transform_create_image__yuv_8_111_bgra32(const struct media_s *restrict media);

struct media_transform_id_s* media_transform_create_image__jpeg_yuv_8_411(const struct media_s *restrict media);
struct media_transform_id_s* media_transform_create_image__jpeg_yuv_8_111(const struct media_s *restrict media);

// inner

void* media_transform_inner_image_cinfo(const struct media_channel_s *restrict channel, uintptr_t *restrict width, uintptr_t *restrict height, uintptr_t *restrict linesize, uintptr_t *restrict lineskip);

// inner d2c1
struct media_transform_param__d2c1_t {
	const uint8_t *src;
	uint8_t *dst;
	uintptr_t src_skip;
	uintptr_t dst_skip;
	uintptr_t w;
	uintptr_t yn;
};
struct media_runtime_task_s* media_transform_inner_image_emit__d2c1(struct media_runtime_task_s *restrict task, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, struct media_runtime_s *restrict rt, struct media_runtime_unit_context_s *restrict uc, media_runtime_deal_f deal, refer_t data);

// inner d2c13
struct media_transform_param__d2c13_t {
	const uint8_t *src;
	uint8_t *dst1;
	uint8_t *dst2;
	uint8_t *dst3;
	uintptr_t src_linesize;
	uintptr_t dst1_linesize;
	uintptr_t dst2_linesize;
	uintptr_t dst3_linesize;
	uintptr_t w;
	uintptr_t n;
	uintptr_t x;
	uintptr_t y;
};
struct media_runtime_task_s* media_transform_inner_image_emit__d2c13(struct media_runtime_task_s *restrict task, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, struct media_runtime_s *restrict rt, struct media_runtime_unit_context_s *restrict uc, media_runtime_deal_f deal, refer_t data);

// inner d2c31
struct media_transform_param__d2c31_t {
	const uint8_t *src1;
	const uint8_t *src2;
	const uint8_t *src3;
	uint8_t *dst;
	uintptr_t src1_linesize;
	uintptr_t src2_linesize;
	uintptr_t src3_linesize;
	uintptr_t dst_linesize;
	uintptr_t w;
	uintptr_t n;
	uintptr_t x;
	uintptr_t y;
};
struct media_runtime_task_s* media_transform_inner_image_emit__d2c31(struct media_runtime_task_s *restrict task, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, struct media_runtime_s *restrict rt, struct media_runtime_unit_context_s *restrict uc, media_runtime_deal_f deal, refer_t data);

#endif
