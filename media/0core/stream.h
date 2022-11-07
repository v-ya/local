#ifndef _media_core_stream_h_
#define _media_core_stream_h_

#include <refer.h>
#include <vattr.h>
#include "stream_spec.h"
#include "stack_id.h"
#include "frame.h"

struct media_container_s;

struct media_stream_s {
	const struct media_stream_spec_s *spec;
	struct media_container_inner_s *inner;
	const struct media_stack_id_s *stack_id;
	const struct media_frame_id_s *frame_id;
	refer_t pri_data;
	struct media_attr_s *attr;
	struct media_stack_s *stack;
};

struct media_stream_s* media_stream_alloc(const struct media_stream_spec_s *restrict spec, struct media_container_s *restrict container);

struct media_frame_s* media_stream_create_frame(struct media_stream_s *restrict stream);
struct media_frame_s* media_stream_read_frame_by_index(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame, uintptr_t index);
struct media_frame_s* media_stream_read_frame(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame);
struct media_stream_s* media_stream_write_frame(struct media_stream_s *restrict stream, const struct media_frame_s *restrict frame);

#endif
