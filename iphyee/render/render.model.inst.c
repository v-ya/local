#include "render.h"
#include "../loader/loader.model.view.h"
#include <memory.h>

/// TODO: move to /model/

static void iphyee_render_model_inst_part_free_func(iphyee_render_model_inst_part_s *restrict r)
{
	if (r->texture_name) refer_free(r->texture_name);
}

iphyee_render_model_inst_part_s* iphyee_render_model_inst_part_alloc(const iphyee_loader_model_view_surface_s *restrict surface, uint32_t fusion_offset, uint32_t fusion_number, uint32_t model_part_index)
{
	iphyee_render_model_inst_part_s *restrict r;
	if ((r = (iphyee_render_model_inst_part_s *) refer_alloz(sizeof(iphyee_render_model_inst_part_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_render_model_inst_part_free_func);
		r->texture_name = (refer_nstring_t) refer_save(surface->texture_name);
		r->fusion_offset = fusion_offset;
		r->fusion_number = fusion_number;
		r->model_part_index = model_part_index;
		r->texture_alpha = 0;
		return r;
	}
	return NULL;
}

// inst

static uintptr_t iphyee_render_model_inst_check_view(const iphyee_loader_model_view_s *restrict view, iphyee_glslc_model_count_t *restrict count)
{
	const iphyee_loader_model_view_surface_s *restrict surface;
	const vattr_vlist_t *restrict vl;
	const uint32_t *restrict pc;
	uintptr_t size, i, n;
	uint32_t fp;
	// check bits
	if (!view || view->bits_value != 32 || view->bits_index != 32)
		goto label_fail;
	// calc size
	size = sizeof(iphyee_glslc_model_inst_t);
	#define d_reserve_size(_name, _type)  (((count->count_##_name = (uint32_t) (view->_name.size / sizeof(_type))) + 1) * sizeof(_type))
	size += d_reserve_size(vertex, iphyee_glslc_vec3_xyz_t);
	size += d_reserve_size(texture, iphyee_glslc_vec2_uv_t);
	size += d_reserve_size(normal, iphyee_glslc_vec3_xyz_t);
	size += d_reserve_size(weight, iphyee_glslc_joint_weight_t);
	size += d_reserve_size(effect, iphyee_glslc_joint_effect_t);
	#undef d_reserve_size
	count->count_fusion = view->fragment.size / (sizeof(uint32_t) * view->length_of_fragment);
	size += count->count_fusion * sizeof(iphyee_glslc_index_fusion_t);
	// check surface
	for (vl = view->surface->vattr; vl; vl = vl->vattr_next)
	{
		surface = (const iphyee_loader_model_view_surface_s *) vl->value;
		pc = (const uint32_t *) surface->view.data;
		if ((n = surface->view.size / (sizeof(uint32_t) * 2)))
		{
			if ((fp = pc[0]) % 3 != 0)
				goto label_fail;
			for (i = 0; i < n; ++i)
			{
				if (pc[0] != fp || pc[1] != 3)
					goto label_fail;
				fp += 3;
				pc += 2;
			}
		}
	}
	return size;
	label_fail:
	return 0;
}

static uintptr_t iphyee_render_model_inst_write_effect(iphyee_glslc_joint_effect_t *restrict dst, const iphyee_glslc_joint_effect_t *restrict src, uint32_t effect_count, uint32_t weight_count)
{
	uint32_t i, res;
	for (i = 0; i < effect_count; ++i)
	{
		if (src[i].weight_offset < weight_count)
		{
			dst[i].weight_offset = src[i].weight_offset;
			res = weight_count - src[i].weight_offset;
			if (src[i].weight_number <= res)
				dst[i].weight_number = src[i].weight_number;
			else dst[i].weight_number = res;
		}
		else
		{
			dst[i].weight_offset = 0;
			dst[i].weight_number = 0;
		}
	}
	dst[i].weight_offset = 0;
	dst[i].weight_number = 0;
	return ((uintptr_t) effect_count + 1) * sizeof(iphyee_glslc_joint_effect_t);
}

static inline uint32_t iphyee_render_model_inst_fetch_index(const uint32_t *restrict fragment, uintptr_t at, uintptr_t length, uint32_t max)
{
	if (at < length && fragment[at] < max)
		return fragment[at];
	return max;
}

static uintptr_t iphyee_render_model_inst_write_fusion(iphyee_glslc_index_fusion_t *restrict fusion, const uint32_t *restrict fragment, const iphyee_loader_model_view_s *restrict view, const iphyee_glslc_model_count_t *restrict count)
{
	uint32_t i, n;
	n = count->count_fusion;
	for (i = 0; i < n; ++i)
	{
		#define d_fetch_index(_name)  fusion[i]._name = iphyee_render_model_inst_fetch_index(fragment, view->_name##_at_fragment, view->length_of_fragment, count->count_##_name)
		d_fetch_index(vertex);
		d_fetch_index(texture);
		d_fetch_index(normal);
		d_fetch_index(effect);
		#undef d_fetch_index
		fragment += view->length_of_fragment;
	}
	return sizeof(iphyee_glslc_index_fusion_t) * n;
}

static iphyee_render_model_inst_s* iphyee_render_model_inst_initial(iphyee_render_model_inst_s *restrict r, const iphyee_loader_model_view_s *restrict view, const iphyee_glslc_model_count_t *restrict count)
{
	const iphyee_loader_model_view_surface_s *restrict surface;
	iphyee_glslc_model_inst_t *restrict model_inst;
	iphyee_render_model_inst_part_s *restrict part;
	const vattr_vlist_t *restrict vl;
	const uint32_t *restrict pc;
	uint8_t *restrict data;
	vattr_vlist_t *result;
	uintptr_t size, n;
	uint32_t model_part_index;
	// set model inst
	model_inst = r->model_inst_data;
	#define d_reserve_end(_name, _type)  (model_inst->data.data_##_name + ((uintptr_t) count->count_##_name + 1) * sizeof(_type))
	model_inst->data.data_vertex = r->model_inst_address + sizeof(iphyee_glslc_model_inst_t);
	model_inst->data.data_texture = d_reserve_end(vertex, iphyee_glslc_vec3_xyz_t);
	model_inst->data.data_normal = d_reserve_end(texture, iphyee_glslc_vec2_uv_t);
	model_inst->data.data_weight = d_reserve_end(normal, iphyee_glslc_vec3_xyz_t);
	model_inst->data.data_effect = d_reserve_end(weight, iphyee_glslc_joint_weight_t);
	model_inst->data.data_fusion = d_reserve_end(effect, iphyee_glslc_joint_effect_t);
	#undef d_reserve_end
	memcpy(&model_inst->count, count, sizeof(*count));
	// set model data
	data = (uint8_t *) (model_inst + 1);
	#define d_write_copy(_name, _type)  \
		if ((size = sizeof(_type) * count->count_##_name))\
			memcpy(data, view->_name.data, size);\
		memset(data += size, 0, sizeof(_type));\
		data += sizeof(_type)
	d_write_copy(vertex, iphyee_glslc_vec3_xyz_t);
	d_write_copy(texture, iphyee_glslc_vec2_uv_t);
	d_write_copy(normal, iphyee_glslc_vec3_xyz_t);
	d_write_copy(weight, iphyee_glslc_joint_weight_t);
	data += iphyee_render_model_inst_write_effect((iphyee_glslc_joint_effect_t *) data,
		(const iphyee_glslc_joint_effect_t *) view->effect.data,
		count->count_effect, count->count_weight);
	#undef d_write_copy
	data += iphyee_render_model_inst_write_fusion((iphyee_glslc_index_fusion_t *) data,
		(const uint32_t *) view->fragment.data, view, count);
	// set part pool
	model_part_index = 0;
	for (vl = view->surface->vattr; vl; vl = vl->vattr_next)
	{
		surface = (const iphyee_loader_model_view_surface_s *) vl->value;
		pc = (const uint32_t *) surface->view.data;
		if ((n = surface->view.size / (sizeof(uint32_t) * 2)))
		{
			if (!(part = iphyee_render_model_inst_part_alloc(surface, pc[0] / 3, n, model_part_index)))
				goto label_fail;
			result = vattr_insert_tail(r->model_part_pool, vl->vslot->key, part);
			refer_free(part);
			if (!result)
				goto label_fail;
		}
		model_part_index += 1;
	}
	r->model_part_count = model_part_index;
	r->model_tri3_count = count->count_fusion / 3;
	return r;
	label_fail:
	return NULL;
}

static void iphyee_render_model_inst_free_func(iphyee_render_model_inst_s *restrict r)
{
	if (r->model_inst_data) iphyee_worker_buffer_unmap(r->model_inst_host);
	if (r->model_inst_device) refer_free(r->model_inst_device);
	if (r->model_inst_host) refer_free(r->model_inst_host);
	if (r->model_part_pool) refer_free(r->model_part_pool);
	if (r->worker) refer_free(r->worker);
}

iphyee_render_model_inst_s* iphyee_render_model_inst_alloc(iphyee_worker_s *restrict worker, const struct iphyee_loader_model_view_s *restrict model_view)
{
	iphyee_glslc_model_count_t count;
	iphyee_render_model_inst_s *restrict r;
	uintptr_t size;
	if ((size = iphyee_render_model_inst_check_view(model_view, &count)))
	{
		if ((r = (iphyee_render_model_inst_s *) refer_alloz(sizeof(iphyee_render_model_inst_s))))
		{
			refer_set_free(r, (refer_free_f) iphyee_render_model_inst_free_func);
			r->worker = (iphyee_worker_s *) refer_save(worker);
			if ((r->model_part_pool = vattr_alloc()) &&
				(r->model_inst_host = iphyee_worker_create_buffer_host(
					worker, size, iphyee_worker_buffer_usage__src)) &&
				(r->model_inst_device = iphyee_worker_create_buffer_device(
					worker, size, iphyee_worker_buffer_usage__dst)) &&
				(r->model_inst_data = iphyee_worker_buffer_map(r->model_inst_host, 0, size)) &&
				(r->model_inst_address = iphyee_worker_buffer_device_address(r->model_inst_device)))
			{
				r->model_inst_size = size;
				r->model_part_count = 0;
				r->model_tri3_count = 0;
				if (iphyee_render_model_inst_initial(r, model_view, &count))
					return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}
