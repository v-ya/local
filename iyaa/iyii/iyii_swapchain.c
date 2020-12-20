#include "iyii_swapchain.h"
#include <stdlib.h>

static void iyii_swapchain_free_func(iyii_swapchain_s *restrict r)
{
	graph_image_view_s *restrict *restrict iv;
	uint32_t i, n;
	if ((iv = r->image_view))
	{
		for (i = 0, n = r->image_number; i < n; ++i)
			if (iv[i]) refer_free(iv[i]);
		free((void *) iv);
	}
	if (r->image_view_param) refer_free(r->image_view_param);
	if (r->swapchain) refer_free(r->swapchain);
}

iyii_swapchain_s* iyii_swapchain_alloc(graph_dev_s *restrict dev, const graph_device_t *restrict device, graph_surface_s *restrict surface)
{
	iyii_swapchain_s *restrict r;
	r = (iyii_swapchain_s *) refer_alloz(sizeof(iyii_swapchain_s));
	if (r)
	{
		graph_image_view_s **iv;
		uint32_t i, n;
		refer_set_free(r, (refer_free_f) iyii_swapchain_free_func);
		r->swapchain = iyii_graph_create_swapchain(dev, device, surface);
		if (!r->swapchain) goto label_fail;
		graph_swapchain_info(r->swapchain, &r->image_number, &r->format, &r->width, &r->height);
		if (!(n = r->image_number) || !r->format || !r->width || !r->height) goto label_fail;
		r->image_view_param = graph_image_view_param_alloc(graph_image_view_type_2D);
		if (!r->image_view_param) goto label_fail;
		r->image_view = iv = (graph_image_view_s **) calloc(n, sizeof(graph_image_view_s *));
		if (!iv) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			if (!(iv[i] = graph_image_view_alloc_by_swapchain(r->image_view_param, r->swapchain, i)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
