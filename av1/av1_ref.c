#include "av1_ref.h"
#include <memory.h>

av1_ref_t* av1_ref_init(av1_ref_t *restrict ref)
{
	return (av1_ref_t *) memset(ref, 0, sizeof(*ref));
}

void av1_ref_mark_ref_frames(register av1_ref_t *restrict ref, register uint32_t current_frame_id, uint32_t additional_frame_id_length, uint32_t delta_frame_id_length)
{
	register uintptr_t i;
	register uint32_t deadline_frame_id;
	additional_frame_id_length += delta_frame_id_length;
	deadline_frame_id = (current_frame_id - (1u << delta_frame_id_length)) & ((1u << additional_frame_id_length) - 1);
	if (deadline_frame_id < current_frame_id)
	{
		// ~ deadline_frame_id, current_frame_id ~
		for (i = 0; i < NUM_REF_FRAMES; ++i)
			if (ref->frame_id[i] < deadline_frame_id || ref->frame_id[i] > current_frame_id)
				ref->valid[i] = 0;
	}
	else
	{
		// current_frame_id ~ deadline_frame_id
		for (i = 0; i < NUM_REF_FRAMES; ++i)
			if (ref->frame_id[i] < deadline_frame_id && ref->frame_id[i] > current_frame_id)
				ref->valid[i] = 0;
	}
}
