#ifndef _media_core_transform_id_h_
#define _media_core_transform_id_h_

#include <refer.h>
#include "runtime.h"
#include "frame.h"
#include "attr.h"

struct media_s;
struct media_transform_id_s;
struct media_transform_s;

typedef refer_t (*media_transform_create_pri_f)(const struct media_transform_id_s *restrict id);
typedef refer_t (*media_transform_open_codec_f)(struct media_transform_s *restrict tf);
typedef const struct media_frame_id_s* (*media_transform_dst_frame_f)(struct media_transform_s *restrict tf);
typedef struct media_runtime_task_s* (*media_transform_post_task_f)(struct media_transform_s *restrict tf, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, const struct media_runtime_task_done_t *restrict done);

struct media_transform_id_func_t {
	media_attr_judge_initial_f initial_judge;
	media_transform_create_pri_f create_pri;
	media_transform_open_codec_f open_codec;
	media_transform_dst_frame_f dst_frame;
	media_transform_post_task_f post_task;
};

struct media_transform_id_s {
	refer_string_t src_frame_compat;
	refer_string_t dst_frame_compat;
	const struct media_attr_judge_s *judge;
	struct media_transform_id_func_t func;
};

void media_transform_id_free_func(struct media_transform_id_s *restrict r);
struct media_transform_id_s* media_transform_id_alloc(uintptr_t size, const struct media_s *restrict media, const char *restrict sfid, const char *restrict dfid, const struct media_transform_id_func_t *restrict func);

#define media_transform_symbol(_tf, _name)            media_transform__##_tf##__##_name

#define d_media_transform__initial_judge(_name)       struct media_attr_judge_s* media_transform_symbol(initial_judge, _name)(struct media_attr_judge_s *restrict judge)
#define d_media_transform__create_pri(_name)          refer_t media_transform_symbol(create_pri, _name)(const struct media_transform_id_s *restrict id)
#define d_media_transform__open_codec(_name)          refer_t media_transform_symbol(open_codec, _name)(struct media_transform_s *restrict tf)
#define d_media_transform__dst_frame(_name)           const struct media_frame_id_s* media_transform_symbol(dst_frame, _name)(struct media_transform_s *restrict tf)
#define d_media_transform__post_task(_name)           struct media_runtime_task_s* media_transform_symbol(post_task, _name)(struct media_transform_s *restrict tf, const struct media_frame_s *restrict sf, struct media_frame_s *restrict df, const struct media_runtime_task_done_t *restrict done)

#define d_media_transform_func_initial(_func, _name)  \
	_func.initial_judge = media_transform_symbol(initial_judge, _name);\
	_func.create_pri = media_transform_symbol(create_pri, _name);\
	_func.open_codec = media_transform_symbol(open_codec, _name);\
	_func.dst_frame = media_transform_symbol(dst_frame, _name);\
	_func.post_task = media_transform_symbol(post_task, _name)

#endif
