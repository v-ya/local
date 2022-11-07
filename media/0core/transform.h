#ifndef _media_core_transform_h_
#define _media_core_transform_h_

#include <refer.h>
#include "transform_id.h"
#include "runtime.h"
#include "frame.h"
#include "media.h"
#include "attr.h"

struct media_transform_s {
	const struct media_transform_id_s *id;
	const struct media_frame_id_s *src_frame_id;
	const struct media_frame_id_s *dst_frame_id;
	const struct media_s *media;
	struct media_runtime_s *runtime;
	struct media_attr_s *attr;
	refer_t pri_data;
	refer_t codec;
};

struct media_transform_s* media_transform_alloc(const struct media_s *restrict media, const struct media_transform_id_s *restrict transform_id, struct media_runtime_s *restrict runtime);

struct media_transform_s* media_transform_open(struct media_transform_s *restrict transform);
void media_transform_close(struct media_transform_s *restrict transform);
const struct media_frame_id_s* media_transform_dst_frame(struct media_transform_s *restrict transform);
struct media_runtime_task_s* media_transform_post_task(struct media_transform_s *restrict transform, const struct media_frame_s *restrict src_frame, struct media_frame_s *restrict dst_frame, const struct media_runtime_task_done_t *restrict done);
struct media_frame_s* media_transform_alloc_conver(struct media_transform_s *restrict transform, const struct media_frame_s *restrict src_frame, uintptr_t *restrict timeout_usec);

#endif
