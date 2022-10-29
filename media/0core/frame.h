#ifndef _media_core_frame_h_
#define _media_core_frame_h_

#include <refer.h>
#include "cell.h"
#include "channel.h"
#include "frame_id.h"
#include "attr.h"

struct media_frame_s {
	const struct media_frame_id_s *id;
	struct media_attr_s *attr;
	uintptr_t *restrict dv;
	uintptr_t channel;
	struct media_channel_s *channel_chip[];
};

struct media_frame_s* media_frame_alloc(const struct media_frame_id_s *restrict frame_id, uintptr_t dimension, const uintptr_t *restrict dimension_value);
struct media_frame_s* media_frame_set_dimension(struct media_frame_s *restrict frame, uintptr_t dimension, const uintptr_t *restrict dimension_value);
struct media_frame_s* media_frame_test_dimension(struct media_frame_s *restrict frame, uintptr_t dimension, const uintptr_t *restrict dimension_value);
struct media_frame_s* media_frame_touch_data(struct media_frame_s *restrict frame);
void media_frame_clear_data(struct media_frame_s *restrict frame);

#endif
