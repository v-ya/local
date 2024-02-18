#include "render.h"

static iphyee_render_tpool_s* iphyee_render_tpool_new_index(iphyee_render_tpool_s *restrict r, iphyee_render_texture_s *restrict texture)
{
	iphyee_glslc_image_t *restrict pi;
	uint64_t *restrict p;
	uint64_t pm, mask;
	uint32_t index, m, n;
	p = r->tpool_mask;
	n = ((m = r->texture_max_count) + 63) / 64;
	for (index = 0; index < n; ++index)
	{
		if (~p[index])
		{
			pm = *(p += index);
			index *= 64;
			mask = 1;
			while (mask && index < m)
			{
				if (!(pm & mask))
				{
					r->texture_use_count += 1;
					*p |= mask;
					pi = r->texture_array + index;
					pi->pixels = texture->texture_address;
					pi->width = texture->texture_width;
					pi->height = texture->texture_height;
					texture->texture_index = index;
					r->need_update = 1;
					return r;
				}
				mask <<= 1;
				index += 1;
			}
		}
	}
	return NULL;
}

static void iphyee_render_tpool_del_index(iphyee_render_tpool_s *restrict r, iphyee_render_texture_s *restrict texture)
{
	iphyee_glslc_image_t *restrict pi;
	uint32_t index;
	if ((index = texture->texture_index) < r->texture_max_count &&
		(pi = r->texture_array + index)->pixels == texture->texture_address)
	{
		texture->texture_vaild = 0;
		texture->texture_index = 0;
		texture = r->texture_miss;
		pi->pixels = texture->texture_address;
		pi->width = texture->texture_width;
		pi->height = texture->texture_height;
		r->tpool_mask[(index + 63) / 64] &= ~((uint64_t) 1 << (index & 63));
		r->texture_use_count -= 1;
		r->need_update = 1;
	}
}

static void iphyee_render_tpool_free_func(iphyee_render_tpool_s *restrict r)
{
	if (r->texture_array) iphyee_worker_buffer_unmap(r->tpool_host);
	if (r->texture_delete) refer_free(r->texture_delete);
	if (r->texture_insert) refer_free(r->texture_insert);
	if (r->texture_pool) refer_free(r->texture_pool);
	if (r->texture_miss) refer_free(r->texture_miss);
	if (r->tpool_device) refer_free(r->tpool_device);
	if (r->tpool_host) refer_free(r->tpool_host);
	if (r->worker) refer_free(r->worker);
}

iphyee_render_tpool_s* iphyee_render_tpool_alloc(iphyee_worker_s *restrict worker, uint32_t texture_max_count, iphyee_render_texture_s *restrict miss)
{
	iphyee_render_tpool_s *restrict r;
	iphyee_glslc_image_t *restrict p;
	uintptr_t size, tpool_size;
	tpool_size = sizeof(iphyee_render_tpool_s) + sizeof(uint64_t) * ((texture_max_count + 63) / 64);
	if ((size = sizeof(iphyee_glslc_image_t) * texture_max_count) && miss &&
		(r = (iphyee_render_tpool_s *) refer_alloz(tpool_size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_render_tpool_free_func);
		r->worker = (iphyee_worker_s *) refer_save(worker);
		r->texture_miss = (iphyee_render_texture_s *) refer_save(miss);
		if ((r->tpool_host = iphyee_worker_create_buffer_host(
				worker, size, iphyee_worker_buffer_usage__src)) &&
			(r->tpool_device = iphyee_worker_create_buffer_device(
				worker, size, iphyee_worker_buffer_usage__dst)) &&
			iphyee_render_texture_device_enable(r->texture_miss) &&
			(r->texture_array = iphyee_worker_buffer_map(
				r->tpool_host, 0, size)) &&
			(r->texture_pool = vattr_alloc()) &&
			(r->texture_insert = queue_alloc_ring(texture_max_count)) &&
			(r->texture_delete = queue_alloc_ring(texture_max_count)) &&
			(r->tpool_address = iphyee_worker_buffer_device_address(r->tpool_device)) &&
			r->texture_insert->push(r->texture_insert, r->texture_miss))
		{
			r->tpool_mask = (uint64_t *) (r + 1);
			r->tpool_size = size;
			r->need_update = 1;
			r->texture_max_count = texture_max_count;
			r->texture_use_count = 0;
			for (p = r->texture_array; texture_max_count; texture_max_count -= 1, p += 1)
			{
				p->pixels = miss->texture_address;
				p->width = miss->texture_width;
				p->height = miss->texture_height;
			}
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_render_tpool_s* iphyee_render_tpool_insert(iphyee_render_tpool_s *restrict r, iphyee_render_texture_s *restrict texture)
{
	vattr_vlist_t *restrict vl;
	queue_s *restrict q;
	if (texture && texture->worker == r->worker && !texture->texture_device &&
		r->texture_use_count < r->texture_max_count &&
		!vattr_get_vslot(r->texture_pool, texture->texture_name) &&
		iphyee_render_texture_device_enable(texture))
	{
		if ((vl = vattr_insert_tail(r->texture_pool, texture->texture_name, texture)))
		{
			if (iphyee_render_tpool_new_index(r, texture))
			{
				q = r->texture_insert;
				if (q->push(q, texture))
					return r;
				iphyee_render_tpool_del_index(r, texture);
			}
			vattr_delete_vlist(vl);
		}
		iphyee_render_texture_device_disable(texture);
	}
	return NULL;
}

iphyee_render_tpool_s* iphyee_render_tpool_delete(iphyee_render_tpool_s *restrict r, const char *restrict texture_name)
{
	iphyee_render_texture_s *restrict texture;
	vattr_vlist_t *restrict vl;
	queue_s *restrict q;
	if (texture_name && (vl = vattr_get_vlist_first(r->texture_pool, texture_name)))
	{
		texture = (iphyee_render_texture_s *) vl->value;
		q = r->texture_delete;
		if (texture->texture_device && q->push(q, texture->texture_device))
		{
			r->need_update = 1;
			iphyee_render_tpool_del_index(r, texture);
			iphyee_render_texture_device_disable(texture);
			vattr_delete_vlist(vl);
			return r;
		}
	}
	return NULL;
}

void iphyee_render_tpool_update(iphyee_render_tpool_s *restrict r, iphyee_worker_command_buffer_s *restrict cb_transfer)
{
	iphyee_render_texture_s *restrict t;
	queue_s *restrict q;
	refer_t v;
	if (r->need_update && cb_transfer)
	{
		q = r->texture_delete;
		while ((v = q->pull(q)))
			refer_free(v);
		q = r->texture_insert;
		while ((t = (iphyee_render_texture_s *) q->pull(q)))
		{
			iphyee_worker_cmd_copy_buffer(cb_transfer, t->texture_host, t->texture_device, 0, 0, t->texture_size);
			t->texture_vaild = 1;
			refer_free(t);
		}
		iphyee_worker_cmd_copy_buffer(cb_transfer, r->tpool_host, r->tpool_device, 0, 0, r->tpool_size);
		r->need_update = 0;
	}
}
