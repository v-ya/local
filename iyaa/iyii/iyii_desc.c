#include "iyii_desc.h"

static void iyii_desc_free_func(iyii_desc_s *restrict r)
{
	if (r->info) refer_free(r->info);
	if (r->sets) refer_free(r->sets);
	if (r->pool) refer_free(r->pool);
}

iyii_desc_s* iyii_desc_alloc(graph_dev_s *restrict dev, iyii_source_s *restrict source, graph_descriptor_set_layout_s *restrict set_layout)
{
	iyii_desc_s *restrict r;
	r = (iyii_desc_s *) refer_alloz(sizeof(iyii_desc_s));
	if (r)
	{
		graph_descriptor_set_layout_s *set_layouts[source->uniform$number];
		uint32_t i, n, k;
		n = (uint32_t) source->uniform$number;
		refer_set_free(r, (refer_free_f) iyii_desc_free_func);
		r->pool = graph_descriptor_pool_alloc(
			dev, 0, n, 2,
			(graph_desc_type_t []) {graph_desc_type_uniform_buffer, graph_desc_type_combined_image_sampler},
			(uint32_t []) {n, n}
		);
		if (!r->pool) goto label_fail;
		for (i = 0; i < n; ++i)
			set_layouts[i] = set_layout;
		r->sets = graph_descriptor_sets_alloc(r->pool, n, set_layouts);
		if (!r->sets) goto label_fail;
		r->info = graph_descriptor_sets_info_alloc(r->sets, n * 2, 0, n, n, 0);
		if (!r->info) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			k = graph_descriptor_sets_info_append_write(
				r->info, i,
				0, graph_desc_type_uniform_buffer,
				0, 1);
			graph_descriptor_sets_info_set_write_buffer_info(
				r->info, k,
				0, source->buffer_uniform,
				i * source->uniform$size,
				source->uniform$size);
			k = graph_descriptor_sets_info_append_write(
				r->info, i,
				1, graph_desc_type_combined_image_sampler,
				0, 1);
			graph_descriptor_sets_info_set_write_image_info(
				r->info, k,
				0, source->sampler,
				source->texture_view,
				graph_image_layout_shader_read_only_optimal);
		}
		graph_descriptor_sets_info_update(r->info);
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
