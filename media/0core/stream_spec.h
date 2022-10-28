#ifndef _media_core_stream_spec_h_
#define _media_core_stream_spec_h_

#include <refer.h>
#include <vattr.h>
#include "stream.h"
#include "stack_id.h"
#include "frame.h"
#include "attr.h"
#include "media.h"

struct media_stream_spec_s;
struct media_stream_s;
struct media_container_s;

typedef refer_t (*media_stream_create_pri_f)(const struct media_stream_spec_s *restrict spec, struct media_container_s *restrict c);
typedef struct media_container_s* (*media_stream_append_pre_f)(struct media_container_s *restrict c, struct media_stream_s *restrict s);
typedef struct media_frame_s* (*media_stream_read_frame_f)(const struct media_stream_s *restrict s, struct media_frame_s *restrict frame, uintptr_t index);
typedef struct media_stream_s* (*media_stream_write_frame_f)(struct media_stream_s *restrict s, const struct media_frame_s *restrict frame);

struct media_stream_spec_param_t {
	const char *stream_type;
	const char *stack_layout;
	const char *frame_id_name;
	media_attr_judge_initial_f initial_judge;
	media_stream_create_pri_f create_pri;
	media_stream_append_pre_f append_pre;
	media_stream_read_frame_f read_frame;
	media_stream_write_frame_f write_frame;
};

struct media_stream_spec_s {
	refer_string_t stream_type;
	refer_string_t stack_layout;
	refer_string_t frame_id_name;
	const struct media_attr_judge_s *judge;
	media_stream_create_pri_f create_pri;
	media_stream_append_pre_f append_pre;
	media_stream_read_frame_f read_frame;
	media_stream_write_frame_f write_frame;
};

vattr_s* media_stream_spec_append(vattr_s *restrict stream_spec, const struct media_s *restrict media, const struct media_stream_spec_param_t *restrict param, uintptr_t nparam);

#define media_stream_symbol(_tf, _name)       media_stream__##_tf##__##_name

#define d_media_stream__initial_judge(_name)  struct media_attr_judge_s* media_stream_symbol(initial_judge, _name)(struct media_attr_judge_s *restrict judge)
#define d_media_stream__create_pri(_name)     refer_t media_stream_symbol(create_pri, _name)(const struct media_stream_spec_s *restrict spec, struct media_container_s *restrict c)
#define d_media_stream__append_pre(_name)     struct media_container_s* media_stream_symbol(append_pre, _name)(struct media_container_s *restrict c, struct media_stream_s *restrict s)
#define d_media_stream__read_frame(_name)     struct media_frame_s* media_stream_symbol(read_frame, _name)(const struct media_stream_s *restrict s, struct media_frame_s *restrict frame, uintptr_t index)
#define d_media_stream__write_frame(_name)    struct media_stream_s* media_stream_symbol(write_frame, _name)(struct media_stream_s *restrict s, const struct media_frame_s *restrict frame)
#define d_media_stream__spec_append(_name)    vattr_s* media_stream_symbol(spec_append, _name)(vattr_s *restrict stream_spec, const struct media_s *restrict media)

#endif
