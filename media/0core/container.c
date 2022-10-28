#include "container.h"
#include "stream.h"

static void media_container_inner_free_func(struct media_container_inner_s *restrict r)
{
	if (r->attr) refer_free(r->attr);
	if (r->io) refer_free(r->io);
	if (r->pri_data) refer_free(r->pri_data);
	if (r->media) refer_free(r->media);
	if (r->id) refer_free(r->id);
}

static struct media_container_inner_s* media_container_inner_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	struct media_container_inner_s *restrict r;
	if ((r = (struct media_container_inner_s *) refer_alloz(sizeof(struct media_container_inner_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_inner_free_func);
		r->id = (const struct media_container_id_s *) refer_save(container_id);
		r->media = (const struct media_s *) refer_save(media);
		if (container_id->func.create_pri && !(r->pri_data = container_id->func.create_pri(container_id)))
			goto label_fail;
		if (!(r->attr = media_attr_alloc(NULL)))
			goto label_fail;
		r->iotype = media_container_io_none;
		// initial r->attr
		if (container_id->judge)
		{
			media_attr_set_judge(r->attr, container_id->judge, r->pri_data);
			media_attr_clear(r->attr);
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static void media_container_free_func(struct media_container_s *restrict r)
{
	if (r->stream) refer_free(r->stream);
	if (r->inner) refer_free(r->inner);
}

struct media_container_s* media_container_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	struct media_container_s *restrict r;
	if ((r = (struct media_container_s *) refer_alloz(sizeof(struct media_container_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_free_func);
		if ((r->inner = media_container_inner_alloc(media, container_id)) &&
			(r->stream = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

struct media_container_s* media_container_done_step(struct media_container_s *restrict container, enum media_container_step_t step)
{
	struct media_container_inner_s *restrict inner;
	const struct media_container_id_s *restrict id;
	enum media_container_step_t step_curr;
	inner = container->inner;
	id = inner->id;
	if (inner->io)
	{
		step_curr = inner->step;
		switch (inner->iotype)
		{
			case media_container_io_input:
				if (step_curr == media_container_step_none && step > media_container_step_none)
				{
					if (id->func.parse_head && !id->func.parse_head(container))
						goto label_fail;
					step_curr = media_container_step_head;
				}
				if (step_curr == media_container_step_head && step > media_container_step_head)
				{
					if (id->func.parse_tail && !id->func.parse_tail(container))
						goto label_fail;
					step_curr = media_container_step_tail;
				}
				inner->step = step_curr;
				return container;
			case media_container_io_output:
				if (step_curr == media_container_step_none && step > media_container_step_none)
				{
					if (id->func.build_head && !id->func.build_head(container))
						goto label_fail;
					step_curr = media_container_step_head;
				}
				if (step_curr == media_container_step_head && step > media_container_step_head)
				{
					if (id->func.build_tail && !id->func.build_tail(container))
						goto label_fail;
					if (!media_io_sync(inner->io))
						goto label_fail;
					step_curr = media_container_step_tail;
				}
				inner->step = step_curr;
				return container;
			default:
				label_fail:
				inner->step = step_curr;
				break;
		}
	}
	return NULL;
}

struct media_container_s* media_container_set_io(struct media_container_s *restrict container, struct media_io_s *restrict io, enum media_container_io_t iotype)
{
	struct media_container_inner_s *restrict inner;
	vattr_clear(container->stream);
	inner = container->inner;
	if (inner->io)
	{
		if (inner->iotype == media_container_io_output)
			media_container_done_step(container, media_container_step_tail);
		refer_free(inner->io);
		inner->io = NULL;
		inner->iotype = media_container_io_none;
		inner->step = media_container_step_none;
	}
	if (io)
	{
		inner->io = (struct media_io_s *) refer_save(io);
		inner->iotype = iotype;
	}
	return container;
}

struct media_stream_s* media_container_new_stream(struct media_container_s *restrict container, const char *restrict stream_type, const char *restrict frame_name)
{
	struct media_container_inner_s *restrict inner;
	const struct media_stream_spec_s *restrict spec;
	struct media_stream_s *restrict stream;
	inner = container->inner;
	if ((spec = media_container_id_get_spec(inner->id, stream_type, frame_name)) &&
		(stream = media_stream_alloc(spec, container)))
	{
		if ((spec->append_pre && !spec->append_pre(container, stream)) ||
			!vattr_insert_tail(container->stream, spec->stream_type, stream))
			container = NULL;
		refer_free(stream);
		return container?stream:NULL;
	}
	return NULL;
}

struct media_stream_s* media_container_find_stream(const struct media_container_s *restrict container, const char *restrict stream_type, uintptr_t index)
{
	const vattr_vlist_t *restrict vl;
	if (stream_type)
	{
		vl = vattr_get_vlist_first(container->stream, stream_type);
		while (vl && index)
		{
			vl = vl->vslot_next;
			index -= 1;
		}
	}
	else
	{
		
		vl = vattr_first(container->stream);
		while (vl && index)
		{
			vl = vl->vattr_next;
			index -= 1;
		}
	}
	return vl?(struct media_stream_s *) vl->value:NULL;
}
