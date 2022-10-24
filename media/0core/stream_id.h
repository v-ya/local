#ifndef _media_core_stream_id_h_
#define _media_core_stream_id_h_

#include <refer.h>
#include "attr.h"

struct media_stream_id_s;
struct media_stream_s;

struct media_stream_id_func_t {
	media_attr_judge_initial_f initial_judge;
};

struct media_stream_id_s {
	refer_string_t name;
	refer_string_t stream_type;
	refer_string_t frame_id_name;
	refer_string_t stack_layout;
	const struct media_attr_judge_s *judge;
	struct media_stream_id_func_t func;
};

void media_stream_id_free_func(struct media_stream_id_s *restrict r);
struct media_stream_id_s* media_stream_id_alloc(uintptr_t size, const char *restrict name, refer_string_t stream_type, refer_string_t frame_id_name, refer_string_t stack_layout, const struct media_stream_id_func_t *restrict func);

#define media_stream_symbol(_tf, _type, _name)            media_stream__##_tf##__##_type##__##_name

#define d_media_stream__initial_judge(_type, _name)       struct media_attr_judge_s* media_stream_symbol(initial_judge, _type, _name)(struct media_attr_judge_s *restrict judge)

#define d_media_stream_func_initial(_func, _type, _name)  \
	_func.initial_judge = media_stream_symbol(initial_judge, _type, _name)

#endif
