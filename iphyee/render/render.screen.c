#include "render.h"

static void iphyee_render_screen_free_func(iphyee_render_screen_s *restrict r)
{
	if (r->screen_data) iphyee_worker_buffer_unmap(r->screen_host);
	if (r->buffer_device) refer_free(r->buffer_device);
	if (r->screen_host) refer_free(r->screen_host);
}

iphyee_render_screen_s* iphyee_render_screen_alloc(iphyee_worker_s *restrict worker, uint32_t width, uint32_t height)
{
	iphyee_render_screen_s *restrict r;
	uint64_t pixels_count;
	uint64_t screen_size;
	uint64_t depth_size;
	uint64_t mutex_size;
	pixels_count = (uintptr_t) width * height;
	screen_size = sizeof(uint32_t) * pixels_count;
	depth_size = sizeof(float) * pixels_count;
	mutex_size = sizeof(uint32_t) * pixels_count;
	if (screen_size && (r = (iphyee_render_screen_s *) refer_alloz(sizeof(iphyee_render_screen_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_render_screen_free_func);
		if ((r->screen_host = iphyee_worker_create_buffer_host(
				worker, screen_size, iphyee_worker_buffer_usage__dst)) &&
			(r->buffer_device = iphyee_worker_create_buffer_device(
				worker, screen_size + depth_size + mutex_size,
				iphyee_worker_buffer_usage__fix)) &&
			(r->screen_data = iphyee_worker_buffer_map(r->screen_host, 0, screen_size)) &&
			(r->screen_address = iphyee_worker_buffer_device_address(r->buffer_device)))
		{
			r->screen_offset = 0;
			r->screen_size = screen_size;
			r->depth_address = r->screen_address + screen_size;
			r->depth_offset = screen_size;
			r->depth_size = depth_size;
			r->mutex_address = r->depth_address + depth_size;
			r->mutex_offset = screen_size + depth_size;
			r->mutex_size = mutex_size;
			r->width = width;
			r->height = height;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}
