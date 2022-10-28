#include "stream.h"
#include "container.h"

static void media_stream_free_func(struct media_stream_s *restrict r)
{
	if (r->stack) refer_free(r->stack);
	if (r->attr) refer_free(r->attr);
	if (r->pri_data) refer_free(r->pri_data);
	if (r->frame_id) refer_free(r->frame_id);
	if (r->stack_id) refer_free(r->stack_id);
	if (r->inner) refer_free(r->inner);
	if (r->spec) refer_free(r->spec);
}

struct media_stream_s* media_stream_alloc(const struct media_stream_spec_s *restrict spec, struct media_container_s *restrict container)
{
	struct media_stream_s *restrict r;
	const struct media_s *restrict media;
	if ((r = (struct media_stream_s *) refer_alloz(sizeof(struct media_stream_s))))
	{
		refer_set_free(r, (refer_free_f) media_stream_free_func);
		media = container->inner->media;
		r->spec = (const struct media_stream_spec_s *) refer_save(spec);
		r->inner = (struct media_container_inner_s *) refer_save(container->inner);
		if (!(r->stack_id = media_save_stack(media, spec->stack_layout)))
			goto label_fail;
		if (!(r->frame_id = media_save_frame(media, spec->frame_id_name)))
			goto label_fail;
		if (spec->create_pri && !(r->pri_data = spec->create_pri(spec, container)))
			goto label_fail;
		if (!(r->attr = media_attr_alloc(NULL)))
			goto label_fail;
		if (spec->judge)
		{
			media_attr_set_judge(r->attr, spec->judge, r->pri_data);
			media_attr_clear(r->attr);
		}
		if ((r->stack = media_stack_alloc(&r->stack_id->param)))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_frame_s* media_stream_create_frame(struct media_stream_s *restrict stream)
{
	return media_frame_alloc(stream->frame_id, stream->frame_id->dimension, NULL);
}

struct media_frame_s* media_stream_read_frame_by_index(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame, uintptr_t index)
{
	const struct media_stream_spec_s *restrict spec;
	struct media_container_inner_s *restrict inner;
	spec = stream->spec;
	inner = stream->inner;
	if (inner->io && frame->id == stream->frame_id && spec->read_frame)
		return spec->read_frame(stream, frame, index);
	return NULL;
}

struct media_frame_s* media_stream_read_frame(struct media_stream_s *restrict stream, struct media_frame_s *restrict frame)
{
	uintptr_t index;
	if ((frame = media_stream_read_frame_by_index(stream, frame, index = stream->stack->stack_index)))
		media_stack_set_index(stream->stack, index + 1);
	return frame;
}

struct media_stream_s* media_stream_write_frame(struct media_stream_s *restrict stream, const struct media_frame_s *restrict frame)
{
	const struct media_stream_spec_s *restrict spec;
	struct media_container_inner_s *restrict inner;
	spec = stream->spec;
	inner = stream->inner;
	if (inner->io && inner->iotype == media_container_io_output &&
		inner->step == media_container_step_head &&
		frame->id == stream->frame_id && spec->read_frame)
		return spec->write_frame(stream, frame);
	return NULL;
}
