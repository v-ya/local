#include "render.h"
#include <memory.h>

static void iphyee_render_rasterize_free_func(iphyee_render_rasterize_s *restrict r)
{
	if (r->rasterize_data) iphyee_worker_buffer_unmap(r->rasterize_host);
	if (r->rasterize_device) refer_free(r->rasterize_device);
	if (r->rasterize_host) refer_free(r->rasterize_host);
	if (r->tpool) refer_free(r->tpool);
	if (r->screen) refer_free(r->screen);
}

iphyee_render_rasterize_s* iphyee_render_rasterize_alloc(iphyee_render_screen_s *restrict screen, iphyee_render_tpool_s *restrict tpool, uint32_t block_size, uint32_t tri3_max_count, uint32_t list_max_count)
{
	iphyee_render_rasterize_s *restrict r;
	iphyee_glslc_rasterize_t *restrict p;
	uint64_t host_size;
	uint64_t device_size;
	uintptr_t block_count;
	uint32_t block_h_count;
	uint32_t block_v_count;
	if (screen && tpool && block_size && tri3_max_count && list_max_count)
	{
		block_h_count = (screen->width + block_size - 1) / block_size;
		block_v_count = (screen->height + block_size - 1) / block_size;
		block_count = (uintptr_t) block_h_count * block_v_count;
		host_size = sizeof(iphyee_glslc_rasterize_t) + sizeof(uint32_t) * 2 * block_count;
		host_size = (host_size + 15) & ~(uint64_t) 15;
		device_size = host_size;
		device_size += sizeof(iphyee_glslc_tri3_render_t) * tri3_max_count;
		device_size += sizeof(iphyee_glslc_index_list_t) * list_max_count;
		if ((r = (iphyee_render_rasterize_s *) refer_alloz(sizeof(iphyee_render_rasterize_s))))
		{
			refer_set_free(r, (refer_free_f) iphyee_render_rasterize_free_func);
			r->screen = (iphyee_render_screen_s *) refer_save(screen);
			r->tpool = (iphyee_render_tpool_s *) refer_save(tpool);
			if ((r->rasterize_host = iphyee_worker_create_buffer_host(
					tpool->worker, host_size, iphyee_worker_buffer_usage__src)) &&
				(r->rasterize_device = iphyee_worker_create_buffer_device(
					tpool->worker, device_size, iphyee_worker_buffer_usage__dst)) &&
				(r->rasterize_data = p = iphyee_worker_buffer_map(r->rasterize_host, 0, host_size)) &&
				(r->rasterize_address = iphyee_worker_buffer_device_address(r->rasterize_device)))
			{
				r->rasterize_reset_size = host_size;
				memset(p, -1, host_size);
				p->image.pixel = r->screen->screen_address;
				p->image.width = r->screen->width;
				p->image.height = r->screen->height;
				p->depth = r->screen->depth_address;
				p->tpool = r->tpool->tpool_address;
				p->tri3 = r->rasterize_address + host_size;
				p->list = r->rasterize_address + host_size +
					sizeof(iphyee_glslc_tri3_render_t) * tri3_max_count;
				p->block_normal = r->rasterize_address + sizeof(iphyee_glslc_rasterize_t);
				p->block_alpha = r->rasterize_address + sizeof(iphyee_glslc_rasterize_t) +
					sizeof(uint32_t) * block_count;
				p->tri3_max_count = tri3_max_count;
				p->tri3_cur_count = 0;
				p->list_max_count = list_max_count;
				p->list_cur_count = 0;
				p->block_width = block_size;
				p->block_height = block_size;
				p->block_h_count = block_h_count;
				p->block_v_count = block_v_count;
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}
