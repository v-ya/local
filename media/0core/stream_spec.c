#include "stream_spec.h"

static void media_stream_spec_free_func(struct media_stream_spec_s *restrict r)
{
	if (r->stream_type) refer_free(r->stream_type);
	if (r->frame_id_name) refer_free(r->frame_id_name);
	if (r->stack_layout) refer_free(r->stack_layout);
	if (r->judge) refer_free(r->judge);
}

static struct media_stream_spec_s* media_stream_spec_create_used_judge(const struct media_s *restrict media, const struct media_stream_spec_param_t *restrict param, const struct media_attr_judge_s *restrict judge)
{
	struct media_stream_spec_s *restrict r;
	if ((r = (struct media_stream_spec_s *) refer_alloz(sizeof(struct media_stream_spec_s))))
	{
		refer_set_free(r, (refer_free_f) media_stream_spec_free_func);
		if ((r->stream_type = (refer_string_t) media_get_string(media, param->stream_type)) &&
			(r->frame_id_name = (refer_string_t) media_get_string(media, param->frame_id_name)) &&
			(r->stack_layout = (refer_string_t) media_get_string(media, param->stack_layout)))
		{
			if (judge) r->judge = (const struct media_attr_judge_s *) refer_save(judge);
			else if (param->initial_judge && !(r->judge = media_attr_judge_create(param->initial_judge)))
				goto label_fail;
			r->create_pri = param->create_pri;
			r->append_pre = param->append_pre;
			r->read_frame = param->read_frame;
			r->write_frame = param->write_frame;
			return r;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

vattr_s* media_stream_spec_append(vattr_s *restrict stream_spec, const struct media_s *restrict media, const struct media_stream_spec_param_t *restrict param, uintptr_t nparam)
{
	struct media_stream_spec_s *restrict spec;
	vattr_vlist_t *restrict vlist;
	const struct media_attr_judge_s *restrict judge;
	media_attr_judge_initial_f last_initial_judge;
	vlist = NULL;
	judge = NULL;
	last_initial_judge = NULL;
	while (nparam)
	{
		if (param->initial_judge != last_initial_judge)
			judge = NULL;
		if (!(spec = media_stream_spec_create_used_judge(media, param, judge)))
			goto label_fail;
		vlist = vattr_insert_tail(stream_spec, spec->stream_type, spec);
		refer_free(spec);
		if (!vlist) goto label_fail;
		// now stream_spec have spec
		judge = spec->judge;
		last_initial_judge = param->initial_judge;
		nparam -= 1;
		param += 1;
	}
	return stream_spec;
	label_fail:
	return NULL;
}
