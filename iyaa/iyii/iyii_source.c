#include "iyii_source.h"

static void iyii_source_free_func(iyii_source_s *restrict r)
{
	if (r->texture_view) refer_free(r->texture_view);
	if (r->texture_view_param) refer_free(r->texture_view_param);
	if (r->cpool_tran) refer_free(r->cpool_tran);
	if (r->buffer_uniform) refer_free(r->buffer_uniform);
	if (r->buffer_dev) refer_free(r->buffer_dev);
	if (r->buffer_host) refer_free(r->buffer_host);
	if (r->buffer_texture) refer_free(r->buffer_texture);
	if (r->texture) refer_free(r->texture);
	if (r->sampler) refer_free(r->sampler);
	if (r->sampler_param) refer_free(r->sampler_param);
	if (r->mheap) refer_free(r->mheap);
}

iyii_source_s* iyii_source_alloc(graph_dev_s *restrict dev, const graph_device_queue_t *transfer)
{
	iyii_source_s *restrict r;
	r = (iyii_source_s *) refer_alloz(sizeof(iyii_source_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) iyii_source_free_func);
		r->mheap = graph_memory_heap_alloc(dev);
		if (!r->mheap) goto label_fail;
		r->sampler_param = graph_sampler_param_alloc();
		if (!r->sampler_param) goto label_fail;
		r->texture_view_param = graph_image_view_param_alloc(graph_image_view_type_2D);
		if (!r->texture_view_param) goto label_fail;
		graph_sampler_param_set_filter(r->sampler_param, graph_filter_linear, graph_filter_linear);
		graph_sampler_param_set_anisotropy(r->sampler_param, 1, 16);
		graph_sampler_param_set_mipmap(r->sampler_param, graph_sampler_mipmap_mode_linear, 0, 0, 0);
		r->sampler = graph_sampler_alloc(r->sampler_param, dev);
		if (!r->sampler) goto label_fail;
		r->cpool_tran = graph_command_pool_alloc(dev, transfer, graph_command_pool_flags_transient, iyii_source_transfer_number);
		if (!r->cpool_tran) goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iyii_source_s* iyii_source_ready(iyii_source_s *restrict source, uintptr_t texture_width, uintptr_t texture_height, uintptr_t vertex_size, uintptr_t index_size, uintptr_t uniform_size, uintptr_t uniform_number)
{
	#define algin(v)  v = ((v + 31) & ~(uintptr_t) 31)
	algin(vertex_size);
	algin(index_size);
	algin(uniform_size);
	#undef algin
	// set texture
	source->texture$width = texture_width;
	source->texture$height = texture_height;
	source->texture$linesize = source->texture$width * 4;
	source->texture$size = source->texture$linesize * source->texture$height;
	// set vertex
	source->vertex$offset = 0;
	source->vertex$size = vertex_size;
	// set index
	source->index$offset = vertex_size;
	source->index$size = index_size;
	// set transfer
	source->transfer$size = vertex_size + index_size;
	// set uniform
	source->uniform$size = uniform_size;
	source->uniform$number = uniform_number;
	// alloc texture && buffer
	source->texture = graph_image_alloc_2d(
		source->mheap,
		graph_format_b8g8r8a8_unorm,
		source->texture$width,
		source->texture$height,
		NULL);
	source->buffer_texture = graph_buffer_alloc(
		source->mheap,
		source->texture$size,
		0,
		graph_buffer_usage_transfer_src,
		graph_memory_property_host_visible |
			graph_memory_property_host_coherent,
		0, NULL);
	if (!source->texture || !source->buffer_texture)
		goto label_fail;
	// alloc texture's image view
	source->texture_view = graph_image_view_alloc(source->texture_view_param, source->texture);
	if (!source->texture_view) goto label_fail;
	// alloc (host && dev) buffer
	source->buffer_host = graph_buffer_alloc(
		source->mheap,
		source->transfer$size,
		0,
		graph_buffer_usage_transfer_src,
		graph_memory_property_host_visible |
			graph_memory_property_host_coherent,
		0, NULL);
	source->buffer_dev = graph_buffer_alloc(
		source->mheap,
		source->transfer$size,
		0,
		graph_buffer_usage_transfer_dst |
			graph_buffer_usage_vertex_buffer |
			graph_buffer_usage_index_buffer,
		graph_memory_property_device_local,
		0, NULL);
	if (!source->buffer_host || !source->buffer_dev)
		goto label_fail;
	// alloc uniform buffer
	source->buffer_uniform = graph_buffer_alloc(
		source->mheap,
		source->uniform$size * source->uniform$number,
		0,
		graph_buffer_usage_uniform_buffer,
		graph_memory_property_host_visible |
			graph_memory_property_host_coherent,
		0, NULL);
	if (!source->buffer_uniform)
		goto label_fail;
	return source;
	label_fail:
	return NULL;
}

iyii_source_s* iyii_source_build_transfer(iyii_source_s *restrict source)
{
	graph_command_pipe_barrier_param_s *param;
	if (!graph_command_pool_begin(source->cpool_tran, ~0, graph_command_buffer_usage_simultaneous_use))
		goto label_fail;
	// iyii_source_transfer$transfer
	graph_command_copy_buffer(source->cpool_tran,
		iyii_source_transfer$transfer,
		source->buffer_dev,
		source->buffer_host,
		0, 0,
		source->transfer$size);
	// iyii_source_transfer$vertex
	graph_command_copy_buffer(source->cpool_tran,
		iyii_source_transfer$vertex,
		source->buffer_dev,
		source->buffer_host,
		source->vertex$offset,
		source->vertex$offset,
		source->vertex$size);
	// iyii_source_transfer$index
	graph_command_copy_buffer(source->cpool_tran,
		iyii_source_transfer$index,
		source->buffer_dev,
		source->buffer_host,
		source->index$offset,
		source->index$offset,
		source->index$size);
	// iyii_source_transfer$texture
	param = graph_command_pipe_barrier_param_alloc(0, 0, 1);
	if (param)
	{
		graph_command_pipe_barrier_param_set_image(
			param, 0,
			source->texture,
			graph_image_aspect_flags_color,
			0, 1,
			0, 1);
		graph_command_pipe_barrier_param_set_image_layout(
			param, 0,
			graph_image_layout_undefined,
			graph_image_layout_transfer_dst_optimal);
		graph_command_pipe_barrier_param_set_image_access(
			param, 0,
			0,
			graph_access_flags_transfer_write);
		graph_command_pipe_barrier(
			source->cpool_tran,
			iyii_source_transfer$texture,
			graph_pipeline_stage_flags_top_of_pipe,
			graph_pipeline_stage_flags_transfer,
			0,
			param);
		graph_command_copy_buffer_to_image(
			source->cpool_tran,
			iyii_source_transfer$texture,
			source->texture,
			source->buffer_texture,
			graph_image_layout_transfer_dst_optimal,
			0,
			graph_image_aspect_flags_color,
			NULL, NULL);
		graph_command_pipe_barrier_param_set_image_layout(
			param, 0,
			graph_image_layout_transfer_dst_optimal,
			graph_image_layout_shader_read_only_optimal);
		graph_command_pipe_barrier_param_set_image_access(param, 0,
			graph_access_flags_transfer_write,
			graph_access_flags_shader_read);
		graph_command_pipe_barrier(
			source->cpool_tran,
			iyii_source_transfer$texture,
			graph_pipeline_stage_flags_transfer,
			graph_pipeline_stage_flags_fragment_shader,
			0,
			param);
		refer_free(param);
	}
	if (!graph_command_pool_end(source->cpool_tran, ~0)) goto label_fail;
	return source;
	label_fail:
	return NULL;
}
