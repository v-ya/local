#include "iyii_pipeline.h"
#include "shader/shader.h"

static iyii_pipeline_s* iyii_pipeline_create_shader(iyii_pipeline_s *restrict r)
{
	r->shader_vertex = graph_shader_alloc(r->dev, iyii_shader$vert$data, iyii_shader$vert$size);
	r->shader_fragment = graph_shader_alloc(r->dev, iyii_shader$frag$data, iyii_shader$frag$size);
	if (r->shader_vertex && r->shader_fragment)
		return r;
	return NULL;
}

static iyii_pipeline_s* iyii_pipeline_create_vertex_desc(iyii_pipeline_s *restrict r)
{
	r->vertex_input_desc = graph_vertex_input_description_alloc(1, 2);
	if (r->vertex_input_desc)
	{
		uint32_t offset = 0;
		// layout (location = 0) in vec2 pos
		graph_vertex_input_description_set_attr(r->vertex_input_desc, 0, 0, 0, offset, graph_format_r32g32_sfloat);
		offset += sizeof(float) * 2;
		// layout (location = 1) in vec3 color
		graph_vertex_input_description_set_attr(r->vertex_input_desc, 1, 1, 0, offset, graph_format_r32g32b32_sfloat);
		offset += sizeof(float) * 3;
		// binding = 0
		graph_vertex_input_description_set_bind(r->vertex_input_desc, 0, 0, offset, graph_vertex_input_rate_vertex);
		return r;
	}
	return NULL;
}

static iyii_pipeline_s* iyii_pipeline_create_viewports(iyii_pipeline_s *restrict r)
{
	r->viewports = graph_viewports_scissors_alloc(1, 1);
	if (r->viewports &&
		graph_viewports_scissors_append_viewport(r->viewports,
			(float) r->output_x, (float) r->output_y,
			(float) r->output_width, (float) r->output_height,
			0, 1) &&
		graph_viewports_scissors_append_scissor(r->viewports,
			r->output_x, r->output_y,
			r->output_width, r->output_height)
	) return r;
	return NULL;
}

static iyii_pipeline_s* iyii_pipeline_create_color_blend(iyii_pipeline_s *restrict r)
{
	r->color_blend = graph_pipe_color_blend_alloc(1);
	if (r->color_blend)
	{
		// color = s_color * s_alpha + d_color * (1 - s_alpha)
		// alpha = s_alpha * 1 + d_alpha * (1 - s_alpha)
		//       = s_alpha + d_alpha - s_alpha * d_alpha
		//       = 1 - (1 - s_alpha) * (1 - d_alpha)
		if (graph_pipe_color_blend_append_attachment(
			r->color_blend, graph_bool_true,
			graph_color_component_mask_all,
			graph_blend_factor_src_alpha,
			graph_blend_factor_one_minus_src_alpha,
			graph_blend_op_add,
			graph_blend_factor_one,
			graph_blend_factor_one_minus_src_alpha,
			graph_blend_op_add
		)) return r;
	}
	return NULL;
}

static iyii_pipeline_s* iyii_pipeline_create_layout(iyii_pipeline_s *restrict r)
{
	graph_descriptor_set_layout_param_s *desc_set_layout_param;
	graph_pipe_layout_param_s *pipe_layout_param;
	iyii_pipeline_s *ret;
	ret = NULL;
	if ((desc_set_layout_param = graph_descriptor_set_layout_param_alloc(2, 0, 0)))
	{
		if (
			graph_descriptor_set_layout_param_set_binding(
				desc_set_layout_param, 0, 0,
				graph_desc_type_uniform_buffer,
				1, graph_shader_stage_flags_vertex) &&
			graph_descriptor_set_layout_param_set_binding(
				desc_set_layout_param, 1, 1,
				graph_desc_type_combined_image_sampler,
				1, graph_shader_stage_flags_fragment) &&
			(r->desc_set_layout = graph_descriptor_set_layout_alloc(
				desc_set_layout_param, r->dev)) &&
			(pipe_layout_param = graph_pipe_layout_param_alloc(1, 0))
		) {
			graph_pipe_layout_param_append_set_layout(pipe_layout_param, r->desc_set_layout);
			if ((r->layout = graph_pipe_layout_alloc(r->dev, pipe_layout_param)))
				ret = r;
			refer_free(pipe_layout_param);
		}
		refer_free(desc_set_layout_param);
	}
	return ret;
}

static iyii_pipeline_s* iyii_pipeline_create_render_pass(iyii_pipeline_s *restrict r)
{
	graph_render_pass_param_s *param;
	iyii_pipeline_s *ret;
	ret = NULL;
	if ((param = graph_render_pass_param_alloc(1, 1, 0, 1)))
	{
		if (
			graph_render_pass_param_set_attachment(
				param, 0,
				r->format, graph_sample_count_1,
				graph_attachment_load_op_clear,
				graph_attachment_store_op_store,
				graph_attachment_load_op_none,
				graph_attachment_store_op_none,
				graph_image_layout_undefined,
				graph_image_layout_khr_present_src
			) &&
			graph_render_pass_param_set_subpass(param, 0, graph_pipeline_bind_point_graphics) &&
			graph_render_pass_param_set_subpass_color(param, 0, 1,
				(uint32_t []) {0},
				(graph_image_layout_t []) {graph_image_layout_color_attachment_optimal}
			)
		) {
			if ((r->render_pass = graph_render_pass_alloc(param, r->dev)))
				ret = r;
		}
		refer_free(param);
	}
	return ret;
}

static iyii_pipeline_s* iyii_pipeline_create_graphics_pipe(iyii_pipeline_s *restrict r)
{
	r->gpipe_param = graph_gpipe_param_alloc(r->dev, 2);
	if (r->gpipe_param)
	{
		if (
			graph_gpipe_param_append_shader(r->gpipe_param,
				r->shader_vertex, graph_shader_type_vertex,
				"main", NULL) &&
			graph_gpipe_param_append_shader(r->gpipe_param,
				r->shader_fragment, graph_shader_type_fragment,
				"main", NULL) &&
			(graph_gpipe_param_set_vertex(r->gpipe_param, r->vertex_input_desc), 1) &&
			graph_gpipe_param_set_assembly(r->gpipe_param, graph_primitive_topology_triangle_list, 0) &&
			graph_gpipe_param_set_viewport(r->gpipe_param, r->viewports) &&
			graph_gpipe_param_set_color_blend(r->gpipe_param, r->color_blend) &&
			(
				graph_gpipe_param_set_rasterization_polygon(r->gpipe_param, graph_polygon_mode_fill),
				graph_gpipe_param_set_rasterization_cull(r->gpipe_param, graph_cull_mode_flags_back),
				graph_gpipe_param_set_rasterization_front_face(r->gpipe_param, graph_front_face_clockwise),
			1) &&
			graph_gpipe_param_set_dynamic(r->gpipe_param, 2,
				(graph_dynamic_t []) {graph_dynamic_viewport, graph_dynamic_line_width}
			) &&
			(graph_gpipe_param_set_layout(r->gpipe_param, r->layout), 1) &&
			(graph_gpipe_param_set_render(r->gpipe_param, r->render_pass, 0), 1) &&
			graph_gpipe_param_ok(r->gpipe_param)
		) {
			if ((r->gpipe = graph_pipe_alloc_graphics(r->gpipe_param, r->cache)))
				return r;
		}
	}
	return NULL;
}

static void iyii_pipeline_free_func(iyii_pipeline_s *restrict r)
{
	if (r->render) refer_free(r->render);
	if (r->gpipe) refer_free(r->gpipe);
	if (r->gpipe_param) refer_free(r->gpipe_param);
	if (r->render_pass) refer_free(r->render_pass);
	if (r->layout) refer_free(r->layout);
	if (r->desc_set_layout) refer_free(r->desc_set_layout);
	if (r->color_blend) refer_free(r->color_blend);
	if (r->viewports) refer_free(r->viewports);
	if (r->vertex_input_desc) refer_free(r->vertex_input_desc);
	if (r->shader_fragment) refer_free(r->shader_fragment);
	if (r->shader_vertex) refer_free(r->shader_vertex);
	if (r->cache) refer_free(r->cache);
	if (r->dev) refer_free(r->dev);
}

iyii_pipeline_s* iyii_pipeline_alloc(graph_dev_s *restrict dev, graph_format_t format, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
	iyii_pipeline_s *restrict r;
	r = (iyii_pipeline_s *) refer_alloz(sizeof(iyii_pipeline_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) iyii_pipeline_free_func);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->output_x = x;
		r->output_y = y;
		r->output_width = w;
		r->output_height = h;
		r->format = format;
		if (!(r->cache = graph_pipe_cache_alloc(r->dev))) goto label_fail;
		if (!iyii_pipeline_create_shader(r)) goto label_fail;
		if (!iyii_pipeline_create_vertex_desc(r)) goto label_fail;
		if (!iyii_pipeline_create_viewports(r)) goto label_fail;
		if (!iyii_pipeline_create_color_blend(r)) goto label_fail;
		if (!iyii_pipeline_create_layout(r)) goto label_fail;
		if (!iyii_pipeline_create_render_pass(r)) goto label_fail;
		if (!iyii_pipeline_create_graphics_pipe(r)) goto label_fail;
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iyii_pipeline_s* iyii_pipeline_set_swapchain(iyii_pipeline_s *restrict pipe, iyii_swapchain_s *restrict swapchain)
{
	if (pipe->render) refer_free(pipe->render);
	if ((pipe->render = iyii_render_alloc(pipe->render_pass, swapchain)))
		return pipe;
	return NULL;
}
