#ifndef _media_core_container_h_
#define _media_core_container_h_

#include <refer.h>
#include <vattr.h>
#include "container_id.h"
#include "media.h"
#include "io.h"
#include "attr.h"

enum media_container_io_t {
	media_container_io_none,
	media_container_io_input,
	media_container_io_output,
};

enum media_container_step_t {
	media_container_step_none,
	media_container_step_head,
	media_container_step_tail,
};

struct media_container_inner_s {
	const struct media_container_id_s *id;
	const struct media_s *media;
	refer_t pri_data;
	struct media_io_s *io;
	struct media_attr_s *attr;
	enum media_container_io_t iotype;
	enum media_container_step_t step;
};

struct media_container_s {
	struct media_container_inner_s *inner;
	vattr_s *stream;
};

struct media_stream_s;

struct media_container_s* media_container_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id);
struct media_container_s* media_container_done_step(struct media_container_s *restrict container, enum media_container_step_t step);
struct media_container_s* media_container_set_io(struct media_container_s *restrict container, struct media_io_s *restrict io, enum media_container_io_t iotype);

struct media_stream_s* media_container_new_stream(struct media_container_s *restrict container, const char *restrict stream_type, const char *restrict frame_name);
struct media_stream_s* media_container_find_stream(const struct media_container_s *restrict container, const char *restrict stream_type, uintptr_t index);

#endif
