#include "transform.h"

static void media_transform_free_func(struct media_transform_s *restrict r)
{
	if (r->codec) refer_free(r->codec);
	if (r->pri_data) refer_free(r->pri_data);
	if (r->attr) refer_free(r->attr);
	if (r->runtime) refer_free(r->runtime);
	if (r->media) refer_free(r->media);
	if (r->dst_frame_id) refer_free(r->dst_frame_id);
	if (r->src_frame_id) refer_free(r->src_frame_id);
	if (r->id) refer_free(r->id);
}

struct media_transform_s* media_transform_alloc(const struct media_s *restrict media, const struct media_transform_id_s *restrict transform_id, struct media_runtime_s *restrict runtime)
{
	struct media_transform_s *restrict r;
	if ((r = (struct media_transform_s *) refer_alloz(sizeof(struct media_transform_s))))
	{
		refer_set_free(r, (refer_free_f) media_transform_free_func);
		r->id = (const struct media_transform_id_s *) refer_save(transform_id);
		if (!(r->src_frame_id = media_save_frame(media, transform_id->src_frame_id)))
			goto label_fail;
		if (transform_id->dst_frame_id && !(r->dst_frame_id = media_save_frame(media, transform_id->dst_frame_id)))
			goto label_fail;
		r->media = (const struct media_s *) refer_save(media);
		r->runtime = (struct media_runtime_s *) refer_save(runtime);
		if (!r->src_frame_id || !r->dst_frame_id || !r->runtime)
			goto label_fail;
		if (!(r->attr = media_attr_alloc(NULL)))
			goto label_fail;
		if (transform_id->judge)
		{
			media_attr_set_judge(r->attr, transform_id->judge, r->pri_data);
			media_attr_clear(r->attr);
		}
		if (transform_id->func.create_pri && !(r->pri_data = transform_id->func.create_pri(transform_id)))
			goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_transform_s* media_transform_open(struct media_transform_s *restrict transform)
{
	media_transform_open_codec_f open_codec;
	if (!transform->codec && (open_codec = transform->id->func.open_codec) &&
		(transform->codec = open_codec(transform)))
		return transform;
	return NULL;
}

void media_transform_close(struct media_transform_s *restrict transform)
{
	if (transform->codec)
	{
		refer_free(transform->codec);
		transform->codec = NULL;
	}
}

const struct media_frame_id_s* media_transform_dst_frame(struct media_transform_s *restrict transform)
{
	media_transform_dst_frame_f dst_frame;
	if (transform->codec && (dst_frame = transform->id->func.dst_frame))
		return dst_frame(transform);
	return transform->dst_frame_id;
}

struct media_runtime_task_s* media_transform_post_task(struct media_transform_s *restrict transform, const struct media_frame_s *restrict src_frame, struct media_frame_s *restrict dst_frame, const struct media_runtime_task_done_t *restrict done)
{
	media_transform_post_task_f post_task;
	if (src_frame && dst_frame && src_frame->id == transform->src_frame_id &&
		(!transform->dst_frame_id || dst_frame->id == transform->dst_frame_id) &&
		transform->codec && (post_task = transform->id->func.post_task))
		return post_task(transform, src_frame, dst_frame, done);
	return NULL;
}

struct media_frame_s* media_transform_alloc_conver(struct media_transform_s *restrict transform, const struct media_frame_s *restrict src_frame, uintptr_t *restrict timeout_usec)
{
	const struct media_frame_id_s *restrict did;
	struct media_frame_s *restrict dst_frame;
	struct media_runtime_task_s *restrict task;
	if ((did = media_transform_dst_frame(transform)) &&
		(dst_frame = media_frame_alloc(did, did->dimension, NULL)))
	{
		if ((task = media_transform_post_task(transform, src_frame, dst_frame, NULL)))
		{
			if (!timeout_usec)
			{
				media_runtime_task_wait(task);
				label_check_task:
				if (media_runtime_task_is_okay(task))
				{
					refer_free(task);
					return dst_frame;
				}
			}
			else if (media_runtime_task_wait_time(task, *timeout_usec))
				goto label_check_task;
			else media_runtime_task_cancel(task);
			refer_free(task);
		}
		refer_free(dst_frame);
	}
	return NULL;
}
