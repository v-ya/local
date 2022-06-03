#ifndef _media_core_media_h_
#define _media_core_media_h_

#include <refer.h>
#include "frame_id.h"
#include <hashmap.h>

struct media_s {
	hashmap_t frame;
};

struct media_s* media_alloc_empty(void);
struct media_s* media_initial_add_frame(struct media_s *restrict media, const struct media_frame_id_s *restrict frame_id);

#endif
