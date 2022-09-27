#include "container.h"

static void media_container_free_func(struct media_container_s *restrict r)
{
	if (r->stream) refer_free(r->stream);
	if (r->attr) refer_free(r->attr);
	if (r->io) refer_free(r->io);
	if (r->pri_data) refer_free(r->pri_data);
	if (r->media) refer_free(r->media);
	if (r->id) refer_free(r->id);
}

struct media_container_s* media_container_alloc(const struct media_s *restrict media, const struct media_container_id_s *restrict container_id)
{
	struct media_container_s *restrict r;
	if ((r = (struct media_container_s *) refer_alloz(sizeof(struct media_container_s))))
	{
		refer_set_free(r, (refer_free_f) media_container_free_func);
		r->id = (const struct media_container_id_s *) refer_save(container_id);
		r->media = (const struct media_s *) refer_save(media);
		if (container_id->func.create_pri && !(r->pri_data = container_id->func.create_pri()))
			goto label_fail;
		if (!(r->attr = media_attr_alloc(NULL)))
			goto label_fail;
		if (container_id->judge)
			media_attr_set_judge(r->attr, container_id->judge, r->pri_data);
		if (!(r->stream = vattr_alloc()))
			goto label_fail;
		r->iotype = media_container_io_none;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

struct media_container_s* media_container_set_io(struct media_container_s *restrict container, struct media_io_s *restrict io, enum media_container_io_t iotype)
{
	const struct media_container_id_s *restrict id;
	id = container->id;
	vattr_clear(container->stream);
	if (container->io)
	{
		if (container->iotype == media_container_io_output && id->func.build_tail)
			id->func.build_tail(container);
		refer_free(container->io);
		container->io = NULL;
		container->iotype = media_container_io_none;
	}
	if (io)
	{
		container->io = (struct media_io_s *) refer_save(io);
		container->iotype = iotype;
		switch (iotype)
		{
			case media_container_io_input:
				if (id->func.parse_head && !id->func.parse_head(container))
					goto label_fail;
				break;
			case media_container_io_output:
				if (id->func.build_head && !id->func.build_head(container))
					goto label_fail;
				break;
			default: break;
		}
	}
	return container;
	label_fail:
	return NULL;
}
