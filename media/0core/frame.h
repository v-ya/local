#ifndef _media_core_frame_h_
#define _media_core_frame_h_

#include "../media.h"
#include "cell.h"
#include "channel.h"
#include "frame_id.h"

struct media_frame_s {
	const struct media_frame_id_s *id;
	refer_t data_source;
	uintptr_t channel;
	struct media_channel_s *channel_chip[];
};

struct media_frame_s* media_frame_alloc(const struct media_frame_id_s *restrict frame_id, uintptr_t dimension, const uintptr_t dimension_value[]);

#endif
