#include "render.h"
#include <memory.h>

static void iphyee_render_texture_free_func(iphyee_render_texture_s *restrict r)
{
	if (r->texture_host) refer_free(r->texture_host);
	if (r->texture_device) refer_free(r->texture_device);
	if (r->texture_name) refer_free(r->texture_name);
	if (r->worker) refer_free(r->worker);
}

iphyee_render_texture_s* iphyee_render_texture_alloc(iphyee_worker_s *restrict worker, const char *restrict name, const uint32_t *restrict pixels, uint32_t width, uint32_t height)
{
	iphyee_render_texture_s *restrict r;
	uintptr_t size;
	void *data;
	if (name && (size = sizeof(uint32_t) * width * height) &&
		(r = (iphyee_render_texture_s *) refer_alloz(sizeof(iphyee_render_texture_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_render_texture_free_func);
		r->worker = (iphyee_worker_s *) refer_save(worker);
		if ((r->texture_name = refer_dump_string(name)) &&
			(r->texture_host = iphyee_worker_create_buffer_host(
				worker, size, iphyee_worker_buffer_usage__src)))
		{
			r->texture_device = NULL;
			r->texture_address = 0;
			r->texture_size = size;
			r->texture_width = width;
			r->texture_height = height;
			r->texture_index = 0;
			r->texture_vaild = 0;
			if ((data = iphyee_worker_buffer_map(r->texture_host, 0, size)))
			{
				if (pixels) memcpy(data, pixels, size);
				else memset(data, 0, size);
				iphyee_worker_buffer_unmap(r->texture_host);
				return r;
			}
		}
		refer_free(r);
	}
	return NULL;
}

iphyee_render_texture_s* iphyee_render_texture_device_enable(iphyee_render_texture_s *restrict r)
{
	if (r->texture_device)
		goto label_okay;
	if ((r->texture_device = iphyee_worker_create_buffer_device(
		r->worker, r->texture_size, iphyee_worker_buffer_usage__dst)))
	{
		if ((r->texture_address = iphyee_worker_buffer_device_address(r->texture_device)))
		{
			label_okay:
			return r;
		}
		refer_free(r->texture_device);
		r->texture_device = NULL;
	}
	return NULL;
}

void iphyee_render_texture_device_disable(iphyee_render_texture_s *restrict r)
{
	if (r->texture_device)
	{
		refer_free(r->texture_device);
		r->texture_device = NULL;
	}
	r->texture_address = 0;
	r->texture_index = 0;
	r->texture_vaild = 0;
}
