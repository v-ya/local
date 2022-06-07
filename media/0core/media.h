#ifndef _media_core_media_h_
#define _media_core_media_h_

#include <refer.h>
#include "frame_id.h"
#include "stream_id.h"
#include "container_id.h"
#include <hashmap.h>

struct media_s {
	hashmap_t frame;
	hashmap_t stream;
	hashmap_t container;
};

struct media_s* media_alloc_empty(void);
struct media_s* media_initial_add_frame(struct media_s *restrict media, const struct media_frame_id_s *restrict frame_id);
struct media_s* media_initial_add_stream(struct media_s *restrict media, const struct media_stream_id_s *restrict stream_id);
struct media_s* media_initial_add_container(struct media_s *restrict media, const struct media_container_id_s *restrict container_id);

#endif
