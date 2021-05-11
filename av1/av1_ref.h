#ifndef _av1_ref_h_
#define _av1_ref_h_

#include <stdint.h>
#include "obu/define.h"

typedef struct av1_ref_t {
	uint32_t frame_id[NUM_REF_FRAMES];
	av1_frame_type_t frame_type[NUM_REF_FRAMES];
	uint32_t upscaled_width[NUM_REF_FRAMES];
	uint32_t render_width[NUM_REF_FRAMES];
	uint32_t render_height[NUM_REF_FRAMES];
	uint32_t valid[NUM_REF_FRAMES];
	uint32_t order_hint[NUM_REF_FRAMES];
	uint32_t frame_sign_bias[NUM_REF_FRAMES];
} av1_ref_t;

av1_ref_t* av1_ref_init(av1_ref_t *restrict ref);
void av1_ref_mark_ref_frames(av1_ref_t *restrict ref, uint32_t current_frame_id, uint32_t additional_frame_id_length, uint32_t delta_frame_id_length);

#endif
