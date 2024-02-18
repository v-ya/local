#include "render.h"

static uint64_t iphyee_render_rasterize_initial_block_array(iphyee_glslc_rasterize_block_t *restrict block, uint64_t address, uint32_t w, uint32_t h, uint32_t block_h_count, uint32_t block_v_count, uint32_t block_size, uint32_t block_max_tri3)
{
	uint32_t x, y;
	for (y = 0; y < block_v_count; y += 1)
	{
		for (x = 0; x < block_h_count; x += 1)
		{
			block->tri3_index = address;
			block->tri3_box.xmin = x * block_size;
			block->tri3_box.xmax = (x + 1) * block_size;
			block->tri3_box.ymin = y * block_size;
			block->tri3_box.ymax = (y + 1) * block_size;
			block->max_count = block_max_tri3;
			block->cur_count = 0;
			if (block->tri3_box.xmax > w)
				block->tri3_box.xmax = w;
			if (block->tri3_box.ymax > h)
				block->tri3_box.ymax = h;
			address += sizeof(uint32_t) * block_max_tri3;
		}
	}
	return address;
}

static void iphyee_render_rasterize_free_func(iphyee_render_rasterize_s *restrict r)
{
	if (r->rasterize_data) iphyee_worker_buffer_unmap(r->rasterize_host);
	if (r->rasterize_device) refer_free(r->rasterize_device);
	if (r->rasterize_host) refer_free(r->rasterize_host);
	if (r->tpool) refer_free(r->tpool);
	if (r->screen) refer_free(r->screen);
}

iphyee_render_rasterize_s* iphyee_render_rasterize_alloc(iphyee_render_screen_s *restrict screen, iphyee_render_tpool_s *restrict tpool, uint32_t block_size, uint32_t rasterize_max_tri3, uint32_t block_max_tri3)
{
	iphyee_render_rasterize_s *restrict r;
	iphyee_glslc_rasterize_t *restrict p;
	uint64_t host_size;
	uint64_t device_size;
	uint64_t address;
	uintptr_t block_count;
	uint32_t block_h_count;
	uint32_t block_v_count;
	if (screen && tpool && block_size && rasterize_max_tri3 && block_max_tri3)
	{
		block_h_count = (screen->width + block_size - 1) / block_size;
		block_v_count = (screen->height + block_size - 1) / block_size;
		block_count = (uintptr_t) block_h_count * block_v_count;
		host_size = sizeof(iphyee_glslc_rasterize_t) + sizeof(iphyee_glslc_rasterize_block_t) * 2 * block_count;
		device_size = host_size;
		device_size += sizeof(iphyee_glslc_tri3_render_t) * 2 * rasterize_max_tri3;
		device_size += sizeof(uint32_t) * 2 * block_max_tri3 * block_count;
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
				r->block_normal = (iphyee_glslc_rasterize_block_t *) (r->rasterize_data + 1);
				r->block_alpha = r->block_normal + block_count;
				r->rasterize_reset_size = host_size;
				address = r->rasterize_address + sizeof(iphyee_glslc_rasterize_block_t);
				p->image.pixels = r->screen->screen_address;
				p->image.width = r->screen->width;
				p->image.height = r->screen->height;
				p->depth = r->screen->depth_address;
				p->tpool = r->tpool->tpool_address;
				p->block_normal = address;
				address += sizeof(iphyee_glslc_rasterize_block_t) * block_count;
				p->block_alpha = address;
				address += sizeof(iphyee_glslc_rasterize_block_t) * block_count;
				p->tri3_normal.tri3_array = address;
				p->tri3_normal.tri3_max_count = rasterize_max_tri3;
				p->tri3_normal.tri3_cur_count = 0;
				address += sizeof(iphyee_glslc_tri3_render_t) * rasterize_max_tri3;
				p->tri3_alpha.tri3_array = address;
				p->tri3_alpha.tri3_max_count = rasterize_max_tri3;
				p->tri3_alpha.tri3_cur_count = 0;
				address += sizeof(iphyee_glslc_tri3_render_t) * rasterize_max_tri3;
				p->block_width = block_size;
				p->block_height = block_size;
				p->block_h_count = block_h_count;
				p->block_v_count = block_v_count;
				address = iphyee_render_rasterize_initial_block_array(
					r->block_normal, address,
					screen->width, screen->height,
					block_h_count, block_v_count,
					block_size, block_max_tri3);
				address = iphyee_render_rasterize_initial_block_array(
					r->block_alpha, address,
					screen->width, screen->height,
					block_h_count, block_v_count,
					block_size, block_max_tri3);
				return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}
