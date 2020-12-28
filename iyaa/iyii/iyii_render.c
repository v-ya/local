#include "iyii_render.h"
#include <stdlib.h>

static void iyii_render_free_func(iyii_render_s *restrict r)
{
	uint32_t i;
	for (i = 0; i < r->image_number; ++i)
		if (r->frame_buffer[i])
			refer_free(r->frame_buffer[i]);
	if (r->swapchain) refer_free(r->swapchain);
	if (r->render_pass) refer_free(r->render_pass);
}

iyii_render_s* iyii_render_alloc(graph_render_pass_s *restrict render_pass, iyii_swapchain_s *restrict swapchain)
{
	iyii_render_s *restrict r;
	uint32_t i, n;
	r = (iyii_render_s *) refer_alloz(sizeof(iyii_render_s) + sizeof(graph_frame_buffer_s *) * (n = swapchain->image_number));
	if (r)
	{
		refer_set_free(r, (refer_free_f) iyii_render_free_func);
		r->render_pass = (graph_render_pass_s *) refer_save(render_pass);
		r->swapchain = (iyii_swapchain_s *) refer_save(swapchain);
		r->image_number = n;
		r->format = swapchain->format;
		r->width = swapchain->width;
		r->height = swapchain->height;
		for (i = 0; i < n; ++i)
		{
			if (!(r->frame_buffer[i] = graph_frame_buffer_alloc(
				render_pass,
				swapchain->image_view[i],
				r->width,
				r->height,
				1
			))) goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
