#include "render.h"
#include <memory.h>

static void iphyee_render_model_free_func(iphyee_render_model_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->model_part_count; i < n; ++i)
	{
		if (r->part[i].part) refer_free(r->part[i].part);
		if (r->part[i].texture) refer_free(r->part[i].texture);
	}
	if (r->model_part_array) iphyee_worker_buffer_unmap(r->model_host);
	if (r->model_device) refer_free(r->model_device);
	if (r->model_host) refer_free(r->model_host);
	if (r->tpool) refer_free(r->tpool);
	if (r->inst) refer_free(r->inst);
}

iphyee_render_model_s* iphyee_render_model_alloc(const iphyee_render_model_inst_s *restrict inst, const iphyee_render_tpool_s *restrict tpool)
{
	const iphyee_render_model_inst_part_s *restrict p;
	const iphyee_render_texture_s *restrict t;
	iphyee_render_model_s *restrict r;
	const vattr_vlist_t *restrict vl;
	iphyee_glslc_model_t *restrict m;
	uint64_t host_size;
	uint64_t device_size;
	uint64_t address;
	uintptr_t size;
	uintptr_t i, n;
	n = inst->model_part_count;
	size = sizeof(iphyee_render_model_s) + sizeof(iphyee_render_model_part_t) * n;
	host_size = sizeof(iphyee_glslc_model_t) * n;
	device_size = host_size + sizeof(iphyee_glslc_tri3_fusion_t) * inst->model_tri3_count;
	if (n && tpool && (r = (iphyee_render_model_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_render_model_free_func);
		r->inst = (const iphyee_render_model_inst_s *) refer_save(inst);
		r->tpool = (const iphyee_render_tpool_s *) refer_save(tpool);
		if ((r->model_host = iphyee_worker_create_buffer_host(
				inst->worker, host_size, iphyee_worker_buffer_usage__src)) &&
			(r->model_device = iphyee_worker_create_buffer_device(
				inst->worker, device_size, iphyee_worker_buffer_usage__dst)) &&
			(r->model_part_array = iphyee_worker_buffer_map(r->model_host, 0, host_size)) &&
			(r->model_part_address = iphyee_worker_buffer_device_address(r->model_device)))
		{
			address = r->model_part_address;
			for (i = 0, vl = inst->model_part_pool->vattr; i < n && vl; i += 1, vl = vl->vattr_next)
			{
				p = (const iphyee_render_model_inst_part_s *) vl->value;
				t = iphyee_render_tpool_find(tpool, p->texture_name?p->texture_name->string:NULL);
				m = r->model_part_array + i;
				memcpy(&m->inst, inst->model_inst_data, sizeof(iphyee_glslc_model_inst_t));
				m->transform_fusion = r->model_part_address + host_size;
				m->fusion_offset = p->fusion_offset;
				m->fusion_number = p->fusion_number;
				m->texture_index = t?t->texture_index:0;
				m->texture_alpha = p->texture_alpha;
				r->part[i].part = (const iphyee_render_model_inst_part_s *) refer_save(p);
				r->part[i].texture = (const iphyee_render_texture_s *) refer_save(t);
				r->part[i].model_part_data = m;
				r->part[i].model_part_address = address;
				address += sizeof(iphyee_glslc_model_t);
			}
			r->model_update_size = sizeof(iphyee_glslc_model_t) * i;
			r->model_part_count = i;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}
